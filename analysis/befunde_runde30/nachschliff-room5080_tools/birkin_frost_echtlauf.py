#!/usr/bin/env python3
"""Birkin-Frost-Schranke im ECHTLAUF (r30_5080_echtlauf.sh, RE15_STATE_LOG): Auswertung je Bild.

Dossier nachschliff-room5080.md Abschnitt 9. Ab dem Eintritt in ROOM5080 (Spieler (-9950,-18200),
Tuer-Datensatz ROOM6010 @0x00FCE; beim Debug-Sprung das erste Bild mit Birkin) wird gezaehlt:
  (a) Bilder VOR der Freigabe, in denen Birkin NICHT eingefroren ist
      (eingefroren = Lage (-18100,200) oder die sub03-Pos_set-Lage (-18100,-17800) @0x00822 — das
      Skript darf ihn im Frost versetzen —, g=33, st=1, ss1=9, mo=0, af=16 — Spawn-Record ROOM5080
      @0x00746, INIT STAGE5 +0x95=0x10 / Sub 9)
  (b) das Bild, in dem grid von 0x33 auf 0x13 wechselt (Member_set(0x0C,0x13) @0x0083E)
  (c) danach: erstes Bild Sub 9 mit Clip 16 (EMERGENCE), erstes Bild Sub 1 (WALK), erstes hp < 100
  N1: Laeufe mit gr=1 (Griff-Kanal des Ports, s_player_grabbed) — Laenge und Pause danach.
Keine Deutung ausser der Ausgabe. Aufruf: birkin_frost_echtlauf.py <laufverzeichnis> [...]"""
import os, re, sys

PL = re.compile(r"^F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=-?\d+,hp=(-?\d+)\)")
GR = re.compile(r" gr=(\d)")
BK = re.compile(r"\[(\d+) t=30 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=(\w+) mo=(\d+) af=(\d+) "
                r"stun=\S+ d=\S+ @\((-?\d+),(-?\d+),r-?\d+\)\]")

for lauf in sys.argv[1:]:
    zeilen = open(os.path.join(lauf, "state.log"), encoding="latin-1").read().splitlines()
    eintritt = None
    vor_n = vor_verl = 0; vor_erst = None
    frei = emerg = walk = hp_erst = None
    hp_min = 100
    gr_an = None; laeufe = []; letzt_ende = None
    g_alt = None
    for z in zeilen:
        m = PL.match(z)
        if not m:
            continue
        f, px, pz, hp = int(m.group(1)), int(m.group(3)), int(m.group(4)), int(m.group(5))
        b = BK.search(z)
        if eintritt is None:
            if (px, pz) == (-9950, -18200) or b:
                eintritt = f
            else:
                continue
        if not b:
            continue
        st, ss1, g, mo, af = int(b.group(2)), int(b.group(3)), int(b.group(5), 16), int(b.group(6)), int(b.group(7))
        bx, bz = int(b.group(8)), int(b.group(9))
        if frei is None and g_alt is not None and (g_alt & 0x20) and not (g & 0x20):
            frei = (f, g_alt, g, bx, bz)
        g_alt = g
        if frei is None:
            vor_n += 1
            if not ((bx, bz) in ((-18100, 200), (-18100, -17800)) and g == 0x33 and st == 1 and ss1 == 9 and mo == 0 and af == 16):
                vor_verl += 1
                if vor_erst is None:
                    vor_erst = (f, bx, bz, hex(g), st, ss1, mo, af)
            continue
        if emerg is None and st == 1 and ss1 == 9 and mo == 16:
            emerg = f
        if walk is None and st == 1 and ss1 == 1:
            walk = f
        if hp < 100 and hp_erst is None:
            hp_erst = (f, hp)
        hp_min = min(hp_min, hp)
        mg = GR.search(z)
        gr = int(mg.group(1)) if mg else None
        if gr == 1 and gr_an is None:
            gr_an = f
        elif gr == 0 and gr_an is not None:
            laeufe.append([gr_an, f - gr_an, None])
            gr_an = None
        if gr == 1 and laeufe and laeufe[-1][2] is None and gr_an == f:
            laeufe[-1][2] = f - (laeufe[-1][0] + laeufe[-1][1])
    dbg = os.path.join(lauf, "debug.log")
    pd = [l.strip() for l in open(dbg, encoding="latin-1") if "Plc_dest" in l] if os.path.exists(dbg) else []
    print(f"== {lauf}")
    print(f"  Eintritt F{eintritt}; Plc_dest (Folge) {pd[0] if pd else 'nicht gelaufen'}")
    print(f"  (a) {vor_n} Bilder vor der Freigabe, nicht eingefroren: {vor_verl}" + (f", erstes {vor_erst}" if vor_erst else ""))
    print(f"  (b) Freigabe: {frei}")
    print(f"  (c) EMERGENCE F{emerg}, WALK F{walk}, erstes hp<100 {hp_erst}, kleinstes hp {hp_min}")
    print(f"  N1: Griff-Laeufe {len(laeufe)}" + (" (+ einer laeuft am Ende)" if gr_an is not None else ""))
    for s, n, p in laeufe[:8]:
        print(f"      ab F{s}: {n} Bilder gegriffen, danach {p} frei")
