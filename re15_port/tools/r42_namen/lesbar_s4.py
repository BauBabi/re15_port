#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 4: gemeinsame Regeln fuer Zeilenzahl und Seitenwechsel neuer bzw. geaenderter Untertitel.

ZEILENZAHL (Beleg, selbst disassembliert):
  - Textbereich: Message_on -> FUN_80027e68 Zweig 0x300 setzt den Stift auf x = 0x22 (@0x80027eec `ori v0,zero,0x22`, sh @0x80027f10)
    und y = 0xB4 = 180 (@0x80027f14 `ori v0,zero,0xb4`, sh @0x80027f1c); jede Glyphe ist ein 16x16-Sprite (FUN_80028868 @0x80028b48).
  - Zeilenwechsel `08` (Sprungtabelle @0x8001099c Eintrag 7 -> 0x80028998):
      Mod-EXE (info/Re1.5/PSX.EXE, Massstab des Ports): @0x8002899c `srl v0,s3,16` / @0x800289ac `addiu v0,v0,16` -> y = Stift-y + 16,
      Zeilen bei y = 180, 196, 212.
      Echte 40%-Disc (Version_40%/PSX.EXE): @0x8002899c `lui v0,0x800c` / @0x800289a0 `lhu v0,-31434(v0)` (= 0x800b8536, Start-y) /
      @0x800289ac `addiu v0,v0,16` -> y = 180 + 16 IMMER: eine dritte Zeile ueberschriebe die zweite. Zensus aller RDTs der 40%-Disc:
      700 Seiten, 440 mit einer, 260 mit zwei, 0 mit drei Zeilen (scratchpad namen/s4/zensus.txt).
  - Letterbox (Set(1,27,1), FUN_80021a0c): zwei POLY_F4 aus der Tabelle @0x80072ecc, unterer Balken (0,216)-(320,240)
    (Halbwort @0x80072f06 = 216). Zeile 3 (y 212..227) liegt damit auf dem Balken. Alle hier betroffenen Szenen laufen im Letterbox
    (ROOM6000 sub06 @0x01072, ROOM6030 sub02 @0x00D82, ROOM10D0 sub21 @0x019FE, ROOM1150 sub08 @0x01118, ROOM2000 sub02 @0x01804,
    jeweils `22 01 1b 01`).
  -> HOECHSTENS 2 ZEILEN je Seite. Breite <= 271 px (texte_stage3a.MAX_PX, 99 % der ausgelieferten Zeilen, tuer1120_1130.c; Zeile 1 mit
     dem Namenskopf). PORT-WAHL (Darstellung): keine Zeile mit nur einem Wort; passt der Satz in eine Zeile, dann eine Zeile, sonst die
     Trennung in zwei Zeilen, deren breitere Zeile am schmalsten ist.

SEITENWECHSEL (Satz einer Aufnahme passt nicht in zwei Zeilen): zwei Seiten EINER Nachricht, jede mit dem Namenskopf (Form 6030 msg 4
aus Stufe 1, Vorbild ROOM3060 msg 7 @0x1244 `04 01 02 3c 04 00`): `... 04 01 02 N 04 00 05 <Farbe> <Name>: 05 00 00 ...`.
  Seite 1 steht sofort (`04 00` zeigt bis zum naechsten `04` in einem Bild, FUN_80028134 case 4 @0x80028334); `02 N` wartet N Bilder
  ohne Taste; Seite 2 steht N + 1 Bilder nach Seite 1 (gemessen: unit_r42_namen_seitenwechsel).
  Takt im PC-Port (platform/pc/main.c, je Bild): re15_audio_tick (~6201) -> re15_msg_tick (~6233) -> re15_game_step (~8213). Das
  Message_on im Spiel-Takt von Bild k oeffnet die Zeile, Seite 1 steht ab Bild k+1. Die Stimme wird in Bild k eingereiht und in
  Bild k+1 gestartet, aber erst NACH dem Mischen (audio_pc.c re15_audio_tick: mischen ~4011, Ereignisse abholen ~4038,
  SCD_AUDIO_VOICE_ON -> re15_voice_play) -> hoerbar ab Bild k+2 = ein Bild nach Seite 1 der Stimm-Zeile.
  PORT-WAHL: Seite 2 erscheint, wenn die Stimme das erste Wort von Seite 2 beginnt. Wort bei t (s ab Beginn der Aufnahme) -> Bild
  1 + round(30 * t) nach Seite 1 der Stimm-Zeile; diese Nachricht oeffnet S Bilder spaeter (0 bei eigener Aufnahme, RE2-Sleep 41/81
  bei Annettes Satz 2), Seite 2 also N + 1 + S Bilder nach Seite 1 der Stimm-Zeile  ->  N = round(30 * t) - S.
  Die Trennstelle: an einer Sprechpause der Aufnahme (>= 120 ms unter -40 dB, scratchpad namen/s4/pausen.py), wenn eine passt; sonst
  die Wortgrenze, an der die kuerzere der beiden Seiten am laengsten steht (namen/s4/teilung.py). Zeiten t: Whisper medium.en
  Wortanfaenge (namen/s4/asr_xa.txt, asr_10d0.txt), an Pausen das Pausenende.
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
import texte_stage3a as T  # noqa: E402

MAX_ZEILEN = 2
BILDER_JE_S = 30


def zeilen(worte, vorweg):
    """Woerter (Strings) -> 1 oder 2 Zeilen nach der Regel oben; None, wenn es nicht passt."""
    voll = ' '.join(worte)
    if vorweg + T.px(voll) <= T.MAX_PX:
        return [voll]
    best = None
    for i in range(2, len(worte) - 1):          # jede Zeile mindestens zwei Woerter
        a, b = ' '.join(worte[:i]), ' '.join(worte[i:])
        wa, wb = vorweg + T.px(a), T.px(b)
        if wa <= T.MAX_PX and wb <= T.MAX_PX and (best is None or max(wa, wb) < best[0]):
            best = (max(wa, wb), [a, b])
    return best[1] if best else None


def seitenwartezeit(t_s, s_bilder):
    """N fuer `02 N`: Seite 2 beim Wort mit Einsatz t_s (Sekunden ab Beginn der Aufnahme), Nachricht S Bilder nach der Stimme offen."""
    n = int(round(BILDER_JE_S * t_s)) - s_bilder
    assert 1 <= n <= 255, n
    return n
