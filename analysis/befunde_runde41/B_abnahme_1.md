# Runde 41 — Spur B "re2gator" — Abnahme 1 (unabhängig)

Prüfer: Abnahme-1-Agent, 2026-10-24 (ab 03:58). Baum `.claude/worktrees/r41_re2gator`, HEAD c59802b5
(Zweig r41/re2gator, Basis master 116e8b0f). Geprüft wird NUR der gebaute Stand; nichts aus dem Dossier wird
ungemessen übernommen. Messdateien: Prüfer-Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn1/`.

(Fortlaufend geschrieben; Endstand 2026-10-24 ~05:2x, Gesamturteil §7.)

## 1. Lektüre + Bau

- Gelesen: AUFTRAG.md (inkl. Nachtrag 4 "RE2-artige Kameraschnitte", Entscheide B1–B5), VERTRAG.md,
  B_abnahme_0.md (Mängel M1–M6), Dossier B_re2gator.md (851 Zeilen, §8 Nachbesserung 1, §9 Nachtrag 4),
  `git log --oneline master..HEAD` (22 Commits), `git diff master...HEAD --stat` (50 Dateien, +4660/−21; der
  zweipunktige `git diff master` zeigt zusätzlich master-Neuerungen seit 116e8b0f als "Löschungen" in
  AUFTRAG.md/UEBERGABE.md/orchestrator — nicht von Spur B), kompletter Code-Diff gelesen:
  `gator_re2_r41.c` (1266 Z.), `re15_gator_re2_r41.h` (185), `gator_re2_pc.c` (276), `enemy_ai_boss_gator.c`
  (+106/−18), `main.c` (+10/−3: 1 include, 4 Haken), `re2_fx.c` (+40), `enemy_ai_re2_zombie.c` (+4), Tests/Probes.
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)`; danach alle Spur-B-
  Quellen + Header per `touch` (nur mtime) und `local_build.sh build` → 785 Schritte, `LOCAL-BUILD-OK (build)`,
  EXIT 0 (03:58:49–03:59:15). 3 Warnungen = main.c 8803/10132/10512 (`-Wdiscarded-qualifiers`, Altbestand, wie
  in Abnahme 0) — keine Warnung in Spur-B-Dateien. re15_pc.exe 03:58.
- Gezielte Unit-Riegel (Regex der Abnahme 0 = Spur + berührte Module gator/re2_zombie/re2_fx/Kraehe/Granaten/
  Wasser/Montage): `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|
  sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|
  re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|
  r36_kleidung_kraehe|r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|
  r38_wasser.*|r38_wasserzombie|r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate)$|^probe_(re2z_stomp|
  crow_kill)$" -j 8` → **100% tests passed, 0 tests failed out of 89** (33,3 s, 04:10:40–04:11:13). Einzelausgaben
  der 10 Spur-Riegel in `B41abn1/unit_r41_verbose.txt` (z.B. kamera "R1-Fall 2: Winkelwahl ohne Grenze Cut 2
  (massstab 0.0470, Grenze 0.0289), gewaehlt Cut 1", kanister "Zeitlupe VSync 4 ueber 38 Takte").

## 2. exe-Läufe (eigene exe-Kopien `re15_pc_b41a1_<Name>.exe`, Ordner `B41abn1/mess/<Name>/`)

Basis jeder Fahrt (`B41abn1/lauf.sh`): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`,
`timeout`, Elza-Fahrten (`lauf_elza.sh`) statt Titel-Autostart mit `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1`. Auswertung: `statelog.py` (Clip-Läufe Slot t=23), `schuettel.py` (Hintergrundversatz),
`sheet.py` (Bildbögen, Aufhellung nur zur Sichtung).

| Lauf | Zusatz | Zweck | Ergebnis (Kurz) |
|---|---|---|---|
| A0/A | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_EXIT_AT=925#2090 RE15_FRAMEDUMP=1-925/1` | Türnische, natürlicher Ablauf (A0 mit EXIT_AT 935 lief in den Game-Over-Neustart; gator_boss.log beider Läufe bitgleich) | Intro F6–F280, Dash F341–F640, Putz, Biss F643, FRESSEN F671–F910 |
| E1 | `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1 RE15_EXIT_AT=900#2090` | ECHTER Weg: Tür 20A0 → 2090 (`[tuer] Sequenz ... DOOR26`), Leon an der Osttür | Intro + Schnitte wie A (Rückkehr auf Leons Cut 2), danach GUARD (Clip 0) + Hochbisse |
| C/C2 | `RE15_PLAYER_POS=2500,-25000,2048 RE15_R39_MESS_SPIELER_HP=100 RE15_AUTOPILOT=xz:...(Südbecken) RE15_EXIT_AT=1500#2090` | Verfolgung im Südbecken über 1500 Bilder (Spinnen-/Bisstod per Messhaken aufgefüllt) | Dash beim Zonenwechsel 2→0, danach Zyklus Paddeln (Clip 1) → Schnapp (Clip 4) → Schwimmen (Clip 0) |
| C3 | wie C2 + `RE15_GATOR_KANISTER=-1500,-23400` | Kanister-Aufnahme in der freien Verfolgung | Aufnahme Stufe 1–7 während des Schnapps (s. Mangel) |
| K | Rezept §7 Dossier (`GB_TEST=5`, Leon (6500,-25000), Kanister (4430,-26160), Pistole, Skript `W2.0,(M0.3,MA0.1)x10`) | Kanister → Schuss → R1 | wie Bau-Lauf KK bitgleich |
| EZ | Elza: `RE15_DEBUG_JUMP=2090@250` (landet in ROOM2091, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`) `RE15_GB_TEST=1 RE15_EXIT_AT=700#2091` | beide Szenarien | gator_boss.log-Folge identisch mit Lauf A |
| S0–S3 | Türnische, `RE15_FIRE_AOT=0@398/400/1109#2090` (Leiter → 2080), Framedump 2080 | Nebenwirkung Nachbarraum: hängt der Schüttel-Versatz nach dem Raumwechsel nach? | 2080-Bilder gegen S0 (ohne Kampf) ohne Versatz — kein Befund (§4.3) |
| N | `RE15_PLAYER_POS=-8200,-25550,3072` (Nische, Blick Nord) `RE15_R39_MESS_SPIELER_HP=100`, Skript `W2,U12,W40`, OHNE `RE15_GB_TEST` | natürlicher Aggro durch Annähern (Distanz < 9000) | Aggro F201 → Intro, Schnitte F201/F271/F325/F475, Dash F565 |
| E2 | wie E1 + `RE15_R39_MESS_SPIELER_HP=100`, Skript `W15,U3,L0.55,U5,W30` | Klettern im echten Ablauf (Leon von der Rampe ins Südbecken) | `CROSS start=(5668,-11900) ende=(6500,-20700) frames=240` + Rück-CROSS |
| X | `RE15_GB_TEST=2` | erzwungener Bogen über die Rampe | `CROSS start=(4450,-11900) ende=(3350,-20700) frames=240` |
| R | Leon (6500,-25000), Pistole, Dauerfeuer ab F360 | R0/R4 im Spiel | nur 1 Treffer (Zielhilfe nimmt die Spinnen) — R4 im Spiel nicht messbar, nur Riegel `sockel` |

## 3. Urteile je Nutzer-Punkt (Wortlaut AUFTRAG.md)

### Punkt 1 — "den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst" → **erfüllt**

Gemessene Clip-Folgen des Gators (state.log Slot 20, t=23, `statelog.py`), alle Läufe deterministisch (A0/A und
EZ bitgleich in gator_boss.log):

| Lauf | Clip-Läufe (Bild von–bis, Clip) |
|---|---|
| A (Nische) | F6–F130 **6** (Durchbruch) · F131–F280 **3** · F281–F340 0 (Sperre) · F341–F490 **2** · F491–F640 **3** (Dash) · F643–F670 4 (Schnapp, Nutzer-Design) · F671–F790 **5** · F791–F910 **11** (RE2-FRESSEN) |
| C2 (Südbecken) | nach dem Dash F344–F643 Zyklus **1** (Paddeln, 14–84 Bilder, gemessene Bahn Mittel 98,0/Bild, max 258,8 = Bild 3 der Schleife 387 x 2/3) → 4 (43) → 0 (34–47) — 9 Paddel-Läufe in 1500 Bildern |
| K (Kanister) | F70–F106 **9** (R1, 37 Bilder), danach Leiche (af 36 gehalten) |
| X (GB_TEST=2) | F1–F246 0 = CROSS-Bogen, danach 1/4/0/1/4/5 |

Clip-Zensus EM23 (selbst: alle `jal 0x8001a330/0x8001a240` im Overlay, `B41abn1/em23_full.dis`): 0–11. Im Spiel
gemessen jetzt 0, 1, 2, 3, 4, 5, 6, 9, 11 (+ CROSS); Clip 10 (R4) nur im Riegel `sockel` ("R4 5 (Abstand 34..46
Treffer), R0 160"), im Spiel nicht messbar, weil die Zielhilfe (nächster Gegner) die Spinnen trifft (Lauf R: 1 Treffer
in 60 Schüssen — Spinnen entfernt Spur F/G). Clip 8 (R3) bleibt ungenutzt = Orchestrator-Entscheid B2. Der Finisher
selbst ist der RE2-FRESSEN-Pfad (Clip 4 → 5 → 11 mit EM23-Opferpaar 3, Runde 6) und läuft mit Leon und mit Elza (EZ:
F643–F910, Elza im Maul, `B41abn1/EZ_bogen.png`). Die PRÄSENTATION des Finishers (Kamera) ist Nachtrag 4 → §3.4, M1.

### Punkt 2 — Verfolgung wie RE2 (Animation, Schreie, an gleichen Stellen beschleunigt), Wände wackeln + Putz, Klettern bleibt → **teilweise**

a) **Animation + Tempo (erfüllt):** Paddeln Clip 1 / Schwimmen Clip 0 über die RE2-Hysterese (C2), Intro Clip 6→3 bei
JEDEM Aggro — auch beim natürlichen Annähern ohne Testhaken (Lauf N: Leon läuft von der Nische nach Norden, Aggro F201
bei Leon (-8440,-18171) → `RE2 INTRO start pos=(-7200,-9200)`), Dash Clip 2→3 beim Zonenwechsel in Leons Zone (A F341,
C2 F344, N F565, EZ F341). Selbst disassembliert: Intro P1/P2 @0x8010086c–948 (`ori v0,v0,0x110`, Clip 6 `addiu a1,zero,6`
@0x801008ac, Clip 3 @0x801008e4, `addiu v0,zero,110 / sb v0,568` @0x8010091c, `addiu v0,v0,1024 / sh v0,118` @0x80100940–48),
Dash-Ende `lhu v0,118 / sb zero,549 / addiu v0,v0,1024 / sh v0,118` @0x80100eb0–c0, Hysterese `lw v0,496 / sltiu v0,v0,0x65`
@0x80100b80–88 bzw. `sltiu 0x5a` @0x80100be4, +0x1F0 = `jal 0x8008d2f4` (SquareRoot0, endet `srl v0,t5,12` @0x8008d364)
von dx²+dz² @0x800265a4–e0. "An gleichen Stellen" = die festen Zonenlinien (RDT-Wasserzonen, PORT-WAHL) — in A/EZ/N
jedes Mal an z ≈ −18100 im Westkanal.

b) **Schreie (erfüllt):** debug.log A Z. 752 `[re2se] se=4 layer=0 bank=17` unmittelbar vor `[gator-re2] Putz-Sicht F342`
= Dash-Start F341 (RE2 `addiu a0,zero,4` @0x80100d64 / `jal 0x8005bd6c` @0x80100d84); weitere se=4 Bank 17 aus den 0x48-
Bildwörtern von Clip 6/3 (Z. 361/581/609/995/1024), se=2 Bank 17 der Paddel-/Schwimm-Wörter.

c) **Wände wackeln (erfüllt):** `schuettel.py mess/A/fd a_ 380 420 381 10 60`: F396 (+1,0), F397 (0,1), F398 (0,2),
F399 (0,0), F400 (0,2) = Modus-2-Tabelle @0x8009db14 Index 4–8; F401–F410 (0,0)/(0,1)/(0,1)/(0,0)/(0,0)/(1,1)/(0,0)/(0,1)/
(0,0)/(0,0) = Modus-1-Tabelle @0x8009daf4 Index 9–2 — Bild für Bild exakt die RE2-Tabellen, ausgelöst vom 0x28-Wort
Clip 2 f50 (F391) + 5 Bilder Bit 4 + 10 Bilder Bit 8. Beim Paddeln (C2 F1083–F1166) das RE2-Dauerzittern (1,1).

d) **Putz (erfüllt):** A `RE2 PUTZ runde 1..6/6 spawns=9 gesamt=54 um=(-8438,0,-25538)`; Zeichner-Messzeile
`[gator-re2] Putz-Sicht` F342–F437 `gezeichnet_vorbild` 27..98 Quads, Welt (-8260,-3500,-25588)..(-9434,-4000,-25937);
im Bild Staubwolken über Leons Kopf F348–F440 (`B41abn1/A_putz_x2.png`, 2x, aufgehellt nur zur Sichtung), ebenso in N
F601/F651 und bei Elza (EZ F351/F401). RE2-Mechanik selbst geprüft: Sce_espr_on @0x800565a4 → FUN_80056a38(0) → Tafel
@0x80011470[0] = 0x80056a78 `lui v0,0x800a / addiu v0,v0,-9404` = 0x8009db44 (Bytes `00 10 00 00 00 00 00 00 00 10 …` =
Einheitsmatrix), Lage als a3 = `addiu a3,sp,16` @0x80056618; Zeiger-Merken `sw a2,108(t0)` @0x8001cd6c und Kopie je Bild
bei Status 0x800 @0x8001d8ac–fc — der Fix (dauerhafte Matrizen statt Stack-Puffer) erklärt den Befund "Teilchen am
Ursprung, 0 Quads" der Bau-Messung.

e) **Klettern bleibt (erfüllt):** im echten Ablauf (E2: Tür 20A0 → 2090, Leon von der Rampe ins Südbecken)
`CROSS start=(5668,-11900) ende=(6500,-20700) frames=240 fenster=62..177` und später zurück; erzwungen X ebenso.

f) **MANGEL (sichtbar, durch diese Spur neu): die RE2-Korridor-Ecke von Clip 3 passt nicht ins Becken.**
Clip 3 (Intro UND Dash) dreht die Pose per Root-ry 438 → 1023 und schiebt die Wurzel quer (−2775 ×2/3), danach
`yaw += 1024` — in ROOM40A0 die Wende in den nächsten Korridorschenkel (Probe: RE2-Intro Clip 3 f149 Wurzel (492,-24096)
yaw 2048). Im Westkanal zeigt der Gator dadurch in die Westwand: Lauf A Cut 7 (RE2-Schnitt "ANGRIFF") F140–F279 ragt der
Kopf über/in die Westwand (`B41abn1/A_bogen_intro.png`), `RE2 INTRO ende pos=(-7416,-17140) yaw=2113`,
`RE2 DASH ende pos=(-7416,-22300) yaw=2048` (Blick zur Westwand x −8900). Geometrisch zwingend: der um bis zu 90°
gedrehte Leib ist 2/3 × 18313 ≈ 12 200 lang, der Westkanal x −8900..−1700 nur 7 200 breit — quer liegend steckt der Kopf
in der Westwand (Lauf N F401–F481 dasselbe Bild, Leon steht dabei unmittelbar vor dem quer liegenden Leib,
`B41abn1/N_leon_im_gator_x2.png`); die Dispatcher-Klemme hält nur die Wurzel. Der Bau-Agent hat das gemessen
(Dossier §9.8: Kopf x −9574 am Clip-6-Ende) und als Nutzerfrage §7 Nr. 5 vorgelegt, statt es zu lösen (Regel "keine
Wahlfragen", Memory reai-v2-original-oder-nicht). → M2.

### Punkt 3 — Kanister aufnehmen "GENAUSO wie im original", beim Tragen beschießbar, Sterbeanimation mit geplatztem Kiefer wie RE2 → **teilweise**

Testhaken-Lauf K (Rezept Dossier §7), bitgleich mit dem Bau-Lauf KK:
`RE2 KANISTER gesetzt (4430,-1200,-26160)` → `Stufe 1 laengs=5868 quer=17` → `angehaengt lokal=(4211,1178,5)` →
`getragen (Stufe 7)` → F70 `RE2 R1 VSync 4 (Zeitlupe) F70` / `RE2 R1 start pos=(4997,-20318) hp=25293` /
`[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1` / Clip 9 F70–F106 / `R1 ende (Leiche) pos=(3413,-25001)` /
`RE2 R1 VSync 2 F108`; Bildtakt `[gator-re2] VSync 4 (Bildbudget 66 ms) ab F70 t=11089 ms` → `VSync 2 ... ab F108
t=13605 ms` = 38 Bilder in 2516 ms = **66,2 ms/Bild** (RE2 VSync 4 = 15 Bilder/s). Bild F71–F84 (Cut 1): Blitz/Fontänen
am Kopf, dunkelrote Kiefer-Scherben, danach Leiche mit offenem Maul (`B41abn1/K_bogen.png`, `K_kopf_x2.png`). Selbst
disassembliert: `addiu v0,zero,4 / lui at,0x800e / sb v0,-998(at)` @0x8010235c–64 (= 0x800dfc1a), `addiu v0,zero,2`
@0x80102668 + `sb v0,-998(at)` @0x8010267c, `addiu v1,zero,-257 / and / sw 0x800cfbd8` @0x80102688–94; 0x800dfc1a ist das
Argument von `jal 0x80085ea0` @0x8002b990–98, und 0x80085ea0 hat die PsyQ-VSync-Struktur (`bgez a0` / Modus 1 /
Vcount-Warten). Die Aufnahme gelingt auch in der freien Verfolgung (C3: `Stufe 1 laengs=5839 quer=-95` → Stufe 7).

**MANGEL: die RE2-Fortbewegung während der Aufnahme fehlt.** RE2 Sub-1/2-Pre @0x80100964 (für Sub 1 UND Sub 2, Tafel
@0x8010463c[1]/[2]): im Fenster `lbu v1,5(s0) / sb v0(=1),550(s0) / addiu v0,zero,2 / bne v1,v0 / addiu v0,zero,257 /
j 0x80100c04` @0x80100b04–18 → `sw v0,4(s0)` @0x80100c04 = Paddeln (Sub 2) wird beim Aufnahmebeginn auf Sub 1
(Schwimmen, Clip 0 Blend 32) gesetzt; solange +0x226 ∉ {0,7} überspringt `lbu a0,550 / beq a0,zero / addiu v0,zero,7 /
bne a0,v0,0x80100c08` @0x80100b20–30 die Paddel-Hysterese, und solange der Kanister im Wasser liegt (Raumflag Bit 0) führt
jeder Fenster-Fehlschlag `beq v0,zero,0x80100c08` @0x80100aec an der Hysterese vorbei. Im Port berührt die Aufnahme die
Fortbewegung nicht (gator_re2_r41.c `gr2_kan_takt` setzt nur `s_kan.stufe = 1`; `re15_gr2_chase_clip` kennt den
Kanister nicht) — gemessen C3: die Stufen 2–7 laufen WÄHREND des Schnapps Clip 4 F660–F702 (debug.log: Stufe 1/2 vor
`[walk] F660`, Stufe 7 vor `[walk] F690`), der Gator beißt beim Aufnehmen. Das Dossier nennt "Sub 2 -> 0x101
(@0x80100b10-18)" selbst (§2.5), umgesetzt ist es nicht. → M3.

### 3.4 Nachtrag 4 — RE2-artige Kameraschnitte (Durchbruch/R1/Finisher/Kanister/Explosion) → **teilweise**

Form selbst geprüft (EM23_OVL_0000.BIN bzw. RE2 PSX.EXE): Intro `addiu v0,zero,70 / bne v1(lbu 333),v0 / jal 0x8002c7ac /
addu a0,zero,zero` @0x80100894–a4 (Cut 0 bei Bild 70), `jal 0x8002c7ac / addiu a0,zero,1` @0x801008c4–c8 (Clip-6-Ende),
Sperre frei `addiu v1,zero,-17 / and / addiu v1,zero,-257 / and / sw` @0x801008f8–918; R1 `ori v0,v0,0x100` @0x8010220c,
Cut 6/13 + `slti v0,v0,-13300` @0x80102218–34, `ori v0,v0,0x40` @0x80102248, `jal 0x8002c7ac / addiu a0,zero,5`
@0x80102268–6c, Rückkehr bei +0x218&0x40 auf 6/13 @0x801024a4–ec; FRESSEN P0 `ori v0,v0,0x100` @0x80100fcc + Cut 7/14
@0x80100fd4–ff4 ohne Freigabe; Schnitt-Setter FUN_8002c7ac `sb a0,0x800cfbf2` @0x8002c7b8; Zonenkamera nur bei Bit
0x100 = 0 `andi v0,v0,0x100 / bne` @0x8002639c–a0; Anwendung erst nach `jal 0x80085ea0` (VSync) @0x8002b998 und
`lh 0x800d4820` / `lbu 0x800cfbf2` @0x8002b9f0–f8. Soll-Tabelle `k_blick` mit der Probe nachgerechnet (`gcc -std=c11
-DRE15_PLATFORM_PC=1 tools/r41_re2gator/probe_kamera_re2.c` gegen libre15_engine.a, Ausgabe = Dossier §9.2: Cut 10
(140,126) Tiefe 11734 +1.00, Cut 0 (145,116) 8898 +0.35, Cut 1 (183,109) 16318 +0.83, Cut 5 (170,111) 13113 +0.99,
Cut 7 (204,118) 11175 +1.00).

Gemessen (state.log `cam=`, Framedump): A/EZ `F6 cam=7, F76 cam=0, F130 cam=7, F280 cam=0`, im Bild jeweils ab dem
Folgebild (F7/F77/F131/F281) wie RE2; E1 (echte Tür) dieselben Schnitte, Rückkehr auf Leons Cut 2; N (natürlicher
Aggro) F201/F271/F325/F475; K R1 → Cut 1 F70 (sichtbar F71), zurück F106, frei F108; die Maßstab-Grenze verwirft Cut 2
(`KAMERA-KANDIDAT ... Cut 2 ... massstab=2839 >grenze`). Kanister-Aufnahme ohne Schnitt = RE2. Der Integrations-Riegel
pinnt die Folge `DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0`.

**MANGEL (Finisher):** (a) Im Nischen-Ablauf (A, EZ, C) wählt die Kamerawahl für FINISHER den schon aktiven Cut 0
(`RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 0 F657 ... frontal=-605 soll=4094`, state.log bleibt `cam=0`) — es gibt
keinen sichtbaren Schnitt, der Gator wird von hinten-seitlich gezeigt (RE2 Cut 7: frontal +1.00, Leon vorn im Bild).
(b) F735–F745 Weißblende, ab F750 übernimmt die RE1.5-Todeskamera (Orbit; F755 steht die Kamera im Gator-Leib, Bild nur
Hautpixel; `B41abn1/A_finisher.png`) und bricht den Finisher-Schnitt nach ~80 Bildern mitten in Clip 5 ab — RE2 hält
Cut 7 gesperrt bis Game Over (FRESSEN P0 @0x80100fbc–ff4 hat keinen Freigabe-Pfad). Der Bau-Agent hat beides gemessen und
als Nutzerfrage §7 Nr. 4/6 geparkt. Nach Nachtrag 4 ("Ja, ich möchte RE2-artige Kameraschnitte") ist die RE2-Form
verlangt → M1. (Nebenbefund N: bei Aggro aus Leons Cut 7 wählt auch DURCHBRUCH den schon aktiven Cut 7 → kein Schnitt;
das ist Folge derselben Wahlregel und für den Durchbruch harmlos, weil das Bild frontal ist.)

## 4. Gates

### 4.1 RE-Gate
- Diff-Grep (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach "deferred|tunable|interim|for now|faithful|
  plausibel|TODO|FIXME|XXX|vorerst|geschaetzt|ungefaehr|approx": **0 Treffer**. Neue Env-Schalter nur
  `RE15_GATOR_KANISTER` (der verlangte Testhaken) und `RE15_GB_STUMM` (Log aus).
- Konstanten-Scan von gator_re2_r41.c (Zahlen ohne `@0x`/PORT/NUTZER in derselben Zeile): alle Treffer sind
  Feld-Offsets mit +0x-Namen, Q12-Arithmetik, Tabellen mit @0x-Kopf oder als gemessen dokumentierte Werte (k_blick =
  Probe, nachgerechnet §3.4); `+ 267` (Kanister-Radius aus dem MD1) als PORT-WAHL kommentiert.
- **Selbst disassembliert (re2_disasm.py; EM23_OVL_0000.BIN bzw. info/re2leon/PSX.EXE), alle stimmen mit dem Behaupteten:**
  R1-VSync @0x8010235c–64 / @0x80102668 / @0x8010267c, R1-Sperre-frei @0x80102688–94, VSync-Ruf @0x8002b990–98 +
  0x80085ea0 = VSync, Spielwert 2 @0x8003b9a8–b0, Schnitt nach VSync @0x8002b9e0–ba0c, Zonenkamera-Bit @0x8002639c–a0,
  FUN_8002c7ac @0x8002c7b8, Intro-Schnitte/Sperren/Yaw @0x8010086c–948, Dash-Ende-Yaw @0x80100eb0–c0, FRESSEN-Sperre +
  Cut 7/14 @0x80100fcc–ff4, R1-Schnitt @0x8010220c–6c + Rückkehr @0x801024a4–ec, Sce_espr_on @0x800565a4–0x80056618 +
  FUN_80056a38/Tafel 0x80011470 → Einheitsmatrix 0x8009db44, Matrix-Zeiger @0x8001cd6c + Kopie @0x8001d8ac–fc, Op 11
  @0x8001e4b4 (Tafel @0x8009d894) und Op 73 @0x8002387c, Distanz +0x1F0 @0x800265a4–e0 + SquareRoot0 @0x8008d2f4, HURT-
  Selektor @0x80101eac–fe8, Kanister-Fenster/Sub-Wechsel @0x80100a0c–0x80100c04 (dort der Befund M3).
- **Falsche/überholte Belege (klein, M4):** re2_fx.c:1579 `Op 73 = FUN_8002387c (Tabelle @0x8009d99c)` — der Tafelplatz
  von Op 73 ist **@0x8009d98c** (`table 0x8009d868`: [73] 0x8009d98c → 0x8002387c, [77] 0x8009d99c → 0x80023c70); die
  Funktionsadresse stimmt. gator_re2_r41.c:28 und Dossier §3.2 (Z. 257) führen "kein `yaw += 0x400` nach Clip 3" und
  "Dash/Intro nur bei freier Bahn" noch als PORT-WAHL, obwohl Nachbesserung 1 beides umgekehrt hat (Code: `yaw += 1024`
  @0x80100eb0-c0/@0x80100938-48, kein Bahn-Gate). test_r41_re2gator.cmake:47 "Leon ... hp < 0 bei F628 (Spinnen)" —
  gemessen stirbt Leon im Lauf A am Biss F656 (20 → −30), FRESSEN ab F657.
- "Erklärt der Fix den Befund?": Abnahme-0-M1 (Dash/Intro zündeten nie) — ja, Bahn-Gate weg + bildweise Wurzel durch die
  Klemme, gemessen A/C2/N/EZ; Abnahme-0-M2 (Putz) — ja, Lage + Einheitsmatrix, Teilchen sichtbar um Leon; M6 — ja,
  `Raum-SE 15`-Zeile in K. Die drei Fortsetzung-3-Befunde (Maßstab-Grenze, Zeitlupe, Putz-Matrix) sind belegt und gemessen.

### 4.2 Vertrag (VERTRAG.md)
- Bank-9-Bits / Nachrichten-Ids: keine (Diff-Grep `game_flag_set|MSG_ID|msg_id|bank9` leer) ✓.
- Pfade (`git diff master...HEAD --name-only`): engine/src, include, platform/pc (main.c + neue gator_re2_pc.c),
  shared_assets/RE2/ (KANISTER40A0.MD1/.TIM, GATOR40A0.VBS — im Dossier §7 genannt), tests/unit/probes/r41_re2gator.cmake +
  tests/integration/test_r41_re2gator.cmake (Probe-Dateien, per `file(GLOB ... probes/*.cmake)` eingesammelt; keine
  Test-CMakeLists geändert), tests/unit/probe_gator_{fress,sweep}.c (mit Beleg angepasst), tools/r41_re2gator/ ✓. Keine
  release/, platform/android/, shared_assets/PSX/, pri/, synchro/ ✓.
- Gemeinsame Dateien: B5 nahm 3 main.c-Haken an; inzwischen 4 (+ Bild-Deckel `re15_gr2_pc_bildbudget`, 3 Zeilen,
  markiert "Runde 41 Spur B") — klein, aber über B5 hinaus: bei der Integration mit D/F/G prüfen (kein Mangel).
- Tests: 10 Unit-Riegel + 1 exe-Riegel, messend (Ausgaben §1), parallel-fest (Zufalls-Id, eigene exe-Kopie
  `re15_pc_r41b_<id>.exe`, nur die eigene Kopie wird gelöscht), Linux-Muster wie die vorhandenen Gator-Proben
  (`m` gelinkt, `cmake_minimum_required`). **Gezielte Läufe am gebauten Stand:** unit 89/89 (§1);
  `ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate)$" --timeout 900 -j 4` →
  **100% tests passed, 0 tests failed out of 4** (04:50:15–04:53:10; r41_re2gator 34,59 s, r36_schuss 140,64 s,
  r35_granate 161,48 s, r38_wasser 174,99 s), keine roten Fenster-Haken.
- Kein Riegel pinnt den Sub-Wechsel der Aufnahme (M3) oder die Kopf/Wand-Lage nach Clip 3 (M2).

### 4.3 Nebenwirkungen
- Elza (ROOM2091, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `JUMP -> 209 ALLIGATOR POOL (ROOM2091)`):
  gator_boss.log-Folge identisch mit Leon (Intro/Schnitte/Dash/Putz/Finisher) ✓ — der Boss gilt für `(raum & 0xFFFE) ==
  0x2090` (enemy_ai_boss_gator.c:313); die Aussage der Abnahme 0 "der Boss-Code kennt 0x2091 nicht" war falsch.
- Echter Türweg 20A0 → 2090 (`[tuer] Sequenz Archiv 2 DOOR26`) ✓; Leiter 2090 → 2080 während des Schüttelns (S1 F398,
  S2 F1109, S3 F400): 2080-Bilder gegen S0 (Leiter-Ausstieg ohne Kampf) beste Verschiebung (0,0) in allen
  Stichproben (S1 F50–F300 und S3 F2–F100 diff 0,00, S2 F5–F100 diff 0,00, F120 0,24) — kein nachhängender
  Hintergrundversatz ✓.
- Integration r36_schuss / r38_wasser / r35_granate grün (Granaten-Gator, Wasser, Treffer-Bruecke) ✓.

## 5. Nebenbefunde (keine Mängel dieser Spur, zur Kenntnis)
- Zielhilfe trifft in 2090 fast nur die Spinnen (Lauf R: 25300 → 25280 in 60 Schüssen) — R0/R4 im Spiel praktisch
  unerreichbar, bis Spur F die Spinnen entfernt.
- Waffenschaden läuft über die RE1.5-Zeile (Pistole 7) statt RE2 (17 @0x800A4A28, Dossier §6) — "geschwächt" (< 25000)
  erst nach 43 statt 18 Pistolentreffern; relevant für Spur G ("oft genug angeschossen").
- E2 F1126–F1157 (Leon auf der Rampe, Gator an der Südkante): Clip 4/0 wechseln jedes Bild (af 1 fest, x −48/Bild =
  off[0] von Clip 0 ×2/3) — Pendeln GUARD ↔ CHASE im Nutzer-Design-Teil; nicht dieser Spur zugeordnet, nicht weiter
  verfolgt.
- X2 (Messartefakt meiner Lage: Leon per Band 1 neben die Plattform gesetzt): der Biss-Knockdown trug Leon bis z +2122
  aus dem Becken (`AUSSEN ... mo=13 gph=3`) — Nutzer-Design-Knockdown, kein Spur-B-Code.
- Kanister im dunklen Becken kaum sichtbar (RE2-Textur, Dossier §4) — Spur G.

## 6. Mängel (nummeriert, nachprüfbar)

1. **M1 (Nachtrag 4, Finisher-Kamera nicht RE2).** Nachprüfung: Lauf A (`RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1
   RE15_EXIT_AT=925#2090 RE15_FRAMEDUMP=1-925/1`): gator_boss.log `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 0 F657
   ... frontal=-605 soll=4094`, state.log `cam=0` vor und nach F657 (kein Schnitt, Ansicht von hinten-seitlich);
   Bilder F735–F745 weiß, F750–F920 RE1.5-Todeskamera (F755 Kamera im Gator-Leib). RE2: FRESSEN P0 `ori v0,v0,0x100`
   @0x80100fcc + `jal 0x8002c7ac` Cut 7/14 @0x80100fd4–ff4, kein Freigabe-Pfad bis Game Over. Verlangt: der RE2-Finisher-
   Schnitt muss sichtbar schneiden (Wahl darf den aktiven Cut nicht als "Schnitt" zählen bzw. braucht eine frontale
   Lösung mit Beleg) und bis Game Over stehen (RE1.5-Todeskamera im Gator-Finisher unterdrücken). Keine Nutzerfrage —
   die RE2-Form ist seit Nachtrag 4 Vorgabe.
2. **M2 (Punkt 2, sichtbar): Clip-3-Wende (Intro + Dash) dreht den Gator im Westkanal in die Westwand.** Nachprüfung:
   Lauf A Bilder F140–F279 (Cut 7, `B41abn1/A_bogen_intro.png`) bzw. Lauf N F401–F481: Kopf über/in der Westwand;
   `RE2 INTRO ende ... yaw=2113`, `RE2 DASH ende ... yaw=2048` bei x −7416 (Wand −8900); Leib 12 200 lang, Kanal 7 200
   breit. Verlangt: belegte PORT-WAHL für Startrichtung/Wende im Becken (z.B. Intro-/Dash-Yaw so, dass die RE2-Wende vom
   Rand weg zeigt) mit Bildbeleg, dass Kopf und Leib im Wasser bleiben; Riegel auf die Kopflage (Bone 6) gegen die
   Wandflächen.
3. **M3 (Punkt 3, "GENAUSO wie RE2"): Kanister-Aufnahme ohne den RE2-Fortbewegungswechsel.** Nachprüfung: Lauf C3
   (`RE15_PLAYER_POS=2500,-25000,2048 RE15_R39_MESS_SPIELER_HP=100 RE15_GATOR_KANISTER=-1500,-23400` + Autopilot
   Südbecken): Stufen 2–7 laufen während Clip 4 F660–F702 (Schnapp) statt im Schwimmen. RE2 @0x80100b04–18 →
   @0x80100c04 (`sw 0x101,4(s0)`: Sub 2 → Sub 1 beim Aufnahmebeginn), Hysterese-Sperre solange +0x226 ∉ {0,7}
   @0x80100b20–30 und solange der Kanister liegt (Fenster-Fehlschlag `beq v0,zero,0x80100c08` @0x80100aec). Verlangt:
   umsetzen und im Riegel `kanister` pinnen (Clip 0 Blend 32 ab Stufe 1, keine Paddel-Hysterese bis Stufe 7).
4. **M4 (klein, Beleg-Pflege):** re2_fx.c:1579 Tafelplatz Op 73 @0x8009d99c → richtig @0x8009d98c; gator_re2_r41.c:28 und
   Dossier §3.2 (Z. 257) nennen überholte PORT-WAHLEN ("kein `yaw += 0x400`", "nur bei freier Bahn");
   test_r41_re2gator.cmake:47 überholte Todesursache (Spinnen F628 statt Biss F656).

## 7. Gesamturteil

`bestanden = false`. Gegenüber Abnahme 0 ist der Kern nachgebessert und am gebauten Spiel belegt: RE2-Intro und
Zonen-Dash zünden im echten Ablauf (auch über die echte Tür 20A0 → 2090, beim natürlichen Annähern und mit Elza in
ROOM2091), die Wände wackeln Bild für Bild nach den RE2-Tabellen, der Putz ist sichtbar um Leon, das Klettern über die
Rampe tritt im echten Ablauf ein, Kanister → Schuss → Kiefer-Explosion mit RE2-Zeitlupe (66,2 ms/Bild) und Raum-Brüller
läuft, die RE2-Schnittform ist für Durchbruch/R1 belegt und gemessen, alle 30+ selbst geprüften Adressen stimmen
(eine Tafeladresse im Kommentar falsch). Offen bleiben M1 (Finisher-Kamera nicht RE2: kein sichtbarer Schnitt, RE1.5-
Todeskamera nach ~80 Bildern), M2 (Clip-3-Wende in die Westwand, sichtbar im RE2-"ANGRIFF"-Schnitt) und M3 (Aufnahme ohne
RE2-Sub-Wechsel), dazu M4 (Doku). M1 und M2 hat der Bau-Agent selbst gemessen, aber als Nutzerfragen geparkt statt nach
dem Original zu entscheiden. Punkt 1 erfüllt, Punkte 2 und 3 sowie Nachtrag 4 teilweise.
