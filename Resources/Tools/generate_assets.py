#!/usr/bin/env python3
"""壁際ジャンパー: 3Dモデル(OBJ+MTL+PNG)を一括生成する。

使い方:  python3 generate_assets.py <出力先(Resources)>
生成したOBJ/MTLは Blender の File > Import > Wavefront (.obj) でそのまま開いて編集できる。
"""
import math
import os
import random
import sys

import numpy as np
from PIL import Image, ImageDraw

from meshlib import Mesh, TEX, cell

OUT = sys.argv[1] if len(sys.argv) > 1 else "Resources"
RNG = np.random.default_rng(20261028)


# ---------------------------------------------------------------- 塗りヘルパ
def shade(c, k):
    return tuple(max(0, min(255, int(v * k))) for v in c)


def new_tex():
    return Image.new("RGB", (TEX, TEX), (128, 128, 128))


def noise(img, rect, amp=8):
    x0, y0, x1, y1 = rect
    a = np.asarray(img).astype(np.int16)
    n = RNG.integers(-amp, amp + 1, size=(y1 - y0, x1 - x0, 1))
    a[y0:y1, x0:x1] = np.clip(a[y0:y1, x0:x1] + n, 0, 255)
    img.paste(Image.fromarray(a.astype(np.uint8)))


def panel(img, rect, color, amp=6, border=True):
    d = ImageDraw.Draw(img)
    x0, y0, x1, y1 = rect
    d.rectangle([x0, y0, x1 - 1, y1 - 1], fill=color)
    noise(img, rect, amp)
    d = ImageDraw.Draw(img)
    if border:
        d.rectangle([x0, y0, x1 - 1, y1 - 1], outline=shade(color, 0.7), width=2)
        d.line([x0 + 3, y0 + 3, x1 - 4, y0 + 3], fill=shade(color, 1.2), width=2)


def vgrad(img, rect, top, bot):
    x0, y0, x1, y1 = rect
    a = np.asarray(img).copy()
    for y in range(y0, y1):
        t = (y - y0) / max(1, y1 - y0 - 1)
        col = [int(top[i] + (bot[i] - top[i]) * t) for i in range(3)]
        a[y, x0:x1] = col
    img.paste(Image.fromarray(a))


def stripes(img, rect, c1, c2, w=12, diag=True):
    x0, y0, x1, y1 = rect
    d = ImageDraw.Draw(img)
    d.rectangle([x0, y0, x1 - 1, y1 - 1], fill=c1)
    for k in range(-(y1 - y0), (x1 - x0) + (y1 - y0), w * 2):
        if diag:
            d.polygon([(x0 + k, y1), (x0 + k + w, y1), (x0 + k + w + (y1 - y0), y0), (x0 + k + (y1 - y0), y0)], fill=c2)
    # はみ出しを消す（セル外を隣セルに塗らない）
    return


def clip_paste(img, rect, painter):
    """セル内だけに描くため、いったん別画像に描いて貼る。"""
    x0, y0, x1, y1 = rect
    tmp = Image.new("RGB", (x1 - x0, y1 - y0))
    painter(tmp)
    img.paste(tmp, (x0, y0))


def bricks(img, rect, color, mortar, rows=4, cols=2):
    x0, y0, x1, y1 = rect
    d = ImageDraw.Draw(img)
    d.rectangle([x0, y0, x1 - 1, y1 - 1], fill=mortar)
    bh = (y1 - y0) / rows
    bw = (x1 - x0) / cols
    for r in range(rows):
        off = (bw / 2) if r % 2 else 0
        for c in range(-1, cols + 1):
            bx0 = x0 + c * bw + off + 1
            by0 = y0 + r * bh + 1
            bx1 = min(x1 - 1, bx0 + bw - 2)
            bx0 = max(x0 + 1, bx0)
            if bx1 > bx0:
                d.rectangle([bx0, by0, bx1, by0 + bh - 2], fill=shade(color, random.uniform(0.9, 1.1)))
    noise(img, rect, 5)


def save_model(mesh, img):
    folder = os.path.join(OUT, mesh.name)
    mesh.write(folder)
    img.save(os.path.join(folder, mesh.name + ".png"))
    return mesh


random.seed(5)
MODELS = {}


def model(fn):
    MODELS[fn.__name__] = fn
    return fn


# ---------------------------------------------------------------- プレイヤー
@model
def player():
    m, t = Mesh("player"), new_tex()
    face, skin, hair, band = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0)
    suit, belly, shoes, scarf, glove = cell(0, 1), cell(1, 1), cell(2, 1), cell(3, 1), cell(0, 2)
    panel(t, skin, (255, 218, 178)); panel(t, hair, (92, 58, 38)); panel(t, band, (225, 55, 60))
    panel(t, suit, (60, 110, 200)); panel(t, belly, (60, 110, 200)); panel(t, shoes, (235, 235, 240))
    panel(t, scarf, (255, 205, 60)); panel(t, glove, (250, 250, 250)); panel(t, face, (255, 218, 178), border=False)
    d = ImageDraw.Draw(t)
    x0, y0 = face[0], face[1]
    for ex in (20, 44):  # 目
        d.ellipse([x0 + ex - 6, y0 + 30, x0 + ex + 6, y0 + 48], fill=(30, 25, 40))
        d.ellipse([x0 + ex - 3, y0 + 33, x0 + ex + 1, y0 + 38], fill=(255, 255, 255))
    d.ellipse([x0 + 6, y0 + 46, x0 + 16, y0 + 52], fill=(255, 150, 150))   # ほお
    d.ellipse([x0 + 48, y0 + 46, x0 + 58, y0 + 52], fill=(255, 150, 150))
    d.arc([x0 + 24, y0 + 44, x0 + 40, y0 + 58], 20, 160, fill=(150, 60, 60), width=3)  # 口
    d.polygon([(belly[0] + 32, belly[1] + 14), (belly[0] + 40, belly[1] + 30), (belly[0] + 32, belly[1] + 46),
               (belly[0] + 24, belly[1] + 30)], fill=(255, 220, 70))  # 胸のマーク
    d.rectangle([belly[0] + 2, belly[1] + 46, belly[0] + 62, belly[1] + 52], fill=(40, 50, 90))  # ベルト
    for k in range(0, 64, 16):  # スカーフ模様
        d.rectangle([scarf[0] + k, scarf[1] + 4, scarf[0] + k + 7, scarf[1] + 60], fill=(255, 150, 40))
    m.box((0, 0.27, 0), (0.62, 0.52, 0.52), {'+z': face, '+y': hair, '-z': hair, 'side': skin, '-y': skin})
    m.box((0, 0.5, -0.03), (0.66, 0.14, 0.58), hair)
    m.box((0, 0.4, 0), (0.67, 0.09, 0.56), band)
    m.box((0, -0.12, 0), (0.46, 0.44, 0.34), {'+z': belly, 'all': suit})
    m.box((0, 0.06, 0), (0.5, 0.1, 0.38), scarf)
    m.box((0, 0.0, -0.3), (0.14, 0.08, 0.34), scarf)
    m.box((0, -0.04, -0.55), (0.12, 0.07, 0.26), scarf, rot=(0.35, 0, 0))
    for s in (-1, 1):
        m.box((s * 0.32, -0.1, 0), (0.14, 0.34, 0.16), suit)
        m.box((s * 0.32, -0.3, 0), (0.15, 0.12, 0.17), glove)
        m.box((s * 0.12, -0.43, 0.02), (0.17, 0.18, 0.22), shoes)
    return save_model(m, t)


# ---------------------------------------------------------------- 敵
@model
def enemy_shooter():
    m, t = Mesh("enemy_shooter"), new_tex()
    metal, eye, dark, cannon, accent = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0), cell(0, 1)
    panel(t, metal, (150, 90, 190)); panel(t, dark, (50, 45, 60)); panel(t, cannon, (110, 110, 125))
    panel(t, accent, (255, 90, 70)); panel(t, eye, (50, 45, 60), border=False)
    d = ImageDraw.Draw(t)
    d.ellipse([eye[0] + 8, eye[1] + 10, eye[0] + 56, eye[1] + 54], fill=(255, 70, 60))
    d.ellipse([eye[0] + 20, eye[1] + 20, eye[0] + 44, eye[1] + 44], fill=(255, 240, 200))
    d.ellipse([eye[0] + 28, eye[1] + 26, eye[0] + 38, eye[1] + 38], fill=(30, 20, 30))
    m.box((0, 0, 0), (0.9, 0.75, 0.8), {'+z': eye, 'all': metal})
    m.box((0, -0.45, 0), (1.0, 0.2, 0.9), dark)
    for s in (-1, 1):
        m.cyl((s * 0.52, -0.45, 0), 0.22, 0.14, dark, axis='x', seg=12)
    m.cyl((0, 0.05, 0.62), 0.16, 0.5, cannon, axis='z', seg=12)
    m.cyl((0, 0.05, 0.9), 0.2, 0.12, dark, axis='z', seg=12)
    m.cyl((0.25, 0.55, -0.1), 0.03, 0.35, dark, seg=8)
    m.sphere((0.25, 0.75, -0.1), (0.07, 0.07, 0.07), accent, seg=8, ring=6)
    return save_model(m, t)


@model
def enemy_flyer():
    m, t = Mesh("enemy_flyer"), new_tex()
    body, belly, wing, eye, beak, dark = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0), cell(0, 1), cell(1, 1)
    panel(t, body, (235, 120, 70)); panel(t, belly, (255, 225, 170)); panel(t, wing, (200, 70, 60))
    panel(t, eye, (255, 255, 255)); panel(t, beak, (255, 200, 40)); panel(t, dark, (30, 25, 35))
    m.sphere((0, 0, 0), (0.4, 0.36, 0.42), body)
    m.sphere((0, -0.1, 0.1), (0.3, 0.27, 0.34), belly)
    for s in (-1, 1):
        m.sphere((s * 0.16, 0.1, 0.38), (0.1, 0.11, 0.06), eye, seg=10, ring=8)
        m.sphere((s * 0.16, 0.1, 0.43), (0.05, 0.06, 0.03), dark, seg=8, ring=6)
        m.box((s * 0.62, 0.1, 0), (0.5, 0.08, 0.4), wing, rot=(0, 0, s * 0.35))
        m.box((s * 0.98, 0.26, 0), (0.4, 0.07, 0.34), wing, rot=(0, 0, s * 0.6))
    m.cone((0, -0.02, 0.5), 0.1, 0.22, beak, axis='z', seg=8)
    m.cone((0, 0, -0.5), 0.12, 0.3, body, axis='-z', seg=8)
    return save_model(m, t)


@model
def enemy_shield():
    m, t = Mesh("enemy_shield"), new_tex()
    armor, helm, shield, visor, plume = cell(0, 0), cell(1, 0), cell(2, 0, 2, 2), cell(0, 1), cell(0, 2)
    panel(t, armor, (80, 110, 170)); panel(t, helm, (120, 140, 190)); panel(t, plume, (225, 55, 60))
    panel(t, visor, (30, 30, 45)); panel(t, shield, (200, 170, 60), amp=4)
    d = ImageDraw.Draw(t)
    d.rectangle([visor[0] + 6, visor[1] + 26, visor[0] + 58, visor[1] + 36], fill=(255, 230, 90))
    sx, sy = shield[0], shield[1]
    d.rectangle([sx + 8, sy + 8, sx + 119, sy + 119], outline=(120, 90, 30), width=5)
    d.polygon([(sx + 64, sy + 22), (sx + 98, sy + 64), (sx + 64, sy + 106), (sx + 30, sy + 64)], fill=(200, 50, 55))
    d.polygon([(sx + 64, sy + 40), (sx + 80, sy + 64), (sx + 64, sy + 88), (sx + 48, sy + 64)], fill=(255, 235, 140))
    m.box((0, -0.1, 0), (0.6, 0.7, 0.4), armor)
    m.box((0, 0.42, 0), (0.44, 0.4, 0.42), {'+z': visor, 'all': helm})
    m.box((0, 0.7, -0.05), (0.08, 0.16, 0.3), plume)
    for s in (-1, 1):
        m.box((s * 0.15, -0.6, 0), (0.2, 0.3, 0.24), armor)
    m.box((0.45, -0.05, 0), (0.14, 0.5, 0.16), armor)
    m.box((0, 0, 0.45), (0.9, 1.3, 0.12), {'+z': shield, 'all': armor})
    return save_model(m, t)


@model
def enemy_split():
    m, t = Mesh("enemy_split"), new_tex()
    slime, eye, pupil, dark = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0)
    panel(t, slime, (110, 220, 110)); panel(t, eye, (255, 255, 255)); panel(t, pupil, (25, 40, 30)); panel(t, dark, (40, 80, 50))
    vgrad(t, (slime[0] + 2, slime[1] + 2, slime[2] - 2, slime[3] - 2), (170, 250, 160), (70, 170, 90))
    d = ImageDraw.Draw(t)
    d.line([slime[0] + 32, slime[1] + 2, slime[0] + 32, slime[1] + 62], fill=(60, 140, 70), width=2)  # 分裂線
    m.sphere((0, 0, 0), (0.55, 0.45, 0.5), slime)
    for s in (-1, 1):
        m.sphere((s * 0.2, 0.1, 0.44), (0.1, 0.12, 0.06), eye, seg=10, ring=8)
        m.sphere((s * 0.2, 0.1, 0.49), (0.05, 0.07, 0.03), pupil, seg=8, ring=6)
    m.box((0, -0.08, 0.5), (0.14, 0.04, 0.04), dark)
    return save_model(m, t)


# ---------------------------------------------------------------- 弾
@model
def bullet_straight():
    m, t = Mesh("bullet_straight"), new_tex()
    core, glow = cell(0, 0), cell(1, 0)
    panel(t, core, (255, 250, 200), border=False); panel(t, glow, (255, 170, 40), border=False)
    m.sphere((0, 0, 0.05), (0.14, 0.14, 0.2), glow, seg=12, ring=8)
    m.cone((0, 0, -0.25), 0.1, 0.3, core, axis='-z', seg=10)
    return save_model(m, t)


@model
def bullet_homing():
    m, t = Mesh("bullet_homing"), new_tex()
    body, nose, fin, flame = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0)
    panel(t, body, (230, 70, 170)); panel(t, nose, (250, 250, 250)); panel(t, fin, (120, 60, 190)); panel(t, flame, (255, 190, 60))
    m.cyl((0, 0, 0), 0.12, 0.5, body, nose, axis='z', seg=12)
    m.cone((0, 0, 0.37), 0.12, 0.24, nose, axis='z', seg=12)
    for sy in (-1, 1):
        m.box((0, sy * 0.18, -0.18), (0.03, 0.2, 0.18), fin)
        m.box((sy * 0.18, 0, -0.18), (0.2, 0.03, 0.18), fin)
    m.cone((0, 0, -0.37), 0.08, 0.25, flame, axis='-z', seg=8)
    return save_model(m, t)


# ---------------------------------------------------------------- ブロック類（2x2x2）
def grass_top(t, r):
    panel(t, r, (96, 190, 80), amp=10, border=False)
    d = ImageDraw.Draw(t)
    for _ in range(18):
        x, y = random.randint(r[0] + 3, r[2] - 6), random.randint(r[1] + 3, r[3] - 8)
        d.line([x, y + 5, x + 1, y], fill=(150, 225, 110), width=2)


@model
def block():
    m, t = Mesh("block"), new_tex()
    top, side, bottom = cell(0, 0), cell(1, 0), cell(2, 0)
    grass_top(t, top)
    panel(t, side, (150, 105, 70), amp=10, border=False)
    panel(t, bottom, (110, 78, 52), amp=8, border=False)
    d = ImageDraw.Draw(t)
    for _ in range(7):
        x, y = random.randint(side[0] + 6, side[2] - 12), random.randint(side[1] + 18, side[3] - 10)
        d.ellipse([x, y, x + random.randint(5, 9), y + random.randint(4, 7)], fill=(120, 82, 55))
    d.rectangle([side[0], side[1], side[2] - 1, side[1] + 11], fill=(96, 190, 80))   # 草のふち
    for x in range(side[0], side[2], 8):
        d.polygon([(x, side[1] + 11), (x + 8, side[1] + 11), (x + 4, side[1] + 17)], fill=(96, 190, 80))
    d.rectangle([side[0], side[1], side[2] - 1, side[1] + 3], fill=(140, 220, 105))
    m.box((0, 0, 0), (2, 2, 2), {'+y': top, '-y': bottom, 'side': side})
    return save_model(m, t)


@model
def block_crumble():
    m, t = Mesh("block_crumble"), new_tex()
    top, side, bottom = cell(0, 0), cell(1, 0), cell(2, 0)
    panel(t, top, (235, 205, 140), amp=10); panel(t, bottom, (170, 140, 90))
    bricks(t, side, (220, 185, 120), (150, 118, 75), rows=4, cols=2)
    d = ImageDraw.Draw(t)
    for r in (top, side):
        x, y = r[0] + 14, r[1] + 6
        pts = [(x, y)]
        for _ in range(6):
            x += random.randint(-6, 9); y += random.randint(6, 9); pts.append((x, y))
        d.line(pts, fill=(95, 70, 45), width=2)
        d.line([(pts[3][0], pts[3][1]), (pts[3][0] + 14, pts[3][1] + 8), (pts[3][0] + 22, pts[3][1] + 20)], fill=(95, 70, 45), width=2)
    m.box((0, 0, 0), (2, 2, 2), {'+y': top, '-y': bottom, 'side': side})
    return save_model(m, t)


@model
def block_ice():
    m, t = Mesh("block_ice"), new_tex()
    top, side, bottom = cell(0, 0), cell(1, 0), cell(2, 0)
    for r in (top, side, bottom):
        vgrad(t, r, (200, 245, 255), (120, 200, 240))
    d = ImageDraw.Draw(t)
    for r in (top, side):
        for k in range(5):
            x = r[0] + random.randint(4, 50); y = r[1] + random.randint(4, 50)
            d.line([x, y, x + 12, y - 8], fill=(255, 255, 255), width=2)
        d.rectangle([r[0], r[1], r[2] - 1, r[3] - 1], outline=(90, 170, 225), width=2)
        d.line([r[0] + 4, r[1] + 4, r[0] + 24, r[1] + 4], fill=(255, 255, 255), width=3)
    noise(t, top, 4); noise(t, side, 4)
    m.box((0, 0, 0), (2, 2, 2), {'+y': top, '-y': bottom, 'side': side})
    return save_model(m, t)


@model
def platform_moving():
    m, t = Mesh("platform_moving"), new_tex()
    top, side, dark = cell(0, 0), cell(1, 0), cell(2, 0)
    panel(t, top, (110, 125, 150)); panel(t, dark, (45, 50, 62))
    clip_paste(t, side, lambda im: stripes(im, (0, 0, 64, 64), (250, 205, 40), (45, 45, 55), w=10))
    d = ImageDraw.Draw(t)
    for dx in (14, 32, 50):  # 矢印
        d.polygon([(top[0] + dx, top[1] + 14), (top[0] + dx + 8, top[1] + 32), (top[0] + dx - 8, top[1] + 32)], fill=(250, 205, 40))
        d.rectangle([top[0] + dx - 3, top[1] + 32, top[0] + dx + 3, top[1] + 50], fill=(250, 205, 40))
    m.box((0, 0, 0), (2.0, 0.6, 2.0), {'+y': top, '-y': dark, 'side': side})
    for s in (-1, 1):
        m.cyl((s * 0.6, -0.38, 0), 0.18, 1.6, dark, axis='z', seg=12)
    return save_model(m, t)


@model
def door():
    m, t = Mesh("door"), new_tex()
    front, metal, lock = (0, 0, 128, 256), cell(2, 0), cell(3, 0)
    panel(t, metal, (120, 125, 140)); panel(t, lock, (250, 205, 60))
    panel(t, front, (130, 90, 60), amp=8)
    d = ImageDraw.Draw(t)
    for x in range(16, 128, 28):
        d.line([x, 6, x, 250], fill=(95, 65, 42), width=2)  # 板目
    for y in (30, 120, 220):
        d.rectangle([4, y, 123, y + 14], fill=(80, 85, 100)); d.line([4, y + 3, 123, y + 3], fill=(150, 155, 170), width=2)
    d.ellipse([lock[0] + 22, lock[1] + 16, lock[0] + 42, lock[1] + 36], fill=(60, 45, 20))
    d.polygon([(lock[0] + 28, lock[1] + 32), (lock[0] + 36, lock[1] + 32), (lock[0] + 40, lock[1] + 54), (lock[0] + 24, lock[1] + 54)], fill=(60, 45, 20))
    m.box((0, 0, 0), (2, 4, 1), {'+z': front, '-z': front, 'side': metal, '+y': metal, '-y': metal})
    m.box((0, 0.2, 0.52), (0.6, 0.7, 0.06), lock)
    m.box((0, 0.2, -0.52), (0.6, 0.7, 0.06), lock)
    return save_model(m, t)


@model
def switch():
    m, t = Mesh("switch"), new_tex()
    base, btn, cap = cell(0, 0), cell(1, 0), cell(2, 0)
    stripe = lambda im: stripes(im, (0, 0, 64, 64), (80, 85, 100), (250, 205, 40), w=8)
    clip_paste(t, base, stripe); panel(t, btn, (235, 60, 60)); panel(t, cap, (190, 40, 45))
    d = ImageDraw.Draw(t)
    d.ellipse([btn[0] + 10, btn[1] + 10, btn[0] + 54, btn[1] + 54], fill=(255, 110, 100))
    m.box((0, -0.05, 0), (1.6, 0.2, 1.6), base)
    m.cyl((0, 0.08, 0), 0.55, 0.16, cap, btn, seg=16)
    return save_model(m, t)


@model
def ceiling_spike():
    m, t = Mesh("ceiling_spike"), new_tex()
    metal, spike, rivet = cell(0, 0), cell(1, 0), cell(2, 0)
    panel(t, metal, (95, 100, 120)); panel(t, spike, (205, 210, 225)); panel(t, rivet, (60, 62, 75))
    d = ImageDraw.Draw(t)
    for (x, y) in ((8, 8), (50, 8), (8, 50), (50, 50)):
        d.ellipse([metal[0] + x, metal[1] + y, metal[0] + x + 6, metal[1] + y + 6], fill=(170, 175, 190))
    d.polygon([(spike[0] + 32, spike[1] + 2), (spike[0] + 44, spike[1] + 62), (spike[0] + 20, spike[1] + 62)], fill=(240, 245, 255))
    m.box((0, 0.5, 0), (2, 1, 2), {'-y': rivet, 'all': metal})
    for sx in (-0.5, 0.5):
        for sz in (-0.5, 0.5):
            m.cone((sx, -0.5, sz), 0.7, 1.0, spike, axis='-y', seg=4, phase=math.pi / 4)
    return save_model(m, t)


@model
def crusher():
    m, t = Mesh("crusher"), new_tex()
    face, side, top, bottom = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0)
    panel(t, top, (70, 72, 88)); panel(t, bottom, (40, 40, 50))
    clip_paste(t, side, lambda im: stripes(im, (0, 0, 64, 64), (235, 70, 60), (45, 45, 55), w=10))
    panel(t, face, (85, 88, 108))
    d = ImageDraw.Draw(t)
    d.polygon([(face[0] + 32, face[1] + 8), (face[0] + 58, face[1] + 52), (face[0] + 6, face[1] + 52)], fill=(250, 205, 40))
    d.rectangle([face[0] + 29, face[1] + 22, face[0] + 35, face[1] + 38], fill=(40, 40, 50))
    d.ellipse([face[0] + 29, face[1] + 42, face[0] + 35, face[1] + 48], fill=(40, 40, 50))
    m.box((0, 0, 0), (2, 2, 2), {'+z': face, '-z': face, '+y': top, '-y': bottom, 'side': side})
    m.box((0, -1.05, 0), (2.1, 0.1, 2.1), bottom)
    return save_model(m, t)


@model
def chase_wall():
    m, t = Mesh("chase_wall"), new_tex()
    wall, spike = cell(0, 0), cell(1, 0)
    vgrad(t, wall, (90, 40, 130), (50, 20, 80)); noise(t, wall, 10)
    panel(t, spike, (210, 215, 235))
    d = ImageDraw.Draw(t)
    for k in range(0, 64, 16):
        d.line([wall[0], wall[1] + k, wall[2], wall[1] + k + 8], fill=(130, 70, 170), width=2)
    d.ellipse([wall[0] + 16, wall[1] + 14, wall[0] + 30, wall[1] + 30], fill=(255, 60, 90))
    d.ellipse([wall[0] + 36, wall[1] + 14, wall[0] + 50, wall[1] + 30], fill=(255, 60, 90))
    d.polygon([(spike[0] + 32, spike[1] + 2), (spike[0] + 50, spike[1] + 62), (spike[0] + 14, spike[1] + 62)], fill=(245, 245, 255))
    m.box((-0.5, 0, 0), (1, 40, 3), wall)
    for iy in range(-19, 20, 2):
        for z in (-1.0, 0.0, 1.0):
            m.cone((0.5, iy, z), 0.7, 1.0, spike, axis='x', seg=4, phase=math.pi / 4)
    return save_model(m, t)


@model
def goal_flag():
    m, t = Mesh("goal_flag"), new_tex()
    pole, flag, base, gold = cell(0, 0), cell(1, 0), cell(2, 0), cell(3, 0)
    panel(t, pole, (235, 235, 240)); panel(t, base, (100, 105, 120)); panel(t, gold, (255, 215, 70))
    d = ImageDraw.Draw(t)
    for i in range(8):
        for j in range(8):
            c = (250, 250, 250) if (i + j) % 2 == 0 else (30, 30, 40)
            d.rectangle([flag[0] + i * 8, flag[1] + j * 8, flag[0] + i * 8 + 7, flag[1] + j * 8 + 7], fill=c)
    m.box((0, 0.1, 0), (0.8, 0.2, 0.8), base)
    m.cyl((0, 1.6, 0), 0.06, 3.0, pole, seg=10)
    m.box((0.45, 2.6, 0), (0.8, 0.5, 0.04), flag)
    m.sphere((0, 3.15, 0), (0.1, 0.1, 0.1), gold, seg=10, ring=8)
    return save_model(m, t)


# ---------------------------------------------------------------- 背景
@model
def bg_sky():
    m, t = Mesh("bg_sky"), new_tex()
    vgrad(t, (0, 0, TEX, TEX), (90, 170, 245), (200, 235, 255))
    a = np.asarray(t).astype(np.int16)
    for y in range(0, TEX, 32):  # うっすら帯（横方向は一様なので左右にタイルしてもつながる）
        a[y:y + 6] += 4
    Image.fromarray(np.clip(a, 0, 255).astype(np.uint8)).save("/dev/null") if False else None
    t = Image.fromarray(np.clip(a, 0, 255).astype(np.uint8))
    m.box((0, 0, 0), (40, 24, 0.02), {'+z': (0, 0, TEX, TEX), '-z': (0, 0, TEX, TEX)})
    return save_model(m, t)


def hills(name, color_top, color_bot, amp, base):
    m, t = Mesh(name), new_tex()
    vgrad(t, (0, 0, TEX, TEX), color_top, color_bot)
    f = lambda x: base + amp * math.sin(2 * math.pi * x / 40) + 0.55 * amp * math.sin(2 * math.pi * 3 * x / 40 + 1.3)
    n = 80
    rect = (0, 0, TEX, TEX)
    for i in range(n):
        x0 = -20 + 40 * i / n; x1 = -20 + 40 * (i + 1) / n
        yb = -14.0
        m.quad((x0, yb, 0), (x1, yb, 0), (x1, f(x1), 0), (x0, f(x0), 0), rect)          # 手前向き(+Z)
        m.quad((x1, yb, 0), (x0, yb, 0), (x0, f(x0), 0), (x1, f(x1), 0), rect)          # 裏向き(-Z)
    return save_model(m, t)


@model
def bg_hills_far():
    return hills("bg_hills_far", (150, 185, 215), (110, 150, 190), 2.2, 2.0)


@model
def bg_hills_near():
    return hills("bg_hills_near", (120, 205, 110), (50, 140, 85), 1.6, 0.0)


@model
def bg_cloud():
    m, t = Mesh("bg_cloud"), new_tex()
    vgrad(t, (0, 0, TEX, TEX), (255, 255, 255), (232, 242, 252))
    for c, r in (((0, 0, 0), (1.2, 0.8, 0.8)), ((-1.2, -0.1, 0), (0.9, 0.6, 0.7)), ((1.3, -0.1, 0), (1.0, 0.65, 0.7)),
                 ((0.4, 0.5, 0.1), (0.8, 0.7, 0.7)), ((-0.5, 0.4, 0), (0.7, 0.6, 0.7))):
        m.sphere(c, r, (0, 0, TEX, TEX), seg=14, ring=9)
    return save_model(m, t)


if __name__ == "__main__":
    only = sys.argv[2:]
    for name, fn in MODELS.items():
        if only and name not in only:
            continue
        mesh = fn()
        print("%-16s tris=%d" % (name, len(mesh.tris)))
