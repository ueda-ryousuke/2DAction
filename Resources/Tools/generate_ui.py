#!/usr/bin/env python3
"""UI / 演出用のスプライト画像(PNG)を生成する。 usage: generate_ui.py <Resources>"""
import math
import os
import sys

from PIL import Image, ImageDraw, ImageFont

OUT = os.path.join(sys.argv[1] if len(sys.argv) > 1 else "Resources", "ui")
os.makedirs(OUT, exist_ok=True)
FONT = "/usr/share/fonts/opentype/noto/NotoSansCJK-Black.ttc"


def font(size):
    return ImageFont.truetype(FONT, size, index=0)  # index 0 = 日本語(JP)


def text_img(text, size, fill=(255, 255, 255), stroke=(30, 40, 90), sw=None, pad=12, shadow=True):
    sw = sw if sw is not None else max(3, size // 10)
    f = font(size)
    probe = ImageDraw.Draw(Image.new("RGBA", (1, 1)))
    l, t, r, b = probe.textbbox((0, 0), text, font=f, stroke_width=sw)
    w, h = r - l + pad * 2, b - t + pad * 2 + (size // 14 if shadow else 0)
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    if shadow:
        d.text((pad - l, pad - t + size // 14), text, font=f, fill=(0, 0, 0, 110), stroke_width=sw, stroke_fill=(0, 0, 0, 110))
    d.text((pad - l, pad - t), text, font=f, fill=fill, stroke_width=sw, stroke_fill=stroke)
    return im


def save(im, name):
    im.save(os.path.join(OUT, name))
    print("ui/" + name, im.size)


# --- 共通 -------------------------------------------------------------
save(Image.new("RGBA", (8, 8), (255, 255, 255, 255)), "white1x1.png")  # 色付き矩形用(Sprite::SetColor で着色)

# --- タイトル ---------------------------------------------------------
save(text_img("壁際ジャンパー", 150, (255, 235, 90), (120, 40, 40), sw=14, pad=24), "title_logo.png")
save(text_img("SPACE キーではじめる", 44, (255, 255, 255), (30, 60, 120), sw=6), "press_start.png")

# --- ステージ選択 -----------------------------------------------------
for i, name in enumerate(["はじまりの草原", "すべる氷のダンジョン", "迫りくる壁"], 1):
    save(text_img("STAGE %d  %s" % (i, name), 56, (255, 255, 255), (30, 60, 120), sw=7), "stage_%d.png" % i)
save(text_img("← → でえらぶ   SPACE できめる", 36, (255, 255, 255), (30, 60, 120), sw=5), "select_hint.png")
save(text_img("STAGE SELECT", 80, (255, 235, 90), (120, 40, 40), sw=9), "select_title.png")

# --- ゲーム中UI -------------------------------------------------------
save(text_img("HP", 40, (255, 255, 255), (30, 30, 50), sw=6, pad=6), "hp_label.png")
save(text_img("SCORE", 36, (255, 240, 120), (60, 40, 20), sw=5, pad=6), "score_label.png")
save(text_img("RELOAD", 30, (255, 120, 100), (60, 20, 20), sw=5, pad=6), "reload_label.png")
save(text_img("STRAIGHT", 26, (255, 230, 120), (60, 40, 20), sw=4, pad=6), "weapon_straight.png")
save(text_img("HOMING", 26, (255, 150, 220), (70, 20, 60), sw=4, pad=6), "weapon_homing.png")

digits = Image.new("RGBA", (64 * 10, 96), (0, 0, 0, 0))   # 0-9 を 64x96 で横並び
for n in range(10):
    g = text_img(str(n), 80, (255, 255, 255), (40, 40, 80), sw=7, pad=4, shadow=False)
    digits.paste(g, (n * 64 + (64 - g.width) // 2, (96 - g.height) // 2), g)
save(digits, "digits.png")

ic = Image.new("RGBA", (24, 48), (0, 0, 0, 0)); d = ImageDraw.Draw(ic)   # 残弾アイコン
d.rounded_rectangle([4, 14, 20, 44], 6, fill=(255, 210, 70), outline=(120, 70, 20), width=2)
d.polygon([(4, 16), (12, 2), (20, 16)], fill=(230, 120, 40), outline=(120, 70, 20))
save(ic, "bullet_icon.png")

# 操作方法パネル
panel = Image.new("RGBA", (560, 330), (0, 0, 0, 0)); d = ImageDraw.Draw(panel)
d.rounded_rectangle([0, 0, 559, 329], 24, fill=(20, 28, 60, 205), outline=(255, 255, 255, 230), width=4)
d.text((28, 16), "操作方法", font=font(40), fill=(255, 235, 90))
rows = [("A / D", "いどう"), ("SPACE", "ジャンプ（空中で二段 / 壁で壁キック）"), ("J", "弾を撃つ"), ("K", "まっすぐ ⇄ ホーミング"),
        ("R", "リロード"), ("ESC / P", "ポーズ"), ("H", "この表示を消す")]
for i, (k, v) in enumerate(rows):
    y = 72 + i * 35
    d.rounded_rectangle([28, y, 150, y + 29], 8, fill=(255, 255, 255, 235))
    d.text((38, y - 2), k, font=font(22), fill=(30, 40, 90))
    d.text((166, y - 1), v, font=font(22), fill=(255, 255, 255))
save(panel, "controls.png")

# --- ポーズ -----------------------------------------------------------
pm = Image.new("RGBA", (6, 6), (10, 12, 30, 255)); save(pm, "pause_dim.png")
save(text_img("ポーズ", 100, (255, 255, 255), (30, 60, 120), sw=10), "pause_title.png")
save(text_img("ゲームをつづける", 52, (255, 255, 255), (30, 60, 120), sw=6), "pause_resume.png")
save(text_img("ゲームを中断してタイトルへ", 52, (255, 255, 255), (30, 60, 120), sw=6), "pause_quit.png")
cur = Image.new("RGBA", (48, 48), (0, 0, 0, 0)); d = ImageDraw.Draw(cur)
d.polygon([(6, 6), (42, 24), (6, 42)], fill=(255, 235, 90), outline=(120, 40, 40)); save(cur, "cursor.png")

# --- 演出バナー -------------------------------------------------------
save(text_img("READY?", 140, (255, 255, 255), (30, 60, 120), sw=14), "ready.png")
save(text_img("GO!", 200, (255, 235, 90), (180, 50, 40), sw=16), "go.png")
save(text_img("STAGE CLEAR!", 130, (255, 235, 90), (30, 100, 60), sw=14), "clear.png")
save(text_img("GAME OVER", 130, (255, 90, 90), (50, 20, 40), sw=14), "gameover.png")
save(text_img("SPACE でタイトルへ", 40, (255, 255, 255), (30, 30, 60), sw=5), "back_title.png")

# --- シーン切り替え用の「壁」 ----------------------------------------
# 縦長のレンガ壁。複数枚を左から順に落として画面を覆う（壁キックのゲームにちなんだ演出）
w, h = 160, 768
wall = Image.new("RGBA", (w, h), (70, 40, 90, 255)); d = ImageDraw.Draw(wall)
bh, bw = 48, 80
for r in range(h // bh):
    off = bw // 2 if r % 2 else 0
    for c in range(-1, w // bw + 2):
        x0 = c * bw + off
        d.rectangle([x0 + 2, r * bh + 2, x0 + bw - 3, r * bh + bh - 3], fill=(150 + (r * 7 + c * 13) % 30, 90, 150))
        d.line([x0 + 4, r * bh + 4, x0 + bw - 6, r * bh + 4], fill=(190, 130, 190), width=2)
save(wall, "wall_strip.png")
