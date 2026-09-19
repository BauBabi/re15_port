"""Sichtpruefung einer nachgezeichneten Maske im laufenden Spiel (Phase 2, 2026-09-19).

Faehrt die exe des Worktree-Builds per Debug-Sprung in den Raum, pinnt den Kamerawinkel
(RE15_FORCE_CUT), laesst Leon nach Skript laufen und dumpt eine Bildserie (RE15_FRAMEDUMP,
komplett komponierter Frame VOR dem Present). Aus befund.log (eine Zeile je 15 Bilder: Weltlage
+ Kamera-z des Fusses) und der Standlinie der Maske (MASKS/ROOM####_PRI##.STAND) werden je ein
Bild ausgesucht, in dem Leon VOR und eines, in dem er HINTER dem Objekt steht — und als PNG
abgelegt: sicht_<ROOM>_C<n>_{vor,hinter}.png.

Aufruf: python sicht.py ROOM10C0 3 "W2,U3,W1,L0.5,U3,W1" [frames]
"""
import os, re, subprocess, sys, glob, shutil
import numpy as np
from PIL import Image

WURZEL = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
EXE_DIR = os.path.join(WURZEL, "re15_port", "build_p2_pri", "platform", "pc")
EXE = os.path.join(EXE_DIR, "re15_pc.exe")
MASKS = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "MASKS")
OUT = os.path.join(WURZEL, "analysis", "befunde_2026-09-19", "pri-masken-phase2")
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
import geom
from geom import load_rdt, cut_view
from abnahme import proj


def lauf(room, cut, script, frames, tag):
    dump_dir = os.path.join(WURZEL, "build", "p2", "sicht", "%s_C%d_%s" % (room, cut, tag))
    if os.path.isdir(dump_dir):
        shutil.rmtree(dump_dir)
    os.makedirs(dump_dir)
    bl = os.path.join(EXE_DIR, "befund.log")
    if os.path.exists(bl):
        os.remove(bl)
    env = dict(os.environ)
    # ⛔ KEIN RE15_FORCE_CUT: der erzwungene Winkel zeigt den Spieler nicht zwangslaeufig
    # (gemessen ROOM1050 C6: Leon stand die ganze Zeit HINTER dessen Kamera). Das Spiel
    # waehlt den Winkel ueber seine Kamerazonen selbst — gesucht werden die Bilder, in
    # denen der AKTIVE Winkel der gesuchte ist und der Koerper die Maske beruehrt.
    env.update({
        "RE15_CONTINUE_TEST": "1", "RE15_CARD_AUTO": "1", "RE15_NOAUDIO": "1",
        "RE15_DEBUG_JUMP": "%s@gp" % room[4:],
        # ⛔ Der Sprung "@gp" feuert erst, wenn das Spiel spielbar ist (player_mode != 2 und
        # Frame > 60). Ein Skript, das bei Frame 60 anfaengt, laeuft davor ab — gemessen
        # ROOM1050 C6: 245 Positionen im Cut, aber keine am Objekt. Deshalb spaet starten.
        "RE15_INPUT_SCRIPT": script, "RE15_INPUT_SCRIPT_START": "300",
        "RE15_FRAMEDUMP": "300-%d/4:%s/f" % (frames, dump_dir.replace("\\", "/")),
        "RE15_BOOT_EXIT_AT": str(frames + 10),
    })
    p = subprocess.run([EXE], cwd=EXE_DIR, env=env, capture_output=True, timeout=600)
    pos = {}
    if os.path.exists(bl):
        for ln in open(bl, errors="replace"):
            m = re.match(r"F(\d+)\s+R([0-9A-F]{4}) C(\d+)\s+hp=\S+\s+pos=\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)\s+rot=\S+\s+vz=\s*(-?\d+)/", ln)
            if m and m.group(2) == room[4:] and int(m.group(3)) == cut:
                pos[int(m.group(1))] = (int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)))
    return dump_dir, pos, p.returncode


def main():
    room = sys.argv[1].upper(); cut = int(sys.argv[2]); script = sys.argv[3]
    frames = int(sys.argv[4]) if len(sys.argv) > 4 else 900
    stand_p = os.path.join(MASKS, "%s_PRI%02d.STAND" % (room, cut))
    stand = np.array([float(v) for v in open(stand_p).read().split("\n")[1].split()])
    rdt, _ = load_rdt(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX"), room)
    import struct
    cam = struct.unpack_from("<I", rdt, 0x24)[0]; R, t, H = cut_view(rdt, cam, cut)
    dump_dir, pos, rc = lauf(room, cut, script, frames, "lauf")
    print("exe rc=%d, %d Positionen im Cut, %d Bilder" % (rc, len(pos), len(glob.glob(dump_dir + "/f*.ppm"))))
    # Deckung des Cuts, damit nur Standplaetze zaehlen, deren Koerperkasten die Maske
    # wirklich beruehrt (sonst ist "vor" oder "hinter" ohne Aussage).
    import maskenbild as MB
    ms = MB.masken(open(os.path.join(MASKS, "%s.MSK" % room), "rb").read(), cut) or []
    idx = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))[0]
    deck, _ = __import__("abnahme").deckung_und_tiefe(ms, idx)
    best = {"vor": None, "hinter": None}
    for f, (x, y, z, fvz) in sorted(pos.items()):
        p = proj(R, t, H, x, y, z)
        pk = proj(R, t, H, x, y - 1500, z)
        if p is None or pk is None:
            continue
        sx, sy, vz = p
        hw = 450 * H / vz
        x0, x1 = int(max(0, sx - hw)), int(min(320, sx + hw))
        y0, y1 = int(max(0, min(pk[1], sy))), int(min(240, max(pk[1], sy)))
        if x1 <= x0 or y1 <= y0:
            continue
        box = deck[y0:y1, x0:x1]
        if not box.any():
            continue                      # Maske beruehrt den Koerper nicht
        sv = stand[x0:x1]
        cols = box.any(0) & np.isfinite(sv) & (sv > 0)
        if not cols.any():
            continue
        n_vor = int((vz < sv[cols] - 1).sum()); n_hin = int((vz > sv[cols] + 1).sum())
        pix = int(box.sum())
        if n_vor and not n_hin:           # eindeutig DAVOR
            if best["vor"] is None or pix > best["vor"][0]:
                best["vor"] = (pix, f, (x, y, z), vz)
        elif n_hin and not n_vor:         # eindeutig DAHINTER
            if best["hinter"] is None or pix > best["hinter"][0]:
                best["hinter"] = (pix, f, (x, y, z), vz)
    os.makedirs(OUT, exist_ok=True)
    for tag in ("vor", "hinter"):
        b = best[tag]
        if b is None:
            print("%s: kein Bild (kein Standplatz %s der Standlinie im Lauf)" % (tag, tag.upper()))
            continue
        pix, f, w, vz = b
        kand = sorted(glob.glob(dump_dir + "/f*.ppm"))
        nums = [int(os.path.basename(k)[1:7]) for k in kand]
        if not nums:
            print("keine Bilder"); return 1
        j = min(range(len(nums)), key=lambda i: abs(nums[i] - f))
        if abs(nums[j] - f) > 5:
            print("%s: kein Bild nahe F%d (naechstes F%d)" % (tag, f, nums[j])); continue
        ziel = os.path.join(OUT, "sicht_%s_C%d_%s.png" % (room, cut, tag))
        Image.open(kand[j]).save(ziel)
        print("%s: Bild F%d (Welt %s, Fuss-vz %d, %d Maskenpunkte am Koerper) -> %s" % (tag, nums[j], w, vz, pix, ziel))
    return 0


if __name__ == "__main__":
    sys.exit(main())
