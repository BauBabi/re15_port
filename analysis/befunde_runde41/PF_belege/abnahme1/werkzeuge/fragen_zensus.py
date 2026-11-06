"""Mass 4: Zensus aller Original-Nachrichten mit Ja/Nein (Steuercode 0x03) in shared_assets/PSX/STAGE*/ROOM*.RDT (eigene Auswertung).
Frageseite = Zeilen nach dem letzten Seitenwechsel (02 xx). Zeilenbreite = Summe DEBUG.BIN @0x4416 (FUN_80028868 @0x800289c0), Zeile 2 endet bei 0x22 + Breite (@0x80027eec).
Auswahl: Zeile 2 (DAT_800b8536 + 0x10, @0x80028630/@0x80028674), Cursor ab x 0xa0 (@0x80028650), 'Yes' ab 0xae (@0x80028680)."""
import os, struct, glob, collections
CD = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/shared_assets/PSX'
W = open(os.path.join(CD, 'BIN', 'DEBUG.BIN'), 'rb').read()[0x4416:0x4416 + 256]
ARG = (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B)
def seiten(b):
    s = [[0]]; i = 0
    while i < len(b):
        c = b[i]
        if c in (0x01, 0x03): return s, c == 0x03
        if c in ARG:
            if c == 0x02: s.append([0])
            i += 2; continue
        if c == 0x07: i += 1; continue
        if c == 0x08: s[-1].append(0); i += 1; continue
        s[-1][-1] += W[c]; i += 1
    return s, False
if __name__ == "__main__":
    erg = []
    for p in sorted(glob.glob(os.path.join(CD, 'STAGE*', 'ROOM*.RDT'))):
        d = open(p, "rb").read()
        if len(d) < 0x60: continue
        ms = struct.unpack_from('<I', d, 0x3C)[0]
        if not ms or ms >= len(d): continue
        n = struct.unpack_from('<H', d, ms)[0] // 2
        if n <= 0 or n > 200: continue
        offs = [struct.unpack_from('<H', d, ms + 2 * i)[0] for i in range(n)]
        for i, o in enumerate(offs):
            b = d[ms + o: ms + o + 600]
            s, frage = seiten(b)
            if frage: erg.append((os.path.basename(p), i, ms + o, s[-1], len(s)))
    zeilen = collections.Counter(len(e[3]) for e in erg)
    print('Fragen (Nachrichten mit 0x03): %d in %d RDTs' % (len(erg), len(set(e[0] for e in erg))))
    print('Zeilen auf der Frageseite:', dict(sorted(zeilen.items())))
    z2 = [0x22 + e[3][1] for e in erg if len(e[3]) == 2]
    print('zweizeilig: Zeile 2 endet bei x %d..%d (Cursor ab 160)' % (min(z2), max(z2)) if z2 else 'keine zweizeilige')
    print('mehrseitig (Frage auf Seite >= 2): %d, davon Frageseite einzeilig %d' % (sum(1 for e in erg if e[4] > 1), sum(1 for e in erg if e[4] > 1 and len(e[3]) == 1)))
    schlecht = [e for e in erg if len(e[3]) > 2 or (len(e[3]) == 2 and 0x22 + e[3][1] >= 160)]
    print('Fragen mit Text auf der Auswahl:', len(schlecht), schlecht[:5])
    print('Beispiel ROOM2060 msg 0:', [e for e in erg if e[0] == 'ROOM2060.RDT' and e[1] == 0])
