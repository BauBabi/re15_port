"""Runde 37 Spur L, Nachbesserung 2: Messertreffer je Anmarsch an der echten exe (Zaehlregel wie Abnahme 1 Abschnitt 2).
Eingang: RE15_STATE_LOG eines Laufs Karte 1150 (Montage) -> Tuer -> ROOM1130, RE15_EQUIP=1, Skript
"W1,A0.2,W2,W<w>" + 50x "M0.4,MA0.1" (R1 halten, alle 0,5 s = 15 Bilder ein Schlag), RE15_EXIT_AT=1400#1130.
Gezaehlt fuer Platz <slot> (Standard 5 = der fette Zombie Typ 0x11, HP 250, 3 je Messertreffer) im ROOM1130-Besuch:
  Anmarsch 1 = HP-Abfaelle vor seinem 1. Griff (Wechsel nach st=1 ss1=3),
  Anmarsch 2 = HP-Abfaelle zwischen 1. und 2. Griff ("+" = bis Bild <bis> kein 2. Griff),
  alle      = alle HP-Abfaelle bis Bild <bis>, Griffe = Griff-Wechsel bis Bild <bis>.
Ein Besuch beginnt, wo die Bildnummer zurueckspringt; ROOM1130 ist der Besuch NACH dem ersten (1150).
Aufruf: python messer_ausw.py <state.log> [slot] [bis]   -> eine Zeile "anm1 anm2[+] alle griffe | Bilder"."""
import re, sys

RX_F = re.compile(r"^F(\d+) ")
RX_E = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=[0-9a-f]+ mo=(\d+) af=\d+ stun=-?\d+ d=(\d+) "
                  r"@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")


def main():
    pfad = sys.argv[1]
    slot = int(sys.argv[2]) if len(sys.argv) > 2 else 5
    bis = int(sys.argv[3]) if len(sys.argv) > 3 else 1400
    besuche, letzte = [], -1
    for z in open(pfad, errors="replace"):
        m = RX_F.match(z)
        if not m:
            continue
        f = int(m.group(1))
        if f < letzte or not besuche:
            besuche.append([])
        letzte = f
        besuche[-1].append((f, z))
    if len(besuche) < 2:
        print("FEHLER: kein zweiter Raumbesuch (ROOM1130) in %s" % pfad)
        return 1
    hp_vor, st_vor, griffe, treffer = None, None, [], []
    for f, z in besuche[1]:
        if f > bis:
            break
        for e in RX_E.finditer(z):
            if int(e.group(1)) != slot:
                continue
            st, s1, hp = int(e.group(3)), int(e.group(4)), int(e.group(11))
            if hp_vor is not None and hp < hp_vor:
                treffer.append(f)
            if st == 1 and s1 == 3 and st_vor != (1, 3):
                griffe.append(f)
            hp_vor, st_vor = hp, (st, s1)
    g1 = griffe[0] if griffe else None
    g2 = griffe[1] if len(griffe) > 1 else None
    anm1 = sum(1 for t in treffer if g1 is None or t < g1)
    anm2 = sum(1 for t in treffer if g1 is not None and t > g1 and (g2 is None or t < g2))
    print("anm1 %d anm2 %d%s alle %d griffe %d | Treffer %s | Griffe %s" % (
        anm1, anm2, "" if g2 is not None else "+", len(treffer), len(griffe),
        ",".join("F%d" % t for t in treffer), ",".join("F%d" % g for g in griffe)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
