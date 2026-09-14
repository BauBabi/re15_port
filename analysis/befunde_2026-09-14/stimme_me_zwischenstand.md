# "They almost caught me" — das abgeschnittene Wort, Zwischenstand 2026-09-14

Nutzer: *„Das 'Me' von 'They almost caught me' im Intro wird noch immer abgeschnitten."*

## 1. Die Zeile ist identifiziert — ROOM1170 **main03**

Dekodiert aus `re15_port/shared_assets/PSX/STAGE1/ROOM1170.RDT`, MSG-Sektion @0x1920
(Adresstabellen-Eintrag 13 @0x3C), Nachricht 3 @Sektions-Offset 0x00BE:

    THOSE THINGS ARE ALREADY CLOSE, THEY ALMOST CAUGHT ME.

Zeichenkodierung: Byte + 4 = ASCII, 0x00 = Leerzeichen (belegt am Klartext
`50 44 41 4E 41` -> `THERE`). 17 Nachrichten insgesamt.

**Damit ist die frühere Arbeitsannahme „main01" widerlegt** — main01 ist die Pilotenzeile
„IS THERE NO ONE ELSE LEFT INSIDE".

## 2. Die Aufnahme ist intakt — der Schnitt passiert im Spiel

`synchro/STAGE1/room1170/main03.wav`: 48000 Hz, mono, 16 bit, 186688 Samples
= **3,889 s = 116,7 Bilder**. Pegel über die letzte Sekunde (Spitze von 32767):

    t=-1,0s 1322 | -0,9s 3092 | -0,8s 3508 | -0,7s 846 | -0,6s 1687
    t=-0,5s 1728 | -0,4s  518 | -0,3s  323 | -0,2s   53 | -0,1s  47

Gesprochen wird also bis etwa **3,6 s (Bild ~108)**, danach nur noch Ausklang.
Die WAV-Heilung vom selben Tag hat main03 nicht angefasst (sie war nicht unter den 68
defekten; Datum unverändert Jun 26).

## 3. Der Killer ist belegt — Message_on 4 @0x14C6, Budget 101 Bilder

Linearer Scan von sub02 (ROOM1170.RDT), Opcode-Längen aus `s_opcode_sizes`:

    0x14A6  2B 03 00 00   Message_on 3     <- "THEY ALMOST CAUGHT ME"
    0x14AA  3F 00 10 00   Plc_motion
    0x14AE  09 0A 19 00   Sleep  0x19 = 25
    0x14B2  3F 00 10 00   Plc_motion
    0x14B6  43 00 80 00   Plc_flg
    0x14BA  09 0A 19 00   Sleep  0x19 = 25
    0x14BE  3F 00 11 00   Plc_motion
    0x14C2  09 0A 33 00   Sleep  0x33 = 51
    0x14C6  2B 04 00 00   Message_on 4     <- der Killer
    0x14CA  09 0A 64 00   Sleep 100
    0x14CE  29 01         Cut_chg 1

Budget = 25 + 25 + 51 = **101 Bilder** gegen 116,7 Bilder Aufnahme.
**Fehlbetrag 15,7 Bilder (0,52 s)** — das ist genau das Wort am Ende.

## 4. Der Riegel IST ausgeliefert und IST mechanisch korrekt

* Erster Anlauf `43cf78eb` -> Tag **v0.7.99**; die endgültige Fassung gegen die echte
  Restlänge `0fe17503` -> Tag **v0.8.0**. Der Nutzer spielt also damit.
* `scd_vm.c:1635` gibt `SCD_R_YIELD` (== 2, `scd_vm.c:560`) zurück, **ohne** den PC
  vorzurücken. Die Ausführungsschleife `scd_vm.c:634` bricht darauf für dieses Bild ab —
  das Parken funktioniert wie gedacht.
* `test_voice_ausreden.exe`: **9 von 9 PASS**, einschließlich des ROOM1170-msg3-Falls.

## 5. Was die Tests NICHT abdecken — und wo die Ursache liegen muss

Der Unit-Test setzt `g_re15_voice_laeuft` und `g_re15_voice_restbilder` **direkt**
(`test_voice_ausreden.c:35-36`). Er prüft damit die **Logik**, nicht die **Datenquelle**.
Beide Werte entstehen allein in `re15_audio_tick` (`audio_pc.c:2983-2989`):

    g_re15_voice_laeuft    = (g_audio.initialized && s_xa.active) ? 1 : 0;
    g_re15_voice_restbilder = laeuft ? ((s_xa.pcm_len - s_xa.pos) * 30) / 44100 : 0;

Statisch geprüft und **ohne Befund**:
* Reihenfolge: `scd_vm_tick` (main.c:4284) vor `re15_audio_tick` (main.c:4378), beide in
  der Hauptschleife, jedes Bild — 1 Bild Latenz, nicht 16.
* Längenrechnung: `int64_t out_n = (int64_t)src_n * 44100 / rate` — der 32-bit-Überlauf
  (186688 * 44100 = 8,23e9) ist bereits abgefangen; 171507 Samples @44100 = 3,889 s.
* Voice-Warteschlange: 16 Plätze (`SCD_AUDIO_QUEUE_SIZE`), verwirft erst darüber.
* Abbruchstellen: nur `audio_pc.c:463` (Clip-Ende), `:1378` (Init) und `:1563`
  (**Raumwechsel**-Cache-Leerung). main03/main04 liegen im selben Raum.

## 6. Warum hier nicht weiter gemessen werden konnte

Diese Sitzung rendert kein Fenster. Fünf Läufe mit `RE15_AUDIO_CAP_SYNC` (Mixer ohne
SDL-Gerät, läuft schneller als Echtzeit) erreichten **kein einziges Message_on** —
`stimme.log` blieb leer, die Läufe endeten nach 603 / 1210 / 2999 Bildern mit exit=1.
Ohne Fensterfokus kommt der Titel-Confirm nicht an.

## 7. Nächster Schritt: die Schiene misst jetzt in beide Richtungen

`scd_vm.c` schrieb bisher **nur, wenn gewartet wurde** — ein Riegel, der gar nicht
anspringt, hinterliess damit eine leere Datei, und daraus folgt nichts. Die Schiene
schreibt jetzt bei **jedem** Message_on:

    raum=1170 nachricht=3  laeuft=1 rest=0  0 gewartet

Damit trennt eine einzige Zeile die drei verbliebenen Möglichkeiten:
* `laeuft=0` -> der Kanal meldet sich nicht als aktiv (Datenquelle).
* `laeuft=1 rest=0` -> die Restlänge ist zu früh auf 0 (Positions-/Längenrechnung).
* `laeuft=1 rest>0` und trotzdem `0 gewartet` -> der Riegel wird nicht erreicht.

Aufruf: `RE15_STIMME_LOG=1` setzen, Intro spielen, `stimme.log` neben der exe lesen.
