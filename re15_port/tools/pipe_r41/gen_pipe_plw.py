#!/usr/bin/env python3
"""
RE1.5 Rebuilt -- Runde 41 Spur "pipe": Waffenbaenke PL00W02.PLW / PL04W02.PLW fuer die PIPE (Item 2).

NUTZER 2026-11-16 (woertlich): "Eigentlich sollte die Pipe so aehnlich funktionieren wie ein Messer, aber mit
mehr Reichweite. Ich weiss nicht ob schon irgendwas fuer die Pipe vorhanden ist. Modell oder Animation oder so."

BEFUND ORIGINAL (echte Disc Version_40%/, Belege im Bericht scratchpad/pipe/BERICHT_r1.md):
  * Version_40%/PSX/PLD/PL00W02.PLW ist ein PLATZHALTER: EDD/EMR byte-gleich mit PL00W06 / PL04W05 / PL04W0A /
    PL04W0E (11-Clip-SCHUSSWAFFEN-Bank, Clips 6=10 B. Heben, 7/9 = 22/23 B. Rueckstoss), Netz = das Netz von
    PL00W00 (Hand + Pistole), TIM = der gemeinsame Platzhalter. Ganze Datei byte-gleich PL04W05/PL04W0A
    (sha256 c3ee7418...). PL04W02.PLW: eigene 11-Clip-Schusswaffen-Bank, Netz = PL04W00. Kein Rohr-Netz,
    keine Nahkampf-Bank (die Nahkampf-FSM FUN_80034E70 braucht Clip 0xD, @0x80074034/@0x80074038).
  * Der Mod-Baum (info/Re1.5 = re15_port/shared_assets) hat W02 durch eine byte-gleiche Kopie von W01 (Messer)
    ersetzt -> im Port sah das Rohr aus wie das Messer.

PORT-WAHL (Vorlage Messer): die Bank des Rohrs = die Messer-Bank W01 desselben Charakters (EDD dir[0], EMR
dir[1], TIM dir[3] unveraendert, Original-Capcom-Bytes: PL00W01/PL04W01 sind auf Disc und im Port byte-gleich),
nur das Netz dir[2] wird umgebaut: die Hand des Messers (alle Flaechen mit Textur-v < 200 = Handkunst) bleibt
UNVERAENDERT, Klinge + Griff (Textur-v >= 200) werden durch ein sechskantiges Rohr ersetzt:
  * Achse   = Hauptachse der Messer-Punkte (Griffende -> Klingenspitze), Rohr beginnt am Griffende
  * Laenge  = ROHR_FAKTOR x Messerlaenge  (PORT-WAHL "doppelt so lang wie das Messer")
  * Radius  = halbe Griffbreite (z-Ausdehnung der Griffpunkte)
  * Textur  = der Klingen-Metallstreifen des Messers (u 74..110, v 238..251, gleiche clut/page wie das Messer)
  * Wicklung = die des Messernetzes (gemessen: cross(v1-v0, v2-v0) . Normale < 0 fuer ALLE 24 Messerflaechen)

REICHWEITE (Nutzerwunsch "mehr Reichweite"): das Original fuehrt fuer das Rohr 1000 = Grundwert
(@0x8006E5A8, Elza-Zeile @0x8006E600) gegen 1100 fuer das Messer (@0x8006E5A4) -- KEIN Vorsprung des Rohrs.
Die Port-Reichweite wird aus den Netzen geeicht: das Messer gewinnt 100 (1100 - 1000) mit seiner Spitze
gegenueber der leeren Hand (PL00.MD1 Netz 11 = rechte Hand, Bone 11). Faktor k = 100 / (Messerspitze -
Handspitze) [Einheiten Bone-11-lokal], Rohr = 1100 + k x (Rohrspitze - Messerspitze). Ausgabe nach
engine/src/gen/pipe_r41.inc (Riegel tests/unit/test_pipe_r41.c rechnet aus den Asset-Bytes nach).

Aufruf (Repo-Wurzel):  python -I re15_port/tools/pipe_r41/gen_pipe_plw.py
"""
import hashlib
import math
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))
PLD = os.path.join(PORT, "shared_assets", "PSX", "PLD")
INC = os.path.join(PORT, "engine", "src", "gen", "pipe_r41.inc")

ROHR_FAKTOR = 2.0        # PORT-WAHL: Rohr doppelt so lang wie das Messer (Nutzerwunsch "mehr Reichweite")
SEGMENTE = 6             # Sechskant
UV_U0, UV_U1 = 74, 110   # Klingen-Metallstreifen des Messers (PL00W01 Quad 0..2 uv u 74..110)
UV_V0, UV_V1 = 238, 251  # (PL00W01 Quad 1 uv v 238/251)
HANDKUNST_V_MAX = 200    # Handflaechen des Messernetzes: Textur-v 108..157; Messerflaechen: v 224..251
REACH_MESSER = 1100      # @0x8006E5A4 (u16 Low-Haelfte des u32 [1], Leser `lhu s6,0(v0)` @0x800120B4)
REACH_GRUND = 1000       # @0x8006E5A0 [0] = @0x8006E5A8 [2] (Rohr im Original)


def plw_split(d):
    off, n = struct.unpack_from("<II", d, 0)
    return [struct.unpack_from("<I", d, off + 4 * i)[0] for i in range(n)], off


def plw_build(sections):
    """Container wie das Original: u32 Verzeichnis-Offset, u32 Anzahl, Abschnitte ab 8, Verzeichnis am Ende."""
    out = bytearray(8)
    offs = []
    for s in sections:
        offs.append(len(out))
        out += s
    diroff = len(out)
    for o in offs:
        out += struct.pack("<I", o)
    struct.pack_into("<II", out, 0, diroff, len(sections))
    return bytes(out)


def md1_parse(m):
    length, unk, oc = struct.unpack_from("<III", m, 0)
    assert oc == 2, "Waffennetz erwartet genau 1 Netz (object_count 2)"
    h = struct.unpack_from("<14I", m, 12)
    b = 12
    tv = [struct.unpack_from("<4h", m, b + h[0] + 8 * k) for k in range(h[1])]
    tn = [struct.unpack_from("<4h", m, b + h[2] + 8 * k) for k in range(h[3])]
    tf = [struct.unpack_from("<6H", m, b + h[4] + 12 * k) for k in range(h[5])]
    tuv = [m[b + h[6] + 12 * k: b + h[6] + 12 * k + 12] for k in range(h[5])]
    qv = [struct.unpack_from("<4h", m, b + h[7] + 8 * k) for k in range(h[8])]
    qf = [struct.unpack_from("<8H", m, b + h[11] + 16 * k) for k in range(h[12])]
    quv = [m[b + h[13] + 16 * k: b + h[13] + 16 * k + 16] for k in range(h[12])]
    assert tv == qv and h[0] == h[7] and h[2] == h[9], "Messernetz: Tri- und Quad-Teil teilen Punkte/Normalen"
    return dict(length=length, unk=unk, v=tv, n=tn, tf=tf, tuv=tuv, qf=qf, quv=quv)


def md1_build(v, n, tf, tuv, qf, quv, unk):
    """Blocklayout des Originals (PL00W01): Punkte @56, Normalen, Tri-Flaechen, Quad-Flaechen, Tri-UV, Quad-UV;
    Tri- und Quad-Teil zeigen auf DENSELBEN Punkt-/Normalenblock. length = Tri-UV-Offset + 12 (PL00W01: 1348+12 =
    1360, PL04W01: 1324+12 = 1336)."""
    o_v = 56
    o_n = o_v + 8 * len(v)
    o_tf = o_n + 8 * len(n)
    o_qf = o_tf + 12 * len(tf)
    o_tuv = o_qf + 16 * len(qf)
    o_quv = o_tuv + 12 * len(tf)
    end = o_quv + 16 * len(qf)
    body = bytearray(end)
    struct.pack_into("<14I", body, 0, o_v, len(v), o_n, len(n), o_tf, len(tf), o_tuv,
                     o_v, len(v), o_n, len(n), o_qf, len(qf), o_quv)
    for k, p in enumerate(v):
        struct.pack_into("<4h", body, o_v + 8 * k, *p)
    for k, p in enumerate(n):
        struct.pack_into("<4h", body, o_n + 8 * k, *p)
    for k, f in enumerate(tf):
        struct.pack_into("<6H", body, o_tf + 12 * k, *f)
    for k, f in enumerate(qf):
        struct.pack_into("<8H", body, o_qf + 16 * k, *f)
    for k, u in enumerate(tuv):
        body[o_tuv + 12 * k: o_tuv + 12 * k + 12] = u
    for k, u in enumerate(quv):
        body[o_quv + 16 * k: o_quv + 16 * k + 16] = u
    return struct.pack("<III", o_tuv + 12, unk, 2) + bytes(body)


def sub(a, b): return (a[0] - b[0], a[1] - b[1], a[2] - b[2])
def add(a, b): return (a[0] + b[0], a[1] + b[1], a[2] + b[2])
def mul(a, s): return (a[0] * s, a[1] * s, a[2] * s)
def dot(a, b): return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
def cross(a, b): return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])
def norm(a):
    l = math.sqrt(dot(a, a))
    return (a[0] / l, a[1] / l, a[2] / l)


def hauptachse(pts):
    """Groesster Eigenvektor der Kovarianz (Potenziteration, deterministisch)."""
    c = tuple(sum(p[i] for p in pts) / len(pts) for i in range(3))
    cov = [[sum((p[i] - c[i]) * (p[j] - c[j]) for p in pts) for j in range(3)] for i in range(3)]
    e = (1.0, 0.0, 0.0)
    for _ in range(200):
        e = norm(tuple(sum(cov[i][j] * e[j] for j in range(3)) for i in range(3)))
    return c, e


def rohr_netz(knife):
    v, n = knife["v"], knife["n"]
    # Hand vs. Messer nach der Texturkunst (v < 200 = Handkunst).
    hand_q = [k for k, u in enumerate(knife["quv"]) if u[1] < HANDKUNST_V_MAX]
    hand_t = [k for k, u in enumerate(knife["tuv"]) if u[1] < HANDKUNST_V_MAX]
    messer_q = [k for k in range(len(knife["qf"])) if k not in hand_q]
    messer_t = [k for k in range(len(knife["tf"])) if k not in hand_t]
    messer_pkt = sorted({knife["qf"][k][i] for k in messer_q for i in (1, 3, 5, 7)} |
                        {knife["tf"][k][i] for k in messer_t for i in (1, 3, 5)})
    hand_pkt = sorted({knife["qf"][k][i] for k in hand_q for i in (1, 3, 5, 7)} |
                      {knife["tf"][k][i] for k in hand_t for i in (1, 3, 5)})
    assert not set(messer_pkt) & set(hand_pkt), "Hand und Messer teilen Punkte"

    pts = [v[k][:3] for k in messer_pkt]
    c, e = hauptachse(pts)
    # Richtung: zur Klingenspitze (weitester Punkt vom Ursprung = Spitze).
    spitze = max(pts, key=lambda p: dot(p, p))
    if dot(sub(spitze, c), e) < 0:
        e = mul(e, -1.0)
    t = [dot(sub(p, c), e) for p in pts]
    t0, t1 = min(t), max(t)
    laenge_messer = t1 - t0
    laenge_rohr = ROHR_FAKTOR * laenge_messer
    # Senkrechte Basis: b2 = z-Richtung (Griffbreite), b1 = e x b2.
    b2 = norm(sub((0.0, 0.0, 1.0), mul(e, e[2])))
    b1 = norm(cross(b2, e))
    griff = [p for p, tt in zip(pts, t) if tt < t0 + 0.25 * laenge_messer]
    radius = (max(p[2] for p in griff) - min(p[2] for p in griff)) / 2.0
    start = add(c, mul(e, t0))
    ende = add(c, mul(e, t0 + laenge_rohr))

    # Neue Punktliste: Hand-Punkte (alte Reihenfolge), dann Rohr-Ringe.
    remap = {}
    nv, nn = [], []
    for k in hand_pkt:
        remap[k] = len(nv)
        nv.append(v[k])
    # Normalen der Handflaechen unveraendert uebernehmen (eigene Indizes).
    nmap = {}
    def n_idx(k):
        if k not in nmap:
            nmap[k] = len(nn)
            nn.append(n[k])
        return nmap[k]
    tf, tuv, qf, quv = [], [], [], []
    for k in hand_t:
        f = knife["tf"][k]
        tf.append((n_idx(f[0]), remap[f[1]], n_idx(f[2]), remap[f[3]], n_idx(f[4]), remap[f[5]]))
        tuv.append(knife["tuv"][k])
    for k in hand_q:
        f = knife["qf"][k]
        qf.append((n_idx(f[0]), remap[f[1]], n_idx(f[2]), remap[f[3]],
                   n_idx(f[4]), remap[f[5]], n_idx(f[6]), remap[f[7]]))
        quv.append(knife["quv"][k])

    clut_page = knife["quv"][messer_q[0]][2:4], knife["quv"][messer_q[0]][6:8]
    def quad_uv(u0, v0, u1, v1, u2, v2, u3, v3):
        return bytes([u0, v0]) + clut_page[0] + bytes([u1, v1]) + clut_page[1] + bytes([u2, v2, 0, 0, u3, v3, 0, 0])
    def tri_uv(u0, v0, u1, v1, u2, v2):
        return bytes([u0, v0]) + clut_page[0] + bytes([u1, v1]) + clut_page[1] + bytes([u2, v2, 0, 0])

    ring = []
    for ende_i, mitte in enumerate((start, ende)):
        r = []
        for s in range(SEGMENTE):
            a = 2.0 * math.pi * s / SEGMENTE
            rad = add(mul(b1, math.cos(a)), mul(b2, math.sin(a)))
            p = add(mitte, mul(rad, radius))
            r.append(len(nv))
            nv.append((round(p[0]), round(p[1]), round(p[2]), 0))
        ring.append(r)
    seiten_n = []
    for s in range(SEGMENTE):
        a = 2.0 * math.pi * (s + 0.5) / SEGMENTE
        rad = add(mul(b1, math.cos(a)), mul(b2, math.sin(a)))
        seiten_n.append(len(nn))
        nn.append((round(rad[0] * 4096), round(rad[1] * 4096), round(rad[2] * 4096), 0))
    kappe_n = []
    for d in (mul(e, -1.0), e):
        kappe_n.append(len(nn))
        nn.append((round(d[0] * 4096), round(d[1] * 4096), round(d[2] * 4096), 0))

    def gewickelt(a, b, cc, nrm):
        """Reihenfolge so, dass cross(b-a, c-a) . Normale < 0 (Wicklung des Messernetzes)."""
        pa, pb, pc = nv[a][:3], nv[b][:3], nv[cc][:3]
        return dot(cross(sub(pb, pa), sub(pc, pa)), nrm) < 0

    for s in range(SEGMENTE):
        s2 = (s + 1) % SEGMENTE
        a0, a1, b0, b1_ = ring[0][s], ring[0][s2], ring[1][s], ring[1][s2]
        ni = seiten_n[s]
        nrm = nn[ni][:3]
        # PSX-Quad Z-Muster: 0-1 eine Kante, 2-3 die Gegenkante; Kultur-Test auf (0,1,2).
        if gewickelt(a0, a1, b0, nrm):
            q = (a0, a1, b0, b1_)
            uv = quad_uv(UV_U0, UV_V0, UV_U0, UV_V1, UV_U1, UV_V0, UV_U1, UV_V1)
        else:
            q = (a1, a0, b1_, b0)
            uv = quad_uv(UV_U0, UV_V1, UV_U0, UV_V0, UV_U1, UV_V1, UV_U1, UV_V0)
        qf.append((ni, q[0], ni, q[1], ni, q[2], ni, q[3]))
        quv.append(uv)
    # Kappen als Dreiecksfaecher.
    for ende_i in (0, 1):
        ni = kappe_n[ende_i]
        nrm = nn[ni][:3]
        r = ring[ende_i]
        for s in range(1, SEGMENTE - 1):
            a, b, cc = r[0], r[s], r[s + 1]
            if not gewickelt(a, b, cc, nrm):
                b, cc = cc, b
            tf.append((ni, a, ni, b, ni, cc))
            tuv.append(tri_uv(UV_U0, UV_V0, UV_U1, UV_V0, UV_U1, UV_V1))

    info = dict(achse=e, start=start, ende=ende, radius=radius, laenge_messer=laenge_messer,
                laenge_rohr=laenge_rohr, hand_pkt=len(hand_pkt), messer_pkt=len(messer_pkt),
                hand_q=len(hand_q), hand_t=len(hand_t))
    return nv, nn, tf, tuv, qf, quv, info


def max_abstand(punkte):
    return max(math.sqrt(p[0] ** 2 + p[1] ** 2 + p[2] ** 2) for p in punkte)


def hand_netz11(pld_md1):
    m = pld_md1
    length, unk, oc = struct.unpack_from("<III", m, 0)
    h = struct.unpack_from("<14I", m, 12 + 11 * 56)
    b = 12
    tv = [struct.unpack_from("<4h", m, b + h[0] + 8 * k)[:3] for k in range(h[1])]
    qv = [struct.unpack_from("<4h", m, b + h[7] + 8 * k)[:3] for k in range(h[8])]
    return tv + qv


def main():
    ergebnisse = {}
    for fam in ("PL00", "PL04"):
        knife_path = os.path.join(PLD, fam + "W01.PLW")
        d = open(knife_path, "rb").read()
        s, diroff = plw_split(d)
        assert len(s) == 4
        grenzen = s[1:] + [diroff]
        teile = [d[a:b] for a, b in zip(s, grenzen)]
        knife = md1_parse(teile[2])
        nv, nn, tf, tuv, qf, quv, info = rohr_netz(knife)
        md1 = md1_build(nv, nn, tf, tuv, qf, quv, knife["unk"])
        out = plw_build([teile[0], teile[1], md1, teile[3]])
        out_path = os.path.join(PLD, fam + "W02.PLW")
        open(out_path, "wb").write(out)
        hand = hand_netz11(open(os.path.join(PLD, fam + ".MD1"), "rb").read())
        hand_spitze = max_abstand(hand)
        messer_spitze = max_abstand([p[:3] for p in knife["v"]])
        rohr_spitze = max_abstand([p[:3] for p in nv])
        k = (REACH_MESSER - REACH_GRUND) / (messer_spitze - hand_spitze)
        reach = round(REACH_MESSER + k * (rohr_spitze - messer_spitze))
        ergebnisse[fam] = dict(reach=reach, k=k, hand=hand_spitze, messer=messer_spitze, rohr=rohr_spitze,
                               sha=hashlib.sha256(out).hexdigest(), quelle=hashlib.sha256(d).hexdigest(),
                               info=info, groesse=len(out), tf=len(tf), qf=len(qf), nv=len(nv))
        print(f"{fam}W02.PLW  {len(out)} B  sha256 {ergebnisse[fam]['sha']}")
        print(f"   Quelle {fam}W01.PLW sha256 {ergebnisse[fam]['quelle']}")
        print(f"   Netz: {len(nv)} Punkte, {len(tf)} Dreiecke, {len(qf)} Vierecke "
              f"(Hand {info['hand_pkt']} Punkte / {info['hand_t']}+{info['hand_q']} Flaechen unveraendert)")
        print(f"   Messerlaenge {info['laenge_messer']:.1f}, Rohrlaenge {info['laenge_rohr']:.1f}, Radius {info['radius']:.1f}")
        print(f"   Spitzen (Abstand zum Bone-11-Ursprung): Hand {hand_spitze:.1f}  Messer {messer_spitze:.1f}  Rohr {rohr_spitze:.1f}")
        print(f"   k = 100 / ({messer_spitze:.1f} - {hand_spitze:.1f}) = {k:.4f}   Reichweite Rohr = {reach}")
    leon = ergebnisse["PL00"]
    with open(INC, "w", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/pipe_r41/gen_pipe_plw.py -- nicht von Hand aendern.\n")
        f.write(" * Reichweite der PIPE (Item 2) = NUTZERWUNSCH \"mehr Reichweite als das Messer\" (16.11.2026),\n")
        f.write(" * geeicht an den Netzen: Messer 1100 (@0x8006E5A4) gegen Grundwert 1000 (@0x8006E5A0).\n")
        for fam, r in ergebnisse.items():
            f.write(f" *   {fam}: Handspitze {r['hand']:.1f}, Messerspitze {r['messer']:.1f}, Rohrspitze {r['rohr']:.1f}, "
                    f"k {r['k']:.4f} -> {r['reach']}\n")
        f.write(" * Die Tabelle hat EINE Zeile je Charakterklasse mit identischen Werten (@0x8006E5A0 / @0x8006E5F8);\n")
        f.write(" * der Port nimmt die Leon-Eichung fuer beide. */\n")
        f.write(f"#define RE15_PIPE_R41_REICHWEITE {leon['reach']}\n")
        f.write(f"#define RE15_PIPE_R41_PL00W02_SHA256 \"{ergebnisse['PL00']['sha']}\"\n")
        f.write(f"#define RE15_PIPE_R41_PL04W02_SHA256 \"{ergebnisse['PL04']['sha']}\"\n")
    print("geschrieben:", INC)
    return 0


if __name__ == "__main__":
    sys.exit(main())
