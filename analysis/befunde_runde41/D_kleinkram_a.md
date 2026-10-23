# Runde 41 — Spur D "kleinkram_a" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kleinkram_a` (Zweig `r41/kleinkram_a`, Basis master 116e8b0f). Begonnen 2026-10-23.
Wortlaut (AUFTRAG.md): (1) Lautstaerke-Option "alles lauter oder leiser"; (2) Feuerloescher ROOM1030 leichter
Blink-Effekt "existiert bereits in RE2, uebernehme den"; (3) Spieler stirbt nicht bei erneutem Spinnengift auf Danger.

## 0. Stand / Protokoll (neueste zuerst)
* 2026-10-23 19:xx: Riegel-Fehler im Lautstaerke-Echtlauf korrigiert (gelesen-Zeile vor der stderr-Umleitung, Eckpixel),
  Shot-/TAB-4-Einstieg laedt den gespeicherten Sound-Modus (main.c, 2 Zeilen, @0x8002f244-4c), Elza-Lauf des Gift-
  Riegels ueber RE15_TITLE_SHOT_CHAR=1 (gemessen: RE15_PSELECT_AUTO liess beim Debug-Sprung Leon stehen). Neubau,
  Endlauf r41_kleinkram_a.
* 2026-10-23 18:xx: gezielte Laeufe (34 unit/probe gruen; 16 Echtlauf: 15 gruen, Lautstaerke-Riegel rot = Riegel-Fehler).
* 2026-10-23 17:4x: VOLUME-Zeile von y 89 (Shot: "Forwar" angeschnitten, DAT_80073d2c[2]/[8]) in den freien Streifen
  y 221 verlegt; Messungen nachher: RMS 0,5000, Puls-Hub 10,5, Danger-Treffer = Tod (Leon).
* 2026-10-23 17:3x: Code gebaut (df6141a5), Messungen nachher fuer alle drei Punkte.
* 2026-10-23: Baum sauber (116e8b0f), AUFTRAG/VERTRAG/UEBERGABE-40 gelesen, Einstiegspunkte lokalisiert:
  Options-FSM `platform/pc/main.c` pc_run_config (CFG_TOP/PICKER/SOUND/EDIT/AI, Zeilen ~2543-2905), Audio
  `platform/pc/src/audio_pc.c` (audio_callback :416, SS_BASE_MVOL :384, s_audio_mono :282), Feuerloescher
  `engine/src/feuerloescher_r37.c`, Spinnen `engine/src/spinnen_r38.c`, Zustand `engine/src/zustand_r39.c`,
  Schaden `engine/src/re15_damage.c`.

## 1. Punkt 1 — Lautstaerke-Option
### 1.1 Messung vorher (Stand 116e8b0f gebaut, Messkopie re15_pc_r41d_cfg.exe)
* Options-Schirm SOUND (RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=sound_vorher.bmp RE15_CONFIG_TAB=4): nur die zwei
  Kacheln STEREO (y 51) / MONO (y 70), keine Lautstaerke (Bild scratch r41d/cfg_vorher/sound_vorher.png).
* Code: kein Master-Pegel im Port — audio_pc.c kennt nur je-Pfad-Pegel (SS_BASE_MVOL = BGM-Layer, Se_on-Pegel,
  cd_vol der XA/FMV); der Sound-Modus s_config_sound (main.c:2495) ist ein static ohne Datei (grep fopen: kein
  Schreiber) -> ein per Kreis gesetztes MONO ist nach dem Neustart weg; Cross liess den umgeschalteten Wert stehen
  (Original laedt die Arbeitskopie bei jedem Eintritt neu, @0x8002f244-4c).
### 1.2 RE-Belege
* RE1.5 SOUND-Schirm = Sub-Treiber FUN_8002f0d4 (Handler-Tabelle @0x80073d9c[5], `XREF 80073db0`), vollstaendig
  gelesen (ghidra1_V2.txt:122560-122800). Aufbau (Phase 0) @0x8002f11c-30: zwei SPRTs w=0x58 (88) h=0x12 (18) x=0x74 (116)
  y=0x33 (51) u=0 v=0xe8 (232), zweiter y=0x46 (70) @0x8002f17c; Arbeitskopie S[7] := DAT_800b0fce @0x8002f244-4c
  (`lbu v0,DAT_800b0fce` / `sb v0,7(v1)`) bei JEDEM Eintritt. Eingabe (Phase 1) @0x8002f264-ec, einzige Pad-Abfrage
  `lhu v1,DAT_800ac762` @0x8002f268: `andi 0x40` (Cross = zurueck ohne Speichern @0x8002f270-8c), `andi 0xa0`
  (Bestaetigen @0x8002f278/90: S[7] -> DAT_800b0fce `sh v0,DAT_800b0fce` @0x8002f2c0, `jal FUN_80043c00` @0x8002f2c8 =
  Anwenden), `andi 0x5000` (UP|DOWN @0x8002f294: `xori v0,v0,0x1` @0x8002f2e8 = Umschalten). LEFT 0x8000 / RIGHT 0x2000
  werden im ganzen Treiber NICHT gelesen -> frei fuer die Port-Zeile. Es gibt in RE1.5 KEINE Lautstaerke (der Schirm
  kennt nur Stereo/Mono), RE2 PSX ebenso (kein "VOLUME"-Text in ghidra_re2_Leon.txt, Options-Schirm Stereo/Monaural)
  -> PORT-WAHL, NUTZER-VORGABE "alles lauter oder leiser".
* FORM (belegt): dritte Zeile im Raster der zwei Original-Kacheln (51/70 -> 89, derselbe 19-px-Schritt, den der Port
  fuer den AI-Schirm schon nutzt, main.c pc_config_draw_overlay), dieselbe CONFIG.TIM-Kachel uv(0,232) 88x18, Text in der
  Spielschrift (TEX.TIM, Vorschuebe BIN/DEBUG.BIN@0x4416): "VOLUME" = 52 px (Codes 32 2b 28 31 29 21), Ziffern 7-8 px,
  "100" = 23 px -> Beschriftung ab x=119 (wie STEREO), Zahl rechtsbuendig bis x=201 (3 px Rand wie links).
* WIRKUNG: eine Endstufe am Ende der Mischkette audio_callback (audio_pc.c), NACH der Mono-Faltung — dort laufen
  Samples (SE/Fussschritte/Waffen/CORE/RE2-Baenke), XA-Stimmen (s_xa), FMV-Ton (s_fmv_audio), BGM (re15_ss_render_bgm)
  und Rotor-Ambience (re15_amb_mix) zusammen; RE15_AUDIO_CAP_SYNC rendert durch DENSELBEN Rueckruf (re15_audio_tick
  @audio_pc.c:3683ff). Linear wie die SPU-Hauptlautstaerke (Q15; 100 % = 0x8000 = unveraendert).
* PERSISTENZ: der Port speichert den Sound-Modus bisher NICHT (s_config_sound = static in main.c, kein Schreiber einer
  Datei; gemessen per grep) — Original: DAT_800b0fce (RE1.5-Preview ohne Memory-Card). PORT-WAHL: Textdatei
  `re15_options.cfg` am selben Ort wie `re15_card.mcr` (Arbeitsverzeichnis, main.c RE15_CARD_PATH), Zeilen
  `volume=<0..100>` und `sound=stereo|mono`; Umgebungs-Override RE15_OPTIONS_FILE fuer Messungen/Tests.
### 1.3 Umsetzung
* NEU `engine/src/lautstaerke_r41.c` + `include/re15_lautstaerke_r41.h` (Zustand Arbeitskopie/gespeichert, Q15,
  Datei-Parser/Formatierer, Eintritt/Bestaetigen/Verwerfen), NEU `platform/pc/src/lautstaerke_r41_pc.c` (Kachel,
  Text, LEFT/RIGHT, Start). Haken: `audio_pc.c` audio_callback-Ende (4 Zeilen, Endstufe nach der Mono-Faltung),
  `main.c` (extern-Block bei s_config_sound; Start 1 Zeile; TOP->SOUND 1 Zeile = Eintritt; Overlay 1 Zeile;
  Eingabe/Confirm/Cancel 3 Zeilen).
* Konstanten: Kachel uv(0,232) 88x18 (`ori t4,0x58` @0x8002f0e0, `ori t3,0x12` @0x8002f0e4, `ori t1,0xe8`
  @0x8002f0ec, `sb zero,0xc(s1)` @0x8002f12c), x 116 (`ori t2,0x74` @0x8002f124); Text ab x 119 (STEREO-Spalte,
  Prim-Scan der Options-Spur), Zahl rechtsbuendig bis 201 (Kachelende 204 - 3); Schritt 10, Klemme 0..100; Q15
  prozent*0x8000/100 (100 % = unveraendert, 50 % = 0x4000).
* LAGE: zuerst y 89 (Raster 51/70/89, wie die AI-Zeile des Ports) — am Shot GEMESSEN: die Kachel 116..204 schneidet
  die Original-Beschriftungskaesten DAT_80073d2c[2] = (46,91) ("Forwar") und [8] = (203,91) an -> verworfen. Jetzt
  im freien Streifen unter dem byte-true Kasten (Kastenende y 220 -> Kachel y 221..238, Text y 223), Vorbild
  CFG_AI_Y 225 (main.c): kein Original-Pixel verdeckt.
* Datei `re15_options.cfg` am Ort von `re15_card.mcr` (CWD-relativ wie RE15_CARD_PATH), Zeilen `volume=<0..100>` /
  `sound=stereo|mono`; Override RE15_OPTIONS_FILE. Der Sound-Modus wird mitgespeichert und beim Start angewendet
  (re15_lautstaerke_r41_pc_start -> re15_audio_set_mono); Cross stellt die Arbeitskopie zurueck, Eintritt laedt sie
  (@0x8002f244-4c / @0x8002f2c0 / @0x8002f27c).
### 1.4 Messung nachher (Stand df6141a5 gebaut, Messkopien re15_pc_r41d_cap.exe / re15_pc_r41d_cfg2.exe)
* Zwei deterministische Mitschnitte (RE15_AUDIO_CAP_SYNC=cap.raw, RE15_AUDIO_CAP_FRAMES=330, Leon ROOM2030,
  Waffe 8 Schuss per Eingabeskript M1,MA0.2 ab Bild 20, Giftkugel-Treffer F19), RE15_OPTIONS_FILE mit volume=100
  bzw. 50 (scratch r41d/cap100, cap50):
  | Datei | RMS (int16, beide Kanaele) | Samples | Spitze |
  |---|---|---|---|
  | cap100 | 1701,145 | 970200 | 16681 |
  | cap50  |  850,572 | 970200 |  8340 |
  Verhaeltnis 50/100 = **0,5000** (Endstufe Q15 0x4000). debug.log: `[lautstaerke] .../opt.cfg gelesen (2
  Schluessel): 50 %, Stereo`.
* Options-Schirm mit volume=50 / 100 (RE15_CONFIG_SHOT, Tab 4): Kachel + "VOLUME 50" / "VOLUME 100" sichtbar
  (scratch r41d/cfg_nachher50/sound.png, cfg_nachher100/sound.png — noch in der Raster-Lage y 89, die wegen des
  angeschnittenen "Forward"-Kastens verworfen wurde; der Riegel prueft die Endlage y 221).

## 2. Punkt 2 — Feuerloescher 1030 Blink-Effekt
### 2.1 Messung vorher (Stand 116e8b0f, Messkopie re15_pc_r41d_cfg.exe)
* Leon ROOM1030 Cut 0 (RE15_DEBUG_JUMP=1030@250, RE15_PLAYER_POS=-11500,-19500,2048 = Stand des Nutzerbilds
  fire_extinguisher.bmp, Runde 38 Spur K), Bildserie RE15_FRAMEDUMP 400..520/20 (7 Bilder), Loescher-Huelle
  x 38..48 / y 111..122 (K_itemorte 1.1): Mittel RGB (51,3 / 59,1 / 69,5), hellstes Pixel 115 — in ALLEN sieben
  Bildern identisch -> kein Puls (Bild scratch r41d/fl_vorher/crop400.png: Loescher am Pfeiler unter dem Monitor).
  debug.log: `[feuerloescher] ROOM1030: Loescher an der Wand obj 5 bei (-2813,-2122,-17904) rot_y 3072`,
  `[prop-render] pi=1 oid=0x05`.
### 2.2 RE-Belege (RE2)
* RE2 kennt KEIN Blinken aufnehmbarer Gegenstaende — ehrlich belegt am Code (ghidra_re2_Leon.txt):
  - `Item_aot_set` (0x4E) = FUN_80054cd4: Item-Modell = Raum-Objekt Om[md1] (`lbu s2,0x14(s0)` @0x80054cf8, Om-Feld
    0x800d0324 Schritt 0x1f8 via `sll 6 / subu / sll 3` @0x80054da0-a8). Genommen (Flag-Test FUN_80077360 @0x80054d78):
    AOT +0 := 0 (@0x80054d94), Om.word0 := 0x80000000 (`lui v1,0x8000` @0x80054db0 / `sw` @0x80054db4 = inaktiv),
    +0x145 := 0 (@0x80054dbc); nicht genommen: +0x145 := action (@0x80054de4), action & 2 -> word0 := 0x80000000
    (@0x80054ddc-ec). Sonst KEIN Feld (kein Zaehler, keine Farbe).
  - Objekt-Zeichenpass FUN_800363f0 (Hauptschleife `jal` @0x80026964), je Om: aktiv word0 & 1 (@0x8003644c),
    uebersprungen bei word0 & 0x80000 (@0x80036458-60); RotMatrix(+0x24) @0x80036470, Licht aus der LAGE
    FUN_80076f88(+0x68) @0x800364ac -> FUN_80077200 (schreibt die Matrizen DAT_8009db64/DAT_8009db84 @0x80077288/c8),
    SetColorMatrix(DAT_8009db84) @0x800364bc, SetLightMatrix @0x800364d8; Zeichner FUN_80027bec/FUN_80027dbc
    (@0x8003666c/@0x8003669c, a2 = +0x7c) bzw. FUN_8002d0e8/d244 (+0x10 & 0x10) bzw. FUN_8002d3c8/d718 (+0x14c & 8:
    Zaehler +0x1ec bis 0x1e, dann word0 := 0 @0x800365bc-dc = EINMALIGES Ausblenden, kein Puls). Weder der Pass noch
    FUN_80076f88/FUN_80077200 lesen einen Bildzaehler (DAT-Zensus der Funktionen, s. Protokoll).
  - RE1.5: Props = FUN_8002c18c (flach 0x808080 bei attr 0x10, sonst NCCT; im Port main.c prop-render) — ebenfalls ohne
    Blinken. Der Nutzer-Glaube "existiert in RE2" trifft nicht zu -> PORT-WAHL mit belegter Form.
* FORM (PORT-WAHL, Vorbild im eigenen Spiel): der Helligkeitspuls der aktiven Titelzeile FUN_801028ec (TITLE.BIN,
  include/re15_title_pulse.h): Wert 0x80, +2 je Takt solange Zaehler < 31 (`sltiu v0,v1,0x1f` @0x801028fc, `addiu
  a0,a0,2` @0x8010290c) bis 0xBE, danach -2 (`addiu a0,a0,-2` @0x80102910), Periode 0x3c (`ori v0,zero,0x3c`
  @0x80102918, Reset 0x80 @0x80102928). Im Titel ein Takt je Hauptschleifen-Durchgang (2 VBlanks); im Spiel ein Takt je
  Spielbild (30 Hz) = Periode 2 s. Anwendung auf das Prop: Lichtkontext (ambient + C) x Wert/0x80 -> Spitze 0xBE/0x80 =
  +48,4 %, dasselbe Farbbyte-Semantik (0x80 = neutral) wie im Titel-Rechteck (`sb` @0x80102850/54/58).
### 2.3 Umsetzung
* NEU `engine/src/blink_r41.c` + `include/re15_blink_r41.h`: Pulswert = FUN_801028ec Schritt fuer Schritt (Start
  0x80 @0x80102928, +2/-2 @0x8010290c/@0x80102910, Schwelle 0x1f @0x801028fc, Periode 0x3c @0x80102918), Tabelle
  (Raum, Obj_model_set-Id, an) mit 8 Eintraegen, Lichtkontext-Skalierung ambient (geklemmt 255) + C (linear) x
  Wert/0x80. Takt = g_engine.frame_count (ein Takt je Spielbild = 30 Hz -> Periode 2 s).
* Haken: `main.c` Prop-Render nach dem Aufbau des Lichtkontexts (1 Zeile, `re15_blink_r41_lichtctx(raum, oid,
  frame_count, &lctx_prop)`), `feuerloescher_r37.c` nach dem Anlegen des Props (1 Zeile): an = zone_item ->
  nur ROOM1030 (Leon, aufnehmbar). ROOM1031 (Elza) hat keinen Loescher (k_lagen ohne Eintrag), ROOM1090/1091
  (liegend, nur Untersuchen) -> aus.
* API fuer Spur F (Schluessel ROOM2090): `re15_blink_r41_setzen(raum, obj_id, 1)` nach dem Anlegen des Props; der
  Prop muss NCCT-beleuchtet sein (attr ohne Bit 0x10; flach 0x808080 und Tint-Pfad ohne Normalen bleiben
  unveraendert — RE2-MD1 tragen Normalen).
### 2.4 Messung nachher (Stand df6141a5, Messkopie re15_pc_r41d_blink.exe, Serie 400..464/4, scratch r41d/fl_nachher)
| Bild | Takt (Bild mod 60) | Pulswert | Mittel RGB Huelle | hellstes Pixel |
|---|---|---|---|---|
| 400 | 40 | 0xAC | 59,1 / 64,2 / 74,1 | 149 |
| 420 |  0 | 0x80 | 51,3 / 59,1 / 69,5 | 115 (= vorher) |
| 436 | 16 | 0xA0 | 57,1 / 62,8 / 72,8 | 139 |
| 452 | 32 | 0xBC | 61,8 / 66,0 / 75,8 | 164 |
| 460 | 40 | 0xAC | 59,1 / 64,2 / 74,1 | 149 (= Bild 400: Periode 60) |
Hub des Mittels 10,5 (51,3 -> 61,8), hellstes Pixel +42,6 % (115 -> 164; Modell-Spitze 0xBE/0x80 = +48,4 %, der
Mittelwert enthaelt Pfeilerpixel). Bild crop452.png (Spitze) vs crop420.png (Grund). Pfeilerflaeche daneben
(x 24..34) konstant (Riegel).

## 3. Punkt 3 — Spinnengift bei Danger toedlich
### 3.1 Messung vorher (Stand 116e8b0f, Messkopie re15_pc_r41d_mess.exe, scratch r41d/gift1)
Leon ROOM2030 (RE15_DEBUG_JUMP=2030@250, RE15_PLAYER_POS=6550,2000,3072,3 wie integration_r38_spinnen_echtlauf),
RE2-Gehirn (Voreinstellung), 2400 Bilder ohne Eingabe, RE15_R39_LOG + RE15_SPINNEN_LOG + RE15_STATE_LOG:
* Giftklumpen-Treffer F19 (100 -> 90), F580 (72 -> 62), F639 (61 -> 51), F1041 (39 -> 29), F1435 (17 -> 7),
  F2041 (1 -> -9): je **-10 HP** (RE1.5 Routine 25 -> FUN_80012d60 Art 0), Gift-Bit ab F19, Gift-Takt -1 HP alle
  31 Bilder bis HP 1 (F1621), danach kein Abzug mehr (RE2 `slti v0,v0,0x2` @0x8003c174).
* Bei DANGER (HP 17, < 20) ueberlebt der Treffer (17 -> 7); erst der Treffer bei HP 1 (F2041) geht unter 0 ->
  st=3 (Tod), Todesfolge, Neuladen des Raums (hp=100). **Der Nutzerpunkt ist reproduziert**: ein Giftklumpen
  toetet nur bei HP <= 9, bei Danger-HP 10..19 nie.
### 3.2 RE-Belege
* RE2 (Ziel, Beta -> Retail) Giftkugel der Spinne = Op 32 @0x800200b0 (ghidra_re2_Leon.txt:100352ff): Treffertest
  `jal FUN_80047664` @0x800200e0 mit a1 = 0x320 (Radius 800, @0x800200d0) und a2 = 0x14 (Schaden 20, @0x800200dc).
* FUN_80047664 (Applier, @0x80047664-848): je Kandidat (Spieler s2 = 0, Partner s2 = 1; Zahl aus DAT_800cfbd8 & 0x1000000
  @0x800476ac-c0) Tore word0 & 1 (@0x800476e0), +0x1d3 != 0 (@0x800476ec-f4), HP < 0 (`lh v0,0x156(s0) / bltz`
  @0x800476fc-704), +0x10e & 0xc000 (@0x8004770c-18); Kasten y/xz (@0x8004772c-7a8); Treffer: Kommando +4 := 2
  (`sw s8,0x4(s3)` @0x800477b8); SPIELER: `jal FUN_800401d4` @0x800477fc mit a0 = Schaden, a1 = 0 (Modus 0), Rueckgabe
  == 2 -> Kommando +4 := 3 = TOD (@0x80047804-0c). Partner: HP -= Schaden direkt, HP < 0 -> 3 (@0x800477bc-f4).
* FUN_800401d4 (RE2_Quellcode_V2/FUN_800401d4.c, Modus 0): HP -= dmg; HP >= 0 -> 0 (ueberlebt); HP < 0: HP < -14
  (`iVar1 < -0xe`) ODER Latch DAT_800cfd4c & 0x1000 -> DAT_800cfb74 |= 0x4000000, DAT_800cfdcb |= 0x80, ret 2 = TOD;
  sonst HP := 0, Latch := 1, ret 1 (= "ein Mal gerettet"). Skalierungen davor: (DAT_800cfb74 & 0x40 || HP > 0x78) &&
  dmg < 0x29 -> dmg += dmg/2; Schwierigkeit DAT_800d482a == 3 -> x5 (dmg < 0x1e) / x2 (dmg < 0x3c). Port-Gegenstueck:
  enemy_ai_re2_zombie.c re2z_player_damage / re15_re2_player_damage_mode (Latch s_re2z_onesave).
* RE1.5 (Beta, bisheriger Port-Weg): Routine 25 @0x80017fa4 -> FUN_80012d60(500, &P, 0) @0x80018008: Art 0 = 10 HP
  (DAT_8006f418[0]), Tod nur bei HP < 0 (`bgez` @0x80012ee8), HP 0 = lebt. Gift-Takt RE2 FUN_8003bfac @0x8003c168-80
  zieht bis HP 1 (`slti v0,v0,0x2` @0x8003c174) und toetet NIE (zustand_r39.c).
### 3.3 Umsetzung
* NEU `engine/src/giftkugel_r41.c` + `include/re15_giftkugel_r41.h`: unter dem RE2-Gehirn der Spinne
  (re15_ai_re2_for_type(0x25)) trifft die Giftkugel wie RE2 Op 32: Schaden 20 (`li a2,0x14` @0x800200dc), Tore
  wie FUN_80047664 (+0x93 hit-once = RE2 +0x1d3 @0x800476ec-f4, Griff, HP < 0 @0x800476fc-704), Kommando 2
  (@0x800477b8, Port-Form Zustand 2 + Richtung + +0x93) bzw. Kommando 3 (@0x8004780c) mit dem Todeszweig von
  FUN_800401d4 (DAT_800cfdcb |= 0x80 @0x800402c0-d0 -> re2z_self1d3, Zustand 3); Treffer-SE 10 wie @0x80012e70.
  Der Treffer-Kasten bleibt der RE1.5-Effektkasten (Radius 500 @0x80017ffc; Hybrid wie das RE1.5-ESP-Geschoss).
* NUTZER-VORGABE "Danger + Treffer = Tod": der Rettungszweig von FUN_800401d4 (HP := 0 + Latch DAT_800cfd4c &
  0x1000, einmal je Raum im Port) wird fuer die Giftkugel NICHT uebernommen — er wuerde genau den gemeldeten Fall
  einmal je Raum erzeugen. Mit 20 Schaden geht jeder Treffer bei Danger (HP < 20, Schwelle `lb` @0x800112b5) unter
  0; HP 20 (Caution) -> 0 lebt.
* RE1.5-Gehirn: unveraendert (Beta-Wert 10, Tod bei HP < 0 @0x80012ee8).
* Haken: `spinnen_r38.c` Routine 25 (2 Zeilen VOR dem RE1.5-Resolver; der setzt +0x93 und zieht danach nicht
  noch einmal ab), `game_step_common.c` Messschiene RE15_R41_HP_AT=<bild>:<hp> (2 Zeilen, Baseline s_prev_hp
  nachgezogen = kein Pseudo-Treffer), `tests/unit/test_r38_spinnen.c` teil_b: Erwartung des RE2-Laufs 100 - 20.
### 3.4 Messung nachher (Stand df6141a5, Messkopie re15_pc_r41d_mess2.exe, scratch r41d/gift2)
Gleicher Aufbau, RE15_R41_HP_AT=5:15 (Danger):
* debug.log: `[giftkugel] F5 Messhaken: HP := 15`, `[giftkugel] F19 RE2-Treffer (Spinne Slot 1): HP 15 -> -5 =
  TOD (Kommando 3)`; gift.log F6 hp=15 -> F20 hp=-5 status 0x0002.
* state.log: pst=3 (Todeskommando) ab F19 durchgehend bis F132 (114 Bilder), Spieler-Motion 200 -> 7 (Todes-Clip)
  ab F20, danach Neuladen (gift.log `F0 aktiv=1 hp=100`) — Todesfolge 0x78 @0x8003694c + Game-Over wie beim
  RE1.5-Tod in 3.1.
* Gegenprobe HP 100 (cap100-Lauf): `[giftkugel] F19 RE2-Treffer (Spinne Slot 1): HP 100 -> 80 (Kommando 2)`,
  Spieler lebt (Zustand 2).

## 4. Tests
Alle neu, registriert nur aus `tests/unit/probes/r41_kleinkram_a.cmake` (GLOB); Werkzeug + Riegel in
`tests/unit/test_r41_kleinkram_a.c`, Echtlauf-Skripte `tests/integration/test_r41_kleinkram_a_{lautstaerke,blink,gift}.cmake`
(eigene Laufordner mit Zufalls-Id, exe-Kopie `re15_pc_r41d_<thema>_<id>`, cmake_minimum_required 3.21, kein Backslash,
libm fuer sqrt, `_DEFAULT_SOURCE` fuer setenv).
* `unit_r41_kleinkram_a` — A Lautstaerke (Parser/Formatierer, Q15 100 -> 0x8000 / 50 -> 0x4000 / 0, Klemme, Schritt,
  Arbeitskopie-Modell Eintritt/Bestaetigen/Cross mit echter Datei ueber RE15_OPTIONS_FILE, Rundreise), B Blink
  (`re15_blink_r41_wert(n)` == Titelpuls title_pulse.c nach n Schritten fuer n = 0..130, Spanne 0x80..0xBE, Spitze 31 /
  Reset 60, Tabelle, Lichtkontext x Wert/0x80 mit Klemme 255, aus = unveraendert), C Giftkugel (RE2-Gehirn + Spinne:
  HP 15 -> -5 Zustand 3 + is_dead, HP 19 -> -1 Tod, HP 30 -> 10 Zustand 2 + hit-once, Schutz vorbei + Danger -> Tod,
  HP 20 -> 0 lebt, RE1.5-Gehirn / fremder Absender / Kasten daneben / Slot 0 -> nicht zustaendig).
  Ergebnis: ALLE PRUEFUNGEN GRUEN (34 Pruefungen).
* `integration_r41_kleinkram_a_lautstaerke` — echte exe: (1) zwei CAP_SYNC-Mitschnitte 100 % / 50 % (Leon ROOM2030,
  Schuss + Giftkugel) -> `rms_vergleich` 500 Promille (soll 500 +- 5), Mindestpegel; (2) Options-Shot Tab 4 mit
  volume=50: VOLUME-Kachel y 221 pixelgleich mit Kachel 1 (Randspalten, Zeilen 2..15), Ziffern "50" rechtsbuendig
  (helle Pixel 186..201 = 18, 172..184 = 0); (3) echte Tastenkette im Options-Schirm (RE15_CONFIG_TEST, Tab 4, Eingabe-
  skript LEFT LEFT Quadrat RIGHT Quadrat, Vorspann uebersprungen): `[lautstaerke] ... geschrieben: 80 %, Stereo`,
  Datei `volume=80`, exe exit 0. Erster Batch-Lauf rot durch zwei Riegel-Fehler (gelesen-Zeile faellt vor die stderr-
  Umleitung; die 8 durchsichtigen Eckpixel der Kachel zeigen verschiedenen Hintergrund) — Riegel korrigiert, Messwerte
  selbst waren gruen (RMS 500 Promille, 18/0 helle Pixel, volume=80).
* `integration_r41_kleinkram_a_blink` — echte exe: Leon ROOM1030 Cut 0, Serie 400..460/4 -> `ppm_puls` Huelle x 38..48
  y 111..122 Hub 10,5 (soll 6..40), Pfeiler x 24..34 Hub 0 (soll <= 1).
* `integration_r41_kleinkram_a_gift` — echte exe: Leon ROOM2030 HP 15 -> `[giftkugel] F19 RE2-Treffer ... HP 15 -> -5 =
  TOD (Kommando 3)`, pst=3 mit hp=-5 in 100/100 Bildern ab F19, Neuladen (gift.log F0 hp=100); Gegenprobe HP 100 ->
  80 (Kommando 2), kein Tod; Elza ROOM2031 (RE15_TITLE_SHOT_CHAR=1 -> PL04; RE15_PSELECT_AUTO greift nur im echten
  Titelweg, gemessen character=0) dieselbe Kette (F19 Tod).
* Angepasst: `unit_r38_spinnen` teil_b RE2-Lauf erwartet 100 - 20 (RE2 Op 32 @0x800200dc), RE1.5-Lauf weiter 100 - 10.
* Gezielte Laeufe (KEINE volle Suite), Stand 4c713ad5 / exe df6141a5-Stand + Shot-Pfad-Patch:
  - `ctest -R "unit_r41_kleinkram_a|unit_r38_spinnen|unit_r39_zustand|unit_r37_feuerloescher|unit_r38_itemorte|unit_damage|unit_bleed_rng|r31_boxen|r38_cursor|r39_elza_klettern|r39_inventar|r38_rolltor20a0_windung"`:
    **100% tests passed, 0 tests failed out of 34**
  - `ctest -R "integration_r41_kleinkram_a|integration_r38_spinnen_echtlauf|integration_r37_feuerloescher|integration_r38_itemorte|integration_r39_zustand|integration_r39_johnruf1041"` (vor der Riegel-Korrektur):
    **94% tests passed, 1 tests failed out of 16** — rot nur integration_r41_kleinkram_a_lautstaerke (Riegel-Fehler s. o.;
    r37_feuerloescher 390 s, r38_spinnen_echtlauf 114 s, r38_itemorte, r39_zustand_kleidung/gift, r39_johnruf1041 a-t11b1d,
    r41 blink 43 s + gift 856 s gruen).
  - Endlauf `ctest -R "r41_kleinkram_a"` nach dem Neubau: (Summenzeile folgt unten im Protokoll)

## 5. OFFEN
* Giftkugel-Kasten: Radius 500 (RE1.5 Routine 25 @0x80017ffc) statt RE2 800 (`li a1,0x320` @0x800200d0) — Hybrid
  wie das RE1.5-ESP-Geschoss (Runde 38 Spur R), Trefferweite unveraendert gelassen.
* RE2-Skalierungen in FUN_800401d4 (DAT_800cfb74 & 0x40 bzw. HP > 0x78 -> x1,5 bei dmg < 0x29; Schwierigkeit
  DAT_800d482a == 3 -> x5/x2) haben kein Port-Gegenstueck (wie bei Zombie/Hund/Kraehe, enemy_ai_re2_zombie.c).
* Vorbestand (nicht Spur D): der RE2-Spinnen-BISS (enemy_ai_re2_spider.c re2s_attack) zieht 20 HP ueber die
  One-Save-Latch-Funktion, setzt aber kein Kommando 2/3 am Spieler ("OPEN, wie beim Hunde-Biss").
* Lautstaerke-Datei speichert nur Lautstaerke + Sound-Modus; AI-Modus und TYPE A/B/C/EDIT bleiben wie bisher
  unpersistiert (nicht zugeteilt).
* Puls nur fuer NCCT-beleuchtete Props; flach gezeichnete (attr 0x10) oder normalenlose Props bekaemen keinen
  Puls (fuer 1030 und den RE2-Schluessel 2090 nicht noetig).

## 6. Fuer den Nutzer
* BEDIENUNG: OPTIONS -> SOUND: LEFT/RIGHT = Lautstaerke 0..100 in Zehnerschritten (sofort hoerbar, auch der
  Menueton), Kreis/Quadrat speichert (zusammen mit Stereo/Mono), Cross verwirft. Die Zeile "VOLUME <Zahl>" steht
  unter dem Options-Kasten (freier Streifen, damit keine Original-Beschriftung verdeckt wird). Gespeichert in
  `re15_options.cfg` neben `re15_card.mcr` (Spielordner); Loeschen der Datei = 100 %, Stereo.
* FEUERLOESCHER 1030: pulsiert in der Helligkeit (2-s-Periode, bis +48 %, Form = Puls der Titelzeile). RE2 hat
  KEIN Blinken aufnehmbarer Gegenstaende (Item_aot_set FUN_80054cd4 + Objektpass FUN_800363f0 gelesen) — der
  Puls ist eine Port-Form. Spur F nutzt dieselbe API fuer den Schluessel in 2090.
* GIFTKUGEL: unter der RE2-KI (Voreinstellung) nimmt der Giftklumpen wie in RE2 20 HP; bei Danger (HP < 20) ist
  jeder Treffer toedlich (Todesanimation, Neuladen). Unter "AI RE1.5" bleibt der Beta-Wert 10.
* Keine neuen Assets (shared_assets/RE2 unveraendert), keine Sprachdateien, keine Bank-9-Bits, keine Nachrichten-Ids.
* ENTSCHEIDUNGSFRAGEN: (1) Sollen auch die ORIGINAL-Item-Props der RDTs (z. B. Munition am Boden) pulsieren?
  (2) RE2 gewaehrt bei einem toedlichen Treffer einmal je Raum die Rettung auf HP 0 (FUN_800401d4) — fuer die
  Giftkugel bewusst nicht uebernommen; soll sie es?
