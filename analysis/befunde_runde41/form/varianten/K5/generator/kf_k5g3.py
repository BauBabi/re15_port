#!/usr/bin/env python3
"""K5g3 - Riesenform mit Textur auf der B4-Geometrie (Runde 41; Auftrag K5g3 Textur-Bauer Runde 1, 2026-11-14). Variante, Spiel-Asset bleibt (G5).

    python kf_k5g3.py bauen [<aus-Ordner>]       Bau in zwei Stufen; Vorgabe K5/k5g/K5g3 (deterministisch: zweiter Lauf byte-gleich)
    python kf_k5g3.py pruef                      zweimal bauen (Scratchpad) und mit K5/k5g/K5g3 vergleichen (Exit 1 bei Unterschied)
    python kf_k5g3.py --f <lo> <hi> <schritte>   haut.f per Bisektion im Spiel (P L* = Median der Original-Gegner 7,71, O3) in k5g3_par.json schreiben
    python kf_k5g3.py --s <lo> <hi> <schritte>   haut.s per Bisektion im Spiel (Zielband Z4/Z8, O3) in k5g3_par.json schreiben
    python kf_k5g3.py --auge <lo> <hi> <schritte> auge.f per Bisektion (O4 Augen sichtbar heller als die Kopfhaut daneben) in k5g3_par.json
    python kf_k5g3.py --fleisch <lo> <hi> <schritte>  fleisch.k per Bisektion (Median C* Zone 27 = 24, Eichung b) in k5g3_par.json
    python kf_k5g3.py --w6                       Bild 2 Video gegen Vorlage (W6, Pruefung des globalen f; schreibt nichts)

Stufe 1 Geometrie = kf_k5g3_geo (Einbau-Bauer, unveraendert aufgerufen): Posen (optionale Posen-Datei G2) -> Krallen 19/20/28/29 aus K5c (G2)
           -> Spalt B4 (G1). Die Textur braucht die Flaechen VOR dem Spalt (Schluessel wie zonen_v2/traeger_texel), darum:
           geometrie_form(stufen=('posen', 'krallen')) -> Textur -> kf_k5g2_geo.mulde_auf_form(form, tabelle=GEO3_SPALT) (G4: Kinder erben
           CLUT, Seite und UV-Verschiebung der Mutter; die Kopfkugel 21 ist weg). Danach wird geprueft, dass Ecken/Normalen/Flaechen-Ecken
           des Ergebnisses genau die der Geometrie-Stufe sind (sonst Abbruch).
Stufe 2 Textur = kf_k5g3_tex.textur (E3, V1-V6, O3-O5), Parameter K5/k5g/K5g3/k5g3_par.json (Quelle je Wert).
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

FE = E.FE
K5 = E.K5
WT = E.WT
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
ASSET = os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM')
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


def bauen(par, aus, log=log):
    emd_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    if emd_a != open(ASSET + '.EMD', 'rb').read() or tim_a != open(ASSET + '.TIM', 'rb').read():
        raise SystemExit('ruhe/K5e_f1 != Spiel-Asset (shared_assets/RE2) - G5 verletzt oder Basis veraendert')
    # ---- Stufe 1: Geometrie (Einbau-Bauer). Flaechen vor dem Spalt fuer die Textur, fertige Geometrie zum Vergleich.
    sek, form, gber = G3.geometrie_form(stufen=('posen', 'krallen'))
    sek_v, form_v, gber_v = G3.geometrie_form()
    if sek != sek_v:
        raise SystemExit('Posen-Sektionen der Teilstufe != Geometrie-Stufe')
    Z = TX.lade_zonen()
    X = E.eingaenge()
    # ---- Stufe 2: Textur
    px, reihen, ber_t, plan, _fz, _zw, _je = T3.textur(form, par, X, Z)
    T = X['T']
    tim = FE.tim_schreiben(px, reihen, T['clut_x'], T['clut_y'], T['bild_x'], T['bild_y'])
    os.makedirs(os.path.join(aus, 'pruef'), exist_ok=True)
    # Pruef-Stand: dieselbe Textur auf den Flaechen VOR dem Spalt (Schluessel wie zonen_v2/traeger_texel) - nur fuer die Textur-Riegel
    pe = GG.emd_mit_md1(sek, form)
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.EMD'), 'wb').write(pe)
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.TIM'), 'wb').write(tim)
    open(os.path.join(aus, 'pruef', 'FORM.EMD'), 'wb').write(G3.form_emd_bytes(pe))
    open(os.path.join(aus, 'pruef', 'FORM.TIM'), 'wb').write(tim)
    form_json(os.path.join(aus, 'pruef'), par.get('name', 'K5g3') + ' (Pruef-Stand vor dem Spalt)')
    # ---- Spalt B4 auf die bemalten Flaechen (G4)
    mber = GG.mulde_auf_form(form, tabelle=G3.GEO3_SPALT)
    if mber['kinder_ausserhalb']:
        raise SystemExit('Spalt-Kinder lesen nach der Kopie fremde Texel: %s' % mber['kinder_ausserhalb'][:10])
    if geo_schluessel(form) != geo_schluessel(form_v):
        raise SystemExit('Geometrie nach der Textur-Stufe != Geometrie-Stufe kf_k5g3_geo')
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
                                  geo_emd_sha256=geo_sha, spalt=dict(muetter_verschoben=mber['muetter_verschoben'],
                                                                      kinder_verschoben=mber['kinder_verschoben'],
                                                                      kinder_neue_zeile=mber['kinder_clut_neu'])),
                   textur=ber_t, zeilen=len(reihen), port_texturhoehe=len(reihen) * 256,
                   sha256={n: sha(open(os.path.join(aus, n), 'rb').read()) for n in DATEIEN if not n.endswith('form.json')},
                   eingang_sha256=dict(k5e_f1_bahnsteig_emd=sha(emd_a), k5e_f1_tim=sha(tim_a),
                                       basis_k5c_tim=sha(open(os.path.join(E.BASIS, 'FORM.TIM'), 'rb').read()),
                                       zonen_v2=sha(open(TX.ZONEN, 'rb').read()), traeger_texel=sha(open(TX.TRAEGER, 'rb').read()),
                                       material_json=sha(open(E.MATERIAL, 'rb').read()),
                                       spalt_tabelle=sha(open(G3.GEO3_SPALT, 'rb').read()), krallen_k5c=sha(open(G3.GEO3_KRALLEN, 'rb').read())))
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


def auge_mass(v):
    """O4: Bild 2 (Buehne, Licht der Szene, ohne Videofilter), je Nutzerpunkt (217,274) / (280,275): L* max im 9x9-Fenster gegen den Median
    der Kopfhaut im Ring 25x25 ohne 9x9 (nur Pixel von Kopf 22). Rueckgabe Liste [(max, ring_median)]."""
    sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
    import v4f
    v4f._KC.pop(v, None)
    sauber, K = v4f.szene_id(v, 2)
    L = v4f.lab(sauber)[..., 0]
    out = []
    for (x, y) in ((217, 274), (280, 275)):
        win = L[y - 4:y + 5, x - 4:x + 5]
        ring = np.ones((25, 25), bool)
        ring[8:17, 8:17] = False
        sub = L[y - 12:y + 13, x - 12:x + 13]
        kopf = K['mesh'][y - 12:y + 13, x - 12:x + 13] == 22
        r = sub[ring & kopf]
        out.append((float(win.max()), float(np.median(r)) if len(r) else 0.0))
    return out


def auge_bisektion(lo, hi, schritte):
    """auge.f (O4): kleinster Faktor, mit dem an BEIDEN Nutzerpunkten in Bild 2 (Licht der Szene) das Augenmaximum >= 2 x Median der
    Kopfhaut daneben ist ('sichtbar heller'; das Video zeigt etwa 30 gegen 7, also 4x - die Haelfte davon, keine Eichung am Video)."""
    par = lade_par()
    proto = []

    def mess(fv):
        p = json.loads(json.dumps(par))
        p['textur']['auge']['f'] = fv
        v = os.path.join(SCR, 'bis', 'auge')
        shutil.rmtree(v, ignore_errors=True)
        bauen(p, v, log=lambda *a: None)
        a = auge_mass(v)
        proto.append([fv] + [round(x, 2) for t in a for x in t])
        log('  f_auge %.4f -> %s' % (fv, a))
        return all(m >= 2.0 * r for m, r in a)
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
        'O4: kleinster Faktor (multiplikativ auf die hellen G4-Augeneintraege, Knie wie die Haut), mit dem in Bild 2 (Buehne, Licht der Szene, '
        'ohne Videofilter) an beiden Nutzerpunkten (217,274)/(280,275) das L*-Maximum im 9x9-Fenster >= 2 x Median der Kopfhaut (Teil 22) im '
        'Ring 25x25 ohne 9x9 ist; Bisektion kf_k5g3.py --auge [%s, %s], %d Schritte. Protokoll [f, max1, ring1, max2, ring2]: %s'
        % (lo, hi, schritte, proto))
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
