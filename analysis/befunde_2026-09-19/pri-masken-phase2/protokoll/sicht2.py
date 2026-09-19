"""Sichtpruefung einer nachgezeichneten Maske im laufenden Spiel (Phase 2, 2026-09-19).

Statt Leon per Tastenskript zu steuern (ein zeitbasiertes Skript trifft den Platz vor einem
Moebel nur zufaellig — gemessen: in ROOM10C0 blieb er im Spawn-Winkel, in ROOM1050 hinter der
Kamera) wird er mit RE15_POCC_TP an einen GEMESSENEN Weltpunkt gesetzt: einmal an einen
begehbaren Standplatz VOR der Objekt-Standlinie, einmal an einen DAHINTER. Die Punkte kommen
aus derselben Rechnung wie die Abnahme (abnahme.standplatz_schiene) auf den begehbaren
Punkten der Engine-Sonde probe_p2_floor_dump.

Ergebnis: analysis/befunde_2026-09-19/pri-masken-phase2/sicht_<ROOM>_C<n>_{vor,hinter}.png
plus die Messzeile aus befund.log (Weltlage, Kamera-z, aktiver Winkel).

Aufruf: python sicht2.py ROOM10C0 3
"""
import os, re, struct, subprocess, sys, glob, shutil
import numpy as np
from PIL import Image

WURZEL = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
EXE_DIR = os.path.join(WURZEL, "re15_port", "build_p2_pri", "platform", "pc")
EXE = os.path.join(EXE_DIR, "re15_pc.exe")
MASKS = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "MASKS")
OUT = os.path.join(WURZEL, "analysis", "befunde_2026-09-19", "pri-masken-phase2")
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
import abnahme, geometrie, maskenbild as MB
from geom import load_rdt, cut_view
from abnahme import proj

TP_FRAME = 400            # nach dem Sprung (@gp feuert ab Frame > 60, Raumladen braucht ~200)
SHOT = TP_FRAME + 260     # POCC_TP gleitet mit ~250 Einheiten/Bild; 260 Bilder reichen weit


def spieler_band(room):
    """⛔ RE15_POCC_TP setzt NUR x, z und rot — die Hoehe (und damit das Band) behaelt der
    Spieler aus dem Raumsprung. Ein Standplatz auf einem anderen Band ist damit unbrauchbar:
    gemessen ROOM10A0 C8, Kandidat auf Band 1, Spieler auf y=-14400 -> vz -1144, also hinter
    der Kamera, und in beiden Bildern war niemand zu sehen. Deshalb zuerst einen Probelauf
    OHNE Teleport und das Band aus befund.log lesen."""
    bl = os.path.join(EXE_DIR, "befund.log")
    if os.path.exists(bl):
        os.remove(bl)
    env = dict(os.environ)
    env.update({"RE15_CONTINUE_TEST": "1", "RE15_CARD_AUTO": "1", "RE15_NOAUDIO": "1",
                "RE15_DEBUG_JUMP": "%s@gp" % room[4:]})
    import time
    p = subprocess.Popen([EXE], cwd=EXE_DIR, env=env,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    ys = []
    t0 = time.time()
    while time.time() - t0 < 120:
        time.sleep(2)
        if os.path.exists(bl):
            for ln in open(bl, errors="replace"):
                m = re.match(r"F(\d+)\s+R([0-9A-F]{4}) C\d+\s+hp=\S+\s+pos=\(\s*-?\d+,\s*(-?\d+)", ln)
                if m and m.group(2) == room[4:] and int(m.group(1)) > TP_FRAME - 100:
                    ys.append(int(m.group(3)))
            if ys:
                break
        if p.poll() is not None:
            break
    p.terminate()
    try:
        p.wait(timeout=20)
    except Exception:
        p.kill()
    if not ys:
        return None
    y = max(set(ys), key=ys.count)
    return int(round(-y / float(0x708)))


def kandidaten(room, cut, n=6, nur_band=None):
    """-> (vor, hinter): je Liste (x, z, band, Maskenpunkte am Koerper), beste zuerst."""
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX"), room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    R, t, H = cut_view(rdt, cam, cut)
    ms = MB.masken(open(os.path.join(MASKS, "%s.MSK" % room), "rb").read(), cut) or []
    idx = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    z = open(os.path.join(MASKS, "%s_PRI%02d.STAND" % (room, cut))).read().split("\n")
    stand = np.array([float(v) for v in z[1].split()])
    eb = np.array([float(v) for v in z[3].split()])
    floor = abnahme.floor_aus_dump(os.path.join(WURZEL, "build", "p2", "floor_p2.txt"), rid)
    vor, hin = [], []
    for band, pts in floor.items():
        if nur_band is not None and band != nur_band:
            continue
        yfoot = -band * 0x708
        for (wx, wz) in pts:
            pf = proj(R, t, H, wx, yfoot, wz); pk = proj(R, t, H, wx, yfoot - 1500, wz)
            if pf is None or pk is None:
                continue
            fsx, fsy, fvz = pf
            # ⛔ Der Standplatz muss WIRKLICH im Bild liegen: Fuss und Kopf zwischen 0 und 240
            # und die Figur nicht breiter als das Bild. Ohne das waehlte die Suche einen Platz
            # direkt vor der Linse (ROOM10C0 C3, vz 2020, Fuss bei Bildzeile 436) — der
            # Koerperkasten wurde auf 0..240 beschnitten und "beruehrte" die Maske nur dort.
            if not (0 <= fsy < 240 and -20 <= pk[1] < 240 and 0 <= fsx < 320):
                continue
            hw = 450 * H / fvz
            if hw > 120:                      # zu dicht an der Kamera
                continue
            x0, x1 = int(max(0, fsx - hw)), int(min(320, fsx + hw))
            y0, y1 = int(max(0, min(pk[1], fsy))), int(min(240, max(pk[1], fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            box = deck[y0:y1, x0:x1]
            if not box.any():
                continue
            sv = stand[x0:x1]
            ref = (wx * R[6] + wz * R[8]) / 4096.0 + t[2] + eb[x0:x1] * R[7] / 4096.0
            c = box.any(0) & (sv > 0)
            if not c.any():
                continue
            nv = int((ref[c] < sv[c] - 1).sum()); nh = int((ref[c] > sv[c] + 1).sum())
            px = int(box.sum())
            if nv and not nh:
                vor.append((px, wx, wz, band))
            elif nh and not nv:
                hin.append((px, wx, wz, band))
    vor.sort(reverse=True); hin.sort(reverse=True)
    return vor[:n], hin[:n]


def lauf(room, cut, wx, wz, tag, cut_env=None):
    dump = os.path.join(WURZEL, "build", "p2", "sicht", "%s_C%d_%s" % (room, cut, tag))
    if os.path.isdir(dump):
        shutil.rmtree(dump)
    os.makedirs(dump)
    bl = os.path.join(EXE_DIR, "befund.log")
    if os.path.exists(bl):
        os.remove(bl)
    env = dict(os.environ)
    # RE15_FORCE_CUT pinnt den Winkel, RE15_POCC_TP setzt Leon an einen Standplatz, der in
    # GENAU DIESEM Winkel im Bild liegt (die Kandidaten kommen aus dessen eigener Projektion).
    # Beides einzeln reicht nicht: mit erzwungenem Winkel und Tastenskript stand er hinter der
    # Kamera (ROOM1050 C6), ohne erzwungenen Winkel zeigt das Spiel an diesen Stellen einen
    # anderen (ROOM10C0: Winkel 0 statt 3) — die Kamerazone gehoert dort einem Nachbarwinkel.
    env.update({
        "RE15_CONTINUE_TEST": "1", "RE15_CARD_AUTO": "1", "RE15_NOAUDIO": "1",
        "RE15_DEBUG_JUMP": "%s@gp" % room[4:],
        "RE15_FORCE_CUT": str(cut),
        "RE15_POCC_TP": "%d,%d,%d,0" % (TP_FRAME, wx, wz),
        "RE15_FRAMEDUMP": "%d:%s/f.ppm" % (SHOT, dump.replace("\\", "/")),
    })
    # ⛔ RE15_BOOT_EXIT_AT zaehlt GAME-BOOTS, keine Bilder (main.c:3863) — die exe lief in den
    # Zeitablauf. Stattdessen auf die Bilddatei warten und dann selbst beenden.
    import time
    p = subprocess.Popen([EXE], cwd=EXE_DIR, env=env,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    ziel_ppm = os.path.join(dump, "f.ppm")
    t0 = time.time()
    while time.time() - t0 < 180:
        if os.path.exists(ziel_ppm) and os.path.getsize(ziel_ppm) > 1000:
            time.sleep(1.5)          # befund.log-Zeile nachziehen lassen
            break
        if p.poll() is not None:
            break
        time.sleep(0.5)
    p.terminate()
    try:
        p.wait(timeout=20)
    except Exception:
        p.kill()
    zeile = None
    if os.path.exists(bl):
        for ln in open(bl, errors="replace"):
            m = re.match(r"F(\d+)\s+R([0-9A-F]{4}) C(\d+)", ln)
            if m and m.group(2) == room[4:] and abs(int(m.group(1)) - SHOT) <= 15:
                zeile = ln.strip()
    p = os.path.join(dump, "f.ppm")
    return (p if os.path.exists(p) else None), zeile


def main():
    room = sys.argv[1].upper(); cut = int(sys.argv[2])
    band = spieler_band(room)
    vor, hin = kandidaten(room, cut, nur_band=band)
    print("%s C%d: Spieler-Band %s, %d VOR-Kandidaten, %d HINTER-Kandidaten"
          % (room, cut, band, len(vor), len(hin)))
    os.makedirs(OUT, exist_ok=True)
    for tag, liste in (("vor", vor), ("hinter", hin)):
        if not liste:
            print("  %s: KEIN Kandidat" % tag); continue
        for (px, wx, wz, band) in liste[:3]:
            ppm, zeile = lauf(room, cut, wx, wz, tag)
            aktiv = int(re.search(r" C(\d+)", zeile).group(1)) if zeile else -1
            if ppm and aktiv == cut:
                ziel = os.path.join(OUT, "sicht_%s_C%d_%s.png" % (room, cut, tag))
                Image.open(ppm).save(ziel)
                print("  %s: Welt(%d,%d) Band %d, %d Maskenpunkte am Koerper -> %s" % (tag, wx, wz, band, px, ziel))
                print("     %s" % (zeile or "(keine Log-Zeile)"))
                break
            print("  %s: Welt(%d,%d) -> Bild %s, aktiver Winkel %d (gesucht %d), naechster Kandidat"
                  % (tag, wx, wz, "ja" if ppm else "NEIN", aktiv, cut))
    return 0


if __name__ == "__main__":
    sys.exit(main())
