#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 2: Mod-Fortsetzungszeilen 10D0/1150/2000 -> engine/src/gen/namen_r42_texte.inc

    python re15_port/tools/r42_namen/fortsetzung_bauen.py           (schreibt die .inc)
    python re15_port/tools/r42_namen/fortsetzung_bauen.py --check   (prueft nur)

NUTZER 18.11.2026: "Du hast bei den übernommenen Sätzen aus Resident Evil 2 nicht den Namen in den Dialog mit davor geschrieben wie
bei den rest auch...." Memory reai-v2-sprechername-vor-satz: jeder gesprochene Satz traegt den Sprechernamen, Systemtexte nicht.

Befund (Zensus scratchpad/sprechernamen/BERICHT.md Teil B): fuenf Mod-Saetze sind auf zwei RDT-Nachrichten verteilt, nur die erste
traegt den Namen, und der Schnitt liegt mitten im Satz - die Aufnahmen (synchro/, Nutzer-Produktion, je Nachricht eine Datei
synchro/STAGEn/roomXXXX/mainNN.wav, audio_pc.c Regel) trennen am Satzende:

  Raum  A/B    Sprecher  RDT A        RDT B        Aufnahme A (Whisper medium.en)            Aufnahme B
  10D0  18/19  Marvin    @0x02212     @0x02262     "...the only one left at the station."    "Tell me, are there other survivors?"
  10D0  24/25  Leon      @0x0241A     @0x02472     "...The phones are down."                 "And with the building besieged ..."
  1150  12/13  Irons     @0x016B9     @0x0170E     "...uniform or not."                      "Those things are no longer what ..."
  2000  4/5    Leon      @0x01C9A     @0x01CEB     "...If I hadn't seen her..."              "Maybe we wouldn't have found ..."
  2000  7/8    Ada       @0x01D59     @0x01DA6     "...what things are in the sewers."       "Zombies are not the only things ..."

Neu: A zeigt genau den Satz der Aufnahme A, B den der Aufnahme B, und B beginnt mit demselben Namenskopf wie A
(`04 00 05 <Farbe> <Name> 16 05 00 00`, aus A kopiert). WORTLAUT DER MOD BLEIBT Glyphe fuer Glyphe (das Werkzeug prueft es); nur
an der neuen Trennstelle:
  - das Satzzeichen am Ende von A ist das Satzende der Aufnahme (Whisper: "." bzw. "..." bei 2000 #4); ein Komma der Mod an der
    Stelle (10D0 #18 "station,", 1150 #12 "not,", 2000 #7 "sewers,") wird dazu ersetzt,
  - der erste Buchstabe von B wird gross (die Aufnahme beginnt einen neuen Satz).
  PORT-WAHL mit Zensus (alle RDTs unter shared_assets/PSX, scratchpad/namen/s2/q3_klein.py): 415 von 416 Mod-Zeilen mit Namen beginnen
  nach dem Namen gross (Ausnahme ROOM1141 msg 8 "Elza: what the hell is that?!"); mit Satzzeichen enden alle Zeilen mit Namen ausser
  genau diesen fuenf Fortsetzungs-Paaren (1150 #12 Komma, 10D0 #18/#24 und 2000 #4/#7 ohne Zeichen).
Umbruch: die Zeilenwechsel `08` der Mod bleiben, wo sie waren. Die wandernden Worte haengen an die letzte Zeile von A bzw. vor die
erste Zeile von B, wenn die Zeile dann noch <= 271 px breit ist (texte_stage3a.MAX_PX, Glyphenbreiten include/font_width.h, Zeile 1
mit dem Namenskopf); sonst bekommen sie eine eigene Zeile. Hoechstens 3 Zeilen (Stufe 2; seit Stufe 4 hoechstens 2, siehe SEITEN_S4 unten; 28 Original-Dialoge haben drei, z. B. ROOM10D0 msg 17
@0x021BD). PORT-WAHL (kein Beleg noetig, Darstellung): so sieht jede Zeile der Mod aus wie vorher, nur die Trennstelle wandert.
Eine UNVERAENDERTE Mod-Zeile darf breiter sein (271 px ist die 99-%-Grenze der ausgelieferten Zeilen, tuer1120_1130.c): ROOM10D0
msg 24 Zeile 2 "the helicopter abandoned us, the phones" misst 273 px wie im RDT (@0x0241A, steht so seit v0.8.30 im Spiel).
Schwanz `04 01 01 N`: jede Nachricht behaelt IHREN Schwanz aus dem RDT (N ist Datenbyte je Zeile, nachlauf_r42.py Punkt 4; z. B.
2000 msg 4 `01 77`, msg 5 `01 63`). Laenge <= 128 Byte (PSX MSG_RAW_LEN, msg_common.c:43) - das Werkzeug bricht sonst ab.
RDT bleibt unveraendert; die Texte setzt engine/src/namen_r42.c nach dem Laden des Nachrichtenblocks ein (msg_common.c
re15_msg_load_room_block). Die Elza-Raeume 10D1/1151/2001 haben diese Nachrichten nicht (10D1: 12 Nachrichten, alles Systemtexte bis
auf das Schild msg 10; 1151: 4, Irons msg 2 mit Namen; 2001: 15, jede mit Namen) - geprueft, nichts zu tun.
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
import texte_stage3a as T  # noqa: E402  (Glyphen, Breiten, MAX_PX, carr)
sys.path.insert(0, HIER)
import lesbar_s4 as LS  # noqa: E402  (Stufe 4: hoechstens 2 Zeilen, Seitenwechsel beim Wort der Aufnahme)

ZIEL = os.path.join(T.PORT, 'engine', 'src', 'gen', 'namen_r42_texte.inc')
PSX_LEN = 128      # PSX MSG_RAW_LEN (msg_common.c:43)
MAX_ZEILEN = LS.MAX_ZEILEN   # Stufe 4: hoechstens 2 Zeilen je Seite (vorher 3; Belege tools/r42_namen/lesbar_s4.py)

# STUFE 4 (Lesezeit und Zeilenzahl): alle Szenen laufen im Letterbox (ROOM10D0 sub21 @0x019FE, ROOM1150 sub08 @0x01118, ROOM2000 sub02
# @0x01804 `22 01 1b 01`), Zeile 3 laege auf dem Balken ab y 216 (@0x80072f06). Regel (lesbar_s4.py): Mod-Umbruch bleibt, wenn er
# hoechstens 2 Zeilen <= 271 px ohne Ein-Wort-Zeile hat; sonst neu umbrechen (eine Zeile oder die ausgeglichene Zweiteilung); passt der
# Satz auch so nicht in zwei Zeilen, dann zwei Seiten EINER Nachricht, jede mit dem Namenskopf, Seite 2 beim Wort der Aufnahme:
# (Raum, Id) -> (Woerter auf Seite 1, Einsatz des ersten Worts von Seite 2 in s, Beleg). Trennstelle = Sprechpause der Aufnahme
# (namen/s4/pausen.py, >= 120 ms unter -40 dB); bei 10D0 #24 drei Pausen, gewaehlt die, an der die kuerzere Seite am laengsten steht.
SEITEN_S4 = {
    (0x10D0, 18): (4, 2.12, 'synchro/STAGE1/room10D0/main18.wav Pause 1.37-1.90/1.94-2.12 nach "him," (Whisper "I"[1.90-2.22])'),
    (0x10D0, 24): (7, 3.34, 'synchro/STAGE1/room10D0/main24.wav Pause 2.84-3.34 nach "try," (Whisper "The"[3.32-3.48]); Pausen '
                            '1.01-1.50 / 5.05-5.29 gaeben eine Seite von 1,5 s bzw. 0,95 s'),
    (0x10D0, 25): (8, 2.76, 'synchro/STAGE1/room10D0/main25.wav Pause 2.24-2.76 nach "things," (Whisper "I"[2.62-2.86])'),
}

KOMMA, PUNKT, LEER, UMBRUCH = 0x18, 0x57, 0x00, 0x08

# (Raum, A, B, Datei-Offset A, Datei-Offset B, erste Worte von B, Satzende von A, Aufnahme A, Aufnahme B)
PAARE = (
    (0x10D0, 18, 19, 0x02212, 0x02262, "tell me,", ".",
     "You've already seen him. I thought he was the only one left at the station.", "Tell me, are there other survivors?"),
    (0x10D0, 24, 25, 0x0241A, 0x02472, "and with", ".",
     "I don't know. It remains to try. The helicopter abandoned us. The phones are down.",
     "And with the building besieged by those things, I don't see any more options."),
    (0x1150, 12, 13, 0x016B9, 0x0170E, "those things...", ".",
     "And remember this, Leon, never let your guard down, uniform or not.",
     "Those things are no longer what they were. Don't make my mistake."),
    (0x2000, 4, 5, 0x01C9A, 0x01CEB, "maybe we", "...",
     "Yes. Before meeting Ada, I saw her coming in this way. If I hadn't seen her...", "Maybe we wouldn't have found this way out."),
    (0x2000, 7, 8, 0x01D59, 0x01DA6, "zombies are", ".",
     "So there is no time to lose. We do not know what things are in the sewers.",
     "Zombies are not the only things we should worry about here."),
)


def rdt_msg(raum, mid):
    d = open(os.path.join(T.PSX, 'STAGE%X' % (raum >> 12), 'ROOM%04X.RDT' % raum), 'rb').read()
    ms = struct.unpack_from('<I', d, 0x3C)[0]
    n = struct.unpack_from('<H', d, ms)[0] // 2
    o = [struct.unpack_from('<H', d, ms + 2 * i)[0] for i in range(n)]
    a = ms + o[mid]
    b = ms + o[mid + 1] if mid + 1 < n else len(d)
    return a, list(d[a:b])


def zerlegen(m, mit_kopf):
    """`04 00 [05 F Name 16 05 00 00] <Text mit 08> 04 01 01 N` -> (Kopf, Text-Glyphen, Schwanz). Alles andere = Fehler."""
    assert m[:2] == [0x04, 0x00], m[:4]
    assert m[-4:-1] == [0x04, 0x01, 0x01] and len([x for x in m if x == 0x01]) >= 1, m[-4:]
    kopf, i = [0x04, 0x00], 2
    if mit_kopf:
        assert m[2] == 0x05
        j = m.index(0x05, 4)
        assert m[j - 1] == 0x16 and m[j + 1] == 0x00 and m[j + 2] == LEER, m[:j + 3]
        kopf, i = m[:j + 2], j + 3          # Kopf bis `05 00`, dann das Leerzeichen 00
    else:
        assert m[2] not in (0x05, 0x16), m[:4]
    text = m[i:-4]
    assert not any(x in (0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0A, 0x0B) for x in text), text
    return kopf, text, m[-4:]


def zeilen_worte(glyphen):
    """Text mit `08` -> [[Wort, ...], ...] je Mod-Zeile."""
    out, akt = [], []
    for g in glyphen + [UMBRUCH]:
        if g == UMBRUCH:
            out.append(worte(akt))
            akt = []
        else:
            akt.append(g)
    return out


def worte(glyphen):
    w, akt = [], []
    for g in glyphen:
        if g in (LEER, UMBRUCH):
            if akt:
                w.append(akt)
            akt = []
        else:
            akt.append(g)
    if akt:
        w.append(akt)
    return w


def wort_str(w):
    return ''.join(T_rev(g) for g in w)


_REV = {v: k for k, v in T.TAB.items()}
_REV[0x57] = '.'
_REV[0x61] = '"'


def T_rev(g):
    if 0x1D <= g <= 0x36:
        return chr(ord('A') + g - 0x1D)
    if 0x3D <= g <= 0x56:
        return chr(ord('a') + g - 0x3D)
    if 0x0C <= g <= 0x15:
        return chr(ord('0') + g - 0x0C)
    return _REV.get(g, '<%02x>' % g)


def breite(glyphen):
    return sum(T.WID[g] for g in glyphen)


def zeile_glyphen(ws):
    g = []
    for i, w in enumerate(ws):
        if i:
            g.append(LEER)
        g += w
    return g


def bauen(kopf, zw, schwanz):
    zeilen = [zeile_glyphen(z) for z in zw]
    b = list(kopf) + [LEER]
    for i, z in enumerate(zeilen):
        if i:
            b.append(UMBRUCH)
        b += z
    b += schwanz
    bw = [breite(kopf[4:-2]) + T.WID[LEER] + breite(zeilen[0])] + [breite(z) for z in zeilen[1:]]
    return b, [' '.join(wort_str(x) for x in worte(z)) for z in zeilen], bw


def zwei_zeilen(woerter, vorweg):
    """lesbar_s4.zeilen auf Glyphen-Woertern: eine Zeile, wenn sie passt, sonst die ausgeglichene Zweiteilung (jede Zeile >= 2 Woerter,
    <= 271 px, Zeile 1 mit dem Namenskopf); None, wenn es nicht passt."""
    if vorweg + breite(zeile_glyphen(woerter)) <= T.MAX_PX:
        return [list(woerter)]
    best = None
    for i in range(2, len(woerter) - 1):
        wa, wb = vorweg + breite(zeile_glyphen(woerter[:i])), breite(zeile_glyphen(woerter[i:]))
        if wa <= T.MAX_PX and wb <= T.MAX_PX and (best is None or max(wa, wb) < best[0]):
            best = (max(wa, wb), [list(woerter[:i]), list(woerter[i:])])
    return best[1] if best else None


def lesbar(raum, mid, ws, vorweg):
    """Stufe 4. None = Mod-Umbruch bleibt (ws). Sonst Liste der Seiten, je Seite die Zeilen (Glyphen-Woerter)."""
    bw = [vorweg + breite(zeile_glyphen(ws[0]))] + [breite(zeile_glyphen(z)) for z in ws[1:]]
    if (raum, mid) not in SEITEN_S4 and len(ws) <= MAX_ZEILEN and max(bw) <= T.MAX_PX and all(len(z) >= 2 for z in ws):
        return None
    alle = [w for z in ws for w in z]
    if (raum, mid) in SEITEN_S4:
        k = SEITEN_S4[(raum, mid)][0]
        teile = [alle[:k], alle[k:]]
    else:
        teile = [alle]
    seiten = []
    for t in teile:
        z = zwei_zeilen(t, vorweg)
        assert z, 'ROOM%04X msg %d: passt nicht in %d Zeilen - Seitenwechsel in SEITEN_S4 eintragen' % (raum, mid, MAX_ZEILEN)
        seiten.append(z)
    return seiten


def bauen_seiten(kopf, seiten, schwanz, n):
    """1 oder 2 Seiten; Seite 2 hinter `04 01 02 N 04 00` mit demselben Namenskopf (Form 6030 msg 4 Stufe 1, ROOM3060 @0x1244)."""
    b, zz, bws = [], [], []
    for si, zw in enumerate(seiten):
        teil, zeilen, bw = bauen(kopf, zw, [])
        if si:
            teil = [0x04, 0x01, 0x02, n] + teil      # kopf beginnt mit `04 00`
        b += teil
        zz.append(zeilen)
        bws.append(bw)
    return b + list(schwanz), zz, bws


def norm(ws):
    """Wortlaut-Vergleich: Kleinbuchstaben, Komma/Punkt am Wortende weg (nur die Trennstelle darf sich darin unterscheiden)."""
    out = []
    for w in ws:
        s = wort_str(w).lower().rstrip(',.')
        out.append(s)
    return out


def main():
    fehler = []
    kopf_inc = ['/* Generiert von re15_port/tools/r42_namen/fortsetzung_bauen.py - nicht von Hand aendern. Runde 42 Strang namen, Stufe 2:',
                ' * Mod-Fortsetzungszeilen am Satzende der Aufnahme geteilt, jede mit Namenskopf (Belege im Kopf des Werkzeugs).',
                ' * RDT unveraendert; eingesetzt von engine/src/namen_r42.c nach re15_msg_load_room_block. */']
    out, tab = list(kopf_inc), []
    for raum, ia, ib, oa, ob, b_anfang, ende, auf_a, auf_b in PAARE:
        a_off, ma = rdt_msg(raum, ia)
        b_off, mb = rdt_msg(raum, ib)
        if a_off != oa or b_off != ob:
            fehler.append('ROOM%04X msg %d/%d @0x%05X/@0x%05X statt @0x%05X/@0x%05X' % (raum, ia, ib, a_off, b_off, oa, ob))
            continue
        kopf, ta, sa = zerlegen(ma, True)
        _, tb, sb = zerlegen(mb, False)
        # Worte mit Herkunft (Nachricht, Mod-Zeile), damit die Zeilenwechsel `08` der Mod stehen bleiben
        flach = [('A', li, w) for li, z in enumerate(zeilen_worte(ta)) for w in z] +                 [('B', li, w) for li, z in enumerate(zeilen_worte(tb)) for w in z]
        alle = [w for _, _, w in flach]
        ziel = b_anfang.split(' ')
        k = None
        for i in range(len(alle) - len(ziel) + 1):
            if [wort_str(w).lower() for w in alle[i:i + len(ziel)]] == ziel:
                assert k is None, 'Trennstelle doppelt'
                k = i
        if k is None:
            fehler.append('ROOM%04X msg %d/%d: "%s" nicht gefunden' % (raum, ia, ib, b_anfang))
            continue
        flach = [(q, li, list(w)) for q, li, w in flach]
        teil_a, teil_b = flach[:k], flach[k:]
        while teil_a[-1][2][-1] == KOMMA:
            teil_a[-1][2].pop()
        teil_a[-1][2].extend([PUNKT] * len(ende))
        g0 = teil_b[0][2][0]
        if 0x3D <= g0 <= 0x56:
            teil_b[0][2][0] = g0 - 0x20                   # Kleinbuchstabe 0x3D.. -> Grossbuchstabe 0x1D.. (msg_common.c Glyphen)
        neu_a = [w for _, _, w in teil_a]
        neu_b = [w for _, _, w in teil_b]
        if norm(neu_a + neu_b) != norm(alle):
            fehler.append('ROOM%04X msg %d/%d: Wortlaut weicht ab' % (raum, ia, ib))
        vorweg = breite(kopf[4:-2]) + T.WID[LEER]

        def gruppen(teil):
            out = []
            for q, li, w in teil:
                if out and out[-1][0] == (q, li):
                    out[-1][1].append(w)
                else:
                    out.append(((q, li), [w]))
            return out

        def zeilen_a(teil):
            g = gruppen(teil)
            eigen = [ws for (q, _), ws in g if q == 'A']
            fremd = [w for (q, _), ws in g if q == 'B' for w in ws]
            if fremd:
                if eigen and (vorweg if len(eigen) == 1 else 0) + breite(zeile_glyphen(eigen[-1] + fremd)) <= T.MAX_PX:
                    eigen[-1] = eigen[-1] + fremd
                else:
                    eigen.append(fremd)
            return eigen

        def zeilen_b(teil):
            g = gruppen(teil)
            fremd = [w for (q, _), ws in g if q == 'A' for w in ws]
            eigen = [ws for (q, _), ws in g if q == 'B']
            if fremd:
                if eigen and vorweg + breite(zeile_glyphen(fremd + eigen[0])) <= T.MAX_PX:
                    eigen[0] = fremd + eigen[0]
                else:
                    eigen.insert(0, fremd)
            return eigen
        # Satz von A = Aufnahme A, Satz von B = Aufnahme B (Woerter ohne Gross/Klein und Satzzeichen)
        def w_auf(s):
            return [x.lower().strip(',.?!') for x in s.replace('...', ' ').split()]
        def w_neu(ws):
            return [x.lower().strip(',.?!') for x in ' '.join(wort_str(w) for w in ws).replace('...', ' ').split()]
        if w_neu(neu_a)[-3:] != w_auf(auf_a)[-3:] or w_neu(neu_b)[:2] != w_auf(auf_b)[:2]:
            fehler.append('ROOM%04X msg %d/%d: Trennstelle passt nicht zur Aufnahme (%s | %s)' % (raum, ia, ib, w_neu(neu_a)[-3:], w_neu(neu_b)[:2]))
        for mid, ws, schwanz, alt_off in ((ia, zeilen_a(teil_a), sa, oa), (ib, zeilen_b(teil_b), sb, ob)):
            seiten = lesbar(raum, mid, ws, vorweg)
            zus = ''
            if seiten is None:                                   # Mod-Umbruch bleibt
                b, zeilen, bw = bauen(kopf, ws, schwanz)
                seiten_z, seiten_w = [zeilen], [bw]
                mod_zeilen = [zeile_glyphen(z) for z in zeilen_worte(ta)[1:] + zeilen_worte(tb)[1:]]
                for i, w in enumerate(bw):
                    if w > T.MAX_PX and i > 0 and zeile_glyphen(ws[i]) in mod_zeilen:
                        print('Hinweis: ROOM%04X msg %d Zeile %d = %d px > %d ist eine unveraenderte Mod-Zeile (bleibt)' % (
                            raum, mid, i + 1, w, T.MAX_PX))
                        bw = bw[:i] + [0] + bw[i + 1:]
                seiten_w = [bw]
            else:                                                # Stufe 4: neu umbrochen bzw. zwei Seiten
                nw = 0
                if len(seiten) > 1:
                    k, t_s, beleg = SEITEN_S4[(raum, mid)]
                    nw = LS.seitenwartezeit(t_s, 0)
                    zus = '; Stufe 4: zwei Seiten, Seite 2 nach `02 %d` beim Einsatz t=%.2f s: %s' % (nw, t_s, beleg)
                else:
                    zus = '; Stufe 4: neu umbrochen (Mod-Umbruch hatte 3 Zeilen bzw. eine Zeile mit einem Wort)'
                b, seiten_z, seiten_w = bauen_seiten(kopf, seiten, schwanz, nw)
            for si, (zs, ww) in enumerate(zip(seiten_z, seiten_w)):
                for i, w in enumerate(ww):
                    if w > T.MAX_PX:
                        fehler.append('ROOM%04X msg %d Seite %d Zeile %d = %d px > %d' % (raum, mid, si + 1, i + 1, w, T.MAX_PX))
                if len(zs) > MAX_ZEILEN:
                    fehler.append('ROOM%04X msg %d Seite %d: %d Zeilen > %d' % (raum, mid, si + 1, len(zs), MAX_ZEILEN))
                if any(len(z.split(' ')) < 2 for z in zs):
                    fehler.append('ROOM%04X msg %d Seite %d: Zeile mit nur einem Wort' % (raum, mid, si + 1))
            name = wort_str(kopf[4:-3])
            zeilen = [' / '.join(z) for z in seiten_z]
            bw = [x for y in seiten_w for x in y]
            trenn = ' // '
            if len(b) > PSX_LEN:
                fehler.append('ROOM%04X msg %d: %d Byte > %d (PSX MSG_RAW_LEN)' % (raum, mid, len(b), PSX_LEN))
            n = 'k_nm_%04x_msg%d' % (raum, mid)
            out += T.carr(n, b, 'ROOM%04X %d %s: %s  (%d B, Zeilen %s px; RDT-Fassung @0x%05X, Schwanz `%s` aus dem RDT%s)' % (
                raum, mid, name, trenn.join(zeilen), len(b), '/'.join(map(str, bw)), alt_off, ' '.join('%02x' % x for x in schwanz),
                zus))
            tab.append((raum, mid, n))
            print('ROOM%04X msg %2d  %3d B  %s px  %s: %s' % (raum, mid, len(b), '/'.join(map(str, bw)), name, trenn.join(zeilen)))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } nm_text_t;')
    out.append('static const nm_text_t k_nm_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define NM_TEXTE_N ((int)(sizeof k_nm_texte / sizeof k_nm_texte[0]))')
    if fehler:
        sys.exit('\n'.join(fehler))
    text = '\n'.join(out) + '\n'
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    with open(ZIEL, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)
    print('geschrieben:', ZIEL)


if __name__ == '__main__':
    main()
