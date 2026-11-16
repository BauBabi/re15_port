#!/usr/bin/env python3
"""K5g3 - Riesenform mit Textur auf der B4-Geometrie (Runde 41; Auftrag K5g3 Textur-Bauer Runde 1, 2026-11-14). Variante, Spiel-Asset bleibt (G5).

    python kf_k5g3.py bauen [<aus-Ordner>]       Bau in zwei Stufen; Vorgabe K5/k5g/K5g3 (deterministisch: zweiter Lauf byte-gleich)
    python kf_k5g3.py pruef                      zweimal bauen (Scratchpad) und mit K5/k5g/K5g3 vergleichen (Exit 1 bei Unterschied)
    python kf_k5g3.py --f <lo> <hi> <schritte>   haut.f per Bisektion im Spiel (P L* = Median der Original-Gegner 7,71, O3) in k5g3_par.json schreiben
    python kf_k5g3.py --s <lo> <hi> <schritte>   haut.s per Bisektion im Spiel (Zielband Z4/Z8, O3) in k5g3_par.json schreiben
    python kf_k5g3.py --auge <lo> <hi> <schritte> auge.f per Bisektion (O4: Median der Augen-Pixel in Bild 2 >= 3 x Kopfhaut daneben) in k5g3_par.json
    python kf_k5g3.py --fleisch <lo> <hi> <schritte>  fleisch.k per Bisektion (Median C* Zone 27 = 24, Eichung b) in k5g3_par.json
    python kf_k5g3.py --w6                       Bild 2 Video gegen Vorlage (W6, Pruefung des globalen f; schreibt nichts)

Stufe 1 Geometrie = kf_k5g3_geo (Einbau-Bauer, unveraendert aufgerufen): Posen (optionale Posen-Datei G2) -> Krallen 19/20/28/29 aus K5c (G2)
           -> Spalt B4 (G1). Die Textur braucht die Flaechen VOR dem Spalt (Schluessel wie zonen_v2/traeger_texel), darum:
           geometrie_form(stufen=('posen', 'krallen')) -> Textur -> kf_k5g2_geo.mulde_auf_form(form, tabelle=GEO3_SPALT) (G4: Kinder erben
           CLUT, Seite und UV-Verschiebung der Mutter; die Kopfkugel 21 ist weg). Danach wird geprueft, dass Ecken/Normalen/Flaechen-Ecken
           des Ergebnisses genau die der Geometrie-Stufe sind (sonst Abbruch).
Stufe 2 Textur = kf_k5g3_tex.textur (E3, V1-V6, O3-O5), Parameter K5/k5g/K5g3/k5g3_par.json (Quelle je Wert).
Runde 2 (Befunde PRUEFUNG_K5G3_r1): Spalt mit den UV der Stufe uvfein (geo/spalt_uv_tabelle.json, GEOMETRIE.md 9.6); Kopie-Maske = konvexe Huelle der
           UV samt Spalt-Kindern (kf_k5g3_tex.lese_maske); danach die Augen als eigene Texel an den Nutzerpunkten (kf_k5g3_auge.dekal, Tabelle
           generator/k5g3_augen_mitten.json); Pruefungen im Bau: UV == Geometrie-Stufe + Kopie-Versatz, kein Zugriff auf einen ungeschriebenen Texel.
R9-Einbau (Nutzerentscheid 15.11.2026 "Variante1"): Stufe 1 liefert zusaetzlich die Posen-Datei mit "n52"-Keys und die Stufe hand18 (Netz 18 =
           Spiegelhand). Die Textur malt weiter auf den K5e_f1-Flaechen (Netz 18 alt, Schluessel zonen_v2/traeger_texel); nach dem Spalt setzt
           hand18_aus_13 Netz 18 = kf_k5g3_geo.spiegelhand(BEMALTES Netz 13): Ecken/Normalen/Flaechen-Ecken wie die Geometrie-Stufe (geprueft),
           UV/CLUT/Seite = die von 13 samt deren Kopie-Versatz (R9 N33 "UV/CLUT/Seite wie 13"). Der Pruef-Stand (pruef/) behaelt das alte Netz 18
           (die Textur-Riegel lesen dort die K5e_f1-Flaechen-Schluessel). Basis: ruhe/K5e_f1 per sha256 (seit dem Einbau ist das Spiel-Asset K5g3).
Ausgabe: BAHNSTEIG_FORM.EMD/.TIM (28 Clips), FORM.EMD/.TIM/form.json (3 Keys, Tafeln/Riegel), pruef/ (Textur auf den Flaechen VOR dem
Spalt, fuer die Textur-Riegel), bau.json.
"""
import hashlib
import json
import os
import shutil
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import numpy as np  # noqa: E402
import kf_k5e as E  # noqa: E402
import kf_k5g2_geo as GG  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402
import kf_k5g3_geo as G3  # noqa: E402
import kf_k5g3_tex as T3  # noqa: E402
import kf_k5g3_auge as AU  # noqa: E402
import kf_k5g3_zaehne as ZV  # noqa: E402

FE = E.FE
K5 = E.K5
WT = E.WT
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
ASSET = os.path.join(K5, 'einbau_k5e_f1', 'BAHNSTEIG_FORM')   # Spiel-Asset K5e_f1 (E32 bis R9-Einbau 15.11.2026), gesichert in einbau_k5e_f1; shared_assets traegt seitdem K5g3 + R9
AUS = os.path.join(K5, 'k5g', 'K5g3')
PAR = os.path.join(AUS, 'k5g3_par.json')
SCR = os.environ.get('K5G3_SCR', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/k5g3tex')
DATEIEN = ('BAHNSTEIG_FORM.EMD', 'BAHNSTEIG_FORM.TIM', 'FORM.EMD', 'FORM.TIM', 'form.json', 'pruef/BAHNSTEIG_FORM.EMD', 'pruef/BAHNSTEIG_FORM.TIM',
           'pruef/FORM.EMD', 'pruef/FORM.TIM', 'pruef/form.json')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def log(*a):
    print(*a, flush=True)


def geo_schluessel(form):
    """Ecken, Normalen und Flaechen-Ecken aller Netze (ohne UV/CLUT/Seite) - was die Textur-Stufe nicht aendern darf."""
    return [[m['ecken'], m['normalen'], [d['e'] for d in m['dreiecke']], [q['e'] for q in m['vierecke']],
             [d['n'] for d in m['dreiecke']], [q['n'] for q in m['vierecke']]] for m in form['meshes']]


def form_json(aus, name):
    fj = json.load(open(os.path.join(K5E, 'form.json'), encoding='utf-8'))
    fj['name'] = name
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)


def kinder_uv(tabelle):
    """{Mutterflaeche (t, a, i): [UV-Dreieck je Spalt-Kind, global (u + 128 x Seite, v)]} aus der Spalt-Tabelle (Runde 2: uvfein-UV)."""
    tab = json.load(open(tabelle, encoding='utf-8'))
    art = ('dreiecke', 'vierecke')
    aus = {}
    for t, a, i, mt, ma, mi in tab['herkunft']:
        f = tab['teile'][str(t)][art[a]][i]
        po = (int(f['page']) & 15) * 128
        aus.setdefault((mt, ma, mi), []).append([[int(u) + po, int(v)] for u, v in f['uv']])
    return aus


def gx(f):
    po = (int(f['page']) & 15) * 128
    return [(int(u) + po, int(v)) for u, v in f['uv']]


def uv_gegen_geo(form, form_v, ausnahmen):
    """Runde 2 (GEOMETRIE.md 9.6): jede Flaeche liest die UV der Geometrie-Stufe (uvfein), hoechstens um eine Kopie verschoben (EIN Versatz je
    Flaeche). Ausnahmen: Augen-Dreiecke (eigene UV). Rueckgabe Liste der Abweichungen."""
    falsch = []
    for t, (m, mv) in enumerate(zip(form['meshes'], form_v['meshes'])):
        for a, art in enumerate(('dreiecke', 'vierecke')):
            for i, (f, fv) in enumerate(zip(m[art], mv[art])):
                if (t, a, i) in ausnahmen:
                    continue
                d = set((x - xv, y - yv) for (x, y), (xv, yv) in zip(gx(f), gx(fv)))
                if len(d) != 1:
                    falsch.append('%d|%d|%d' % (t, a, i))
    return falsch


def abtasten(form, px, cy, n=16):
    """Texel-Zugriffe im Inneren jedes Render-Dreiecks (n x n baryzentrische Proben, floor wie der Renderer; Vierecke (0,1,2)+(1,3,2) UND
    (0,1,3)+(0,3,2)). Rueckgabe Liste (flaeche, zeile, x, y, index)."""
    ws = [(i + 0.5) / n for i in range(n)]
    W = np.array([(a, b, 1 - a - b) for a in ws for b in ws if a + b < 1], float)
    aus = []
    for t, m in enumerate(form['meshes']):
        for a, art in enumerate(('dreiecke', 'vierecke')):
            for i, f in enumerate(m[art]):
                g = np.array(gx(f), float)
                z = ((int(f['clut']) >> 6) & 0x1FF) - cy
                tris = [(0, 1, 2)] if a == 0 else [(0, 1, 2), (1, 3, 2), (0, 1, 3), (0, 3, 2)]
                for tr in tris:
                    P = W @ g[list(tr)]
                    xs = np.clip(np.floor(P[:, 0]).astype(int), 0, px.shape[1] - 1)
                    ys = np.clip(np.floor(P[:, 1]).astype(int), 0, px.shape[0] - 1)
                    for x, y in sorted(set(zip(xs.tolist(), ys.tolist()))):
                        aus.append(((t, a, i), z, x, y, int(px[y, x])))
    return aus


K5E_SHA = ('f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1', '31a35e66')    # ruhe/K5e_f1 EMD / TIM (Commit 62bd0001b)


def hand18_aus_13(form, form_v):
    """R9-Einbau: Netz 18 = spiegelhand(bemaltes Netz 13). Ecken, Normalen und Flaechen-Ecken muessen die der Geometrie-Stufe sein."""
    m = G3.spiegelhand(form['meshes'][G3.HAND_LINKS])
    v = form_v['meshes'][G3.HAND]
    if (m['ecken'] != [list(e)[:3] for e in v['ecken']] or m['normalen'] != [list(n)[:3] for n in v['normalen']]
            or [d['e'] for d in m['dreiecke']] != [d['e'] for d in v['dreiecke']] or [q['e'] for q in m['vierecke']] != [q['e'] for q in v['vierecke']]):
        raise SystemExit('Netz 18 aus dem bemalten Netz 13 != Netz 18 der Geometrie-Stufe (hand18)')
    form['meshes'][G3.HAND] = m
    return m


def bauen(par, aus, log=log):
    emd_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    if sha(emd_a) != K5E_SHA[0] or not sha(tim_a).startswith(K5E_SHA[1]):
        raise SystemExit('ruhe/K5e_f1 hat nicht die sha256 der Basis (EMD f277916d, TIM 31a35e66) - Basis veraendert')
    # ---- Stufe 1: Geometrie (Einbau-Bauer). Flaechen vor dem Spalt fuer die Textur, fertige Geometrie zum Vergleich.
    sek, form, gber = G3.geometrie_form(stufen=('posen', 'krallen'))
    sek_v, form_v, gber_v = G3.geometrie_form()
    if sek != sek_v:
        raise SystemExit('Posen-Sektionen der Teilstufe != Geometrie-Stufe')
    Z = TX.lade_zonen()
    X = E.eingaenge()
    # ---- Stufe 2: Textur
    kinder = kinder_uv(G3.GEO3_SPALT_UV)
    px, reihen, ber_t, plan, _fz, _zw, _je, belegt = T3.textur(form, par, X, Z, kinder)
    T = X['T']
    os.makedirs(os.path.join(aus, 'pruef'), exist_ok=True)
    # Pruef-Stand: dieselbe Textur auf den Flaechen VOR dem Spalt (Schluessel wie zonen_v2/traeger_texel) - nur fuer die Textur-Riegel
    pe = GG.emd_mit_md1(sek, form)
    # ---- Spalt B4 auf die bemalten Flaechen (G4), Runde 2 mit den UV der Stufe uvfein (GEOMETRIE.md 9.6: GEO3_SPALT_UV statt GEO3_SPALT).
    # kinder_ausserhalb (Kind-UV ausserhalb des Mutter-Rechtecks +-1) bricht nicht mehr ab: die Kopie-Maske (kf_k5g3_tex.lese_maske) nimmt die
    # Kind-UV mit; geprueft wird unten am Ergebnis (kein Zugriff auf einen ungeschriebenen Texel).
    mber = GG.mulde_auf_form(form, tabelle=G3.GEO3_SPALT_UV)
    if 'hand18' in gber_v:
        hand18_aus_13(form, form_v)
    if geo_schluessel(form) != geo_schluessel(form_v):
        raise SystemExit('Geometrie nach der Textur-Stufe != Geometrie-Stufe kf_k5g3_geo')
    # ---- Augen: eigene Texel an den Nutzerpunkten (kf_k5g3_auge.dekal; E3, O4)
    geschrieben = belegt.copy()        # textur: Lesemasken der Ausgangs-Flaechen (mit Spalt-Kindern) + Kopie-Ziele
    for m in form['meshes']:
        for art in ('dreiecke', 'vierecke'):
            for f in m[art]:
                belegt |= T3.lese_maske(f, belegt.shape)
    vor = belegt.copy()
    aber = AU.dekal(form, px, reihen, T, ber_t['auge_rampe'], par, belegt)
    geschrieben |= belegt & ~vor
    augen = set((AU.TEIL, 0, i) for i in aber['dreiecke'])
    falsch = uv_gegen_geo(form, form_v, augen)
    if falsch:
        raise SystemExit('UV != Geometrie-Stufe (uvfein) + Kopie-Versatz: %s' % falsch[:10])
    # ---- Stufe zaehne_vorn (Nutzer 16.11.2026 "ganz vorne im Maul keine Zaehne", "Oben und unten"): die Seitenreihen um die Schnauzenspitze
    # fortsetzen (kf_k5g3_zaehne, Regeln R1-R6 dort). Nur ANGEHAENGT an Netz 22/23 (Ecken, Normalen, je Zahn 3 Dreiecke mit UV/CLUT/Seite des
    # vordersten alten Zahns); alte Ecken/Normalen/Flaechen, Posen und TIM bleiben bytegleich. Danach wird mit abgetastet (Zahnzeile).
    zber = ZV.einsetzen(form, ZV.vorgabe())
    # kein Zugriff auf einen Texel, den weder die Ausgangs-Form gelesen noch eine Kopie/ein Augen-Block geschrieben hat (Pruefer K5g3 r1 ROT 2:
    # 26 Zugriffe auf leere Kopie-Texel = Index 0 = 0x7FFF auf 10|1|76)
    zug = abtasten(form, px, int(T['clut_y']))
    leer = [z for z in zug if not geschrieben[z[3], z[2]]]
    if leer:
        raise SystemExit('Zugriffe auf ungeschriebene Texel: %d, z. B. %s' % (len(leer), leer[:5]))
    nz = len(reihen)
    if nz > 16:
        raise SystemExit('%d Zeilen > 16' % nz)
    tim = FE.tim_schreiben(px, reihen, T['clut_x'], T['clut_y'], T['bild_x'], T['bild_y'])
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.EMD'), 'wb').write(pe)
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.TIM'), 'wb').write(tim)
    open(os.path.join(aus, 'pruef', 'FORM.EMD'), 'wb').write(G3.form_emd_bytes(pe))
    open(os.path.join(aus, 'pruef', 'FORM.TIM'), 'wb').write(tim)
    form_json(os.path.join(aus, 'pruef'), par.get('name', 'K5g3') + ' (Pruef-Stand vor dem Spalt)')
    ber_t['zeilen'] = ber_t['zeilen'] + ['auge']
    ber_t['augen_dekal'] = aber
    ber_t['zugriffe_abgetastet'] = len(zug)
    ber_t['zugriffe_ungeschrieben'] = len(leer)
    emd_bahn = GG.emd_mit_md1(sek, form)
    emd_form = G3.form_emd_bytes(emd_bahn)
    for n, b in (('BAHNSTEIG_FORM.EMD', emd_bahn), ('BAHNSTEIG_FORM.TIM', tim), ('FORM.EMD', emd_form), ('FORM.TIM', tim)):
        open(os.path.join(aus, n), 'wb').write(b)
    form_json(aus, par.get('name', 'K5g3'))
    geo_ziel = G3.GEO3_ZIEL
    geo_sha = sha(open(geo_ziel, 'rb').read()) if os.path.exists(geo_ziel) else None
    bericht = dict(variante=par.get('name', 'K5g3'), generator='generator/kf_k5g3.py (Stufe 1 kf_k5g3_geo, Stufe 2 kf_k5g3_tex)', par=par,
                   geometrie=dict(stufe='kf_k5g3_geo.geometrie_form', teilstufe=gber, voll=gber_v,
                                  geometrie_gleich_kf_k5g3_geo='ja (Ecken, Normalen, Flaechen-Ecken aller Netze; sonst Abbruch)',
                                  geo_emd_sha256=geo_sha, spalt_tabelle='geo/spalt_uv_tabelle.json (uvfein)',
                                  spalt=dict(muetter_verschoben=mber['muetter_verschoben'], kinder_verschoben=mber['kinder_verschoben'],
                                             kinder_neue_zeile=mber['kinder_clut_neu'],
                                             kinder_ausserhalb_mutter_rechteck=len(mber['kinder_ausserhalb'])),
                                  uv_gegen_geo='gleich bis auf den Kopie-Versatz (ausser %d Augen-Dreiecke)' % len(augen)),
                   zaehne_vorn=dict(stufe='kf_k5g3_zaehne.einsetzen', bericht=zber, reihen=ZV.bericht_reihen(ZV.vorgabe())),
                   textur=ber_t, zeilen=len(reihen), port_texturhoehe=len(reihen) * 256,
                   sha256={n: sha(open(os.path.join(aus, n), 'rb').read()) for n in DATEIEN if not n.endswith('form.json')},
                   eingang_sha256=dict(k5e_f1_bahnsteig_emd=sha(emd_a), k5e_f1_tim=sha(tim_a),
                                       basis_k5c_tim=sha(open(os.path.join(E.BASIS, 'FORM.TIM'), 'rb').read()),
                                       zonen_v2=sha(open(TX.ZONEN, 'rb').read()), traeger_texel=sha(open(TX.TRAEGER, 'rb').read()),
                                       material_json=sha(open(E.MATERIAL, 'rb').read()),
                                       spalt_tabelle=sha(open(G3.GEO3_SPALT, 'rb').read()), krallen_k5c=sha(open(G3.GEO3_KRALLEN, 'rb').read()),
                                       spalt_uv_tabelle=sha(open(G3.GEO3_SPALT_UV, 'rb').read().rstrip(b'\r\n')),
                                       augen_mitten=sha(open(AU.TABELLE, 'rb').read().replace(b'\r\n', b'\n')),
                                       zaehne_kopf_k5c=sha(open(ZV.KOPF_JSON, 'rb').read().replace(b'\r\n', b'\n')),
                                       zaehne_vorn_laengen=sha(open(ZV.LAENGEN, 'rb').read().replace(b'\r\n', b'\n'))))
    json.dump(bericht, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False, sort_keys=True)
    log('K5g3 gebaut: %s  Zeilen %d  EMD %s  TIM %s' % (aus.replace(chr(92), '/'), len(reihen), bericht['sha256']['BAHNSTEIG_FORM.EMD'][:16],
                                                       bericht['sha256']['BAHNSTEIG_FORM.TIM'][:16]))
    return bericht


def lade_par():
    return json.load(open(PAR, encoding='utf-8'))


def pruef():
    par = lade_par()
    gleich = True
    for lauf in ('lauf1', 'lauf2'):
        d = os.path.join(SCR, 'pruef_' + lauf)
        shutil.rmtree(d, ignore_errors=True)
        bauen(par, d, log=lambda *a: None)
        for n in DATEIEN + ('bau.json',):
            if n.endswith('.json'):         # Text: inhaltlich vergleichen (git kann beim Auschecken das Zeilenende tauschen)
                a = json.load(open(os.path.join(AUS, n), encoding='utf-8')) if os.path.exists(os.path.join(AUS, n)) else None
                b = json.load(open(os.path.join(d, n), encoding='utf-8'))
            else:
                a = open(os.path.join(AUS, n), 'rb').read() if os.path.exists(os.path.join(AUS, n)) else b''
                b = open(os.path.join(d, n), 'rb').read()
            if a != b:
                gleich = False
                log('VERSCHIEDEN %s (%s)' % (n, lauf))
    log('zwei Laeufe und Abgabe byte-gleich: %s' % ('JA' if gleich else 'NEIN'))
    return gleich


# ------------------------------------------------------------------ Bisektionen (schreiben k5g3_par.json mit Protokoll)
def _spiel_paar(v, name):
    """Zielband-Lauf (Messblatt K5F 1.1): Schluessel-Paar, 5080 Bild-1-Lage Bilder 360-440, Masse P/Q (kf_k5f_spielmass.paar)."""
    import subprocess
    import kf_k5f_spielmass as KS
    KS.SP = os.path.join(SCR, 'spiel').replace(chr(92), '/')
    KS.schluessel(v, os.path.join(SCR, 'ueber', 'form_' + name))
    for art in ('normal', 'key'):
        subprocess.run([shutil.which('bash') or 'bash', os.path.join(HIER, 'kf_k5g3_zlauf.sh'), 'f80_%s_%s' % (name, art), '5080', '360-440/20',
                        'RE15_BIRKIN_FORM=bahnsteig', 'RE15_BB_POSE=-20711,-15984,756,0,11,1', 'RE15_FORCE_CUT=1', 'RE15_PLAYER_POS=500,-9175,2048,0',
                        'RE15_RE2_ASSET_ROOT=%s' % os.path.join(SCR, 'ueber', 'form_%s_%s' % (name, art)).replace(chr(92), '/')],
                       check=False, stdout=subprocess.DEVNULL)
    r = KS.paar('f80_%s_normal' % name, 'f80_%s_key' % name)
    if r is None:
        raise SystemExit('Spiel-Lauf %s ohne Bilder' % name)
    return r['mittel']


def s_bisektion(lo, hi, schritte):
    """haut.s (O3): kleinste Buntheit, mit der die Form im Spiel (5080 Bild-1-Lage, eigenes Normal-/Schluessel-Paar) Z4 P-C* >= 4,41 und
    Z8 Q-C* >= 12,09 erreicht, dazu die Untergrenzen der Buntheits-Streuung Z5 P C-std >= 2,39 und Z9 Q C-std >= 6,53 (Messblatt K5F 2.1;
    gemessen 2026-11-14: bei f 1,86 erreicht s 0,4375 Z4/Z8, Z9 erst ab s 0,53). 'Wenig bunt innerhalb des Bandes' = kleinste."""
    par = lade_par()
    proto = []

    def mess(sv):
        p = json.loads(json.dumps(par))
        p['textur']['haut']['s'] = sv
        n = 'sbis_%s' % ('%.4f' % sv).replace('.', '')
        v = os.path.join(SCR, 'bis', n)
        shutil.rmtree(v, ignore_errors=True)
        bauen(p, v, log=lambda *a: None)
        m = _spiel_paar(v, n)
        proto.append([sv] + [m[k] for k in ('L_mittel', 'C_mittel', 'Q_C_mittel', 'C_std', 'Q_C_std')])
        log('  s %.4f -> P L* %.2f | P C* %.2f Q C* %.2f | P C-std %.2f Q C-std %.2f' % tuple(proto[-1]))
        return m['C_mittel'] >= 4.41 and m['Q_C_mittel'] >= 12.09 and m['C_std'] >= 2.39 and m['Q_C_std'] >= 6.53
    if not mess(hi):
        raise SystemExit('auch s %.4f erreicht Z4/Z8 nicht' % hi)
    for _ in range(schritte):
        sv = round(0.5 * (lo + hi), 4)
        if mess(sv):
            hi = sv
        else:
            lo = sv
    par = lade_par()
    par['textur']['haut']['s'] = round(hi, 4)
    par['textur']['haut']['s_quelle'] = (
        'O3 + Messblatt K5F 3.1: kleinste Buntheit (C\' = s x L\') mit Z4 P-C* >= 4,41, Z8 Q-C* >= 12,09, Z5 P C-std >= 2,39 und Z9 Q C-std >= 6,53 im echten Spiel (5080 Bild-1-Lage, '
        'Bilder 360-440, eigenes Normal-/Schluessel-Paar, kf_k5g3_zlauf.sh mit RE15_FORCE_CUT=1); Bisektion kf_k5g3.py --s [%s, %s], %d Schritte. '
        'Protokoll [s, P L*, P C*, Q C*, P C-std, Q C-std]: %s' % (lo, hi, schritte, proto))
    json.dump(par, open(PAR, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    log('fertig s', par['textur']['haut']['s'])


ZIEL_L = 7.71     # Median P L* der Original-Gegner (Messblatt K5F 1.2: 8,04 / 7,38 / 18,80 / 5,57)


def f_bisektion(lo, hi, schritte):
    """haut.f (O3 "wie die restlichen Gegner"): kleinster globaler Helligkeitsfaktor, mit dem die Form im Spiel (5080 Bild-1-Lage, eigenes
    Normal-/Schluessel-Paar) P L* >= ZIEL_L erreicht = Median der vier Original-Gegner im eigenen Raumlicht (Messblatt K5F 1.2/2.1: Zombie 8,04,
    Gorilla 7,38, EM30 18,80, Tyrant 5,57 -> Median 7,71). Der Rand des Bandes (5,57) waere die dunkelste Figur der Familie (Tyrant); die
    Video-Eichung W6 (K5g2: f >= 1,339) gibt nach O3 nur die Richtung und bleibt untere Schranke."""
    par = lade_par()
    proto = []

    def mess(fv):
        p = json.loads(json.dumps(par))
        p['textur']['haut']['f'] = fv
        n = 'fbis_%s' % ('%.4f' % fv).replace('.', '')
        v = os.path.join(SCR, 'bis', n)
        shutil.rmtree(v, ignore_errors=True)
        bauen(p, v, log=lambda *a: None)
        m = _spiel_paar(v, n)
        proto.append([fv] + [m[k] for k in ('L_mittel', 'L_cv', 'C_mittel', 'Q_C_mittel')])
        log('  f %.4f -> P L* %.2f | P L-cv %.3f | P C* %.2f Q C* %.2f' % tuple(proto[-1]))
        return m['L_mittel'] >= ZIEL_L
    if not mess(hi):
        raise SystemExit('auch f %.4f erreicht P L* %.2f nicht' % (hi, ZIEL_L))
    for _ in range(schritte):
        fv = round(0.5 * (lo + hi), 4)
        if mess(fv):
            hi = fv
        else:
            lo = fv
    fv = max(round(hi + 0.0049, 2), 1.339)
    par = lade_par()
    par['textur']['haut']['f'] = fv
    par['textur']['haut']['f_quelle'] = (
        'O3 + Messblatt K5F 1.2/2.1: kleinster globaler Faktor mit P L* >= 7,71 (Median der vier Original-Gegner Zombie 8,04 / Gorilla 7,38 / '
        'EM30 18,80 / Tyrant 5,57 im eigenen Raumlicht) im echten Spiel (5080 Bild-1-Lage, Bilder 360-440, eigenes '
        'Normal-/Schluessel-Paar, kf_k5g3_zlauf.sh), aufgerundet auf 0,01; zweite Untergrenze W6 (Bild 2 Video >= Vorlage 15,37: f >= 1,339 aus '
        'K5g2 Runde 2, dieselbe Haut-Abbildung). Bei s %.4f gemessen; Bisektion kf_k5g3.py --f, %d Schritte. Protokoll [f, P L*, P L-cv, P C*, '
        'Q C*]: %s' % (float(par['textur']['haut']['s']), schritte, proto))
    json.dump(par, open(PAR, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    log('fertig f', fv)


def fleisch_mass(v):
    """Median L*/C* der Fleisch-Material-Texelzugriffe der Zone 27 (Pruef-Stand; Definition wie Riegel V3.3 riegel_k5g2.v3_fleisch)."""
    import riegel_k5g2 as R2
    nm = json.load(open(os.path.join(v, 'bau.json'), encoding='utf-8'))['textur']['zeilen']
    S = R2.Stand(os.path.join(v, 'pruef'))
    Ls = []
    for fs in json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']['27']:
        k = TX.schluessel(fs)
        _y, _x, idx = S.texel(k)
        z = S.zeile[k]
        b = nm.index(nm[z].split('_')[1]) if nm[z].startswith('fleck_') else z
        m = np.array(S.cl[z])[idx] != np.array(S.cl[b])[idx]
        Ls.append(S.lab[z][idx[m]])
    L = np.vstack(Ls)
    return float(np.median(L[:, 0])), float(np.median(np.hypot(L[:, 1], L[:, 2])))


def fleisch_bisektion(lo, hi, schritte):
    """fleisch.k (Eichung b, GEGENPRUEFUNG_EICHUNG belastbar: Buckel C* 22-26): kleinstes k mit Median C* der Zone-27-Material-Texelzugriffe
    >= 24 (Mitte des Bandes), gemessen am gebauten Pruef-Stand wie Riegel V3.3. Grund: k = C_ziel / Quellmedian ergab 18,5 (K5g2) bzw. mit
    weicher Grenze und Rundung im Generator 20,2 - die Verteilung der Zone-27-Texel auf die Rampe ist nicht die der Quellgewichte."""
    par = lade_par()
    proto = []

    def mess(kv):
        p = json.loads(json.dumps(par))
        p['textur']['fleisch']['k'] = kv
        v = os.path.join(SCR, 'bis', 'fleisch')
        shutil.rmtree(v, ignore_errors=True)
        bauen(p, v, log=lambda *a: None)
        Lm, Cm = fleisch_mass(v)
        proto.append([kv, round(Lm, 2), round(Cm, 2)])
        log('  k %.4f -> L* %.2f C* %.2f' % (kv, Lm, Cm))
        return Cm >= float(par['textur']['fleisch']['C_ziel'])
    if not mess(hi):
        raise SystemExit('auch k %.4f reicht nicht' % hi)
    for _ in range(schritte):
        kv = round(0.5 * (lo + hi), 4)
        if mess(kv):
            hi = kv
        else:
            lo = kv
    par = lade_par()
    par['textur']['fleisch']['k'] = round(hi, 4)
    par['textur']['fleisch']['k_quelle'] = (
        'Eichung b (Buckel C* 22-26, Mitte 24): kleinstes k mit Median C* der Zone-27-Material-Texelzugriffe >= 24 am gebauten Pruef-Stand '
        '(wie Riegel V3.3); Bisektion kf_k5g3.py --fleisch [%s, %s], %d Schritte. Protokoll [k, L*, C*]: %s' % (lo, hi, schritte, proto))
    json.dump(par, open(PAR, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    log('fertig k', par['textur']['fleisch']['k'])


NUTZERPUNKTE = ((217.4, 274.3, 6.5), (280.2, 275.1, 12.3))     # zonen_v2.json augen.nutzerpunkte: punkt_vorlage, punkt_px_vorlage
AUGE_FAKTOR = 3.0


def auge_mass(v):
    """O4 (Runde 2): Bild 2 (Buehne, Licht der Szene, ohne Videofilter). Augen-Pixel = Pixel von Teil 22, die einen Augen-Eintrag der Zeile 'auge'
    lesen (bau.json textur.augen_dekal), je Nutzerpunkt im Umkreis 8 px. Rueckgabe je Auge dict(px, nutzer_px, mitte_abstand, L_median, L_max,
    ring_median) - Ring = Pixel von Teil 22 im Abstand 4-12 px ohne Augen-Pixel."""
    sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
    import v4f
    v4f._KC.pop(v, None)
    a = json.load(open(os.path.join(v, 'bau.json'), encoding='utf-8'))['textur']['augen_dekal']
    sauber, K = v4f.szene_id(v, 2)
    L = v4f.lab(sauber)[..., 0]
    eye = (K['zeile'] == a['zeile']) & np.isin(K['eintrag'], a['augen_eintraege']) & (K['mesh'] == 22)
    yy, xx = np.mgrid[0:L.shape[0], 0:L.shape[1]]
    out = []
    for x, y, u in NUTZERPUNKTE:
        d = np.hypot(xx + 0.5 - x, yy + 0.5 - y)
        e = eye & (d < 8)
        ring = (K['mesh'] == 22) & (d >= 4) & (d < 12) & ~eye
        ys, xs = np.nonzero(e)
        out.append(dict(px=int(e.sum()), nutzer_px=u, mitte_abstand=float(np.hypot(xs.mean() + 0.5 - x, ys.mean() + 0.5 - y)) if len(xs) else None,
                        L_median=float(np.median(L[e])) if e.any() else 0.0, L_max=float(L[e].max()) if e.any() else 0.0,
                        ring_median=float(np.median(L[ring]))))
    return out


def auge_ok(a):
    return all(t['L_median'] >= AUGE_FAKTOR * t['ring_median'] for t in a)


def auge_bisektion(lo, hi, schritte):
    """auge.f (O4, Runde 2): kleinster Faktor (multiplikativ auf die hellen G4-Augeneintraege, Knie wie die Haut), mit dem in Bild 2 (Licht der
    Szene) an BEIDEN Nutzerpunkten der Median der Augen-Pixel >= AUGE_FAKTOR x Median der Kopfhaut daneben ist. Runde 1 nahm das 9x9-Maximum
    >= 2 x (Sicht-Pruefer: ein 4-Pixel-Strich reicht dafuer, im Spielfenster war kein Auge zu sehen); jetzt der Median des ganzen Auges."""
    par = lade_par()
    proto = []

    def mess(fv):
        p = json.loads(json.dumps(par))
        p['textur']['auge']['f'] = fv
        v = os.path.join(SCR, 'bis', 'auge')
        shutil.rmtree(v, ignore_errors=True)
        bauen(p, v, log=lambda *a: None)
        a = auge_mass(v)
        proto.append([fv] + [round(x, 2) for t in a for x in (t['L_median'], t['ring_median'])])
        log('  f_auge %.4f -> %s' % (fv, proto[-1][1:]))
        return auge_ok(a)
    if not mess(hi):
        raise SystemExit('auch f_auge %.3f reicht nicht' % hi)
    for _ in range(schritte):
        fv = round(0.5 * (lo + hi), 4)
        if mess(fv):
            hi = fv
        else:
            lo = fv
    par = lade_par()
    par['textur']['auge']['f'] = round(hi, 4)
    par['textur']['auge']['f_quelle'] = (
        'O4 (Runde 2): kleinster Faktor (multiplikativ auf die hellen G4-Augeneintraege, Knie wie die Haut), mit dem in Bild 2 (Buehne, Licht der '
        'Szene, ohne Videofilter) an beiden Nutzerpunkten der Median der Augen-Pixel (eigene Augen-Texel, kf_k5g3_auge) >= %.1f x Median der '
        'Kopfhaut daneben ist (Ring 4-12 px). Runde 1: 9x9-Maximum >= 2 x (Sicht-Pruefer r1: im Spielfenster kein Auge). Gemessen am Stand mit f 1,98: '
        'Bild 2 2,1/2,5-fach, im Spielfenster 5080 von vorn 2,2/2,7-fach (L* 10,1/11,2 gegen 4,7/4,1) - zu schwach. Bisektion kf_k5g3.py --auge '
        '[%s, %s], %d Schritte. Protokoll [f, Median1, Ring1, Median2, Ring2]: %s' % (AUGE_FAKTOR, lo, hi, schritte, proto))
    json.dump(par, open(PAR, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    log('fertig f_auge', par['textur']['auge']['f'])


def w6(v=None):
    """W6 (Messblatt K5F 3.3, Pruefung): Bild 2 Video (3 Saaten, Pixelmenge V4F 3.1) gegen Vorlage."""
    sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
    import rl
    import riegel_k5d as RK
    v = v or AUS
    vor = RK.Vorlage()
    _s, Kc = rl.v4f.szene_id(E.BASIS, 2)
    fig = RK.nutzer_union(2) & Kc['maske'] & ~vor.d[2]['clip']
    ziel = float(vor.d[2]['LV'][fig][:, 0].mean())
    rl.v4f._KC.pop(v, None)
    s_, _K = rl.v4f.szene_id(v, 2)
    wert = float(np.mean([RK.labm(rl.v4f.video(s_, 2, sa))[fig][:, 0].mean() for sa in rl.v4f.SAATEN]))
    log('W6 Bild 2 Video %.3f gegen Vorlage %.3f' % (wert, ziel))
    return wert, ziel


if __name__ == '__main__':
    a = sys.argv[1:]
    if not a:
        raise SystemExit(__doc__)
    if a[0] == 'bauen':
        bauen(lade_par(), a[1] if len(a) > 1 else AUS)
    elif a[0] == 'pruef':
        sys.exit(0 if pruef() else 1)
    elif a[0] == '--s':
        s_bisektion(float(a[1]), float(a[2]), int(a[3]))
    elif a[0] == '--f':
        f_bisektion(float(a[1]), float(a[2]), int(a[3]))
    elif a[0] == '--fleisch':
        fleisch_bisektion(float(a[1]), float(a[2]), int(a[3]))
    elif a[0] == '--auge':
        auge_bisektion(float(a[1]), float(a[2]), int(a[3]))
    elif a[0] == '--w6':
        w6(a[1] if len(a) > 1 else None)
    else:
        raise SystemExit(__doc__)
