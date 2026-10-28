#!/usr/bin/env python3
"""サンプルステージの CSV を出力する（100列 x 20行 / 0=空 1=ブロック / 1行目が最上段）。
ギミック・敵の配置は Source/Scene/StageBuilder.cpp 側で同じ(列,行)を使っているので、
ここを変えたら StageBuilder の数字も合わせること。  usage: generate_stages.py <Resources>
"""
import os
import sys

W, H = 100, 20
OUT = sys.argv[1] if len(sys.argv) > 1 else "Resources"


def blank():
    return [[0] * W for _ in range(H)]


def fill(g, r0, r1, c0, c1, v=1):
    for r in range(r0, r1 + 1):
        for c in range(c0, c1 + 1):
            g[r][c] = v


def save(g, name):
    with open(os.path.join(OUT, name), "w") as f:
        for row in g:
            f.write(",".join(str(v) for v in row) + "\n")
    print(name, W, "x", H)


def shaft(g, left, right, top, door_rows=3):
    """壁キックの縦穴: left/right の列を壁にして、左壁の下 door_rows 段は入口として空ける。"""
    fill(g, top, 17, right, right)
    fill(g, top, 17 - door_rows, left, left)


# ---- ステージ1: はじまりの草原 ------------------------------------------------
g = blank()
fill(g, 18, 19, 0, 99)
fill(g, 18, 19, 30, 32, 0)   # 穴 → CrumbleFloor の橋
fill(g, 18, 19, 62, 66, 0)   # 穴 → MovingPlatform
fill(g, 14, 14, 8, 11)       # 足場(ジャンプ練習)
fill(g, 12, 12, 14, 17)
fill(g, 10, 10, 20, 23)
shaft(g, 40, 45, 4)          # 壁キックの縦穴(内側41-44)
fill(g, 4, 4, 46, 58)        # 縦穴を登った先の足場
fill(g, 0, 13, 80, 80)       # スイッチ扉の壁(下の14-17行は SwitchDoor が塞ぐ)
save(g, "stage1.csv")

# ---- ステージ2: すべる氷のダンジョン ---------------------------------------------
g = blank()
fill(g, 18, 19, 0, 99)
fill(g, 18, 19, 8, 27, 0)    # 氷の床A(ギミック側で置く)
fill(g, 18, 19, 46, 60, 0)   # 氷の床B
shaft(g, 85, 90, 6)          # ゴールへ続く壁キックの縦穴(内側86-89)
fill(g, 6, 6, 91, 97)        # 縦穴の上のゴール足場
fill(g, 12, 12, 12, 15)      # 氷の上の足場
fill(g, 9, 9, 20, 23)
save(g, "stage2.csv")

# ---- ステージ3: 迫りくる壁(強制スクロール) ----------------------------------------
g = blank()
fill(g, 18, 19, 0, 99)
for a, b in ((20, 22), (34, 37), (50, 52), (66, 69), (82, 84)):
    fill(g, 18, 19, a, b, 0)  # 穴(50-52 は崩れる床、66-69 は移動床)
fill(g, 16, 17, 28, 29)      # 段差
fill(g, 8, 17, 44, 44)       # 高い壁(壁キックで越える)
fill(g, 14, 14, 56, 60)
fill(g, 12, 12, 62, 64)
fill(g, 16, 17, 74, 75)
fill(g, 17, 17, 76, 77)
save(g, "stage3.csv")
