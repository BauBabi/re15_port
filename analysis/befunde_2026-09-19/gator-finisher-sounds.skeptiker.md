# SKEPTIKER — gator-finisher-sounds.md (Runde 16, 2026-09-19)

Eigener Build: `re15_port/build_r16_sk_gator-finisher-sounds/` (Ninja, mingw64 GCC), Sonden des
Dossiers dort gebaut und ausgefuehrt; Gegen-Sonde
`re15_port/tests/unit/probe_r16_sk_gator_finisher_sounds.c` (registriert in
`tests/unit/probes/r16_sk_gator-finisher-sounds.cmake`). Disassembly selbst mit
`re2_disasm.py` / `re15_disasm.py`; Tabellen/RDT/VBS-Bytes selbst geparst (Python).
Keine Aenderung an Engine-/Plattform-Code oder Dossiers.

## Ergebnis-Tabelle

| # | Befund (Dossier) | haelt / widerlegt | Beleg (eigene Adressen / Messwerte) |
|---|---|---|---|
| 1 | §2.1 RE2 Anker-Paar FUN_80015B94/CB8: `anchor = pos − RotY(yaw_g)·off` auf Identitaetsmatrix, +0x164/166/168, Kopie in a0; Platzierung `pos = anchor + RotY·off` incl. **y** | **haelt** | Disasm 0x80015b94-cb4: `jal 0x80015db0` (kf+6/8/10 → sp+48, @0x80015e58-78 `lhu 6/lw 8/lhu 10`), Identitaet `0x8009db44` = [4096,0,0,0,4096,0,0,0,4096] (selbst gelesen), `lh a0,118(s0)` @0x80015c24, `sh v0,356/358/360(s0)` @0x80015c50/64/78, Kopie `sh …(s3)` @0x80015c7c/88/94; FUN_80015CB8 `sw v0,56/60/64(s1)` @0x80015d68/7c/90 |
| 2 | §2.2 RE1.5 Render-Scale FUN_8001e8c8: Flag 0x800, `lh +0x166` ×3, ScaleMatrix VOR Translation → jeder Rig-Vektor erscheint als `s·RotY·v` um G | **haelt** | Disasm: `andi v0,v0,0x800` @0x8001e904, `lh v0,358(v1)` @0x8001e91c/28/38, `jal 0x80065ff0` @0x8001e940 (ScaleMatrix = M·diag(s), Elementweise `multu`/`sra 12` @0x80065ff0-64 → Skalierung im LOKALEN Raum vor der Rotation), `jal 0x80053fc0` @0x8001e94c mit a0 = entity+52. Port: main.c:8258-8261 skaliert `nyaw`, :8447-8457 wendet es auf JEDE Bone-Translation incl. Wurzel an (+npc->x/y/z) — damit ist auch der §5-Punkt „Wurzel-POSE durch die skalierte Matrix" fuer den PORT belegt |
| 3 | §1.1 Messung: Leon-Wurzel gator-lokal heute (7892,−1804,295), 6=0/7=0; Original (5264,−2403,198) 6=1/7=1; S-Variante ±5; LIVE Teil B = Spielwerte (Anker (−8368,−21962), yaw 113, pl=(−65,0,−23215), scale 2731) | **haelt** | Eigener Build, bitgleich: `T heute … LOKAL=(7892,-1804,295) … 6=0/7=0`, `S SKALIERT … (5261,-2402,194) 6=1/7=1`, `LIVE P3 … (7892,-1804,295)`, `LIVE: anker=(-8368,-21962) yaw=113 pl=(-65,0,-23215) scale=2731`; gator_boss.log:347/366 (echtes Spiel) `FRESS-P2 Anker=(-8368,-21962) yaw=113` / `FSYNC2 sf=120 … pl=(-65,0,-23215)`. Gegen-Sonde ueber ALLE 121 Frames (sf 0..119 + P3): S vs Original max|dx,dy,dz| = 3/2/3, Kiefer-6/7-Treffer in 0 Frames abweichend; Original hat 6=1&&7=1 in 91/121 Frames |
| 4 | §1.1 Datenzensus: Opfer-Clip 1 POSE x/z = 0 in allen kf; SPEED y = 0 in allen Gator- und Opfer-Frames; Opfer-Clip 0 x/z-POSE 0 | **haelt** (die Dossier-Sonde druckt das NICHT; ihr Zensus zaehlt den ganzen 680-kf-Pool: 515/510/516 ≠ 0) | Gegen-Sonde je Frame ueber den 0x8000-Resolver: clip 0 (30 F) POSE x/z≠0: 0/0, SPEED y≠0: 0; clip 1 (120 F) POSE x/z≠0: 0/0, SPEED y≠0: 0 [0..0], SPEED z≠0: 117; Gator-Clips 4/5/11 SPEED y≠0: 0/0/0 |
| 5 | §1.2 Biss-SE: `gb_se(3)` an Lunge-Start/Biss (:389/:1345/:1409/:1751) → main.c:742 → `re15_audio_room_se(2)` = ROOM2090 snd1 Rec 00003214, Stimme 4, prio 2, VAG 3 = 2752 B ≈ 0,22 s; Bank-17-SE 4 wird nie gerufen; Clip 4 datenstumm | **haelt** | Code gelesen (gator.c:389/1345/1409/1751; main.c:742 `if (se_id == 3 && !frameflag) re15_audio_room_se(2)`); ROOM2090.RDT selbst geparst: snd1 EDT @0xC3D8, Rec[2] = `00 00 32 14` → prog 0/tone 3/voice 4/prio 2, VH tone[0][3] vag 3, VAG-Tabelle 2752 B → 4816 Samples = 0,218 s @22050; SE-Protokoll eigener Lauf: F0 id3, F11 id3, F28 id1, F106 id3, F343/429 id2 — nie id 4; Frame-Flag-Zensus EM23: c3/f50+f77 SE4, c5/f2 SE1, c5/f80 SE3, **kein c4-Eintrag** |
| 6 | §2.3 RE2 EM23: SE 4 @0x80100d84 (a0 @0x80100d64) in Lunge-Phase 0, danach Clip 2 frac 16 @0x80100dac-b4, Clip 3 @0x80100e20-28, Biss-Fenster Clip 3 f≥114 @0x80100e74-a4, Yaw+0x400 @0x80100eb0-c0; 4 jal-Stellen; 55-F-Sperre nur ueber 0x48-Frame-Wort | **haelt** | Eigener Opcode-Scan `jal 0x8005bd6c` in EM23_OVL_0000.BIN: genau {0x80100d84, 0x801020c4 (a0=5), 0x801021ec (a0=4), 0x80102c10 (a0=0)}; Disasm 0x80100d48-d88: `lbu v1,6(s1)` Phase, `beq v1,zero,0x80100d84 / addiu a0,zero,4`, danach Phase++; 0x80100dac-b4 `addiu a1,zero,2 / addiu a2,zero,16 / jal 0x8001a330`; 0x80100e20-28 Clip 3; 0x80100e64-a4 `+0x21A==2 && +0x14C==3 && !(f<0x72) → +0x218 \|= 0x10`; 0x80100eb0-c0 `addiu v0,v0,1024 / sh v0,118(s1)`; 0x80100424-34 `lui v1,0x4800 / and / bne / addiu v0,zero,55 / sb v0,562(s0)`; FUN_80016028 `lui 0x800 / and / srl a0,a0,28 / jal` @0x80016040-54 |
| 7 | §1.2/§1.3 Bank-Latch: EIN `s_re2se_bank_sel`; Hooks nur in pc_enemy_load_ex (Spinne → 11, Gator → 17), Roster aufsteigend, Gator im LETZTEN Slot → Latch 17; Bank 17 id 6..31 SILENT (Spinnen-Schritte 8/9 stumm, id 1 = Gator-Sample); RE2 haette fuer Gator+Spinne keine Bank | **haelt** (Code-Pfad; im laufenden Spiel nicht gemessen — das raeumt das Dossier §5 selbst ein) | audio_pc.c:1007 einziger Latch, :1112 Gate `bank_cur != bank_sel`, :1031-1044 gibt Vorgaenger-Bank frei; main.c:855 (0x25/0x26 → Hook, Bank 11 — auch Baby: `RE2SPIDER_BABY_ENEMSE_BANK 11`), :951 (0x23 → 17), :6067-6068 Slot rueckwaerts, :6082-6088 aufsteigend; `re15_audio_re2_enemy_bank` wird sonst nirgends gerufen (grep). Eigener Lauf: `Latch -1 -> 11`, `Latch 11 -> 17`; Bank 17 EDT @0x128000: id0..5 belegt, **id6..31 FFFFFFFF**; Bank 11 id1 = vag 2016 B vol 127, id8/9 = vag 1984 B. Paar-Tabelle @0x800a7400 selbst gelesen: Zeile 17 = {22,0}, Zeile 11 = {16,0}, Terminator 0xFF in Zeile 73; 0x16 nur in 17/67/68 ({22,31},{22,32}), 0x10 nur in 11/65/66 → keine Zeile {0x16,0x10}. ROOM40A0.RDT @0x25A0 = `44 00 00 23 00 00 01 16` (selbst gelesen) |
| 8 | §2.4 Spinne: Biss-SE 1 @0x80105b34-38 und @0x80105bbc-c0 jeweils nach `jal 0x801059f0`; 9 jal-Stellen; FUN_80016028 nur @0x80101074/@0x80102ae8; einziges Frame-SE c11/f5 SE3; Sonde: je Biss genau ein SE 1 | **haelt** | Scan EMS25.BIN `jal 0x8005bd6c` = {0x80101134, 0x8010189c, 0x80103304, 0x801036e8, 0x8010400c, 0x801043d0, 0x80105b38, 0x80105bc0, 0x80106050}; `jal 0x80016028` = {0x80101074, 0x80102ae8}; Disasm 0x80105b28-3c / 0x80105bb0-c4 (`jal 0x801059f0 / beq v0,zero / addiu a0,zero,1 / jal 0x8005bd6c`); Schritt-Tabelle `lbu a0,26312(at)` = 0x801066c8 @0x8010604c. Eigener Lauf: Szenario B je Sprung `SE id=1` @F54/F50/F51/F51 mit hp 200→180, A: 11 Rufe nur ids 0/8/9, sub 7 nie |
| 9 | §3 Port-Ist Zeilen (1924-1925 P0, 1939-1940 Anker xz-only, 1981-1986 P2, 2017-2022 P3, e->y = GB_WATER_Y) | **haelt** | enemy_ai_boss_gator.c gelesen: :1922 `e->y = GB_WATER_Y` (−1200, :78), :1924-1925 Teleport, :1939-1940 `re15_clip_anchor_set_pub(e,…,4,13); pl->anchor_x/z = e->anchor_x/z`, :1981-1986 zwei `re15_clip_root_motion_abs_pub`, :2017-2022 P3; enemy_ai_common.c:620-676 xz-only ohne Scale; `pl->y` wird im FRESSEN-Block nirgends geschrieben |

Nichts widerlegt. Alle zitierten Adressen, Basisregister, Offsets und Tabellenindizes stimmen; die
Messzahlen reproduzieren sich bitgleich in einem frischen Build.

## Anmerkungen (keine Widerlegung, aber Praezisierungen)

- **Zensus-Zeile der Dossier-Sonde** (`OPFER-Bank: 680 kf; POSE x!=0: 515 …`) belegt den
  Satz „POSE x/z = 0 in ALLEN kf" nicht — sie zaehlt den ganzen Keyframe-Pool. Der Satz ist
  trotzdem richtig (Gegen-Sonde je Frame der Clips 0/1, Tabelle #4).
- **Latch 17 im laufenden Spiel** ist eine Code-Folgerung, keine Messung (kein Audio-Dateilog;
  §5 sagt das). Der Pfad ist deterministisch (nur zwei Hook-Stellen, ein Latch); eine
  Widerlegung muesste einen dritten `re15_audio_re2_enemy_bank`-Rufer zeigen — es gibt keinen.
- **SE-4-Dauer**: §0.3/§1.2 sagen „2,40 s", §4.B1 „18032 B, 1,43 s Attack". 18032 B = 31556
  Samples = 1,43 s bei 22050 Hz; 2,40 s setzt ~13,1 kHz voraus. Die VH traegt keine
  Abtastrate (nur center 85). Fuer den Fix-Plan B1 ist damit die zeitliche Aussage („endet ~1 s
  nach dem Schnapp") nicht aus der VBS ableitbar, sondern aus einer frueheren Wellenform-Messung.

## Abschnitt 4 (Fix-Plan) — Schritte auf unbelegter Annahme

1. **A1 P0-Teleport „RotY·(10643,−915) skalieren"**: Im Original ist das KEIN gedrehter
   Versatz. @0x8010106c-84: `lw v0,56(s0) / addiu v0,v0,10643 / sw v0,0x800cfc30` (Spieler-x =
   Gator-x + 10643) und `addiu v1,zero,-915 / sw v1,0x800cfc38` (Spieler-z = **−915 absolut**,
   Korridor-Koordinate). Die Form „Gator + RotY·(10643,−915)" ist eine Port-Verallgemeinerung
   (steht so im Port-Kommentar); „−915·s" skaliert also einen Port-eigenen Wert, kein
   Original-Offset. Wirkung ohnehin nur in P1 (13 Frames), weil der P2/P3-Anker vom Gator
   kopiert wird, nicht aus pl.
2. **A3 y-Formel setzt voraus, dass NICHTS ausser dem Boss-Tick pl->y im Victim-Modus schreibt**
   (Dossier-Risiko (i)). Belegt ist nur: heute bleibt es 0 (gator_boss.log:356/366,
   FSYNC2 pl y=0). Ob ein Boden-Snap in game_step einen Wert ≠ 0 zurueckschreibt, ist nicht
   gemessen — nach dem Fix mit RE15_STATE_LOG pruefen, wie das Dossier selbst fordert.
3. **A3 Renderer-Annahme**: Leons Wurzel = pl->(x,y,z) + RotY(pl->rot_y)·POSE_v. Belegt fuer den
   Port ueber main.c:6788 (`model_pos_y = player_ref->y`) und :6977-6983 (Victim-Keyframes im
   PL00-Skelett); ein Live-Bild-Messwert (F9/Framedump) fehlt.
4. **A Scale 2731**: ohne @0x (Nutzer-Entscheidung) — Fix A ist geometrische Folge des Port-
   Scale-Pfads, kein Original-Wert; das Dossier sagt das (§2.2/§5).
5. **B1 Timing** („Brueller endet ~1 s nach dem 45-F-Schnapp"): haengt an der ungeklaerten
   Abtastrate (1,43 s vs 2,40 s, s.o.); die Aussage „hoerbar als Angriff, zeitlich nicht wie
   RE2" bleibt qualitativ.
6. **B2 „Maul offen ab ~f54"** (Clip 3): aus Runde 4/6 uebernommen, hier nicht neu gemessen; die
   Clip-2→3-Sequenz mit `+0x22C`-Doppelschritt (@0x80100da4 / @0x80100e18: bei +0x22C≠0 wird
   0x8001a330 ZWEIMAL pro Tick gerufen = doppelte Anim-Geschwindigkeit) fehlt im Fix-Plan
   voellig — wer „Clip 2 → Clip 3 byte-naeher" baut, muss diesen Doppelschritt mitnehmen.
7. **C Mehrbank-Cache**: reines Port-Design ohne Original-Vorbild (Dossier sagt es);
   „Kanal-Prioritaeten bleiben EIN Satz (FUN_8005c92c-Gate)" hier nicht geprueft.

## Sonden-Logs (eigener Build)

- `probe_r16_gator_finisher_anker`: LOKAL heute (7892,−1804,295) 6=0/7=0; S (5261,−2402,194) 6=1/7=1; LIVE bitgleich zu gator_boss.log.
- `probe_r16_gator_se`: Latch −1→11→17; SE F0/F11 id3, F28 id1, F106 id3, F343/F429 id2; Bank 17 id6..31 SILENT.
- `probe_r16_spider_se`: 26 Rufe, Latch 11 (Sonde allein); B: je Biss `id=1`.
- `probe_r16_sk_gator_finisher_sounds` (Gegen-Sonde): Zensus je Frame + 121-Frame-Vergleich O/S/F — Fix-Formel F (ohne s·(off_v.y−off_g.y)) ≡ S ±3, Kiefer-Treffer identisch mit Original in allen Frames.
