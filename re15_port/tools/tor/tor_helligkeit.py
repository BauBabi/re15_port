#!/usr/bin/env python3
"""tor_helligkeit.py - Helligkeit einer Tuersequenz im Port gegen Texel und gemaltes Vorbild.

Runde 32, Nutzer-Befund "Das von uns erstellte Tor in ROOM 1170 ... ist zu dunkel."
Dossier: analysis/befunde_runde32/tor_helligkeit.md.

Aufruf
    python re15_port/tools/tor/tor_helligkeit.py licht                 # Lichtrechnung Tor + DOOR2E
    python re15_port/tools/tor/tor_helligkeit.py bild <ppm> tor <var> [bild [bk]]  # Serienbild gegen Modell
    python re15_port/tools/tor/tor_helligkeit.py bild <ppm> 2E <var> [bild]        # dito RE2 DOOR2E
      (bild = Nummer der Door_move-Schleife: Objektstand aus tuerkatalog.VM; ohne = Aufbau-Stand)
    python re15_port/tools/tor/tor_helligkeit.py gemalt                # Schild in Cut 0/11/12

Was gemessen wird
    Der Port zeichnet eine Tuerecke als Texel x Eckfarbe / 128 (PSX-Modulation, psx-spx
    GPU:1444; Port: render_pc.c psx_prim_to_sdl_vert). Die Eckfarbe kommt aus NCCT mit dem
    RE2-Tuerlicht (analysis/tor_1170/08_re_zeichnen.md 1.1/3):
      L    @0x8009a470  (400,800,-500) (-1800,-1000,-2700) (3500,6700,1200)
      LCM  @0x8009a490  neunmal 1600
      BK   68<<4, mit Objekt-Flag 0x1000 136<<4   (@0x800142ac lhu 304(s1), @0x800142b4 andi 0x1000,
           @0x800142bc..c4 addiu 136 / @0x800142e8..f0 addiu 68, @0x800142d4..dc / @0x80014300..08 ctc2)
      RGBC 0x808080 (Mesh binden @0x80014b4c..5c)
    `bild` rastert das Modell im selben Bild (Lage/Drehung aus dem Door_model_set der Variante,
    Kamera 08 1.2, H 290, Bildmitte 160/120, Abtastphase 0.375 wie render_pc.c) im 3x-Raster
    der Rueckleseserie (960x720), und vergleicht je Bildpunkt:
      F  = gezeigter Wert im Serienbild (RE15_TUER_SERIE, SDL_RenderReadPixels, beschleunigter Renderer)
      T  = Texel (TIM 5 Bit << 3, wie re15_tim_rgb555_to_argb8888)
      P  = gemaltes Original an derselben Stelle (nur Tor: der Cut-12-Bildpunkt, aus dem der Texel
           stammt - tor_modell.textur_bauen vor der 5-Bit-Rundung)
      M  = vorhergesagte Modulation min(c,128)/128 (PC-Kappung) bzw. c/128 (PSX)
    nur im Inneren der Dreiecke (Abstand >= 1,5 Bildpunkte im 3x-Raster von jeder Kante).
"""
import json
import math
import os
import re
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import do2_format as fmt  # noqa: E402

PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
TOR_INC = os.path.join(PORT, "engine", "src", "gen", "tor_1170_door.inc")
RE2_DOOR = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR%02X.DO2")
RE2_EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")

L_TUER = np.array([[400, 800, -500], [-1800, -1000, -2700], [3500, 6700, 1200]], np.int64)  # @0x8009a470
LCM_TUER = 1600                                                                              # @0x8009a490
KAMERA = np.array([[0, 0, 4096], [0, 4096, 0], [-4096, 0, 0]], np.int64)   # 08 1.2 (@0x80076cb0)
KAMERA_T = np.array([0, 0, 10000], np.int64)
H = 290                                                                    # @0x80013e34
ABTAST = 0.375                                                             # re15_abtastphase.h:106
SKAL = 3                                                                   # Rueckleseserie 960x720


# ---------------------------------------------------------------------------------------------
# Archive
# ---------------------------------------------------------------------------------------------
def archiv(name):
    """-> (md1, tim, skripte). name 'tor' = eingebackenes Torarchiv, sonst RE2-Nummer hex."""
    if name == "tor":
        txt = open(TOR_INC).read()
        body = txt[txt.index("re15_tor1170_door["):]
        teil = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", body))
        md1_rel, tim_rel = struct.unpack_from("<II", teil, 0)
        skripte = fmt.scd_lesen(teil[8:md1_rel])
        md1 = fmt.Md1.lesen(teil[md1_rel:tim_rel])
        tim, _ = fmt.Tim.lesen(teil[tim_rel:])
        return md1, tim, skripte
    d = fmt.Do2.lesen(open(RE2_DOOR % int(name, 16), "rb").read())
    return fmt.Md1.lesen(d.md1), fmt.Tim.lesen(d.tim)[0], d.skripte


def tim_rgb(tim):
    """TIM -> (H, W, 3) int wie der Port (5 Bit << 3) und Maske 'gezeichnet' (Wert != 0)."""
    w, h = tim.breite_px, tim.hoehe_px
    pal = np.array(tim.clut[:256], np.int64)
    idx = np.frombuffer(tim.pix, np.uint8).reshape(h, w)
    c = pal[idx]
    rgb = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1)
    return rgb, c != 0


def modell_saetze(skripte, variante):
    """Door_model_set (0x4D, 22 B) am Anfang des Aufbauskripts der Variante (Skript 1 + variante)."""
    s = skripte[1 + variante]
    out, p = [], 0
    while p + 22 <= len(s) and s[p] == 0x4D:
        b = s[p:p + 22]
        out.append(dict(obj=b[1], mesh=b[5], flags=struct.unpack_from("<H", b, 6)[0],
                        pos=struct.unpack_from("<3h", b, 10), rot=struct.unpack_from("<3H", b, 16)))
        p += 22
    return out


# ---------------------------------------------------------------------------------------------
# RE2-Rechnung (08_re_zeichnen.md 1.4 RotMatrix, 1.1 Schritte e/i/k, 4.3 NCCT)
# ---------------------------------------------------------------------------------------------
_TAB = None


def rcossin(a):
    global _TAB
    if _TAB is None:
        d = open(RE2_EXE, "rb").read()
        ta = struct.unpack_from("<I", d, 0x18)[0]
        _TAB = struct.unpack_from("<4096I", d, 0x800 + 0x800ADEAC - ta)   # rcossin_tbl @0x800adeac
    s16 = lambda v: ((v & 0xFFFF) ^ 0x8000) - 0x8000
    a = s16(a)
    if a < 0:
        w = _TAB[(-a) & 0xFFF]
        return -s16(w), s16(w >> 16)
    w = _TAB[a & 0xFFF]
    return s16(w), s16(w >> 16)


def rotmatrix(rx, ry, rz):
    """RotMatrix @0x8008e1f4 (Tabelle 08 1.4), Produkte >> 12."""
    sx, cx = rcossin(rx)
    sy, cy = rcossin(ry)
    sz, cz = rcossin(rz)
    m = np.zeros((3, 3), np.int64)
    m[0, 0] = (cz * cy) >> 12
    m[0, 1] = (-(sz * cy)) >> 12
    m[0, 2] = sy
    t8 = (cz * -sy) >> 12
    t8b = (sz * -sy) >> 12
    m[1, 0] = ((sz * cx) >> 12) - ((t8 * sx) >> 12)
    m[1, 1] = ((cz * cx) >> 12) + ((t8b * sx) >> 12)
    m[1, 2] = (-(cy * sx)) >> 12
    m[2, 0] = ((sz * sx) >> 12) + ((t8 * cx) >> 12)
    m[2, 1] = ((cz * sx) >> 12) - ((t8b * cx) >> 12)
    m[2, 2] = (cy * cx) >> 12
    return m


def mul(a, b):
    """MulMatrix0 (MVMVA sf=1 je Spalte): (a*b) >> 12, IR gesaettigt."""
    return np.clip((a @ b) >> 12, -32768, 32767)


def ncct(llm, n, bk):
    """NCCT sf=1 lm=1, RGBC 0x808080 (08 4.3 Handrechnung): Rueckgabe Eckfarbe 0..255."""
    ir = np.clip((llm @ np.asarray(n, np.int64)) >> 12, 0, 32767)
    irc = min(max(((bk << 4) * 4096 + LCM_TUER * int(ir.sum())) >> 12, 0), 32767)
    return min(max(((((128 << 4) * irc) >> 12) >> 4), 0), 255)


def welt(satz):
    r = rotmatrix(*satz["rot"])
    w = mul(KAMERA, r)
    t = np.clip(((KAMERA @ np.array(satz["pos"], np.int64)) >> 12) + KAMERA_T, -32768, 32767)
    return w, t


def projizieren(w, t, v):
    s = (w @ np.array(v[:3], np.int64)) >> 12
    mac = s + t
    ir = np.clip(mac, -32768, 32767)
    sz = int(min(max(mac[2], 0), 0xFFFF))
    n = (H * 65536 // sz) if sz else 0x1FFFF            # Naeherung der UNR-Division (bildpunktgenau)
    return 160 + ((int(ir[0]) * n) >> 16), 120 + ((int(ir[1]) * n) >> 16), sz


def dreiecke(md1, tim_rgb_, satz, bk_override=None, satz_vor=None):
    """-> Liste der gezeichneten Dreiecke des Satzes: (xy[3], uv[3], c[3], nclip).
    satz_vor: Stand des Objekts im VORIGEN Bild (Licht mit W_vorbild, 08 1.3); ohne = statisch."""
    mesh = md1.meshes[satz["mesh"]]
    w, t = welt(satz)
    w_vor = welt(satz_vor)[0] if satz_vor is not None else w
    llm = mul(L_TUER, w_vor)      # LLM = L * W_vorbild (Schritt e liest obj+84 vor Schritt i)
    bk = bk_override if bk_override is not None else (136 if satz["flags"] & 0x1000 else 68)
    aus = []
    for tri, tx in zip(mesh.tris, mesh.tri_tex):
        n0, v0, n1, v1, n2, v2 = tri
        e = [projizieren(w, t, mesh.vertices[v]) for v in (v0, v1, v2)]
        (x0, y0, _), (x1, y1, _), (x2, y2, _) = e
        mac0 = x0 * y1 + x1 * y2 + x2 * y0 - x0 * y2 - x1 * y0 - x2 * y1
        if mac0 < 0:
            continue
        otz = (341 * (e[0][2] + e[1][2] + e[2][2])) >> 12
        if otz < 64:
            continue
        c = [ncct(llm, mesh.normals[k][:3], bk) for k in (n0, n1, n2)]
        uv = [(tx[0], tx[1]), (tx[3], tx[4]), (tx[6], tx[7])]
        aus.append(dict(xy=[(x0, y0), (x1, y1), (x2, y2)], uv=uv, c=c, n=mesh.normals[n0][:3]))
    return aus


# ---------------------------------------------------------------------------------------------
# Rastern im 3x-Raster der Rueckleseserie
# ---------------------------------------------------------------------------------------------
def rastern(tris, tex, tex_ok, gemalt=None, rand=1.5):
    """-> dict von Bildern (720x960): T, P, c, innen, id."""
    Hh, Ww = 240 * SKAL, 320 * SKAL
    T = np.zeros((Hh, Ww, 3), np.float32)
    P = np.zeros((Hh, Ww, 3), np.float32)
    C = np.zeros((Hh, Ww), np.float32)
    innen = np.zeros((Hh, Ww), bool)
    ident = np.full((Hh, Ww), -1, np.int32)
    for k, tr in enumerate(tris):       # Reihenfolge egal: nur Innenpunkte, Ueberdeckung verworfen
        x = np.array([p[0] for p in tr["xy"]], float) + ABTAST
        y = np.array([p[1] for p in tr["xy"]], float) + ABTAST
        xs, ys = x * SKAL, y * SKAL
        x0, x1 = max(int(xs.min()), 0), min(int(math.ceil(xs.max())), Ww - 1)
        y0, y1 = max(int(ys.min()), 0), min(int(math.ceil(ys.max())), Hh - 1)
        if x0 > x1 or y0 > y1:
            continue
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(d) < 1e-9:
            continue
        l0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / d
        l1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / d
        l2 = 1 - l0 - l1
        # Abstand zur Kante in Bildpunkten des 3x-Rasters
        def kante(la, i, j):
            ln = math.hypot(xs[i] - xs[j], ys[i] - ys[j])
            return la * abs(d) / ln if ln > 0 else 0 * la
        dist = np.minimum(np.minimum(kante(l0, 1, 2), kante(l1, 2, 0)), kante(l2, 0, 1))
        drin = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        sub = (slice(y0, y1 + 1), slice(x0, x1 + 1))
        uu = l0 * tr["uv"][0][0] + l1 * tr["uv"][1][0] + l2 * tr["uv"][2][0]
        vv = l0 * tr["uv"][0][1] + l1 * tr["uv"][1][1] + l2 * tr["uv"][2][1]
        ti = np.clip(np.floor(uu).astype(int), 0, tex.shape[1] - 1)
        tj = np.clip(np.floor(vv).astype(int), 0, tex.shape[0] - 1)
        cc = l0 * tr["c"][0] + l1 * tr["c"][1] + l2 * tr["c"][2]
        ok = drin & tex_ok[tj, ti]
        # schon belegt -> Ueberdeckung, beide verwerfen
        doppelt = ok & (ident[sub] >= 0)
        innen[sub][doppelt] = False
        neu = ok & (ident[sub] < 0)
        ident[sub][neu] = k
        T[sub][neu] = tex[tj, ti][neu]
        if gemalt is not None:
            P[sub][neu] = gemalt[tj, ti][neu]
        C[sub][neu] = cc[neu]
        innen[sub][neu & (dist >= rand)] = True
    return dict(T=T, P=P, c=C, innen=innen, id=ident)


def lum(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def ppm(pfad):
    from PIL import Image
    return np.asarray(Image.open(pfad).convert("RGB"), np.float32)


def bereich_tor(uv):
    v = uv[1]
    if v < 80:
        return "schild"
    if v < 128:
        return "rohr"
    return "laschen_fuesse"


# ---------------------------------------------------------------------------------------------
# Befehle
# ---------------------------------------------------------------------------------------------
def licht():
    """Eckfarben an den Flaechennormalen des Tors (statisches Bild, Variante 0/1) und DOOR2E."""
    out = {}
    for name in ("tor", "2E"):
        md1, tim, sk = archiv(name)
        tex, ok = tim_rgb(tim)
        for var in (0, 1):
            for satz in modell_saetze(sk, var):
                if satz["mesh"] >= len(md1.meshes):
                    continue
                for bk in (68, 136):
                    tr = dreiecke(md1, tex, satz, bk)
                    cs = [c for t in tr for c in t["c"]]
                    key = "%s V%d obj%d mesh%d flags 0x%04x BK %d" % (name, var, satz["obj"], satz["mesh"], satz["flags"], bk)
                    # Flaechengewicht (Bildflaeche)
                    fl, cw = 0.0, 0.0
                    for t in tr:
                        (a, b), (c_, d), (e, f) = t["xy"]
                        A = abs((c_ - a) * (f - b) - (e - a) * (d - b)) / 2.0
                        fl += A
                        cw += A * sum(t["c"]) / 3.0
                    out[key] = dict(dreiecke=len(tr), c_min=min(cs) if cs else None, c_max=max(cs) if cs else None,
                                    c_flaechenmittel=round(cw / fl, 1) if fl else None,
                                    ueber_128=sum(1 for c in cs if c > 128), ecken=len(cs))
                    if name == "tor" and satz["mesh"] == 0:
                        sch = [t for t in tr if bereich_tor(t["uv"][0]) == "schild"]
                        if sch:
                            out[key]["schild_c"] = sorted(set(c for t in sch for c in t["c"]))
                            out[key]["schild_normale"] = sch[0]["n"]
    print(json.dumps(out, indent=1))
    return out


def vm_saetze(name, var, n):
    """Objektstaende der Bilder n und n-1 aus dem Katalog-Simulator (tuerkatalog.VM, derselbe,
    gegen den unit_door_seq die Port-Maschine Bild fuer Bild prueft)."""
    import tuerkatalog as tkat
    import tuerseq_referenz as tref
    if name == "tor":
        txt = open(TOR_INC).read()
        body = txt[txt.index("re15_tor1170_door["):]
        teil = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", body))
        door = tref.TeilTuer(teil, 0)
    else:
        door = tkat.Door(int(name, 16))
    vm = tkat.VM(door, variant=var, sound_ready_tick=0).run()
    def conv(st):
        return {o["obj"]: dict(obj=o["obj"], mesh=o["mesh"], flags=o["flags"], parent=o["parent"],
                               pos=tuple(o["pos"]), rot=tuple(r & 0xFFFF for r in o["rot"])) for o in st}
    return conv(vm.frames[n]), conv(vm.frames[n - 1])


def bild(pfad, name, var, bk_override=None, gemalt_an=True, bildnr=None):
    md1, tim, sk = archiv(name)
    tex, ok = tim_rgb(tim)
    gemalt = None
    if name == "tor" and gemalt_an:
        import tor_modell as tm
        g, _ = tm.textur_bauen()
        gemalt = np.asarray(g)[..., :3].astype(np.float32)
    F = ppm(pfad)
    alle = []
    bereiche = {}
    if bildnr is None:
        saetze = [(sz, None) for sz in modell_saetze(sk, var)]
    else:
        jetzt, vor = vm_saetze(name, var, bildnr)
        saetze = [(sz, vor.get(k)) for k, sz in sorted(jetzt.items()) if sz["parent"] < 0]
    for satz, satz_vor in saetze:
        if satz["mesh"] >= len(md1.meshes):
            continue
        if name != "tor" and satz["obj"] != 0:
            continue                                   # RE2: nur das Blatt (Objekt 0)
        tr = dreiecke(md1, tex, satz, bk_override, satz_vor)
        for t in tr:
            t["bereich"] = ("pfosten" if satz["mesh"] == 1 else bereich_tor(t["uv"][0])) if name == "tor" else "blatt"
        alle += tr
    R = rastern(alle, tex, ok, gemalt)
    m = R["innen"]
    # Modellpruefung: vorhergesagt (PC) gegen gezeigt
    sdl = np.minimum(255, np.floor((R["c"] * 255 + 64) / 128))     # psx_prim_to_sdl_vert
    pred_pc = R["T"] * (sdl / 255.0)[..., None]
    pred_psx = np.minimum(255, R["T"] * (R["c"] / 128.0)[..., None])
    res = dict(bild=os.path.basename(pfad), archiv=name, variante=var, bildpunkte_innen=int(m.sum()))
    res["abweichung_F_gegen_modell_pc_mittel"] = round(float(np.abs(lum(F) - lum(pred_pc))[m].mean()), 2)
    res["abweichung_F_gegen_modell_psx_mittel"] = round(float(np.abs(lum(F) - lum(pred_psx))[m].mean()), 2)
    # Bildpunkte mit Eckfarbe ueber 0x80 (dort unterscheiden sich PC-Kappung und PSX, psx-spx GPU:349-354)
    mu = m & (R["c"] > 128.5) & (lum(R["T"]) >= 8)
    res["ueber_0x80"] = dict(bildpunkte=int(mu.sum()))
    if mu.sum():
        res["ueber_0x80"].update(
            F_durch_T=round(float(lum(F)[mu].sum() / lum(R["T"])[mu].sum()), 4),
            modell_pc_durch_T=round(float(lum(pred_pc)[mu].sum() / lum(R["T"])[mu].sum()), 4),
            modell_psx_durch_T=round(float(lum(pred_psx)[mu].sum() / lum(R["T"])[mu].sum()), 4),
            abweichung_pc=round(float(np.abs(lum(F) - lum(pred_pc))[mu].mean()), 2),
            abweichung_psx=round(float(np.abs(lum(F) - lum(pred_psx))[mu].mean()), 2))
    for b in sorted(set(t["bereich"] for t in alle)):
        ids = [k for k, t in enumerate(alle) if t["bereich"] == b]
        mb = m & np.isin(R["id"], ids) & (lum(R["T"]) >= 8)
        if mb.sum() == 0:
            continue
        e = dict(bildpunkte=int(mb.sum()))
        e["F_mittel"] = round(float(lum(F)[mb].mean()), 2)
        e["T_mittel"] = round(float(lum(R["T"])[mb].mean()), 2)
        e["F_durch_T"] = round(float(lum(F)[mb].sum() / lum(R["T"])[mb].sum()), 4)
        e["c_mittel"] = round(float(R["c"][mb].mean()), 2)
        e["modell_pc_durch_T"] = round(float(lum(pred_pc)[mb].sum() / lum(R["T"])[mb].sum()), 4)
        e["modell_psx_durch_T"] = round(float(lum(pred_psx)[mb].sum() / lum(R["T"])[mb].sum()), 4)
        if gemalt is not None:
            e["P_mittel"] = round(float(lum(R["P"])[mb].mean()), 2)
            e["F_durch_P"] = round(float(lum(F)[mb].sum() / lum(R["P"])[mb].sum()), 4)
            e["T_durch_P"] = round(float(lum(R["T"])[mb].sum() / lum(R["P"])[mb].sum()), 4)
            e["zu_dunkel_faktor_P_durch_F"] = round(float(lum(R["P"])[mb].sum() / lum(F)[mb].sum()), 3)
        bereiche[b] = e
    # gesamt
    mb = m & (lum(R["T"]) >= 8)
    g = dict(bildpunkte=int(mb.sum()), F_durch_T=round(float(lum(F)[mb].sum() / lum(R["T"])[mb].sum()), 4),
             c_mittel=round(float(R["c"][mb].mean()), 2))
    if gemalt is not None:
        g["F_durch_P"] = round(float(lum(F)[mb].sum() / lum(R["P"])[mb].sum()), 4)
        g["zu_dunkel_faktor_P_durch_F"] = round(float(lum(R["P"])[mb].sum() / lum(F)[mb].sum()), 3)
    res["gesamt"] = g
    res["bereiche"] = bereiche
    print(json.dumps(res, indent=1))
    return res


def gemalt_cuts():
    """Schild des gemalten Tors in Cut 0/11/12: mittlere Helligkeit (Bildpunkte, die das
    Modell-Schild im jeweiligen Cut deckt, Deckung >= 0,99)."""
    import tor_modell as tm
    import tor_kamera as tk
    S = tm.Sammler()
    tm.fluegel_bauen(S)
    tex, _ = tm.textur_bauen()
    texa = np.asarray(tex)
    out = {}
    x0, y0, bw, bh = tm.BEREICH["schild"]
    for cut in (0, 11, 12):
        bg = tk.hintergrund(cut).astype(np.float32)
        tris = tm.raum_projektion(S, cut)
        # nur Schild-Dreiecke (uv im Schildbereich)
        sch = [t for t in tris if all(y0 <= q[1] < y0 + bh for q in t[3])]
        _, m, _ = tm.rastern(sch, texa, bg.shape[1], bg.shape[0], ueber=4, nclip=False)
        mm = m >= 0.99
        out["cut%02d" % cut] = dict(bildpunkte=int(mm.sum()), schild_lum=round(float(lum(bg)[mm].mean()), 2))
    t = texa[y0:y0 + bh, x0:x0 + bw, :3].astype(np.float32)
    out["textur_schild_gemalt_lum"] = round(float(lum(t).mean()), 2)
    t5 = ((t.astype(np.int64) >> 3) << 3).astype(np.float32)
    out["textur_schild_5bit_lum"] = round(float(lum(t5).mean()), 2)
    print(json.dumps(out, indent=1))
    return out


def main(argv):
    if not argv or argv[0] == "licht":
        licht()
    elif argv[0] == "bild":
        nr = int(argv[4]) if len(argv) > 4 else None
        bk = int(argv[5]) if len(argv) > 5 else None
        bild(argv[1], argv[2], int(argv[3]), bk, bildnr=nr)
    elif argv[0] == "gemalt":
        gemalt_cuts()
    else:
        print(__doc__)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
