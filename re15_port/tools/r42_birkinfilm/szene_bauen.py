#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""szene_bauen.py - Runde 42 Strang "birkinfilm", Stufe 1: Leons erstes Treffen mit Annette (ROOM6000 sub06) mit Rahmenzeilen
und dem RE2-Film "Birkins Mutation" -> engine/src/gen/birkinfilm_r42.inc

    python re15_port/tools/r42_birkinfilm/szene_bauen.py           (schreibt die .inc)
    python re15_port/tools/r42_birkinfilm/szene_bauen.py --check   (prueft nur)

NUTZER (18.11.2026): "Zusaetzlich moechte ich das du die Birkin Mutation Erklaerungscutscene aus Resident Evil 2 irgendwo
sinnvoll mit einbaust." ORCHESTRATOR-ENTSCHEID (18.11., Ort vom Nutzer freigestellt): Leon in ROOM6000, Film nach "I see..."
(msg 2 @0x0113A) und vor "Come on..." (msg 3 @0x01156); davor und danach wenige Rahmenzeilen Annettes, RE2-Originalsaetze
samt RE2-Stimme bevorzugt; Uebergang wie RE2 bei genau diesem Film; einmal je Spielstand.

ORIGINAL ROOM6000 sub06 [@0x0106A, @0x011A6) (Zeigertabelle sub_scd @0x00F0C), gestartet von sub00 @0x00F58 `04 ff 18 06` nur
bei Ck(3,100)=0 (@0x00F40); sub06 setzt als ERSTES Set(3,100,1) @0x0106A `22 03 64 01` -> die Szene (und damit der Film) laeuft
einmal je Spielstand, ohne neues Bit.
  @0x0113A `2b 02 00 00` msg 2 "Annette: I see..."; @0x0113E Work_set Annette (2/0); @0x01142..@0x01155 Gestenblock
  `3f 00 10 00 09 0a 32 00 3f 00 10 00 43 00 80 00 09 0a 33 00`; @0x01156 `2b 03 00 00` msg 3 "Come on. ..."

PROGRAMM = sub06 [@0x0106A, @0x01156) + EINSCHUB + sub06 [@0x01156, @0x011A6) (Original-Opcodes, sub06 sonst bytegleich).
EINSCHUB (Vorlage RE2 Leon-Disc ROOM4090 sub02, selbst disassembliert; Zeilenform = msg 3 @0x01156, Gesten = @0x01142):
  2b 28 00 00  msg 40 Annette Satz 1 (Xa 0x09)   RE2 @0x02718 `2b 00 0a 00 00 00` msg 0x0A + @0x0271E `59 00 09 00` Xa_on 0x09
  09 0a 29 00  Sleep 41                          RE2 @0x02722 `09 0a 29 00`
  2b 30 00 00  msg 48 Annette Satz 2             RE2 @0x02726 `2b 00 0b 00 00 00` msg 0x0B (+ @0x0272C `0b 0c` Wsleep)
  <Gesten @0x01142..@0x01155>                    PORT-WAHL: Annettes Sprechgeste nach msg 2 in derselben Szene
  2b 29 00 00  msg 41 Annette Satz 1 (Xa 0x11)   RE2 @0x027CE `2b 00 14 00 00 00` msg 0x14 + @0x027D4 `59 00 11 00` Xa_on 0x11
  09 0a 51 00  Sleep 81                          RE2 @0x027D8 `09 0a 51 00`
  2b 31 00 00  msg 49 Annette Satz 2             RE2 @0x027DC `2b 00 15 00 00 00` msg 0x15
  <Gesten>
  2b 2b 00 00  Warte-Zeile 43 (kein Text)        RE2 @0x027E2 `0b 0c` Wsleep/Wsleeping: bis die Stimme zu Ende ist
  09 0a 02 00  Sleep 2                           RE2 @0x027E4 `09 0a 02 00`
  09 0a 3c 00  Sleep 60                          RE2 @0x027F0 `09 0a 3c 00`
  54 00 04 00 00 00  MAIN anhalten               RE2 @0x02804 `51 00 04 01 33 41` (Form 6000 main00 @0x00D52 `54 00 01 00 46 40`,
                                                 Op 4 = SsSeqPause; Lautstaerke/Pan 0 = keine Schreibung, wie r42/finale)
  2b 2c 00 00  Film-Zeile 44 (kein Text)         RE2 @0x027F4 Abblende 64 / @0x02812 + @0x02818 `6f 0b` Film / @0x0281C Einblende
                                                 (engine/src/zws_r41.c, Blendenart RE15_ZWS_BLENDART_4090)
  09 0a 3f 00  Sleep 63                          RE2 @0x02820 `09 0a 3f 00` (die Einblende laeuft)
  54 00 03 00 00 00  MAIN weiter                 RE2 @0x0282B `51 00 03 01 33 41` (Op 3 = SsSeqReplay)
  2b 2a 00 00  msg 42 Annette Satz 1 (Xa 0x14)   RE2 @0x02846 `2b 00 17 00 00 00` msg 0x17 + @0x0284C `59 00 14 00` Xa_on 0x14
  09 0a 51 00  Sleep 81                          RE2 @0x02850 `09 0a 51 00`
  2b 32 00 00  msg 50 Annette Satz 2             RE2 @0x02854 `2b 00 18 00 00 00` msg 0x18 (+ @0x0285A `0b 0c` Wsleep)
  <Gesten>
  -> Original @0x01156 msg 3 "Come on. ..." (der Stimmen-Riegel scd_vm.c laesst Xa 0x14 ausreden)
Sleep-Form `09 0a NN 00` = RE1.5 (ROOM6000 sub06 @0x01146 `09 0a 32 00`) und RE2 (@0x02722) gleich.
RE2 pausiert dort SUB (@0x027FA) und MAIN (@0x02804); in ROOM6000 laeuft nur MAIN (main00 @0x00D52 Slot 0) -> nur Slot 0
(r42/finale hat gemessen: Op 3 auf einem stillen Slot schaltet ihn ein).

TEXTE (Untertitel; RE2-US zeigt zu diesen Saetzen keinen Text, ROOM4090 Block 2 @0x3FB0 msg 0..32 leer): Wortlaut = gesprochene
RE2-Saetze (Abschrift Whisper medium.en, scratchpad zws_birkin/voice04_asr.txt). JE SATZ EINE NACHRICHT wie RE2 (Runde 42 Strang
namen, Stufe 1): RE2 hat je Rahmenzeile zwei Nachrichten (0x0A/0x0B, 0x14/0x15, 0x17/0x18) mit Sleep 41/81 dazwischen und EINEM
Xa_on davor (Adressen oben) - die Stimme laeuft ueber beide Saetze. Der Port zeigt Satz 1 als msg 40/41/42 (mit der RE2-Stimme)
und Satz 2 als msg 48/49/50 (ohne eigene Stimme, die laufende spricht weiter); beide tragen den Namenskopf "Annette:".
NUTZER 18.11.2026: "Du hast bei den uebernommenen Saetzen aus Resident Evil 2 nicht den Namen in den Dialog mit davor geschrieben
wie bei den rest auch...." / "Ich meinte bei Anette." (Memory reai-v2-sprechername-vor-satz: jeder gesprochene Satz traegt den
Namen.) Vorher (Strang birkinfilm) waren es zwei Seiten EINER Nachricht (`04 01 02 N 04 00`, Form ROOM3060 @0x1244), Seite 2 ohne
Namen. Die Satz-2-Nachrichten 48/49/50 laufen am Stimmen-Riegel vorbei (birkinfilm_r42.c re15_birkinfilm_r42_folgeseite): sie
gehoeren zur laufenden RE2-Stimme; ohne diese Ausnahme wartete Satz 2 auf das Ende der Stimme, die ihn spricht.
Farbe Annette 02 (ROOM6000 msg 0 @0x01240 `04 00 05 02`), Schwanz `04 01 01 63` = Nachlauf der Original-Untertitel von ROOM6000
(tools/r42_story/nachlauf_r42.py).
STIMMEN: shared_assets/RE2/R409_XA09/11/14.XAS (tools/r42_birkinfilm/xa_schneiden.py), Zuordnung (Raum, Id) ->
Datei in engine/src/birkinfilm_r42.c, gespielt ueber den Sprachkanal (platform/pc/src/audio_pc.c re15_voice_load_clip).
Ids 40..44: ROOM6000 RDT 0..7, Port 0/3 (story_r36 Neutext), 30/31 (r42/lageplaene Lageplan) -> 40..44 frei, < 64 (VOICE_MAX_MSG).
Ids 48..50 (Strang namen): ebenso frei (das Werkzeug prueft: RDT-Nachrichtenzahl 8, kein Message_on 48..50 im SCD von ROOM6000);
45..47 bleiben frei, damit 46/47 nicht mit Elzas Warte-/Film-Zeile in ROOM5011 verwechselt werden.

STUFE 2 - ELZA ROOM5011 (ORCHESTRATOR-ENTSCHEID 18.11.: Film zwischen msg 3 @0x00640 "John. I read some files ... terrifying
things." und msg 4 @0x00690 "She's been infected by what they were working on here."; keine neuen Saetze).
ORIGINAL ROOM5011 sub02 [@0x0055A, @0x0093A) (Zeigertabelle sub_scd @0x00524), gestartet von sub00 @0x0053E `04 ff 18 02` nur bei
Ck(4,237)=1 (@0x0053A); sub02 loescht als ERSTES Set(4,237,0) @0x0055A `22 04 ed 00`. Zensus (--zensus, alle RDTs): (4,237)=1
schreibt nur ROOM5021 sub02 @0x01356 (dort vorher Set(4,192,1) @0x01352, sub00 @0x012EA startet nur bei Ck(4,192)=0) -> die Szene
(und der Film) laeuft einmal je Spielstand, ohne neues Bit.
PROGRAMM = sub02 [@0x0055A, @0x00690) + EINSCHUB + sub02 [@0x00690, @0x0093A); sub02 hat keine Sprung-Opcodes.
EINSCHUB vor @0x00690 (msg 4) = der Einschub von 6000 ab der Warte-Zeile, ohne Rahmenzeilen:
  2b 2e 00 00  Warte-Zeile 46        RE2 @0x027E2 Wsleep (Stimme von msg 3 + Untertitel zu Ende)
  09 0a 02 00  Sleep 2               RE2 @0x027E4
  09 0a 3c 00  Sleep 60              RE2 @0x027F0
  54 00 04 00 00 00  MAIN anhalten   RE2 @0x02804 (ROOM5011: Musiktabelle @0x800748C4 `24 ff` = MAIN 0x24, kein SUB)
  2b 2f 00 00  Film-Zeile 47         RE2 @0x027F4 / @0x02812 / @0x02818 `6f 0b` / @0x0281C (Blendenart RE15_ZWS_BLENDART_4090)
  09 0a 3f 00  Sleep 63              RE2 @0x02820
  54 00 03 00 00 00  MAIN weiter     RE2 @0x0282B
Ids 46/47: ROOM5011 RDT 0..45, Port 0x2C/0x2D (RDT-Ids), r42/rest 40/45 (RDT-Ids) -> frei, < 64.
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
sys.path.insert(0, os.path.join(PORT, 'tools', 'r42_story'))
import texte_stage3a as T  # noqa: E402

ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'birkinfilm_r42.inc')
RDT = os.path.join(PORT, 'shared_assets', 'PSX', 'STAGE6', 'ROOM6000.RDT')
SUB06 = (0x0106A, 0x011A6)
ANKER = 0x01156
GESTEN = (0x01142, 0x01156)
MAX_ZEILEN = 3
PSX_LEN = 128   # PSX MSG_RAW_LEN (msg_common.c): je Satz eine Nachricht passt hinein
SCHWANZ = [0x04, 0x01, 0x01, 0x63]

MSG_XA09, MSG_XA11, MSG_XA14, MSG_WARTEN, MSG_FILM = 40, 41, 42, 43, 44
MSG_XA09_S2, MSG_XA11_S2, MSG_XA14_S2 = 48, 49, 50   # Satz 2 (RE2 msg 0x0B / 0x15 / 0x18)

RDT5011 = os.path.join(PORT, 'shared_assets', 'PSX', 'STAGE5', 'ROOM5011.RDT')
SUB02_5011 = (0x0055A, 0x0093A)
ANKER_5011 = 0x00690
MSG5011_WARTEN, MSG5011_FILM = 46, 47
# Ablauf-Opcodes mit Sprungweite: Ifel_ck, Else_ck, Endif, For, Next, While, Ewhile, Do, Edwhile, Switch, Case, Default, Eswitch, Goto (scd_dump_room.NAMES)
SPRUENGE = {0x06, 0x07, 0x08, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17}
ANNETTE = (0x02, 'Annette')
# (Id Satz 1, Satz 1, Id Satz 2, Satz 2, RE2-Sleep dazwischen, Beleg) - je Satz eine Nachricht wie RE2
UNTERTITEL = (
    (MSG_XA09, "Annette Birkin.",
     MSG_XA09_S2, "My husband is the man responsible for the creation of the T-virus. William Birkin.",
     41, "RE2 ROOM4090 @0x02722 Sleep 41 zwischen msg 0x0A @0x02718 und 0x0B @0x02726"),
    (MSG_XA11, "And it's all Umbrella's fault.",
     MSG_XA11_S2, "None of this would have happened if they hadn't tried to steal his research away from him.",
     81, "RE2 ROOM4090 @0x027D8 Sleep 81 zwischen msg 0x14 @0x027CE und 0x15 @0x027DC"),
    (MSG_XA14, "As a result of his virus-induced transmutation,",
     MSG_XA14_S2, "William should have lost any prior memories he had as a human by now.",
     81, "RE2 ROOM4090 @0x02850 Sleep 81 zwischen msg 0x17 @0x02846 und 0x18 @0x02854"),
)
RE2_4090 = os.path.normpath(os.path.join(PORT, '..', 'info', 're2leon', 'PL0', 'RDT', 'ROOM4090.RDT'))
# RE2 ROOM4090 sub02 je Rahmenzeile: Satz 1, Xa_on, Sleep, Satz 2 (Adresse, Bytes) - das Werkzeug prueft sie, wenn info/ da ist
RE2_BELEG = (
    ((0x02718, '2b000a000000'), (0x0271E, '59000900'), (0x02722, '090a2900'), (0x02726, '2b000b000000')),
    ((0x027CE, '2b0014000000'), (0x027D4, '59001100'), (0x027D8, '090a5100'), (0x027DC, '2b0015000000')),
    ((0x02846, '2b0017000000'), (0x0284C, '59001400'), (0x02850, '090a5100'), (0x02854, '2b0018000000')),
)


def satz(text):
    """Ein Satz = eine Nachricht mit Namenskopf (texte_stage3a.untertitel), Schwanz `04 01 01 63`."""
    farbe, name = ANNETTE
    b, z, bw = T.untertitel(farbe, name, text, 0x6000)
    assert b[-4:] == SCHWANZ, b[-4:]
    return b, z, bw


def re2_pruefen():
    if not os.path.exists(RE2_4090):
        print('Hinweis: %s fehlt - RE2-Belege nicht nachgeprueft' % RE2_4090)
        return
    r = open(RE2_4090, 'rb').read()
    for zeile, u in zip(RE2_BELEG, UNTERTITEL):
        for adr, hx in zeile:
            assert r[adr:adr + len(hx) // 2].hex() == hx, 'RE2 ROOM4090 @0x%05X: %s statt %s' % (adr, r[adr:adr + 6].hex(), hx)
        assert r[zeile[2][0] + 2] == u[4], 'Sleep @0x%05X' % zeile[2][0]


def ids_frei(d):
    """ROOM6000: RDT-Nachrichtenzahl und Message_on-Ids im SCD (Satz-2-Ids 48..50 frei)."""
    ms = int.from_bytes(d[0x3C:0x40], 'little')
    n = int.from_bytes(d[ms:ms + 2], 'little') // 2
    assert n == 8, 'ROOM6000 hat %d RDT-Nachrichten (erwartet 8)' % n
    L = rdt_walk()
    msgs = set()
    for (tag, idx), ops in L.regionen(d).items():
        for pc, op, sz in ops:
            if op == 0x2b:
                msgs.add(d[pc + 1])
    for u in UNTERTITEL:
        for mid in (u[0], u[2]):
            assert mid >= n and mid not in msgs, 'msg %d nicht frei (SCD %s)' % (mid, sorted(msgs))
    print('ROOM6000: %d RDT-Nachrichten, Message_on-Ids im SCD %s -> 40..42/48..50 frei' % (n, sorted(msgs)))


def programm(d):
    a, e = SUB06
    sub = d[a:e]
    assert d[0x0106A:0x0106E] == bytes([0x22, 0x03, 0x64, 0x01]), "Set(3,100,1) @0x0106A"
    assert d[0x0113A:0x0113E] == bytes([0x2b, 0x02, 0x00, 0x00]), "msg 2 @0x0113A"
    assert d[0x0113E:0x01142] == bytes([0x2e, 0x02, 0x00, 0x00]), "Work_set Annette @0x0113E"
    g = d[GESTEN[0]:GESTEN[1]]
    assert g == bytes.fromhex('3f001000090a32003f00100043008000090a3300'), g.hex()
    assert d[ANKER:ANKER + 4] == bytes([0x2b, 0x03, 0x00, 0x00]), "msg 3 @0x01156"
    assert d[e - 2:e] == bytes([0x01, 0x00]), "Evt_end am Ende von sub06"
    ein = [
        (bytes([0x2b, MSG_XA09, 0, 0]), 'msg 40 Annette Satz 1 (RE2 msg 0x0A @0x02718, Xa 0x09 @0x0271E)'),
        (bytes([0x09, 0x0a, 41, 0x00]), 'Sleep 41 = RE2 @0x02722'),
        (bytes([0x2b, MSG_XA09_S2, 0, 0]), 'msg 48 Annette Satz 2 (RE2 msg 0x0B @0x02726, Stimme laeuft weiter)'),
        (g, 'Gesten = @0x01142..@0x01155 (PORT-WAHL)'),
        (bytes([0x2b, MSG_XA11, 0, 0]), 'msg 41 Annette Satz 1 (RE2 msg 0x14 @0x027CE, Xa 0x11 @0x027D4)'),
        (bytes([0x09, 0x0a, 81, 0x00]), 'Sleep 81 = RE2 @0x027D8'),
        (bytes([0x2b, MSG_XA11_S2, 0, 0]), 'msg 49 Annette Satz 2 (RE2 msg 0x15 @0x027DC)'),
        (g, 'Gesten = @0x01142..@0x01155'),
        (bytes([0x2b, MSG_WARTEN, 0, 0]), 'Warte-Zeile 43 = RE2 Wsleep @0x027E2 `0b 0c`'),
        (bytes([0x09, 0x0a, 0x02, 0x00]), 'Sleep 2 = RE2 @0x027E4'),
        (bytes([0x09, 0x0a, 0x3c, 0x00]), 'Sleep 60 = RE2 @0x027F0'),
        (bytes([0x54, 0x00, 0x04, 0x00, 0x00, 0x00]), 'MAIN anhalten = RE2 @0x02804 (Op 4)'),
        (bytes([0x2b, MSG_FILM, 0, 0]), 'Film-Zeile 44: Abblende 64 / Film / Einblende 64 = RE2 @0x027F4..@0x0281C'),
        (bytes([0x09, 0x0a, 0x3f, 0x00]), 'Sleep 63 = RE2 @0x02820'),
        (bytes([0x54, 0x00, 0x03, 0x00, 0x00, 0x00]), 'MAIN weiter = RE2 @0x0282B (Op 3)'),
        (bytes([0x2b, MSG_XA14, 0, 0]), 'msg 42 Annette Satz 1 (RE2 msg 0x17 @0x02846, Xa 0x14 @0x0284C)'),
        (bytes([0x09, 0x0a, 81, 0x00]), 'Sleep 81 = RE2 @0x02850'),
        (bytes([0x2b, MSG_XA14_S2, 0, 0]), 'msg 50 Annette Satz 2 (RE2 msg 0x18 @0x02854)'),
        (g, 'Gesten = @0x01142..@0x01155'),
    ]
    vor = d[a:ANKER]
    nach = d[ANKER:e]
    prog = vor + b''.join(x for x, _ in ein) + nach
    return sub, prog, ein, len(vor)


def rdt_walk():
    sys.path.insert(0, os.path.join(PORT, 'tools'))
    import scd_walk_lib as L  # noqa: E402
    return L


def programm5011(d):
    """ROOM5011 sub02 mit dem Einschub vor msg 4 @0x00690 (Belege im Kopf)."""
    a, e = SUB02_5011
    L = rdt_walk()
    ss = L.u32(d, 0x44)
    assert ss == 0x00524 and ss + L.u16(d, ss + 4) == a and ss + L.u16(d, ss + 6) == e, "Zeigertabelle sub_scd @0x00524: sub02"
    assert d[0x0053A:0x0053E] == bytes([0x21, 0x04, 0xed, 0x01]), "Ck(4,237)=1 @0x0053A"
    assert d[0x0053E:0x00542] == bytes([0x04, 0xff, 0x18, 0x02]), "Evt_exec sub02 @0x0053E"
    assert d[a:a + 4] == bytes([0x22, 0x04, 0xed, 0x00]), "Set(4,237,0) @0x0055A"
    assert d[0x00640:0x00644] == bytes([0x2b, 0x03, 0x00, 0x00]), "msg 3 @0x00640"
    assert d[0x0068C:0x00690] == bytes([0x09, 0x0a, 0x1a, 0x00]), "Sleep 26 @0x0068C"
    assert d[ANKER_5011:ANKER_5011 + 4] == bytes([0x2b, 0x04, 0x00, 0x00]), "msg 4 @0x00690"
    assert d[e - 2:e] == bytes([0x01, 0x00]), "Evt_end am Ende von sub02"
    ops = L.walk_ops(d, a, e)
    assert ops[-1][0] == e - 2 and sum(z for _, _, z in ops) == e - a, "sub02 opcode-genau bis Evt_end"
    assert ANKER_5011 in [pc for pc, _, _ in ops], "msg 4 ist ein Opcode-Anfang"
    spr = [(pc, op) for pc, op, _ in ops if op in SPRUENGE]
    assert not spr, "Sprung-Opcodes in sub02: %r" % spr
    ein = [
        (bytes([0x2b, MSG5011_WARTEN, 0, 0]), 'Warte-Zeile 46 = RE2 Wsleep @0x027E2 `0b 0c` (Stimme msg 3 + Untertitel)'),
        (bytes([0x09, 0x0a, 0x02, 0x00]), 'Sleep 2 = RE2 @0x027E4'),
        (bytes([0x09, 0x0a, 0x3c, 0x00]), 'Sleep 60 = RE2 @0x027F0'),
        (bytes([0x54, 0x00, 0x04, 0x00, 0x00, 0x00]), 'MAIN anhalten = RE2 @0x02804 (Op 4; 5011 = MAIN 0x24 @0x800748C4)'),
        (bytes([0x2b, MSG5011_FILM, 0, 0]), 'Film-Zeile 47: Abblende 64 / Film / Einblende 64 = RE2 @0x027F4..@0x0281C'),
        (bytes([0x09, 0x0a, 0x3f, 0x00]), 'Sleep 63 = RE2 @0x02820'),
        (bytes([0x54, 0x00, 0x03, 0x00, 0x00, 0x00]), 'MAIN weiter = RE2 @0x0282B (Op 3)'),
    ]
    vor = d[a:ANKER_5011]
    nach = d[ANKER_5011:e]
    prog = vor + b''.join(x for x, _ in ein) + nach
    return d[a:e], prog, ein, len(vor)


def zensus():
    """(4,237) ueber alle RDTs und die Message-Ids im SCD von ROOM5011 (Beleg 'einmal je Spielstand', Ids 46/47 frei)."""
    import glob
    L = rdt_walk()
    treffer = []
    for p in sorted(glob.glob(os.path.join(PORT, 'shared_assets', 'PSX', 'STAGE*', 'ROOM*.RDT'))):
        d = open(p, 'rb').read()
        if len(d) < 0x100:
            continue                      # 4-Byte-Platzhalter (ROOM1270 usw.)
        for (tag, idx), ops in L.regionen(d).items():
            for pc, op, sz in ops:
                if op in (0x21, 0x22) and d[pc + 1] == 4 and d[pc + 2] == 237:
                    treffer.append((os.path.basename(p)[:-4], tag, idx, pc, 'Ck' if op == 0x21 else 'Set', d[pc + 3]))
    for t in sorted(treffer):
        print('%s %s%02d @0x%05X %s(4,237)=%d' % t)
    soll = [('ROOM5011', 'sub', 0, 0x0053A, 'Ck', 1), ('ROOM5011', 'sub', 2, 0x0055A, 'Set', 0),
            ('ROOM5021', 'sub', 2, 0x01356, 'Set', 1)]
    assert sorted(treffer) == sorted(soll), treffer
    d = open(RDT5011, 'rb').read()
    msgs = set()
    for (tag, idx), ops in L.regionen(d).items():
        for pc, op, sz in ops:
            if op == 0x2b:
                msgs.add(d[pc + 1])
    assert MSG5011_WARTEN not in msgs and MSG5011_FILM not in msgs, sorted(msgs)
    print('Message_on-Ids im SCD von ROOM5011:', sorted(msgs), '-> 46/47 frei')


def main():
    if '--zensus' in sys.argv:
        zensus()
        return
    d = open(RDT, 'rb').read()
    fehler = []
    re2_pruefen()
    ids_frei(d)
    sub, prog, ein, n_vor = programm(d)
    out = ['/* Generiert von re15_port/tools/r42_birkinfilm/szene_bauen.py - nicht von Hand aendern. Herleitung im Kopf des',
           ' * Werkzeugs und in include/re15_birkinfilm_r42.h (Runde 42 Strang birkinfilm, ROOM6000 sub06 + RE2-Film R409). */']
    out += T.carr('k_bf_6000_sub06', list(sub), 'ROOM6000 sub06 [@0x0106A, @0x011A6) bytegleich = Signatur (%d B)' % len(sub))
    out.append('#define BF_EINSCHUB_AB %d   /* Programm-Offset des Einschubs = @0x01156 - @0x0106A */' % n_vor)
    kom = ['/* Einschub vor @0x01156 (msg 3):']
    for x, k in ein:
        kom.append(' *   %-60s %s' % (x.hex(' '), k))
    kom.append(' */')
    out += kom
    out += T.carr('k_bf_6000_prog', list(prog),
                  'Programm = sub06 [@0x0106A,@0x01156) + Einschub + sub06 [@0x01156,@0x011A6) (%d B)' % len(prog))
    tab = []
    for mid1, t1, mid2, t2, n, beleg in UNTERTITEL:
        for mid, text, was in ((mid1, t1, 'Satz 1, mit der RE2-Stimme'),
                               (mid2, t2, 'Satz 2 nach Sleep %d = %s; Stimme laeuft weiter' % (n, beleg))):
            b, zeilen, bw = satz(text)
            for i, w in enumerate(bw):
                if w > T.MAX_PX:
                    fehler.append('msg %d Zeile %d = %d px > %d' % (mid, i + 1, w, T.MAX_PX))
            if len(zeilen) > MAX_ZEILEN:
                fehler.append('msg %d: mehr als %d Zeilen' % (mid, MAX_ZEILEN))
            if len(b) > PSX_LEN:
                fehler.append('msg %d: %d Byte > %d (PSX MSG_RAW_LEN, msg_common.c)' % (mid, len(b), PSX_LEN))
            nm = 'k_bf_6000_msg%d' % mid
            out += T.carr(nm, b, 'ROOM6000 %d Annette: %s  (%s; %d B, Zeilen %s px)' % (
                mid, ' / '.join(zeilen), was, len(b), '/'.join(map(str, bw))))
            tab.append((mid, nm))
    d5 = open(RDT5011, 'rb').read()
    sub5, prog5, ein5, n_vor5 = programm5011(d5)
    out += T.carr('k_bf_5011_sub02', list(sub5), 'ROOM5011 sub02 [@0x0055A, @0x0093A) bytegleich = Signatur (%d B)' % len(sub5))
    out.append('#define BF5011_EINSCHUB_AB %d   /* Programm-Offset des Einschubs = @0x00690 - @0x0055A */' % n_vor5)
    kom = ['/* ROOM5011 Einschub vor @0x00690 (msg 4):']
    for x, k in ein5:
        kom.append(' *   %-60s %s' % (x.hex(' '), k))
    kom.append(' */')
    out += kom
    out += T.carr('k_bf_5011_prog', list(prog5),
                  'Programm = sub02 [@0x0055A,@0x00690) + Einschub + sub02 [@0x00690,@0x0093A) (%d B)' % len(prog5))
    out.append('typedef struct { uint8_t id; const uint8_t *b; uint16_t n; } bf_text_t;')
    out.append('static const bf_text_t k_bf_texte[] = {')
    for mid, nm in tab:
        out.append('    { %d, %s, (uint16_t)sizeof %s },' % (mid, nm, nm))
    out.append('};')
    out.append('#define BF_TEXTE_N ((int)(sizeof k_bf_texte / sizeof k_bf_texte[0]))')
    if fehler:
        sys.exit(chr(10).join(fehler))
    text = chr(10).join(out) + chr(10)
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline=chr(10), encoding='utf-8').write(text)
    print('geschrieben', ZIEL, '- Programm', len(prog), 'B')


if __name__ == '__main__':
    main()
