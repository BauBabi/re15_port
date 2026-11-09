"""zonen_v2.json zusammenstellen (Rohzonen + Sicherheit/Aenderung + Krallen + Augen + Kopf). Aufruf: python g_ausgabe.py [ziel.json]"""
import json, sys
import numpy as np
from g_basis import *
from g_meta import META, KRALLE_ZUORDNUNG

ROH = json.load(open(ARBEIT + '/zonen_v2_roh.json', encoding='utf-8'))
AK = json.load(open(ARBEIT + '/augen_kopf.json', encoding='utf-8'))
KS = json.load(open(ARBEIT + '/krallen_spitzen.json', encoding='utf-8'))
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
R2 = dict(np.load(ARBEIT + '/render_asset_2.npz'))
RK = dict(np.load(ARBEIT + '/render_k5c_2.npz'))
ka = kand(ASSET)
ALLE = alle_flaechen(ka.mo)
S2 = s_t(2)[0] ** 2
STELLE = ('fleck', 'fleck_n', 'wachsen', 'wachsen_n')


def mesh_flaechen(bs):
    return [f for f in ALLE if int(f.split('|')[0]) in bs]


def zone(e):
    m = e['modell']
    si, grund, aend = META.get(e['id'], (None, None, None))
    if si is None and e['bau'] == 'abschnitt':
        if e['eltern'] == 'vorderbein_rechts':
            si, grund, aend = 'unsicher', 'Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien', ''
        else:
            si, grund, aend = 'sicher', 'Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet', ''
    z = dict(nr=e['nr'], id=e['id'], bild=e['bild'], nutzerbild=e['nutzerbild'], nutzerwort=e['nutzerwort'], linienfarbe=e.get('linienfarbe'),
             bau=e['bau'], material=e['material'], eltern=e.get('eltern'), auftrag=e.get('auftrag'),
             sicherheit=si, sicherheit_grund=grund, aenderung_v1=aend)
    if 'asset' in m:  # Schleifen
        b = KRALLE_ZUORDNUNG[e['id']]
        z['zone_px_vorlage'] = m['asset']['zone_px_vorlage']
        z['modell_zuordnung'] = dict(regel='Kralle nach Spitzenabstand (K5c-Geometrie, Pose Bild 2)', je_mesh={str(x): len(mesh_flaechen([x])) for x in b},
                                     flaechen=len(mesh_flaechen(b)), sichtbar=None, liste=mesh_flaechen(b))
        z['modell_in_der_stelle'] = dict(asset={k: v['px_vorlage'] for k, v in m['asset']['meshes_in_zone'].items()},
                                         k5c={k: v['px_vorlage'] for k, v in m['k5c']['meshes_in_zone'].items()})
        nr = int(e['id'][-1])
        z['spitze'] = KS['vorn_links']['schleife%d' % nr]
        MS[e['id'] + '__modell'] = np.isin(R2['mesh'], b) & R2['maske']
        return z
    z['zone_px_vorlage'] = m['zone_px_vorlage']
    zu = m['zugeordnet']
    z['modell_zuordnung'] = dict(regel=('Kern der Stelle (1 px abgetragen), Flächen mit >= 10 % davon' if e['bau'] in STELLE else
                                        'Modellbedeutung' if e['bau'] in ('modellbedeutung',) or e['id'] in ('gesicht_vorn', 'kopf_hinten', 'maul_vorn', 'unterkiefer_vorn')
                                        else 'Zehenspitzen nach Geometrie' if e['bau'] == 'zehen' else
                                        'Teile des benannten Teils; Fläche >= 50 % in der Zone oder >= 15 % der Modellpixel der Zone'),
                                 **{k: zu[k] for k in ('flaechen', 'sichtbar', 'px_sichtbar_vorlage', 'je_mesh', 'liste')})
    z['messung'] = {k: m[k] for k in ('modell_px_in_zone_vorlage', 'deckung_modell', 'iou_zu_umrandung', 'anteil_zone_von_flaechen_gedeckt',
                                      'anteil_flaechenpixel_in_zone') if k in m}
    z['meshes_in_der_stelle'] = {k: v['px_vorlage'] for k, v in m['meshes_in_zone'].items()}
    z['fremd_im_umriss'] = {k: v['px_vorlage'] for k, v in m['fremd_im_umriss'].items()}
    for k in ('nach_bildlage', 'hinter_dem_modellkopf', 'kopf_im_umriss_px_vorlage', 'nutzermarken_auf_modell', 'in_nutzerzone_px_vorlage', 'iou_zur_nutzerzone'):
        if k in m:
            z[k] = m[k]
    if e['bau'] in STELLE:
        z['stellen'] = e.get('bau_info', {}).get('stellen')
        z['teilflaeche'] = m.get('anteil_flaechenpixel_in_zone', 1) < 0.5
    if 'ganzteil' in m:
        g = m['ganzteil']
        z['ganzes_teil'] = dict(meshes=g['meshes'], teile=g['teile'], flaechen_gesamt=g['flaechen_gesamt'],
                                davon_in_diesem_bild_sichtbar=g['davon_in_diesem_bild_sichtbar'], rest_material='Haut (nicht belegt)',
                                flaechen_liste=mesh_flaechen(g['meshes']))
    return z


def main(ziel):
    zonen = [zone(e) for e in ROH]
    # Krallenformen vorn rechts (eigene Zonen)
    for k, (name, wort) in enumerate((('form1', 'Krallen (Form 1, rechts im Bild)'), ('form3', 'Krallen (Form 3, links im Bild)'),
                                      ('dazwischen', 'Krallen (Fläche zwischen den Formen, mit Schriftzug)')), start=1):
        zid = 'krallen_vorn_rechts_' + name; b = KRALLE_ZUORDNUNG[zid]; mg = MS[zid]; e = KS['vorn_rechts'][name]
        si = {'form1': ('sicher', 'Form deckt Kralle 31 (Asset sichtbar); unteres Ende %.1f px neben der Spitze von 31' % e.get('abstand_unteres_ende_zu_spitze_k5c_px', 0), 'neu (eigene Zone)'),
              'form3': ('unsicher', 'Form liegt auf Kralle 19 in K5c-Form; deren Spitze liegt 42 px tiefer als Ihr Strichende; im Asset ist 19 eingezogen', 'neu (eigene Zone)'),
              'dazwischen': ('verdeckt', 'Annahme nach Ihrem Wort "wahrscheinlich auch 4 ... wegen der Schrift": Krallen 20 und 29 liegen hier verdeckt', 'neu (eigene Zone)')}[name]
        MS[zid + '__modell'] = np.isin(R2['mesh'], b) & R2['maske']
        zonen.append(dict(nr='35.%d' % k, id=zid, bild=2, nutzerbild='vorne_rechte_Hand_und_Kralle.png', nutzerwort=wort, linienfarbe='hellblau', bau='krallenform',
                          material='Kralle', eltern='krallen_vorn_rechts', sicherheit=si[0], sicherheit_grund=si[1], aenderung_v1=si[2],
                          zone_px_vorlage=e['px_vorlage'],
                          modell_zuordnung=dict(regel='Kralle nach Lage (K5c-Geometrie, Pose Bild 2)', je_mesh={str(x): len(mesh_flaechen([x])) for x in b},
                                                flaechen=len(mesh_flaechen(b)), sichtbar=None, liste=mesh_flaechen(b)),
                          modell_in_der_stelle=dict(asset={str(x): round(float(((R2['mesh'] == x) & mg).sum() * S2)) for x in (18, 19, 20, 29, 31)},
                                                    k5c={str(x): round(float(((RK['mesh'] == x) & mg).sum() * S2)) for x in (18, 19, 20, 29, 31)}),
                          form=e))
    zonen.sort(key=lambda z: [int(x) for x in z['nr'].split('.')])
    np.savez_compressed(ARBEIT + '/zonen_v2_masken.npz', **MS)
    zaehl = {}
    for z in zonen:
        zaehl[z['sicherheit']] = zaehl.get(z['sicherheit'], 0) + 1
    meta = dict(erstellt='2026-11-09', aufgabe='Grundlage v2 (Korrektor Runde 1): erkannte Zuordnung zur Abnahme durch den Nutzer; keine Textur, kein Asset, keine Farbwahl',
                modell=dict(emd='re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD', emd_md5='6e00f105f6ea854b521e23fa1f4b9d5c',
                            stand='K5e_f1 (unveraendert)', vergleich_k5c='analysis/befunde_runde41/form/stufe3/basis_k5c (nur Krallen 19/20/28/29 anders)'),
                posen={'1': 'Clip 0 Bild 11, Lage -18564/0/-18386, Gier 80', '2': 'Clip 12 Bild 0, Lage 941/0/-23776, Gier 3874'},
                abbildung={str(n): dict(formel='Vorlage = s * Grundlage + t (Pixelmitte ganzzahlig)', s=s_t(n)[0], t=list(s_t(n)[1])) for n in (1, 2)},
                kennungs_render='v4f.dreiecke_id/rastern_id mit Kamera-Matrix x (Vorlage -> Grundlage); Gegenprobe Silhouette gegen Vorlage-Render IoU 0,9956 (Bild 1) / 0,978 (Bild 2, Rest = Modellteile links ausserhalb des Grundlage-Rahmens)',
                pixelzahlen='px_vorlage = Grundlage-Pixel x s^2 (Vorlage-Massstab wie v1)',
                flaechen_schluessel='mesh|viereck|index (viereck 0 = Dreieck, 1 = Viereck) wie ruhe/messung/material.json',
                seiten='aus Sicht des Wesens; Ausnahme "Maul nach unten rechts blickend" = Bildrichtung',
                material_regel='Material NUR aus dem Nutzerwort; Teilnamen ohne Farbwort = "Haut (Teil benannt, kein Farbwort)"; was im Video nicht sichtbar ist = "Haut (nicht belegt)"',
                sicherheit_zaehlung=zaehl, zonen_zahl=len(zonen))
    out = dict(meta=meta, zonen=zonen, augen=AK['augen'], kopf=AK['kopf'], krallen=dict(spitzen=KS))
    json.dump(out, open(ziel, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('Zonen', len(zonen), zaehl)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/zonen_v2.json')
