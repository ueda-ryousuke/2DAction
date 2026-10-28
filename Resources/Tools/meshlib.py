"""OBJ/MTL 生成用の小さなメッシュライブラリ（Blender なしで同等のOBJを出力する）。

規約:
  * 右手系・Y up。キャラの正面は +Z（Blender の既定エクスポート Forward=-Z, Up=Y で
    「Blender上で -Y を向いたキャラ」を書き出したときと同じ向き）。
  * 面は反時計回り(CCW)で外向き。各プリミティブは生成時に外向きかを自己検査する。
  * UV は OBJ 規約（v が上向き）。テクスチャは PNG で、MTL の map_Kd で参照する。
"""
import math
import os

TEX = 256  # テクスチャ解像度


def cell(i, j, w=1, h=1):
    """256px テクスチャを 4x4(64px) のセルに見立てたときの矩形(x0,y0,x1,y1)。"""
    return (i * 64, j * 64, (i + w) * 64, (j + h) * 64)


def _sub(a, b):
    return (a[0] - b[0], a[1] - b[1], a[2] - b[2])


def _cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def _dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def _norm(a):
    l = math.sqrt(_dot(a, a))
    return (a[0] / l, a[1] / l, a[2] / l) if l > 1e-12 else (0.0, 0.0, 0.0)


def rotate(p, rot, c):
    """点 p を中心 c まわりで Euler(rx,ry,rz) 回転（Z→X→Y の順）。"""
    x, y, z = p[0] - c[0], p[1] - c[1], p[2] - c[2]
    rx, ry, rz = rot
    if rz:
        x, y = x * math.cos(rz) - y * math.sin(rz), x * math.sin(rz) + y * math.cos(rz)
    if rx:
        y, z = y * math.cos(rx) - z * math.sin(rx), y * math.sin(rx) + z * math.cos(rx)
    if ry:
        x, z = x * math.cos(ry) + z * math.sin(ry), -x * math.sin(ry) + z * math.cos(ry)
    return (x + c[0], y + c[1], z + c[2])


class Mesh:
    def __init__(self, name):
        self.name = name
        self.tris = []  # ((p,uv),(p,uv),(p,uv))

    # ---- 低レベル ----
    def _uv(self, rect, fx, fy, inset=1.0):
        """矩形内の割合(fx:左→右, fy:下→上)を OBJ の UV に変換。"""
        x0, y0, x1, y1 = rect
        x0 += inset; y0 += inset; x1 -= inset; y1 -= inset
        px = x0 + (x1 - x0) * fx
        py = y1 - (y1 - y0) * fy  # 画像座標は下向きが+
        return (px / TEX, 1.0 - py / TEX)

    def _emit(self, tris, interior):
        """tris: [(a,b,c,uva,uvb,uvc)]。凸形状の外向き検査をして追加。"""
        for a, b, c, ua, ub, uc in tris:
            n = _cross(_sub(b, a), _sub(c, a))
            if _dot(n, n) < 1e-14:
                continue  # 縮退
            if interior is not None and _dot(_norm(n), _sub(a, interior)) < -1e-9:
                raise ValueError("inward face in %s" % self.name)
            self.tris.append(((a, ua), (b, ub), (c, uc)))

    def quad(self, p0, p1, p2, p3, rect, interior=None, flip_u=False):
        """p0=左下,p1=右下,p2=右上,p3=左上 (外から見て CCW)。"""
        l, r = (1, 0) if flip_u else (0, 1)
        u0 = self._uv(rect, l, 0); u1 = self._uv(rect, r, 0)
        u2 = self._uv(rect, r, 1); u3 = self._uv(rect, l, 1)
        self._emit([(p0, p1, p2, u0, u1, u2), (p0, p2, p3, u0, u2, u3)], interior)

    # ---- プリミティブ ----
    def box(self, c, s, rects, rot=(0, 0, 0)):
        """c=中心, s=全サイズ(x,y,z)。rects: 矩形 または {'+z':rect,'side':rect,'+y':..,'all':..}"""
        hx, hy, hz = s[0] / 2, s[1] / 2, s[2] / 2
        faces = {
            '+z': [(-hx, -hy, hz), (hx, -hy, hz), (hx, hy, hz), (-hx, hy, hz)],
            '-z': [(hx, -hy, -hz), (-hx, -hy, -hz), (-hx, hy, -hz), (hx, hy, -hz)],
            '+x': [(hx, -hy, hz), (hx, -hy, -hz), (hx, hy, -hz), (hx, hy, hz)],
            '-x': [(-hx, -hy, -hz), (-hx, -hy, hz), (-hx, hy, hz), (-hx, hy, -hz)],
            '+y': [(-hx, hy, hz), (hx, hy, hz), (hx, hy, -hz), (-hx, hy, -hz)],
            '-y': [(-hx, -hy, -hz), (hx, -hy, -hz), (hx, -hy, hz), (-hx, -hy, hz)],
        }
        for key, pts in faces.items():
            if isinstance(rects, dict):
                rect = rects.get(key) or (rects.get('side') if key[1] in 'xz' else None) or rects.get('all')
            else:
                rect = rects
            if rect is None:
                continue
            w = [rotate((c[0] + p[0], c[1] + p[1], c[2] + p[2]), rot, c) for p in pts]
            self.quad(w[0], w[1], w[2], w[3], rect, interior=c)

    def sphere(self, c, radii, rect, seg=16, ring=10, rot=(0, 0, 0)):
        rx, ry, rz = radii
        pts = []
        for i in range(ring + 1):
            th = math.pi * i / ring
            row = []
            for j in range(seg + 1):
                ph = 2 * math.pi * j / seg
                p = (c[0] + rx * math.sin(th) * math.cos(ph), c[1] + ry * math.cos(th),
                     c[2] + rz * math.sin(th) * math.sin(ph))
                row.append(rotate(p, rot, c))
            pts.append(row)
        tris = []
        for i in range(ring):
            for j in range(seg):
                a, b = pts[i][j], pts[i][j + 1]
                d, e = pts[i + 1][j], pts[i + 1][j + 1]
                ua = self._uv(rect, j / seg, 1 - i / ring); ub = self._uv(rect, (j + 1) / seg, 1 - i / ring)
                ud = self._uv(rect, j / seg, 1 - (i + 1) / ring); ue = self._uv(rect, (j + 1) / seg, 1 - (i + 1) / ring)
                # 上から見て外向きになる巻き順（検査で保証）
                tris.append((a, b, d, ua, ub, ud))
                tris.append((b, e, d, ub, ue, ud))
        self._emit(tris, c)

    def cyl(self, c, r, h, rect_side, rect_cap=None, axis='y', seg=16):
        rect_cap = rect_cap or rect_side
        rot = {'y': (0, 0, 0), 'z': (math.pi / 2, 0, 0), 'x': (0, 0, -math.pi / 2)}[axis]
        top = (c[0], c[1] + h / 2, c[2]); bot = (c[0], c[1] - h / 2, c[2])
        tris = []
        for j in range(seg):
            a0 = 2 * math.pi * j / seg; a1 = 2 * math.pi * (j + 1) / seg
            p0 = (c[0] + r * math.cos(a0), c[1] - h / 2, c[2] - r * math.sin(a0))
            p1 = (c[0] + r * math.cos(a1), c[1] - h / 2, c[2] - r * math.sin(a1))
            q0 = (p0[0], c[1] + h / 2, p0[2]); q1 = (p1[0], c[1] + h / 2, p1[2])
            u0 = self._uv(rect_side, j / seg, 0); u1 = self._uv(rect_side, (j + 1) / seg, 0)
            v0 = self._uv(rect_side, j / seg, 1); v1 = self._uv(rect_side, (j + 1) / seg, 1)
            tris.append((p0, p1, q1, u0, u1, v1)); tris.append((p0, q1, q0, u0, v1, v0))
            cu = self._uv(rect_cap, 0.5, 0.5)
            tu0 = self._uv(rect_cap, 0.5 + 0.5 * math.cos(a0), 0.5 + 0.5 * math.sin(a0))
            tu1 = self._uv(rect_cap, 0.5 + 0.5 * math.cos(a1), 0.5 + 0.5 * math.sin(a1))
            tris.append((top, q0, q1, cu, tu0, tu1))  # 上面
            tris.append((bot, p1, p0, cu, tu1, tu0))  # 底面
        tris = [tuple(rotate(p, rot, c) for p in t[:3]) + t[3:] for t in tris]
        self._emit(tris, c)

    def cone(self, c, r, h, rect, axis='y', seg=16, phase=0.0, rot=(0, 0, 0)):
        """底面中心が c.y - h/2、頂点が c.y + h/2。seg=4,phase=pi/4 で四角錐。"""
        base_rot = {'y': (0, 0, 0), 'z': (math.pi / 2, 0, 0), 'x': (0, 0, -math.pi / 2),
                    '-y': (math.pi, 0, 0), '-x': (0, 0, math.pi / 2), '-z': (-math.pi / 2, 0, 0)}[axis]
        apex = (c[0], c[1] + h / 2, c[2]); bot = (c[0], c[1] - h / 2, c[2])
        tris = []
        for j in range(seg):
            a0 = phase + 2 * math.pi * j / seg; a1 = phase + 2 * math.pi * (j + 1) / seg
            p0 = (c[0] + r * math.cos(a0), bot[1], c[2] - r * math.sin(a0))
            p1 = (c[0] + r * math.cos(a1), bot[1], c[2] - r * math.sin(a1))
            uA = self._uv(rect, 0.5, 1); u0 = self._uv(rect, j / seg, 0); u1 = self._uv(rect, (j + 1) / seg, 0)
            tris.append((p0, p1, apex, u0, u1, uA))
            cu = self._uv(rect, 0.5, 0.5)
            tris.append((bot, p1, p0, cu, self._uv(rect, 0.8, 0.2), self._uv(rect, 0.2, 0.2)))
        tris = [tuple(rotate(rotate(p, base_rot, c), rot, c) for p in t[:3]) + t[3:] for t in tris]
        self._emit(tris, c)

    # ---- 出力 ----
    def bounds(self):
        xs = [p[0][i] for t in self.tris for p in t for i in range(3)]
        return xs

    def write(self, folder):
        os.makedirs(folder, exist_ok=True)
        vs, vts, vns, fs = {}, {}, {}, []
        vl, vtl, vnl = [], [], []

        def idx(d, l, key):
            if key not in d:
                l.append(key); d[key] = len(l)
            return d[key]
        for tri in self.tris:
            a, b, c = tri[0][0], tri[1][0], tri[2][0]
            n = _norm(_cross(_sub(b, a), _sub(c, a)))
            n = tuple(round(x, 5) for x in n)
            f = []
            for p, uv in tri:
                f.append((idx(vs, vl, tuple(round(x, 5) for x in p)),
                          idx(vts, vtl, (round(uv[0], 5), round(uv[1], 5))),
                          idx(vns, vnl, n)))
            fs.append(f)
        mat = self.name + "_mat"
        with open(os.path.join(folder, self.name + ".obj"), "w") as o:
            o.write("# %s  generated by Tools/generate_assets.py\n" % self.name)
            o.write("mtllib %s.mtl\no %s\n" % (self.name, self.name))
            for v in vl: o.write("v %.5f %.5f %.5f\n" % v)
            for v in vtl: o.write("vt %.5f %.5f\n" % v)
            for v in vnl: o.write("vn %.5f %.5f %.5f\n" % v)
            o.write("usemtl %s\ns off\n" % mat)
            for f in fs: o.write("f " + " ".join("%d/%d/%d" % t for t in f) + "\n")
        with open(os.path.join(folder, self.name + ".mtl"), "w") as m:
            m.write("# %s\nnewmtl %s\nNs 10.0\nKa 1.0 1.0 1.0\nKd 1.0 1.0 1.0\nKs 0.0 0.0 0.0\n"
                    "d 1.0\nillum 1\nmap_Kd %s.png\n" % (self.name, mat, self.name))
