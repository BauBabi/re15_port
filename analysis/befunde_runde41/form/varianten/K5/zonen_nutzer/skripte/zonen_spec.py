from zonen_bau import *


def Z(name, bild, datei, text, f_, bau, **kw):
    return dict(name=name, bild=bild, datei=datei, text=text, farbe=f_, bau=bau, kw=kw)


SPEC = [
    # ---- Bild 1, Rueckseite ----
    Z('oberkoerper_hinten', 1, 'Rücken.png', 'Oberkörper', 'rot', 'flut', waende=['rot'], saaten=[(400, 200)], dil=7),
    Z('wunde_hinten', 1, 'Rücken.png', 'fleischige Wunde/Haut', 'grau', 'flut', waende=['rot', 'grau', 'gruen'], saaten=[(290, 110)]),
    Z('musterung_rechts_hinten', 1, 'Rücken.png', 'Oberkörper Musterung rechts', 'gruen', 'flut', waende=['rot', 'grau', 'gruen'], saaten=[(290, 165)]),
    Z('musterung_links_hinten', 1, 'Rücken.png', 'Oberkörper Musterung links', 'gruen', 'huelle', farbe='gruen', box=(129, 118, 152, 150)),
    Z('kopf_hinten', 1, 'Kopf_von_hinten.png', 'Kopf', 'rot', 'flut', waende=['rot'], saaten=[(480, 180)], dil=7),
    Z('auge_hinten', 1, 'Kopf_von_hinten.png', 'wahrscheinlich etwas Auge', 'gruen', 'huelle', farbe='gruen', box=(505, 190, 520, 206)),
    Z('mund_hinten', 1, 'Kopf_von_hinten.png', 'Mund?', 'lila', 'huelle', farbe='lila', box=(494, 200, 513, 216)),
    Z('schwanz', 1, 'Schwanz_hinten.png', 'Schwanz der in den Rücken übergeht', 'rot', 'flut', waende=['rot', 'blau'], saaten=[(620, 120), (612, 300)]),
    Z('schwanz_muster', 1, 'Schwanz_hinten.png', 'Schwanz Muster / Maserung', 'gruen', 'huelle_n',
      teile=[(580, 180, 596, 222), (605, 175, 650, 231), (589, 329, 636, 443), (629, 445, 652, 470)]),
    Z('zipfel', 1, 'Schwanz_hinten.png', 'Zipfel - separater Schwanzabschnitt am Schluss', 'blau', 'flut', waende=['blau', 'rot'], saaten=[(612, 540)],
      box=(480, 440, 660, 597)),
    Z('zipfel_musterung', 1, 'Schwanz_hinten.png', 'Schwanz Musterung', 'orange', 'huelle_n', farbe_n='orange', teile=[(586, 482, 641, 531), (546, 515, 634, 566)]),
    Z('rotes_fleisch_zipfel', 1, 'Schwanz_hinten.png', 'verschieden farbiges rotes Fleisch wie oben am Becken', 'lila', 'punkte',
      pts=[(642, 458), (632, 496), (624, 530), (618, 575)], r=8),
    Z('bein_hinten_links', 1, 'linkes_bein_hinten.png', 'linkes Bein', 'rot', 'flut', waende=['rot'], saaten=[(345, 290), (320, 380)],
      sehnen=[((364, 134), (370, 200)), ((370, 200), (372, 330)), ((372, 330), (379, 358))], box=(260, 125, 385, 450)),
    Z('weisse_musterung_bein_hinten_links', 1, 'linkes_bein_hinten.png', 'weiße Musterung', 'gruen', 'punkte', pts=[(352, 209)], r=8),
    Z('braeunliche_musterung_bein_hinten_links', 1, 'linkes_bein_hinten.png', 'bräunliche Musterung', 'dunkelrot', 'punkte', pts=[(340, 244)], r=8),
    Z('bein_hinten_rechts', 1, 'rechtes_bein_hinten.png', 'rechtes Bein hinten', 'rot', 'flut', waende=['rot'], saaten=[(300, 300), (290, 420)],
      box=(228, 160, 400, 478)),
    Z('weisse_musterung_bein_hinten_rechts', 1, 'rechtes_bein_hinten.png', 'weiße Musterung', 'gruen', 'punkte', pts=[(315, 219)], r=8),
    Z('braeunliche_musterung_bein_hinten_rechts', 1, 'rechtes_bein_hinten.png', 'bräunliche Musterung', 'dunkelrot', 'band_punkte',
      box=(276, 222, 335, 241), r=3, pts=[(303, 242)]),
    Z('fuss_hinten_links', 1, 'linker_rechter_Fuß_hinten.png', 'linker Fuß mit Teil der Fußkralle vorne', 'rot', 'huelle', farbe='rot',
      box=(455, 375, 545, 500)),
    Z('fusskrallen_hinten_links', 1, 'linker_rechter_Fuß_hinten.png', 'Teil der Fußkralle vorne (wahrscheinlich 3 Krallen)', 'rot', 'band',
      farbe='rot', box=(510, 378, 545, 460), r=3),
    Z('vorderbein_rechts', 1, 'Bein_Vorne_rechts.png', 'rechtes Bein und Fuß', 'rot', 'flut', waende=['rot', 'hellblau'],
      saaten=[(450, 300), (430, 375)], box=(395, 185, 520, 395)),
    Z('weisse_musterung_vorderbein_rechts', 1, 'Bein_Vorne_rechts.png', 'weiße Musterung', 'gruen', 'flut', waende=['rot', 'gruen'],
      saaten=[(440, 210)], box=(395, 185, 520, 240)),
    Z('fuss_kralle_vorderbein_rechts', 1, 'Bein_Vorne_rechts.png', 'Fuß und Kralle', 'hellblau', 'loecher', farbe='hellblau', box=(470, 340, 512, 380)),
    # ---- Bild 2, Vorderseite ----
    Z('oberkoerper_vorn', 2, 'Vorne_Oberkörper.png', 'Oberkörper', 'rot', 'flut', waende=['rot'], saaten=[(250, 150)]),
    Z('fleisch_vorn', 2, 'Vorne_Oberkörper.png', 'großer fleischfarbener Teil', 'gruen', 'flut', waende=['rot', 'gruen'], saaten=[(300, 150), (340, 112)]),
    Z('gesicht_vorn', 2, 'Gesicht vorne.png', 'Vorderseite Gesicht mit Maul nach unten rechts blickend', 'rot', 'flut', waende=['rot', 'hellblau'], saaten=[(520, 300), (600, 440)], dil=5),
    Z('maul_vorn', 2, 'Gesicht vorne.png', 'geöffnetes Maul', 'hellblau', 'flut', waende=['hellblau'], saaten=[(600, 440)], box=(510, 325, 700, 520)),
    Z('unterkiefer_vorn', 2, 'Gesicht vorne.png', 'Teil des Unterkiefers', 'lila', 'flut_band', waende=['rot', 'hellblau'], saaten=[(650, 415)],
      box=(595, 330, 705, 500), band_farbe='lila', band_box=(600, 350, 700, 480), band_r=9),
    Z('augen_vorn', 2, 'Gesicht vorne.png', 'Augen', 'gruen', 'punkte', pts=[(441, 245), (568, 248)], r=5),
    Z('arm_hand_krallen_vorn_rechts', 2, 'vorne_rechte_Hand_und_Kralle.png', 'vorderer rechter Arm mit Krallen', 'rot', 'flut',
      waende=['rot', 'hellblau', 'lila'], saaten=[(345, 440), (360, 520), (360, 600), (312, 592), (440, 545)], box=(290, 395, 470, 620)),
    Z('arm_vorn_rechts', 2, 'vorne_rechte_Hand_und_Kralle.png', 'vorderer rechter Arm', 'braun', 'flut', waende=['rot', 'lila'], saaten=[(345, 440)],
      box=(290, 395, 470, 620)),
    Z('hand_vorn_rechts', 2, 'vorne_rechte_Hand_und_Kralle.png', 'vordere rechte Hand mit Krallen', 'lila', 'flut', waende=['rot', 'lila', 'hellblau'],
      saaten=[(360, 520)], box=(290, 395, 470, 620)),
    Z('krallen_vorn_rechts', 2, 'vorne_rechte_Hand_und_Kralle.png', 'Krallen', 'hellblau', 'flut', waende=['rot', 'hellblau'],
      saaten=[(360, 600), (312, 592), (440, 545)], box=(290, 500, 470, 620)),
    Z('vorderbein_links', 2, 'Vorderes_Linkes_Bein_mit_fuß.png', 'Vorderes linkes Bein mit Krallen', 'rot', 'flut', waende=['rot', 'hellblau', 'lila'],
      saaten=[(420, 250), (450, 440)], box=(335, 85, 575, 550)),
    Z('beinabschnitt_vorn_links', 2, 'Vorderes_Linkes_Bein_mit_fuß.png', 'Beinabschnitt', 'grau', 'flut', waende=['rot', 'lila', 'hellblau'],
      saaten=[(424, 284)], box=(335, 85, 575, 550)),
    Z('fussabschnitt_vorn_links', 2, 'Vorderes_Linkes_Bein_mit_fuß.png', 'Fußabschnitt', 'lila', 'flut', waende=['rot', 'lila', 'hellblau'],
      saaten=[(450, 440)], box=(335, 85, 575, 550)),
    Z('krallen_vorn_links', 2, 'Vorderes_Linkes_Bein_mit_fuß.png', 'Exakt 4 Krallen', 'hellblau', 'loecher', farbe='hellblau', box=(378, 438, 572, 546)),
]


def baue(z):
    lf = z['datei']; kw = dict(z['kw']); b = z['bau']
    if b == 'flut':
        return flut(lf, **kw)
    if b == 'huelle':
        return huelle(lf, **kw)
    if b == 'loecher':
        return loecher(lf, **kw)
    if b == 'band':
        return band(lf, **kw)
    if b == 'punkte':
        return punkte(lf, kw['pts'], kw.get('r', 6))
    if b == 'huelle_n':
        m = None
        for bx in kw['teile']:
            q, _ = huelle(lf, kw.get('farbe_n', 'gruen'), bx)
            m = q if m is None else m | q
        return m, {}
    if b == 'band_punkte':
        a, _ = band(lf, z['farbe'], kw['box'], kw['r']); c, _ = punkte(lf, kw['pts'], 8)
        return a | c, {}
    if b == 'flut_band':
        a, i = flut(lf, kw['waende'], kw['saaten'], box=kw['box']); c, _ = band(lf, kw['band_farbe'], kw['band_box'], kw['band_r'])
        return a & c, i
    raise ValueError(b)
