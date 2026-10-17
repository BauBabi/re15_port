# Runde 38 — Spur R "spinnen": Abnahme 0 (unabhaengig)

Datum 2026-10-17. Pruefer: unabhaengige Abnahme (Workflow-Unteragent). Baum `.claude/worktrees/r38_spinnen`, Zweig
`r38/spinnen`, gepruefter Code-Stand `e7131aca` (danach nur `54f4aeaa`, Orchestrator-Nachtrag in AUFTRAG.md),
Basis (merge-base) `e2cc7673`.

**Ergebnis: NICHT BESTANDEN.** Punkt 1 erfuellt, Punkt 3 erfuellt, Punkt 2 teilweise: Das Spucken vergiftet und das
Gegengift heilt. Aber jeder Gift-Takt loest im Port eine Treffer-Reaktion aus (Taumeln, Rueckstoss, Zielabbruch). Das
Original tut das nicht. Daraus folgt auch die Regression `unit_r34_wurf`. Dazu kommt der Integrationspunkt aus dem
Nachtrag von 07:36 (Giftklumpen ausserhalb des Szenenschutzes).

Messordner (ausserhalb des Baums, eigener Unterordner):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\r38R_abn0\`
(unten `R0\`). Laufskript `R0\lauf_r38R.sh <neu|alt> <name> <exit_at> ENV=...`: "neu" startet die Messkopie
`re15_pc_r38R_abn0.exe` im Bauordner des Spurbaums (Kopie von `re15_pc.exe`). "alt" startet die v0.8.25-exe aus
`release/pkg-win/re15_port_v0.8.25` als Kopie `R0\v0825bin\re15_pc_v0825_abn0.exe` mit `RE15_CD_ROOT` auf deren
`shared_assets`. Beide laufen mit `RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`;
"neu" schreibt zusaetzlich `RE15_SPINNEN_LOG=spinnen.log`. Eigene Auswerter: `R0\adult_ana.py`, `R0\baby_ana.py`,
`R0\hp_ana.py`, `R0\sheet.py`. Bilder liegen unter `R0\belege\`.
(Hinweis: Der Ordner `scratchpad\abn0` wird von anderen Abnahme-Agenten mitbenutzt; deren `lauf.sh` hat meines dort
ueberschrieben. Deshalb liegt alles unter `r38R_abn0`.)

## 0. Grundlage
- Gelesen: AUFTRAG.md (Wortlaut Z. 68-70, dazu der neue "Nachtrag Integration" von 07:36), VERTRAG.md (Spur R und 1.4),
  das Dossier R_spinnen.md (541 Zeilen), `git log --oneline master..HEAD` (18 Commits + 54f4aeaa) und
  `git diff master...HEAD` (Drei-Punkt-Diff). Der Zwei-Punkt-Diff zeigt zusaetzlich Loeschungen unter `orchestrator/`;
  die kommen nur daher, dass master seit der Basis weiterlief. Den Code-Diff habe ich vollstaendig gelesen:
  spinnen_r38.c (776 Zeilen), re15_spinnen_r38.h und die Haken in 8 gemeinsamen Dateien.
- Bau: `bash re15_port/tools/local_build.sh configure` und danach `build` meldeten `LOCAL-BUILD-OK` und "ninja: no work
  to do". Die exe (06:34:46) ist juenger als alle Quellen, `git status` ist sauber. Der HEAD-Commit e7131aca aendert nur
  das Dossier.

## 1. Gezielte Tests (keine volle Suite)
Befehl: `ctest --test-dir re15_port/build -R <86 Tests> --timeout 900 --output-on-failure`. Die Auswahl umfasst die 40
Tests des Bau-Agenten und dazu die Testgruppen aller Module, die der Diff beruehrt:
- Spieler-Status und Gift: unit_member, unit_inv_fsm, unit_savedata, unit_dog_grab_anchor, unit_r34_wurf
- ESP: unit_r34_re2fx*, unit_r36_blut_pin_*, unit_r26_mg_blut, unit_fx_emitter_ai, integration_fx_region_cull,
  unit_weste_effekt, unit_montage_fx
- Gegner-KI: unit_dog_ai, unit_crow_ai, unit_birkin_ai, unit_re2_dog_ai, unit_re2_crow_ai, unit_re2_zombie_ai,
  unit_g5_tentakel, unit_r20_birkin_push_pin
- 1090 und SCD: unit_1090_*, probe_1090_*, unit_scd_*
- Stage 2: unit_r36_gegnerzahl_raum20b0, unit_r34_wasser

Ergebnis: **85 von 86 gruen, einer rot: `unit_r34_wurf`** mit FAIL 119/120/121 ("HOCH vergiftet: Abzug -1, Spawn
A+0").

Gegenprobe ohne Spur R: Ich habe dieselbe Test-exe aus `r38_cursor` und aus `r38_integration` gestartet (beide Baeume
ohne Spur R). Dort sind alle vier 6d-Faelle gruen ("HOCH vergiftet: Abzug 0, Spawn A+19"). Der rote Test ist also eine
**Regression durch Spur R**. Die Ursache steht in Abschnitt 3.2: Der neue Gift-Takt loest den Treffer-Detektor des Ports
aus.

`unit_r38_spinnen` ist gruen (Teile A, A2, B, B2, B3, C und D, "0 FAIL", die Ausgabe habe ich gelesen).
`integration_r38_spinnen_echtlauf` ist gruen (113 s).

## 2. RE-Gate: Stichproben selbst disassembliert (richtige Binaerdatei, Sprungziele mitgelesen)
Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` fuer RE1.5 (`info/Re1.5/PSX.EXE`, `STAGE2.BIN`) und
`re2_disasm.py` fuer RE2 (`info/re2leon/PSX.EXE`, `COMMON/BIN/EMS25.BIN`, `EMS26.BIN`). Alle Stichproben stimmen:

| Behauptung | selbst gelesen |
|---|---|
| EMS26 Wurzel-Schwanz @0x801000ac-c4 | `801000ac beq a0,zero,0x801000c8` / `801000b4 jal 0x80035530` (a0=s0) / `801000c0 jal 0x8003567c` / `801000c4 addiu a1,zero,1024`; davor +0x106 = (-Y)/1800 (`lui v1,0x91a2 / ori 0xb3c5 / mult / mfhi / sra 10`, `sb v1,262(s0)` @0x801000b0) |
| Sprungziel FUN_80035530 (RE2 PSX.EXE) | `80035544 jal 0x80035408`, `80035550 sb v0(255),13(s1)`, Liste ab 0x800cfe14 (Ende = *0x800ce334), `80035574 beq s1,a0`, `80035584 andi v0,v0,0x1`, `80035590 jal 0x80034d0c` / Delay `addu a1,s1,zero` |
| Sprungziel FUN_8003567c | `8003569c sw zero,272(s0)`, `80035694 andi v0,v1,0x8 / bne`, `800356a0 lw v0,484(s0) / bne`, `800356cc lhu a1,144(s0)`, `800356d0 jal 0x8004c1bc` / `sllv a2,v0,a2` |
| Kugeln Spieler/Baby | RE2 `8003bdc0 addiu v1,zero,450 / 8003bdc4 sh v1,154(s2)`, `8003bde0 addiu v0,zero,-1530 / sh 152`, `8003bde8 addiu v0,zero,1530 / sh 158`; EMS26 `80100168 addiu v0,zero,-10 / sh 152`, `80100170 addiu v0,zero,10 / sh 154,158,156,144(+0x90),146` |
| Babybiss-Reichweite | EMS26 `80100e2c lw s1,-488(s1)` (=0x800cfe18), `80100e34 lhu v0,154(s1)`, `80100e3c addiu v0,v0,10`, `80100e40 sltu v0,v1,v0`; Griff-Tor `80100e78 lbu v0,467(s1) / andi 0x80 / 80100e84 bne` |
| Adult +0x9A = 1000 | EMS25 @0x801063A0/C0/E0: s16-Index 11 je Tabelle = 1000 (Index 10 = +0x98: -1400/-100/0, Index 13 = +0x9E 1400) |
| Adult-Schub/-Wandpass, Zitat im Haken | EMS25 `8010016c jal 0x80035530`, `80100178 jal 0x8003567c` / `addiu a1,zero,1536`; STAGE2 `80110a6c jal 0x8002aec4` / Delay `addiu a0,s0,20` |
| RE2-Spucken | EMS25 `8010101c jal 0x801058a4`; FUN_801058A4 `801058f0 lui v0,0x1d00 / ori v0,v0,0x1000`, Versatz `80105910 addiu v0,zero,1000 / sh 16(sp)`, `sh zero,18(sp)`, `addiu v0,zero,-100 / sh 20(sp)`, `801059c4 jal 0x8001bf10` |
| RE1.5 "Gift-Wuerfel" | PSX.EXE `8001af28 lhu t1,0(v0)` (geladen, nie benutzt), `8001af30 srl v1,a0,7 / andi 0xff / addu a0,a0,v1 / andi a0,a0,0xff`; Aufrufer `80012e70 jal 0x800453d0` / Delay `ori a0,zero,0xa`, `80012e78`/`80012e80 jal 0x8001af20`, `80012e90 mult s0,v0`, `80012eac ori v0,v0,0x2`. Das Ergebnis haengt nur am Rest-a0. Die GDB-Messung habe ich nicht wiederholt. Der Beleg `R_belege/orig_2060_rng_a0.txt` zeigt 13 von 13 Zeilen `pc=80012e78 a0=0000000a r1=10 r2=10 gift=0`. |
| ESP-Routinen 24/25 | Tabelle `0x80071da0 -> 0x80017f50`, `0x80071da4 -> 0x80017fa4` (Dispatcher FUN_80019e20 `lw v0,0(at)=>PTR_LAB_80071d40` @0x80019e94); Routine 25: `80017fc4 addiu v0,v0,30` (x2), `80017ff4 jal 0x8001c6e8` (a2=8, a3=0x100), `80017ffc ori a0,zero,0x1f4` / `80018004 addu a2,zero,zero` / `80018008 jal 0x80012d60`, `8001804c lui v0,0x30c / addiu a0,a0,500 / jal 0x800199d4`, `8001805c lui a0,0x303 / ori a0,a0,0x1 / jal 0x80045024`, `80018098 sb v1(0xd),110(v0)`. Zensus (Skript des Agenten `esp_a2425.py` nachgefahren): nur Effekt 0x0d nutzt A 24/25 (41 Dateien); kein SCD-`Sce_espr_on` mit Effekt 0x0d in irgendeinem RDT (eigene Zaehlung ueber alle ROOM*.RDT) -> Routine 25 laeuft im Port nur fuer Spinnen-Klumpen. |
| Gift-Wirkung | PSX.EXE `80031e50 andi v0,a0,0x7 / beq`, `80031e68 addiu v0,v1,255`, `80031e78 ori v0,zero,0x78`, `80031e8c ori v0,zero,0x8`, `80031ea4 addiu v0,v0,-2`, `80031eb4 bgez`, `80031ebc bne v1,zero` / `ori v0,zero,0x1` |
| RE2 Op 32 | `800200e0 jal 0x80047664` (a2=20), `800200e8 beq v0,zero,0x800201e4`, `800200f8 sltiu v0,v0,0x10`, `80020118 andi v0,v0,0x8`, `80020120 addiu v0,zero,5400`, `80020130 sh v0,-496(at)` (= 0x800cfe10) |
| Schacht STAGE2.BIN | Tabelle @0x8010027c = {80114f08, 80114f8c, 80114ff8, 80115068, 801150a8, 80115118, 80115158, 801151e4, 80115338}; `80114ee0 sltiu v0,v1,0x9`; `80114f18 ori v0,zero,0x41 / beq`; Phase 2: `80115014 lw v0,56(v1)` (+0x38 = y) `/ 8011501c addiu v0,v0,1000`, `8011502c ori v0,zero,0x12c`, `80115030 jal 0x800245d8` (a0 = 0x800), `80115044 sh zero,442(v0)`, `8011505c addiu v0,v0,1024` (+0x6c) |

Fuer Mangel 1 habe ich zusaetzlich selbst gelesen:
- RE2-Gift-Takt: `8003c12c lhu 0x800cfe10 / beq`, `8003c14c addiu v0,v1,255`, `8003c15c addiu v0,zero,30`,
  `8003c174 slti v0,v0,2 / addiu v0,v1,-1 / sh v0,342(s0)`, danach direkt `jalr` des Zustands (@0x8003c1a4). Dort gibt
  es **keinen Zustands- oder Kommando-Schreiber**.
- RE1.5 @0x80031e44-ecc: schreibt ebenfalls nur HP und Zaehler.

Weitere Pruefungen:
- Suchwoerter in den neuen Code-Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt`):
  keine Treffer. Einziges `getenv` ist die Messschiene `RE15_SPINNEN_LOG`; sie veraendert kein Spielverhalten.
- Konstanten ohne Adresse: keine gefunden.
- PORT-WAHL und NUTZER-VORGABE sind gekennzeichnet: Gift-Bit nur ueber den Klumpen einer Spinne; Gift-Wuerfel tot;
  RE1.5-Effekt 0x0d statt RE2-0x1D; Wandklemme Maske 4; Kugel-Zuordnung fremder Typen; RE1.5-Schacht-Choreografie auch
  unter der RE2-KI.
- Erklaert der Fix den gemessenen Befund? Ja fuer alle drei Vorher-Befunde:
  - RE2-INIT-Eintrag 1 fuer grid 0x41 ist leer, deshalb lief die Spinne im Bodenmodus auf Schachthoehe.
  - Babybank Paar 3 hat eine Wurzel bei -1797. Den Babys fehlten Wandpass und Schub (sie liefen vom Steg). +0x1C2 war 0.
  - Das Spucken war nicht verdrahtet (OPEN).

## 3. Nutzerpunkt 2 — Gift: TEILWEISE
### 3.1 Spucken, Vergiftung und Gegengift funktionieren
- Lauf `R0\p2d_gegengift`: `lauf_r38R.sh neu p2d_gegengift 700 RE15_DEBUG_JUMP=2030@250
  RE15_PLAYER_POS=6550,2000,3072,3 RE15_GIVE=35:1 RE15_INPUT_SCRIPT=S0.1,W2,A0.1,W1,D0.1,W0.6,A0.1,W1,A0.1,W2,S0.1,W3
  RE15_INPUT_SCRIPT_START=250 RE15_INPUT_SCRIPT_BASIS=spiel RE15_FRAMEDUMP=260-560/20:fd_`.
- Messschiene in ROOM2030:
  - `F19 hp 100->90 ps 0000->0002`: Spucktreffer, Gift gesetzt
  - `F42 90->88` und `F185 88->86`: Gift-Takt
  - `F439 86->111 ps 0002->0000`: Antidote Spray
  - `F689 111->101 ps 0000->0002`: neuer Treffer, wieder vergiftet
- Bild `R0\belege\gegengift_ekg_F280-460.png`: Das EKG zeigt "Poison" (violett) von F280 bis F420. Dann ITEM, Spray,
  USE. Bei F440/F460 zeigt es "Fine" (gruen), und das Spray ist aus der Liste verschwunden.
- Lauf `R0\p1_tuer_2060_re15ki` mit der RE1.5-KI (`RE15_AI_FLAVOR=re15`): Spucken (Unterzustand 7) trifft bei F402
  (HP 100 -> 90, ps 0002 = Gift); bei F425 folgt der erste Gift-Takt (-2).
### 3.2 MANGEL: jeder Gift-Takt loest im Port eine Treffer-Reaktion aus (Original: nein)
- Lauf `R0\p2b_gift_2030`: wie oben, aber ohne Menue, 1300 Bilder, mit `RE15_STATE_LOG=state.log`.
  - Nach JEDEM der 10 Gift-Takte (HP -2, Spielerzustand pst=1, kein Angreifer) spielt der Spieler 22 Bilder lang den
    Treffer-Clip 8 (bzw. 9). Die Gift-Takte liegen bei F42, F185, F328, F493, F636, F779, F922 (Clip 9), F1065 und
    F1252. Beispiel: `F42 HP 90->88 pst 1 | mo danach: 200,8,8,8,...(22x)`.
  - Das ist dieselbe Reaktion wie auf den echten Spucktreffer (F19, F462).
- Bild `R0\belege\gift_tick_zuckt_F182-198.png`: Bei F182/184 steht Leon. Ab F186 reisst er die Arme hoch und taumelt
  zurueck, obwohl in F185 nur der Gift-Takt lief (`gz=120` = Zaehler neu geladen).
- Ursache im Code:
  - Der neue Takt `re15_spinnen_r38_gift_tick` (game_step_common.c:1631) senkt die HP im Kommando-1-Prolog.
  - Im naechsten Bild sieht der generische Treffer-Detektor des Ports `pl->hp < s_prev_hp` (game_step_common.c:1293)
    den Abfall. Er ruft `re15_player_stagger_cmd2` auf: Clip 8/9/0xa, Rueckstoss 0xc8, laut Code Treffer-SE
    `re15_audio_core_se(1|2)` und Zielabbruch `re15_player_aim_interrupt`.
  - Das Original schreibt beim Gift-Takt NUR HP und Zaehler: RE1.5 @0x80031e44-ecc und RE2 @0x8003c12c-180 (siehe
    Abschnitt 2).
- Folgen:
  - (a) Die Gift-Periode in der exe betraegt 143 statt 121 Bilder (F42 -> F185 -> F328). Die 22 Reaktionsbilder laufen
    nicht durch den Kommando-1-Prolog. Die Dossier-Aussage 4.2 "alle 121 Bilder" stimmt in der exe nicht. Der
    Unit-Test "B Wirkung" ruft den Takt direkt auf und kann das nicht sehen.
  - (b) **Regression `unit_r34_wurf`** (Abschnitt 1): Der erste Gift-Takt beim Halten bricht das Zielen ab, also keine
    Granate.
  - Im Spiel: Ein vergifteter Spieler verliert etwa alle 4,8 s das Zielen bzw. den Wurf und wird zurueckgestossen.

## 4. Nutzerpunkt 3 — Minispinnen am Boden, sie greifen an: ERFUELLT (gemessene Lagen)
- Leon, Lauf `R0\p3_babys_leon`: Lauf wie integration_r38_spinnen_echtlauf, aber 2400 Bilder, dazu `RE15_STATE_LOG` und
  `RE15_FRAMEDUMP=200-2400/100`.
  - Dossier-Werkzeug ana_bo.py: Typ 0x26 hat 20025 Bilder, davon 0 schwebend, 450 im Sprung und 19575 am Boden.
  - Eigene Auswertung `R0\baby_ana.py` (unabhaengig von ana_bo): Das erste Baby erscheint in Bild 176, bis zu 9
    gleichzeitig. Baby-Y ist nur -5400 (Steg) oder ein Sprungbogen von -5700 bis -6680.
  - HP-Abzuege: 1 HP x22 (Bisse), 2 HP x15 (Gift), 10 HP x1.
  - Bild `R0\belege\babys_leon_2030_F300-1400.png`: kleine braune Spinnen auf dem Stegboden um Leons Fuesse.
- Elza, Lauf `R0\p3_babys_elza` (`RE15_TITLE_SHOT_CHAR=1`, Raum R2031, 1500 Bilder): 11925 Baby-Bilder, davon 0
  schwebend und 345 im Sprung; 16 Bisse. Bild `R0\belege\babys_elza_2031_F400-1200.png` (Elza in Rot, Babys am Boden).
- Bodenraum ROOM2070, Lauf `R0\p3_babys_2070` (Leon, Sprung-Spawn, Schrotflinte, beide Spinnen getoetet): 17 Babys,
  19317 Bilder, davon 0 schwebend und 642 im Sprung; 29 Bisse. Bild `R0\belege\babys_2070_F300-1400.png`.
- Grenze der Messung: Gemessen habe ich nur Lagen, an denen der Elternteil auf seiner Ebene stirbt. Eine Spinne, die an
  einer Ebenenkante stirbt, deckt der Port ueber den Boden des Elternteils ab (Dossier OFFEN 6); das habe ich nicht
  gesondert gemessen.

## 5. Nutzerpunkt 1 — Spinnen in der Luft, Schaechte: ERFUELLT
### 5.1 Vorher (v0.8.25) bestaetigt
- Lauf `R0\v0825_tuer_2060`: echter Weg ueber die Tuer ROOM2030 -> 2060.
  - Befehl: `alt ... RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=-7700,-7250,2048,9 RE15_PAD_AT=260:A
    RE15_INPUT_SCRIPT=U1.8,W40 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel`.
  - Bild `R0\belege\schacht_2060_tuerweg_v0825_F280-560.png`: Beide Schachtspinnen laufen in Deckenhoehe ueber Leon und
    treffen ihn aus der Luft. In einem laengeren Lauf (`R00825_tuer_2060_lang\state.log`) faellt HP in Schritten
    von 20 (F196, F402, F438, F471, F503); bei F531 ist HP -20 und Leon stirbt (pst 7).
- Lauf `R0\v0825_20a0_kante` (ROOM20A0): Die Spinnen haengen in der Luft ueber dem Gang bzw. ueber dem unteren Boden.
  Bild `R0\belege\schacht_20a0_v0825_F160-300.png`.
### 5.2 Nachher: Schachtspinnen
- Lauf `R0\p1_tuer_2060` (ECHTE Tuer 2030 -> 2060, `[aot] DOOR FIRE slot=3 ... spawn=(7600,0,3950)`, danach zu Fuss in
  Cut 1):
  - Spinne 1 wartet in Zustand 4/1 auf y -3600, bis Cut 1 kommt. Bei F308 wechselt grid 0x41 -> 0x42.
  - F308-338: Kriechbahn bis (5975,4325).
  - F339: y -2600, Neigung 1024.
  - F340-378: Abstieg.
  - F379: **Landung (5975,0,3137)**.
  - F380: RE2-INIT; danach Zustand 1 auf y 0, und sie spuckt (HP 100 -> 88 -> 78 ...).
  - Spinne 2 wartet ohne Ausloeser im Schacht weiter.
  - Bild `R0\belege\schacht_2060_tuerweg_neu_F280-440.png`: Die Spinne kriecht aus dem Deckenschacht, haengt an der
    Wand, steigt ab und steht neben Leon.
- Elza, Lauf `R0\p1_elza_tuer_2061`: echte Tuer 2031 -> 2061 als Elza, Bild fuer Bild derselbe Ablauf mit Landung
  (5975,0,3137).
- Lauf `R0\p1_tuer_2060_re15ki`: RE1.5-KI, derselbe Ablauf, Landung (5975,0,3137).
- Spinne 2 in ROOM2060, Lauf `R0\p1_2060_sub03`: Mechanikprobe mit `RE15_FORCE_EVENT=3@300`, also das Original-Skript
  sub03 nach dem INIT. Spinne 2 kriecht heraus (F301-331), steigt ab und **landet bei (-3337,0,475)** (F372).
  - Den Weg in Cut 5 habe ich zu Fuss nicht erreicht. Der Autopilot und Tueren aus 2040 blieben stecken.
  - Mit `RE15_FORCE_CUT=5` ab Bild 1 feuert der Ausloeser VOR dem INIT. Dann laeuft die Spinne im Deckenmodus
    (m222=1) — das ist der Kunstfall, den das Dossier als OFFEN 3 fuehrt. Ueber Tueren ist er nicht erreichbar.
- ROOM20A0, Lauf `R0\p1_20a0_kante`:
  - Die echte Tuer 2090 -> 20A0 funktioniert: `[aot] DOOR FIRE slot=1 ... spawn=(4950,-3600,4100)`. Danach warten beide
    Spinnen in Zustand 4/1 (Lauf `R0\p1_tuer_20a0_probe`).
  - Den Raumweg bis zur Kante habe ich ab dem Sprung-Spawn des Gangs gefahren (`UX4.15,A0.2,W3,U1.5`). Den Weg ab der
    Tuer hat der Autopilot nicht geschafft.
  - Ablauf: Kante bei F161, Cut 4; Spinne 1 kriecht aus dem Wandschacht (F161-191), steigt ab und **landet bei
    (22732,0,-25194)** (F249). Danach Angriff (HP 100 -> 80).
  - Bild `R0\belege\schacht_20a0_neu_F160-300.png`.
### 5.3 Nachher: alle Adultspinnen am Boden (Messgroesse bu der Schiene; am Spieler gegengeprueft, |y-bu| <= 100)
- Typ 0x25 ausserhalb der Schacht-Choreografie, 0 schwebende Bilder in jedem Lauf:
  - ROOM2030 (Steg): 3900 Bilder (`p2b`)
  - ROOM2030 oberer Steg ueber die ECHTE Tuer 2060 -> 2030: Bild `R0\belege\oberer_steg_2030_tuerweg_F60-570.png`
  - ROOM2050: 1800 Bilder
  - ROOM2070: 3000 Bilder
  - ROOM2090: 520 Bilder
  - ROOM2060: 1611 Bilder nach der Landung
  - ROOM20A0: 356 Bilder
- Ausnahme ROOM20A0: 5 Bilder, in denen die Schachtspinne VOR ihrem ersten Takt auf ihrer Spawnlage im Schacht steht.
  Das Dossier fuehrt das ebenfalls; es ist nicht sichtbar.
- Modus +0x222 aller Adulten in allen Laeufen: 0 (Boden).

## 6. Vertrag und Nachtrag
- Pfade: Nur `analysis/befunde_runde38/R_*` sowie engine/include/platform/pc/tests. Keine Datei unter `release/`,
  `platform/android/` oder `shared_assets/PSX/`, keine Test-CMakeLists; `probes/r38_spinnen.cmake` wird per GLOB
  geladen.
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine neuen Assets.
- Gemeinsame Dateien: Die Haken sind klein und tragen "Runde 38 Spur R":
  - scd_vm.c: 2 Zeilen, game_step_common.c: 2, main.c: 2, re2_ems.c: 2, re15_esp.c: 5.
  - re15_damage.c: 1 Codezeile ersetzt, dazu Kommentar.
  - enemy_ai_common.c: 5 Hakenstellen. VERTRAG 1.4 weist diese Datei B und T zu, nicht R. Die RE1.5-Spinnen-KI liegt
    aber dort, die Haken sind einzeilig. Das ist kein Mangel, ist aber bei der Integration zu beachten (moegliche
    Konflikte mit B und T).
- Seiteneffekte gegengeprueft:
  - Die Lage-Spiegel +0x40/+0x42/+0x44, die Sce_em_set jetzt fuer alle Gegner setzt, liest ausserhalb von spinnen_r38.c
    niemand (nur Spieler-Spiegel; grep).
  - Die Babybank-Weiche wirkt nur auf echte RE2-Babys: Die 0x26-Feueranker in 1090 laden mit allow_re2=0
    (main.c:1196). Die 1090-Tests sind gruen.
  - Der Wegfall des 1/4-Wuerfels in re15_damage.c trifft nur Typ < 2, also im Port nur Routine 25 und die ruhende
    re15_enemy_attack. Der Alligator-Biss ist Typ 5.
  - unit_damage wurde auf 0/2000 umgestellt; die Begruendung (Beleg 13/13) steht im Dossier 2.7.
- **Nachtrag Integration (AUFTRAG 07:36, "Die Abnahme prueft diesen Punkt mit")**:
  - Der Babybiss liest das Original-Tor +0x1D3 & 0x80 (`80100e78-84`, selbst gelesen) ueber re15_player_is_grabbed()
    und laeuft im Gegner-Takt. Spur Bs Sicht erfasst ihn also.
  - Der Giftklumpen (Routine 25) liest die Original-Tore +0x93&1 (Resolver @0x80012e24-30), +0x1D3 (RE2
    @0x800476ec-f4) und HP >= 0. Er laeuft aber im ESP-Takt AUSSERHALB von re15_enemy_ai_run_all.
  - Spur B (im Integrationsbaum, `szenenschutz_r38.c`: "Die Sicht gilt NUR waehrend des Ticks dieses einen Gegners")
    deckt ihn nicht ab, und auch ihr HP-Sicherheitsnetz greift dort nicht.
  - Folge in der Integration: Trifft ein Klumpen, waehrend eine Szene den Spieler haelt (player_mode 2), kostet das
    10 HP UND setzt das Gift-Bit. Das ist genau der Fall des Nachtrags.
  - Das Dossier (Abschnitt 6, "Fuer die Integration") benennt das, aber es gibt weder Haken noch Riegel -> Mangel 3.

## 7. Urteile
| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1 "Spinnen fliegen fast alle in der Luft und kommen teilweise nicht aus den Schaechten raus" | erfuellt | 5.1-5.3: vorher in v0.8.25 schwebend in 2060/20A0 (Bilder). Nachher kriechen sie ueber den echten Tuerweg (2060 Leon/Elza, RE2- und RE1.5-KI) und in 20A0 heraus und landen auf y 0. Alle Adulten haben in allen Stage-2-Raeumen 0 schwebende Bilder. |
| 2 "Spinnen verschiessen kein Gift wie im Original ... einziger Grund fuer das Gegengift" | teilweise | 3.1: Spucken -> Gift -> EKG "Poison" -> Antidote -> "Fine" funktioniert. 3.2: Jeder Gift-Takt loest aber eine Treffer-Reaktion aus, die das Original nicht hat (10/10 Takte, Clip 8/9, Bild). Folge: Periode 143 statt 121 und die Regression unit_r34_wurf. |
| 3 "Minispinnen fliegen weitestgehend in der Luft und greifen einen nicht an" | erfuellt | 4: 0 schwebende Baby-Bilder (Leon 2030, Elza 2031, Bodenraum 2070), gezeichnet auf dem Boden (Bilder), 16-29 Bisse je Lauf. |

## 8. Maengel (nummeriert, nachpruefbar)
1. **Gift-Takt loest eine Treffer-Reaktion aus.**
   - Was passiert: `re15_spinnen_r38_gift_tick` (game_step_common.c:1631) senkt die HP nach der Baseline
     `s_prev_hp = pl->hp` (game_step_common.c:1326). Der Port-Detektor `pl->hp < s_prev_hp` (:1293) ruft im naechsten
     Bild `re15_player_stagger_cmd2`.
   - Gemessen: `R0\p2b_gift_2030\state.log`, 10 von 10 Takten mit `mo=8/9` fuer 22 Bilder; Bild
     `R0\belege\gift_tick_zuckt_F182-198.png`. Gift-Periode in der exe 143 statt 121 Bilder.
   - Original: kein Kommando-/Zustandsschreiber im Gift-Takt (RE1.5 @0x80031e44-ecc, RE2 @0x8003c12c-180).
   - Erwartet: Der Gift-Takt aendert nur HP und Zaehler. Kein Taumeln, kein Rueckstoss, kein Treffer-SE, kein
     Zielabbruch. Periode 121 Bilder in der exe.
   - Riegel: im echten Spielschritt (re15_game_step mit rdt_ok) oder im Echtlauf — Clip/Kommando des Spielers nach dem
     Takt unveraendert.
2. **Regression `unit_r34_wurf`** (FAIL 119/120/121, "HOCH vergiftet: Abzug -1, Spawn A+0").
   - Dieselbe Ursache wie 1: Der erste Gift-Takt im Halten bricht das Zielen ab.
   - Gruen ohne Spur R (Test-exe aus r38_cursor und r38_integration).
   - Der Test stand nicht in der Testauswahl des Bau-Agenten (40 Tests), obwohl er den Gift-Status des Spielers
     prueft (`pl->status_flags |= 0x0002`, probe_r34_wurf.c:916).
3. **Giftklumpen ausserhalb des Szenenschutzes (Nachtrag 07:36).**
   - Routine 25 (spinnen_r38.c `routine25`, ESP-Takt) liegt ausserhalb von re15_enemy_ai_run_all. Spur Bs Sicht und
     ihr HP-Netz erfassen sie nicht. Im Integrationsstand wuerde ein Klumpen waehrend einer haltenden Szene 10 HP kosten
     und das Gift-Bit setzen.
   - Erwartet: Auf dem Integrationsstand prueft routine25 vor dem Gift-Bit UND vor re15_resolve_attack
     `re15_szenenschutz_haelt()`, so wie es das Dossier in Abschnitt 6 selbst vorschlaegt.
   - Riegel: Klumpen trifft den Spieler bei player_mode 2 -> keine HP, kein Gift. Gegenprobe frei -> 10 HP und Gift.

## 9. Hinweise (keine Maengel)
- `re15_spinnen_r38_reset()` wird im Spiel nie gerufen (nur in Tests). Der Gift-Zaehler `s_gift_zaehler` ueberlebt
  dadurch Neues Spiel/Laden. Wirkung: Der erste Takt nach einer neuen Vergiftung kann bis zu 120 Bilder verschoben sein.
- Der 1/4-Wuerfel faellt auf Grundlage von 13/13 Treffern in einem Raum. Das Dossier fuehrt selbst den Pfad "SPU-Zeiger
  in a0" (2.7). Das ist sauber als PORT-WAHL/NUTZER-VORGABE gekennzeichnet und passt zum Wortlaut ("das Gift ist das
  einzige, das vergiften kann").
- Die Dossier-OFFEN-Punkte 1-7 sind ehrlich deklariert. OFFEN 3 (Kunstfall Ausloeser vor INIT) habe ich mit
  `RE15_FORCE_CUT=5` reproduziert; ueber Tueren ist er nicht erreichbar.
