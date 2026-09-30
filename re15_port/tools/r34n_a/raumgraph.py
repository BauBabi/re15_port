#!/usr/bin/env python3
"""raumgraph.py — Spur A (Runde 34 Nacht): Tuergraph einer Stage mit den Bedingungen, unter
denen jede Tuer installiert wird. Frage: trennt das Rolltor in ROOM1050 Raeume, die fuer den
Fortschritt noetig sind, und liegt die Fundstelle der Sicherung (ROOM1150) VOR dem Rolltor?

Quelle: die ausgelieferten RDTs (re15_port/shared_assets/PSX/STAGE<n>/ROOM*.RDT), opcode-exakt
ueber scd_walk_lib (EINE Laengentabelle, dieselbe wie scd_dump_room.py / scd_vm.c).

Door_aot_set (0x3B, 32 B; 40 B bei sat&0x80): Nutzlast ab pc+14 (pc+22 bei Viereck):
  +0/+2/+4 Ziel-x/y/z, +6 Blickrichtung, +8 Stage, +9 Raum, +10 Cut
  (Feldliste wie scd_vm.c op_door_aot_set; Leser im Original FUN_8001d600 @0x8001d930/@0x8001d94c/
   @0x8001d960 — siehe scd_room_setup.c:173-176).
Bedingung = die Ck-Opcodes (0x21 bank bit val) direkt hinter jedem umschliessenden Ifel_ck
(0x06; Blockende = pc+4+len) bzw. deren Verneinung im Else-Zweig (0x07; Ende = pc+len).

Aufruf:  python raumgraph.py [stage=1]          -> Tuertabelle auf stdout
         python raumgraph.py 1 wege               -> Erreichbarkeit Fundstelle/Rolltor je Szenario
"""
import os, sys, struct, glob

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, ".."))
import scd_walk_lib as L  # noqa: E402

ROOT = os.path.normpath(os.path.join(HIER, "..", "..", "shared_assets", "PSX"))


def tueren(pfad):
    d = open(pfad, "rb").read()
    if len(d) < 0x48:
        return []
    raus = []
    for (tag, idx), ops in sorted(L.regionen(d).items()):
        stapel = []          # [(ende, text)]
        letzte_if = None     # (ende, [conds]) des zuletzt geoeffneten If
        i = 0
        while i < len(ops):
            pc, op, sz = ops[i]
            while stapel and pc >= stapel[-1][0]:
                stapel.pop()
            if op == 0x06:                    # Ifel_ck
                ln = L.u16(d, pc + 2)
                conds = []
                j = i + 1
                while j < len(ops) and ops[j][1] in (0x21, 0x23, 0x3E, 0x51, 0x58):
                    q = ops[j][0]
                    if ops[j][1] == 0x21:
                        conds.append("Ck(%d,%d,%d)" % (d[q + 1], d[q + 2], d[q + 3]))
                    else:
                        conds.append("op%02X@0x%05X" % (ops[j][1], q))
                    j += 1
                ende = pc + 4 + ln
                letzte_if = (ende, conds)
                stapel.append((ende, " & ".join(conds) or "?"))
            elif op == 0x07:                  # Else_ck
                ln = L.u16(d, pc + 2)
                # der If-Zweig ist hier zu Ende; Else-Zweig = Verneinung
                if stapel:
                    stapel.pop()
                neg = "NICHT(" + (" & ".join(letzte_if[1]) if letzte_if else "?") + ")"
                stapel.append((pc + ln, neg))
            elif op == 0x3B:                  # Door_aot_set
                viereck = (d[pc + 3] & 0x80) != 0
                nl = pc + (22 if viereck else 14)
                stage, raum, cut = d[nl + 8], d[nl + 9], d[nl + 10]
                x, z = L.s16(d, pc + 6), L.s16(d, pc + 8)
                w, h = L.s16(d, pc + 10), L.s16(d, pc + 12)
                raus.append({
                    "ort": "%s%02d@0x%05X" % (tag, idx, pc), "slot": d[pc + 1], "sce": d[pc + 2],
                    "ziel": "ROOM%X%02X0" % (stage + 1, raum), "cut": cut,
                    "rect": (x, z, w, h), "bed": [s[1] for s in stapel],
                })
            i += 1
    return raus


def kanten_variante1(stage=1):
    """Wie kanten(), aber fuer Szenario 1 (Elza): nur ROOM…1-Dateien, Ziel-Id (Basis) auf die
    …1-Datei abgebildet, wenn es sie gibt (sonst Basis)."""
    import collections
    k = collections.defaultdict(set)
    da = set(os.path.basename(p)[:-4] for p in glob.glob(os.path.join(ROOT, "STAGE%d" % stage, "ROOM*.RDT")))

    def kn(r, z=None, slot=None, x=None):
        basis = r[:-1] + "0"
        n = knoten(basis, z=z, slot=slot, x=x)
        return (r + n[len(basis):]) if n.startswith(basis) else n

    for pfad in sorted(glob.glob(os.path.join(ROOT, "STAGE%d" % stage, "ROOM*1.RDT"))):
        name = os.path.basename(pfad)[:-4]
        d = open(pfad, "rb").read()
        for t in tueren(pfad):
            if t["sce"] == 0:
                continue
            pc = int(t["ort"].split("@0x")[1], 16)
            nl = pc + (22 if (d[pc + 3] & 0x80) else 14)
            ziel = t["ziel"][:-1] + "1"
            if ziel not in da:
                ziel = t["ziel"]
            k[kn(name, slot=t["slot"])].add((kn(ziel, z=L.s16(d, nl + 4), x=L.s16(d, nl)), name, t["slot"],
                                            t["ort"], " | ".join(t["bed"]) or "immer", L.s16(d, nl + 2)))
    return k


def main():
    stage = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    if len(sys.argv) > 2 and sys.argv[2] == "wege":
        # Die Messung des Dossiers: Fundstelle und Rolltor-Seiten je Szenario.
        for titel, k, start, ziele in (
                ("Szenario 0 (Leon)", kanten(stage), "ROOM1170",
                 ["ROOM1150", "ROOM1050N", "ROOM1050S", "ROOM1000-Umkleide", "ROOM10A0"]),
                ("Szenario 1 (Elza)", kanten_variante1(stage), "ROOM1031",
                 ["ROOM1151", "ROOM1051N", "ROOM1051S"])):
            w = bfs(k, start)
            print("### %s, Start %s" % (titel, start))
            for z in ziele:
                print("== %s %s" % (z, "erreichbar" if z in w else "NICHT erreichbar"))
                for zeile in pfad(w, z):
                    print("    " + zeile)
        return
    for datei in sorted(glob.glob(os.path.join(ROOT, "STAGE%d" % stage, "ROOM*.RDT"))):
        name = os.path.basename(datei)[:-4]
        for t in tueren(datei):
            print("%s  slot %2d sce %d  -> %s cut %2d  rect %-26s  %s  [%s]" % (
                name, t["slot"], t["sce"], t["ziel"], t["cut"], str(t["rect"]), t["ort"],
                " | ".join(t["bed"]) or "immer"))




# ---------------------------------------------------------------------------------------------
# Erreichbarkeit: ROOM1050 wird am Rolltor geteilt (Rolltor-Modell obj 0 bei z = -10424,
# sub00 @0x0C36 `2d 00 … 48 3c 00 00 48 d7` = (15432, 0, -10424)). Nordteil N = Tueren 0/1/2,
# Suedteil S = Tueren 3/4/5 (ihre Rechtecke liegen alle eindeutig auf einer Seite). Eine Tuer
# in den Raum landet in N, wenn ihr Ziel-z > -10424 ist, sonst in S.
# Bedingte Tueren gelten als OFFEN (optimistisch) — das Ergebnis "S ohne Rolltor erreichbar"
# ist damit eine OBERGRENZE; ob der gefundene Weg wirklich begehbar ist, pruefen die Ebenen
# (y des Ziels) im Dossier.
ROLLTOR_Z = -10424


def knoten(raum, z=None, slot=None, x=None):
    """Knoten = Raum; ROOM1050 geteilt in N/S (Rolltor), ROOM1000 in seine drei getrennten
    Teilraeume (RVD-Gruppen der Kameratabelle: Umkleide Cut 0-2 x 14500..25000, WC-A Cut 3-5
    z -7200..1800, WC-B Cut 6-8 z 1300..10300 — ohne Uebergangszone zwischen den Gruppen)."""
    if raum == "ROOM1050":
        if slot is not None:
            return "ROOM1050" + ("N" if slot in (0, 1, 2) else "S")
        return "ROOM1050" + ("N" if z is not None and z > ROLLTOR_Z else "S")
    if raum == "ROOM1000":
        if slot is not None:
            return {0: "ROOM1000-Umkleide", 1: "ROOM1000-WC-A", 2: "ROOM1000-WC-B"}.get(slot, raum)
        if x is not None and x >= 14500:
            return "ROOM1000-Umkleide"
        return "ROOM1000-WC-A" if (z is not None and z < 1300) else "ROOM1000-WC-B"
    return raum


def kanten(stage=1):
    import collections
    k = collections.defaultdict(set)
    for pfad in sorted(glob.glob(os.path.join(ROOT, "STAGE%d" % stage, "ROOM*.RDT"))):
        name = os.path.basename(pfad)[:-4]
        if not name.endswith("0"):
            continue                     # nur Szenario 0 (Leon); Varianten …1 = eigener Ablauf
        d = open(pfad, "rb").read()
        for t in tueren(pfad):
            if t["sce"] == 0:
                continue                 # sce 0 = inert (Handler[0] @0x8004305C)
            von = knoten(name, slot=t["slot"])
            # Ziel-z aus der Nutzlast nachlesen
            pc = int(t["ort"].split("@0x")[1], 16)
            viereck = (d[pc + 3] & 0x80) != 0
            nl = pc + (22 if viereck else 14)
            zz = L.s16(d, nl + 4)
            nach = knoten(t["ziel"], z=zz, x=L.s16(d, nl))
            k[von].add((nach, name, t["slot"], t["ort"], " | ".join(t["bed"]) or "immer",
                        L.s16(d, nl + 2)))
    return k


def bfs(k, start, verboten=()):
    import collections
    weg = {start: None}
    q = collections.deque([start])
    while q:
        n = q.popleft()
        for (m, raum, slot, ort, bed, zy) in sorted(k.get(n, ())):
            if (n, m) in verboten or m in weg:
                continue
            weg[m] = (n, raum, slot, ort, bed, zy)
            q.append(m)
    return weg


def pfad(weg, ziel):
    out = []
    while weg.get(ziel):
        n, raum, slot, ort, bed, zy = weg[ziel]
        out.append("%s -[%s slot %d %s y=%d {%s}]-> %s" % (n, raum, slot, ort, zy, bed, ziel))
        ziel = n
    return list(reversed(out))


if __name__ == "__main__":
    main()
