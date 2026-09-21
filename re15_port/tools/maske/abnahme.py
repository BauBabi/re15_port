"""Abnahme einer nachgezeichneten Maske je Cut — BEVOR sie geschrieben wird.

Phase 2, Punkt 6 (analysis/befunde_2026-09-19/pri-masken-audit.md §4.6, Gegenpruefung
Punkte 5/9/12): drei Pruefungen auf den Daten, die die Engine spaeter liest:

  (a) Deckung == Soll: die opaken Punkte, die der Zeichner aus Sektion + Atlas blittet
      (derselbe Leser wie die Sonde probe_r16_pri_masken_audit, gegen die Engine mit
      0 Abweichungen in 84 Cuts verifiziert), muessen BITGENAU die Sollflaeche sein.
  (b) Standplatz-Schiene: fuer jeden BEGEHBAREN Standplatz (Bodenpunkte je Band aus dem
      Engine-Dump, re15_collision_on_floor) wird der Koerperkasten (Fuss..Kopf 1500,
      +-450 — Messgroessen wie in test_pri_kopfschnitt.c / befund.log) projiziert und je
      Bildzeile mit dem ORIGINAL-Urteil verglichen (geometrie.verdeckt: depth <
      (1023*vz)>>16). VOR der Objekt-Standlinie (Spalten, in denen der Fuss NAEHER liegt
      als die Standlinie der Maske) darf kein Standplatz zu >= 95 % verdeckt sein
      (VORverd == 0); DAHINTER soll er zu >= 95 % verdeckt sein (HINTfrei -> Restliste).
      ⛔ Die 95 % sind eine HEURISTIK (Gegenpruefung #12), kein belegter Wert; sie stehen
      hier als Parameter und werden im Bericht so genannt.
  (c) Pruefbild: Hintergrund, Sollflaeche/Deckung (magenta = beides, rot = Loch,
      gelb = zu viel), Rechtecke mit Tiefe.

Standlinie der Maske je Spalte = die Kamera-z des UNTERSTEN opaken Punkts der Spalte im
Tiefenmodell (fuer Zellen-Objekte die Vorderflaeche der Zelle, sonst der Bodenkontakt).
"""
import os
import numpy as np
from PIL import Image, ImageDraw

import geometrie

KOPF = 1500
HALB = 450
VOLL = 0.95         # Heuristik (s. Kopf)


def proj(R, t, H, x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    if vz <= 64:
        return None
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


def deckung_und_tiefe(ms, idx):
    """Rechteck-Blit atlas[src] -> screen[dst], Palettenindex != 0 (bg_pc.c pri_publish_tim,
    render_pc.c SDL_RenderCopy). -> (deck bool, tief int32 = naechste Tiefe je Punkt)."""
    deck = np.zeros((240, 320), bool)
    tief = np.zeros((240, 320), np.int32)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w); y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if sub.shape != (y1 - y0, x1 - x0):
            continue
        op = sub != 0
        deck[y0:y1, x0:x1] |= op
        z = tief[y0:y1, x0:x1]
        tief[y0:y1, x0:x1] = np.where(op & ((z == 0) | (dep < z)), dep, z)
    return deck, tief


def standplatz_schiene(R, t, H, deck, tief, stand_vz, floor_by_band, voll=VOLL, stand_y0=None):
    """-> dict mit VORn/VORverd/VORteil/HINTn/HINTfrei/plaetze/beruehrt + Restlisten.

    stand_vz: (320,) float, NaN wo keine Maske; Standlinie der Maske je Spalte (Kamera-z
              des untersten opaken Punkts, gerechnet auf der Bodenebene y0 des Objekts).
    stand_y0: (320,) float, die Bodenebene je Spalte, auf der stand_vz gilt. VOR/HINTER wird
              auf DIESER Ebene verglichen: der Standplatz wird auf y0 der Spalte projiziert
              (gleiche (x,z), Hoehe y0), sonst laege ein Standplatz auf einem hoeheren Band
              (y=-1800) allein wegen seiner Hoehe "vor" einer Standlinie auf y=0 (gemessen
              2026-09-19, ROOM10C0 C2, Band 1 (1600,-4650): falsches VORverd).
    floor_by_band: {band: [(wx, wz), ...]} begehbare Bodenpunkte (Engine-Dump)."""
    aus = dict(plaetze=0, beruehrt=0, VORn=0, VORverd=0, VORteil=0, HINTn=0, HINTfrei=0,
               vor_rest=[], hint_rest=[])
    if stand_y0 is None:
        stand_y0 = np.zeros(320)
    for band, pts in floor_by_band.items():
        yfoot = -band * 0x708
        for (wx, wz) in pts:
            pf = proj(R, t, H, wx, yfoot, wz)
            pk = proj(R, t, H, wx, yfoot - KOPF, wz)
            if pf is None or pk is None:
                continue
            aus["plaetze"] += 1
            fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
            hw = HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            box = deck[y0:y1, x0:x1]
            if not box.any():
                continue
            aus["beruehrt"] += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)
            verd = box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs[:, None])
            sv = stand_vz[x0:x1]
            # Kamera-z des Standplatzes auf der Bodenebene der jeweiligen Spalte
            basis = (wx * R[6] + wz * R[8]) / 4096.0 + t[2]
            ref = basis + stand_y0[x0:x1] * R[7] / 4096.0
            colmask = box.any(0) & np.isfinite(sv)
            # ⛔ TOLERANZ = EIN OT-BUCKET, nicht eine Einheit. Der Zeichner kennt die
            # Entfernung nur in Buckets: Figur-OT = otz>>4 (@0x8002565c sra v1,v1,4) mit
            # otz = (1023*vz)>>12 (ZSF3 = 341, @0x80066c70 addiu t0,zero,341 /
            # @0x80066c74 ctc2 t0,cr29) -> ein Bucket ist 65536/1023 = 64,0625 vz breit.
            # Liegen Standplatz und Standlinie im GLEICHEN Bucket, entscheidet nicht die
            # Tiefe, sondern die Reihenfolge — und die Masken haengen als LETZTE Prims ein
            # (@0x8001ce54 jal 0x80039590, AddPrim haengt vorne ein), die Figur liegt also
            # obenauf. "VOR" und "HINTER" sind dort schlicht nicht definiert.
            # Gemessen ROOM10A0 C2: der einzige beanstandete Standplatz (Band 3,
            # 22250/22150) liegt 9..63 Einheiten von der Standlinie entfernt, also
            # INNERHALB eines Buckets; die alte Toleranz von 1 machte daraus 3 "VOR"-
            # Spalten, von denen zwei ueberhaupt keine Maske haben, und ein einziger
            # Maskenpunkt in Spalte 77 ergab die Quote 1,00.
            vor_cols = colmask & (ref < sv - geometrie.BUCKET)
            hin_cols = colmask & (ref > sv + geometrie.BUCKET)
            if vor_cols.any():
                aus["VORn"] += 1
                q = verd[:, vor_cols].sum() / max(1, box[:, vor_cols].sum())
                if q >= voll:
                    aus["VORverd"] += 1
                    aus["vor_rest"].append((band, int(wx), int(wz), float(q)))
                elif q > 0.05:
                    aus["VORteil"] += 1
            if hin_cols.any():
                aus["HINTn"] += 1
                q = verd[:, hin_cols].sum() / max(1, box[:, hin_cols].sum())
                if q < voll:
                    aus["HINTfrei"] += 1
                    aus["hint_rest"].append((band, int(wx), int(wz), float(q)))
    return aus


def standlinie(dep_objekte):
    """Standlinie je Spalte aus den Tiefenkarten: die Kamera-z des untersten opaken Punkts
    jeder Spalte, ueber alle Objekte die NAECHSTE, dazu die Bodenebene y0 des Objekts, das
    sie liefert. dep_objekte: [(region, vz float, y0)] -> (stand (320,), y0 (320,)); NaN
    bzw. 0 ohne Maske."""
    stand = np.full(320, np.nan)
    ebene = np.zeros(320)
    for (reg, vzm, y0) in dep_objekte:
        for x in np.nonzero(reg.any(0))[0]:
            rows = np.nonzero(reg[:, x])[0]
            v = float(vzm[rows.max(), x])
            if v > 0 and (np.isnan(stand[x]) or v < stand[x]):
                stand[x] = v
                ebene[x] = float(y0)
    return stand, ebene


def pruefbild(bg, soll, deck, ms, titel, pfad):
    ov = bg.astype(float) * 0.55
    both = deck & soll; loch = soll & ~deck; ueber = deck & ~soll
    ov[both] = ov[both] * 0.4 + np.array([255, 0, 200]) * 0.6
    ov[loch] = ov[loch] * 0.3 + np.array([255, 30, 30]) * 0.7
    ov[ueber] = ov[ueber] * 0.3 + np.array([255, 230, 0]) * 0.7
    Z = 3
    im = Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for (sx, sy, X, Y, w, h, dep) in ms:
        d.rectangle([X * Z, Y * Z, (X + w) * Z - 1, (Y + h) * Z - 1], outline=(0, 255, 255))
        d.text((X * Z + 2, Y * Z + 1), str(dep), fill=(0, 255, 255))
    d.text((4, 4), "%s  magenta=Soll&Maske  rot=Loch  gelb=zu viel  cyan=Rechteck+Tiefe" % titel,
           fill=(255, 255, 255))
    os.makedirs(os.path.dirname(pfad), exist_ok=True)
    im.save(pfad)
    return pfad


def pbm_schreiben(pfad, soll):
    """Ziel-Silhouette als P4 (1 Bit je Punkt, 1 = Maske), 320x240 — fuer test_pri_silhouette."""
    bits = np.packbits(np.asarray(soll, bool), axis=1)
    with open(pfad, "wb") as f:
        f.write(b"P4\n320 240\n")
        f.write(bits.tobytes())


HERKUNFT = "# QUELLE klemmpfad"


def floor_aus_dump(pfad, rid):
    """BEGEHBARE Standplaetze je Band EINES Raums aus dem Dump der Sonde
    probe_p2_floor_dump (Zeilen "ROOM <hex> ...", "B <band>", "F <x> <z>").

    ⛔ BERICHTIGT 2026-09-21 (Runde 19, Synthese §3 Schritt 4). Diese Funktion las bis
    dahin den Dump von probe_r16_pri_masken_audit und nannte seine Punkte "begehbar".
    Das war FALSCH: jene Sonde nimmt re15_collision_on_floor = FUN_8003b7f0, den
    Containment-Scan ("welche Zelle ENTHAELT (x,z)"), und die SCA-Zellen sind die
    HINDERNISSE — der Spieler laeuft im bandgleichen KOMPLEMENT (Memory
    reai-v2-kollisionszellen-sind-waende). Nachgezaehlt: 20081 von 20081 Dumppunkten in
    ROOM10D0, 3350/3350 in ROOM1010, 12915/12915 in ROOM1140 und 6636/6636 in ROOM10E0
    liegen INNERHALB einer soliden bandgleichen Zelle. Jede VORn/VORverd/HINTn/HINTfrei-
    Zahl, die vor diesem Datum aus dem Bauwerkzeug kam, beschreibt also unerreichbare
    Orte und ist als Beleg wertlos.

    Richtig ist die Regel des Spielers selbst: begehbar ist ein Punkt, den
    re15_collision_constrain (FUN_8003b0a4, Radius PR=450, Solid-Maske 1, Band gesetzt)
    NICHT verschiebt. Genau das liefert probe_p2_floor_dump.

    Weil beide Dumps gleich AUSSEHEN, wird die Herkunft jetzt verlangt statt geglaubt:
    fehlt die Marke "# QUELLE klemmpfad" in der ersten Zeile, bricht diese Funktion ab.
    -> {band: [(x, z), ...]}"""
    with open(pfad, "r") as f:
        kopf = f.readline()
    if not kopf.startswith(HERKUNFT):
        raise SystemExit(
            "ABBRUCH: %s traegt keine Herkunftsmarke '%s'.\n"
            "  Dieser Dump stammt vermutlich aus probe_r16_pri_masken_audit, und dessen\n"
            "  'Bodenpunkte' liegen IN den Hindernis-Zellen, nicht auf begehbarem Boden\n"
            "  (s. Docstring). Erzeuge den Dump mit der Klemmpfad-Sonde:\n"
            "      <build>/tests/unit/probe_p2_floor_dump 1000 7000 > build/p2/dump_klemmpfad.txt"
            % (pfad, HERKUNFT))
    out = {}
    cur = None; band = None
    with open(pfad, "r") as f:
        for ln in f:
            if ln.startswith("ROOM "):
                p = ln.split(); cur = int(p[1], 16)
                if cur != rid and out:
                    break
                continue
            if cur != rid:
                continue
            if ln.startswith("B "):
                band = int(ln.split()[1]); out.setdefault(band, [])
            elif ln.startswith("F "):
                p = ln.split(); out[band].append((int(p[1]), int(p[2])))
    return out


def pbm_lesen(pfad):
    b = open(pfad, "rb").read()
    assert b.startswith(b"P4\n320 240\n")
    a = np.frombuffer(b[len(b"P4\n320 240\n"):], np.uint8).reshape(240, 40)
    return np.unpackbits(a, axis=1)[:, :320].astype(bool)
