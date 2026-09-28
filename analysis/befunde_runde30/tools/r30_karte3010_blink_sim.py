#!/usr/bin/env python3
"""r30_karte3010_blink_sim.py - der RE2-Blinkzaehler des Kartenhinweises, Tick fuer Tick.

Nachbildung von FUN_8006F1C4 @0x8006F20C-0x8006F284 (RE2 Leon PSX.EXE), Startwerte aus
dem Hinweis-Init FUN_8006F6A8:
    richtung 0x800D5C19 = 1   (addiu v1,zero,1 @0x8006F6B4 / sb @0x8006F6C4)
    zaehler  0x800D5C18 = 10  (addiu v0,zero,10 @0x8006F6DC / sb @0x8006F6F4)
Je Tick (= ein Durchlauf der Status-Schleife, VSync(0): 0x800DFC1A = 0 @0x80068A1C):
    richtung != 0 (@0x8006F218 beq):
        zaehler < 10 (sltiu v0,v0,0xa @0x8006F22C):
            Se_on(0x022B0000,0) (lui a0,0x22b @0x8006F234 / jal 0x8005ba28 @0x8006F238)
            richtung = 0        (sb zero @0x8006F244)
        zaehler -= 2            (addiu v0,v0,-2 @0x8006F254, sb @0x8006F284)
    sonst:
        zaehler >= 0x51 (sltiu v0,v1,0x51 @0x8006F264 / bne @0x8006F268):
            richtung = 1        (addiu v0,zero,1 @0x8006F270 / sb @0x8006F278)
        zaehler += 2            (addiu v0,v1,2 @0x8006F26C bzw. @0x8006F27C, sb @0x8006F284)
Danach zeichnet derselbe Durchlauf den Zielraum:
    richtung == 0 -> CLUT-Y s2+1 = 502 (addiu s2,s2,1 @0x8006F514)   = "aktueller Raum"
    richtung != 0 -> CLUT-Y 498        (addiu s2,zero,498 @0x8006F5DC) = "unbesucht, Umriss"
"""
def run(n):
    z, r = 10, 1
    out = []
    for t in range(1, n + 1):
        ton = 0
        if r:
            if z < 10:
                ton = 1; r = 0
            z = (z - 2) & 0xFF
        else:
            if z >= 0x51: r = 1
            z = (z + 2) & 0xFF
        out.append((t, z, r, ton, 502 if r == 0 else 498))
    return out

if __name__ == "__main__":
    res = run(400)
    toene = [t for t, z, r, ton, c in res if ton]
    wechsel = [(t, c) for i, (t, z, r, ton, c) in enumerate(res) if i == 0 or res[i - 1][4] != c]
    print("Ton-Ticks (1-basiert ab Init):", toene)
    print("Abstand der Toene:", [b - a for a, b in zip(toene, toene[1:])])
    print("Zustandswechsel (Tick, CLUT-Y):", wechsel[:10])
    rot = [b[0] - a[0] for a, b in zip(wechsel, wechsel[1:])]
    print("Dauer der Phasen in Ticks:", rot[:8])
    print("Zaehler min/max:", min(z for _, z, _, _, _ in res), max(z for _, z, _, _, _ in res))
    print("erste 6 Ticks:", res[:6])
