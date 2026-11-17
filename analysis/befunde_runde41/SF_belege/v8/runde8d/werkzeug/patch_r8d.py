"""Patch Runde 8d (Nutzer 14.11. zum Endstand 8c: "ich weiss nicht wo du die Streifen bei den Schwanz Abschnitten von Birkin her nimmst, die
gehoeren da nicht hin ... der Schwanz wirkt auch ueber die Abschnitte hinweg einheitlich"): Farbe b ohne Ringe, Fugen mit dem Bauchfeld;
riegel_schwanz_r8.py R8d (3) "keine Ringe, Fugen mit Bauchfeld, nahtlos"."""
import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
S = None


def lade(n):
    global S
    S = open(os.path.join(W, n), encoding='utf-8').read()


def ers(alt, neu, n=1):
    global S
    assert S.count(alt) == n, (alt[:70], S.count(alt))
    S = S.replace(alt, neu)


# ---------------------------------------------------------------- schwanz_r8.py
lade('schwanz_r8.py')
ers("""FARBEN = {'a': dict(fleisch='gedeckt', band=False, ring=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, ring=True, name='gedeckt, weiche rosa Ringe an den Gliedfugen')}""",
    """FARBEN = {'a': dict(fleisch='gedeckt', band=False, ring=False, name='gedeckt, ohne Baender (seit Runde 8d gleich b)'),
          'b': dict(fleisch='gedeckt', band=False, ring=False, name='gedeckt, einheitlich ueber die Gliedfugen (Runde 8d)'),
          'ring': dict(fleisch='gedeckt', band=True, ring=True, name='Runde 8c: weiche Ringe an den Gliedfugen (verworfen, nur Gegenprobe)')}
# Runde 8d (Nutzer 14.11. zum Endstand 8c d31f9534, woertlich: "Im prinzip fast perfekt, aber - ich weiss nicht wo du die Streifen bei den
# Schwanz Abschnitten von Birkin her nimmst, die gehoeren da nicht hin. Schau doch auf das Original Video, der Schwanz wirkt auch ueber die
# Abschnitte hinweg einheitlich."): die Ringe entfallen. Die Fugenflaechen im Bauchsektor (BAUCH_J) bekommen dasselbe Bauchfeld C1 wie der
# uebrige Bauch (laengs je Glied gespiegelt -> an jedem Gelenk dieselbe Feldzeile auf beiden Seiten, nahtlos), die uebrigen Sektoren grau wie
# die Flanke daneben; Grenze Bauch/Flanke, Form, Einschnuerungen und Nadel wie 8c. Farbe b (Standard) = a.""")
ers("""  band  = Probe b: Ring umlaufend auf allen 11 Sektoren der Fugen (Mantel S-C / D-E und Deckel), Original-Zeile 480 (Asset 493), Seite 0,
          Feld RB u 93-100 v 10-17 mit Verlauf hell (u 100, am Gelenk) -> dunkel (u 93, am vollen Querschnitt), ohne Texel unter L* 18;""",
    """  band  = Runde 8c Probe b (seit Runde 8d verworfen, nur noch Farbe "ring" fuer die Gegenprobe): Ring umlaufend auf allen 11 Sektoren der
          Fugen, Original-Zeile 480 (Asset 493), Seite 0, Feld RB u 93-100 v 10-17, Verlauf hell (am Gelenk) -> dunkel;
  Runde 8d: keine Ringe - die Fugen tragen im Bauchsektor das Bauchfeld C1, sonst grau (Nutzer: "der Schwanz wirkt auch ueber die
          Abschnitte hinweg einheitlich");""")
ers("""  Proben (Koordinator: nur noch zwei, wenn sie sich sichtbar unterscheiden): a ohne Baender, b mit rosa Baendern (STANDARD; Runde 8c: Ringe).""",
    """  Proben (Koordinator: nur noch zwei, wenn sie sich sichtbar unterscheiden): a ohne Baender, b mit rosa Baendern (STANDARD; Runde 8c: Ringe;
  Runde 8d: b ohne Ringe = a).""")
open(os.path.join(W, 'schwanz_r8.py'), 'w', encoding='utf-8').write(S)

# ---------------------------------------------------------------- riegel_schwanz_r8.py
lade('riegel_schwanz_r8.py')
ers("""                  in den Gliedmitten daneben; (3) Farbprobe b (Runde 8c, Ring): die Band-Flaechen (Klasse aus dem Ring-Feld, schwanz_r8.klasse_uv)
                  sind GENAU die Fugen-Flaechen aller 11 Sektoren (Ringe S/C bzw. D/E mit Deckel-Mitte, aus den Ring-Lagen lage()) an allen 6
                  Gelenken, und in PV sind Band-Bildpunkte an >= 5 Gelenken sichtbar; Farbprobe a: keine Band-Flaeche.""",
    """                  in den Gliedmitten daneben; (3) Runde 8d (Farbe b = a, Nutzer "Streifen ... gehoeren da nicht hin"): KEINE Band-/Ring-Flaeche;
                  jede Fugen-Flaeche (Ringe S/C bzw. D/E mit Deckel-Mitte, aus den Ring-Lagen lage()) ist bauch (Bauchfeld) oder grau, an
                  allen 6 Gelenken auf beiden Seiten Fugen-Flaechen mit dem Bauchfeld; nahtlos: die Bauch-UV der Ecken des Gelenkrings liegt auf
                  beiden Seiten in derselben Feldzeile (Median-v gleich +-1 Texel); in PV Bauch-Bildpunkte auf Fugen-Flaechen an >= 5 Gelenken.
                  Farbe "ring" (Runde 8c, Gegenprobe): Band-Flaechen = genau die Fugen-Flaechen aller 11 Sektoren, in PV an >= 5 Gelenken.""")
ers("""    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(not abweich[b] and band[b] > 0 for b in range(16, 23)) and gel_sicht >= 5
    else:
        ok3 = all(band[b] == 0 for b in range(16, 23))
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, fugen_flaechen=fuge_n, band_ungleich_fuge=abweich, band_pv_px=band_pv,
                gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'], ok3=bool(ok3))""",
    """    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    # Runde 8d: Fugen mit dem Bauchfeld, nahtlos
    fuge_kl, fuge_bauch, fuge_bauch_pv, naht, fs_alle = {}, {}, {b: 0 for b in range(16, 23)}, {}, {}
    for b in range(16, 23):
        fs = fs_alle[b] = fugen_flaechen(b, ms[b])
        fuge_kl[b] = sorted({str(kl[b][k]) for k in fs})
        fuge_bauch[b] = sum(1 for k in fs if kl[b][k] == 'bauch')
    for y, x in zip(ys, xs):
        b = int(bo[y, x]); k = str(fk[y, x])
        if kl[b].get(k) == 'bauch' and k in fs_alle[b]:
            fuge_bauch_pv[b] += 1
    for j in range(1, 7):
        ve = gelenk_v(ms[15 + j], kl[15 + j], lage(15 + j)['E'])
        vs_ = gelenk_v(ms[16 + j], kl[16 + j], lage(16 + j)['S'])
        naht[j] = (ve, vs_, None if ve is None or vs_ is None else round(abs(ve - vs_), 1))
    gel_fuge_pv = sum(1 for j in range(1, 7) if fuge_bauch_pv[15 + j] + fuge_bauch_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(not abweich[b] and band[b] > 0 for b in range(16, 23)) and gel_sicht >= 5
    else:
        ok3 = (all(band[b] == 0 for b in range(16, 23)) and all(set(fuge_kl[b]) <= {'bauch', 'grau'} for b in range(16, 23))
               and all(fuge_bauch[15 + j] > 0 and fuge_bauch[16 + j] > 0 for j in range(1, 7))
               and all(n_[2] is not None and n_[2] <= 1.0 for n_ in naht.values()) and gel_fuge_pv >= 5)
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, fugen_flaechen=fuge_n, band_ungleich_fuge=abweich, band_pv_px=band_pv,
                gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'], fugen_klassen=fuge_kl, fugen_bauch=fuge_bauch,
                fugen_bauch_pv_px=fuge_bauch_pv, gelenke_mit_fugen_bauch_pv=gel_fuge_pv, naht_v=naht, ok3=bool(ok3))


def gelenk_v(m, kl_b, k0):
    \"\"\"Runde 8d: Median der Bauch-UV-v aller Ecken des Gelenkrings (Ring-Ecken k0 .. k0 + S_ - 1) in Bauch-Flaechen; None = keine.\"\"\"
    ring = set(range(k0, k0 + Q.S_))
    vs = []
    for key, F, U in (('d', m['tf'], m['tuv']), ('v', m['qf'], m['quv'])):
        for i, (f, uv) in enumerate(zip(F, U)):
            if kl_b.get('%s%d' % (key, i)) != 'bauch':
                continue
            for k, e in enumerate(f[0::2]):
                if e in ring:
                    vs.append(uv[3 * k + 1])
    return float(np.median(vs)) if vs else None""")
ers("""             '(>= 1,0) %s; (3) Ring (Probe %s, Runde 8c): Band-Flaechen je Glied %s, Fugen-Flaechen je Glied %s, Abweichung Band <> Fuge %s, '
             'PV-Bildpunkte %s, Gelenke mit Band in PV %d (>= 5) %s -> %s'
             % (d['geometrie'], g(d['ok1']), d['vorn']['mittel_je_ansicht'], g(d['vorn']['ok']), 'mit Ringen' if d['band'] else 'ohne Ringe',
                d['band_flaechen'], d.get('fugen_flaechen'), {k: v for k, v in d.get('band_ungleich_fuge', {}).items() if v}, d['band_pv_px'],
                d['gelenke_mit_band_pv'], g(d['ok3']), g(d['ok'])))""",
    """             '(>= 1,0) %s; (3) Fugen (Farbe %s): Band-Flaechen je Glied %s (Soll 0), Fugen-Flaechen je Glied %s, Klassen der Fugen %s, davon mit '
             'Bauchfeld %s, Naht je Gelenk (v Ende | v Anfang | Abstand, <= 1) %s, PV Bauch-Bildpunkte auf Fugen %s, Gelenke damit %d (>= 5) %s -> %s'
             % (d['geometrie'], g(d['ok1']), d['vorn']['mittel_je_ansicht'], g(d['vorn']['ok']), 'mit Ringen (Gegenprobe 8c)' if d['band'] else 'einheitlich',
                d['band_flaechen'], d.get('fugen_flaechen'), d.get('fugen_klassen'), d.get('fugen_bauch'), d.get('naht_v'),
                d.get('fugen_bauch_pv_px'), d.get('gelenke_mit_fugen_bauch_pv', 0), g(d['ok3']), g(d['ok'])))""")
open(os.path.join(W, 'riegel_schwanz_r8.py'), 'w', encoding='utf-8').write(S)
print('ok')
