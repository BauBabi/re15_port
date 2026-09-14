# Abgeschnittene Sprachaufnahmen — vollstaendige Erhebung (2026-09-14)

Nutzer-Auftrag: *„Außerdem gibt es diverse andere cutscenes wo die voiceovers ebenfalls
abgeschnitten werden, weil sie zu lang sind bevor das nächste startet. Ermittele sie und
verlängere dann jeweils den Bereich davor vor den Start des nächsten Dialoges, damit der
davor noch zu Ende gehen kann.“*

Dieses Dossier ist die **Ermittlung**. Es aendert keinen Port-Code (einzige Ergaenzung:
die neue Messsonde `re15_port/tests/unit/probe_stimme_takt.c` + ihr CMake-Eintrag).

---

## 0. Kurzfassung

* **87 Aufnahmen** liegen unter `synchro/STAGE{1,2}/room*/main*.wav`, verteilt auf **9 Raeume**
  (1050, 1090, 10D0, 1150, 1170, 11B0, 11C0, 1240, 2000).
* **Jede** dieser 87 hat ein `Message_on` im RDT ihres Raums — es gibt **keine unerreichbare
  Aufnahme**.
* **24 werden abgeschnitten**, weil die naechste vertonte Zeile zu frueh startet.
  Groesster Fehlbetrag: **ROOM1150 main09 mit 92,5 Bildern (3,08 s)**.
* **6 weitere sind knapp** (< 10 Bilder Reserve).
* **14 haben keinen „Killer“** — nach ihnen kommt im Raum kein weiteres *vertontes*
  `Message_on`; sie laufen aus.
* Der seit heute eingebaute Riegel (`scd_thread_t.voice_wait`, Deckel 90) deckt **23 der 24**
  Faelle ab. Er scheitert an **zwei** Stellen:
  1. **ROOM1150 main09** braucht 92,5 Bilder — der **Deckel 90** schneidet 2,5 Bilder ab.
  2. **ROOM1150 main12** (67,7 Bilder Fehlbetrag) **wird vom Riegel gar nicht erfasst**:
     das Gate `!g_scd.message_fsm_active` ist dort falsch, weil der Text der Vorzeile
     (Standzeit 121 Bilder) beim naechsten `Message_on` (Abstand 120 Bilder) noch steht.

---

## 1. Werkzeuge und ihre Absicherung

### 1.1 Der SCD-Walker (offline, Python)

`analysis/voiceover_2026-09-14/scd_zeit.py` + `stellen.py`.

Laengentabelle **uebernommen** aus `re15_port/engine/src/scd_vm.c:166` (`s_opcode_sizes`,
abgeleitet aus der Dispatch-Tabelle `PTR_LAB_800744a8` der PSX.EXE) — nicht neu geraten.
Dazu die drei **datenabhaengigen** Laengen (Bit 0x80 in `pc[3]`):
`0x2C Aot_set 28/20`, `0x3B Door_aot_set 40/32`, `0x50 Item_aot_set 30/22`.

**Blockgrenzen** (das war der Desynchronisations-Fallstrick): das Ende eines Sub ist der
kleinste Wert groesser als sein Start aus (a) allen main/sub/extra-Eintraegen und (b) allen
24 Zeigern der RDT-Adresstabelle `0x08..0x5c` (`rdt_common.c`). Ohne (b) laeuft der lineare
Scan in den naechsten Block und erfindet Opcodes.

**GEGENPROBE ROOM1170 sub02** (gefordert, hier das Ergebnis —
`analysis/voiceover_2026-09-14/gegenprobe_1170_sub02.txt`):

```
0x146E  Message_on   msg=0 mask=0000   | 2b 00 00 00
0x1472  Plc_motion                     | 3f 00 0f 00
0x1476  Sleep        50 Bilder         | 09 0a 32 00
0x147A  Plc_motion                     | 3f 00 0f 00
0x147E  Plc_flg                        | 43 00 80 00
0x1482  Sleep        51 Bilder         | 09 0a 33 00
0x1486  Message_on   msg=1 mask=0000   | 2b 01 00 00
0x148A  Sleep        100 Bilder        | 09 0a 64 00
0x148E  Message_on   msg=2 mask=0000   | 2b 02 00 00
```

Das ist exakt die geforderte Folge **msg 0 / Sleep 50 / Sleep 51 / msg 1 / Sleep 100 / msg 2**.
Ebenso reproduziert der Walker die bereits belegten Zahlen:
`msg 3 @0x14A6` → Sleep 25 + 25 + 51 = **101 Bilder** gegen **116,7 Bilder** Aufnahme
(= 15,7 ≈ die dokumentierten 16 fehlenden Bilder), und `msg 6 @0x15DC` → **100** gegen
**102,1** (die dokumentierten „102 gegen 100“).

### 1.2 Die C-Sonde (echter VM + echter Dialog-FSM)

`re15_port/tests/unit/probe_stimme_takt.c` (NEU, kein `add_test`).
Sie misst mit den **Originalfunktionen des Ports**:

* **Abstand** — `scd_thread_reseed(0, pc+4)` hinter jedem `Message_on`, dann `scd_vm_tick()`
  zaehlen, bis `g_scd.message_id` wieder gesetzt wird. (Das eigene Bild wird abgezogen:
  `Message_on` und das folgende `Sleep` liegen im gleichen Tick, weil `op_message_on`
  `1 = weiter` zurueckgibt.)
* **Standzeit** — `re15_dialog_open_mask(id,0,0)`, dann `re15_msg_tick()` zaehlen, bis
  `g_scd.message_fsm_active` faellt.

Bau/Lauf:
```
bash re15_port/tools/local_build.sh configure
cmake --build re15_port/build --target probe_stimme_takt     # msys64-PATH!
re15_port/build/tests/unit/probe_stimme_takt.exe
```
Ergebnis: `analysis/voiceover_2026-09-14/probe_stimme_takt.txt`.

**Warum beide Werkzeuge gebraucht werden.** Die Sonde ist der Schiedsrichter, kann aber zwei
Dinge nicht:
* Sie zaehlt **jedes** `Message_on` als Killer. Der Port toetet aber nur bei **vertonten** Ids
  (`re15_voice_play` kehrt vor `re15_xa_read_s` zurueck, wenn `re15_voice_load_clip`
  fehlschlaegt — `audio_pc.c:1711`). Beispiel ROOM1170 main14: die Sonde findet nach 100
  Bildern `msg15` (unvertont), das echte Budget bis `msg16` ist **200**.
* Sie bleibt in **Ankunfts-Warteschleifen** stehen (`Gosub subN` → `Do { Evt_next } Edwhile
  Ck(...)`), weil kein Spieler-Aktor laeuft. Der Walker nimmt dort den kuerzesten Weg
  (1 Bild) und liefert damit eine **Untergrenze**.

**Wo beide messen koennen, stimmen sie Bild fuer Bild ueberein** — 60 von 87 Zeilen, darunter
alle 16 Zeilen von ROOM10D0 sub21 und alle 9 von ROOM1150 sub08. Die Laengentabelle ist damit
nicht nur uebernommen, sondern gegen den laufenden VM verifiziert.

### 1.3 Die Aufnahme-Laengen

`analysis/voiceover_2026-09-14/wav_inventar.py` rechnet **genau wie der Port**:
`wav_find_data` (`audio_pc.c:1463`) klemmt die `data`-Groesse **unsigned** gegen die
tatsaechlich vorhandenen Bytes; danach `out_n = src_n * 44100 / rate` (`audio_pc.c:1659`),
Dauer `= out_n / 44100`, Bilder `= Dauer * 30`.
Kontrollrechnung an der bereits belegten Datei: `room1170/main03.wav`, 373376 B data,
48 kHz, mono, 16 bit → 3,889 s → **116,7 Bilder** (belegt: „3,89 s = 117 Bilder“). ✔

---

## 2. Was die Zeit ausmacht (Opcode-Ebene)

Im Port gibt es **genau drei** Stellen, die einen SCD-Thread Bilder kosten
(`return 2` in `scd_vm.c`, Zeilen 767 / 1121 / 1123 / 1505):

| Opcode | Kosten | Beleg |
|---|---|---|
| `0x09 Sleep` + `0x0A Sleeping` | N Bilder (u16 LE an `pc+2`) | `op_sleep` scd_vm.c:1072, `op_sleeping` :1098 |
| `0x02 Evt_next` | 1 Bild | scd_vm.c:767 |
| `0x2B Message_on` | parkt (Auswahl-Dialog bzw. neuer `voice_wait`) | scd_vm.c:1505 / :1607 |

Alles andere ist **kostenlos**: `Plc_motion (0x3F)`, `Plc_dest (0x40)`, `Plc_flg (0x43)`,
`Plc_neck (0x41)`, `Cut_chg (0x29)`, `Member_set (0x34)`, `Work_set (0x2E)`,
`Evt_exec (0x04)`, `Sce_bgm_control (0x54)`. `Wsleep/Wsleeping (0x0B/0x0C)` sind im Port
**nicht registriert** (→ `op_unknown`, 1 Byte weiter, keine Zeit); in den neun vertonten
Raeumen kommen sie in den Dialog-Subs nicht vor.

**Vierte, indirekte Zeitquelle: der globale Skript-Freeze.** `Message_on` ver-ODERt
`pc[2..3] << 16` in `g_pauseflags` (`scd_vm.c:1533`, byte-true `LAB_800404f4`
@0x80040504/08/18/1c → `FUN_80027e68` @0x80027ed0). Bit `RE15_PAUSE_SCD = 0x02000000`
(`re15_scd.h:469`) laesst `scd_vm_tick` am Kopf zurueckkehren (`scd_vm.c:586`, byte-true
`FUN_8003f038` @0x8003f040-4c) — der ganze VM steht, bis der Text weg ist.

**Fuer die Vertonung ist das irrelevant, und das ist gemessen:** *alle* `Message_on`, die
eine vertonte Zeile oeffnen, tragen Maske **0x0000**. Die Freeze-Masken in den vertonten
Raeumen gehoeren ausschliesslich zu **unvertonten** Examine-/Abfrage-Texten:

| Raum | Freeze-`Message_on` (Maske) |
|---|---|
| 1050 | msg0 @sub02 (0xff80) |
| 1090 | msg9 @sub03, msg7+msg8 @sub06 (0xffff) |
| 10D0 | msg0 @sub17, msg1-4 @sub18, msg5 @sub19, msg7/8/9 @sub20 (0xffff), msg11 @sub21 (0xff80) |
| 1150 | msg1 @sub06, msg3 @sub07 (0xffff) |
| 11B0 | msg13 @sub03 (2x, 0xffff) |
| 1170, 11C0, 1240, 2000 | **keine** |

Die Zeit zwischen zwei vertonten Zeilen besteht deshalb in **allen 24 Fehlbetrags-Faellen
ausschliesslich aus `Sleep`-Ketten** (plus gelegentlich ein `Gosub` in eine
Ankunfts-Warteschleife, die nur laenger machen kann). Beispiel ROOM1150 main09 @0x11FA:
`Sleep 35 + 35 + 35 + 35 + 10 = 150`.

**Mehrfachvorkommen:** keine der 87 vertonten Ids hat mehr als eine `Message_on`-Stelle in
ihrem Raum (ROOM11B0 msg13 kommt zweimal vor, ist aber unvertont). Die Regel „kuerzester
Abstand gewinnt“ ist implementiert (`bericht.py`), aendert hier aber nichts.

---

## 3. Der Schnittmechanismus, noch einmal praezise

1. `op_message_on` → `scd_queue_voice(msg_id)` (`scd_vm.c:1602/1634/1646`) legt ein
   `SCD_AUDIO_VOICE_ON` in die Warteschlange.
2. `re15_audio_tick` (`audio_pc.c:3047`) → `re15_voice_play(g_current_room_id, id)`.
3. `re15_voice_play` (`audio_pc.c:1708`): **`if (!re15_voice_load_clip(room, voice_id)) return;`**
   — ohne Datei **kein** Zugriff auf den Stream. Eine unvertonte Zeile toetet also **nichts**.
4. Mit Datei: `re15_xa_read_s(...)` (`audio_pc.c:1390`) setzt `s_xa.pos = 0` — der laufende
   Satz bricht im Wort ab.
5. **Zweiter Killer: der Raumwechsel.** `re15_voice_load_clip` verwirft bei
   `s_voice_room != room` den ganzen Cache und loest den Stream
   (`s_xa.active = 0`, `audio_pc.c:1563`). Betroffen waeren die letzten Zeilen vor einem
   `Aot_on`-Tuersprung: ROOM1240 msg5 (@0x05FA → `Aot_on` @0x0618 nach 80+80+2+90 = **252**
   Bildern, Aufnahme 165) und ROOM11C0 msg9 (@0x1B86 → `Aot_on` @0x1C08 nach
   40+40+40+20+5+20 = **165**+ Bildern, Aufnahme 113,5). Beide passen.

Reihenfolge im Bild (`re15_port/platform/pc/main.c`): `scd_vm_tick()` (:4284) →
`re15_audio_tick()` (:4378) → `re15_msg_tick()` (:4409). Der SCD laeuft mit **30 Hz**
(`target_fps == 30 || (frame_count & 1) == 0`, :4282).

---

## 4. Die Tabelle

`Aufnahme` = Bilder bei 30 Hz. `Budget` = Bilder, die das Skript der Zeile laesst
(Walker; mit dem VM gegengeprueft). `Fehlt` = Aufnahme − Budget.
`Stand` = Standzeit des Dialog-FSM in Bildern (Sonde). `VM` = Sondenwert des Abstands
(`-1` = Sonde steht in einer Warteschleife). `Riegel` = greift der heutige `voice_wait`?

### 4.1 ABGESCHNITTEN (24), nach Fehlbetrag absteigend

| Raum | Msg | Aufnahme | Budget | **Fehlt** | Stand | Riegel | Stelle | Killer |
|---|---|---:|---:|---:|---:|---|---|---|
| 1150 | main09 | 242,5 | 150 | **92,5** | 141 | JA (aber Deckel 90!) | sub08 @0x11FA | main10 |
| 10D0 | main24 | 190,2 | 121 | **69,2** | 121 | JA | sub21 @0x1C8E | main25 |
| 1150 | main12 | 187,7 | 120 | **67,7** | 121 | **NEIN** | sub08 @0x1252 | main13 |
| 11B0 | main08 | 176,2 | 110 | **66,2** | 107 | JA | sub06 @0x166E | main09 |
| 10D0 | main23 | 204,8 | 140 | **64,8** | 121 | JA | sub21 @0x1C56 | main24 |
| 10D0 | main18 | 149,1 | 101 | **48,1** | 101 | JA | sub21 @0x1B5C | main19 |
| 1150 | main13 | 176,2 | 130 | **46,2** | 121 | JA | sub08 @0x1262 | main14 |
| 11B0 | main05 | 183,9 | 140 | **43,9** | 137 | JA | sub06 @0x1590 | main06 |
| 2000 | main04 | 163,0 | 121 | **42,0** | 121 | JA | sub02 @0x18C2 | main05 |
| 1150 | main07 | 170,1 | 130 | **40,1** | 121 | JA | sub08 @0x11B6 | main08 |
| 10D0 | main21 | 147,7 | 110 | **37,7** | 101 | JA | sub21 @0x1BE2 | main22 |
| 11B0 | main01 | 143,7 | 110 | **33,7** | 107 | JA | sub06 @0x14EE | main02 |
| 10D0 | main20 | 171,0 | 140 | **31,0** | 121 | JA | sub21 @0x1B98 | main21 |
| 10D0 | main17 | 159,2 | 131 | **28,2** | 101 | JA | sub21 @0x1B3C | main18 |
| 2000 | main07 | 146,6 | 122* | **24,6** | 121 | JA | sub02 @0x1926 | main08 |
| 11B0 | main04 | 113,8 | 90 | **23,8** | 87 | JA | sub06 @0x1574 | main05 |
| 10D0 | main26 | 163,0 | 140 | **23,0** | 121 | JA | sub21 @0x1CF2 | main27 |
| 1170 | main03 | 116,7 | 101 | **15,7** | — | JA (Volltext) | sub02 @0x14A6 | main04 |
| 1240 | main04 | 210,0 | 202 | **8,0** | — | JA (Volltext) | sub02 @0x05DA | main05 |
| 11B0 | main11 | 106,6 | 100 | **6,6** | 97 | JA | sub06 @0x178E | main12 |
| 1150 | main04 | 127,5 | 121 | **6,5** | 121 | JA | sub08 @0x117E | main05 |
| 11C0 | main08 | 91,3 | 85 | **6,3** | 83 | JA | sub03 @0x1B7A | main09 |
| 11C0 | main02 | 93,7 | 90 | **3,7** | 88 | JA | sub02 @0x18EC | main03 |
| 1170 | main06 | 102,1 | 100 | **2,1** | — | JA (Volltext) | sub02 @0x15DC | main07 |

\* enthaelt ein `Gosub sub6` (Ankunfts-Warteschleife) → **Untergrenze**; im Spiel mehr.

### 4.2 KNAPP (Reserve < 10 Bilder) — kippt bei jeder Ungenauigkeit

| Raum | Msg | Aufnahme | Budget | Reserve | Stelle | Killer |
|---|---|---:|---:|---:|---|---|
| 11C0 | main00 | 97,5 | 101* | 3,5 | sub02 @0x18A0 | main01 |
| 10D0 | main22 | 135,1 | 140 | 4,9 | sub21 @0x1C1E | main23 |
| 11C0 | main01 | 84,6 | 90 | 5,4 | sub02 @0x18D4 | main02 |
| 1240 | main03 | 196,2 | 202 | 5,8 | sub02 @0x05BA | main04 |
| 11C0 | main05 | 81,9 | 90 | 8,1 | sub02 @0x1A5E | main06 |
| 1170 | main00 | 91,3 | 101 | 9,7 | sub02 @0x146E | main01 |

\* `Sleep 40+40+20` + `Gosub sub6` → Untergrenze 101.

### 4.3 OHNE KILLER (14) — laufen aus, kein Handlungsbedarf

ROOM1050 main08 (61 Bilder bis `Evt_end`) · ROOM1090 main02 (100), main06 (154) ·
ROOM10D0 main27 (300) · ROOM1150 main02 (260), main14 (181) · ROOM1170 main07 (100),
main16 · ROOM11B0 main00 (250), main12 · ROOM11C0 main06, main09 (165 bis `Aot_on`) ·
ROOM1240 main05 (252 bis `Aot_on`) · ROOM2000 main11.
In allen Faellen ist die Restlaufzeit des Subs deutlich groesser als die Aufnahme.

### 4.4 UNERREICHBARE AUFNAHMEN

**Keine.** Alle 87 Ids haben ein `Message_on` in ihrem Raum. (Der Ordner `synchro/unused/`
mit 179 Rohexporten wird vom Loader gar nicht gelesen — `re15_voice_load_clip` baut den Pfad
strikt als `synchro/STAGE<n>/room<ID>/main<NN>.wav`, `audio_pc.c:1575`.)

---

## 5. Defekte in den WAV-Dateien

**68 von 87** Dateien tragen `data`-Groesse **`0xFFFFFFFF`** (und `RIFF`-Groesse ebenfalls
`0xFFFFFFFF`) — nicht nur `room1170/main01.wav`. Es sind **alle** mit `LIST/INFO "Lavf58.7x"`
gemuxten 32-kHz-Mono-Exporte plus `room1170/main01`.

| Raum | defekt | gesamt |
|---|---:|---:|
| 1050 | 1 | 1 |
| 1090 | 7 | 7 |
| 10D0 | 16 | 16 |
| 1150 | 8 | 11 |
| 1170 | 1 | 11 |
| 11B0 | 13 | 13 |
| 11C0 | 10 | 10 |
| 1240 | 0 | 6 |
| 2000 | 12 | 12 |

Beispiel `synchro/STAGE1/room10D0/main24.wav` (405778 B):
`52 49 46 46 ff ff ff ff 57 41 56 45` (RIFF-Groesse defekt), `fmt ` @12,
`LIST/INFO` @36, `data` @70 mit Groesse `ff ff ff ff`; Rumpf ab 78 → 405700 B nutzbar.

**Wirkung: keine** — die Klemme in `wav_find_data` (`audio_pc.c:1489-1490`, unsigned-Vergleich
gegen `avail`) faengt das ab, und hinter `data` steht in diesen Dateien nichts mehr. Die oben
genannten Laengen sind damit korrekt. **Aber**: die Klemme zaehlt *alles* hinter `data` als
Audio — kaeme jemals ein Export mit nachlaufendem Chunk dazu, waere die Laenge zu gross.
Empfehlung: die Dateien einmal mit `ffmpeg -i x.wav -c copy y.wav` (Datei statt Stream)
neu schreiben, dann steht die echte Groesse drin.

Formate im Bestand: 69× 32 kHz/16 bit/mono, 9× 22,05 kHz/16 bit/mono, 3× 48 kHz/16 bit/mono,
6× 48 kHz/**24 bit/stereo** (ganz ROOM1240). Alle werden vom Loader akzeptiert
(`audio_pc.c:1610-1665`). Kein Format-Rueckweiser.

---

## 6. Beurteilung des heutigen Riegels

### (a) Reicht der Deckel von 90 Bildern?

Benoetigt wird genau der **Fehlbetrag**. Maximum ueber alle 24 Faelle: **92,5 Bilder**
(ROOM1150 main09, „I know. I know... but some officers were trying to repair it. you may be
luckier than them.“). Der Deckel schneidet dort **2,5 Bilder (0,08 s)** ab — hoerbar
grenzwertig, aber es ist der **einzige** Fall ueber 90. Nr. 2 ist ROOM10D0 main24 mit 69,2.

**Fazit (a): der Deckel 90 reicht fuer 23 von 24 Faellen und verfehlt einen um 2,5 Bilder.**
Der Kommentar ueber dem Deckel (`scd_vm.c:1502-1504`) begruendet die 90 mit „die laengste
Aufnahme des Projekts ist 117 Bilder“ — das galt fuer ROOM1170. Ueber den ganzen Bestand ist
die laengste Aufnahme **242,5 Bilder** (ROOM1150 main09) und der groesste Fehlbetrag 92,5.
Die Begruendung traegt also nicht mehr.

### (b) Besser: der Deckel aus der echten Restlaenge

Der Port kennt die Restlaenge **exakt**, im selben Modul, in dem er heute schon
`g_re15_voice_laeuft` stempelt:

* `s_xa.pcm_len` — Clip-Laenge in Mono-Samples bei `RE15_AUDIO_RATE = 44100`
  (`audio_pc.c:241`, gesetzt in `re15_xa_read_s` :1393-1394 aus
  `s_voice_clip[id].len`, :1700).
* `s_xa.pos` — Abspielposition, vom Mixer **1 Sample pro Geraete-Frame** vorgerueckt
  (`audio_pc.c:467 s_xa.pos++`), also 1:1 bei 44100 Hz.

Damit ist
`Restbilder = (s_xa.pcm_len - s_xa.pos) * 30 / RE15_AUDIO_RATE`
(Maximum ~10,7 Mio. — passt in `int32`).

**Sauberer Durchreichweg — genau das Muster, das `g_re15_voice_laeuft` schon benutzt:**

1. `scd_vm.c` haelt neben `int g_re15_voice_laeuft` (:1459) ein zweites
   `int g_re15_voice_restbilder = 0;`. Die Engine haelt die Variable selbst, damit das
   **PSX-Target ohne Sprachpfad linkt** (dort bleibt sie 0) — die Begruendung steht schon
   im Kommentar bei `g_re15_voice_laeuft`.
2. `re15_audio_tick` stempelt sie im **gleichen Block** wie das Flag
   (`audio_pc.c:2976-2982`, Stempel-Zeile :2982):
   ```c
   g_re15_voice_restbilder =
       (g_audio.initialized && s_xa.active && s_xa.pcm_len > s_xa.pos)
       ? (int)(((int64_t)(s_xa.pcm_len - s_xa.pos) * 30) / RE15_AUDIO_RATE) : 0;
   ```
3. `op_message_on` ersetzt die feste 90 durch
   `t->voice_wait < (unsigned)(g_re15_voice_restbilder + 2)`, mit einer **absoluten**
   Notbremse (z. B. 300 Bilder = 10 s, groesser als die laengste Aufnahme 242,5), damit ein
   haengender Kanal das Skript nicht dauerhaft anhaelt.

Vorteile, belegbar: der Riegel wartet **genau so lange, wie noch Ton da ist** — kein Fall
wird mehr abgeschnitten (auch ROOM1150 main09 nicht), und wo die Aufnahme hineinpasst,
wartet er **null** Bilder, weil `Restbilder` dann schon 0 ist. Das ist praeziser als jede
feste Zahl und braucht keine Konstante „nach Gefuehl“.

Zu beachten:
* `s_xa.pos` wird vom **SDL-Audio-Thread** fortgeschrieben. Der Lesezugriff ist ein
  `int`-Lesen ohne Sperre — genau wie das heutige `s_xa.active`-Lesen in derselben Zeile.
  Ein um ein Bild veralteter Wert ist unschaedlich (der Riegel prueft jedes Bild neu).
* Der Stempel entsteht **nach** `scd_vm_tick` im selben Bild (main.c:4284 vs. :4378), der VM
  liest also immer den Wert des Vorbildes. Das ist 1 Bild Latenz und mit dem `+2` gedeckt.

### (c) Wo ein Warten SCHADET

Der Riegel parkt **den ganzen Thread** (`return 2` ohne PC-Vorruecken, `scd_vm.c:1505`) —
nicht nur die Untertitelzeile. Alles, was hinter dem `Message_on` steht, verschiebt sich mit.

1. **Die Verzoegerung summiert sich ueber eine Szene.** ROOM10D0 sub21 hat 16 vertonte
   Zeilen mit zusammen **302 Bildern (10,1 s)** Fehlbetrag. Mit Riegel wird die
   Marvin-Szene 10 s laenger, und **jeder** `Cut_chg` darin wandert mit. Beispiele fuer
   Kamerawechsel direkt hinter einer verzoegerten Zeile: ROOM1240 msg4 @0x05DA → `Sleep 90`
   → `Cut_chg 6` @0x05E2; ROOM1170 msg4 @0x14C6 → `Sleep 100` → `Cut_chg 1` @0x14CE;
   ROOM1150 msg14 @0x127E → … → `Cut_chg 5` @0x12C4. Kein Schnitt faellt in das Warte-
   Fenster selbst (ueberall liegt mindestens ein `Sleep` dazwischen), aber die ganze
   Bild-/Ton-Choreografie verschiebt sich.

2. **SAVE-TELEFON und ITEM-BOX in ROOM1150 haengen hinter dem Riegel.** Das Gate steht
   **vor** den beiden Abfangungen:
   * Riegel: `scd_vm.c:1500-1516`
   * Save-Point: `if (re15_savepoint_is(...))` `scd_vm.c:1545` — ROOM1150 msg **1**
     (`re15_savepoint.c:36  { 0x1150, 0x01, 0 }`), `Message_on` @sub06 0x10EC
   * Item-Box: `if (... re15_itembox_is(...))` `scd_vm.c:1560` — ROOM1150 msg **3**
     (`re15_itembox.c:63  { 0x1150, 0x03 }`), `Message_on` @sub07 0x10FA

   Examiniert der Spieler Telefon oder Box, waehrend noch eine Irons-Zeile klingt, passiert
   bis zu 90 Bilder (3 s) **nichts** — kein Menue, kein Text, keine Rueckmeldung. Das Fenster
   existiert real: ROOM1150 main02 ist 124,3 Bilder lang, sein Text steht nur 101 Bilder
   (`fsm_active` faellt), main14 ist 26,8 Bilder lang. In der Luecke greift das Gate.

3. **YES/NO-Abfragen in vertonten Raeumen.** Auswahl-Texte (Steuercode `0x03` im .msg):
   ROOM1050 msg0, ROOM1090 msg8, ROOM10D0 msg7 und msg11. Der Riegel laeuft **vor**
   `re15_msg_is_choice` (`scd_vm.c:1599`), parkt also auch den Abfrage-Dialog — der Spieler
   drueckt, und das Menue kommt erst Sekunden spaeter.

4. **Latent, aber scharf, sobald jemand eine Zeile in sub01 vertont:** `scd_vm_tick` setzt
   Thread-Slot 1 **jedes Bild** neu auf `sub_scd[1]` (`scd_vm.c:601`), und
   `scd_thread_reseed` macht das mit `memset(t, 0, sizeof(*t))` (`scd_vm.c:478`) —
   `voice_wait` wird also **jedes Bild genullt** und erreicht den Deckel nie. Ein haengender
   Kanal wuerde sub01 unbegrenzt anhalten. Heute harmlos: **keiner** der neun vertonten
   Raeume hat ein `Message_on` in sub01 (nachgezaehlt, Abschnitt 2).

5. **Tuer-/Raumwechsel-Subs**: ROOM1240 sub02 und ROOM11C0 sub03 enden mit `Aot_on` direkt
   nach der letzten vertonten Zeile. Da beide Zeilen Reserve haben (252 bzw. 165 Bilder
   Budget), wartet der Riegel dort ohnehin nicht.

**Empfehlung aus (c):** das Gate zusaetzlich auf die Faelle einschraenken, in denen es
gebraucht wird — nur warten, wenn das **neue** `Message_on` selbst eine Aufnahme hat
(`re15_voice_load_clip`-Vorabfrage) **und** Maske 0x0000 traegt (also eine Kino-Caption ist,
kein Examine/Abfrage/Save/Box-Text). Damit fallen Punkte 2 und 3 komplett weg, und die
24 echten Faelle bleiben gedeckt — alle 24 haben Maske 0x0000.

---

## 7. Dateien dieser Erhebung

* `analysis/voiceover_2026-09-14/wav_inventar.py` — Laengen aller 87 WAVs, portgenau
* `analysis/voiceover_2026-09-14/scd_zeit.py` — Laengentabelle + Zeit-CFG (Dijkstra)
* `analysis/voiceover_2026-09-14/stellen.py` — Message_on-Stellen mit korrekten Blockgrenzen
* `analysis/voiceover_2026-09-14/spur.py` — Opcode-Spur zwischen zwei Zeilen
* `analysis/voiceover_2026-09-14/bericht.py` / `endtabelle.py` — Auswertung
* `analysis/voiceover_2026-09-14/fsm_aktiv.py` — Riegel-Gate-Analyse (Python-Vorstufe)
* `analysis/voiceover_2026-09-14/endtabelle.txt`, `bericht_roh.txt`,
  `probe_stimme_takt.txt`, `gegenprobe_1170_sub02.txt` — Ergebnisse
* `re15_port/tests/unit/probe_stimme_takt.c` (+ CMake-Eintrag) — die C-Sonde
