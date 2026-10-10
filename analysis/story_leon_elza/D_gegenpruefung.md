# D — Unabhaengige Gegenpruefung von C_zeitleiste.md (Leon <-> Elza)

Stand 2026-10-10 (Runde 36, Spur P, Gegenpruefung). NUR ANALYSE, kein Port-Code geaendert, nichts committet.
Geprueft wird `C_zeitleiste.md` (Empfehlung, Belege, Widersprueche, Vorschlaege), gestuetzt auf `A_zensus.md`,
`B_figuren.md`, `zensus.md`, `zeitleiste.py`. Jeder Beleg wurde SELBST an den RDTs in
`re15_port/shared_assets/PSX/STAGEn/ROOMxxxx.RDT` nachgelesen (Werkzeuge `re15_port/tools/scd_dump_room.py`,
`rdt_msgdump.py`; neu: `gegen_sca.py` in diesem Ordner fuer SCA-Zellen). Offsets = Datei-Byte-Offsets,
Flags `(Bank,Bit)` hex, `msg N` = Nachrichten-Index. Unsicheres ist als **VERMUTUNG** markiert.

---

## 0. Bewertung (Kurzfassung)

**Die Empfehlung traegt — mit Aenderungen.** Modell (c) "parallel, raumweise versetzt" bleibt das einzige
widerspruchsfreie (Zyklenpruefung selbst nachgerechnet, §3). Die harten Kreuz-Belege Roy (H1), Kendo/John (H2),
Sherry (H5), Container/Schacht (H7), Hinterhof (H4), Irons (U1-Mechanik), Marvin (H9), Ada-Leiche (H10) und die
Varianten-Zuordnung xxx0=Leon/xxx1=Elza sind byte-richtig. **Falsch ist die Lesart der "Werkstatt-Tuer" ROOM11B0
(§2.1)**: sie ist keine "Nicht-Tuer", sondern eine per Skript geschaltete SCA-Kollisionszelle (3,0x0C) @0x00EC8,
bei Leon offen genau dann, wenn beim Betreten Strom ist — exakt der Nutzer-Befund "ohne Strom zu, mit Strom
offen". Vorschlag W1 ("bei Leon von Anfang an 'The door is locked.'") ist damit unhaltbar: Leon MUSS durch diese
Tuer (sub04 fuehrt ihn hindurch), und die Port-Spur J (Marvin verriegelt) kollidiert mit Elzas Rueckkehr-Zustand
(Tuer offen bei (3,0x83)=1). Uebersehen hat das Dossier (§4): das **Strom-Fenster** (10 Raumpaare lesen (4,0xF3);
daraus folgt `E13 < L11E` als ZWANG, nicht als Vorschlag), **Elzas Motorrad in Leons Lobby** (ROOM1030 Slot 11 msg 8
"whoever rode this crashed it through the front gate" = harte Kante `E02 < L06`), das **doppelt geloeste
Lobby-Notrolltor** (1030/1031, (4,0x0F)) und die Einwegtuer **10F1 -> 1091** (macht G2 unvollstaendig). Kleinere
Fehler: B_figuren nennt den Irons-Dialog-Flag "(3,0x94)" — es ist (3,0x5E) (dezimal 94); `L03 < E17` (Hubschrauber)
ist kein harter Beleg (vor der Landung steht dort ebenfalls keiner).

---

## 1. Belege nachgeprueft (eigener Befund an den RDTs)

| # | Beleg (C/B) | Ergebnis | Eigener Befund (Bytes) |
|---|---|---|---|
| 1 | Varianten-Zuordnung xxx0 = Leon, xxx1 = Elza (A_zensus §1) | **OK** | `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei = addr-0x80010000+0x800): @0x800397e4 `000427c2` = `srl a0,a0,31`, @0x800397e8 `0c004ed8` = `jal 0x80013b60`, @0x800397ec `00822021` = `addu a0,a0,v0`; Neues Spiel @0x8001d2a4 `0440001f` = `bltz v0,0x8001d324`, @0x8001d2a8 `34020017` = `ori v0,zero,0x17`, @0x8001d324 `34020003`; Basistabelle @0x8007429c = 681,684,687,...; Dateitabelle 681 @0x80070984 = 0x2b388 = 177032 B = ROOM1000.RDT (gemessen), 682 = 0x2b370 = 177008 B = ROOM1001.RDT. ROOM1240 @0x531 = 0x17 (Heliport), ROOM1241 @0x531 = 0x03 (Lobby). Dazu die Sprecher in den Texten ("Leon:" nur xxx0, "Elza:" nur xxx1). |
| 2 | H1 Roy: Leons Leiche = Elzas Szenen-Leiche | **OK** | ROOM1020 main00@0x01DEE `2d 06 00 00 01 00 0a 00 00 00 78 ee 00 00 44 d6 00 00 00 0c` = obj 6 (-4488,0,-10684) rot y=3072, UNBEDINGT (vor dem ersten Ifel_ck @0x01E10); AOT Slot 5 @0x01DDA -> sub02 Message_on @0x01F86 msg 0 "It's the RPD officer, Roy. He appears to have died recently." (msg @0x2034). ROOM1021 main00@0x01E28 `21 03 5f 01` -> @0x01E40 obj 6 mit IDENTISCHEN Bytes `78 ee 00 00 44 d6 00 00 00 0c`; (3,0x5F) wird nur in ROOM1021 sub02@0x01FE4 `22 03 5f 01` gesetzt (flag_xref Z.1253). msg 9 @0x24FB "He's dead. Poor Roy..." |
| 3 | H2 Kendo/John: Zelle = ROOM1211, Leon 1210 ohne NPC/Text, 10 Zombies frei | **OK** | ROOM1211 sub00@0x01DEA `21 04 ec 00` -> @0x01DEE Slot 8 sce 1 msg 10 rect (-21924,-22528,782,1748); @0x01E1A `21 04 fc 00` -> @0x01E36 `44 00 49 40 ... 8f 99 00 00 f4 9d` = Typ 0x49 (-26225,0,-25100); @0x01E6E `21 03 2c 01` -> 10x Typ 0x1A @0x01E86..@0x01F3A (Kill-Bits 0x8a,0x8b,0xe4..0xeb). sub02 @0x01F82 `22 04 60 01`, @0x01F8A `22 03 2c 01`. ROOM1210 main00@0x01D86..@0x01E3A dieselben 10 Records (nur Slot-Index um 1 verschoben) OHNE Ck; ROOM1210 hat keinen Nachrichtenblock (msg_sec=0); Auto-AOT Slot 6 @0x01EAE rect (-22100,-15000,5200,1700) = Elzas Slot 7 @0x01E72. ROOM1221 bytegleich (zensus "DETENTI. CELLS" same=True). |
| 4 | H3 "Prisons"-Tuer: (3,0x8B) nur Kartenleser + Leons Vorspann | **OK als Zustand / VERMUTUNG als Absicht** | ROOM1240 sub02@0x0055A `22 03 8b 01`; ROOM1241 sub02@0x0055A `22 03 6f 01`, @0x0055E `22 03 70 01` (kein 0x8B). ROOM11E1 sub01@0x01A72 `21 03 8b 00` + 4x Ck(5,0x0D..0x10) -> @0x01A8A `22 03 8b 01`. Leser ROOM11E0 main00@0x01576 `21 03 8b 00`: Text-Slot 3 msg 6 "Prisons / It's electronically locked" + Leser-AOT Slot 4 + Tuer Slot 5 sce 0 (inert); Else @0x016C4 `3b 03 02 ...` Tuer -> Raum 0x21 (1210). Einschraenkung: Der MZD-Umbau laesst Leon im SELBEN Korridor das Drahtgitter trotzdem selbst durchtrennen ((3,0x7F)/(4,0x24), 11E0 sub00@0x01798), obwohl Elza es nach derselben Logik vorher getan haette (E10 < E11 < L11E). Das offene Prisons-Flag ist also ebenso gut die Abkuerzung des Modders fuer Leons fehlende Yellow Keycard wie eine erzaehlte Spur. Fuer die Story verwendbar, aber nicht "haerter" als das Gitter, das das Dossier als Doppelung duldet. |
| 5 | H4 Hinterhof 1090/1091 | **OK (+ Zusatzbeleg)** | ROOM1091 main00@0x0216E..@0x021AA = 4 Records `44 00 10 02 01 00 00 f8 ...` .. `44 03 11 02 01 00 01 fb ...` bytegleich ROOM1090 sub00@0x023A0..@0x023DC (unter @0x0237A `21 03 bb 01`). ZUSAETZLICH: ROOM1091 obj 2 @0x02202 `6a 09 a3 f5 2e ff 40 00 40 07` (2410,-2653,-210 / rot 64,1856) = Leons NACH-Rettungs-Objekt @0x0237E (gleiche Bytes), nicht der Vorzustand @0x022AA/@0x021F2 (3060,-2754,-1560 / rot 80,1878). Kein Typ 0x26 (Feuer) in 1091, kein Nachrichtenblock, Slot 3 (Schuppen-Tuer) nur 1090 sub00@0x02352. |
| 6 | H5 Sherry: gleiche Position in beiden Szenen | **OK** | ROOM11B1 sub02@0x011FC `32 00 da c5 00 00 10 a2` = (-14886,0,-24048) nach `2e 02 00` (NPC-Slot 0 = Sherry Typ 0x4B main00@0x01044); ROOM11B0 sub04@0x01324 `32 00 da c5 00 00 10 a2` nach `2e 02 02` (NPC-Slot 2 = Typ 0x4B main00@0x010A8); @0x013AC `32 00 d0 8a 00 00 d0 8a` = (-30000,-30000) geparkt; @0x01404 `40 00 05 00 dc f8 07 ab` Leon zu (-1828,-21753). Elza waehrend des Gespraechs @0x01220 bei (-22662,-20452) = WESTLICH der Wand x=-22170 (s. §2.1), Sherry oestlich davon. |
| 7 | H7 Container/Schacht, gleiches Flag (3,0x83) | **OK** | ROOM11B1 sub00@0x010FA `21 03 83 01` -> @0x0111E `2e 03 00` + @0x01122 `34 02 58 b4` (obj 0 z=-19368); ROOM11B0 main00@0x010C0 `21 04 cd 00` -> obj 0 z=-20918 (@0x010C4), Else @0x010EA obj 0 (-2168,0,-19368). Setzer: ROOM11B0 sub06@0x01478 `22 03 83 01`, ROOM10B1 sub04@0x0195E `22 03 83 01` (flag_xref Z.1458-1463). Tuer Slot 5: 11B0 @0x01010 sce 0 / @0x01034 sce 2 (bei (4,0xCD)), 11B1 main00@0x00FFA sce 2, sub00@0x0113E Aot_reset sce 0 solange (3,0x83)==0. |
| 8 | Irons 1150/1151: Original beide lebend, msg 2, (3,0x9D) ohne weitere Leser | **OK — aber Flag-Tippfehler in B** | ROOM1151 main00@0x00E6A `44 00 45 00 ... 62 ad 30 fd ed 99` = Typ 0x45 (-21150,-720,-26131); sub01@0x00E9E `23 00 00 00 02 00` + @0x00EA4 `21 03 9d 00` -> @0x00EA8 `22 03 9d 01` -> sub03 @0x00EFA `2b 02` msg 2 "Irons: I'll be fine. Just worry about yourself for now." ROOM1150 identisch @0x00E88/@0x00EC6/@0x00ECA + Dialog sub08. **FALSCH in B_figuren §1.3:** Leons Dialog-Gate ist `21 03 5e 00` @0x00DE6 und `22 03 5e 01` @0x01110 = (3,0x5E) (= dezimal 94, vom Tool als "bit=94" gedruckt). (3,0x94) ist ein anderes Flag (ROOM4000/4020/5000/50C0, flag_xref Z.1565ff) und hat mit Irons nichts zu tun; (3,0x5E) wird ausser in 1150 nirgends gelesen (flag_xref Z.1249-1251). Betrifft C §4.2 R1-Alternativtext und B §1.3; die Zeitleiste selbst nicht. |
| 9 | Pliers brauchen in BEIDEN Varianten Strom (C §1 Korrektur) | **OK** | ROOM11B0 sub01@0x011F6 `21 04 f3 00` -> @0x01216/@0x01220/@0x0122A `46 0s 01 31 0f 00` (Slots 1, 4, 7 -> msg 15 "It's too dark to see anything... I must find way to activate the energy."); ROOM11B1 sub01@0x011A0 `21 04 f3 00` -> @0x011C0/@0x011CA/@0x011D4/@0x011DE Slots 1, 4, 7, 8 -> msg 14. Pliers = Item 0x30 main00@0x00FF2 (11B0) / @0x00FE4 (11B1), beide Slot 4, Taken-Bit (9,0x0A). |
| 10 | Kesselraum ohne Pliers erreichbar (C §1) | **OK** | SCA 11E0 @0xF7C (5x22 Zellen, alle Gruppen gleich): Zelle 0 @0x00F94 x -14100..-6800 / z -12600..-10600, Default u0=0x00, wird von sub00@0x017B0ff `37 0r 00 ff` solid gesetzt, solange (3,0x7F)==0. Sie liegt im Nord-Sued-Gang zwischen Zelle 8 (x -16598..-11882) und Zelle 1 (x -9488..-4592) = Weg zur Leichenhalle (Slot 1 @0x01532, (1000,-25000)). Die Kessel-Tuer Slot 0 @0x01512 (-27100..-23100, -10300..-8800) sitzt in der Nordwand (Zelle 10, x -29990..-19046 / z -15590..-10118) der Bucht x -26552..-23384 am Hauptgang z -5816..-2720 (zwischen Zelle 7 und Zelle 9/2) — vom Eingang Slot 2 (4200,-6700) ohne Beruehrung der Zelle 0 erreichbar. |
| 11 | H9 Marvin: 30C1 allein/ungebissen, 30E0 gebissen zurueck | **OK (sogar enger)** | ROOM30C1 sub00@0x00BEA `21 03 39 01` -> @0x00BEE Typ 0x40; msg 2 "I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.", msg 4 "He's all right and he's helping me and Ada escape the city.", msg 3 "We got separated back at the RPD." (@0x0F37). ROOM30E0 msg 3 "Marvin is out in the hall. He can't get out because of all the monsters outside" (Leon zu Ada, VOR Marvins Ankunft), msg 10 "That wound... Did those things get you?", msg 11 "The hall is no good...!" -> Elzas Treffen (30C = LOBBY = "the hall") liegt WAEHREND Leons 30E0-Szene, zwischen Adas Frage (msg 2) und Marvins Ankunft (msg 9). |
| 12 | H10 Ada-Leiche 50B0/50B1 | **OK** | ROOM50B0 sub00@0x00566 `44 00 42 40 ... e8 d1 00 00 b0 d2 00 00 00 08` (-11800,0,-11600) rot 2048 nach (3,0x62); ROOM50B1 sub00@0x004F2 `2d 00 ... e8 d1 00 00 b0 d2 00 00 00 08` gleiche Position/Rotation; msg 0 @0x055A "It's Ada. She's dead. It looks like one of those gorillas killed her." (Leon: Typ 0x29 @0x00526/@0x0053A). |
| 13 | H11 Hubschrauber nur in 1170 sub15 | **OK als Byte, NICHT hart als Zeitkante** | ROOM1170 Obj 2-5 @0x017E6/@0x01808/@0x0182A/@0x0184C nur in sub15 (Elliot-Szene, (4,0xF2)==1); ROOM1171 nur obj 0/1 @0x01224/@0x01246. Aber: auch bei LEON steht der Hubschrauber nur WAEHREND der Szene; vor der Landung und nach dem Abflug fehlt er ebenso. Elzas leeres Dach ist mit `E17 < L02` genauso vertraeglich wie mit `L03 < E17`. Kante herabstufen auf VERMUTUNG; die Modelle (b)/(b1) scheitern auch ohne sie (L20 < E21, L18 < E22, L33 < E40). |
| 14 | H12 Zug 6030/6031 | **OK** | ROOM6030 msg 1 "Leon: Where's Elza?", msg 2 "Sherry: She's out there fighting the monsters!"; ROOM6031 msg 0 "Elza: Where's Leon?", msg 1 "Sherry: He's fighting off monsters. He wants you to start the train." |
| 15 | H8 Sherry im Kanal | **OK** | ROOM2001 sub00@0x01806 `21 03 44 01` -> @0x0180A `44 00 4b 40 ...`; ROOM2000 msg 2 "There is a little girl that came in here", msg 4/5 "before meeting Ada I saw her coming in this way", msg 6 "She must have thought I was a zombie." |
| 16 | 4.1 K2-Rechteck / 4.3 frueher 3F-Zugang Elzas | **OK** | ROOM1041 Slot 2 @0x010D8 -> 1061 Cut 7 unbedingt; ROOM1061 Slot 0 @0x00D12 (Etage 8) -> 1121 unbedingt (1060 = Treppenhaus, Slots 3-10 sce 13 = Treppen); ROOM1121 Slot 2 @0x00CD8 -> 1131; ROOM1120/1121 und 1130/1131 bytegleich (zensus). Elza kann 1151 ab Spielstart erreichen. |

Zwischenfazit: 15 von 16 Belegen halten; Nr. 4 und 13 sind als Zustand richtig, als Zeitkante weicher als in C
angegeben; Nr. 8 traegt einen Flag-Tippfehler aus B. Der einzige inhaltlich FALSCHE Punkt ist die Werkstatt-Tuer
(§2.1).

---

## 2. Falsch oder unvollstaendig

### 2.1 ⛔ Die "Werkstatt-Tuer" ROOM11B0/11B1 ist eine geschaltete Kollisionszelle — der Nutzer-Befund ist Original

C §4.5: "sie ist in KEINER Variante eine Tuer ... Leon: KEIN AOT (nie etwas) ... 'Ohne Strom zu, mit Strom offen'
beschreibt die PARKPLATZ-Tuer". **Das ist falsch.** Es gibt keinen Door_aot_set, aber die Garage ist durch eine
Wand in zwei Haelften geteilt, und der Durchgang in dieser Wand ist eine eigene SCA-Zelle, die das Skript schaltet:

SCA beider Varianten identisch (`gegen_sca.py`, 11B0 = 11B1 byte-fuer-byte), SCA @0xDE4, Gruppen (0,0,5,14,0):
- r3 i07 @0x00E8C: x -22170..-21390, z -27641..-16731 — die Trennwand (780 breit, 10,9 m lang), u0=0xff solid.
- **r3 i12 @0x00EC8: x -22150..-21410, z -17230..-15230 — der Tuerdurchgang** (740 x 2000), u0=0xff solid.
- r3 i00/r2 i00 @0x00E38/@0x00DFC: Nordwand z -15300..-13300; r3 i02/r2 i01 Suedwand z -28720..-26720.
  i07 + i12 + Nord-/Suedwand schliessen die Linie x = -22150 LUECKENLOS; der einzige Durchgang ist i12.
- Westlich der Wand (x -27100..-22170) liegen: die Tuer zum B1-Korridor (Slot 0, Spawn aus ROOM1180 Slot 3 @0x00A74
  = (-26483,-18094)), die Parkplatz-Tuer (Slot 1 (-26730,-29940); Spawn aus ROOM11C0 Slot 0 @0x01712 =
  (-25552,-25331)), die Pliers (Slot 4, x -23350..-22350) und der Rolltor-Text Slot 7 (x -22400.., z -24300..-18300,
  in der Wand i07). Oestlich liegen Sherrys Auto (-14886,-24048), der Container (-2168,-20918) und der Schacht
  (-895,-21121). Elzas Text-Slot 8 "The door is locked." (ROOM11B1 sub00@0x0112A rect (-22400,-17200,1200,1700))
  liegt GENAU auf der Zelle i12.

Schaltung:
- **Leon ROOM11B0 sub00@0x01170 `21 04 06 01`** ((4,0x06) wird nur main00@0x00F56 bei (4,0xF3)==1 gesetzt,
  flag_xref Z.1879-1882) -> @0x0118C `37 03 0c 00` (Zelle (3,0x0C) u0=0 = begehbar; Handler LAB_8004175c, Port
  scd_vm.c op_sca_id_set) + @0x01190 `39 03 0c 40`, dazu (2,4) und (3,9). Ohne Strom laeuft nichts davon: Zelle i12
  bleibt solid, Leon steht im dunklen Westteil (Cuts 13/14, sub01@0x011FA) mit drei "too dark"-Texten (msg 15).
  **= "ohne Strom zu, mit Strom offen" — byte-true Original, kein Port-Defekt.** Die Szene sub04 fuehrt Leon dann
  durch genau diesen Durchgang: Start @0x012B6 (-25938,-17849) westlich, Plc_dest @0x01364 (-24181,-18582) ->
  @0x01372 (-23722,-17816) -> @0x01380 (-21755,-16024) -> @0x0138E (-20000,-15893) = Kreuzung der Zelle i12
  (x -22150..-21410 bei z -17816..-16024) -> Container/Schacht (-1828,-21753) @0x01404.
- **Elza ROOM11B1 sub00@0x010FA `21 03 83 01`**: wenn (3,0x83)==1 -> @0x01116 `37 03 0c 00` (Tuer offen) + Container
  verschoben; sonst (@0x0112A) Text-Slot 8 msg 12 "The door is locked.", Schacht inert, Zellen (2,4)/(3,9) frei.
  Ohne Strom ersetzt sub01@0x011DE den Slot 8 durch msg 14 "too dark". Elza kommt beim ersten Besuch also NICHT in
  den Ostteil: sie spricht mit Sherry DURCH die Wand (Elza @0x01220 (-22662,-20452) westlich, Sherry oestlich),
  msg 1 "You got trapped in here, right?", msg 7 "Just wait there. I'll come rescue you in a bit.", und in 10B1
  msg 1/3 "There's still a little girl trapped in there! I found her in the basement garage!" — alles konsistent
  mit einer verschlossenen Tuer, nicht mit einem offenen Durchgang.

Folgen fuer C:
1. **W1 ist unhaltbar.** "Leon 11B0: von Anfang an 'The door is locked.' vor UND nach Marvins Szene" wuerde entweder
   den Original-Durchgang zumauern (Leon kaeme nie zu Sherry/Container/Schacht) oder einen Text "locked" auf eine
   offene Tuer legen. Leons Original-Gate (Strom -> Durchgang) muss bleiben.
2. **P2 ("Elza sieht die Werkstatt-Tuer ohnehin 'locked' — keine Aenderung") gilt nur bis (3,0x83).** Bei Elzas
   Rueckkehr E21 ((3,0x83)=1, gesetzt 10B1 sub04@0x0195E VOR ihrer Rueckkehr) ist die Tuer OFFEN (Zelle begehbar,
   kein AOT). Spur J ("Marvin schliesst ... die Werkstatt-Tuer ab", P09 < E21) ist damit nur vertraeglich, wenn
   Marvins Schloss von der Garagenseite (Ostteil, wo Elza aus dem Schacht kommt) zu oeffnen ist — ein einseitiges
   Schloss/Riegel. Das muss Spur J so bauen; die Zeitleiste zwingt es.
3. **Ein echter, ungeloester Widerspruch bleibt:** Bei Elza ist die Tuer MIT Strom verschlossen (E09, Strom ist dort
   Pflicht, Beleg Nr. 9), bei Leon MIT Strom offen (L18), und `E09 < L18` ist hart (H5). Zwischen E09 und L18 muss
   also jemand aufgeschlossen haben. Marvin/Ada scheiden aus (P05 liegt in der Kette NACH L18: L18 < L12a < L12b <
   L13 < L14 < L15 < L16 < P05). Bleibt nur Sherry auf der Garagenseite — **VERMUTUNG als Erzaehlangebot:** Sherry
   hat sich eingeschlossen ("trapped"), oeffnet nach Elzas Versprechen die Tuer fuer "Elzas Rueckkehr", sieht aber
   Leon ("She must have thought I was a zombie", 2000 msg 6) und flieht in den Schacht. Kein Byte-Beleg; aber die
   einzige Figur, die zur richtigen Zeit auf der richtigen Seite ist. Alternative ohne Figur: die Tuer als
   elektrischen Riegel erzaehlen — dann muss Elzas msg 12 geaendert werden (sie hat Strom und sieht "locked").
4. **Die Frage des Nutzers beantwortet sich so:** "ohne Strom zu, mit Strom offen" ist im Original Leons einziges
   Fortschritts-Gate in der Garage (sonst waere der Generator-Umweg sinnlos). Sinnvoll wird es nur als elektrische
   Tuer. Kleinster Eingriff bei Leon: ein Text-Platz auf dem Rechteck (-22400,-17200,1200,1700) OHNE Strom mit
   Leons eigener msg 15 (@0x1C62, "It's too dark to see anything... I must find way to activate the energy.") —
   dieselbe Mechanik wie Slots 1/4/7 — und MIT Strom kein AOT (wie Original). Das erklaert die Tuer als Teil der
   stromlosen Garage, ohne Elzas Zustand anzufassen.
5. §6.5 von C ("nicht gemessen ... Messweg RE15_DEBUG_JUMP") ist damit erledigt: der Mechanismus steht in den
   Bytes; eine Laufzeitmessung ist nur noch noetig, um zu pruefen, dass der Port `Sca_id_set` (3,0x0C) in sub00
   tatsaechlich ausfuehrt (scd_vm.c:4680 op_sca_id_set, Zelle = flat[5+14 ... ] via sca_entry_at(3,12)).

### 2.2 G2 ist unvollstaendig: 1091 hat einen zweiten Zugang (10F1 Slot 1)

ROOM10F1 main00@0x00F52 `3b 01 02 31 01 00 ... e0 ca d8 dc 14 05 00 0c 00 09 00 05` = Tuer Etage 1 (-5350,-2250)
-> Raum 0x09 (1091) Cut 0, Spawn (-13600,-9000,1300) (Feuerleiter; 1091 obj 1 @0x021E0 bei y=-9000). ROOM1091 hat
keine Tuer zurueck nach 10F1 (nur Slot 0 -> 1051, Slot 1 -> 1101 @0x0213A). Mit Spur Q (Prison Key in 10F1) wird
10F1 fuer Elza PFLICHT in Phase 2 — und von dort steht der Hinterhof offen, zeitlich VOR Leons L10 moeglich
(Phase 2 ist gegen Leons Phase 1 nicht geordnet). G2 ("Tuer 1051 -> 1091 bis (4,0xE9)") muss auch 10F1 Slot 1
sperren, sonst ist das Gate wirkungslos. G1 (hinnehmen + Spur) bleibt die einfachere Wahl.

### 2.3 `L03 < E17` (Hubschrauber) herabstufen — s. Tabelle Nr. 13. Ohne Folgen fuer das Ergebnis.

### 2.4 B_figuren (3,0x94) -> (3,0x5E) — s. Tabelle Nr. 8. C §4.2 R1-Alternative "(3,0x94)" entsprechend.

### 2.5 T1/T2 (30C1 msg 3): der Ersatztext braucht eine Quelle fuer den Namen "Leon"

Original msg 2 "I'm here with Leon Kennedy." -> msg 3 "Leon?! How is he? We got separated back at the RPD."
T1 "I never ran into him back at the RPD..." setzt weiterhin voraus, dass Elza weiss, dass es im RPD einen Leon
gab. Im Original weiss sie es nirgends sonst (grep "Leon" in texte_elza.md: nur 30C1 msg 2/3 und 6031). Quelle
im Original vorhanden: T2s Streifenwagen — oder besser Leons eigener Satz in der Lobby (§4.2: das Motorrad). Wenn
T1 bleibt, braucht es einen Satz wie "Leon? ...The cop whose patrol car is parked out front?" (T2) oder Marvin
nennt ihn zuerst. Rein textlich, aber ohne Quelle widerspricht T1 dem "nie begegnet" genauso wie das Original.
Analog ROOM50B1 msg 0 "It's Ada. She's dead." — Elza hat Ada nie gesehen; sie kennt den Namen nur aus 30C1 msg 4/6.
Konsistent waere "A woman... Marvin's 'Ada'?" (VERMUTUNG, Nutzer-Wahl).

---

## 3. Logik-Pruefung von zeitleiste.py

- Zyklenpruefung nachgerechnet (Modul in-memory geladen, ohne Datei zu schreiben): Modell c 0 Zyklen; a: 1 SCC
  (85 Knoten), a1: 1 SCC (39), b: 1 SCC (75), b1: 1 SCC (37) — jeweils mit und ohne Vorschlags-Kanten. Das
  Verfahren (Tarjan-SCC + Kahn-Schichten) ist korrekt; die Modelle werden durch je EINE Zusatzkante abgebildet,
  was fuer die Fragestellung ausreicht.
- Die Ablauf-Ketten CHAIN_LEON/CHAIN_ELZA stimmen mit den gepruefen Gates ueberein (L17<L18: 11B0 main00@0x00F56;
  L18<L12a: Slot 4 ohne Strom msg 15; E08<EPW<E09: 11B1 @0x011A0/@0x011CA; E11<E12: 1211 @0x01DEA; E12<E13: 1021
  @0x01DDE; E13<E14: 1141 (4,0xF0)).
- Die Zusatzkanten aus §4 (E13<L11E, L06<EPW, E02<L06) bleiben in Modell c zyklenfrei (nachgerechnet, auch ohne
  Vorschlags-Kanten; ebenso die Variante E13<L06).
- Schwaeche der Modellierung: optionale Raeume (L20a, L21a, E91) haengen nur an Kreuz-Kanten, nicht an den
  Ketten — ihre "frueheste Position" in den Schichten ist deshalb bedeutungslos (C §6.4 sagt das selbst).

---

## 4. Uebersehene Bedingungen und Widersprueche

### 4.1 Das Strom-Fenster: (4,0xF3) wird in 10 Raumpaaren gelesen — ein Zustand, den beide sehen

flag_xref Z.2140-2179: Leser in 1040/1041 (Fahrstuhl, main00@0x0107A Slot 0 -> Text msg 1 ohne Strom), 10A0/10A1
(main00@0x00DD6), 10C0/10C1, 1120/1121, 1160/1161, 1180/1181 (dunkler B1-Korridor; mit Strom fuehrt 11B0 Slot 0
stattdessen nach 1230 "B1 CORR. LIGHT", main00@0x00F64/@0x00F88), 1190/1191, 11B0/11B1, 11D0/11D1, 1230/1231.
Das ist dieselbe Art Kreuz-Spur wie Roys Leiche: ein Zustand, den der jeweils andere sehen muesste. C modelliert
den Strom nur als Vorschlags-Kante `E13 < L17` ("Generator faellt aus"). Folgen in Modell c:
- Elzas Strom ist von EPW bis E13 an (ihr Satz 1021 msg 5 "The building's electric power is starting to fail...").
  Leon sieht in L06 (1120 @0x00C9A, 1040 @0x0107A) und L11E (10A0 @0x00DD6) DUNKLE Raeume -> keiner dieser Besuche
  darf im Fenster liegen: **`E13 < L11E` (Zwang)** und **`L06 < EPW` oder `E13 < L06`**. `E13 < L17` ist zu schwach
  um einen Schritt (L11E < L17 in der Kette) und ist keine Wahl, sondern Folge; ohne sie muesste Leon den Generator
  starten, waehrend er laeuft.
- Zwischen L06 und L11E betritt Leon keinen stromabhaengigen Raum (1050 Slots 3/4/5 -> 1000/10A0/1090; 1000 Slots
  -> 1050; 1090 Slots -> 1050/1100; keiner liest (4,0xF3)) -> das Fenster passt genau dort hinein: `L06 < EPW ...
  E13 < L11E`. Die Linearisierung in C §3.3 erfuellt das zufaellig (Phase 2 zwischen L11 und L11E); der Graph
  sollte es explizit tragen (nachgerechnet zyklenfrei).
- Elzas Besuche NACH E13 in stromabhaengigen Raeumen (E14: 1041, 1121) sind bei ihr hell ((4,0xF3) bleibt 1) —
  im gemeinsamen Modell nur nach L17 widerspruchsfrei. Mit der Nutzer-Kante `P08 < E14` ist das erfuellt; OHNE die
  Nutzer-Vorgaben (I2-Alternative) braucht es `L17 < E14` als eigene Kante.

### 4.2 Elzas Motorrad in Leons Lobby — eine harte Elza->Leon-Spur, die C nicht hat

ROOM1030 main00@0x01D6A `2c 0b 01 31 00 00 f7 d1 6a d0 4d 13 d4 08 08 00` = Slot 11 (-11785,-12182,4941,2260)
msg **8** "Seems like whoever rode this crashed it through the front gate. Wonder if they're ok..." (@0x2A70);
ROOM1031 main00@0x01DD8 derselbe Slot mit msg **7** "Seems like the crash broke the axle. Poor thing..." (@0x2C5B).
zensus.md ROOM103 fuehrt den Unterschied ("AOT nur Leon ... Text 8 / nur Elza ... Text 7"), B/C deuten ihn nicht.
Der Modder hat den Untersuch-Text also bewusst getrennt: Leon findet Elzas verungluecktes Motorrad (ihr Vorspann
1241/1031 = Sturz durch das Vordertor; msg 12 "glass from the front door all over the floor" in beiden) und
fragt sich, ob die Fahrerin okay ist. Harte Kante **`E02 < L06`** (Leons erster Lobby-Besuch). Fuer die Story ist
das die fehlende Quelle fuer "Leon weiss von einem anderen Ueberlebenden" (§2.5) — und ein Anker fuer T2.
Zyklenfrei (nachgerechnet).

### 4.3 Doppelt geloest, aber nicht in C §4.6: das Lobby-Notrolltor 1030/1031 ((4,0x0F))

ROOM1030 sub00@0x01FE8 `22 03 74 01` (Leon setzt (3,0x74) bei JEDEM Betreten); sub01@0x02184..@0x0219C:
`Ck(3,0x74)==1 & Ck(4,0x0F)==0 & Ck(5,0x20)==1 & Ck(5,0x22)==1 -> Set(4,0x0F)=1, Evt_exec sub08` = Rolltor-Szene
mit den 20 Zombies (sub08@0x02776 Se_on, Cut_replace 0/3/4/6 -> 9/10/11/12, `37 02 06 f7`/`37 03 06 f7`); danach
Slot 17 msg 0 "This should keep those things out... For now..." -> msg 1 "Guess it was only a matter of time..."
(@0x0276C). ROOM1031 identische Mechanik (sub01@0x02270ff, (3,0x74) aus 1011 sub02@0x00B06 = nach Roy) plus
sub13 mit msg 18 "Elza: That was close, this must contain them for some time...". Beide lassen also DASSELBE
Notrolltor herunter — ein dramatischer Einmal-Vorgang, nicht nur ein Schalter. Gehoert in die Liste §4.6; der
Zweite muesste (4,0x0F)=1 vorfinden. Text dafuer existiert in BEIDEN RDTs: msg 2 "The emergency shutters have
already been activated." (1030 @0x2917 / 1031 @0x2B3B, Slot 7 @0x01D1A/@0x01D88). Reihenfolge nicht belegbar
(Leon L06 in Phase 1, Elza E08 in Phase 1/2) — Nutzer-Wahl, aber eine Wahl, die C nicht stellt.

### 4.4 Weitere Kreuz-Zustaende, die C nicht nennt (schwach, der Vollstaendigkeit halber)

- ROOM1000/1001 (BATH-LOCKERS): Leons Feuerloescher 0x31 (sub00@0x00C24, Taken-Bit 134) ist bei Elza durch
  H. Gun Bullets 0x15 mit DEMSELBEN Taken-Bit 134 im selben Slot ersetzt (zensus.json ROOM100 E). Lesart "Leon hat
  ihn genommen" = `L09 < E(1001)`; 1001 ist fuer Elza optional (1051 Slot 3). Passt zu G1 ("empty extinguisher").
- ROOM1140/1141: H. Gun Bullets (tk168) nur bei Leon; Elza nichts -> konsistent mit `P08 < E16` (Leon war zuerst).
- Doppelt genommene Items (RE2-Zapping wuerde sie dem Zweiten wegnehmen): Pliers 0x30 (11B0/11B1 Slot 4, gleiches
  Taken-Bit (9,0x0A)), Blue Keycard 0x38 (1110/1111 @0x00B2E, tk 52), Minidisc (1200/1201), Red Keycard 0x37
  (1190/1191), Green Keycard 0x36 (3040/3041). C duldet die Mechanik-Doppelungen (§4.6), nennt die Item-Doppelungen
  aber nicht; mit `E09 < L18` nimmt Elza die Pliers VOR Leon — im strengen Modell haette Leon keine.
- ROOM1021 msg 4 "Elza: John, I couldn't call for help using the radio." (@0x23AC) + ROOM10F1 msg 0 "A large
  communications device. It's out of commission." = Elza war im Original VOR E13 im Funkraum (optional ueber Blue
  Keycard 1111 + Code 4312 aus 1110, Kartenleser 10D1 msg 7 "Blue Keycard"). Das STUETZT Spur Q (EPK < E12/E13)
  mit einem Original-Satz; C nennt den Satz nicht.
- ROOM2000 msg 4 "before meeting Ada I saw her coming in this way": "Ada treffen" meint das Wiedersehen auf dem
  Parkplatz (11C0, L19), nicht die Rettung (L10) — nur so passt L10 < L18 < L19. Mit der Port-Szene 10F0 (Ada
  geht mit Marvin zum Parkplatz, P05 < L19) bleibt der Satz stimmig. Kein Widerspruch, aber erwaehnenswert, weil
  der Satz isoliert gelesen L18 < L10 nahelegen wuerde.

### 4.5 Vertraeglichkeit mit den Port-Szenen (Runde 35/36)

- Szene 10F0 (K): Ada + Marvin gehen zum Parkplatz. Weg: 10F0 -> ... -> B1-Korridor -> 11B0 WESTTEIL (Slot 0 bei
  (-29430,-18940)) -> Parkplatz-Tuer Slot 1 (SW, ebenfalls Westteil). Sie brauchen die Werkstatt-Tuer NICHT.
  Vertraeglich. Spur Q (Parking-Lot-Schluessel (9,82) fuer Leon) muss nur erklaeren, wie Marvin durchkam (eigener
  Schluessel) — Nutzer-Sache.
- Spur J (Marvin verriegelt Parkplatz- und Werkstatt-Tuer, P09): Parkplatz-Tuer unkritisch (Elza hat keinen
  Schluessel, W2). Werkstatt-Tuer: s. §2.1 Punkt 2 — nur als einseitiger Riegel (von der Garagenseite zu oeffnen),
  weil Elza bei E21 von innen kommt und die Tuer offen vorfindet ((3,0x83)=1 -> `37 03 0c 00`).
- Spur L (Irons tot in 1151, Fenster 1121 kaputt): mechanisch frei (Nr. 8/16); Zeitkanten `P07 < E15`, `P08 < E14`
  wie in C. Die weiche Stelle (frueher 3F-Besuch Elzas) ist real: 1031 -> 1041 Slot 2 -> 1061 Slot 0 -> 1121 ->
  1131 -> 1151 ist ab Spielstart offen. I2/V6 (Gate (3,0x5F)) ist die sauberere Variante.
- Irons-Tod P07 in Leons Phase 3: mit Strom-Fenster (§4.1) liegt er nach E13 — Elzas Dach-Akt (E14..E17) nach P08
  ist damit auch stromseitig konsistent (hell bei beiden).

---

## 5. Bewertung der Vorschlaege in C

| Vorschlag | Urteil | Begruendung |
|---|---|---|
| K1 (Johns Satz 1211) | OK | additiv; VERMUTUNG Ausnuechterungszelle durch msg 1/2 gedeckt. |
| K2 (Leon 1210 Zellen-Text) | OK | Rechteck (-21924,-22528,782,1748) = 1211 Slot 8 nachgeprueft; 1210 ohne Nachrichtenblock -> neue msg 0. |
| K3 (leerer Haken 'Prison' in 10F0) | OK, gestuetzt | Elzas 1021 msg 4 belegt den Funkraum-Besuch vor E13 auch im Original (§4.4). |
| R1 (Roy-Leiche ab (4,0xF3)) | OK mit Praezisierung | Mit §4.1 liegt E13 sogar vor L11E; (4,0xF3) als Gate ist hinreichend. Alternativtext "(3,0x94)" in B/C -> (3,0x5E). |
| R2 (Blutlache 1010) | OK | additiv. |
| R3 (Schuetze 1041) | OK | offen lassen. |
| I1 / I2 / V6 | I2+V6 vorziehen | wegen der ab Spielstart offenen Treppe (Nr. 16). V6 braucht zusaetzlich `L17 < E14` NICHT, solange `P08 < E14` gilt. |
| G1 / G2 | G1 OK; G2 unvollstaendig | 10F1 Slot 1 @0x00F52 -> 1091 muss mitgesperrt werden (§2.2). |
| **W1** | **TRAEGT NICHT** | Leon muss durch den Durchgang (3,0x0C); "locked" auf einer offenen Tuer oder Zumauern des Original-Gates (§2.1). Ersatz: ohne Strom msg 15 am Rechteck, mit Strom offen; Elza unveraendert; offener Punkt "wer schloss zwischen E09 und L18 auf" (VERMUTUNG Sherry). |
| W2 (Elza Parkplatz immer 'locked') | OK | Folge von Spur Q/J; 11C1 hat nur Slot 0 zurueck und keine Items (zensus ROOM11C). |
| W3 | OK als Alternative | zwei Zustaende, konsistent. |
| T1 / T2 | T2 oder T1+Quelle | §2.5: Name "Leon" braucht eine Quelle; Motorrad/Streifenwagen liefern sie. |
| Vorschlags-Kante E13 < L17 | zu schwach -> E13 < L11E, dazu L06 < EPW (oder E13 < L06) | §4.1. |
| E03 < L07 (SIG), E04 < L06 (Schuss) | OK als Vorschlag | unveraendert; E02 < L06 (Motorrad) ist die haertere Kante im selben Bereich. |
| E17 < E91 | nur mit §2.2 | sonst Loch ueber 10F1. |

---

## 6. Ergebnis

- **Belege:** 15/16 OK; 2 davon weicher als angegeben (H3 Absicht, H11 Zeitkante); 1 Flag-Tippfehler ((3,0x94) ->
  (3,0x5E)); 1 Beleg FALSCH gelesen (Werkstatt-Tuer = SCA-Zelle (3,0x0C) @0x00EC8, Leon-Gate ueber (4,0x06)).
- **Logik:** Zyklenpruefung korrekt und reproduziert; Modell (c) bleibt das einzige widerspruchsfreie. Fehlende
  Bedingungen: Strom-Fenster (10 Raumpaare), Motorrad (E02 < L06), Lobby-Notrolltor (Doppelung), 10F1 -> 1091,
  Item-Doppelungen. Alle Zusatzkanten bleiben in (c) zyklenfrei; die Empfehlung aendert sich dadurch nicht, wird
  aber zwingender (E13 < L11E statt Vorschlag).
- **Unvereinbar mit Original/Port:** W1 (Original-Gate), Spur J in der naiven Form (Elzas E21-Zustand), G2 ohne
  10F1-Sperre, T1 ohne Namensquelle.
- **Urteil: TRAEGT MIT AENDERUNGEN** — W1 streichen/ersetzen, Spur J als einseitigen Riegel bauen, G2 ergaenzen,
  Strom-Kanten und Motorrad in zeitleiste.py aufnehmen, Lobby-Rolltor in §4.6 aufnehmen, Flag-Tippfehler korrigieren.
