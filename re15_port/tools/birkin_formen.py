#!/usr/bin/env python3
"""Beta-Weiterentwicklung des Birkin-Modells EM30 (Typ 0x30) und roter Affe erzeugen.

    python re15_port/tools/birkin_formen.py                 # EMDs schreiben
    python re15_port/tools/birkin_formen.py --blatt DIR     # + Ansichtsblaetter nach DIR

Ausgabe (vom PC-Port ueber RE15_BIRKIN_FORM geladen, platform/pc/main.c):
    re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD     Hangar 50A0/50A1 und Labor 50E0/50F1 (RE15_BIRKIN_FORM=hangar)

⛔ KEIN BYTE-TRUE-ASSET. Diese Form existiert auf der ausgelieferten Disc nicht. Sie ist NUR aus
Beta-Screenshots bekannt (Hangar C-2: Birkin packt Elza und schleudert sie). Jede Zahl in FORMEN
unten ist eine GESTALTUNGSENTSCHEIDUNG nach diesen Bildern, kein RE-Wert, und hat deshalb
absichtlich KEINE @0x-Adresse. Sie darf nirgends als Original-Konstante zitiert werden.

Die Bahnsteig-Form ist seit Nachtrag 38 kein umgehaltener EM30 mehr, sondern RE2-G4 als Vorlage:
tools/birkin_bahnsteig.py. umhalten()/haltung/gang bleiben hier als Werkzeug, keine Form nutzt sie derzeit.

Byte-true geerbt (unveraendert aus CDEMD0.EMS Blob 16 = EM30):
    EDD (alle 21 Clips), EMR-Hierarchie, Keyframes (Hangar), Dreieckstopologie, UVs,
    TPages, CLUT-Zuordnung, TIM-Pixel. Nur diese Dinge aendern sich:
      1. MD1-Vertices je Knochen  - skaliert um den Knochenursprung (Mesh-lokaler Raum)
      2. MD1-Normalen             - inverse-transponiert mitgefuehrt, Laenge beibehalten
      3. EMR-Bindoffsets          - Kindgelenk mit der Skalierung des Elternmeshes mitgezogen,
                                    damit die Glieder verbunden bleiben
      4. (faerbe_tim: CLUT-Einfaerbung, derzeit von keiner Form genutzt)
      5. Hangar (Runde 41 Spur SF): Schwanz als eigene Knochenkette 16-22 an Knochen 8 (schwanz_kette): EMR-Bank 0
         bekommt 7 Knochen und je Keyframe 7 x 36 Bit dahinter (die ersten 84 Byte jedes Keyframes bleiben EM30),
         Mesh 16 (knochenlose Hand mit Rohr) entfaellt, Meshes 16-22 = Glieder. Sektionen und Verzeichnis werden
         neu gerechnet. Vorher (bis Runde 41) war der Schwanz starre Geometrie an Mesh 8.
      6. Hangar: Umtexturieren einzelner Teile (umtexturieren: nur UV/CLUT/TPage, vorhandene Texel)
    Der MD1-Serializer schreibt unveraendertes EM30 byte-gleich zurueck (Assert in main), und
    re15_emd_parse_container liest die Datei ohne Aenderung (tests/unit/test_birkin_formen.c).

Knochen von EM30 (aus dem EMR gelesen, Parent in Klammern):
    0 Becken (Wurzel)   1-3 Bein rechts  4-6 Bein links   7 Bauch (0)   8 Brust (7)
    9 Kopf (8)          10-12 Arm links (8)               13-15 Arm rechts mit Sichel/Klaue (8)
    16-22 Schwanzkette (8, dann je Vorglied) - nur EM30_HANGAR
"""
import argparse, os, struct, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402  (Leser: EMS-Kette, MD1, EMR, TIM)

AUS_DIR = os.path.join(EA.REPO, 're15_port', 'shared_assets', 'PSX', 'EMD')
TYP     = 0x30

# Gestaltung je Form (siehe Docstring: Nachbildung nach Screenshots, keine RE-Werte).
#   skal[b]  = (sx, sy, sz) im Mesh-lokalen Raum von Knochen b. Y laeuft bei den
#              Gliedmassen entlang des Knochens (Laenge), X/Z ist die Dicke.
#   rel[b]   = (dx, dy, dz) Zusatzverschiebung des Bindoffsets von b NACH dem Mitziehen.
#   tint     = None oder (Matrix 3x3 auf RGB, Helligkeit) fuer alle CLUT-Eintraege.
FORMEN = {
    # Bilder 2-10 (Hangar C-2): der vorhandene Koerper, aber oben massiger. Buckel und
    # Schultern breiter und hoeher, Kopf tiefer zwischen die Schultern gesunken, beide
    # Arme deutlich dicker und etwas laenger (Haende auf Kniehoehe), Beine unveraendert.
    'hangar': dict(
        datei='EM30_HANGAR.EMD',
        titel='Birkin Hangar-Form',
        # Runde 41 SF N2: Brust quer (z) 1,30 -> 1,15 und Arme in der Dicke 1,30-1,40 -> 1,17-1,23 (Anteil 0,57 der alten
        # Verdickung). Video 4 (von Hand nachgezeichnete Umrisse, SF_belege/nb2) ist schmaler als die Form: Hoehe/Breite
        # Video 1,70 im Mittel (h0/40/64/100/116/124), Port mit Haltung 1,47; Gittersuche (tools/r41_sichel/nb2/anpassen.py
        # form_variante): IoU gleich (0,667 -> 0,665), H/B 1,65. Schultern/Arme stehen im Video nicht seitlich ab.
        skal={
            7:  (1.15, 1.10, 1.15),
            8:  (1.30, 1.25, 1.15),
            9:  (1.05, 1.05, 1.05),
            10: (1.20, 1.12, 1.20), 11: (1.229, 1.12, 1.229), 12: (1.171, 1.15, 1.171),
            13: (1.20, 1.12, 1.20), 14: (1.229, 1.12, 1.229), 15: (1.171, 1.15, 1.171),
        },
        # Runde 41 SF N2 (Kopf): vorher (60, 170, 0) = Kopf 170 tiefer zwischen die Schultern (Beta-Screenshots). Video 4
        # zeigt die rote/helle Kopfkuppe AN der Oberkante des Umrisses (h0 2, h100 0, h116 3, h124 5 Bildpunkte darunter;
        # Port vorher 14-25, mit der geduckten Haltung 31-39) -> Kopf 350 hoeher im Brustraum (Gittersuche
        # tools/r41_sichel/nb2/kopfmass2.py: dy -1/8/8/4). Groesse unveraendert (sichtbare Breite Video 20-30, Port 19-23).
        rel={9: (60, -180, 0)},
        tint=None,
        # Runde 41 SF N2 (Haltung): geduckt, breitbeinig, Oberkoerper vor, Arme naeher am Koerper - fuer ALLE Clips der Bank 0
        # (ducken()). Werte = Koordinatensuche gegen die von Hand nachgezeichneten Video-Umrisse h0/h40/h64/h100/h116/h124
        # (SF_belege/nb2/umrisse_video.json; tools/r41_sichel/nb2/anpassen.py, Lage aus dem Video-Fusspunkt, Clip-Bild/Gier je
        # Bild frei): IoU nach Verschiebung 0,585 -> 0,667. GESTALTUNG, keine RE-Werte.
        ducken=dict(becken=110, spreiz=340, knie_aussen=0.2, rumpf=3, brust=20, kopf=0, arm_halten=0.575, arm_innen=20),
        # Nutzerhinweis 2026-10-30: "Die Hangar-Form hat auch direkt einen Tail am Ruecken."
        # Runde 41 Spur SF, Nutzerurteil 2026-11-02 "Der Schwanz ist viel zu starr": der Schwanz ist jetzt eine
        # eigene Kette aus 7 Knochen (16-22), angehaengt an Knochen 8 (Brust), Lage der Ruhegestalt wie bisher
        # (Mittellinie im Knochen-8-Raum, X vorn, Y unten). Gegen Complete Disc Video 4 geprueft (Teil T7 im
        # Dossier): in Clip 0x0B Bild 20 liegt er dort, wo Bild 1408-1420 das rot-weisse Glied zeigt (rechts
        # neben der Huefte bis zum Boden), von vorn haengt er zwischen den Beinen (1484-1500). Die Winkel je
        # Keyframe rechnet schwanz_bewegung (Nachschwingen aus der Rumpfbewegung, Gestaltung).
        schwanz_kette=dict(
            knochen=8,
            punkte=[(-380, 120, 0), (-760, 420, 60), (-1150, 860, 120), (-1500, 1380, 90),
                    (-1760, 1880, -20), (-1900, 2280, -120), (-1960, 2520, -160)],
            radien=[300, 260, 215, 170, 125, 80, 34],
            glieder=7, seiten=8, ueberlapp=0.45,
            # Runde 41 SF N1.5: rot-weiss gebaendert wie im Video (Schwanz h18-h28 Bild-links, h58-h70 Bild-rechts; vorher
            # Fleischtextur TPage 128 u 4-60 v 4-124). Bandstreifen TPage 130 u 0-42 v 92-130 (senkrechte hellere Streifen),
            # Laenge auf u (laengs='u') -> die Streifen liegen als Ringe um den Schwanz; eigene CLUT-Zeile 483 (cluts unten).
            uv=dict(clut=483 << 6, page=130, u=(0, 42), v=(92, 130), laengs='u'),
            # Nachschwingen (schwanz_bewegung): Feder und Nachlauf Wurzel -> Spitze, Daempfung je Bild, Schwere, Grenzwinkel
            bewegung=dict(steif=(0.22, 0.08), traeg=(0.55, 0.92), daempfung=0.80, schwere=0.004, grenze=55),
        ),
        # Umtexturieren (nur UV/CLUT/TPage, vorhandene Texel, keine neuen Farben). Quelle der UVs = die
        # Ausdehnung der gewaehlten Faces; Ziel = Rechteck im TIM. Teile T3-T6 im Dossier.
        # Runde 41 SF N1.6: Oberschenkel-Vierecke (Mesh 1/4, Ringe bei y ~76 und ~818) an y 360 teilen, damit die Jeans
        # dort enden koennen (vorher reichte das Viereck 76..818 als Ganzes bis ans Knie). Form unveraendert.
        teilen=[dict(meshes=(1, 4), schnitt=360)],
        umtexturieren=[
            # T5 (Runde 41 SF) zurueckgenommen in N1.2/N1.7: das rot-weisse Glied h18-h28 war der Schwanz, der Unterarm
            # (Mesh 14) traegt wieder die EM30-Haut (im Video h96-h100 rotgrau (55,43,46) am Ellbogen).
            # T7 Sichel (Mesh 15): eigene CLUT-Zeile 484 (dunkel wie im Video h128-h132, cluts unten), UVs unveraendert.
            dict(meshes=(15,), clut=484 << 6),
            # T3 Kurze Hose: Jeans am Schienbein (Mesh 2/5, TPage 129) und am Oberschenkel unterhalb y 360
            # (Mesh 1/4, nach teilen) -> Beinhaut wie der untere Schienbeinteil (TPage 128, CLUT 0, u 19-55 v 55-115).
            # Video h120-h132: Jeans y 136..157, Haut 157..178 (PSX, Blau-/Hautprofil der Beine) = Hosenende bei 62 % von
            # Bund bis Knie; im Port Bund 145, Knie (Knochen 2) 187 -> Ende bei y 171 = Oberschenkel-lokal ~350.
            # Vorher (y_ab 520) traf die Regel nur das Kniedreieck 818-951, die Jeans reichten bis y 187 (Lauf n1).
            dict(meshes=(2, 5), nur_page=129, page=128, clut=30720, u=(19, 55), v=(55, 115)),
            dict(meshes=(1, 4), y_ab=360, page=128, clut=30720, u=(19, 55), v=(55, 115)),
            # T4 Fuesse rot statt schwarzer Schuhe (alle Hangar-Bilder; 1355/1470 rote Fuesse) -> Wundfleisch
            # der Schulter (TPage 128, CLUT 0, u 66-126 v 4-90)
            dict(meshes=(3, 6), page=128, clut=30720, u=(66, 126), v=(4, 90)),
            # T6 Kopf: kein blondes Menschengesicht im Video (vorn 1460-1500: rotes Fleisch oben in der
            # Mitte) -> dasselbe Wundfleisch; Runde 41 SF N1.10 (Nutzer: "Kopf ... noch nicht ganz akkurat"): eigene
            # CLUT-Zeile 485 (Kopf heller und weniger gesaettigt, rosa Lichter wie im Video, cluts unten).
            dict(meshes=(9,), page=128, clut=485 << 6, u=(66, 126), v=(4, 90)),
        ],
        # Runde 41 SF N1.4/N1.5/N1.7: CLUT-Arbeit (nur Farben der Paletten, Texel unveraendert; Werte = Videomessung,
        # abgeglichen an der Buehne tools/r41_sichel/farbmass.py, die das Port-Fenster nachweislich trifft).
        #  Haut (Zeilen 480-482): Video h40-h64 Rumpf L 66 / Rotanteil 0,33 / hell (78,62,61) / rot (72,40,40), Modell vorher
        #   L 94 / 0,16 / (114,91,87) / (82,40,40) -> blasse Eintraege x0,64, rote (r/g >= 1,6) satter; Jeans (b > r) bleiben.
        #  Zeile 483 Schwanz aus Zeile 482: dunkle Streifen rot, helle weisslich (Video 2-Mittel (92,54,55) 63 % /
        #   (137,90,91); Buehne danach (90,47,54) 64 % / (138,96,97)).
        #  Zeile 484 Sichel aus Zeile 482: dunkel, fast unbunt (Video h128-h132 (21,23,18) = 0,58 der Hauthelligkeit; der
        #   Gruenstich ist Videokompression - der dunkle Torbogen daneben misst ebenso (14,16,16)). Lauf n2 mit
        #   (38,46,32)/(76,90,64) wirkte im Fenster olivgruen -> unbunt.
        cluts=dict(
            haut_zeilen=(0, 1, 2),
            haut=dict(dunkel=0.63, q0=1.2, q1=1.6, rot_hell=0.95, satt=0.82),
            # Schwanz: Schwellen 44/52 = Tal/Gipfel der Streifen (Spalten-L des Bandstreifens 41..63) -> klare Baender
            # Lauf n5 (echtes Fenster, Schwanz h20-h26 Bild-links): (99,66,66) 41 % / (176,128,128) - heller als das Video,
            # die Flanke steht dort im Licht -> helle Farbe x0,78, Rot satter; Lauf n6 (Schwellen 48/56): Farben getroffen
            # (88,50,50) / (142,96,96), aber nur 42 % rot (Video 63 %) -> Schwellen 52/58.
            zusatz=[dict(aus=2, l0=52, l1=58, dunkel=(96, 44, 46), hell=(140, 100, 100)),
                    dict(aus=2, l0=20, l1=90, dunkel=(40, 40, 35), hell=(84, 81, 72)),
                    # Zeile 485 Kopf aus Zeile 480 (Wundfleisch, L-Quantile 30/70 = 93/126): Video h108-h132 Kopfbereich
                    # Median r/g ~1,5, 40-50 % kraeftig rot, Lichter rosa (p90 ~(104-123,77-104,77-105)), 1,3x so hell
                    # wie der Rumpf; vorher im Fenster (Lauf n10) r/g 2,0, 72 % kraeftig rot, so hell wie der Rumpf.
                    # Erster Stand (110,62,60)/(200,165,160), Schwellen 93/126: im Fenster blassrosa (Lauf k1). Gittersuche an der
                    # Buehne (Gang Clip 1 B34, Kopf = Differenz mit/ohne Mesh 9): (120,60,58)/(175,135,132) 109/140 -> r/g 1,55,
                    # 42 % kraeftig rot, Kopf/Rumpf 1,41; x0,85 fuer Kopf/Rumpf ~1,2 wie das Video.
                    # Runde 41 SF N2: Kopf 350 hoeher -> Kopf/Rumpf an der Buehne (Gang h100-h124) 1,07-1,09 statt 1,2 (Video
                    # 1,11-1,28) -> beide Farben x1,12.
                    dict(aus=0, l0=109, l1=140, dunkel=(114, 57, 55), hell=(167, 129, 125))],
        ),
    ),
}

u16, u32 = EA.u16, EA.u32


# ═══════════════════════════════════════════════════ MD1: Vertex-/Normalen-Offsets
def md1_bloecke(md1):
    """Je Mesh die Byte-Offsets (relativ zum MD1-Anfang) der Vertex- und Normalenbloecke.
    Gleiches Layout wie EA.md1_parse; Tri- und Quad-Teil koennen auf DENSELBEN Block
    zeigen, darum wird ueber (Offset, Anzahl) dedupliziert."""
    obj, base, out = u32(md1, 8), 12, []
    for i in range(obj // 2):
        (tvo, tvc, tno, tnc, _tfo, _tfc, _tuo,
         qvo, qvc, qno, qnc, _qfo, _qfc, _quo) = struct.unpack_from('<14I', md1, 12 + i*56)
        vb = {(base + tvo, tvc), (base + qvo, qvc)}
        nb = {(base + tno, tnc), (base + qno, qnc)}
        out.append(([b for b in vb if b[1]], [b for b in nb if b[1]]))
    return out


def skaliere_md1(md1, skal):
    """Vertices v -> S v, Normalen n -> S^-1 n (auf die Ursprungslaenge gebracht)."""
    md1 = bytearray(md1)
    gesehen = set()                       # ein Block, den zwei Meshes teilen, nur einmal
    for b, (vbl, nbl) in enumerate(md1_bloecke(md1)):
        if b not in skal:
            continue
        s = np.array(skal[b], dtype=np.float64)
        for off, n in vbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                v = np.rint(np.array([x, y, z]) * s).astype(int)
                v = np.clip(v, -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(v[0]), int(v[1]), int(v[2]), w)
        for off, n in nbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                alt = np.array([x, y, z], dtype=np.float64)
                laenge = np.linalg.norm(alt)
                if laenge == 0:
                    continue
                neu = alt / s
                neu = neu / np.linalg.norm(neu) * laenge
                neu = np.clip(np.rint(neu).astype(int), -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(neu[0]), int(neu[1]), int(neu[2]), w)
    return bytes(md1)


# ═══════════════════════════════════════════════════ MD1: neu serialisieren + Schwanz
def md1_lesen(md1):
    """MD1 in bearbeitbare Listen. EM30 teilt je Mesh EINEN Vertex- und EINEN Normalenblock
    zwischen Tri- und Quad-Teil (tvo==qvo, tno==qno) - das wird geprueft und beibehalten."""
    ms = EA.md1_parse(md1)
    out = []
    for i, m in enumerate(ms):
        t = struct.unpack_from('<14I', md1, 12 + i*56)
        assert t[0] == t[7] and t[2] == t[9], 'Mesh %d: getrennte Vertexbloecke' % i
        out.append(dict(v=[list(x) for x in m.tv], n=[list(x) for x in m.tn],
                        tf=[list(x) for x in m.tf], tuv=[list(x) for x in m.tuv],
                        qf=[list(x) for x in m.qf], quv=[list(x) for x in m.quv]))
    return out


def md1_schreiben(meshes):
    """Gleiche Blockreihenfolge wie das Original: Tabelle, alle Vertexbloecke, alle
    Normalenbloecke, je Mesh Tri-/Quad-Faces, je Mesh Tri-/Quad-UVs. Unveraendertes EM30
    ergibt damit byte-gleich die Original-MD1 (siehe main --selbsttest)."""
    n = len(meshes)
    cur = n * 56
    vo, no, tfo, qfo, tuo, quo = [], [], [], [], [], []
    for m in meshes:
        vo.append(cur); cur += 8 * len(m['v'])
    for m in meshes:
        no.append(cur); cur += 8 * len(m['n'])
    for m in meshes:
        tfo.append(cur); cur += 12 * len(m['tf'])
        qfo.append(cur); cur += 16 * len(m['qf'])
    uv_start = cur
    for m in meshes:
        tuo.append(cur); cur += 12 * len(m['tf'])
        quo.append(cur); cur += 16 * len(m['qf'])
    out = bytearray(12 + cur)
    struct.pack_into('<III', out, 0, 12 + uv_start, 0, n * 2)
    for i, m in enumerate(meshes):
        nv, nn = len(m['v']), len(m['n'])
        struct.pack_into('<14I', out, 12 + i*56, vo[i], nv, no[i], nn, tfo[i], len(m['tf']), tuo[i],
                         vo[i], nv, no[i], nn, qfo[i], len(m['qf']), quo[i])
        for k, x in enumerate(m['v']):   struct.pack_into('<4h', out, 12 + vo[i] + k*8, *x)
        for k, x in enumerate(m['n']):   struct.pack_into('<4h', out, 12 + no[i] + k*8, *x)
        for k, x in enumerate(m['tf']):  struct.pack_into('<6H', out, 12 + tfo[i] + k*12, *x)
        for k, x in enumerate(m['qf']):  struct.pack_into('<8H', out, 12 + qfo[i] + k*16, *x)
        for k, x in enumerate(m['tuv']): struct.pack_into('<BBHBBHBBH', out, 12 + tuo[i] + k*12, *x)
        for k, x in enumerate(m['quv']): struct.pack_into('<BBHBBHBBHBBH', out, 12 + quo[i] + k*16, *x)
    return bytes(out)


def _catmull(p, n):
    """Catmull-Rom durch die Stuetzpunkte, n Zwischenschritte je Abschnitt."""
    p = [np.array(x, float) for x in p]
    p = [2*p[0] - p[1]] + p + [2*p[-1] - p[-2]]
    out = []
    for i in range(1, len(p) - 2):
        for k in range(n):
            t = k / n
            a, b, c, d = p[i-1], p[i], p[i+1], p[i+2]
            out.append(0.5 * ((2*b) + (-a + c)*t + (2*a - 5*b + 4*c - d)*t*t
                              + (-a + 3*b - 3*c + d)*t*t*t))
    out.append(p[-2])
    return out


def haenge_schwanz_an(meshes, sw):
    """Verjuengte Roehre entlang der Mittellinie + Spitze, in Mesh sw['knochen'].
    Windung wie im Original: cross(v1-v0, v2-v0) zeigt GEGEN die Normale (alle 347 Quads
    von EM30 erfuellen das). Normalen in der Originallaenge 4096."""
    m = meshes[sw['knochen']]
    mitte = _catmull(sw['punkte'], sw['unterteilung'])
    rad = np.interp(np.linspace(0, len(sw['radien']) - 1, len(mitte)),
                    np.arange(len(sw['radien'])), sw['radien'])
    S = sw['seiten']
    uv = sw['uv']
    v0, n0 = len(m['v']), len(m['n'])
    ringe = []
    oben = np.array([0.0, -1.0, 0.0])
    for i, c in enumerate(mitte):
        t = mitte[min(i+1, len(mitte)-1)] - mitte[max(i-1, 0)]
        t /= np.linalg.norm(t)
        a = np.cross(t, oben); a /= np.linalg.norm(a)
        b = np.cross(t, a)
        ring = []
        for k in range(S):
            w = 2*np.pi*k/S
            r = np.cos(w)*a + np.sin(w)*b
            p = c + r*rad[i]
            ring.append(len(m['v']))
            m['v'].append([int(round(x)) for x in p] + [0])
            m['n'].append([int(round(x)) for x in r*4096] + [0])
        ringe.append(ring)
    spitze_dir = mitte[-1] - mitte[-2]; spitze_dir /= np.linalg.norm(spitze_dir)
    spitze = mitte[-1] + spitze_dir * rad[-1] * 3
    sv = len(m['v'])
    m['v'].append([int(round(x)) for x in spitze] + [0])
    m['n'].append([int(round(x)) for x in spitze_dir*4096] + [0])

    def u_(k):  return int(round(uv['u'][0] + (uv['u'][1]-uv['u'][0]) * k / S))
    def v_(i):  return int(round(uv['v'][0] + (uv['v'][1]-uv['v'][0]) * i / (len(ringe)-1)))

    def orient(ia, ib, ic):
        P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
        N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
        return np.cross(P[1]-P[0], P[2]-P[0]) @ N < 0

    for i in range(len(ringe) - 1):
        for k in range(S):
            k2 = (k + 1) % S
            a, b = ringe[i][k], ringe[i][k2]
            c, d = ringe[i+1][k], ringe[i+1][k2]
            uvs = [(u_(k), v_(i)), (u_(k+1), v_(i)), (u_(k), v_(i+1)), (u_(k+1), v_(i+1))]
            if not orient(a, b, c):           # Quad-Split (v0,v1,v2)+(v1,v3,v2): a<->b tauschen
                a, b, c, d = b, a, d, c
                uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
            m['qf'].append([a - v0 + n0, a, b - v0 + n0, b, c - v0 + n0, c, d - v0 + n0, d])
            m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                             uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
    letzte = ringe[-1]
    for k in range(S):
        a, b = letzte[k], letzte[(k + 1) % S]
        if not orient(a, b, sv):
            a, b = b, a
        m['tf'].append([a - v0 + n0, a, b - v0 + n0, b, sv - v0 + n0, sv])
        m['tuv'].append([u_(k), uv['v'][1], uv['clut'], u_(k+1), uv['v'][1], uv['page'],
                         (u_(k)+u_(k+1))//2, uv['v'][1], 0])
    assert v0 == n0
    return meshes


# ═══════════════════════════════════════════════════ Schwanzkette (eigene Knochen, Runde 41 Spur SF)
# Nutzerurteil 2026-11-02: "Der Schwanz ist viel zu starr". Statt starrer Geometrie an Mesh 8 bekommt der
# Schwanz eine eigene Knochenkette, ANGEHAENGT an den Knochenbaum (Knochen 16 .. 16+N-1), damit die Indizes
# 0-15 (KI, Szene Knochen 15, Clips) unveraendert bleiben. Jedes Glied hat sein Mesh (Mesh i == Knochen i),
# laeuft entlang seiner lokalen +Y und reicht mit einer Manschette (ueberlapp) in das vorige Glied, damit die
# Gelenke bei Biegung keine Luecke zeigen (E9 Bahnsteig-Form: optisch aus einem Stueck). Die Schwanzwinkel
# jedes Keyframes kommen aus einer Nachschwing-Rechnung ueber die Rumpfbewegung je Clip (schwanz_bewegung).
# Alle Zahlen hier sind Gestaltung nach dem Video (keine RE-Werte).

def _min_rot(a, b):
    """Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues)."""
    a = a / np.linalg.norm(a); b = b / np.linalg.norm(b)
    v = np.cross(a, b); c = float(a @ b); s = np.linalg.norm(v)
    if s < 1e-9:
        if c > 0:
            return np.eye(3)
        q = np.cross(a, [1.0, 0, 0]) if abs(a[0]) < 0.9 else np.cross(a, [0, 1.0, 0])
        q /= np.linalg.norm(q)
        return 2 * np.outer(q, q) - np.eye(3)
    k = v / s
    K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
    return np.eye(3) + s * K + (1 - c) * (K @ K)


def _quant(L):
    """Lokale Drehung so, wie der Port sie aus dem Keyframe liest (12-Bit-Winkel, RotMatrix @0x80068130)."""
    return EA.mat_euler(*_zerlege(L)).astype(float) / 4096.0


def kette_ruhe(sw):
    """Ruhegestalt: Gelenkpunkte im Raum des Ankerknochens (gleichlange Abschnitte auf der Catmull-Rom-Linie),
    Gliedlaengen, Ruhe-Lokaldrehungen (quantisiert) und Bindversaetze der neuen Knochen."""
    linie = np.array(_catmull(sw['punkte'], 24))
    seg = np.linalg.norm(np.diff(linie, axis=0), axis=1)
    s = np.r_[0, np.cumsum(seg)]
    n = sw['glieder']
    ziel = np.linspace(0, s[-1], n + 1)
    p = np.array([np.interp(ziel, s, linie[:, k]) for k in range(3)]).T
    L = np.linalg.norm(np.diff(p, axis=0), axis=1)
    lokal, W = [], np.eye(3)
    for i in range(n):
        d = W.T @ (p[i+1] - p[i])
        Li = _quant(_min_rot(np.array([0.0, 1.0, 0.0]), d))
        lokal.append(Li)
        W = W @ Li
    rel = [tuple(int(round(c)) for c in p[0])] + [(0, int(round(L[i-1])), 0) for i in range(1, n)]
    rad = np.interp(ziel, np.linspace(0, s[-1], len(sw['radien'])), sw['radien'])
    return dict(p=p, L=[int(round(x)) for x in L], lokal=lokal, rel=rel, radien=rad, s=ziel / s[-1])


def kette_meshes(sw, ruhe):
    """Je Glied ein Rohr-Mesh im Knochenraum (Y = Laengsachse): Manschette bei -ueberlapp, Ringe bei 0, L/2, L;
    das letzte Glied endet in einer Spitze. Normalen radial in Laenge 4096, Windung wie haenge_schwanz_an."""
    S, uv = sw['seiten'], sw['uv']
    out = []
    n = sw['glieder']
    for i in range(n):
        Li, r0, r1 = ruhe['L'][i], ruhe['radien'][i], ruhe['radien'][i+1]
        s0, s1 = ruhe['s'][i], ruhe['s'][i+1]
        ue = sw['ueberlapp'] * r0 if i > 0 else 0.0
        ringe_y = ([-ue] if ue else []) + [0.0, Li / 2.0, float(Li)]
        ringe_r = ([r0] if ue else []) + [r0, (r0 + r1) / 2.0, r1]
        ringe_s = ([s0] if ue else []) + [s0, (s0 + s1) / 2.0, s1]
        m = dict(v=[], n=[], tf=[], tuv=[], qf=[], quv=[])
        ringe = []
        for y, r in zip(ringe_y, ringe_r):
            ring = []
            for k in range(S):
                w = 2 * np.pi * k / S
                nx, nz = np.cos(w), np.sin(w)
                ring.append(len(m['v']))
                m['v'].append([int(round(nx * r)), int(round(y)), int(round(nz * r)), 0])
                m['n'].append([int(round(nx * 4096)), 0, int(round(nz * 4096)), 0])
            ringe.append(ring)
        if i == n - 1:
            sv = len(m['v'])
            m['v'].append([0, int(round(Li + 2.5 * r1)), 0, 0])
            m['n'].append([0, 4096, 0, 0])

        laengs = uv.get('laengs') == 'u'
        def u_(k):  return int(round(uv['u'][0] + (uv['u'][1] - uv['u'][0]) * k / S))
        def v_(t):  return int(round(uv['v'][0] + (uv['v'][1] - uv['v'][0]) * t))

        def tx(k, t):
            """(u, v) fuer Seite k und Laengenanteil t. laengs='u' (Runde 41 SF N1.5): Laenge auf u, Umfang auf v, damit
            die senkrechten Streifen des Bandstreifens (TPage 130 u 0-42) als Ringe ueber dem Schwanz liegen."""
            if laengs:
                return (int(round(uv['u'][0] + (uv['u'][1] - uv['u'][0]) * t)),
                        int(round(uv['v'][0] + (uv['v'][1] - uv['v'][0]) * k / S)))
            return (u_(k), v_(t))

        def orient(ia, ib, ic):
            P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
            N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
            return np.cross(P[1] - P[0], P[2] - P[0]) @ N < 0
        for j in range(len(ringe) - 1):
            for k in range(S):
                k2 = (k + 1) % S
                a, b = ringe[j][k], ringe[j][k2]
                c, d = ringe[j+1][k], ringe[j+1][k2]
                uvs = [tx(k, ringe_s[j]), tx(k + 1, ringe_s[j]), tx(k, ringe_s[j+1]), tx(k + 1, ringe_s[j+1])]
                if not orient(a, b, c):
                    a, b, c, d = b, a, d, c
                    uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
                m['qf'].append([a, a, b, b, c, c, d, d])
                m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                                 uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
        if i == n - 1:
            letzte = ringe[-1]
            for k in range(S):
                a, b = letzte[k], letzte[(k + 1) % S]
                if not orient(a, b, sv):
                    a, b = b, a
                m['tf'].append([a, a, b, b, sv, sv])
                p0, p1 = tx(k, 1.0), tx(k + 1, 1.0)
                m['tuv'].append([p0[0], p0[1], uv['clut'], p1[0], p1[1], uv['page'],
                                 (p0[0] + p1[0]) // 2, (p0[1] + p1[1]) // 2, 0])
        out.append(m)
    return out


def emr_mit_kette(emr, sw, ruhe, winkel):
    """EMR-Bank 0 mit N angehaengten Knochen neu schreiben. Knochen 0-15: Bindversaetze, Kinderlisten und
    Keyframe-Bits unveraendert (16 x 36 Bit = 72 Byte, byteweise kopiert); Knochen 16 haengt am Anker,
    16+i am Vorgaenger. winkel[kf] = Liste von N (ax, ay, az) im 12-Bit-Format."""
    s = EA.emr_parse(bytes(emr))
    nb0, n = s.bone_count, sw['glieder']
    nb = nb0 + n
    tab0 = u16(emr, 0)
    kinder = []
    for b in range(nb0):
        cc, co = u16(emr, tab0 + b*4), u16(emr, tab0 + b*4 + 2)
        kinder.append(list(emr[tab0 + co:tab0 + co + cc]) if cc else [])
    kinder[sw['knochen']].append(nb0)
    for i in range(n):
        kinder.append([nb0 + i + 1] if i < n - 1 else [])
    rel = list(s.rel) + list(ruhe['rel'])
    tab = (8 + 6 * nb + 3) & ~3
    kb = bytearray()
    eintraege = []
    for b in range(nb):
        eintraege.append((len(kinder[b]), (4 * nb + len(kb)) if kinder[b] else 0))
        kb += bytes(kinder[b])
    kf_off = (tab + 4 * nb + len(kb) + 3) & ~3
    alt_ks = s.kf_size
    assert (nb0 * 36) % 8 == 0 and alt_ks == 12 + nb0 * 36 // 8, 'Altknochen nicht byteweise kopierbar'
    ks = 12 + (((nb * 36 + 7) // 8 + 3) & ~3)
    out = bytearray(kf_off + ks * s.kf_count)
    struct.pack_into('<4H', out, 0, tab, kf_off, nb, ks)
    for b, (x, y, z) in enumerate(rel):
        struct.pack_into('<3h', out, 8 + b*6, x, y, z)
    for b, (cc, co) in enumerate(eintraege):
        struct.pack_into('<HH', out, tab + b*4, cc, co)
    out[tab + 4*nb:tab + 4*nb + len(kb)] = kb
    for k in range(s.kf_count):
        alt = s.kf_data[k*alt_ks:(k+1)*alt_ks]
        o = kf_off + k*ks
        out[o:o + alt_ks] = alt
        for i, w in enumerate(winkel[k]):
            for j in range(3):
                _bits_schreiben(out, o + 12, (nb0 + i)*36 + j*12, w[j] & 0xFFF)
    return bytes(out)


def _exp(w):
    """Drehvektor -> Matrix (Rodrigues)."""
    t = np.linalg.norm(w)
    if t < 1e-12:
        return np.eye(3)
    k = w / t
    K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
    return np.eye(3) + np.sin(t) * K + (1 - np.cos(t)) * (K @ K)


def _log(R):
    """Matrix -> Drehvektor."""
    c = np.clip((np.trace(R) - 1) / 2, -1.0, 1.0)
    t = np.arccos(c)
    if t < 1e-9:
        return np.zeros(3)
    w = np.array([R[2, 1] - R[1, 2], R[0, 2] - R[2, 0], R[1, 0] - R[0, 1]])
    return w * (t / (2 * np.sin(t))) if t < np.pi - 1e-6 else w / max(np.linalg.norm(w), 1e-9) * t


def schwanz_bewegung(emr, edd, sw, ruhe):
    """Nachschwing-Rechnung je Clip, Gelenk fuer Gelenk im Winkelraum ("Wackelknochen"): jedes Glied hat eine
    Abweichung th (Drehvektor, lokal hinter seiner Ruhedrehung) und eine Winkelgeschwindigkeit om. Dreht sich das
    Vorglied (bzw. die Brust), behaelt das Glied zum Anteil 'traeg' seine Weltlage (Nachlauf), die Feder 'steif'
    zieht es zur Ruhe zurueck, 'daempfung' nimmt die Schwingung heraus, 'schwere' zieht die Glieder nach unten.
    Weil jedes Glied am TATSAECHLICHEN Vorglied haengt, laeuft die Verzoegerung zur Spitze hin auf (Peitsche beim
    Angriff, Schwingen beim Gehen, Folgebewegung beim Werfen). Kreisende Clips (erstes ~ letztes Bild) laufen
    dreimal, gespeichert wird der letzte Umlauf; sonst Start in Ruhe. Rueckgabe: winkel[kf] = N x (ax, ay, az)
    12 Bit; ein Keyframe, den mehrere Bilder nutzen, nimmt das erste Bild."""
    mo = EA.Modell.__new__(EA.Modell)
    mo.skel = EA.emr_parse(bytes(emr))
    clips, frames = EA.edd_parse(bytes(edd))
    n, A = sw['glieder'], sw['knochen']
    dyn = sw['bewegung']
    k = np.linspace(dyn['steif'][0], dyn['steif'][1], n)
    f = np.linspace(dyn['traeg'][0], dyn['traeg'][1], n)
    grenze = np.radians(dyn['grenze'])
    unten = np.array([0.0, 1.0, 0.0])
    winkel = {}
    ruhe_w = [_zerlege(Li) for Li in ruhe['lokal']]
    posen = {}

    rel0 = np.array(ruhe['rel'][0], float)
    rad = ruhe['radien']

    def anker(kf):
        if kf not in posen:
            R, T = mo.pose(kf)
            posen[kf] = (R[A].astype(float) / 4096.0, T[A].astype(float))
        return posen[kf][0]

    def boden(kf, i, P, W, basis):
        """Glied i von P aus; liegt sein Ende unter y = -0,5 Radius (Boden, Y unten), in der Senkrechten
        hochdrehen (schleift auf dem Boden). Rueckgabe: neue Weltdrehung oder None."""
        d = W @ np.array([0.0, 1.0, 0.0])
        Li = ruhe['L'][i]
        grenze_y = -0.5 * rad[i + 1]
        if P[1] + Li * d[1] <= grenze_y:
            return None
        dy = np.clip((grenze_y - P[1]) / Li, -1.0, 1.0)
        h = d - d[1] * unten
        h = h / np.linalg.norm(h) if np.linalg.norm(h) > 1e-6 else np.array([-1.0, 0.0, 0.0])
        d2 = h * np.sqrt(max(0.0, 1 - dy * dy)) + unten * dy
        return _min_rot(d, d2) @ W

    for c, (f0, nf) in enumerate(clips):
        kfs = [frames[f0 + i] & 0xFFF for i in range(nf)]
        kreis = nf > 8 and np.linalg.norm(anker(kfs[0]) - anker(kfs[-1])) < 0.15
        th, om = np.zeros((n, 3)), np.zeros((n, 3))
        welt_alt = None
        for u in range(3 if kreis else 1):
            for kf in kfs:
                W = anker(kf)
                P = W @ rel0 + posen[kf][1]
                welt = []
                for i in range(n):
                    basis = W @ ruhe['lokal'][i]
                    if welt_alt is not None:     # Nachlauf: Anteil f der alten Weltlage behalten
                        th_traeg = _log(basis.T @ welt_alt[i])
                        th[i] = th[i] + f[i] * (th_traeg - th[i])
                    # Schwere: Drehmoment zur Senkrechten, im Gliedraum
                    achse = basis @ _exp(th[i]) @ np.array([0.0, 1.0, 0.0])
                    om[i] = dyn['daempfung'] * om[i] - k[i] * th[i] + dyn['schwere'] * (basis.T @ np.cross(achse, unten))
                    th[i] = th[i] + om[i]
                    t = np.linalg.norm(th[i])
                    if t > grenze:
                        th[i] *= grenze / t; om[i] *= 0.5
                    W = basis @ _exp(th[i])
                    W2 = boden(kf, i, P, W, basis)
                    if W2 is not None:
                        W = W2
                        th[i] = _log(basis.T @ W)
                        om[i] *= 0.3
                    P = P + W @ np.array([0.0, ruhe['L'][i], 0.0])
                    welt.append(W)
                welt_alt = welt
                if u == (2 if kreis else 0) and kf not in winkel:
                    Wp, w = anker(kf), []
                    for i in range(n):
                        Li = Wp.T @ welt[i]
                        w.append(_zerlege(Li))
                        Wp = Wp @ _quant(Li)
                    winkel[kf] = w
    return [winkel.get(kk, ruhe_w) for kk in range(mo.skel.kf_count)]


def umtexturieren(meshes, regeln):
    """Faces auf ein anderes TIM-Rechteck legen: UVs der gewaehlten Faces linear von ihrer eigenen
    Ausdehnung auf (u, v) des Ziels abbilden, CLUT und TPage setzen. Filter je Regel:
    nur_page = nur Faces dieser TPage; y_ab = nur Faces, deren Vertices alle Mesh-lokal y >= y_ab haben."""
    for r in regeln:
        for b in r['meshes']:
            m = meshes[b]
            wahl = []                     # (Liste, Index, Eckenzahl)
            for art, fl, ul, n in (('t', m['tf'], m['tuv'], 3), ('q', m['qf'], m['quv'], 4)):
                for i, (f, uv) in enumerate(zip(fl, ul)):
                    if 'nur_page' in r and uv[5] != r['nur_page']:
                        continue
                    if 'y_ab' in r and min(m['v'][f[2*k+1]][1] for k in range(n)) < r['y_ab']:
                        continue
                    wahl.append((ul, i, n))
            if not wahl:
                continue
            us = [ul[i][3*k] for ul, i, n in wahl for k in range(n)]
            vs = [ul[i][3*k+1] for ul, i, n in wahl for k in range(n)]
            su, sv = (min(us), max(us)), (min(vs), max(vs))

            def abb(x, s, t):
                return int(round(t[0] + (x - s[0]) * (t[1] - t[0]) / max(1, s[1] - s[0])))
            for ul, i, n in wahl:
                uv = list(ul[i])
                if 'u' in r:
                    for k in range(n):
                        uv[3*k], uv[3*k+1] = abb(uv[3*k], su, r['u']), abb(uv[3*k+1], sv, r['v'])
                uv[2], uv[5] = r['clut'], r.get('page', uv[5])
                ul[i] = uv
    return meshes


def teile_quer(meshes, regeln):
    """Runde 41 SF N1.6 (kurze Hose): Vierecke, die die Ebene y = schnitt (Mesh-lokal) kreuzen, an dieser Ebene in zwei
    Vierecke teilen. Die Form bleibt gleich (neue Ecken liegen auf den alten Kanten, Normalen und UVs linear dazwischen);
    danach kann umtexturieren(y_ab=schnitt) den unteren Teil allein umlegen. EM30-Viereck = (0,1,3)+(0,3,2): Kanten 0-1,
    1-3, 3-2, 2-0."""
    for r in regeln:
        y_s = r['schnitt']
        for b in r['meshes']:
            m = meshes[b]
            neu_f, neu_uv = [], []
            for f, uv in zip(m['qf'], m['quv']):
                vi, ni = f[1::2], f[0::2]
                y = [m['v'][j][1] for j in vi]
                oben = [k for k in range(4) if y[k] < y_s]
                if len(oben) != 2 or min(y) >= y_s or max(y) <= y_s:
                    neu_f.append(f); neu_uv.append(uv); continue
                if sorted(oben) in ([0, 1], [2, 3]):
                    kanten = [(0, 2), (1, 3)]           # Schnitt auf den Kanten 0-2 und 1-3
                elif sorted(oben) in ([0, 2], [1, 3]):
                    kanten = [(0, 1), (2, 3)]
                else:
                    raise AssertionError('Mesh %d: Viereck mit diagonaler Lage zur Schnittebene' % b)
                neue = {}
                for a, c in kanten:
                    t = (y_s - y[a]) / float(y[c] - y[a])
                    P = [int(round(m['v'][vi[a]][q] + t * (m['v'][vi[c]][q] - m['v'][vi[a]][q]))) for q in range(3)]
                    P[1] = y_s
                    N = np.array(m['n'][ni[a]][:3], float) * (1 - t) + np.array(m['n'][ni[c]][:3], float) * t
                    N = N / max(np.linalg.norm(N), 1e-9) * 4096
                    m['v'].append(P + [0]); m['n'].append([int(round(x)) for x in N] + [0])
                    U = (int(round(uv[3*a] + t * (uv[3*c] - uv[3*a]))), int(round(uv[3*a+1] + t * (uv[3*c+1] - uv[3*a+1]))))
                    neue[(a, c)] = (len(m['n']) - 1, len(m['v']) - 1, U)
                def ecke(k):
                    return (ni[k], vi[k], (uv[3*k], uv[3*k+1]))
                (a0, c0), (a1, c1) = kanten
                if kanten == [(0, 2), (1, 3)]:
                    teil = [[ecke(0), ecke(1), neue[(0, 2)], neue[(1, 3)]], [neue[(0, 2)], neue[(1, 3)], ecke(2), ecke(3)]]
                else:
                    teil = [[ecke(0), neue[(0, 1)], ecke(2), neue[(2, 3)]], [neue[(0, 1)], ecke(1), neue[(2, 3)], ecke(3)]]
                for q in teil:
                    neu_f.append([x for e in q for x in (e[0], e[1])])
                    neu_uv.append([q[0][2][0], q[0][2][1], uv[2], q[1][2][0], q[1][2][1], uv[5],
                                   q[2][2][0], q[2][2][1], 0, q[3][2][0], q[3][2][1], 0])
            m['qf'], m['quv'] = neu_f, neu_uv
    return meshes


# ═══════════════════════════════════════════════════ TIM: CLUT-Zeilen (Runde 41 SF N1.4-N1.7)
def _clut(tim):
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', tim, 8)
    c = np.array(struct.unpack_from('<%dH' % (cw * ch), tim, 20), dtype=np.int64).reshape(ch, cw)
    return cx, cy, cw, ch, c, 8 + cs


def _rgb(c):
    return np.stack([(c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8], -1).astype(float)


def _pack(rgb, alt):
    q = np.clip(np.rint(rgb / 8.0), 0, 31).astype(np.int64)
    w = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10) | (alt & 0x8000)
    w = np.where(alt == 0, 0, np.where(w == 0, 1, w))      # 0 bleibt transparent, nichts wird neu transparent
    return w


def haut_zeile(c, p):
    """Haut dunkler und roter gefleckt (Video 4, N1.4): je Eintrag Rotgrad q = r/g; Blaues (b > r, Jeans) bleibt.
    blass = alt * dunkel; rot = (r*rot_hell, g*rot_hell*satt, b*rot_hell*satt); Mischung w = linear in q zwischen q0..q1."""
    rgb = _rgb(c); r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    q = r / np.maximum(g, 8.0)
    w = np.clip((q - p['q0']) / (p['q1'] - p['q0']), 0, 1)[..., None]
    blass = rgb * p['dunkel']
    rot = np.stack([r * p['rot_hell'], g * p['rot_hell'] * p['satt'], b * p['rot_hell'] * p['satt']], -1)
    neu = blass * (1 - w) + rot * w
    neu = np.where((b > r)[..., None], rgb, neu)
    return _pack(neu, c)


def band_zeile(c, p):
    """Zweifarbige Zeile nach Helligkeit des Texels: dunkle Eintraege -> p['dunkel'], helle -> p['hell'] (linear
    zwischen den Helligkeits-Quantilen l0..l1 der Zeile). Farben = Videomessung (N1.5 Schwanz, N1.7 Sichel)."""
    rgb = _rgb(c); L = rgb @ np.array([0.299, 0.587, 0.114])
    w = np.clip((L - p['l0']) / (p['l1'] - p['l0']), 0, 1)[..., None]
    neu = np.array(p['dunkel'], float) * (1 - w) + np.array(p['hell'], float) * w
    return _pack(neu, c)


def cluts_bauen(tim, cl):
    """Hautzeilen umfaerben und Zusatzzeilen (aus einer Quellzeile abgeleitet) anhaengen. Die Texel bleiben; nur der
    CLUT-Block waechst (h 3 -> 3 + n), Pixelblock unveraendert dahinter."""
    cx, cy, cw, ch, orig, ende = _clut(tim)
    c = orig.copy()
    for z in cl.get('haut_zeilen', ()):
        c[z] = haut_zeile(orig[z], cl['haut'])
    extra = [band_zeile(orig[zz['aus']], zz) for zz in cl.get('zusatz', ())]   # Zusatzzeilen aus der Originalzeile
    alle = np.vstack([c] + [e[None] for e in extra]).astype('<u2')
    blk = struct.pack('<I4H', 12 + alle.size * 2, cx, cy, cw, alle.shape[0]) + alle.tobytes()
    return bytes(tim[:8]) + blk + bytes(tim[ende:])


# ═══════════════════════════════════════════════════ EMR: Bindoffsets mitziehen
def passe_emr_an(emr, skal, extra):
    """rel[b] liegt im Raum des Eltern-Knochens -> mit dessen Mesh-Skalierung mitziehen."""
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    for b in range(s.bone_count):
        p = s.parent[b]
        x, y, z = s.rel[b]
        if p >= 0 and p in skal:
            sx, sy, sz = skal[p]
            x, y, z = x*sx, y*sy, z*sz
        dx, dy, dz = extra.get(b, (0, 0, 0))
        x, y, z = (int(round(c)) for c in (x + dx, y + dy, z + dz))
        struct.pack_into('<3h', emr, 8 + b*6, x, y, z)
    return bytes(emr)


# ═══════════════════════════════════════════════════ TIM: CLUTs einfaerben
def faerbe_tim(tim, tint):
    """15-Bit-CLUT-Eintraege umfaerben. 0x0000 (PSX: voll transparent) und das STP-Bit
    bleiben erhalten; ein Eintrag, der durch das Umfaerben 0 wuerde, bleibt mindestens 1."""
    if tint is None:
        return tim
    mat, hell = tint
    tim = bytearray(tim)
    cs  = u32(tim, 8)
    cw, ch = u16(tim, 16), u16(tim, 18)
    for k in range(cw * ch):
        o = 20 + k*2
        c = u16(tim, o)
        if c == 0:
            continue
        rgb = np.array([c & 31, (c >> 5) & 31, (c >> 10) & 31], dtype=np.float64)
        neu = np.clip(np.rint(mat @ rgb * hell), 0, 31).astype(int)
        w = int(neu[0]) | (int(neu[1]) << 5) | (int(neu[2]) << 10) | (c & 0x8000)
        if w == 0:
            w = 1
        struct.pack_into('<H', tim, o, w)
    assert 20 + cw*ch*2 <= 8 + cs
    return bytes(tim)


# ═══════════════════════════════════════════════════ Zusammenbau
# ═══════════════════════════════════════════════════ EMR: Keyframes umhalten
def _rz(grad):
    a = np.radians(grad)
    return np.array([[np.cos(a), -np.sin(a), 0], [np.sin(a), np.cos(a), 0], [0, 0, 1.0]])


def _zerlege(m):
    """Umkehrung von EA.mat_euler (RotMatrix @0x80068130): m[0][2]=sy, m[1][2]=-cy*sx,
    m[2][2]=cy*cx, m[0][0]=cz*cy, m[0][1]=-sz*cy. Roundtrip-Fehler <= 1/4096 Umdrehung."""
    k = 4096 / (2*np.pi)
    ay = np.arcsin(np.clip(m[0, 2], -1, 1))
    ax = np.arctan2(-m[1, 2], m[2, 2])
    az = np.arctan2(-m[0, 1], m[0, 0])
    return [int(round(a*k)) & 0xFFF for a in (ax, ay, az)]


def _bits_schreiben(buf, start, bitoff, wert):
    for i in range(12):
        bi = start + ((bitoff + i) >> 3)
        mask = 1 << ((bitoff + i) & 7)
        buf[bi] = (buf[bi] | mask) if (wert >> i) & 1 else (buf[bi] & ~mask)


def _welt(skel, rel, lokal, wurzel):
    n = skel.bone_count
    R, T = [None]*n, [None]*n
    for b in range(n):
        p = skel.parent[b]
        if p < 0:
            R[b], T[b] = lokal[b], np.array(wurzel, float)
        else:
            R[b] = R[p] @ lokal[b]
            T[b] = R[p] @ np.array(rel[b], float) + T[p]
    return R, T


def _rx(grad):
    a = np.radians(grad)
    return np.array([[1, 0, 0], [0, np.cos(a), -np.sin(a)], [0, np.sin(a), np.cos(a)]])


def _beine_ik(H, A, L1, L2, pol):
    """Zwei-Glied-IK: Huefte H, Ziel A (Fussgelenk), Laengen L1/L2, Knie zur Seite pol. Rueckgabe Knie, Ziel (geklemmt)."""
    d_vec = A - H
    d = float(np.linalg.norm(d_vec))
    d_kl = float(np.clip(d, abs(L1 - L2) + 1.0, L1 + L2 - 1.0))
    u = d_vec / max(d, 1e-9)
    A = H + u * d_kl
    a = (L1 * L1 - L2 * L2 + d_kl * d_kl) / (2 * d_kl)
    h = np.sqrt(max(0.0, L1 * L1 - a * a))
    w = pol - (pol @ u) * u
    w = w / max(np.linalg.norm(w), 1e-9)
    return H + u * a + w * h, A


def ducken(emr, p):
    """Runde 41 SF Nachbesserung 2 (GESTALTUNG nach Video 4, keine RE-Werte): geduckte, breitbeinige Haltung fuer ALLE
    Keyframes der EMR-Bank 0 (Stand, Gang, Angriffe, Griff/Wurf; die Baenke 1/2 sind Opfer-Clips des Spielers und bleiben).
    Je Keyframe, gewichtet mit der Aufrichtung des Rumpfs a = R7[1,1] (Anteil w = (a - 0,35)/0,45 in [0,1]; liegend -> 0,
    damit Niederschlag/Tod/Aufstehen nicht durch den Boden knicken):
      Becken     um becken*w tiefer (Wurzel-Y des Keyframes, Y zeigt nach unten)
      Beine      Fussgelenke behalten ihre Lage im Modellraum (Gangbild, Bodenkontakt) und ruecken je spreiz*w nach aussen
                 (Beckenquerachse waagerecht); Oberschenkel/Schienbein per Zwei-Glied-IK, Knie zur Seite des alten Knies
                 + knie_aussen x Aussenrichtung; Fuss behaelt seine Weltdrehung (flach wie im Clip)
      Rumpf      Bauch (7) um rumpf*w Grad, Brust (8) um brust*w Grad nach vorn (Drehung um Z im Elternraum)
      Kopf       (9) um kopf*w Grad (Z, im Brustraum)
      Arme       Oberarme (10, 13) behalten zum Anteil arm_halten ihre alte Weltdrehung (haengen trotz vorgebeugtem Rumpf)
                 und ruecken um arm_innen*w Grad zur Koerpermitte (Drehung um X im Brustraum)."""
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    nb, ks, kf_off = s.bone_count, s.kf_size, u16(emr, 2)
    rel = [np.array(r, float) for r in s.rel]
    L1, L2 = float(np.linalg.norm(rel[2])), float(np.linalg.norm(rel[3]))
    beine = ((1, 2, 3, -1.0), (4, 5, 6, 1.0))         # Knochen 1-3 rechts (rel z -229), 4-6 links
    for k in range(s.kf_count):
        o = kf_off + k * ks
        sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(emr[o:o + ks])
        lokal = [EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096 for b in range(nb)]
        wurzel = np.array(EA.kf_pos(sk, 0), float)
        R0, T0 = _welt(s, rel, lokal, wurzel)
        w = float(np.clip((R0[7][1, 1] - 0.35) / 0.45, 0.0, 1.0))
        if w <= 0.0:
            continue
        neu = list(lokal)
        wurzel1 = wurzel + np.array([0.0, p['becken'] * w, 0.0])
        # Rumpf, Kopf
        neu[7] = _rz(p['rumpf'] * w) @ neu[7]
        neu[8] = _rz(p['brust'] * w) @ neu[8]
        neu[9] = _rz(p.get('kopf', 0.0) * w) @ neu[9]
        # Beine
        quer = R0[0][:, 2].copy(); quer[1] = 0.0
        quer = quer / max(np.linalg.norm(quer), 1e-9)
        vorn = R0[0][:, 0].copy()
        R1, T1 = _welt(s, rel, neu, wurzel1)
        for hb, kb, fb, seite in beine:
            H = T1[hb]
            A = T0[fb] + quer * seite * p['spreiz'] * w
            u0 = T0[fb] - T0[hb]; u0 = u0 / max(np.linalg.norm(u0), 1e-9)
            pol0 = (T0[kb] - T0[hb]) - ((T0[kb] - T0[hb]) @ u0) * u0
            pol0 = pol0 / np.linalg.norm(pol0) if np.linalg.norm(pol0) > 1e-3 * L1 else vorn
            pol = pol0 + quer * seite * p['knie_aussen'] * w
            K, A = _beine_ik(H, A, L1, L2, pol)
            ey = np.array([0.0, 1.0, 0.0])
            Wo = _min_rot(R0[hb] @ ey, K - H) @ R0[hb]
            Wu = _min_rot(R0[kb] @ ey, A - K) @ R0[kb]
            neu[hb] = R1[0].T @ Wo
            neu[kb] = Wo.T @ Wu
            neu[fb] = Wu.T @ R0[fb]
        # Arme: Weltdrehung zum Anteil arm_halten behalten, dann zur Mitte
        R1, T1 = _welt(s, rel, neu, wurzel1)
        for ab, innen in ((10, -1.0), (13, 1.0)):     # 10 links (rel z +), 13 rechts (rel z -)
            folgt = R1[8] @ lokal[ab]
            alt = R0[8] @ lokal[ab]
            W = folgt @ _exp(p['arm_halten'] * _log(folgt.T @ alt))
            neu[ab] = _rx(innen * p['arm_innen'] * w) @ (R1[8].T @ W)
        struct.pack_into('<h', emr, o + 2, int(max(-32768, min(32767, round(wurzel1[1])))))
        for b in (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13):
            for i, wi in enumerate(_zerlege(neu[b])):
                _bits_schreiben(emr, o + 12, b * 36 + i * 12, wi)
    return bytes(emr)


def _tiefster_punkt(R, T, verts):
    """Groesstes Y (Y zeigt nach unten) ueber alle Mesh-Vertices = Bodenkontakt."""
    return max(float((v @ R[b].T + T[b])[:, 1].max()) for b, v in verts.items())


def umhalten(vor, d, md1, haltung):
    """Alle Keyframes aller drei Baenke (EMR dir[2]/[4]/[6]) umhalten. Arbeitet auf den
    bereits angepassten Bindoffsets und der skalierten MD1, damit der Bodenkontakt zur
    fertigen Geometrie passt."""
    struktur = EA.emr_parse(bytes(vor[d[2]:d[3]]))
    rel = struktur.rel
    ks  = struktur.kf_size
    verts = {}
    for b, m in enumerate(EA.md1_parse(md1)[:struktur.bone_count]):
        if m.tv:
            verts[b] = np.array([v[:3] for v in m.tv], float)
    gang = _gang_plan(vor, d, struktur, haltung.get('gang'))
    for a, e in ((d[2], d[3]), (d[4], d[5]), (d[6], d[7])):
        kf_off = u16(vor, a + 2)
        n = (e - a - kf_off) // ks
        for k in range(n):
            o = a + kf_off + k*ks
            sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
            winkel = [EA.kf_angles(sk, 0, b) for b in range(struktur.bone_count)]
            wurzel = list(EA.kf_pos(sk, 0))
            lokal  = [EA.mat_euler(*w).astype(float) / 4096 for w in winkel]
            R0, T0 = _welt(struktur, rel, lokal, wurzel)
            # Anteil der Haltung = Aufrichtung des Rumpfs im Original (Y-Achse von Knochen 7
            # gegen die Senkrechte): aufrecht -> voll, liegend (Niederschlag/Tod) -> nichts,
            # sonst knickt ein schon liegender Rumpf um weitere 80 Grad durch.
            f = float(np.clip(R0[7][1, 1], 0.0, 1.0))
            neu = list(lokal)
            for b, g in haltung['kippen'].items():
                neu[b] = _rz(g * f) @ lokal[b]
            # Arme: Weltorientierung wie vorher -> haengen senkrecht vom gekippten Rumpf
            R1, _ = _welt(struktur, rel, neu, wurzel)
            for b in haltung['welt_halten']:
                p = struktur.parent[b]
                neu[b] = R1[p].T @ R0[p] @ lokal[b]
            for b, g in haltung['nachkippen'].items():
                neu[b] = _rz(g * f) @ neu[b]
            geaendert = set(haltung['kippen']) | set(haltung['welt_halten']) | set(haltung['nachkippen'])
            if a == d[2] and k in gang:
                # Vorderbein-Schritt: Zusatzdrehung um die Welt-Seitenachse Z (Laufrichtung +X),
                # danach Ellbogen-Knick in der Vorschwung-Phase.
                for arm, (zusatz, knick) in gang[k].items():
                    R1, _ = _welt(struktur, rel, neu, wurzel)
                    p = struktur.parent[arm]
                    neu[arm] = R1[p].T @ _rz(zusatz) @ R1[arm]
                    ell = haltung['gang']['arme'][arm]
                    neu[ell] = _rz(knick) @ neu[ell]
                    geaendert |= {arm, ell}
            R1, T1 = _welt(struktur, rel, neu, wurzel)
            # Bodenkontakt: im Lauf an den Hinterfuessen (sonst hoebe eine zurueckschwingende
            # Klaue den ganzen Koerper an), sonst am tiefsten Punkt des Modells.
            boden = verts
            if a == d[2] and k in gang:
                boden = {b: verts[b] for b in haltung['gang']['fuesse'] if b in verts}
            wurzel[1] += int(round(_tiefster_punkt(R0, T0, boden) - _tiefster_punkt(R1, T1, boden)))
            struct.pack_into('<h', vor, o + 2, max(-32768, min(32767, wurzel[1])))
            for b in geaendert:
                for i, w in enumerate(_zerlege(neu[b])):
                    _bits_schreiben(vor, o + 12, b*36 + i*12, w)


def _gang_plan(vor, d, struktur, gang):
    """Je Keyframe der Laufclips (Bank 0) und je Vorderbein: (Zusatzwinkel, Ellbogenknick) in
    Grad. Schwung p = Neigung der Original-Armachse in der Laufebene (atan2(x, y), Y = unten);
    der Zusatz verstaerkt die Abweichung vom Clip-Mittel, der Knick folgt der Schwung-
    Geschwindigkeit (nur vorwaerts, normiert auf das Clip-Maximum)."""
    if not gang:
        return {}
    clips, frames = EA.edd_parse(bytes(vor[d[1]:d[2]]))
    a = d[2]
    kf_off, ks = u16(vor, a + 2), struktur.kf_size
    plan = {}
    for c in gang['clips']:
        if c >= len(clips):
            continue
        f0, n = clips[c]
        kfs = [frames[f0 + i] & 0xFFF for i in range(n)]
        for arm in gang['arme']:
            pw = []
            for kf in kfs:
                o = a + kf_off + kf*ks
                sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
                lokal = [EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096
                         for b in range(struktur.bone_count)]
                R, _ = _welt(struktur, struktur.rel, lokal, EA.kf_pos(sk, 0))
                v = R[arm] @ np.array([0.0, 1.0, 0.0])
                pw.append(np.degrees(np.arctan2(v[0], v[1])))
            pw = np.array(pw)
            mitte = pw.mean()
            ges = np.roll(pw, -1) - np.roll(pw, 1)
            gmax = max(ges.max(), 1e-6)
            for i, kf in enumerate(kfs):
                zusatz = -(gang['faktor'] - 1.0) * (pw[i] - mitte)
                knick = gang['anheben'] * max(0.0, ges[i]) / gmax
                plan.setdefault(kf, {})[arm] = (zusatz, knick)
    return plan


def lade_em30():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    return ems[off:off+ln]


def baue(blob, form):
    """Blob = [u32 dir_off][u32 n=9][Sektionen dir[0..8]][Verzeichnis n*4]. MD1 waechst durch den Schwanz,
    mit Schwanzkette auch die EMR-Bank 0 (dir[2]); alle folgenden Sektionen verschieben sich, das Verzeichnis
    wird aus den Sektionslaengen neu gerechnet."""
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n*4 == len(blob) and d[0] == 8 and n == 9
    # Verzeichnis EM30: dir[2]=EMR Bank 0 (Struktur), dir[7]=MD1, dir[8]=TIM
    emr_a, emr_e = d[2], d[3]
    md1_a, md1_e = d[7], d[8]
    vor = bytearray(blob[:md1_a])
    vor[emr_a:emr_e] = passe_emr_an(blob[emr_a:emr_e], form['skal'], form['rel'])
    md1 = skaliere_md1(blob[md1_a:md1_e], form['skal'])
    if form.get('haltung'):
        umhalten(vor, d, md1, form['haltung'])
    emr0 = bytes(vor[emr_a:emr_e])
    if form.get('ducken'):
        emr0 = ducken(emr0, form['ducken'])
    kette = form.get('schwanz_kette')
    if kette and form.get('ducken'):
        # Runde 41 SF N2: die Ruhegestalt des Schwanzes haengt im Brustraum; Rumpf+Brust kippen um (rumpf+brust) Grad nach
        # vorn -> Ruhelinie um denselben Winkel zurueckdrehen (um den Ansatz), damit er in der Welt haengt wie vorher.
        g = -(form['ducken']['rumpf'] + form['ducken']['brust'])
        p0 = np.array(kette['punkte'][0], float)
        kette = dict(kette, punkte=[tuple(p0 + _rz(g) @ (np.array(q, float) - p0)) for q in kette['punkte']])
    if form.get('schwanz') or form.get('umtexturieren') or kette:
        meshes = md1_lesen(md1)
        if form.get('teilen'):
            teile_quer(meshes, form['teilen'])
        if form.get('umtexturieren'):
            umtexturieren(meshes, form['umtexturieren'])
        if form.get('schwanz'):
            haenge_schwanz_an(meshes, form['schwanz'])
        if kette:
            # Mesh 16 (Hand mit Rohr, ohne Knochen) entfaellt: Mesh 16.. = Glieder der Kette (Mesh i == Knochen i).
            # Der Port tauscht Teil 15 nur bei genau 16 Knochen gegen Mesh 16 (main.c bk_rohr).
            ruhe = kette_ruhe(kette)
            meshes = meshes[:EA.emr_parse(emr0).bone_count] + kette_meshes(kette, ruhe)
            emr0 = emr_mit_kette(emr0, kette, ruhe, schwanz_bewegung(emr0, blob[d[1]:d[2]], kette, ruhe))
        md1 = md1_schreiben(meshes)
    tim = faerbe_tim(blob[md1_e:dir_off], form['tint'])
    if form.get('cluts'):
        tim = cluts_bauen(tim, form['cluts'])
    if len(tim) % 4:
        tim += bytes(4 - len(tim) % 4)
    sek = [bytes(vor[d[i]:d[i+1]]) for i in range(7)]
    sek[2] = emr0
    sek += [md1, tim]
    neu_dir, o = [], 8
    for x in sek:
        assert len(x) % 4 == 0
        neu_dir.append(o)
        o += len(x)
    neu = b''.join(sek)
    kopf = struct.pack('<II', 8 + len(neu), n)
    return kopf + neu + struct.pack('<%dI' % n, *neu_dir)


def modell_aus_blob(blob):
    """EA.Modell ohne EMS-Zugriff aus einem fertigen Blob aufbauen (fuer das Ansichtsblatt)."""
    mo = EA.Modell.__new__(EA.Modell)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = blob, TYP, EA.EMS_ORDER.index(TYP), len(blob)
    d = EA.emd_dir(blob)
    mo.meshes            = EA.md1_parse(blob[d[7]:d[8]])
    mo.skel              = EA.emr_parse(blob[d[2]:d[3]])
    mo.clips, mo.frames  = EA.edd_parse(blob[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(blob[d[8]:])
    mo.texh, mo.texw     = mo.tex.shape
    return mo


# ═══════════════════════════════════════════════════ Roter Affe (ROOM4080/4081)
# Video 1 4:11-4:19 (farbtreu): der Affe in 4080 ist tief dunkelrot; gemessen an den roten Bildpunkten
# r/g ~ 2.1 gegen r/g ~ 1.7 der EM29-Textur, deren Fell braun-grau ist (r/g ~ 1.2). Gestaltung: das ganze
# Fell nach dunklem Weinrot ziehen (Zielwert r/g ~ 2.5-3, b ~ g), deutlich abdunkeln. Nur die CLUTs aendern sich.
ROT_AFFE = dict(typ=0x29, datei='EM29_ROT.EMD',
                tint=(np.array([[0.70, 0.40, 0.20],
                                [0.08, 0.20, 0.08],
                                [0.08, 0.14, 0.22]]), 0.62))


def baue_rot_affe():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(ROT_AFFE['typ']))
    blob = bytearray(ems[off:off+ln])
    d = EA.emd_dir(bytes(blob))
    dir_off = u32(blob, 0)
    tim_a = d[-1]                                   # letzte Sektion = TIM (dir[8])
    blob[tim_a:dir_off] = faerbe_tim(bytes(blob[tim_a:dir_off]), ROT_AFFE['tint'])
    return bytes(blob)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--blatt', help='Verzeichnis fuer Ansichtsblaetter (PNG)')
    ap.add_argument('--clip', type=lambda s: int(s, 0), default=0)
    ap.add_argument('--aus', default=AUS_DIR, help='Zielverzeichnis der EMDs')
    a = ap.parse_args()

    affe = baue_rot_affe()
    ziel = os.path.join(a.aus, ROT_AFFE['datei'])
    os.makedirs(a.aus, exist_ok=True)
    with open(ziel, 'wb') as f:
        f.write(affe)
    print('geschrieben: %s  (%d Byte, roter Affe)' % (ziel, len(affe)))
    if a.blatt:
        mo = EA.Modell.__new__(EA.Modell)
        mo.blob, mo.typ, mo.blob_index, mo.blob_len = affe, 0x29, EA.EMS_ORDER.index(0x29), len(affe)
        dd = EA.emd_dir(affe)
        mo.meshes = EA.md1_parse(affe[dd[7]:dd[8]]); mo.skel = EA.emr_parse(affe[dd[2]:dd[3]])
        mo.clips, mo.frames = EA.edd_parse(affe[dd[1]:dd[2]])
        mo.tex, mo.pal, mo.clut_y = EA.tim_decode(affe[dd[8]:]); mo.texh, mo.texw = mo.tex.shape
        mo.titel = 'Roter Affe ROOM4080 - abgeleitet von EM29 (Typ 0x29)'
        mo.untertitel = 'NACHBILDUNG nach Video 1 4:11-4:19 - EM29_ROT.EMD - Basis CDEMD0.EMS Blob %d'
        mo.hinweis = 'NICHT byte-true: nur die CLUTs sind umgefaerbt (birkin_formen.py ROT_AFFE). Alles andere = EM29.'
        os.makedirs(a.blatt, exist_ok=True)
        EA.blatt(mo, 0, os.path.join(a.blatt, 'affe_rot.png'))
    basis = lade_em30()
    d = EA.emd_dir(basis)
    roh = basis[d[7]:d[8]]
    assert md1_schreiben(md1_lesen(roh)) == roh, 'MD1-Serializer nicht byte-gleich zu EM30'
    os.makedirs(a.aus, exist_ok=True)
    for name, form in FORMEN.items():
        blob = baue(basis, form)
        ziel = os.path.join(a.aus, form['datei'])
        with open(ziel, 'wb') as f:
            f.write(blob)
        print('geschrieben: %s  (%d Byte, Form %s)' % (ziel, len(blob), name))
        if a.blatt:
            os.makedirs(a.blatt, exist_ok=True)
            mo = modell_aus_blob(blob)
            mo.titel      = '%s - abgeleitet von EM30 (Typ 0x30)' % form['titel']
            mo.untertitel = 'NACHBILDUNG nach Beta-Screenshots - ' + form['datei'] + ' - Basis CDEMD0.EMS Blob %d'
            mo.hinweis    = ('NICHT byte-true: Vertex-/Normalen-Skalierung, Bindoffsets und Farben sind Gestaltung '
                             '(birkin_formen.py FORMEN)' + (', Schwanz angehaengt an Mesh 8' if form.get('schwanz') else '')
                             + (', Keyframes umgehalten (Vierbein)' if form.get('haltung') else '')
                             + '. Unveraendert aus EM30: Topologie, UVs, Skelett-Hierarchie, Clip-Tabelle.')
            EA.blatt(mo, a.clip,
                     os.path.join(a.blatt, 'birkin_%s.png' % name))


if __name__ == '__main__':
    main()
