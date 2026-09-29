#!/usr/bin/env python3
"""Runde 34 / Granate: exakte Ganzzahl-Nachbildung von Wurf/Flug/Abprall/Zuender der Hand
Grenade (CORE00.ESP Effekt 4 sub 0x0D) — Reihenfolge und Arithmetik wie im Original:

  Bild 0 = Spawn-Bild (FUN_80019700 im Spieler-FSM @0x800336ec/4c/a4, VOR dem ESP-Tick @0x8001ce2c).
  Je Bild (FUN_80019e20):
    Schleife 1 (@0x80019e64-ec): Routine A   (Bild 0: R30; danach 0 bzw. R31)
    Hauptlauf  (@0x80019ee0-): Weltlage (@0x8001a118-2a4) aus xlat VOR der Integration,
               Routine B (@0x8001a2b4-d4: R29), dann Physik (@0x8001a2f0-388, nur Flags&0x20==0):
               xlat += vel (s32 += s16), vel += acc (u16-Wrap).
  R30 @0x8001843c: vel/acc_x/Zaehler/Zuender (Tabelle im Dossier §1.1).
  R29 @0x80018320: Welt-y > 0 -> Zaehler!=0: vx -= vx/3; Zaehler--; xlat_y -= Welt-y; vy = -(vy/3)
                                  Zaehler==0: Flags 0x63 (Physik+Bild aus), A=31, B=0 (KEINE y-Korrektur)
  R31 @0x8001854c: Zuender 42 zaehlt je Bild ab dem Folgebild; ==7 Explosion, ==2 Nachbrand, ==0 frei.
  Division "/3" = 0x55555556-Idiom = auf 0 gerundet (C-Semantik).

Die Weltlage y haengt NICHT von der Gier ab (Euler = 0, RotMatrix(0,gier,0) laesst y unveraendert),
nur von der Spawn-Hoehe h (Welt-y des Spawnpunkts, PSX: negativ = ueber dem Boden y=0).

Aufruf: python wurf_sim.py <HOCH|MITTE|TIEF> <h> [--zaehler N] [--trace]
        python wurf_sim.py tabelle           (Tabelle ueber h fuer alle drei Hoehen)
"""
import sys

ROW_ACC_Y = 10          # CORE00.ESP @0x1AB8 Zeile 0: ay = 10 (Datei-Bytes, esp_effekt.py)
WURF = {                # Routine 30 (@0x80018494-0x80018538)
    "HOCH":  dict(v=(380, -110, 21), ax=-2, zaehler=7),   # +0x26 = rng%4+7, rng(0x8000)=0
    "MITTE": dict(v=(280, -50, 24),  ax=-1, zaehler=7),   # rng(0x4000)=0x80 -> 0x80%4=0
    "TIEF":  dict(v=(80, 0, 1),      ax=-1, zaehler=5),   # fest @0x80018530-38
}

def div3(x):            # (s16)x * 0x55555556 >> 32, minus Vorzeichen = trunc(x/3)
    return int(x / 3)

def s16(x):
    x &= 0xffff
    return x - 0x10000 if x & 0x8000 else x

def sim(hoehe, h, zaehler=None, trace=False):
    w = WURF[hoehe]
    vx, vy, vz = w["v"]
    ax, ay, az = w["ax"], ROW_ACC_Y, 0
    n = w["zaehler"] if zaehler is None else zaehler
    xl = [0, 0, 0]
    flags = 0x03
    A, B = 30, 0
    fuse = None
    ev = []
    k = 0
    rest = None
    kontakte = []
    while k < 2000:
        # --- Schleife 1: Routine A ---
        if A == 30:
            A, B, fuse = 0, 29, 42
        elif A == 31:
            f = fuse
            if f == 0:
                ev.append((k, "Zuender 0: Slot frei + Kind 0x030B5800"))
                break
            if f == 7:
                ev.append((k, "Zuender 7: EXPLOSION (Schaden 1000/500, Kind 0x03195000, SE 0x04080001, Licht)"))
                flags = 0x61
            if f == 2:
                ev.append((k, "Zuender 2: Kinder 0x03195000 + 0x030B5400"))
            fuse -= 1
        # --- Hauptlauf: Weltlage (vor Integration) ---
        wy = s16(h + xl[1])
        if B == 29 and wy > 0:
            if n == 0:
                ev.append((k, "Kontakt %d: Liegen (SE 0x010A0001), Welt-y %d bleibt (keine Korrektur)" % (len(kontakte) + 1, wy)))
                kontakte.append((k, wy, 0))
                flags, A, B = 0x63, 31, 0
                rest = (k, wy, xl[0], xl[2])
            else:
                vx = s16(vx - div3(vx))
                n -= 1
                xl[1] -= wy
                vy = s16(-div3(vy))
                kontakte.append((k, wy, n))
                ev.append((k, "Kontakt %d: Abprall, Eindringtiefe %d, SE 0x010A0001|(%d<<8), neu v=(%d,%d,%d)"
                           % (len(kontakte), wy, n, vx, vy, vz)))
        if trace:
            print("  B%3d  A=%2d B=%2d Welt-y=%6d xlat=(%6d,%6d,%6d) v=(%4d,%4d,%3d) Z=%d F=%s flags=%02x"
                  % (k, A, B, wy, xl[0], xl[1], xl[2], vx, vy, vz, n, fuse, flags))
        if not (flags & 0x20):
            xl[0] += vx; xl[1] += vy; xl[2] += vz
            vx = s16(vx + ax); vy = s16(vy + ay); vz = s16(vz + az)
        k += 1
    return dict(ev=ev, rest=rest, kontakte=kontakte, ende=k)

def main():
    a = sys.argv[1:]
    if not a or a[0] == "tabelle":
        print("| Hoehe | h (Spawn-Welt-y) | 1. Kontakt (Bild) | Kontakte | Liegen (Bild) | Explosion (Bild) | Zuender 2 | frei | Weg lokal x / z bis Liegen |")
        print("|---|---|---|---|---|---|---|---|---|")
        for hoehe in ("HOCH", "MITTE", "TIEF"):
            for h in range(-400, -2001, -200):
                r = sim(hoehe, h)
                k0 = r["kontakte"][0][0]
                kr, wy, lx, lz = r["rest"]
                ex = [e[0] for e in r["ev"] if "EXPLOSION" in e[1]][0]
                z2 = [e[0] for e in r["ev"] if "Zuender 2" in e[1]][0]
                fr = r["ende"]
                print("| %s | %d | %d | %d | %d | %d | %d | %d | %d / %d |"
                      % (hoehe, h, k0, len(r["kontakte"]), kr, ex, z2, fr, lx, lz))
        return
    hoehe = a[0].upper(); h = int(a[1])
    z = None
    if "--zaehler" in a:
        z = int(a[a.index("--zaehler") + 1])
    r = sim(hoehe, h, z, trace="--trace" in a)
    for k, t in r["ev"]:
        print("Bild %3d: %s" % (k, t))

if __name__ == "__main__":
    main()
