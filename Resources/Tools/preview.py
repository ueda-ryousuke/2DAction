#!/usr/bin/env python3
"""OBJ+PNG を簡易ラスタライズして一覧画像にする（確認用）。 usage: preview.py <Resources> <out.png>"""
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw


def load(folder, name):
    v, vt, vn, faces = [], [], [], []
    for line in open(os.path.join(folder, name, name + ".obj")):
        p = line.split()
        if not p: continue
        if p[0] == 'v': v.append([float(x) for x in p[1:4]])
        elif p[0] == 'vt': vt.append([float(x) for x in p[1:3]])
        elif p[0] == 'f': faces.append([[int(i) - 1 for i in q.split('/')] for q in p[1:4]])
    tex = np.asarray(Image.open(os.path.join(folder, name, name + ".png")).convert("RGB"))
    return np.array(v), np.array(vt), faces, tex


def render(folder, name, size=240, yaw=35, pitch=18, flip=False):
    v, vt, faces, tex = load(folder, name)
    ya, pa = math.radians(yaw), math.radians(pitch)
    ry = np.array([[math.cos(ya), 0, math.sin(ya)], [0, 1, 0], [-math.sin(ya), 0, math.cos(ya)]])
    rx = np.array([[1, 0, 0], [0, math.cos(pa), -math.sin(pa)], [0, math.sin(pa), math.cos(pa)]])
    R = rx @ ry
    pv = v @ R.T
    mn, mx = pv.min(0), pv.max(0)
    sc = 0.9 * size / max(mx[0] - mn[0], mx[1] - mn[1])
    cx, cy = (mx[0] + mn[0]) / 2, (mx[1] + mn[1]) / 2
    sx = (pv[:, 0] - cx) * sc + size / 2
    sy = size / 2 - (pv[:, 1] - cy) * sc
    sz = pv[:, 2]
    img = np.full((size, size, 3), 235, np.uint8)
    zb = np.full((size, size), -1e9)
    light = np.array([0.4, 0.8, 0.5]); light /= np.linalg.norm(light)
    H, W = tex.shape[:2]
    for f in faces:
        i = [t[0] for t in f]; u = [t[1] for t in f]
        a, b, c = v[i[0]], v[i[1]], v[i[2]]
        n = np.cross(b - a, c - a)
        ln = np.linalg.norm(n)
        if ln < 1e-12: continue
        n /= ln
        nv = R @ n
        if nv[2] <= 0: continue   # カメラは+Z側から-Zを見る。背面は除外
        lam = 0.45 + 0.55 * max(0.0, float(n @ light))
        p = np.array([[sx[k], sy[k]] for k in i]); z = sz[i]
        x0, x1 = int(max(0, p[:, 0].min())), int(min(size - 1, p[:, 0].max() + 1))
        y0, y1 = int(max(0, p[:, 1].min())), int(min(size - 1, p[:, 1].max() + 1))
        if x1 < x0 or y1 < y0: continue
        xs, ys = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (p[1, 1] - p[2, 1]) * (p[0, 0] - p[2, 0]) + (p[2, 0] - p[1, 0]) * (p[0, 1] - p[2, 1])
        if abs(d) < 1e-9: continue
        w0 = ((p[1, 1] - p[2, 1]) * (xs - p[2, 0]) + (p[2, 0] - p[1, 0]) * (ys - p[2, 1])) / d
        w1 = ((p[2, 1] - p[0, 1]) * (xs - p[2, 0]) + (p[0, 0] - p[2, 0]) * (ys - p[2, 1])) / d
        w2 = 1 - w0 - w1
        m = (w0 >= -0.02) & (w1 >= -0.02) & (w2 >= -0.02)
        if not m.any(): continue
        zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
        uv = [vt[k] for k in u]
        U = w0 * uv[0][0] + w1 * uv[1][0] + w2 * uv[2][0]
        V = w0 * uv[0][1] + w1 * uv[1][1] + w2 * uv[2][1]
        tx = np.clip((U * W).astype(int), 0, W - 1); ty = np.clip(((1 - V) * H).astype(int), 0, H - 1)
        col = (tex[ty, tx] * lam).astype(np.uint8)
        sub_z = zb[y0:y1 + 1, x0:x1 + 1]
        upd = m & (zz > sub_z)
        sub_z[upd] = zz[upd]
        img[y0:y1 + 1, x0:x1 + 1][upd] = col[upd]
    return Image.fromarray(img)


if __name__ == "__main__":
    folder, out = sys.argv[1], sys.argv[2]
    names = sorted(n for n in os.listdir(folder) if os.path.exists(os.path.join(folder, n, n + ".obj")))
    only = sys.argv[3:]
    if only: names = [n for n in names if n in only]
    cols = 5; size = 240
    rows = math.ceil(len(names) / cols)
    sheet = Image.new("RGB", (cols * size, rows * (size + 18)), (255, 255, 255))
    d = ImageDraw.Draw(sheet)
    for k, n in enumerate(names):
        im = render(folder, n, size)
        x, y = (k % cols) * size, (k // cols) * (size + 18)
        sheet.paste(im, (x, y + 18)); d.text((x + 4, y + 2), n, fill=(0, 0, 0))
    sheet.save(out)
    print(len(names), "models ->", out)
