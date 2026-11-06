# Spur S (spinnengore), Abnahme 1 (Runde 41, 2026-11-01)

Ich habe als unabhängiger Abnehmer geprüft und dem Bau nichts geglaubt. Geprüft wurde der Zweig `r41/spinnengore` mit Kopf
`4e7eb80cd` (Commits `ebfd10c31`, `9fb1c73e2`, `4e7eb80cd` auf Basis `aa88a1b6a`).

Der Bau ist selbst gelaufen, über `local_build.sh configure` und `local_build.sh build`. Ninja meldete „no work to do“, die exe
war also aktuell. Nach der Gegenprobe (Abschnitt 3) habe ich den Stand wiederhergestellt und neu gebaut. Die exe ist danach
vom `2026-11-01 19:20:21`, `git status` ist sauber.

Alle Fensterläufe liefen über die eigene Messkopie `re15_pc_S41abn.exe` mit dem beschleunigten Renderer. Ich habe weder
`AUTOSHOT` noch `SOFTWARE_RENDER` gesetzt und auch kein `TITLE_SHOT`. Für Titel und Spielerwahl liefen die echten Menüs
(`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`, für Elza zusätzlich `_SWITCH`). Danach kam der Sprung über das Debug-Menü
(`RE15_DEBUG_JUMP`). Aufgenommen habe ich mit gdigrab am Fenster-Handle der eigenen PID, mit 10 Bildern/s. Skript und
Rohbilder liegen unter `scratchpad/S41abn1/`, die Belege unter `S_belege/abn1/`.

## Urteil: NICHT bestanden

Der Kern hält. Unter der Standard-KI platzt die Spinne bei einem Magnum-Treffer sichtbar, mit Leon und mit Elza. Das habe ich
selbst im Fenster belegt. Die zitierten Adressen stimmen mit den Bytes überein, auf dem Mod-Baum und auf der Disc. Ohne den Bau
sind die Riegel rot, und der Schalter wirkt.

Es bleibt aber ein Mangel genau am Spinnen-Gore, also am Gegenstand der Nutzerfrage. **Die Blutfarbe der Spinne ist falsch: rot
statt grün.** Dazu kommt, dass die Gore-Liste der Videos im Ergebnis nur teilweise als offen benannt ist (Abschnitt 5).

## 1. Eigene Läufe im echten Fenster, Standard-KI, Super Redhawk (w7)

Die Spinnen-Messschiene `RE15_SPINNEN_LOG` mit der `{G..}`-Klammer habe ich mit einem eigenen Auswerter gelesen
(`scratchpad/S41abn1/ana.py`).

| Lauf | Raum | Ergebnis Log | Ergebnis Bild |
|---|---|---|---|
| a Leon | ROOM2070 | F68 Slot 2: st 3, Zeile 5, +0x93 = 0x41, o=1 (RE2-Gehirn), **ta=1, sh=80001, fl=7fff8**. F122 Slot 1 ebenso | Aufnahmebilder 276–285: gelbgrüne Scherben fliegen strahlenförmig, eine **rote** Wolke steht am Rumpf, die Beine liegen am Boden (`a01`) |
| b Elza | ROOM2071 | F95 Slot 2 und F176 Slot 1 wie bei Leon (HP -20/-19, 0x41, sh 80001, fl 7fff8) | Bilder 278–286: Platzen wie bei Leon. Ab Bild 283 fliegt eine Scherbe bis vor die Kamera (`a02`) |
| c Leon, `RE15_KEIN_80PROZENT_GORE=1` | ROOM2070 | F68 Slot 2: st 3, Zeile 5, 0x41, **ta=0** | Bilder 382–399: Die Spinne dreht sich in Clip 12 auf den Rücken und bleibt ganz, es fliegen keine Scherben (`a03`) |

- Die Babys (Typ 0x26) kommen im Todesbild in freie Plätze. Bei allen steht ta=0, keines erbt also die geplatzten Teile
  (Platz leeren in `re15_actor_alloc`).
- Nach der Landung verschwinden einige Oberschenkel (un von 0x0a8a8 bis 0x8a8a9). Das ist die Beta-Eigenheit aus Dossier
  Offen 8. Die Bytes habe ich geprüft: `lh v0,58(a2)` / `bne` @0x8001f104-10c, `lw v0,24(s1)` / `bne` @0x8001f114-11c,
  `and v0,v0,-2` / `sw` @0x8001f12c-134, Mod = Disc.
- Die Scherbe, die bis vor die Kamera fliegt, habe ich ebenfalls nachgeprüft:
  - Die Formel steht @0x80023528-78: `lhu`/`sll 16`/`sra 21` mal Tabelle, y mit `addiu v0,v0,300`/`sra 6`.
  - Die Tabelle @0x80073454 lautet {30,59,…,465}. Sie stimmt Wert für Wert mit `s_scherben_k` in `teile_r15.c` überein.
  - Die Weite ist also Mechanik des Originals, wie der Bauer schreibt.

## 2. Zitierte Adressen gegen die Bytes

Disassembliert habe ich mit einem eigenen Wrapper (`scratchpad/S41abn1/dis2.py`). Er liest jede Stelle im Mod-Baum
`info/Re1.5` und auf der Disc `Version_40%` und markiert jede Abweichung. Die Dateien als Ganzes unterscheiden sich
(STAGE1/STAGE2/PSX.EXE). **An allen geprüften Stellen ist die Zahl der Abweichungen 0.**

| Stelle | Gelesen | Stimmt |
|---|---|---|
| Krit, PSX.EXE @0x80012370-bc | `andi v0,v0,0x1`/`sb` (+0x93 &= 1), `ori v0,zero,0x8`/`bne`, `lw v0,-2592(v0)` (0x8008f5e0), `sltiu v0,v0,0xbb8`, `ori v0,zero,0x7`/`bne`, `ori v0,v0,0x40`/`sb v0,147(s1)` @0x800123b4-b8 | ja: w7 immer, w8 unter Abstand 3000, ohne Zufall |
| +0x5 = Waffe, @0x800124bc | `sb fp,5(s1)` | ja |
| DEATH-Wurzel @0x80113fc0-fe0 | `lbu v0,5(v0)`, `addiu at,at,-28872` (0x80118f38), `jalr v0` | ja |
| Tabelle @0x80118f38 | Index 7–11, 13, 16–18, 21 -> 0x80113ff8; 0–6, 12, 14, 15, 19, 20 -> 0x8011461c | ja (= `re15_spinnengore_platzwaffe`) |
| Phase 0 @0x8011403c-14c | Blut `jal 0x80019700` @0x801140c4; Teil 0: `lui a3,0x20`/`ori 0x7f60`, `sw a3,104(t0)`, `sh` {0,-50,0,3,0,0} @+148..158, `ori v0,v0,0x4a`; Teil 19 dasselbe über +3268/+3372/+3416.. | ja |
| Blut 0/19 @0x80114164 / @0x8011418c | `addiu a2,t0,64` bzw. `addiu a2,a2,3332`, SE 5 @0x80114190 | ja |
| Beine @0x8011419c-52c | +0x1e0 = 9 und `--`; Stride 344, +172; `andi v1,a0,0x41`/`bne` (==1) @0x80114218-21c; `ori v0,a0,0x1062` @0x80114220, B @0x80114264; `sb t2(20),289`, `sh t1(-128),302`, `sb a3(8),290`, `sh 0/-100/0` @228/230/232 und +344 | ja (Paare 2i+1/2i+2, i = 8..1) |
| HURT @0x80113b50-94 | Blut @0x80113b50, SE 2, `andi v0,v0,0x40` @0x80113b74, `jal 0x8001af20`, `andi v0,v0,0x7` | ja |
| Tabelle @0x80118f28 | `03 05 07 09 0b 0d 0f 11` | ja |
| Krit-Bein-Farbe @0x80113c98/cdc-cfc, Blut @0x80113dc4-c8 | `ori t0,t0,0x7f60`, `sw t0,104(v1)`, {0,-50,0,3,0,0}, `ori 0x4a`; `addiu a2,a2,408` | ja |
| 0x8011461c | Blut @0x80114654 bei +0x7 == 0, Krit `andi v0,v1,0x40` @0x801146b0 nur in Phase 1/3 und mit `+0x93 & 1` | ja |
| RE2 EMS25 FUN_80105BF0 | `sltiu v1,v1,0x3` @0x80105c20, Teile 2k+3/2k+4, `ori 0x4a`, 600/-120/10/-100, `lui a1,0x2f`/`ori 0x2f2f`, +0x220 `sb` @0x80105c64 | ja |
| RE2 FUN_80104890, Aufrufe | Teil 19 `ori 0x4a` @0x801048cc, -240/15, 0x2f2f2f, 1+4x `jal 0x801056dc`. Aufrufe @0x80104588 und @0x80104848; Zeile 9 `jal 0x801044d0` @0x80104a2c; Säure Teil 19 @0x80104bcc-bf4; +0x220 `sb` @0x80103a18 | ja |
| RE2 Hund EMD0G_MOD0 0x80104440 | Tabelle @0x80105680 = `02 03 04 07 08 09 0a`, `ori v0,v0,0x4a` @0x801044a4, 800/-150/10/-100, Farbe `0x101040` (`lui a2,0x10`/`ori 0x1040`), `sltiu v0,a1,0x7` | ja |
| STAGE1 DEATH-Tabelle, Zeilen 7/8/19 @0x8011ff8c/@0x8011ffac/@0x8012010c | Spalte 0 jeweils 0x80108abc | ja (Zeilenabstand 32 ab 0x8011feac) |

Schönheitsfehler: Der Kommentar in `spinnengore_r41.c` (`re15_spinnengore_platzwaffe`) nennt die Tabelle „24 Worte“. Die
Waffentabelle hat aber nur **22** Einträge. Wort 22 und 23 (0x80114e60, 0x80114ec8) zeigen auf Code einer anderen Tabelle.
Code und Unit-Pin prüfen richtig 22.

## 3. Sind die Riegel ohne den Bau rot?

Für die Gegenprobe habe ich sechs Dateien auf `aa88a1b6a` zurückgesetzt: `actor_common.c`, `enemy_ai_common.c`,
`enemy_ai_re2_spider.c`, `enemy_ai_re2_zombie.c`, `enemy_ai_re2_dog.c` und `platform/pc/main.c`. Modul, Test und
Messschiene blieben stehen. Gebaut habe ich mit `local_build.sh build`.

- `test_r41_spinnengore.exe`: **18 Fehler**, nämlich B, E, F x2, G x2, H, I, J, K x10. Das passt zu den 8 + 10 Fehlern des
  Bauers.
- `integration_r41_spinnengore`: **rot**. L7 und E7 melden „Platzen 0, Teilschicht 0“, L8 meldet „Bein 0“. L7AUS bleibt wie
  erwartet grün.
- Danach habe ich den Stand wiederhergestellt (`git checkout HEAD --`) und neu gebaut. Die Unit meldet wieder
  `OK unit_r41_spinnengore`, `git status` ist sauber.

## 4. Die Gore-Inventur in Stichproben selbst gemessen

- **Sonde** `probe_r41_gore_inventur` selbst gestartet: Die Ausgabe stimmt Zeile für Zeile mit dem Dossier überein. Spinne w7
  unter beiden KIs: sh=80001, fl=7fff8. Hund unter RE2: bridge=1, p40=0079c. Krähe: p40=01fff. Würgearm: owner=RE2 in
  beiden. Gorilla und 0x29: owner=RE1.5.
- **Hund 0x20, RE2-Teile-Wurf im Fenster** (Lauf d, ROOM1190, Leon, Ereignis 10 erzwungen, Redhawk):
  - Laut Log sterben drei Hunde an Waffe 7 (F632, F758, F821).
  - In den Aufnahmebildern 652–665 platzt der Hund vor Leons Füßen in **dunkelrote Dreiecke** auseinander. Das ist die Farbe
    0x101040 aus @0x801044c8. Die Dreiecke fliegen etwa 1 s strahlenförmig (`a04`).
  - Damit ist Offen-Punkt 5 des Bauers („Bild fehlt“) geschlossen.
  - Eine A/B-Gegenprobe im Fenster habe ich nicht gemacht, weil die Hund-Brücke an keinem Schalter hängt. Dass die Dreiecke
    vom Bau stammen, folgt aus dem Code: Vorher kannte `re15_re2z_gore_active` den Typ 0x20 nicht.
- **MC51 nach unten** (Lauf f, ROOM1140, Messhaken `RE15_ZGORE_TEST=320:18:0`, also RE2-Zeile 18, Spalte 0): Slot 2 stirbt in
  F74 und steht in F127 mit HP 10 wieder auf (st=1). Das ist der Weg 7438 mit `death=1`. Vor dem Bau wäre es MAIN gewesen,
  also der Sturz zur Leiche.
  - Das Bild taugt nicht als Beleg, weil Leon im Zombie-Pulk steht.
  - Im normalen Spiel hängt Spalte 0 am Tief-Zielen (Spalte = Zone + 3*Klammer). Ein freier MC51-Lauf (Lauf e) hat einen Tod
    in F41 geliefert, aber ohne Spaltenprotokoll. Ob die Beinzone dort traf, ist offen.
  - Am Rande: Der Kommentar über `re2z_hit_knockdown` behauptet noch, Spalte 0 sei „im Spiel unerreichbar“. Das ist seit
    Runde 16 überholt (`re15_damage.c` ~3066: `zone + 3*bracket`, TIEF -> Beine). Der Kommentar ist vorbestehend und nicht
    vom Bau.
  - **Folge für das Spiel**, nicht nur Gore: Ein tödlicher MC51-Treffer in die Beine tötet unter der Standard-KI nicht mehr. Der
    Zombie steht als Kriecher mit HP 10 wieder auf (`rand&3`-Zweig). Das deckt sich mit RE1.5 @0x8012010c, dort steht er mit HP
    30 auf. Es ist aber eine Änderung am Treffer-Ausgang. Für die Nutzerregel „RE2 = Treffer“ gehört das in die Release Notes.

## 5. Das Maximum aus RE1.5, RE2 und den Videos je Typ (Stichprobe am Video)

Für die Stichprobe habe ich selbst aus `Version_80%/Resident Evil 1.5 (All Footage).mp4` geschnitten, mit 10 Bildern/s.

- **S01 (B 5:17.6–5:19.9)**: Gelbgrüne Brocken fliegen in alle Richtungen. Es ist **keine rote Wolke** zu sehen. Danach liegt
  der dunkle Kadaver mit Beinen (`a06`).
- **S02 (B 5:30.8–5:33.0), Bild 18 = B 5:32.5**: Ein deutlicher **grüner Fleck** sitzt an der getroffenen Spinne. Bei
  **S05 (B 7:40.4)** ist ein schwacher grüner Fleck zu sehen (`a05`). Die Aussage „grüne Treffer-Spritzer“ aus
  `S_video_gore.md` S02/S05 bestätige ich damit.

### MANGEL 1: Das Spinnenblut ist im Port rot, im Original und im Video grün

Der Bau setzt an der Spinne sechs RE1.5-Blutsätze `0x00082000` (`spinnengore_r41.c` Z. 93/96/97/110/111/150). Das RE1.5-Gehirn
setzt dazu weitere (`enemy_ai_common.c` 11424/11434/11435/11467). Das Dossier nennt sie byte-true „Blut 0x82000“. Der
Port-Helfer, über den sie laufen, wirft aber das Bit weg, das im Original die Farbe wählt.

So zerlegt das Original den Parameter, FUN_80019700 in PSX.EXE (Mod = Disc):

```
8001970c: srl  v0,a0,16          ; Byte (a0>>16)
80019728: srl  t8,a0,24          ; Effekt-Id
80019730: andi t7,v0,0xff
80019734: andi v0,t7,0x7         ; Sub-Index (Zeilenstrom)
8001973c: srl  v1,t7,3           ; obere 5 Bits
80019754: sll  s0,v1,6           ; * 0x40 = CLUT-Zeile
8001987c: lhu  v0,4(t5)          ; EFF-Kopf +4 = CLUT-Wort
80019884: addu v0,v0,s0
80019888: sh   v0,50(t0)         ; Platz +0x32 = CLUT
```

Für `0x82000` ergibt das Effekt 0, Sub 0 und **CLUT + 0x40**, also die nächste Palettenzeile. Effekt 0 kommt aus CORE00.ESP,
denn die Raumbank von ROOM2070 hat nur die Ids 06/0d/0c. Sein Kopf trägt das CLUT-Wort 0x7951, also x 272, y 485.

TEX.TIM (Disc = `shared_assets`, an x 272 für y 485/486 byteweise geprüft) hat dort zwei Zeilen (`a07`):

- y 485 = `601010 580808 500808 …` ist **rot**. Das ist das Zombieblut `0x2000`.
- y 486 = `185008 184808 184008 203810 …` ist **grün**. Das ist das Spinnenblut `0x82000`.

Der Port ruft `re15_enemy_fx19700` auf (`re15_damage.c:3382`):
`re15_esp_fx_spawn_ex(..., (uint8_t)(a0 >> 24), (uint8_t)((a0 >> 16) & 7), ...)`. Das `& 7` löscht die CLUT-Bits. Der Zeichner
`re15_pc_esp_clut` (`fx_plattform_pc.c:63-70`) rechnet zwar richtig `hdr + (sub_index >> 3) * 0x40`, bekommt aber Sub 0 und
greift damit zur roten Zeile 485.

Am Bild gemessen: In Lauf a, Bilder 277–279, steht eine große **rote** Wolke am platzenden Rumpf (`a01`). Das Video zeigt an
derselben Stelle keine rote Wolke, sondern grüne Brocken und Flecken.

Der Helfer selbst stammt nicht aus diesem Bau. Der Bau verkauft die Farbe aber als byte-true RE1.5, und er zieht sie jetzt in
das sichtbare Platzen unter der Standard-KI hinein. Dazu fällt der Mangel genau unter die Nutzerregel „Maximum inklusive
Videos“, denn S01/S02/S05 sind grün. In der Inventur und in der Offen-Liste des Bauers fehlt er.

Nachbesserung: Das ganze Byte `(a0>>16)&0xff` muss zum Zeichner kommen, und nur für die Strom- und Zeilensuche wird `&7`
genommen. Die Zombie-Aufrufe (`0x2000`, `0x1500`, `0x2500`) ändern sich dadurch nicht. Danach ist im Fenster nachzumessen, ob
das Platzen und jedes RE1.5-Spinnenblut grün sind.

### MANGEL 2: Die Offen-Liste deckt `S_video_gore.md` §4 nicht ab

Im Ergebnis und in der Offen-Liste des Bauers stehen das Ganz-Zerplatzen durch Granaten (Z08/Z11/Z24), die Waffe von S01 und
die Krähenfedern (K01). **Nicht benannt** sind diese Merkmale, die das eigene Video-Dossier belegt:

- Spinne: die grünen Treffer-Spritzer S02/S05 und die grünen Flecken, die nach S01 gut 1 s auf Boden und Wand liegen bleiben.
  Das ist Mangel 1.
- Zombie Z28: Wandspritzer, die an der Wand bleiben.
- Zombie Z05/Z06: Kopf weg und kopflos weiter mit einer Nicht-Magnum-Waffe (wahrscheinlich Schrot). Im Port gibt es das nur in
  92C4 (Magnum).
- Zombie Z23: eine große Lache unter dem lebenden, liegenden Zombie.
- Zombie Z19: Die brennende Leiche brennt weiter. Am Port ist das nicht geprüft.

Ob diese Merkmale gebaut werden, ist PORT-WAHL. Nach dem Auftrag („Maximum je Typ, auch aus den Videos“) müssen sie aber
mindestens als offen benannt sein. Sonst entsteht der falsche Eindruck, die Inventur sei vollständig.

Was ich nicht als Mangel werte:

- Dass der Rumpf nach dem Platzen verschwindet, während im Video der Kadaver liegen bleibt. Das ist die RE1.5-Mechanik (Teil
  0/19 `|= 0x4a`), also das stärkere Gore.
- Die vom Bauer benannten offenen Punkte 1, 2, 3, 4, 6, 7, 9 und 10.

## 6. Nebenwirkungen

Gezielte Unit-Auswahl, 41 Tests, **41/41 grün**:

- Spinne und Babys: `unit_r41_spinnengore`, `unit_r38_spinnen`, `unit_r41_spinnenanim`, `unit_spinne_klettern` (Wandspinnen,
  Runde 42), `unit_re2_spider_ai`, `unit_adult_spider_ai`, `unit_spider_ai`, `unit_re2_baby_spider_dmg`,
  `unit_re2_hit_repeat_crow_spider` (+0x93-Latch), `unit_re2_spider_water_2090`, `unit_r41_gatorszene_spinnen`,
  `probe_spinne_elevation`, `unit_r38_spiderman`.
- Zombie-Gore und Trefferfilter: `unit_zombie_gore`, `unit_re2_gore80`, `unit_re2_gore`, `_render`, `_fly`, `_decoder`,
  `unit_re15_re2z_gore_se`, `unit_re2_weapon_rows`, `unit_r34_reaktion`, `unit_r35_redhawk`, `unit_r36_granate`,
  `unit_re2_zombie_hitfilter`, `unit_writher_kill_flag`.
- Hund und Krähe: `unit_re2_dog_ai`, `unit_r30_hund_tod`, `_n2`, `_elza`, `unit_dog_hit`, `unit_re2_dog_hitwindow`,
  `unit_r39_gegner_hund_riegel`, `_re15`, `unit_r36_kleidung_hund`, `_kraehe`, `unit_re2_crow_ai`, `unit_r36_schuss_kraehe`,
  `unit_crow_death_corpse`.
- Teile: `unit_r35_affen_teile`, `unit_r41_masken36_teile`.

Integration einzeln nacheinander, **4/4 grün**: `integration_r35_redhawk` 21 s, `integration_r38_spinnen_echtlauf` 154 s,
`integration_r41_spinnenanim` 152 s (Schachtspinne, Figur-Matrix), `integration_r41_spinnengore` 121 s.

Aus dem Code:

- Die Babys 0x26 laufen über `re15_re2spider_baby_tick` und nicht über die Hebung.
- Die Hebung hängt nur an `re15_re2spider_tick`, und dort nur in case 2 und 3.
- `re15_actor_alloc` leert die Teilschicht nur für die Plätze 1 bis MAX. Der Spieler auf Platz 0 und seine Birkin-Wurf-Teile
  bleiben unberührt.
- Die Tod-Bits (`em_flag_id`, `hit_react |= 2` im RE1.5-Gehirn) sind unverändert.
- Die Änderung in `main.c` lässt nur noch nicht-neutrale RE1.5-Teilfarben über die RE2-Tinte schreiben. Für RE1.5-Typen ist
  `gore_tint` vorbelegt neutral, das Bild bleibt also gleich.

## 7. Der Schalter `RE15_KEIN_80PROZENT_GORE`

Er wirkt. Das zeigen Lauf c (Log ta=0 und Bild ohne Scherben), L7AUS in der Integration und Unit C. Am Schalter hängen nur die
Hebung ins RE2-Gehirn und die MC51-Zelle. Das RE1.5-Gehirn (Tabelle @0x80118f38) und die Hund-Brücke hängen bewusst nicht
daran, beide sind Original-Mechanik.

## Mängel (für die Nachbesserung)

1. **Spinnenblut rot statt grün.** `re15_enemy_fx19700` (`re15_damage.c:3382`) maskiert `(a0>>16)&7` und verliert so die
   CLUT-Zeile (FUN_80019700 `srl v1,t7,3` @0x8001973c, `sll s0,v1,6` @0x80019754, `addu` @0x80019884).
   - Bei `0x82000` müsste TEX.TIM CLUT y 486 genommen werden, die grüne Zeile `185008…`. Der Port nimmt y 485, die rote Zeile
     `601010…`.
   - Das trifft alle sechs Blutsätze der Hebung und alle RE1.5-Spinnenblutsätze.
   - Am Bild ist es in Lauf a, Bilder 277–279, eine rote Wolke. Im Video (S01/S02/S05) ist es grün.
2. **Offen-Liste unvollständig gegenüber `S_video_gore.md` §4.** Es fehlen die grünen Spinnenspritzer und -flecken, Z28
   Wandspritzer, Z05/Z06 kopflos mit Nicht-Magnum, Z23 die Lache unter dem lebenden Zombie und Z19 die brennende Leiche.
3. (klein) Der Kommentar „24 Worte“ zur Tabelle @0x80118f38 ist falsch, es sind 22 Einträge. In die Release Notes gehört außerdem
   die Folge der MC51-Hebung für das Spiel: Der Zombie steht als Kriecher mit HP 10 wieder auf, statt zu sterben.
