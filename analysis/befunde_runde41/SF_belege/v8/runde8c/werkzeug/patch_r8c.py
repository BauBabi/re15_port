"""Patch Runde 8c (Koordinator 14.11.: Probe b heller / weniger rot nur durch AUSWAHL anderer Original-Texel; helle Stellen als weiche Ringe
umlaufend an den Gliedfugen): schwanz_r8.py - Fleischfeld mit eigener Zeile/Seite aus schwanz_r8.json, Ring-Modus fuer Probe b."""
import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()


def ers(alt, neu, n=1):
    global s
    assert s.count(alt) == n, (alt[:80], s.count(alt))
    s = s.replace(alt, neu)


# 1) FARBEN: b = Ringe umlaufend
ers("""FARBEN = {'a': dict(fleisch='gedeckt', band=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, name='gedeckt, rosa Baender')}""",
    """FARBEN = {'a': dict(fleisch='gedeckt', band=False, ring=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, ring=True, name='gedeckt, weiche rosa Ringe an den Gliedfugen')}
# Runde 8c (Koordinator 14.11., Nutzer waehlt b: "wobei das Original Video der Schwanz noch etwas heller ist und nicht ganz so rot"; Pruefer
# gruenblut: Port-Bauch Saettigung 0,50-0,55 gegen Video 0,38-0,41, Baender "2-4 kleine Flecken seitlich, keine weichen Ringe an den Gliedfugen"):
# - Bauch: ein anderes ORIGINAL-Feld (schwanz_r8.json felder.fleisch mit "zeile"/"seite"), ausgewaehlt nach der Vorhersage im Spiellicht
#   (x12_vorrat.py / Spiellauf), keine Farbverschiebung;
# - Probe b: Band-Klasse auf ALLEN 11 Sektoren der Fugen (Mantel S-C und D-E = KERBE_LAENGE x L um jedes Gelenk, dazu die Deckel), UV aus dem
#   Ring-Feld felder.ring (Original-Zeile 480 = Asset 493, Seite 0): quer gespiegelt |theta| / 180 (Bauch u0 -> Ruecken u1), laengs vom hellen
#   Ende am Gelenk (t 0) zum dunklen Ende am vollen Querschnitt (t 1) = weicher Verlauf aus den Original-Texeln des Felds.""")
# 2) klasse_von: Ring-Modus
ers("""    art, e, (ra, rb), j = f
    if art == 'spitze' or j not in BAUCH_J:
        return 'grau'""",
    """    art, e, (ra, rb), j = f
    if art == 'spitze':
        return 'grau'
    if farbe.get('ring') and farbe['band']:
        if g == 6:
            fuge_r = art == 'deckel_anf' or (art == 'mantel' and 'S' in (ra, rb))
        else:
            fuge_r = art.startswith('deckel') or (art == 'mantel' and bool({ra, rb} & {'S', 'E'}))
        if fuge_r:
            return 'band'                                  # Runde 8c: Ring umlaufend (alle Sektoren j)
    if j not in BAUCH_J:
        return 'grau'""")
# 3) uv_setzen: Ring-UV
ers("""            else:
                # Band: quer gependelt ueber die zwei mittleren Felder, laengs v0 am Gelenkring (S / E), v1 am vollen Querschnitt (C / D)
                nm = [n_ for n_ in (ra, rb) if info['pos'][n_] <= i < info['pos'][n_] + S_] if i in ring_k else [ra]
                am_gelenk = 1.0 if (nm and nm[0] in ('S', 'E')) else 0.0
                uv_e.append(_uv_feld(feld_band, s_um, 1.0 - am_gelenk))""",
    """            else:
                nm = [n_ for n_ in (ra, rb) if info['pos'][n_] <= i < info['pos'][n_] + S_] if i in ring_k else [ra]
                am_gelenk = 1.0 if (nm and nm[0] in ('S', 'E')) else 0.0
                if farbe.get('ring'):
                    # Runde 8c: Ring - quer gespiegelt |theta| / 180 (Bauch 0, Ruecken 1), laengs t 0 am Gelenk (helles Ende des Felds) -> 1 am vollen
                    # Querschnitt (dunkles Ende); Richtung des hellen Endes "richt" (v- = hell bei v1)
                    ts = th if th <= 180.0 else th - 360.0
                    fr = felder['ring']
                    t_ = 1.0 - am_gelenk
                    vv = fr['v1'] - t_ * (fr['v1'] - fr['v0']) if fr['richt'] == 'v-' else fr['v0'] + t_ * (fr['v1'] - fr['v0'])
                    uv_e.append((fr['u0'] + abs(ts) / 180.0 * (fr['u1'] - fr['u0']), vv))
                else:
                    # Band (Runde 8b): quer gependelt ueber die zwei mittleren Felder, laengs v0 am Gelenkring (S / E), v1 am vollen Querschnitt (C / D)
                    uv_e.append(_uv_feld(feld_band, s_um, 1.0 - am_gelenk))""")
# 4) uv_eintrag: Zeile/Seite je Klasse aus den Feldern
ers("""def uv_eintrag(uvl, kl):
    zeile, seite = {'grau': (ZEILE_GRAU, 2), 'bauch': (ZEILE_FLEISCH, SEITE_FLEISCH), 'band': (ZEILE_BAND, SEITE_BAND)}[kl]""",
    """def zeilen_seiten(felder, farbe):
    \"\"\"Klasse -> (Asset-Zeile, Seite). Runde 8c: Bauch aus felder.fleisch "zeile"/"seite" (Standard 495 / 2), Ring aus felder.ring.\"\"\"
    ff = felder['fleisch']
    zs = {'grau': (ZEILE_GRAU, 2), 'bauch': (int(ff.get('zeile', ZEILE_FLEISCH)), int(ff.get('seite', SEITE_FLEISCH))), 'band': (ZEILE_BAND, SEITE_BAND)}
    if farbe.get('ring'):
        zs['band'] = (int(felder['ring']['zeile']), int(felder['ring']['seite']))
    return zs


def uv_eintrag(uvl, kl, zs=None):
    zeile, seite = (zs or {'grau': (ZEILE_GRAU, 2), 'bauch': (ZEILE_FLEISCH, SEITE_FLEISCH), 'band': (ZEILE_BAND, SEITE_BAND)})[kl]""")
# 5) stufe: zs
ers("""        uvs = uv_setzen(g, V_, F, ringe, info, fa, felder, L)""",
    """        uvs = uv_setzen(g, V_, F, ringe, info, fa, felder, L)
        zs = zeilen_seiten(felder, fa)""")
ers("""                m['qf'].append([x for i in e for x in (i, i)]); m['quv'].append(uv_eintrag(uvl, kl))
            else:
                m['tf'].append([x for i in e for x in (i, i)]); m['tuv'].append(uv_eintrag(uvl, kl))""",
    """                m['qf'].append([x for i in e for x in (i, i)]); m['quv'].append(uv_eintrag(uvl, kl, zs))
            else:
                m['tf'].append([x for i in e for x in (i, i)]); m['tuv'].append(uv_eintrag(uvl, kl, zs))""")
open(p, 'w', encoding='utf-8').write(s)
print('ok')
