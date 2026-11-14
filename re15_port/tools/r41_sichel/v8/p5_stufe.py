"""SF V8 Runde 7c, Stufe p (Kette v8_bau.py --stufen ...bp): Luecke Ruecken/Kopf ohne Kragen (P5) = Kandidat R3 des P5-Ablaufs.

    python p5_stufe.py <eingang.emd> <ausgang.emd> [--r3]       (--r3: ohne die Pflicht-Korrekturen der Runde 7c = Modell R3)

HERKUNFT (Runde 41, Spur SF, Sonderlauf P5, drei Runden mit je zwei unabhaengigen Pruefern, Ordner
  P = C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5):
  - Code der Schritte 1-6 = P/kand_r3/stufe_p5.py (Kandidat R3, Sieger; Modell P/kand_r3/EM30_HANGAR_R3.EMD sha256 9860f6bd78bf...,
    Eingang V8 r6 e9e71bca), unveraendert bis auf den Werkzeugpfad (_laden: Werkzeuge dieses Baums statt der Bank-Kopie) und die
    Pflicht-Korrekturen K-a..K-e (Schritt 7, Runde 7c, Messungen in sfv8/r7c/NOTIZ.md).
  - Arbeitsnotiz P/kand_r3/NOTIZ.md; Pruefberichte P/pruef_zahlen-r3/BERICHT.md, P/pruef_sicht-r3/BERICHT.md (und -r2).
  - Messbank P/bank/bank_p5.py.
  Entscheide des Orchestrators (2026-11-13) E9-E13: Vorn-Soll als Klasse gegen V7, Ecken 17/30/46 frei (E10), Ergaenzungsdreiecke
  zaehlen als vorhandene Flaechen (E11).

Original-Kopf des R3-Werkzeugs:
SF V8 P5, Kandidat R3 (Runde 41, Spur SF, Sonderlauf P5, Nachbesserung Runde 3; Ausgang = Stufe R2 kand_r2/stufe_p5.py): Luecke Ruecken/Kopf ohne Kragen.

    python stufe_p5.py <eingang.emd> <ausgang.emd>

Aendert AUSSCHLIESSLICH Mesh 8 (Rumpf mit Ruecken und Buckel). Mesh 9 (Kopf) bleibt bytegleich (V7-Kopf, nichts angehaengt; Auftrag E6
erlaubt Anhaenge, R2 braucht keinen: Kandidat A's Kopfanhang stand laut Zahlen-Pruefer in F333-F344 als Spitze ueber dem Kopf).
Knochen, Keyframes, Clips (Abschnitte 0-6), Texturbild und CLUT schreibt KB.packen unveraendert zurueck; alle anderen Meshes werden nicht
gelesen und nicht veraendert (die Stufe stuetzt sich nicht auf sie). Deterministisch: keine Zufallszahlen, keine Zeit, Rundung fest.
Eingang: V8 r6 (e9e71bca...) oder ein spaeterer Stand der Kette, dessen Mesh 8 noch so aussieht - die Stufe prueft jede Ecke und Flaeche,
die sie anfasst, gegen den erwarteten Vorher-Wert und bricht sonst ab.

Kein RE-Befund: die Sichel-Form (zweibeiniger Birkin) gibt es auf keiner Disc. Jede Zahl ist PORT-WAHL mit Messung (Herkunft je Konstante;
Messungen in P/kand_r2/NOTIZ.md, P = C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/
scratchpad/sfv8/p5). Koordinaten: Knochenraum 8, Modelleinheiten, PSX (y < 0 = oben, x < 0 = hinten; Kopfgelenk (111, -971, -133)).

Schritte:
 1. Nackenstueck der V8-Runde 6 entfernen (Ecken/Normalen 69-75, Dreiecke 77-86). Auftrag E8 (nicht geschuetzt); Pruefer r6: "Zelt".
 2. r5d-Restecken 6 und 43 zurueck auf V8 r4 (S18 a1 des Eingangs: Ecke 6 in 76, Ecke 43 in 3 Ansichten; kand_b/kand_a gemessen).
 3. Ecken 30 und 46 verschieben (Luecke an der Nutzerstelle PV/P2 zu; Spitze der Griff-Platte hinter den Kopf). Ecke 0 bleibt r4
    (Kandidat B verschob sie: von vorn Kante + oranger Fleck ueber dem Kopf, Loch PV+1280, Buckel nur ueber UV-Mitfuehrung).
 4. UV der verschobenen Ecken mitfuehren (Haut bleibt mit ihren Original-Texeln am Ort; Zeilen unveraendert).
 5. Viereck v8 (Ecken 30, 0, 50, 49) ERGAENZEN: seine Haelften (0,1,3) und (0,3,2) als Dreiecke d77/d78 anhaengen (gleiche Ecken, Normalen,
    Texel, Zeile). Grund: der Port verwirft ein Viereck nach dem Testdreieck (e0,e1,e2) (engine/src/nclip_r38.c, f_rast.dreiecke Z. 47-50);
    v8 ist seit r4 verdreht und fehlt dadurch in 75-79 von 177 Ansichten (kand_r2/w/d8_cull.py; V7: 0) -> Falte/V-Kerbe F262-F267,
    PV+1024, P2+1024 (kand_a T8). Wo v8 gezeichnet wird, liegen die Haelften deckungsgleich darunter.
"""
import copy
import hashlib
import os
import sys

# R3 lud die Werkzeuge aus der Bank-Kopie (Worktree HEAD 3d2d230f5); in der Kette die Werkzeuge dieses Baums (kopf_orig_bau.py dort
# unveraendert seit 3d2d230f5, geprueft Runde 7c)
WERKZEUG = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))      # .../re15_port/tools

MESH_RUMPF, MESH_KOPF = 8, 9                       # Auftrag: Rumpf = Mesh 8, Kopf = Mesh 9
V7_KOPF = dict(v=61, n=61, tf=73, qf=18)           # Mesh 9 V7 (staende/V7.EMD 319bbd17): kand_a/d_ecken.py, Zahlen-Pruefer struktur_A.txt

# 1. Nackenstueck r6 (Eingang e9e71bca, Mesh 8): 76 Ecken / 76 Normalen / 87 Dreiecke / 27 Vierecke; Stueck = Ecken 69-75, Dreiecke 77-86.
#    Werte: kand_r2/w/d1_r4_r6.py (r4 69/77/27, r6 76/87/27), erk/ERKUNDUNG_P5.md M3.
RUMPF_EINGANG = dict(v=76, n=76, tf=87, qf=27)
NACKEN_R6_ECKEN = [(-150, -1180, -300), (-70, -1185, -370), (30, -1210, -250), (60, -1250, -230), (-120, -1330, -325), (-30, -1340, -355),
                   (-180, -1270, -340)]
NACKEN_R6_ERSTE_ECKE, NACKEN_R6_ERSTES_DREIECK = 69, 77

# 2. r5d-Restecken -> V8 r4 (staende/R4.EMD 10d02173 Mesh 8). (Eingang, Ziel). Herkunft: kand_b NOTIZ 1 (a4_s2c.py: S2c bleibt 2 px),
#    Bank D0 S18 a1 Ecke 6 (76 Ans.) / 43 (3 Ans.); kand_r2 probe2 (s5.py): a1 der Eingangs-Ecken danach 0.
R4_ZURUECK = {6: ((-466, -869, 343), (-506, -869, 373)),
              43: ((-1022, -386, 430), (-1022, -386, 530))}

# 3. Ecken (Eingang = r4, Ziel). Keine hoeher als KRAGEN_Y (bank_p5.KRAGEN_Y = -1270: K-VORN (a)).
ECKEN = {
    # 30 = Huegelspitze am Nacken (P2-Seite, r4 (-122,-1265,85)); zugleich Spitze der hellen Platte rechts neben dem Kopf in der Griffphase
    # (kand_r2/w/d3_platte.py: F278/F310/F311 Spitze an Ecke 30). x -60 (62 E. zum Kopf vor) schliesst mit 46 den PV-Schlitz (kand_b scan9:
    # PV 11/0/1,4). z 40 (45 E. zur Kopfmitte): in PV/P2 ist z die Blicktiefe (Huegel bleibt), in der Griffphase waagrecht -> Spitze rueckt
    # hinter die Kopf-Oberkante. Gemessen kand_r2/w/s5.py (E1 z 85 / E2 z 40, Variante 0): F278 163 -> 147 px, F311 182 -> 155, PV 11 -> 7,
    # Buckel 59,0 -> 54,2 (>= 50). Bank Versuch 1 (z 40): v15 malt 4 px ueber die Gesichtsmaske (F264/F265/F291/F292/F296/F297, d10_verd.py);
    # s7.py: z 25 -> Gesicht frei (nur Eingangs-Bilder F253/PV+1024), PV 7/0/1,0, P2 21/1/2,0, Buckel 54,9 (mit v8-Ergaenzung).
    # x weiter hinten (-64..-90) haelt das Gesicht nur bei x <= -75 frei und oeffnet P2 (27/5/2,2; s6/s7).
    # R3 (Auftrag Runde 3 (1), E10: 30 frei): R2-Lage (-60,-1268,25) -> (-30,-1268,-35). Messung kand_r3/NOTIZ.md Abschnitt 1 (p3.py, kurz.sh):
    # z nach -z zieht die Plattenecke in F310 an den Kopf (echt: V in F310 zu, d/E_nah.png) und senkt die F311-Spitze (echt E: 217 -> 185);
    # x +30 haelt P2 im Soll (E (-60,..,-50): P2 34/1/2,0; diese Lage 21/1/2,0); x weiter nach vorn (A/C/K/N) oeffnet F332-F340 (echt bis 171).
    30: ((-122, -1265, 85), (-30, -1268, -35)),
    # 46 = Gratpunkt links hinter dem Kopf (r4 (-124,-1233,-305)); Ziel = kand_b (Vorversuch D mit Hoehe auf -1268 gekappt). Ohne 46 bleibt der
    # PV-Schlitz offen (kand_r2 s5.py E6: PV 14/7/16,3; kand_b e9: 13/7/16,1).
    46: ((-124, -1233, -305), (-104, -1268, -280)),
    # R3: 17 = Nachbarecke von 30 an der Seitenkante der Platte (v15 [25,23,17,30] / v17 [51,17,50,30]), r4 (-29,-1008,283). Die F311-Spitze
    # ist die v17-Ecke 30 ueber der Kante 17-30, links davon Hintergrund (Flaechen-ID-Lauf d/fid_R2.png). 17 hoeher und naeher (z 240)
    # macht die Kante flach -> echter Kurzlauf S3 (lauf/k_S3): F311 184 -> 91/62/8,5 (V7 115), F310 108 -> 71, vorn 23/1381 (V7 28/1796);
    # S1 (17 nur auf y -1100) F311 91, F310 108. d/S_311.png angesehen: keine Spitze, kein V.
    17: ((-29, -1008, 283), (-29, -1160, 240)),
}
KRAGEN_Y = -1270

# 4b. UV-Lage: UV der Flaechen an Ecke i werden an dieser Lage (statt an der neuen Geometrie) mitgefuehrt (befuellt unten)
UV_LAGE = {30: (-60, -1268, 25)}   # = R2-Lage von 30: Buckel-Rot PV 50,0 statt 46,5 bei Auswertung an der neuen Lage (kand_r3 uvs.py/p3.py E4/E4u)

# 6. UV-Rueckbau auf r4: Flaeche -> (Eingangs-UV, r4-UV) (befuellt unten)
# Herkunft: staende/R4.EMD (10d02173) Mesh 8, gleiche Ecken und gleiche Zeile 493 (31552 = Zeile 493 << 6); r5/r6 hatten die UV verschoben
# -> dunkelroter Dreiecksfleck links neben dem Gesicht F346-F378 (Auftrag Runde 3 (4); kand_r2 D4/D5, kand_r3 d4_fleck.py).
UV_R4 = {
    'd66': ((35, 46, 31552, 50, 59, 128, 53, 49, 0), (37, 39, 31552, 53, 53, 128, 56, 42, 0)),
    'v4': ((10, 44, 31552, 25, 46, 128, 9, 31, 0, 23, 32, 0), (37, 39, 31552, 56, 42, 128, 36, 23, 0, 54, 24, 0)),
    'v24': ((46, 59, 31552, 43, 37, 128, 31, 53, 0, 29, 37, 0), (58, 59, 31552, 54, 35, 128, 41, 53, 0, 39, 35, 0)),
    'd26': ((20, 19, 31552, 17, 30, 128, 37, 25, 0), (56, 42, 31552, 53, 53, 128, 73, 48, 0)),
}

# 5. Viereck-Ergaenzung: Index -> erwartete Ecken (sonst Abbruch).
ERGAENZEN = {8: (30, 0, 50, 49),
             # R3: v17 [51,17,50,30] wird durch die neue Lage von 17 ebenso verdreht wie v8 (kand_r3 s17.py: Test-Dreieck (51,17,50) verworfen,
             # obwohl eine Haelfte zur Kamera zeigt, in 22..69 von 177 Ansichten je nach Lage von 17; Folge PV+256 Spalt 20/20/8,0, d/d6_pv256.png).
             # Ergaenzung wie v8 (E11): Haelften als d79/d80, gleiche Ecken/Normalen/Texel/Zeile; danach PV+256 0 (s16v.py V2h).
             17: (51, 17, 50, 30)}
HAELFTEN = None     # None = beide Haelften (0,1,3),(0,3,2) (befuellt unten)


def _laden():
    for p in (WERKZEUG, WERKZEUG + '/r41_sichel/v6kopf', WERKZEUG + '/r41_sichel/v4', WERKZEUG + '/r41_sichel/v2'):
        if p not in sys.path:
            sys.path.insert(0, p)
    import kopf_orig_bau as KB          # noqa: E402  (teile/packen = birkin_formen.md1_lesen/md1_schreiben)
    return KB


def _basis(P):
    import numpy as np
    c = P.mean(0); a = P[1] - P[0]; n = np.cross(P[1] - P[0], P[2] - P[0])
    ex = a / np.linalg.norm(a); ey = np.cross(n / np.linalg.norm(n), ex)
    return c, ex, ey


def uv_mitfuehren(m_alt, m_neu, verschoben):
    """Je Flaeche mit verschobener Ecke: affine Abbildung (Eingangs-3D in der Flaechenebene -> Eingangs-UV) an der neuen Lage auswerten
    (Code = kand_b/stufe_p5.uv_mitfuehren). Nur die UV der verschobenen Ecken aendert sich; CLUT-/Seitenfeld bleibt."""
    import numpy as np
    for F, U in ((m_neu['tf'], m_neu['tuv']), (m_neu['qf'], m_neu['quv'])):
        for i, f in enumerate(F):
            e = list(f[1::2])
            if not (set(e) & set(verschoben)):
                continue
            P0 = np.array([m_alt['v'][j][:3] for j in e], float); P1 = np.array([m_neu['v'][j][:3] for j in e], float)
            uv = U[i]; n = len(e)
            UV = np.array([[uv[3 * k], uv[3 * k + 1]] for k in range(n)], float)
            c, ex, ey = _basis(P0)
            M, *_ = np.linalg.lstsq(np.stack([(P0 - c) @ ex, (P0 - c) @ ey, np.ones(n)], 1), UV, rcond=None)
            UVn = np.stack([(P1 - c) @ ex, (P1 - c) @ ey, np.ones(n)], 1) @ M
            neu = list(uv)
            for k in range(n):
                if e[k] in verschoben:
                    neu[3 * k] = int(np.clip(round(float(UVn[k, 0])), 0, 255)); neu[3 * k + 1] = int(np.clip(round(float(UVn[k, 1])), 0, 255))
            U[i] = neu


def ergaenzen(m, qi, ecken, haelften=((0, 1, 3), (0, 3, 2))):
    q, uv = m['qf'][qi], m['quv'][qi]
    assert tuple(q[1::2]) == tuple(ecken), ('Viereck v%d' % qi, q[1::2], ecken)
    c = [(q[0], q[1]), (q[2], q[3]), (q[4], q[5]), (q[6], q[7])]
    t = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])]
    for a, b, cc in haelften:
        m['tf'].append([c[a][0], c[a][1], c[b][0], c[b][1], c[cc][0], c[cc][1]])
        m['tuv'].append([t[a][0], t[a][1], uv[2], t[b][0], t[b][1], uv[5], t[cc][0], t[cc][1], 0])


# 7. Runde 7c: Pflicht-Korrekturen K-a..K-e (Nebenwirkungen von R3; beide Pruefer R3). Messungen: sfv8/r7c/NOTIZ.md (Abschnitt 2),
#    Pruefer-Code-Kopie sfv8/r7c/pz (flaechen.py, t8such.py, gesicht_alle.py), Bank P/bank/bank_p5.py unveraendert, Szenenlauf r7c/lauf.
#    Keine Disc-Konstante (die Form gibt es auf keiner Disc): PORT-WAHL je Zeile mit Messung.
# 7a. UV (nur Textur-Koordinaten; Zeilen, Texturbild und CLUT unveraendert): Flaeche -> [(Ecke im Umlauf, R3-UV, neue UV)]
KORR_UV = {
    # K-a: t60 [17,60,25] las bei R3 an Ecke 17 (57,0) = oberer Texturrand v 0-4 (35 Texel auf Zeile 494, die kein frueherer Stand
    #      benutzt, 3 davon orange: Pruefer-Zahlen r3 texel_herkunft.txt; oranger Fleck in 130 von 226 Szenenbildern). Zurueck auf die
    #      R2/r6-UV (61,6): danach 0 Texel auf Zeile 494 ausserhalb r6 (r7c/pz/flaechen.py KA1), t60 Dichte 0,98-1,01, Streckung 1,41-1,47.
    't60': [(0, (57, 0), (61, 6))],
    # K-a: q15 [25,23,17,30] Ecke 17 R3 (74,11) -> R2/r6 (77,15) (2 der 35 fremden Texel); q15.1 danach 1,05-1,07-fach.
    'q15': [(2, (74, 11), (77, 15))],
    # K-b: t8 [60,17,4] R3 (29,38) = 0,47-fach (Pruefer flaechen.py) / 0,49 (Bank S19) -> (23,41): 0,53-fach, Streckung 1,80 an der
    #      Endgeometrie (r7c/pz/t8such.py / t8such2.py: Dichte >= 0,5 (Auftrag E4), nur Texel, die R6/R4/V7/R2/Stand 7b auf Zeile 493 schon
    #      benutzen). Erste Wahl (25,36) gab 112 fast weisse Bildpunkte von vorn (riegel_v8 S14 b, r7c/w/weiss.py; R3 (29,38): 1);
    #      (23,41): 0 (die Nachbarn (24..28, 35..38) 58-140).
    't8': [(1, (29, 38), (23, 41))],
    # K-a / Riegel S7 (a): q2 [16,46,23,0] las seit der UV-Mitfuehrung der Ecke 46 (P5 R2/R3) mit (42,0) 3 Texel auf Zeile 494, die weder
    #      eine Original-EM30-Flaeche (S7 a "texel_ohne_original_flaeche" 3 -> ROT) noch Eingang/r4/V7 benutzen. Ecke 46 zurueck auf r6 (41,0)
    #      allein gaebe Streckung 2,13 (> 2, E4); dazu Ecke 16 (37,2) -> (38,4): S7 (a) 0, fremde Texel 0, Streckung q2 1,61 / 1,04,
    #      Dichte 1,10 / 1,21 (r7c/w/q2such.py, fremd.py; (38,3) 1,90, (37,3)/(37,4)/(36,x) mit fremden Texeln).
    'q2': [(1, (42, 0), (41, 0)), (0, (37, 2), (38, 4))],
}
# 7b. K-c/K-d (Geometrie, so wenig wie moeglich; E10: 17 frei). Messungen r7c/NOTIZ.md Abschnitt 2:
#     Versuch 1 (verworfen): 17 nur tiefer (-29,-1140,240) loeste S9 F344 (3 -> 2 px) und P2+2048 offline, brachte aber in den echten
#       Bildern F274-F282 / F306-F309 eine helle Zacke mit V-Kerbe rechts neben dem Kopf in Folge zurueck (r7c/pz/zacke2.py) und liess
#       den Gesichts-Bildpunkt F313 stehen.
#     Versuch 2: 17 (-29,-1160,240) -> (-10,-1160,198): +x senkt 17 in F344 um 0,58 px je 10 E. und schiebt ihn in der Griffphase
#       (F276/F308) 0,49 px je 10 E. zum Kopf, -z schiebt ihn in P2+2048 0,52 px je 10 E. vom Gesicht weg (r7c/w/jac.py). Offline:
#       S9 F344 2 px, P2+2048 0 Gesichts-Bildpunkte (Variante 0); echte Griffphase unveraendert gegen R3 (S16s 2504 / 1553 wie R3B).
#     Der Gesichts-Bildpunkt F313 (621,281) liegt unter der Kante 30-25 von q15 (Flaechen-Kennlauf r7c/pz/fid17.py; 23 verschieben
#       wirkt nicht, 25 verschieben schon). 25 (220,-710,170) -> (216,-708,178): kleinste geprueft wirksame Lage, bei der die Streckung
#       des Nachbarn t36 [15,26,25] nicht steigt (r6/R3 2,96 -> 2,91; (215,-708,170) gab 3,02). 25 bewegt sich im Bild < 1 px (S18 a1 <= 2).
KORR_ECKEN = {17: ((-29, -1160, 240), (-10, -1160, 198)),
              25: ((220, -710, 170), (216, -708, 178))}


def korrektur(m):
    for key, liste in sorted(KORR_UV.items()):
        U = m['tuv'] if key[0] == 't' else m['quv']; i = int(key[1:])
        for k, alt, neu in liste:
            assert (U[i][3 * k], U[i][3 * k + 1]) == tuple(alt), ('UV', key, k, U[i], alt)
            U[i][3 * k], U[i][3 * k + 1] = neu
    for i, (alt, neu) in sorted(KORR_ECKEN.items()):
        assert tuple(m['v'][i][:3]) == tuple(alt), ('Ecke', i, m['v'][i], alt)
        assert neu[1] >= KRAGEN_Y
        m['v'][i][:3] = list(neu)


def stufe(blob, ecken=None, uv_r4=None, uv_lage=None, haelften=None, ergaenzen_vierecke=None, r3=False):
    KB = _laden()
    ecken = ECKEN if ecken is None else ecken
    d, _o, ms, tim = KB.teile(blob)
    m, k9 = ms[MESH_RUMPF], ms[MESH_KOPF]
    for k, n in V7_KOPF.items():                   # Mesh 9 wird nicht angefasst; nur pruefen, dass es der V7-Kopf ist
        assert len(k9[k]) == n, ('Mesh 9 nicht V7', k, len(k9[k]))
    for k, n in RUMPF_EINGANG.items():
        assert len(m[k]) == n, ('Mesh 8 nicht im Eingangszustand r6', k, len(m[k]))
    # 1.
    assert [tuple(v[:3]) for v in m['v'][NACKEN_R6_ERSTE_ECKE:]] == NACKEN_R6_ECKEN, 'Nackenstueck r6 nicht gefunden'
    m['v'] = m['v'][:NACKEN_R6_ERSTE_ECKE]; m['n'] = m['n'][:NACKEN_R6_ERSTE_ECKE]
    m['tf'] = m['tf'][:NACKEN_R6_ERSTES_DREIECK]; m['tuv'] = m['tuv'][:NACKEN_R6_ERSTES_DREIECK]
    for f in m['tf'] + m['qf']:
        assert max(f) < NACKEN_R6_ERSTE_ECKE, 'Flaeche verweist noch auf das Nackenstueck'
    # 2.
    for i, (alt, neu) in sorted(R4_ZURUECK.items()):
        assert tuple(m['v'][i][:3]) == alt, ('Ecke', i, m['v'][i])
        m['v'][i][:3] = list(neu)
    # 3. + 4.
    alt8 = copy.deepcopy(m)
    for i, (alt, neu) in sorted(ecken.items()):
        assert tuple(m['v'][i][:3]) == tuple(alt), ('Ecke', i, m['v'][i], alt)
        assert neu[1] >= KRAGEN_Y, ('Ecke', i, 'hoeher als die K-VORN-Grenze', neu)
        m['v'][i][:3] = [int(x) for x in neu]
    uvl = UV_LAGE if uv_lage is None else uv_lage
    for i, p in uvl.items():                       # UV an dieser Lage auswerten (Textur bleibt wie dort), Geometrie bleibt 'neu'
        m['v'][i][:3] = list(p)
    uv_mitfuehren(alt8, m, sorted(ecken))
    for i in uvl:
        m['v'][i][:3] = [int(x) for x in ecken[i][1]]
    # 5.
    for qi, e in sorted((ERGAENZEN if ergaenzen_vierecke is None else ergaenzen_vierecke).items()):
        ergaenzen(m, qi, e, **({'haelften': haelften} if haelften is not None else ({'haelften': HAELFTEN} if HAELFTEN else {})))
    # 6. UV einzelner Flaechen zurueck auf r4 (gleiche Zeile)
    for key, (alt, neu) in sorted((UV_R4 if uv_r4 is None else uv_r4).items()):
        U = m['tuv'] if key[0] == 'd' else m['quv']; i = int(key[1:])
        assert list(U[i]) == list(alt), ('UV', key, U[i], alt)
        U[i] = list(neu)
    # 7. (Runde 7c; r3=True liefert das R3-Modell zum Vergleich)
    if not r3:
        korrektur(m)
    return KB.packen(blob, d, ms, tim)


def main():
    ein, aus = sys.argv[1], sys.argv[2]
    neu = stufe(open(ein, 'rb').read(), r3='--r3' in sys.argv[3:])
    with open(aus, 'wb') as f:
        f.write(neu)
    print(aus, len(neu), hashlib.sha256(neu).hexdigest())


if __name__ == '__main__':
    main()
