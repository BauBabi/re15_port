#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 6: Fingerabdruck der 15 VERALTETEN Aufnahmen (alter Satz unter neuem Untertitel).

    python re15_port/tools/r42_namen/altaufnahmen_bauen.py           (schreibt die beiden erzeugten Tabellen)
    python re15_port/tools/r42_namen/altaufnahmen_bauen.py --check   (prueft nur: Tabellen bytegleich zum Stand des Tags)

WAS: Die Story v0.8.31 gibt 15 Saetzen einen neuen Text (tools/r42_story/texte_stage3a.py, texte_stage3b.py, texte_stage6.py), die
Datei synchro/STAGEn/roomXXXX/mainNN.wav traegt aber noch die Aufnahme des ALTEN Satzes - in 6030 sogar mit anderem Sprecher
(AUFNAHMELISTE "Warum diese 15 Saetze zuerst?"). PORT-WAHL (Entscheid 19.11.2026; synchro/ ist Eigenproduktion des Projekts, kein
Original-Verhalten - RE1.5 hat keine englische Sprachausgabe): eine Datei, die nachweislich noch die alte Aufnahme ist, gilt wie
eine FEHLENDE Datei. Der Satz laeuft stumm mit Untertitel und Textdauer; die neue Aufnahme des Nutzers spielt von selbst, weil
sie einen anderen Fingerabdruck hat.

QUELLE DES FINGERABDRUCKS: der Stand des Tags v0.8.30 (`git show v0.8.30:synchro/...`), NICHT der Arbeitsbaum - sonst wuerde
eine schon ersetzte Datei wieder als "alt" eingetragen. Je Datei: Groesse in Byte + FNV-1a 64 bit ueber den ganzen Inhalt
(Startwert 0xcbf29ce484222325, Faktor 0x100000001b3). Dazu fuer die Integrationstests SHA-256 (cmake file(SHA256), zweite,
unabhaengige Pruefsumme). synchro/ wird nur gelesen.

AUSGABE:
    engine/src/gen/altaufnahme_r42_tabelle.inc      Tabelle fuer engine/src/altaufnahme_r42.c
    tests/integration/altaufnahmen_r42.cmake        SHA-256 je Datei + Funktion re15_altaufnahme_stand()
"""
import hashlib
import os
import subprocess
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
REPO = os.path.normpath(os.path.join(PORT, '..'))
TAG = 'v0.8.30'

# (Raum, Dateinummer, alter Sprecher, alter Satz, neuer Sprecher, neuer Satz) - Wortlaut alt/neu aus der AUFNAHMELISTE
# (reAi_v2_entscheidung_2026-11-06/11_story_v0831/AUFNAHMELISTE.md, Spalte "Art": neu aufnehmen (alt: ...)); der neue Satz steht
# in engine/src/gen/story3a_r42_texte.inc, story3b_r42_texte.inc, story6_r42_texte.inc. Reihenfolge = Raum, Nummer.
SAETZE = (
    (0x3020, 0, 'Leon', 'Dammit! the door does not open!', 'Leon', "Damn it! It won't budge."),
    (0x3020, 1, 'Marvin', "There's an open window, maybe we could enter through there?",
     'Marvin', "There's an open window up there. We could get in that way."),
    (0x3020, 2, 'Marvin', 'Leon give me an hand!', 'Marvin', 'Leon, give me a hand!'),
    (0x30E0, 3, 'Leon', "Y-yeah, Marvin is out in the hall. He can't get out because of all the monsters outside...",
     'Leon', "He's back in the lobby. Some dogs got his leg... he can barely walk."),
    (0x30E0, 10, 'Leon', 'That wound... Did those things get you? How careless...', 'Leon', "That's a bite... They got you?"),
    (0x30E0, 11, 'Marvin', 'The hall is no good...! Those things broke through the main gate and got inside!',
     'Marvin', "The lobby's gone...! They smashed through the front glass and got inside!"),
    (0x30E0, 13, 'Marvin', "The entire place is overrun! And if they're already all the way out here... the surface might just "
     'be done for...', 'Marvin', "There's more of them than I thought... If they've come this far, the city's finished."),
    (0x6030, 2, 'Sherry', "She's out there fighting the monsters!", 'Leon', 'Where is she now?'),
    (0x6030, 3, 'John', "She's trying to keep the outside clear so we can escape.",
     'John', 'We got split up in the fire. One second she was right behind us...'),
    (0x6030, 4, 'Leon', "Why aren't you out there?", 'Leon', "If she's as tough as you say, she'll find another way out."),
    (0x6030, 5, 'John', 'She told me to stay with the kid... and look after your friend here.',
     'John', "Then I'm staying with the kid. And I'll keep an eye on your friend here."),
    (0x6030, 21, 'John', "Name's John. This is Sherry. We made it down here with a girl named Elza.",
     'John', "Name's John. This is Sherry. We came down here with a girl named Elza."),
    (0x6030, 22, 'John', "She said she'd catch up with us.", 'Sherry', "Elza's still out there...!"),
    (0x6031, 1, 'Sherry', "A policeman! He's fighting off monsters. He wants us to start the train.",
     'Sherry', 'A policeman brought Marvin down here. Then he went back into the fire.'),
    (0x6031, 2, 'Elza', "I'll do it!", 'Elza', 'Back? Why?'),
)


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
    for raum, nr, spr_alt, alt, spr_neu, neu in SAETZE:
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
                           spr_alt=spr_alt, alt=alt, spr_neu=spr_neu, neu=neu, jetzt=jetzt))
    return tag_commit, zeilen


def c_text(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('*/', '* /')


def inc_bauen(tag_commit, zeilen):
    o = []
    o.append('/* ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.')
    o.append(' * Fingerabdruck der %d veralteten Aufnahmen: Stand des Tags %s (Commit %s), `git show %s:<Pfad>`.' %
             (len(zeilen), TAG, tag_commit[:12], TAG))
    o.append(' * Groesse in Byte + FNV-1a 64 bit ueber die ganze Datei (Startwert 0xcbf29ce484222325, Faktor 0x100000001b3).')
    o.append(' * Pfad klein geschrieben (Vergleich ohne Gross/Klein, audio_pc.c probiert room30E0 und room30e0). */')
    for z in zeilen:
        o.append('/* ROOM%04X msg %d: Aufnahme = %s: "%s"  ->  Untertitel v0.8.31 = %s: "%s"%s' %
                 (z['raum'], z['nr'], z['spr_alt'], c_text(z['alt']), z['spr_neu'], c_text(z['neu']),
                  '  (SPRECHERWECHSEL)' if z['spr_alt'] != z['spr_neu'] else ''))
        o.append(' *   git-Blob %s, SHA-256 %s */' % (z['blob'], z['sha']))
        o.append('    { 0x%04Xu, %2d, %6uu, 0x%016Xull, "%s" },' % (z['raum'], z['nr'], z['n'], z['fnv'], z['pfad'].lower()))
    return '\n'.join(o) + '\n'


def cmake_bauen(tag_commit, zeilen):
    o = []
    o.append('# ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.')
    o.append('# Runde 42 Strang "namen", Stufe 6: SHA-256 der %d veralteten Aufnahmen (Stand Tag %s, Commit %s).' %
             (len(zeilen), TAG, tag_commit[:12]))
    o.append('# Eine Datei mit genau dieser Pruefsumme behandelt der Port wie eine fehlende Datei (engine/src/altaufnahme_r42.c,')
    o.append('# Haken audio_pc.c re15_voice_load_clip). Die Integrationstests fragen damit den Stand der Datei im Baum ab und')
    o.append('# bleiben gruen, wenn der Nutzer seine neue Aufnahme abgelegt hat (dann muss die Zeile SPIELEN).')
    o.append('# Liste flach: je Datei vier Eintraege (Raum 4 Hex; Nummer; Pfad; SHA-256).')
    o.append('set(RE15_ALTAUFNAHMEN_R42')
    for z in zeilen:
        o.append('    "%04X;%d;%s;%s"' % (z['raum'], z['nr'], z['pfad'], z['sha']))
    o.append(')')
    o.append('# re15_altaufnahme_stand(<out> <synchro-Wurzel> <Raum 4 Hex> <Nummer>): ALT = Datei traegt noch die alte Aufnahme,')
    o.append('# NEU = Datei ersetzt, FEHLT = keine Datei, KEINE = (Raum, Nummer) steht nicht in der Liste.')
    o.append('function(re15_altaufnahme_stand _out _synchro _raum _nr)')
    o.append('    set(${_out} "KEINE" PARENT_SCOPE)')
    o.append('    foreach(_i RANGE 0 %d)' % (len(zeilen) - 1))
    o.append('        math(EXPR _a "${_i} * 4")')
    o.append('        math(EXPR _b "${_a} + 1")')
    o.append('        math(EXPR _c "${_a} + 2")')
    o.append('        math(EXPR _d "${_a} + 3")')
    o.append('        list(GET RE15_ALTAUFNAHMEN_R42 ${_a} _r)')
    o.append('        list(GET RE15_ALTAUFNAHMEN_R42 ${_b} _n)')
    o.append('        list(GET RE15_ALTAUFNAHMEN_R42 ${_c} _p)')
    o.append('        list(GET RE15_ALTAUFNAHMEN_R42 ${_d} _s)')
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
    return '\n'.join(o) + '\n'


def main():
    check = '--check' in sys.argv[1:]
    tag_commit, zeilen = messen()
    ziele = (
        (os.path.join(PORT, 'engine', 'src', 'gen', 'altaufnahme_r42_tabelle.inc'), inc_bauen(tag_commit, zeilen)),
        (os.path.join(PORT, 'tests', 'integration', 'altaufnahmen_r42.cmake'), cmake_bauen(tag_commit, zeilen)),
    )
    print('Tag %s = Commit %s' % (TAG, tag_commit))
    for z in zeilen:
        print('%-40s %6d B  FNV-1a-64 %016X  im Baum: %s' %
              (z['pfad'], z['n'], z['fnv'], {None: 'FEHLT', True: 'noch die alte Aufnahme', False: 'ERSETZT'}[z['jetzt']]))
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
