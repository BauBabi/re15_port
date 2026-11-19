#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 6 + 7: Fingerabdruck der 13 VERALTETEN Aufnahmen (ANDERER Satz unter neuem Untertitel).

    python re15_port/tools/r42_namen/altaufnahmen_bauen.py           (schreibt die beiden erzeugten Tabellen)
    python re15_port/tools/r42_namen/altaufnahmen_bauen.py --check   (prueft nur: Tabellen bytegleich zum Stand des Tags)

WAS: Die Story v0.8.31 gibt 15 Saetzen einen neuen Text (tools/r42_story/texte_stage3a.py, texte_stage3b.py, texte_stage6.py), die
Datei synchro/STAGEn/roomXXXX/mainNN.wav traegt aber noch die Aufnahme des ALTEN Satzes - in 6030 sogar mit anderem Sprecher
(AUFNAHMELISTE "Warum diese 15 Saetze zuerst?"). PORT-WAHL (Entscheid 19.11.2026; synchro/ ist Eigenproduktion des Projekts, kein
Original-Verhalten - RE1.5 hat keine englische Sprachausgabe): eine Datei, die nachweislich noch die alte Aufnahme ist, gilt wie
eine FEHLENDE Datei. Der Satz laeuft stumm mit Untertitel und Textdauer; die neue Aufnahme des Nutzers spielt von selbst, weil
sie einen anderen Fingerabdruck hat.

STUFE 7 (Nachtrag, Entscheid des Orchestrators 19.11.2026, PORT-WAHL; Pruefbefund Stufe 6 gelb 2): "stumm bleibt nur, was einen
ANDEREN Satz spricht; eine Aufnahme, die den neuen Satz woertlich oder bis auf ein Wort spricht, spielt." Zwei der 15 Aufnahmen
fallen darunter und stehen NICHT mehr in der Tabelle (FAST_WOERTLICH unten): 3020 main02 spricht "Leon, give me a hand." (genau
der Untertitel) und 6030 main21 "... We made it down here ..." (Untertitel "... We came down here ..."). Die Regel ist hier
messbar gefasst (fast_woertlich()) und wird beim Bauen an JEDEM Satz geprueft, mit dem abgehoerten Wortlaut (Spalte "gehoert",
Spracherkenner Whisper small.en / medium.en / large-v3-turbo am 19.11.2026, alle drei gleich bis auf Satzzeichen).

QUELLE DES FINGERABDRUCKS: der Stand des Tags v0.8.30 (`git show v0.8.30:synchro/...`), NICHT der Arbeitsbaum - sonst wuerde
eine schon ersetzte Datei wieder als "alt" eingetragen. Je Datei: Groesse in Byte + FNV-1a 64 bit ueber den ganzen Inhalt
(Startwert 0xcbf29ce484222325, Faktor 0x100000001b3). Dazu fuer die Integrationstests SHA-256 (cmake file(SHA256), zweite,
unabhaengige Pruefsumme). synchro/ wird nur gelesen.

AUSGABE:
    engine/src/gen/altaufnahme_r42_tabelle.inc      Tabelle fuer engine/src/altaufnahme_r42.c
    tests/integration/altaufnahmen_r42.cmake        SHA-256 je Datei + Funktion re15_altaufnahme_stand()
    tests/unit/gen_r42_namen_altsaetze.inc          alle 15 Saetze mit Wortlaut (gehoert / Untertitel) und Abdruck fuer die Riegel
"""
import hashlib
import os
import re
import subprocess
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
REPO = os.path.normpath(os.path.join(PORT, '..'))
TAG = 'v0.8.30'

# (Raum, Dateinummer, alter Sprecher, alter Satz laut Skript, GEHOERT, neuer Sprecher, neuer Satz)
#   alter Satz / neuer Satz: AUFNAHMELISTE (reAi_v2_entscheidung_2026-11-06/11_story_v0831/AUFNAHMELISTE.md, Spalte "Art": neu
#   aufnehmen (alt: ...)); der neue Satz steht in engine/src/gen/story3a_r42_texte.inc, story3b_r42_texte.inc, story6_r42_texte.inc.
#   GEHOERT: was die Datei wirklich spricht - Spracherkenner am Stand des Tags, 19.11.2026 (Whisper medium.en; small.en und
#   large-v3-turbo gleich bis auf Satzzeichen). Die Regel fast_woertlich() misst GEHOERT gegen den neuen Satz.
# Reihenfolge = Raum, Nummer. SAETZE = stumm (anderer Satz), FAST_WOERTLICH = spielt (Stufe 7).
SAETZE = (
    (0x3020, 0, 'Leon', 'Dammit! the door does not open!', 'Damn it! The door does not open!',
     'Leon', "Damn it! It won't budge."),
    (0x3020, 1, 'Marvin', "There's an open window, maybe we could enter through there?",
     "There's an open window. Maybe we could enter through there.",
     'Marvin', "There's an open window up there. We could get in that way."),
    (0x30E0, 3, 'Leon', "Y-yeah, Marvin is out in the hall. He can't get out because of all the monsters outside...",
     "Yeah, Marvin is out in the hall. He can't get out because of all the monsters outside.",
     'Leon', "He's back in the lobby. Some dogs got his leg... he can barely walk."),
    (0x30E0, 10, 'Leon', 'That wound... Did those things get you? How careless...',
     'That wound. Did those things get you? How careless.', 'Leon', "That's a bite... They got you?"),
    (0x30E0, 11, 'Marvin', 'The hall is no good...! Those things broke through the main gate and got inside!',
     'The hall is no good. Those things broke through the main gate and got inside.',
     'Marvin', "The lobby's gone...! They smashed through the front glass and got inside!"),
    (0x30E0, 13, 'Marvin', "The entire place is overrun! And if they're already all the way out here... the surface might just "
     'be done for...', "The entire place is overrun, and if they're already all the way out here, the surface might just be "
     'done for.', 'Marvin', "There's more of them than I thought... If they've come this far, the city's finished."),
    (0x6030, 2, 'Sherry', "She's out there fighting the monsters!", "She's out there fighting the monsters!",
     'Leon', 'Where is she now?'),
    (0x6030, 3, 'John', "She's trying to keep the outside clear so we can escape.",
     "She's trying to keep the outside clear so we can escape.",
     'John', 'We got split up in the fire. One second she was right behind us...'),
    (0x6030, 4, 'Leon', "Why aren't you out there?", "Why aren't you out there?",
     'Leon', "If she's as tough as you say, she'll find another way out."),
    (0x6030, 5, 'John', 'She told me to stay with the kid... and look after your friend here.',
     'She told me to stay with the kid and look after your friend here.',
     'John', "Then I'm staying with the kid. And I'll keep an eye on your friend here."),
    (0x6030, 22, 'John', "She said she'd catch up with us.", "She said she'd catch up with us.",
     'Sherry', "Elza's still out there...!"),
    (0x6031, 1, 'Sherry', "A policeman! He's fighting off monsters. He wants us to start the train.",
     "A policeman. He's fighting off monsters. He wants us to start the train.",
     'Sherry', 'A policeman brought Marvin down here. Then he went back into the fire.'),
    (0x6031, 2, 'Elza', "I'll do it!", "I'll do it.", 'Elza', 'Back? Why?'),
)

# Stufe 7 (Entscheid 19.11.2026, PORT-WAHL): diese Aufnahmen sprechen den neuen Satz woertlich oder bis auf ein Wort - sie
# stehen NICHT in der Tabelle und SPIELEN. Gleiche Spalten wie SAETZE.
#   3020 main02: geaendert wurde im Text nur "an hand" -> "a hand"; die Aufnahme spricht "a hand" (erzwungene Dekodierung,
#                Whisper medium.en: "Leon, give me a hand." -0,54 gegen "... an hand." -12,72) -> gehoert = Untertitel.
#   6030 main21: gesprochen "made it down here", Untertitel "came down here": EIN Wort des Untertitels ("came") wird nicht
#                gesprochen, die Aufnahme sagt dafuer zwei ("made it"). Bleibt auf der Liste der Neuaufnahmen (AUFNAHMELISTE).
FAST_WOERTLICH = (
    (0x3020, 2, 'Marvin', 'Leon give me an hand!', 'Leon, give me a hand.', 'Marvin', 'Leon, give me a hand!'),
    (0x6030, 21, 'John', "Name's John. This is Sherry. We made it down here with a girl named Elza.",
     "Name's John. This is Sherry. We made it down here with a girl named Elza.",
     'John', "Name's John. This is Sherry. We came down here with a girl named Elza."),
)

# REGEL "woertlich oder bis auf ein Wort" (Entscheid 19.11.2026), messbar gefasst - Woerter klein, ohne Satzzeichen, in ihrer
# Reihenfolge verglichen (laengste gemeinsame Wortfolge L):
#   (a) hoechstens REGEL_UNTERTITEL_FREMD = 1 Wort des Untertitels wird nicht gesprochen   (Woerter Untertitel - L <= 1)
#   (b) die Aufnahme sagt hoechstens REGEL_AUFNAHME_FREMD = 2 Woerter, die nicht im Untertitel stehen (Woerter Aufnahme - L <= 2)
# Die 1 ist der Wortlaut des Entscheids ("bis auf ein Wort"). Die 2 ist der entschiedene Fall 6030 main21 ("came down here" gegen
# "made it down here", im Entscheid woertlich genannt): ein Untertitelwort gegen zwei gesprochene. Abstand der 13 stummen Saetze:
# bei jedem fehlen mindestens 2 Woerter des Untertitels in der Aufnahme (6031 main02: 2 von 2; 3020 main00: 3 von 5; sonst >= 4).
REGEL_UNTERTITEL_FREMD = 1
REGEL_AUFNAHME_FREMD = 2


def woerter(satz):
    return re.findall(r"[a-z0-9]+(?:'[a-z0-9]+)*", satz.lower())


def wortmass(gehoert, untertitel):
    """-> (Woerter Untertitel, davon nicht gesprochen, Woerter Aufnahme, davon nicht im Untertitel)"""
    a, u = woerter(gehoert), woerter(untertitel)
    d = [[0] * (len(u) + 1) for _ in range(len(a) + 1)]
    for i in range(len(a)):
        for j in range(len(u)):
            d[i + 1][j + 1] = d[i][j] + 1 if a[i] == u[j] else max(d[i][j + 1], d[i + 1][j])
    gemeinsam = d[len(a)][len(u)]
    return len(u), len(u) - gemeinsam, len(a), len(a) - gemeinsam


def fast_woertlich(gehoert, untertitel):
    _, u_fremd, _, a_fremd = wortmass(gehoert, untertitel)
    return u_fremd <= REGEL_UNTERTITEL_FREMD and a_fremd <= REGEL_AUFNAHME_FREMD


def pfad(raum, nr):
    """Ablage-Regel synchro/README.md / audio_pc.c re15_voice_load_clip: synchro/STAGE<n>/room<RAUM 4 Hex>/main<NN>.wav"""
    return 'synchro/STAGE%u/room%04X/main%02d.wav' % (raum >> 12, raum, nr)


def git(*arg):
    return subprocess.run(('git', '-C', REPO) + arg, check=True, stdout=subprocess.PIPE).stdout


def fnv1a64(b):
    h = 0xcbf29ce484222325
    for x in b:
        h = ((h ^ x) * 0x100000001b3) & 0xFFFFFFFFFFFFFFFF
    return h


def messen():
    tag_commit = git('rev-parse', TAG + '^{commit}').decode().strip()
    zeilen = []
    alle = sorted([z + (True,) for z in SAETZE] + [z + (False,) for z in FAST_WOERTLICH])
    for raum, nr, spr_alt, alt, gehoert, spr_neu, neu, stumm in alle:
        # Regel Stufe 7 an jedem Satz: stumm <=> die Aufnahme spricht einen ANDEREN Satz
        if fast_woertlich(gehoert, neu) == stumm:
            raise SystemExit('REGEL VERLETZT: ROOM%04X main%02d ist als %s eingetragen, die Aufnahme spricht den Untertitel aber %s '
                             '(Mass %s)' % (raum, nr, 'stumm' if stumm else 'spielt', 'fast woertlich' if stumm else 'NICHT',
                                            wortmass(gehoert, neu)))
        p = pfad(raum, nr)
        blob = git('rev-parse', '%s:%s' % (TAG, p)).decode().strip()
        b = git('show', '%s:%s' % (TAG, p))
        assert b[:4] == b'RIFF' and b[8:12] == b'WAVE', p
        jetzt = None
        lokal = os.path.join(REPO, p.replace('/', os.sep))
        if os.path.isfile(lokal):
            with open(lokal, 'rb') as f:
                jetzt = f.read() == b
        zeilen.append(dict(raum=raum, nr=nr, pfad=p, blob=blob, n=len(b), fnv=fnv1a64(b), sha=hashlib.sha256(b).hexdigest(),
                           spr_alt=spr_alt, alt=alt, gehoert=gehoert, spr_neu=spr_neu, neu=neu, jetzt=jetzt, stumm=stumm,
                           mass=wortmass(gehoert, neu)))
    return tag_commit, zeilen


def c_text(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('*/', '* /')


def inc_bauen(tag_commit, zeilen):
    stumm = [z for z in zeilen if z['stumm']]
    spielt = [z for z in zeilen if not z['stumm']]
    o = []
    o.append('/* ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.')
    o.append(' * Fingerabdruck der %d veralteten Aufnahmen: Stand des Tags %s (Commit %s), `git show %s:<Pfad>`.' %
             (len(stumm), TAG, tag_commit[:12], TAG))
    o.append(' * Groesse in Byte + FNV-1a 64 bit ueber die ganze Datei (Startwert 0xcbf29ce484222325, Faktor 0x100000001b3).')
    o.append(' * Pfad klein geschrieben (Vergleich ohne Gross/Klein, audio_pc.c probiert room30E0 und room30e0).')
    o.append(' * In der Tabelle steht nur, was einen ANDEREN Satz spricht als sein Untertitel (Stufe 7, Entscheid 19.11.2026). */')
    for z in stumm:
        o.append('/* ROOM%04X msg %d: Aufnahme = %s: "%s"  ->  Untertitel v0.8.31 = %s: "%s"%s' %
                 (z['raum'], z['nr'], z['spr_alt'], c_text(z['alt']), z['spr_neu'], c_text(z['neu']),
                  '  (SPRECHERWECHSEL)' if z['spr_alt'] != z['spr_neu'] else ''))
        o.append(' *   gehoert: "%s" - %d von %d Woertern des Untertitels werden nicht gesprochen' %
                 (c_text(z['gehoert']), z['mass'][1], z['mass'][0]))
        o.append(' *   git-Blob %s, SHA-256 %s */' % (z['blob'], z['sha']))
        o.append('    { 0x%04Xu, %2d, %6uu, 0x%016Xull, "%s" },' % (z['raum'], z['nr'], z['n'], z['fnv'], z['pfad'].lower()))
    o.append('/* NICHT in der Tabelle (Stufe 7): diese Aufnahmen des Tags %s sprechen den neuen Untertitel woertlich oder bis auf' % TAG)
    o.append(' * ein Wort und SPIELEN (Regel: hoechstens %d Wort des Untertitels nicht gesprochen, hoechstens %d gesprochene Woerter' %
             (REGEL_UNTERTITEL_FREMD, REGEL_AUFNAHME_FREMD))
    o.append(' * nicht im Untertitel).')
    for z in spielt:
        o.append(' * ROOM%04X msg %d: gehoert "%s"  /  Untertitel %s: "%s"' %
                 (z['raum'], z['nr'], c_text(z['gehoert']), z['spr_neu'], c_text(z['neu'])))
        o.append(' *   %d von %d Woertern des Untertitels nicht gesprochen, %d von %d gesprochenen nicht im Untertitel; %d Byte,' %
                 (z['mass'][1], z['mass'][0], z['mass'][3], z['mass'][2], z['n']))
        o.append(' *   FNV-1a-64 0x%016X, git-Blob %s' % (z['fnv'], z['blob']))
    o.append(' */')
    return '\n'.join(o) + '\n'


def saetze_bauen(tag_commit, zeilen):
    """Alle 15 Saetze fuer die Riegel (tests/unit/test_r42_namen.c Teil altaufnahme, probe_r42_namen_altlader.c)."""
    o = []
    o.append('/* ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.')
    o.append(' * Die 15 Saetze mit neuem Untertitel seit v0.8.31, deren Datei am Stand des Tags %s (Commit %s) den alten Satz traegt.' %
             (TAG, tag_commit[:12]))
    o.append(' * { Raum, Nummer, stumm (1 = in der Tabelle der veralteten Aufnahmen, 0 = spielt, fast woertlich), Groesse und')
    o.append(' *   FNV-1a-64 der Datei am Tag, gehoert (Spracherkenner 19.11.2026), Untertitel ohne Sprechernamen }')
    o.append(' * Regel Stufe 7: RE15_ALTSATZ_UNTERTITEL_FREMD / RE15_ALTSATZ_AUFNAHME_FREMD (Entscheid 19.11.2026, PORT-WAHL). */')
    o.append('#ifndef RE15_ALTSATZ_UNTERTITEL_FREMD')
    o.append('#define RE15_ALTSATZ_UNTERTITEL_FREMD %d' % REGEL_UNTERTITEL_FREMD)
    o.append('#define RE15_ALTSATZ_AUFNAHME_FREMD %d' % REGEL_AUFNAHME_FREMD)
    o.append('#endif')
    o.append('#ifdef RE15_ALTSATZ_ZEILEN')
    for z in zeilen:
        o.append('    { 0x%04Xu, %2d, %d, %6uu, 0x%016Xull, "%s",\n      "%s" },' %
                 (z['raum'], z['nr'], 1 if z['stumm'] else 0, z['n'], z['fnv'], c_text(z['gehoert']), c_text(z['neu'])))
    o.append('#endif')
    return '\n'.join(o) + '\n'


def cmake_bauen(tag_commit, zeilen):
    spielt = [z for z in zeilen if not z['stumm']]
    zeilen = [z for z in zeilen if z['stumm']]
    o = []
    o.append('# ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.')
    o.append('# Runde 42 Strang "namen", Stufe 6 + 7: SHA-256 der %d veralteten Aufnahmen (Stand Tag %s, Commit %s).' %
             (len(zeilen), TAG, tag_commit[:12]))
    o.append('# Eine Datei mit genau dieser Pruefsumme behandelt der Port wie eine fehlende Datei (engine/src/altaufnahme_r42.c,')
    o.append('# Haken audio_pc.c re15_voice_load_clip). Die Integrationstests fragen damit den Stand der Datei im Baum ab und')
    o.append('# bleiben gruen, wenn der Nutzer seine neue Aufnahme abgelegt hat (dann muss die Zeile SPIELEN).')
    o.append('# Stufe 7 (Entscheid 19.11.2026): nicht in der Liste und deshalb KEINE = spielt: %s' %
             ', '.join('%04X msg %d' % (z['raum'], z['nr']) for z in spielt))
    o.append('# (sprechen den neuen Untertitel woertlich oder bis auf ein Wort).')
    o.append('# Liste flach: je Datei vier Eintraege (Raum 4 Hex; Nummer; Pfad; SHA-256).')
    o.append('set(RE15_ALTAUFNAHMEN_R42')
    for z in zeilen:
        o.append('    "%04X;%d;%s;%s"' % (z['raum'], z['nr'], z['pfad'], z['sha']))
    o.append(')')
    o.append('# re15_altaufnahme_stand(<out> <synchro-Wurzel> <Raum 4 Hex> <Nummer>): ALT = Datei traegt noch die alte Aufnahme,')
    o.append('# NEU = Datei ersetzt, FEHLT = keine Datei, KEINE = (Raum, Nummer) steht nicht in der Liste.')
    o.append('function(re15_altaufnahme_stand _out _synchro _raum _nr)')
    o.append('    set(${_out} "KEINE" PARENT_SCOPE)')
    o.append('    set(_liste ${RE15_ALTAUFNAHMEN_R42})')
    o.append('    list(LENGTH _liste _len)')
    o.append('    math(EXPR _max "${_len} / 4 - 1")')
    o.append('    foreach(_i RANGE 0 ${_max})')
    o.append('        math(EXPR _a "${_i} * 4")')
    o.append('        math(EXPR _b "${_a} + 1")')
    o.append('        math(EXPR _c "${_a} + 2")')
    o.append('        math(EXPR _d "${_a} + 3")')
    o.append('        list(GET _liste ${_a} _r)')
    o.append('        list(GET _liste ${_b} _n)')
    o.append('        list(GET _liste ${_c} _p)')
    o.append('        list(GET _liste ${_d} _s)')
    o.append('        if(_r STREQUAL "${_raum}" AND _n EQUAL ${_nr})')
    o.append('            string(REGEX REPLACE "^synchro/" "" _rel "${_p}")')
    o.append('            if(NOT EXISTS "${_synchro}/${_rel}")')
    o.append('                set(${_out} "FEHLT" PARENT_SCOPE)')
    o.append('            else()')
    o.append('                file(SHA256 "${_synchro}/${_rel}" _ist)')
    o.append('                if(_ist STREQUAL "${_s}")')
    o.append('                    set(${_out} "ALT" PARENT_SCOPE)')
    o.append('                else()')
    o.append('                    set(${_out} "NEU" PARENT_SCOPE)')
    o.append('                endif()')
    o.append('            endif()')
    o.append('            return()')
    o.append('        endif()')
    o.append('    endforeach()')
    o.append('endfunction()')
    o.append('# Stufe 7: die fast woertlichen Aufnahmen (nicht in der Liste oben, sie SPIELEN) - gleiche Form, fuer Durchlaeufe, die')
    o.append('# dazusagen wollen, ob unter dem Namen noch die Aufnahme des Tags liegt.')
    o.append('set(RE15_ALTAUFNAHMEN_R42_FAST_WOERTLICH')
    for z in spielt:
        o.append('    "%04X;%d;%s;%s"' % (z['raum'], z['nr'], z['pfad'], z['sha']))
    o.append(')')
    o.append('# re15_altaufnahme_fastwoertlich_stand(<out> <synchro-Wurzel> <Raum 4 Hex> <Nummer>): ALT = noch die Aufnahme des Tags')
    o.append('# (spielt, fast woertlich), NEU = ersetzt (spielt), FEHLT = keine Datei, KEINE = steht nicht in dieser Liste.')
    o.append('function(re15_altaufnahme_fastwoertlich_stand _out _synchro _raum _nr)')
    o.append('    set(RE15_ALTAUFNAHMEN_R42 ${RE15_ALTAUFNAHMEN_R42_FAST_WOERTLICH})')
    o.append('    re15_altaufnahme_stand(_st "${_synchro}" ${_raum} ${_nr})')
    o.append('    set(${_out} "${_st}" PARENT_SCOPE)')
    o.append('endfunction()')
    return '\n'.join(o) + '\n'


def main():
    check = '--check' in sys.argv[1:]
    tag_commit, zeilen = messen()
    ziele = (
        (os.path.join(PORT, 'engine', 'src', 'gen', 'altaufnahme_r42_tabelle.inc'), inc_bauen(tag_commit, zeilen)),
        (os.path.join(PORT, 'tests', 'integration', 'altaufnahmen_r42.cmake'), cmake_bauen(tag_commit, zeilen)),
        (os.path.join(PORT, 'tests', 'unit', 'gen_r42_namen_altsaetze.inc'), saetze_bauen(tag_commit, zeilen)),
    )
    print('Tag %s = Commit %s' % (TAG, tag_commit))
    for z in zeilen:
        print('%-40s %6d B  FNV-1a-64 %016X  im Baum: %-22s  %s (Untertitel: %d von %d Woertern nicht gesprochen; Aufnahme: '
              '%d von %d nicht im Untertitel)' %
              (z['pfad'], z['n'], z['fnv'], {None: 'FEHLT', True: 'noch die alte Aufnahme', False: 'ERSETZT'}[z['jetzt']],
               'STUMM ' if z['stumm'] else 'SPIELT', z['mass'][1], z['mass'][0], z['mass'][3], z['mass'][2]))
    rc = 0
    for ziel, text in ziele:
        alt = None
        if os.path.isfile(ziel):
            with open(ziel, 'r', encoding='utf-8', newline='') as f:
                alt = f.read()
        if alt == text:
            print('unveraendert: ' + ziel)
        elif check:
            print('ABWEICHUNG:   ' + ziel)
            rc = 1
        else:
            with open(ziel, 'w', encoding='utf-8', newline='\n') as f:
                f.write(text)
            print('geschrieben:  ' + ziel)
    return rc


if __name__ == '__main__':
    sys.exit(main())
