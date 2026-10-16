# Runde 38 — Spur J "rekorder" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_rekorder` (Zweig `r38/rekorder`, Basis 1bcbd0ff). Auftrag: `AUFTRAG.md` (Wortlaut Zeile 25),
Vertrag `VERTRAG.md` Spur J (Nachrichten-Ids ROOM1100/1101 40-44).

## Wortlaut (AUFTRAG.md, unveraendert)

> Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er
> fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der
> erfolgreich message auf den Panel.

---

## 1. RE-Belege (selbst gelesen)

### 1.1 Das Raetsel im Original — ROOM1100.RDT == ROOM1101.RDT (`cmp`: bytegleich, Leon/Elza)

Werkzeuge: `re15_port/tools/scd_dump_room.py` (Opcode-Walk), `re15_port/tools/rdt_msgdump.py` (Texte), Rohbytes per Python.

Nachrichten (msg_sec @0x0D9C): 1 @0x0DC7 "It's a control panel. A type of voice device is required / to open the lock.",
2 @0x0E16 "You've opened the lock.", 3 @0x0E31 "Will you use the Minidisc Player w/ Disc?" (Ja/Nein, ctrl `03`),
4 @0x0E64 "You've used the Minidisc Player w/ Disc.", 5 @0x0E94 "I don't need to use this anymore."

Ablauf (Opcode-Walk + Rohbytes):
```
sub00 @0x00AC4 Ifel Ck(3,0x67)==0 -> Platz 2 sce 1 (msg 1)         [Minidisc noch nicht genommen]
      @0x00B02 elif Ck(4,0xE8)==0 -> @0x00B14 Aot_set Platz 2 sce 3 `.. ff 00 18 07` (Ereignis sub07),
                 @0x00B3C/@0x00B50 Platz 4/5 sce 5 flags 0x44 (Cursor-Raster), @0x00B64 Obj_model_set type 4 (Cursor)
sub07 @0x00D50 `2b 01 ff ff` msg 1 / @0x00D56 `2b 03 ff ff` msg 3 (Frage) / @0x00D5C Ifel Ck(12,31)==0 (Ja):
      @0x00D64 `29 0c` Cut_chg 12 (Anzeige leer), @0x00D66..0x00D7A Set(5,0..4,6)=1 (Cursor frei)
sub01 @0x00C04 Ck(5,4,1) Member_cmp(0,0x0F,4) Sce_key_ck(1,0x40) -> Evt_exec sub08
      @0x00C2C `06 00 24 00 21 05 05 01 2e 03 00 00 06 00 16 00 3e 00 0f 00 05 00 06 00 0a 00 51 01 40 00 04 ff 18 02`
             = Ck(5,5,1) Work_set(3,0) Member_cmp(0,0x0F,5) Sce_key_ck(1,0x40) -> Evt_exec sub02   ["reinsprechen"]
sub08 @0x00D82 `22 05 04 00 29 07 09 0a 1e 00 22 05 05 01 29 08 01 00` = Set(5,4,0) Cut_chg 7 Sleep 30 Set(5,5,1) Cut_chg 8
sub02 @0x00C68 Set(4,0xE8,1) ... @0x00C8C `22 02 07 01` Set(2,7,1) (Spieler gesperrt)
      @0x00C90 `2b 04 ff ff`  Message_on 4 "You've used the Minidisc Player w/ Disc." (Pausemaske 0xffff)
      @0x00C94 `02 00`        Evt_next + Nop (wartet, bis die Nachricht zu ist)
      @0x00C96 `29 0c` @0x00C98 `09 0a 05 00`   Cut_chg 12 (leer), Sleep 5
      @0x00C9C `29 0b` @0x00C9E `09 0a 05 00`   Cut_chg 11 (ACCEPTED), Sleep 5     <- erstes ACCEPTED
      @0x00CA2 `29 0c` / @0x00CA8 `29 0b` / @0x00CAE `29 0c` / @0x00CB4 `29 0b`, je Sleep 5 (ACCEPTED blinkt 3x)
      @0x00CBA `2b 02 ff ff`  Message_on 2 "You've opened the lock."
      @0x00CBE `02 00` Evt_next / @0x00CC0 `22 02 07 00` Set(2,7,0) / @0x00CC4 `29 06` Cut_chg 6 / @0x00CC6 `3c 01` Cut_auto 1
      @0x00CC8 Aot_reset Platz 1 sce 2 (Tuer nach ROOM1110 frei, main00 @0x009BA), Platz 2 sce 1 (msg 5), Platz 3 sce 0
```
Bedeutung der Cuts 7..12 (Anzeige des Panels): Runde 37 E §0 "SYSTEM READY / SAY HELLO / INVALID / TRY AGAIN / ACCEPTED /
leer" — am Hintergrund selbst nachgesehen in §2 (Messung).

RE1.5 spielt in dem ganzen Ablauf KEINEN Ton (kein `36` Se_on in sub02/sub07/sub08); seit Runde 36 Spur G haengt die
RE2-Folge Piep (+0) / Aufschliessen (+15) an Message_on 2 (`gen/r36_aufschliessen.inc`: `{ 0x1100, 2, KARTE_AUF, 2 }`,
`{ 0x1101, 2, KARTE_AUF, 2 }`, "sub02 @0x00CBA").

### 1.2 RE2-Form fuer "Piep + Erfolgsanzeige im selben Takt, Aufschliessen danach" (ROOM2110 sub08, selbst gelesen)

`info/re2leon/PL0/RDT/ROOM2110.RDT`:
```
@0x01B2C 29 0c                            Cut_chg 12 (Kamera auf den Leser)
@0x01B4C 36 02 0c 00 ..                   Se_on(2,0x0C) Karte ein
@0x01B58 09 0a 0f 00                      Sleep 15
@0x01B5C 36 02 0d 00 ..                   Se_on(2,0x0D) PIEP
@0x01B68 65 01 65 02                      Lampen aus                  } im SELBEN Skript-Takt wie der Piep
@0x01B6C 64 01 16 08 .. / @0x01B7C 64 02 16 08 ..  Lampen BLAU (offen)  } (kein Sleep dazwischen)
@0x01B8C 09 0a 0f 00                      Sleep 15
@0x01B90 36 02 0b 00 ..                   Se_on(2,0x0B) AUFSCHLIESSEN
@0x01B9C 29 09 3c 01                      Kamera zurueck
```
Die Saetze liegen seit Runde 36 als Mini-Bank `shared_assets/RE2/KARTSE.VBS` (`gen/re2_karten_bank.inc`):
Satz 1 PIEP = RE2 ROOM2110 Satz 0x0D (VAG 4640 B, sha1 b03b6dff...), Satz 2 AUF_E = RE2 ROOM2110 Satz 0x0B (VAG 11264 B,
sha1 da805fa3...). Dieselben Saetze und derselbe Abstand 15 spielen die Kartenleser und Code-Panels
(`aufschliessen_r36.c` k_folgen[KARTE_AUF] = { PIEP +0, AUF_E +15 }, Sleep-Semantik RE2 @0x800539dc/@0x80053a24).

### 1.3 Die Ton-Datei des Nutzers

`synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav` (169458 B, committet e2cc7673): RIFF/WAVE, `fmt ` @12 Laenge 16:
Tag 1 (PCM), 1 Kanal, 32000 Hz, 16 bit; `data` @36, Laenge (u32 @40) 169414 B = 84707 Samples = **2,647 s =
79,4 Spielbilder** (30 Hz). Spitze 30146 (92 % Vollaussteuerung -> der Lader hebt nichts an, audio_pc.c Ziel 14745).
Sie liegt NICHT im Schema `main<NN>.wav` (synchro/README.md) -> eigene Bindung noetig; `unit_synchro_stimme` meldet sie
bisher als "noch nicht zugeordnet".
