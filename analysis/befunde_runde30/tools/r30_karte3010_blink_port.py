#!/usr/bin/env python3
"""r30_karte3010_blink_port.py - der RE2-Blinkzaehler im TAKT DES PORTS.

RE2 zaehlt je VBlank (Statusschirm: VSync-Teiler [0x800DFC1A] = 0 @0x80068A1C).
Der Port tickt sein Menue einmal je Host-Bild; Host-Bild = 1000/target_fps ms,
target_fps = 30 (platform/pc/main.c:3140, Logzeile "[fps] target=30 FPS").
Ein Port-Tick entspricht also 2 VBlanks.

Verglichen werden zwei Bauarten:
  A  ein RE2-Zaehlschritt je Port-Tick   (zaehlgleich, aber halb so schnell)
  B  zwei RE2-Zaehlschritte je Port-Tick (zeitgleich)
Zaehler-Logik = r30_karte3010_blink_sim.py (FUN_8006F1C4 @0x8006F20C-0x8006F284).
"""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))

def schritt(z, r):
    ton = 0
    if r:
        if z < 10:
            ton = 1; r = 0
        z = (z - 2) & 0xFF
    else:
        if z >= 0x51: r = 1
        z = (z + 2) & 0xFF
    return z, r, ton

def lauf(schritte_je_tick, ticks, hz):
    z, r = 10, 1
    toene = []; phasen = []; letzter = None; seit = 0
    for t in range(1, ticks + 1):
        ton = 0
        for _ in range(schritte_je_tick):
            z, r, tn = schritt(z, r)
            ton |= tn
        if ton: toene.append(t)
        rot = (r == 0)
        if letzter is None: letzter = rot; seit = t
        elif rot != letzter:
            phasen.append(("ROT" if letzter else "UMRISS", t - seit)); letzter = rot; seit = t
    ab = [b - a for a, b in zip(toene, toene[1:])]
    print("  Schritte je Port-Tick: %d  (Port-Takt %.1f Hz)" % (schritte_je_tick, hz))
    print("    Ton in Port-Tick        : %s" % toene[:6])
    print("    Ton-Abstand in Ticks    : %s  = %s s" % (ab[:5], ["%.3f" % (x / hz) for x in ab[:5]]))
    print("    Phasen (Zustand, Ticks) : %s" % phasen[:6])
    print("    Phasen in Sekunden      : %s" % ["%.3f" % (n / hz) for _, n in phasen[:6]])

print("RE2 selbst (1 Schritt je VBlank, 59.826 Hz):")
lauf(1, 400, 59.826)
print("Port, Bauart A:")
lauf(1, 400, 30.0)
print("Port, Bauart B:")
lauf(2, 400, 30.0)
