# RE2 → RE1.5-Port: Ermittlungsbericht (Endkampf, Menue-Audio, Tuer-Sequenz)

Vorbemerkung zur Beweislage: Alle Angaben sind gegengeprueft. Wo die Gegenpruefung eine Aussage gekippt hat, steht sie hier nicht — ausser als ausdrueckliche `⚠ WIDERLEGT`-Warnung, damit sie nicht ein zweites Mal auftaucht. Stufen: **[byte-belegt]** = selbst disassembliert/gelesen, **[abgeleitet]** = Bytes stimmen, Deutung nicht belegt, **[offen]** = nicht ermittelt.

**Pfad-Warnung vorab:** `CDEMD0.EMS` liegt unter `info/re2leon/PL0/PLD/`, **nicht** unter `COMMON/`. Alle Sektor-Angaben unten beziehen sich auf diese Datei (Byte-Offset = Sektor × 0x800).

---

# 1. RE2-Endkampf — Boss, KI, Modell, Sound

## 1.1 Wo der Endkampf stattfindet

**[byte-belegt]** STAGE7 = ROOM7000..ROOM7040. Tuer-Records sind Opcode `0x3B`, 32 Byte, Handler `@0x80054be4`, PC-Advance `@0x80054c40 addiu v0,v0,32`. Ein Scan aller 250 `ROOM*.RDT` auf Ziel-Stage-Byte 6 liefert genau 17 Records: Eingaenge ROOM6010@0x502, ROOM6170@0x35a2, ROOMF170@0x3572; innen 7000@0x143a→1, 7000@0x145a→3, 7010@0x1e52→0, 7010@0x1e72→2, 7020@0x1106→1, 7030@0x1c58→0, 7030@0x1c78→4, 7030@0x2470→3, 7040@0x10fe→3; dazu ROOMG000 (2), ROOMG010 (2), ROOMG040@0x10d6.

**ROOM7040 hat exakt eine Tuer (zurueck nach 7030) = Sackgasse = Arena.**

**[abgeleitet]** Die Feldlage `+22 Stage / +23 Raum / +24 Cut` folgt aus Datenkonsistenz. Der **konsumierende Code wurde nie gefunden**: ein EXE-weiter Scan auf die AOT-Tabelle `0x800ce558` findet nur Setzer (`0x800530c4, 0x80054b04, 0x80054b70, 0x80054bf4, 0x80054c60`) und zwei Loads (`0x80055170, 0x800551f4`). Der 610-Records-Zensus in Abschnitt 3 bestaetigt die Feldlage empirisch, aber nicht per Instruktion.

⚠ WIDERLEGT: "ROOM7xx0 = Szenario A, ROOMGxx0 = Szenario B" ist **nicht** belegt — die EXE-Routine, die aus Stage+Szenario den RDT-Dateinamen bildet, wurde nie lokalisiert. Ausserdem ist die G-Kette **keine Kette**: ROOMG010/G020/G030 haben je nur 2 SCD-Subs und **beide** MSG-Header-Eintraege = 0 (kein Text). Nur ROOMG000 (13 Subs) und ROOMG040 (16 Subs, Text wortgleich zu ROOM7040) sind gefuellt.

## 1.2 Wer der Boss ist

**[byte-belegt]** `Sce_em_set` = Opcode `0x44`, 22 Byte, Handler `@0x8005714c`, Advance `@0x800576e4 addiu v1,v1,22`.

**Feldlage relativ zum Opcode-Byte** (das ist die korrigierte Version — siehe Warnung unten):
`+2` Slot (`@0x80057204 lb v0,2(v0)`, **signed**) · `+3` **Typ** → Entity+0x08 (`@0x80057334 lbu v0,3(v1)` / `@0x8005733c sb v0,8(s0)`) · `+4` u16 → +0x10E · `+6` Etage → +0x106, und +0x1C2 = −1800×floor (`@0x800573a0-c0`) · `+7` **Sound-Id** → +0x1FA · `+8` → +0x1CF · `+9` em_flag → +0x1CE · `+10/12/14` X/Y/Z · `+16` Yaw → +0x76 · `+18/20` → +0x158/+0x15A.

⚠ WIDERLEGT: Die Lesart "kind steht auf Byte +5" (aus dem PC-Stand des Bedingungs-Evaluators `FUN_80053f50`) ist falsch. Der Haupt-Interpreter `@0x800536fc lw v0,28(s0) / @0x80053704 lbu v0,0(v0)` setzt den PC **auf** das Opcode-Byte. Beweiskette: Byte+3 → Entity+0x08 → Binder `@0x8001aac8 lbu s0,8(s1)` / `@0x8001ab04 addiu s0,s0,-16` / `@0x8001ab48 sll s0,s0,2` = TOC-Index. Empirisch: ROOM7040 hat fuenf **lueckenlos** anschliessende 22-Byte-Befehle mit Byte+2 = 0,1,2,3,4.

**ROOM7040.RDT Rohbytes:**
```
@0x0011ac: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00
@0x0011c2: 44 00 01 37 01 00 00 27 00 ff 00 83 00 00 00 83 ...
@0x0011d8 / @0x0011ee / @0x001204: 44 00 0{2,3,4} 37 ...
```
Identisch in ROOMG040.RDT `@0x001184..0x0011dc`.

**[byte-belegt]** Census ueber alle 250 RDTs: Typ `0x36` kommt **2×** vor (7040, G040), Typ `0x37` **8×** (4+4, dieselben zwei Raeume). In keinem anderen Raum.

**[byte-belegt]** Alle fuenf Records sind auf **(−32000, 0, −32000)** geparkt (`00 83` = 0x8300). Der Boss wird **nicht** aus dem Record positioniert — das macht das Raum-Skript. Zum Kontrast ROOM7020@0x1236 (Typ 0x2B): (−8000, 9000, −25000).

**[byte-belegt]** Typ 0x36 startet **schlafend**: Byte+4/5 = `00 80` = 0x8000 → Entity+0x10E. Die Tick-Schleife ueberspringt die KI bei gesetztem Bit: `@0x80026590 lhu v0,270(s0) / @0x80026598 andi v0,v0,0x8000 / @0x8002659c bne v0,zero,0x800265fc`. Das Skript muss ihn scharfschalten.

**[byte-belegt]** em_flag `+9` = 0xFF bei allen fuenf → kein Kill-Bitmap-Test, spawnt bei jedem Betreten neu (`@0x800571c0 lbu a1,9(v0) / @0x800571c8 beq a1,255 → Test ueberspringen`).

**[byte-belegt]** Abbruchbedingung: ROOM7040 Bank1/sub07 pollt Member `0x17` des Gegners in em_set-Slot 0:
```
@0x001996: 2e 03 00       work_set kind=3, idx=0
@0x00199a: 3d 10 17       member_copy var0x10 <- member 0x17
@0x00199e: 26 00 06 10 02 00   AND 2
... danach @0x0019be: 51 01 05 00 00 00 (bgm_control) / @0x0019ce: 29 0b (cut_chg) / @0x0019ea: 59 00 0d 00 (xa_on 0x0d)
```
`work_set kind=3` → `@0x8005597c sll v0,a1,2 / lw v0,-484(at)` = `0x800CFE1C + idx*4`; em_set legt den Entity-Zeiger genau dorthin (`@0x80057234 sw v1,15404(v0)`, s4=0x800CC1E8).

⚠ Praezisierung: Es gibt sehr wohl `work_set kind=3` auf Slot 1 und 3 (@0x17a0, @0x1a10, @0x1a1c) — nur der **member-0x17-Poll** existiert allein fuer Slot 0.

**[byte-belegt] Der Name der Kreatur steht nicht in den Daten.** Englischer MSG-Block ROOM7040 `RDT+0x40 = 0x1cf4`, ptrs [6,38,125]; Eintrag[1] = *"I can't put my friends in danger / I have no choice but to stop **that creature** here."* (Codierung ASCII = Byte+0x24). Ein ASCII-Scan der RE2-EXE inkl. der um −4 verschobenen Stringtabelle ab ~0x8ed00 (`OAIE;=QPKI=PE?` → `SEMI_AUTOMATIC`) liefert nur Item-/Beschreibungstexte — **kein Eigenname eines Gegners**. Jede Namensnennung waere eine Erfindung.

## 1.3 Die KI — Adresse, Tabelle, Werte

**[byte-belegt]** Laufzeit-Dispatch: `@0x800265dc lbu v1,8(s0) / sll v1,v1,2 / addu v1,s3,v1 / lw v0,15476(v1)` mit s3 = 0x800CC1E8 → **Tabelle 0x800CFE5C**. Im EXE-Image komplett `0x00000000` (96/96) — sie wird beim Overlay-Wechsel gefuellt aus der Bank-Tabelle `0x800CFFDC`, Stride 384 (`@0x8001b7ec-0x8001b80c`).

**Statische Bank-Eintraege:**
| Typ | Bank 0 (@0x80100000) | Bank 1 (@0x8010D000) | Setzer |
|---|---|---|---|
| 0x36 | `0x801000BC` | `0x8010D0BC` | `@0x8001b45c` / `@0x8001b694` |
| 0x37 | `0x80100178` | `0x8010D178` | `@0x8001b46c` / `@0x8001b6a4` |
| 0x2B | `0x8010187C` | `0x8010E87C` | `@0x8001b3cc` / `@0x8001b60c` |

⚠ WIDERLEGT: "Die Boss-KI liegt nicht im Repo / ist zu 0 % byte-belegt." **Falsch.** Die Bytes sind vollstaendig da, als Chunk in `CDEMD0.EMS`. Ich habe sie geschnitten und disassembliert.

**[byte-belegt] KI-Wurzel Typ 0x36** (Schnitt `CDEMD0.EMS[3456*2048 .. +23476]`, Ladebasis 0x80100000):
```
801000bc  addiu sp,sp,-56
801000c4  addu  s3,a0,zero
801000cc  lw    v0,-1060(v0)      ; 0x800CFBDC
801000d0  lui   v1,0x2000
801000e4  bne   v0,zero,0x8010038c ; Gate
801000ec  lbu   v1,467(s3)         ; +0x1D3 Timer
801000f4  andi  v0,v1,0x7f
80100100  sb    v0,467(s3)
80100104  lhu   v0,550(s3)         ; +0x226
--- DISPATCH ---
80100164  lbu   v0,4(s3)           ; Zustandsbyte Entity+0x4
8010016c  sll   v0,v0,2
80100178  lw    v0,21964(at)       ; = 0x801055CC + zustand*4
80100180  jalr  v0
```

**[byte-belegt] Zustands-Zeigertabelle @0x801055CC**, 8 Slots:
`[0]=0x801003CC [1]=0x80100784 [2]=0x801025BC [3]=0x80102BBC [4]=0x80103834 [5]=0 [6]=0 [7]=0x80103878`; `[8]` ist bereits Datenmaterial. Kein `sltiu`-Bereichstest vor dem Dispatch.

**[byte-belegt] Start-HP = 600, bzw. 400 bei gesetztem Bit 0x20 in `0x800CFB74`** (Init-Zustand, Slot 0):
```
801003e8  sw    v0,4(s0)          ; Zustand := 1
801003fc  addiu v0,zero,600
80100400  sh    v0,342(s0)        ; 342 = 0x156 = HP
80100404  lw    v0,0(v1)          ; 0x800CFB74
8010040c  andi  v0,v0,0x20
80100410  beq   v0,zero,0x8010041c
80100414  addiu v0,zero,400
80100418  sh    v0,342(s0)
```
Weitere HP-Stores desselben Overlays: `0x80103004`, `0x80103060` (Phasenwechsel-Kandidaten, [offen]).

**[byte-belegt] Die vier Begleiter Typ 0x37 sind UNVERWUNDBAR** — HP wird mit −1 initialisiert:
```
8010052c  sw    v0,4(s1)
80100530  addiu v0,zero,-1
80100534  sh    v0,342(s1)        ; HP := -1
```
Das ist der **einzige** `sh` auf Offset 342 im gesamten 0x37-Overlay (Vollscan). Damit traegt die Rahmung "Endgegner + 4 toetbare Begleiter" nicht: die vier sind Hindernisse/Gefahren. Das erklaert auch, warum das Skript nur fuer Slot 0 eine Abbruchbedingung pollt.

**[byte-belegt] Schaden an den Spieler** (`FUN_800401D4`, a0 rueckwaerts aufgeloest):
- Typ 0x36: `@0x80100904 a0=500`, `@0x80103EA0 a0=500`, `@0x8010416C a0=Register`.
- Typ 0x37: `@0x801016D0 a0=15`, `@0x80102D78 a0=15`, `@0x80103730 a0=15`, `@0x801037E8 a0=10`.

500 ist bei Spieler-HP in der Groessenordnung 200 ein Sofort-Tod (Griff/Finisher), kein regulaerer Treffer — [abgeleitet], da die Max-HP nicht mitgemessen wurde.

**[byte-belegt] Schadens-API `FUN_800401D4`**: Spieler-HP = u16 bei `0x800CFD4E` (= Spielerbasis 0x800CFBF8 + 0x156). Skalierung: `@0x800401FC slti v0,v0,121` / `@0x80040200 bne v0,zero,0x80040214` — der Boost `dmg += dmg>>1` greift bei **HP ≥ 121**, nicht bei HP < 121. (Die inverse Lesart war ein Abschreibfehler.)

**[byte-belegt] Schaden AM Boss.** Treffer-Aufloeser `FUN_800470C8`. Zeigertabelle `0x800A6A88` (nur Indizes 0x00..0x3F sind Zeiger; ab 64 Daten — die Grenze deckt sich exakt mit der Gegner/NPC-Trennung `@0x80057410 sltiu v1,v1,0x40`). `[0x36] = 0x800A5EDC` (eigene Tabelle), `[0x37] = 0x800A412C` (Default), `[0x2B] = 0x800A5478`.

Schaden = `((*(DMG[typ] + (waffe-1)*20)) >> (zone*10)) & 0x3FF`, danach `HP(+0x156) -= dmg`; HP<0 → Zustand +0x4 = 3 (Tod), sonst 2 (Reaktion); +0x1D2 = 1.

**Typ 0x36, Zone0/1/2 je Waffen-ID 1..19:**
```
 1:[15,0,0]     2:[17,16,15]   3:[17,16,15]   4:[17,16,15]   5:[75,73,70]
 6:[100,100,100] 7:[50,48,45]  8:[70,68,65]   9:[80,80,80]  10:[70,70,5]
11:[70,70,10]  12:[20,20,20]  13:[17,16,15]  14:[60,60,60]  15:[10,10,10]
16:[5,5,5]     17:[200,200,200] 18:[17,17,17] 19:[17,16,15]
```

**[byte-belegt] Vorberechneter Spieler-Abstand.** Die Tick-Schleife legt **vor** dem KI-Aufruf `sqrt(dx²+dz²)` in Entity+0x1F0 ab: `@0x800265a4 lw v0,56(s0)` … `@0x800265d4 jal 0x8008D2F4` (GTE-Ganzzahlwurzel) … `@0x800265e0 sw v0,496(s0)`. Alle Abstands-`sltiu` der Overlays vergleichen dagegen.

**[byte-belegt] Gemeinsame EXE-Bibliothek** (exakte Schnittmenge der jal-Ziele aller fuenf vorhandenen Overlays, **genau 14**): `0x800152C8, 0x80015558, 0x80015614, 0x80015910, 0x80015FE8, 0x80016480, 0x8001BF10, 0x8002959C, 0x80035530, 0x8003567C, 0x8003947C, 0x80039514, 0x800401D4, 0x8005BD6C`.

Aufrufzahlen im 0x36-Overlay: RNG `0x80015FE8` ×144, Sound `0x8001BF10` ×46, ENEM-SE `0x8005BD6C` ×38, Anim-Advance `0x8002959C` ×20, Effekt `0x8003947C` ×16, Spieler-Schaden ×3.

⚠ WIDERLEGT: `FUN_80015910` ist **kein** Positions-Kegeltest. Es vergleicht zwei **Blickrichtungen** (+0x76 gegen +0x76): `lh v0,118(a1) / lh v1,118(a0) / subu / addiu +1024 / andi 0xfff / slti 2048`. Keine Position, kein atan2 — ein Facing-Alignment-Test.

⚠ [abgeleitet], nicht belegt: die Deutung von `0x800CFBDC & 0x20000000` als "Freeze". Die Zelle hat 36 Zugriffe in der RE2-EXE; keine Stelle wurde gezeigt, die das Bit als Freeze **setzt**.

## 1.4 Modell und Textur

**[byte-belegt] Gegner-TOC @0x8009ADF4** (Datei 0x8B5F4), 8 B je Eintrag (u32 Sektor, u32 Groesse), 4 Records je Typ ab 0x10, Index = `(typ−0x10)*4 + k`:
- `k=0` AI-Overlay (gegen 0x8010D000 gelinkt), `k=1` AI-Overlay (gegen 0x80100000), **`k=2` TIM**, **`k=3` EMD**.

⚠ WIDERLEGT: die Zuordnung "k=2 = EMD, k=3 = TIM" ist **vertauscht**. Ground truth im Repo: Typ 0x20 idx66 (k=2) size 66592 = `EM_TYPE20.TIM`, idx67 (k=3) size 121144 = `EM_TYPE20.EMD`. Zusatzindiz: k=2 ist ueber viele Typen identisch gross (66592 / 33312 / 99872), k=3 je Typ verschieden. Code: `@0x8001ab4c ori a0,s0,0x2` → laden → `@0x8001ab6c jal 0x80076a40` (VRAM-Upload) vs. `@0x8001ab7c ori a0,s0,0x3` → `@0x8001ab88/94/a0` (Verzeichnis-Parse).

**Endkampf-Records:**

| Typ | AI k=0 | AI k=1 | TIM k=2 | EMD k=3 |
|---|---|---|---|---|
| 0x36 | 3444 / 23476 | 3456 / 23476 | 3468 / 132640 | 3533 / 112340 |
| 0x37 | 3588 / 23024 | 3600 / 23024 | 3612 / 33312 | 3629 / 59376 |
| 0x2B | 2315 / 19816 | 2325 / 19816 | — | — |

Verifiziert durch Byte-Identitaet: `EMZ0.BIN` = Sektor 26 (Typ 0x10 k=1), `EMD0G_MOD0.BIN` = 1206 (0x20 k=1), `EMOVL21_S0.BIN` = 1320, `EMS25.BIN` = 1825, `EMS26.BIN` = 1901.

**[byte-belegt] Zweite TOC @0x8009B880** (fuer `CDEMD1.EMS`), Selektor `@0x8001b940 lbu v1,-1024(v1)` = `0x800CFC00`, CD-Datei-Ids 469 vs 470 (`@0x8001b970 beq v1,14 → @0x8001ba14 addiu v0,zero,470`). Alle 56 EMD-Records treffen gueltig, abgedeckter Bereich endet bei 0xaad0f0 = exakt 5467 Sektoren = 11.196.416 B = Dateigroesse. **Szenario B ist byte-true adressierbar** — die gemeldete "fehlende CDEMD1-TOC" existiert also.

**[byte-belegt] Modell Typ 0x36** (EMS @0x006E6800, dir_off 0x1b6b4, dir_count 8):
- Paar1: 11 Clips / 1360 Frames, Laengen `[100,180,150,100,180,150,100,50,50,150,150]`; EMR **nur 2 Bones**, kf_size 24, 1360 Keyframes (1 Keyframe/Frame). `parent[] = [-1,0]`.
- Paar3: 1 Clip / 150 Frames, 15 Bones (Opfer-Rig).
- MD1: nObj 14 = 7 Meshes, **878 Vertices**, 963 Faces (358 Tri + 605 Quad).
- TIM: **512×256**, 3 CLUTs (alle uebrigen Birkin-Formen 384×256).

⚠ WIDERLEGT: alle in der Voruntersuchung genannten Vertexzahlen waren **verdoppelt** — tri_vertex_count und quad_vertex_count zeigen in jedem Mesh auf **denselben** Block (identischer Offset UND Count). Korrekte Werte: 0x30 = 713, 0x31 = 684, 0x33 = 788, 0x34 = 1031, **0x36 = 878**, 0x37 = 78, RE1.5-0x30 = 599, RE1.5-0x36 = 275.

**[byte-belegt] RE1.5 hat eigene Typen 0x30 und 0x36** in `shared_assets/PSX/EMD/CDEMD0.EMS`: Blob 16 @0x00276800 (226.404 B, 16 Bones, 21 Clips, 599 V / 738 F) und Blob 17 @0x002ae000 (128.600 B, 16 Bones, 5 Clips, 275 V / 349 F). Beide mit **TIM IM EMD** (dir_count 9 statt 8) und identischem `parent[]`.

**[byte-belegt] Rig-Bruch:** RE1.5-0x30 `parent[] = [-1,0,1,2,0,4,5,0,7,8,8,10,11,8,13,14]` gegen RE2-0x30 `[-1,0,1,2,2,4,5,2,7,8,0,10,11,0,13,14]` — gleiche Bone-Zahl, **andere Hierarchie**. RE2-Keyframes posieren ein RE1.5-Mesh ohne Permutation nicht.

**[byte-belegt] DATENHYGIENE — die vorextrahierten Dateien sind unbrauchbar.** `info/re2leon/PL0/PLD/CDEMD0/EM0xx.*` sind ein sektorverschobener Naiv-Split: alle Groessen sind 2048-Vielfache, die echten Records nicht. `EM036.TIM` (133120 B) findet sich bei EMS-Offset 0x61B800 = Sektor **3127**, die TOC sagt **3468**. `EM036.EMD` (149504 B) liegt bei Sektor **3192**, TOC sagt **3533**, und enthaelt ab 0xB800 einen fremden Overlay-Chunk. **Wer aus EM036.* misst, misst nicht Typ 0x36.** `CDEMD1/EM059.EMD` (8.947.712 B) ist praktisch das ganze Archiv-Ende in einer Datei.

**[byte-belegt] Port-Datenlage:** `re15_port/shared_assets/RE2/CDEMD0.EMS` ist md5-identisch (`10137142e8d3f2f96c06afc00fd49bfe`) mit dem Original; die TOC ist als `re15_port/engine/src/gen/re2_ems_toc.inc` vendored und stimmt Zahl fuer Zahl; `re2_emd_parse_bank` prueft nur `dir_cnt==8` — fuer 0x30/0x31/0x33/0x34/0x36 erfuellt.

**Aber [byte-belegt]:** `re15_port/platform/pc/main.c:717-724` ruft unmittelbar nach `pc_enemy_load_re2()` **bedingungslos** `pc_enemy_hybrid_re15_models(type, eb)` und tauscht Mesh/Textur/Bind-Laengen gegen RE1.5 zurueck; der reine RE2-Modell-Zweig ist laut Kommentar "tot und weg". Fuer 0x31/0x33/0x34/0x3A gibt es ausserdem **kein** RE1.5-Gegenstueck (`s_ems_order`, `re15_ems.c:64-68`, kennt nur 0x30 und 0x36).

## 1.5 Sound des Endkampfs

**[byte-belegt] SE-Ids des Bosses.** Zensus `jal 0x8005BD6C` ueber den geschnittenen 0x36-Chunk, a0 rueckwaerts aufgeloest — **38 Aufrufe, Id-Menge {6,7,8,9,10,11,12,13,14}**:
```
0x80100AB8 a0=9   0x80100DEC a0=10  0x80100F80 a0=11  0x80101BB8 a0=6
0x80101BD0 a0=7   0x80102940 a0=8   0x80102B30 a0=12  0x801030F4 a0=13
0x801041A8 a0=14  (+29 weitere, davon 9 registerabhaengig)
```
Typ 0x37: 18 Aufrufe.

**[byte-belegt] Der SE-Pfad.** `FUN_8005bd6c(a0=SE-Id, a1=Entity)` ist die auf Bank 3 (ENEM) fest verdrahtete Fassung: EDT-Zeiger `0x800dbb84`, VH-Zeiger `0x800d75ac`, VAB-Id `0x800d4c4b`. Entity-Flagwort Bit 0x2000 addiert 16 auf die Id (`@0x8005bd90-a4`).

EDT-Record (4 B, Index = SE-Id): `b0&0x80` → VAB-Id-Override, `b1&0x7f` Programm, `b2>>4` Tone, `b2&0xf` Prio-Nibble, `b3&0x1f` SPU-Kanal, `b3>>5` Zusatz-Layer (`@0x8005bddc-0x8005be4c`).

**[byte-belegt] ENEMSE-TOC @0x800a7b1c**, 16 B je Bank = `[u32 EDT-Groesse][u24 Sektor][u8][u32 VBD-Groesse][u24 Sektor][u8]`. **Bank 25** (ROOM7040): `@0x800a7cac: a8 0e 00 00 e5 03 00 00 10 7a 01 00 e7 03 00 2a` → EDT @0x1F2800 (0xEA8), VBD @0x1F3800 (96784 B), nProg=2 nTone=19 nVAG=15, live Ids 0..15. An jedem berechneten EDT+0x80 steht `pBAV`. Bank 4 (ROOM7000) EDT@0x04F000, Bank 26 (ROOM7020) EDT@0x20B800.

**[abgeleitet] Die Bank-25-Zuordnung von ROOM7040 haelt nicht ganz.** Der Resolver `FUN_80052b38` vergleicht ein **Paar** aus `0x800d8cd0` (zwei Sound-Ids, gefuellt in `@0x80057274-0x800572b8`) gegen die 2-Byte-Paartabelle `0x800a7400` — und zwar in **beiden** Reihenfolgen (`@0x80052b7c-0x80052bcc`). Zeile 25 = `(27,00)` matcht nur, wenn Slot 1 leer bleibt. Ein Rohmuster-Scan von room7000/sub08 findet `@0x1e: 44 00 09 0a 1a 00 66 22 ...` mit Sound-Id 0x22; fuer das Paar (2e,22) existiert **keine** Tabellenzeile. Ohne echten SCD-Walk ist die Bank nicht hart.

**[byte-belegt] BGM.** Zwei getrennte Opcodes:
- `0x51` = `Sce_bgm_control`, 6 B, Handler `@0x80057e20` → `FUN_8005b6f0`, 6-Kommando-Sprungtabelle `@0x80011608` = `{0x8005b974, 0x8005b7b0, 0x8005b82c, 0x8005b898, 0x8005b910, 0x8005b948}`. Macht **nur** Play/Stop/Fade. **Drei** Abbruch-Gates, nicht zwei: zuerst `@0x8005b6f0 lw v0,0x800cfb74 / @0x8005b710 andi 0x2000 / @0x8005b714 bne → 0x8005b9d0` (globale Stummschaltung), dann die zwei SEQ-Handle-Gates. Es gibt **keine** Bank-Obergrenze — Bank-2-Aufrufe sind real (room7030/sub12 `@0x192`, `@0x1d6`: `51 02 02 00 00 00`, lueckenlos zwischen zwei 6-B-Nachbarn).
- `0x57` = die **eigentliche Track-Wahl**, 8 B, Handler `@0x80057e98` → `FUN_8005b9f4`: schreibt `{mainId, subId}` in das RAM-Array `0x800d46d0 + stage_basis*2 + room*2`.

Leser: `FUN_8005a444` (gerufen von `FUN_80049e48 @0x80049ebc`), `@0x8005a474-0x8005a4bc`. Stage-Basis-Tabelle: Schreiber nutzt `0x800A80AF` (1-basiert), Leser `0x800A80B0` (0-basiert) — beide landen fuer STAGE7 auf `0x800a80b6` = **134**. Beide Seiten mit `andi 0x3f` maskiert (`@0x8005a4ec/f0`).

**Die acht 0x57-Zuweisungen mit stage=7 (exhaustiv):**
```
room7020/sub16 @0x036: 57 00 02 07 11 2f 00 80  -> Raum 02: MAIN 0x11, SUB 0x2F
room7020/sub16          Raum 03: MAIN 0x23, SUB 0x2C
room7030/sub00 @0x014: 57 00 01 07 23 2c 40 80  -> Raeume 00,01,02,04: MAIN 0x23, SUB 0x2C
room7030/sub02 @0x076: 57 00 03 07 23 ff 00 00  -> Raum 03: MAIN 0x23, SUB 0xFF (kein SUB)
room7030/sub02          Raum 04: MAIN 0x23, SUB 0x30
```
**Endkampf-BGM = MAIN-Id 0x23, Layer SUB 0x2C / 0x2F / 0x30; ROOM7020-Vorstufe MAIN 0x11.**

⚠ [abgeleitet], nicht byte-belegt: die Gleichsetzung MAIN-Id 0x23 = `MAIN23.BGM`. Der Code laedt einen **CD-Index** aus `0x800a8168` (0x40 u16-Eintraege): Id 0x23 → Index **389**. `0x800a81e8` fuer SUB (413..468). Die Zuordnung Index→Dateiname folgt nur aus der Sequenz.

**[byte-belegt] Prioritaets-Gate.** RE2 `FUN_8005c92c` vs RE1.5 `FUN_80045a18` sind instruktionsgleich bis auf: RE2 verdoppelt den Kanal-Index (`sll a0,a0,1`) und liest `0x800d4ca0` (Stride 2), RE1.5 liest `0x800b22cc` (Stride 1). ⚠ Der behauptete Gleichstand-Unterschied ist im tatsaechlichen Wertebereich **kein** Verhaltensunterschied: der Prio-Parameter ist `b2&0xf` = 0..15 (`@0x8005be00 andi s5,v1,0xf`), und dort ist RE2s `andi 0x8 / sltu` bitgleich zu RE1.5s `andi 0xff / sltiu 0x8 / xori 1`. Divergenz erst ab a1≥0x10, was der Aufrufpfad ausschliesst. **Kein Port-Fix noetig.**

**[byte-belegt] Raum-Baenke liegen im RDT, nicht in COMMON/SOUND.** RDT-Kopf: `+0x08` EDT, `+0x0C` VH, `+0x10` VB. Alle fuenf ROOM70x0 fuehren **genau eine** Bank (Bank-1-Felder = 0). ROOM7040: 3616 B VH / 72320 B VB, nProg=2 nTone=24 nVAG=17; Summenprobe der VAG-Groessentabelle == VB-Groesse in allen fuenf. SE-Ids 26..31 sind in allen fuenf bytegleich (`00 00 43/53/63/13/23/33 16`).

**[byte-belegt] 980 ENEMSE-WAVs liegen bereits extrahiert** in `info/re2leon/COMMON/SOUND/` (`ENEMSE_00000.wav` ff., von 1954 wav gesamt). Mit den SE-Ids {6..14} und Bank 25 ist die Kette **Boss-Zustand → EDT-Record → VAG → hoerbares Sample** vollstaendig herstellbar, ohne neue Daten.

## 1.6 Was am Endkampf noch offen ist

**[offen]** Die konkreten Zustandsfunktionen hinter `0x801055CC[0..4,7]`: Uebergaenge, Abstands-`sltiu`-Schwellen gegen +0x1F0, Clip-Indizes (Anim-Kopplung), Schadensfenster, Phasenwechsel (die HP-Stores `0x80103004`/`0x80103060`). Die Bytes liegen vor, die Arbeit ist noch nicht gemacht.

**[offen]** Die raumseitige Inszenierung (wer schaltet das 0x8000-Schlafbit ab, wer setzt die Kampfposition) — das steht im STAGE7-Code-Overlay ab ~0x8011A000, das nicht im Repo ist.

---

# 2. Menue-Sounds und -Effekte: was RE2 hat und RE1.5/der Port nicht

## 2.1 Der Einstieg in beiden Spielen

**[byte-belegt]** `Se_on` = RE1.5 `FUN_80045024` / RE2 `FUN_8005ba28`, gleiche gepackte Argumentform `a0 = (bank<<24)|(rec<<16)|low`, `a1` = Positionszeiger oder 0.

**[byte-belegt]** RE1.5: Bank 4 = CORE ist hart belegt — Sprungtabelle `@0x80010e70[4] = 0x8004511c`, dort `@0x80045124 lui a0,0x801f / @0x8004512c ori a0,a0,0xbd00` → fester Puffer `0x801fbd00`; Lader `FUN_800440c4 @0x80044130 jal 0x80013b60` mit a3 = `0x80010e28`, und `bytes 0x80010e28` = `43 4f 52 45 20 45 44 48` = **"CORE EDH"**.

⚠ **[offen] fuer RE2:** "Bank 4 = CORE" ist dort **nicht** belegt. Der Bank-Index laeuft ueber `0x800d4c48` (`@0x8005ba64 lb v0,19528(at)`) und `0x800dbb78` (`@0x8005ba8c lw a1,-17544(at)`) — **beide im statischen EXE-Image komplett 0x00** (BSS). Es fehlt die Ladestelle.

## 2.2 Zensus — die harte Zahl

| | RE1.5 | RE2 |
|---|---|---|
| EXE gesamt (`jal Se_on`) | 41 | 138 |
| davon Bank 4 | 8 | 56 |
| Statusschirm/Inventar | 7 (EXE) + 22 (DEBUG.BIN) | **53** (49 Bank 4 + 4 Bank 2) |
| Titel-Overlay | 4 | 47 |
| CONFIG-Overlay | **0** (existiert nicht) | 34 |
| MEM_CARD-Overlay | **0** (existiert nicht) | 24 |
| SELECT / DIEDEMO / RESULT / OPENING / ENDING | alle nicht vorhanden | 4 / 1 / 0 / 0 / 2 |

⚠ WIDERLEGT: "56 Stellen im Statusschirm (51 Bank 4 + 5 Bank 2)". Gemessen im Bereich 0x8006A000–0x80073000: **53** = `(4,4)×11, (4,5)×18, (4,6)×10, (4,8)×4, (4,9)×3, (4,10)×3, (2,20)×3, (2,43)×1`. Die 56 ist die Zahl **aller** Bank-4-Aufrufe der ganzen EXE; 7 davon liegen ausserhalb (`0x80026530`, Message-Box `0x80030954/6c/a0`, Gameplay `0x8003f6ec/0x8003fa88/0x8003fd00`).

⚠ Weitere Zaehlkorrekturen: CONFIG.BIN = `{(4,4):22, (4,5):7, (4,6):5}`; MEM_CARD.BIN = `{(2,34):1, (2,36):1, (4,4):5, (4,5):8, (4,6):9}`; RE2-TITLE.BIN = `{(4,0):3, (4,4):22, (4,5):10, (4,6):12}`.

## 2.3 Wo RE1.5 komplett stumm ist

**[byte-belegt]** und zwar haerter belegbar als per Bereichsscan: im gesamten RE1.5-EXE-Text gibt es **zwischen 0x80018e40 und 0x8002c70b** sowie **zwischen 0x8002c980 und 0x8003338b kein einziges `jal 0x80045024`**, und das Datenwort `0x80045024` kommt nirgends vor — es existiert also auch kein indirekter Pfad.

Damit sind stumm:
- **OPTIONS/Controller-Config** (Task `FUN_8002dde4`, Handler-Tabelle `@0x80073d9c`) — 0 SE. RE2 CONFIG.BIN: 34.
- **Memory-Card/Save-Screen** (`FUN_80025c00`-Kette) — 0 SE. RE2 MEM_CARD.BIN: 24.
- **YES/NO-Box im Message-System** (`FUN_80028134`, 420 Instruktionen, jal-Ziele `0x800283f4, 0x800284f0, 0x800284f8, 0x800285a8, 0x80028654, 0x8002867c, 0x80028750`) — 0 SE. RE2 `FUN_80030844`: rec4/5/6.
- **Combine/EXCHANGE** (`0x8004b37c`, `0x8004b408`) — 0 SE. RE2 `FUN_8006b358`: `@0x8006b584` rec4, `@0x8006b5bc` rec6/7, `@0x8006b5d4` rec5.
- **Game-Over** (`FUN_8001500c`, Tabelle `@0x80071d10`) — 0 SE. RE2 DIEDEMO.BIN: 1 (Bank 4, Record 0, `@0x80192a64`).
- **Item-Get-Modal** (`FUN_8001db28`) — 0 SE.

**Das sind ganze SCHIRME, nicht einzelne Toene.**

## 2.4 Welche Samples RE1.5 wirklich fehlen

⚠ **WIDERLEGT — wichtigster Fehlbefund des ganzen Strangs:** "RE1.5 spielt CORE-Record 7 (Fehler-Ton) nirgends." **Falsch.** RE1.5 spielt ihn an vier Stellen:
```
STAGE3.BIN @0x8011aa00 lui a0,0x407 / @0x8011aa04 ori a0,a0,0x1 -> @0x8011aa40 jal 0x80045024
STAGE3.BIN @0x8011acc4 ... -> @0x8011accc jal
STAGE5.BIN @0x8011b214 ... -> @0x8011b254 jal
STAGE5.BIN @0x8011b4d8 ... -> @0x8011b4e0 jal
```
(Byte-Scan nach `0x3C040407`: STAGE3 @0x1aa00/0x1acc4, STAGE5 @0x1b214/0x1b4d8.) Damit ist auch die Trennlinie "Records 0..3 = Gameplay, 4..10 = UI" hinfaellig — rec 7 wird in RE1.5 **ausschliesslich** im Stage-Overlay-Code benutzt. Die tatsaechliche RE1.5-Record-Menge ueber EXE+DEBUG+TITLE+STAGE1-6 ist `{0..10}`.

⚠ **WIDERLEGT:** "Die Menue-Bank ist in beiden Spielen dieselbe Datei." Belegt war nur ein Zwei-Datei-Vergleich. Von 11 gemeinsamen CORE-Baenken sind **4 identisch (CORE00, CORE0F, CORE12, CORE13), 7 unterschiedlich** — und bei CORE10/CORE11 ist der Inhalt zwischen den Spielen **VERTAUSCHT** (RE1.5 CORE10 == RE2 CORE11 und umgekehrt). Gerade CORE10..13 haben das menuetypische Record-Profil. Ausserdem ist die geladene Bank-4-Datei nicht konstant CORE00: RE1.5 waehlt per ID-Tabelle `@0x80073a88`.

**[byte-belegt] Was RE1.5 wirklich als Sample fehlt:**
1. **CORE-Record 16.** Alle RE1.5-CORE-Baenke (CORE00..CORE13) haben `nrec=16` → Record 16 existiert dort nirgends. RE2 hat zusaetzlich CORE14/CORE15, und `CORE15.EDH` hat `pbav=0xC0` → `nrec=48` mit gueltigen Records `{4,6,10,11,12,16,34,35,36}`. Benutzt von `SELECT.BIN @0x02adc` (a0 = `0x04100000`) — dem Szenario-Auswahlschirm, den RE1.5 gar nicht hat.
2. **Die Bank-2-Menuetoene.** RE2s Statusschirm spielt Record 20 dreimal (`@0x8006FEFC, @0x80070044, @0x800701D4`) und Record 43 einmal (`@0x8006F238`); MEM_CARD.BIN spielt Record 34 und 36 (`@0x02780`, `@0x02794`); ENDING.BIN zwei weitere. **RE1.5 hat in seinem gesamten Menue-/Inventar-/Card-Pfad NULL Bank-2-Aufrufe** — alle sechs RE1.5-Bank-2-Stellen (`0x800180E0, 0x8002C70C, 0x8002C97C, 0x80035A1C, 0x80052AD8, 0x80052BCC`) liegen im Gameplay-Code. Das ist eine geschlossene, quantifizierte Luecke. **[offen]**: welche Datei Bank 2 in RE2 traegt.

## 2.5 Verhaltensunterschied bei gleichem Ton

**[byte-belegt] Cursor-Ton rec 4: RE1.5 piept unbedingt, RE2 nur bei echter Bewegung.**
- RE1.5 `@0x8004a464 andi v0,v1,0x2000` → `@0x8004a474 sb` → `@0x8004a478 lui a0,0x404` → `@0x8004a47c jal` — der Ton haengt **nur** am Pad-Bit. Im Item-Gitter (`DEBUG.BIN @0x800c62c8-dc`) laeuft die Grenzpruefung (`@0x800c63e8-f4`) erst **danach**.
- RE2 `@0x8006ab70 lbu v0,12(s0)` (Cursor nachher) / `beq a2,v0,...` (a2 = Cursor vorher, gelesen `@0x8006a9f4`) → `@0x8006ab7c lui a0,0x404` → `@0x8006ab80 jal`. Ebenso Kommandoleiste `@0x8006a884-94`.

Das ist echter, byte-belegter Verhaltensunterschied — aber **kein Port-Bug**: RE1.5s Version ist die richtige fuer den Port.

## 2.6 Effekte

**[byte-belegt] RE1.5 hat sehr wohl Menue-Animationen** — der Eindruck "RE1.5 fehlen die Effekte" ist fuer die Slides falsch:
- Inventar-Slide ±14/Frame (`LAB_8004a6e0` / `LAB_8004a440`).
- MAP-Tab 25 Frames: Gate `@0x8004c0bc sltiu 0x19`; list_x +15 `@0x8004c0e0`, ecg_y +9 `@0x8004c0f4`, cond_x −9 `@0x8004c108`, equip_y −7 `@0x8004c11c`.
- FILE-Tab 30 Frames: `@0x800c6cdc sltiu 0x1e`, Deltas `@0x800c6d08-60`.
- Combine-Puls 17 Frames: Gate `@0x8004b414 sltiu 0x11`, Tabelle `@0x80010ff4`, Handler `@0x8004b468-74` (±1 auf 0x800b25d0..d3).
- Cursor-Blinken: `@0x800284c8 ori v0,zero,0x18 / sllv v0,v0,s1 / and / beq` — verschiebbare Maske. RE2 nutzt zwei feste Masken (`@0x800309e0 andi 0x30` / `@0x800309f4 andi 0x18`).

**[byte-belegt] Was RE1.5 wirklich fehlt: die 3D-Item-Drehung beim Untersuchen.**
RE2 Zustaende 6/7 der Tabelle `@0x80011c30`:
```
8006cd18  lbu v0,3(s2)        ; Frame
8006cd20  addiu v1,v0,1
8006cd24  sltiu v0,v0,0xb     ; 11 Inkremente (0..10)
8006cd28  bne v0,zero,0x8006cd40
8006cd40  sll v0,v0,2 / lhu v1,46(s2) / addiu v0,v0,20 / addu / sh v1,46(s2)
```
→ Winkel (`0x800d5c1e`) `+= (Frame+1)*4 + 20`, **11** Schritte (nicht 12 — beim 12. Tick springt es nach `@0x8006cd30` in Zustand 8). Zustand 7 identisch mit `subu` `@0x8006cd94`.

RE1.5s CHECK (`DEBUG.BIN FUN_800c6630`, Sprungtabelle `@0x800c6864`) hat **kein Winkelregister**: Zustand 1 `@0x800c6704-671c` schiebt ein flaches TIM-Foto mit +22 px/Frame von −207 bis 13, Zustand 3 mit −22 zurueck.

⚠ WIDERLEGT: "rec 10 ist der Drehton". Die rec-10-Stellen `@0x8006cb24` / `@0x8006cb48` setzen `sb v0,2(s2)` auf **4** bzw. **5**; Tabelle `@0x80011c30[4] = [5] = 0x8006cbcc`, und dieser Handler fasst `46(s2)` nie an. Der Winkel liegt in `[6]`/`[7]` — und dort wird kein rec10 gespielt.

**[abgeleitet] Kein Screen-Shake** in beiden Menues: alle Positions-Deltas sind monotone Slides bzw. der 17-Frame-Puls; kein Zufalls-/Oszillator-Offset gefunden.

## 2.7 Port-Stand

**[byte-belegt] Der Port hat zwei RE1.5-Toene NICHT:**
1. CORE rec 6 im Titel-Zustand 3 — RE1.5 `@0x80102ac4 lui a0,0x406` / `@0x80102ac8 jal 0x80045024`. Der Port hat im Titelpfad nur `core_se(4)` (`main.c:2385`) und `core_se(0)` (`main.c:2400`).
2. CORE rec 9 `@0x8004a154-58` (Debug-Item-Werkzeug: SELECT im Inventar setzt Item-Typ mit Menge 255). Kein `se4(9)`/`core_se(9)` im ganzen Port.

**[byte-belegt] Der Port hat drei Toene ZUVIEL:** `main.c:1167-1169` spielt `core_se(4)/(6)/(5)` auf dem Save-Screen, wo RE1.5 stumm ist — im Code als bewusste RE2-Uebernahme dokumentiert (`main.c:1154-1161`). Ebenso `menu_common.c:1106/1109/1119` (Combine-Triade).

⚠ Praezisierung: Die Behauptung "`RE2DOG_ENEMSE_BANK=6` ist die einzige Bank-Quelle im Port" ist falsch — es gibt **vier**: `enemy_ai_re2_crow.c:109` (7), `enemy_ai_re2_dog.c:130` (6), `enemy_ai_re2_spider.c:133/134` (11), `enemy_ai_re2_zombie.c:731` (0, mit env-Override); vier Hooks `main.c:697/703/710/716`. Zusaetzlich uebersehen: weitere `bse(4/5/6)`-Stellen in `re15_itembox.c`.

---

# 3. Tuer-Sequenz als Level-Uebergang in RE2

## 3.1 Gleiche Architektur, unterschiedlicher Ausbau

**[byte-belegt]** Beide Spiele: ein DO2-Archiv (Modell + TIM + eigenes Skript + eigene VAB) wird nach `0x801a1000` geladen; die Tuer-Demo laeuft als **Koroutine auf Task-Slot 1**, waehrend der Hauptthread den Zielraum von CD nachlaedt; Synchronisation ueber **dasselbe Bit 0x2000000**.

Koroutinen-Installer RE2 `FUN_80031f6c @0x80031f6c-90`: `sll a0,a0,7 / sw a1,30376(at) / sh 2,30372(at)`. RE1.5: `FUN_80029a98` / Yield `FUN_80029ac8`.

**Init/Main/Exit-Tripel**, NULL-terminierte Funktionsliste:
- RE2 `@0x8009a7b4` = `{0x80013c1c, 0x80013eb4, 0x8001417c}`, abgearbeitet von `FUN_80013bc4 @0x80013bc4-c04`.
- RE1.5 `@0x80071d30` = `{0x800161e0, 0x800164c8, 0x80016664}`, abgearbeitet von `FUN_80016188 @0x8001618c-c4`.

**[byte-belegt] Die Skript-VM der Tuer-Demo ist dieselbe wie die Raum-SCD-VM** — Dispatch ueber `PTR_LAB_800a74c8` (RE2) bzw. `PTR_LAB_800744a8` (RE1.5), Objekt-Slots 10..13 (Stride 0x174 ab 0x800d86e8 bzw. 0x170 ab 0x800b39ac). Kein Sonder-Interpreter.

## 3.2 Der entscheidende Unterschied: 55 gegen 1

**[byte-belegt]** RE2 hat **55** DO2-Archive (DOOR00..DOOR36). Datei-Index-Tabelle `@0x8009a4b0` (u16, 0x00EA..0x0120); Chunk-Tabelle `@0x8009a520`, **12 Byte je Eintrag**: `+0x00 u16` Groesse Sound-Chunk, `+0x02 u16` Groesse Textur/Modell-Chunk, `+0x04 u32` Sektor-Offset des 2. Chunks, `+0x08/+0x09` je ein Byte nach `DAT_800d531e`.

Verifiziert durch Arithmetik ueber alle 55: DOOR00 `0x4b08` + 10 Sektoren `0x5000` + `0x8dd0` = `0xDDD0` = 56784 = exakte Dateigroesse. DOOR01: `0x5d78` + 12 Sektoren + `0xba70` = `0x11A70` = 72304. Gegenprobe: `0x8009a520 + 55*12 = 0x8009a7b4` = exakt die Koroutinen-Zeigertabelle.

⚠ Praezisierung: bei `0x8009a51e` steht ein **56.** u16 = `0x00EA`. Und `+0x08/+0x09` sind **keine "Pruefbytes"** — `DAT_800d531e` ist eine globale CD-Anforderungs-Zelle mit 20 Schreib-Xrefs, deren Default aus `0x800988ab` kommt (`FUN_80012fb8 @0x8001300c`). Der Vergleich `@0x800131b8` prueft etwas Ungenanntes. [abgeleitet]

**[byte-belegt]** RE1.5 hat **genau eine**: `info/Re1.5/PSX/DOOR/DOOR00.DO2` (57016 B). Die dtex-Tabelle `@0x80071d2c` hat nur **einen** gueltigen Eintrag: `25 00 00 00 | e0 61 01 80 | c8 64 01 80 ...` — ab `0x80071d30` stehen bereits die **drei Funktionszeiger der Tuer-Demo**. Ein `dtex != 0` wuerde in RE1.5 einen Funktionszeiger als CD-Datei-Index lesen. Zugriff: `FUN_800171f4 @0x80017214-24: sll v0,v0,0x1 / addiu at,at,0x1d2c / addu / lhu a0,0(at)`.

**[byte-belegt] DOOR00 ist in beiden Spielen dasselbe Asset**: `RE1.5[0x18:0x798] == RE2[0x5224:0x59A4]` (MD1) und `RE1.5[0x9A8:0x8BC8] == RE2[0x5BB0:0x5000+0x8DD0]` (TIM, 33312 B), je 0 abweichende Bytes.

**[byte-belegt] RE1.5s DOOR00-Skript ist LEER.** SCD-Bereich = 4 Byte bei Datei `@0x9A4`: `02 00 01 00` = 1-Eintrag-Offsettabelle + Opcode `0x01` = END. Der END-Handler `PTR_LAB_800744a8[1] = 0x8003f1f0` deaktiviert das Objekt (`@0x8003f1f0-0x8003f20c: lb v1,2(a1) / bne / sb zero,1(a1) / ori v0,zero,0x2`) — baugleich zu RE2 `@0x800537fc`. **RE1.5s Tuer-Demo laeuft genau EIN Bild.**

## 3.3 Die Choreografie liegt bereits decompiliert im Repo

**[byte-belegt]** `info/re2leon/COMMON/DOOR/DOOR*/` — **55 Unterverzeichnisse mit 498 `.c`-Routinen**, dazu je Tuer `.md1`, `.tim`, `.OBJ`, `.vh`, `.vb`, `.bmp`, `.ini`. Plus 110 bereits entpackte `.bmp` direkt in `COMMON/DOOR/`. **Dieses Verzeichnis war bisher nie geoeffnet.**

`DOOR00/DOOR0001.c` (die Tuer-Hauptbewegung), woertlich:
```c
Obj_model_move(0,0,0,1,0,40970,4096,-12281,-12786,8,0,0,0);
Obj_model_move(1,0,226,1,1,53248,4096,-32256,26867,-11022,0,0,0);
Sce_fade_set(0,2,7,0,254);
Sce_fade_adjust(0,0,28);
Sleep(17920);              // 0x4600 byte-vertauscht = 70 Bilder
goto Sub08();
Se_on(0,0,0,0,0,0,0,0);
Evt_exec(11,0x1803);
Evt_exec(12,0x1804);
Sleep(12800);              // = 50
Evt_exec(11,0x1805);
Sleep(15360);              // = 60
goto Sub06();
```
`DOOR00/DOOR0000.c`: `switch(12){ case 0: goto Sub01(); case 256: goto Sub02(); }`

⚠ **WIDERLEGT (beide Seiten):** Die Bytefolge `04 0b 18 03` ist **kein Gosub**, sondern `Evt_exec(11, 0x1803)`. Es gibt in DOOR0001 genau **ein** Gosub (Sub08) plus ein abschliessendes Sub06. Damit sind auch die Summen "180 Bilder"/"230 Bilder" **[abgeleitet]**, nicht byte-belegt — Sub05 traegt einen eigenen `09 0a 12 00` (18 Bilder), und die aufgerufenen Routinen wurden nie auf Yields geprueft.

**[byte-belegt] Der `switch(12)` ist der Tuer-Typ-Switch.** Opcode `0x13` Handler `@0x80054020` ist ein Switch auf `*(s16*)(0x800d47ec + arg*2)`; mit `arg=0x0c` → `0x800D4804` = Payload `+0x0D & 0x7F`. Gesetzt in `FUN_80013c1c`: `DAT_800d4804 = payload[0x0D] & 0xff7f; DAT_800d4808 = payload[0x0D] & 0x80`.

**[byte-belegt] Schlaf-Mechanik**: Opcode `0x09` `@0x800539dc` speichert den Zaehler (`lhu a0,2(a2) / sh a0,0(v0)`) und setzt PC = a2+1, sodass der naechste Dispatch Opcode `0x0A` `@0x80053a24` trifft (`lhu / addiu -1 / sh / bne`).

Routinenzahl je Tuer variiert: DOOR00=9, DOOR0E=4, DOOR25=7, DOOR28=6, DOOR2A=11, DOOR32=2, DOOR34=2, DOOR36=2.

## 3.4 Einstieg und Nutzlast

**[byte-belegt]** Beide nutzen den AOT-Scan mit derselben Vorwaerts-Reichweite `0x26C` = 620 (RE1.5 `FUN_80042bac @0x80042bd0 ori v0,zero,0x26c`; RE2 `FUN_80051088`). Aber **unterschiedlicher SCE-Index**:
- RE1.5 = **Index 2**, Tabelle `@0x8007469c`, Handler `@0x800430bc` — **13 Instruktionen, KEINE einzige Verzweigung**: `sw a0,DAT_800ac9a8 / sb v0,DAT_800b5359 / lw / lui v1,0xff00 / or / sw / jr ra`.
- RE2 = **Index 1**, Tabelle `@0x800a73c4`, Handler `@0x80051514` — vollstaendige Schloss-/Schluessel-Logik.

**RE2-Schloss-Logik [byte-belegt]:**
```
@0x80051514  Abbruch wenn DAT_800cfddc != 0
@0x8005156x  lbu a1,15(s2) / andi 0x80 -> "hat Schloss"; a1&0x3f = Flag-Index
             jal 0x80077360 (Flag-Test auf 0x800cc1e8+0x884c) -> gesetzt: oeffnet sofort
@0x800515xx  lbu s0,16(s2) = Schluessel-Art:
               0xFE -> Meldung 10 (jal 0x8002fe38) + Se_on 0x226 + Flag setzen (jal 0x8007730c)
               0xFF -> Se_on 0x216 + Meldung 11 (bleibt zu)
               sonst -> jal 0x800696cc (Item-Suche)
                         Treffer:    Meldung 5, DAT_800e873f = id, Se_on 0x225,
                                     Rueckruf 0x80051718, Flag setzen
                         Fehlschlag: Se_on 0x216 + Meldung (id-76)
```

**[byte-belegt] Nutzlast-Offsets, gleich in beiden Spielen:**
`+0x00/02/04` u16 Spawn X/Y/Z · `+0x06` u16 Gierwinkel · `+0x08` Ziel-Stage · `+0x09` Ziel-Raum · `+0x0A` Ziel-Kameraschnitt · `+0x0B` Etage (→ `× -0x708` in Weltkoordinaten, beide Spiele) · `+0x0C` **dtex** · `+0x0D` Tuer-Typ (Bit 7 gesondert). RE2 zusaetzlich `+0x0F` Schloss-Flag, `+0x10` Schluessel-Art.

**[byte-belegt] Raum-ID-Kodierung unterscheidet sich.** RE2 `@0x80026d44-68`: `addiu v1,a2,1 / sll v1,v1,8 / lhu v0,0x800d481e / addu / sh v0,0x800d4822` = `(stage+1)*0x100 + room`, Stage vorher `% 9`.

⚠ WIDERLEGT fuer RE1.5: Es gibt dort im Tuerpfad **gar keine kombinierte ID**. `FUN_8001d600` legt drei getrennte Halbworte ab: `@0x8001d95c sh v0,4066(at)` = 0x800b0fe2 (Raum), `@0x8001d948 sh v1,4068(at)` = 0x800b0fe4 (Cut), `@0x8001d97c sh v0,4064(at)` = 0x800b0fe0 (Stage). Keine Schiebe-/Oder-Kombination im ganzen Zweig.

## 3.5 dtex-Zensus — die Zuordnung Tuer → Raum

**[byte-belegt]** Scan aller 250 `ROOM*.RDT` auf Opcode `0x3B` (32 B), validiert mit `+22<9, +24<32, +26<=0x36, +31==0`: **610 Tuer-Records.** Feldlage bestaetigt sich als `+22` Stage, `+23` Raum, `+24` Cut, `+25` Etage, `+26` dtex, `+27` door_type, `+28` knock, `+29` keyid, `+30` key_type.

**Alle 55 Tuer-Archive sind belegt ausser dtex 0x21 (0 Treffer).** Verteilung:
```
00:60 01:8  02:6  03:18 04:5  05:14 06:16 07:33 08:39 09:13 0a:19 0b:4  0c:3  0d:38
0e:9  0f:5  10:8  11:4  12:5  13:4  14:4  15:4  16:51 17:19 18:8  19:7  1a:23 1b:4
1c:12 1d:12 1e:3  1f:6  20:2  21:0  22:4  23:4  24:5  25:19 26:26 27:10 28:17 29:14
2a:8  2b:4  2c:6  2d:2  2e:3  2f:4  30:8  31:4  32:1  33:2  34:1  35:1  36:1
```
Damit bekommen die Sonderfaelle ihren Raum: **dtex 0x32** (Text "Please wait a minute") existiert **genau einmal, in ROOM2080**; **dtex 0x34** ("The side pack is being equipped") **genau einmal, in ROOM2140**; der VRAM-Sonderfall **dtex 0x28** hat 17 Vorkommen. Die Endkampf-Kette nutzt **dtex 0x2A** (8×: ROOM7000/7030/7040/G000/G040) und **0x2E** (3×: ROOM7000/7010/G010).

**[abgeleitet] Umfang der Schloss-Mechanik:** 26 Tueren mit `key_type = 0x80` (Item-Schluessel), 15 mit `keyid = 254` (der 0xFE-Zweig), 2 mit `key_type = 1`, 2 mit `(86, 0x80)` — **rund 45 von 610 Tueren sind ueberhaupt verschlossen**. `door_type`: 0×236, 1×169, 4×45, 5×45, 0x78×32, 2×27, 3×22, 0xF0×15. ⚠ Warnung zur Methode: mustergestuetzt, kein vollstaendiger SCD-Walk. Die dtex-Hauptverteilung ist robust, die Randspalten (vereinzelte door_type 44/45/120/240, knock 57/59/102/116) sind vermutlich Fehltreffer in Datenregionen.

## 3.6 Ton und Ladepause

**[byte-belegt] Der Tuer-Ton kommt aus der VAB IM Archiv, nicht aus einer globalen Bank.** `FUN_80014cd0` oeffnet sie nach SPU 0x3dc50 (`SsVabOpenHeadSticky`) und legt die VAB-Id in `DAT_800d4c48` = **Bank-Slot 0**. Der Skript-Opcode `0x36` (Handler `@0x80056428`, 12 B) baut a0: `@0x80056518-34 sll v1,a3,24 / andi v0,a0,0xff / sll v0,v0,16 / or / srl a0,a0,8 / jal 0x8005ba28 / or a0,v1,a0`.

⚠ Praezisierung: das u16 bei `+2` ist **nicht** "Programm" — nur das Low-Byte wird zum Programm (Bits 16-23), das High-Byte fliesst in die unteren 8 Bit von a0.

**[byte-belegt] Die klangliche Tuer-Typ-Unterscheidung ist DATEN, kein Code.** Jedes der 55 Archive bringt **eigene 2-3 VAG-Samples** mit; die Auswahl geschieht allein ueber `dtex` (Byte +0x0C), das Archiv und Ton in einem Zug bestimmt. Verteilung (VabHdr `ps/ts/vs` bei Datei-Offset 0x22/0x24/0x26 ueber alle 55): **41× ts=3/vs=3, 12× ts=2/vs=2, 2× ts=3/vs=2** (DOOR25, DOOR2A). Alle 55 haben `ps=1`.

**[byte-belegt] Schliess-Ton am Ende**, positioniert: `FUN_8001417c @0x800141cc-f4`: `lhu v0,584(v0) / beq v0,zero / addiu a1,a1,-976` (= 0x800cfc30, die Ziel-Spawn-Position) `/ jal 0x8005ba28 / lui a0,0x1` = Bank 0, Programm 1.

**[byte-belegt] Wer wartet auf wen.** `FUN_80026b7c`:
1. `DAT_800cfb74 |= 0x2000000`
2. `FUN_80031f6c(1, &LAB_80013bc4)` — Demo starten
3. `FUN_80031f94(1)` — ein Bild yield
4. `FUN_80014cd0()` — DOOR SOUND laden
5. `while (DAT_800cfbd8 & 0x20000) FUN_80031f94(1)` — auf Sound-Uebertragung warten
6. `FUN_8005a714 / FUN_8001633c / FUN_8001645c / FUN_80049e48` — **Zielraum laden**
7. `while (DAT_800cfb74 & 0x2000000) FUN_80031f94(1)` — auf Demo-Ende warten

**Bei schnellem Laden gewinnt die Animation** (Schritt 7 blockiert). **Bei langsamem Laden haelt die Demo an**: `FUN_80013eb4 @0x80013f54-68` setzt `DAT_800d4806` erst auf 0, wenn Bit 0x20000 geloescht ist. Bit 0x2000000 gesetzt in `FUN_80013c1c`, geloescht in `FUN_8001417c @0x80014218` (`and v0,v0,0xfdffffff`). RE1.5 `FUN_8001d600` ist der direkte Zwilling — mit **`0x800aca38`** statt `0x800cfb74`, und **ohne** DOOR-SOUND-Lader.

**[byte-belegt] Textkaschierung** fuer dtex 0x32/0x34, in **jedem** Bild des Fensters 41 ≤ Zaehler ≤ 259 (219 Bilder): `@0x80013f88 lhu v0,558(v0) / addiu v0,v0,-41 / sltiu v0,v0,0xdb`. Textbytes ab `0x8009a42c`, Offsets `@0x8009a468/6a/6c` = 0x0000/0x0017/0x002F. Zeichensatz aus den Bytes: `a=0x3D, e=0x41, i=0x45, l=0x48, m=0x49, n=0x4A, s=0x4F, t=0x50, u=0x51, w=0x53`, `A=0x1D`, `0x00` Wortluecke, `0xFE` Zeilenumbruch, `0x01` Ende.

**[byte-belegt] VRAM-Sonderfall dtex 0x28**: `FUN_80013c1c` uebergibt `(0,0)` statt `(0x15,0x1f)` an `FUN_80076b60`.

**[byte-belegt] Uebergeordnete FSM**: `DAT_800df348` (RE2), 7 Zustaende, Sprungtabelle `@0x800109c4` = `{0x80025970, 0x80025aa0, 0x80025c5c, 0x80025d3c, 0x80025d64, 0x80025d8c, 0x80025dc4}`, Dispatch `@0x8002594c-68`; Zustand 1 ruft `FUN_80026b7c @0x80025a70`. Waehrend `!= 0` ist Pausieren gesperrt.

**[byte-belegt] Blenden**: RE2s Exit blendet aus (`@0x8001417c`: `jal 0x8002c1a0(0,0,7,1)` + `jal 0x8002c2b0(0,0x7fff,0x00ffffff,0)`, `DAT_800dfc1a = 2`). RE1.5s Exit `FUN_80016664` (22 Instruktionen, jal-Ziele nur `0x8002178c`, `0x80029ac8`, `0x80029afc`) macht das **nicht** — die Blenden liegen dort ausserhalb der Demo.

**[byte-belegt] Kamera ist in beiden statisch.** RE1.5 `@0x8001645c-98`: `sw 0x7530,0(v1)` und `sw 0x55f0,...` (30000 / 22000), dann `jal 0x80053ca4`. RE2 `FUN_80013c1c @0x80013c2c addiu a1,a1,2092` (= 0x8001082c) laedt 8 Woerter, ruft `@0x80013e38 FUN_80076cb0(sp+20, sp+32)` — Vektoren ab 0x80010830 = (10000,0,0) und (0,0,0). ⚠ Die Deutung "Auge/Ziel, Abstand 8000/10000" ist **[abgeleitet]**: weder `FUN_80053ca4` noch `FUN_80076cb0` wurden disassembliert.

## 3.7 Port-Stand

**[byte-belegt] Der Port hat von der Tuer-Sequenz NICHTS.** `grep -rn "DO2\|door_demo\|door_seq" re15_port/engine re15_port/include` = **0 Treffer**; DO2 kommt nur in `platform/psx` als Test-Mesh/Test-VAB vor. Umgesetzt sind aus `FUN_8001d600` nur der Nutzlast-Warp (`aot_common.c:468-600`) und die Blenden-FSM (`room_common.c:142-232`).

Fehlend, obwohl im **RE1.5-Auslieferungsstand** vorhanden und ausgefuehrt:
1. `FUN_800171f4` — DOOR TEXTURE nach 0x801a1000 laden
2. `FUN_800161e0` — Modell/TIM/VAB entpacken, Kamera 30000/22000, `SetBackColor(0xff,0xff,0xff)`, Bildschirmmitte (160,120)
3. `FUN_80029a98(1, &LAB_80016188)` — Demo-Koroutine
4. die 0x2000000-Warteschleife
5. `FUN_800170e0` — `SsVabOpenHeadSticky` der Tuer-VAB auf SPU 0x1020
6. `DAT_80072ec4 = 0x3c` am Ende

Weil RE1.5s einziges Tuer-Skript nur ein END enthaelt, ist der **sichtbare** Unterschied ein Bild — die Zustandsfolge ist trotzdem nicht byte-true.

⚠ [byte-belegt] Detailkorrektur fuer die Implementierung: `FUN_80014cd0` kopiert **0xC38** Byte, nicht 0xC30. Nach beiden Kopierschleifen (Schranke `+0xC30`) folgt unbedingt `@0x80014e78-94` zwei `lwl/lwr`- + zwei `swl/swr`-Paare = 8 weitere Byte. **Genau die** machen den anschliessenden `lw` auf Block+0xC30 (`@0x80014ea4`) gueltig.

⚠ [byte-belegt] RE1.5 laedt seine DOOR00-VAB sehr wohl (`FUN_800170e0`, SPU 0x1020, Id nach `DAT_800b21ec`), aber die Strings `"DOOR EDH"` `@0x80010e10` und `"DOOR VBD"` `@0x80010e1c` haben in `ghidra1_V2.txt` **keinen einzigen Xref** — es gibt keinen separaten Nachlade-Pfad. Der Ton steckt, wie in RE2, im DO2.

---

# 4. Was im Repo FEHLT (exakte Dateinamen) und was ohne diese Dateien nicht geht

## 4.1 Echte Luecken (Datei existiert nicht)

| Fehlende Datei | Was sie enthielte | Was ohne sie nicht geht |
|---|---|---|
| **Die echten RE2-Stage-Code-Overlays** (Ladeadresse ~0x8011A000). `info/re2leon/COMMON/BIN/STAGE1..7.BIN` sind 420..9984 B **Init-Stubs**: STAGE7.BIN registriert nur `0x8011A054→0x800CFAE8`, `0x8011A0F4→0x800CE540`, `0x8011A144→0x800CE544` und ruft `jal 0x80031fe4`. | Die raumseitige Event-/Boss-Inszenierung. | **Wer das 0x8000-Schlafbit des Bosses loescht und wer ihn von (−32000,0,−32000) an die Kampfposition setzt.** Das ist die groesste inhaltliche Luecke des Endkampfs. |
| **`RE2_Quellcode_Overlays/STAGE1..7/`** — je 1 Datei (`FUN_80100000.c`), STAGE6 hat 4; `STAGE1..7_overlay.c` je 266 B (STAGE6: 6070 B). ⚠ Die frueher gemeldete Fassung "nur STAGE6 hat 4 Dateien" war falsch wiedergegeben; die Schlussfolgerung stimmt. | Decompilate der Gegner-Overlays. | Ein Gegenlese-Text zu den AI-Roots `0x801000BC` / `0x80100178`. Ersatz: selbst disassemblieren (die Bytes SIND da). |
| **RE2-Savestate / RAM-Abzug aus ROOM7040.** `stage_saves/` ist reines RE1.5. | Die zur Laufzeit gefuellten Tabellen. | `0x800CFE5C` (Typ→Handler, im Image 96×0), `0x800D46D0[134..138]` (live BGM-Ids), `0x800D424B` (tatsaechliche ENEM-Bank), `0x800D8CD0` (Sound-Id-**Paar**), `0x800D4C48`/`0x800DBB78` (die Belege fuer "Bank 4 = CORE" in RE2). |
| **`info/re2leon/COMMON/SOUND/CORE02..CORE0A.{EDH,VB}`** — 9 Bank-Paare (je ~3176 B + ~40 KB). | Die restlichen Ingame-SE-Baenke. | Ein vollstaendiger Record-fuer-Record-Vergleich RE2 gegen RE1.5 CORE00..CORE13 — **noetig**, weil 7 von 11 gemeinsamen Baenken sich unterscheiden und CORE10/CORE11 vertauscht sind. |
| **`info/re2leon/PL0/RDT/roomg040/`** ist ein **komplett leeres Verzeichnis** (0 Eintraege), obwohl `ROOMG040.RDT` (162.808 B) vorhanden ist. | SCD-Auszug der Szenario-B-Arena. | Nichts Kritisches — die RDT-Bytes reichen. |
| **Eine RE2-SCD-Opcode-Laengentabelle.** Existiert **nicht als Bytetabelle** im EXE-Image (Suche nach `[0x51]=6,[0x02]=2,[0x13]=4,[0x14]=6,[0x1a]=2`: 0 Treffer). Jeder der 143 Handler ab `0x800A74C8` schiebt den PC selbst, teils zweigabhaengig. | — | Ein **sauberer SCD-Walk**. Alle Zensus-Ergebnisse hier (auch die 610 dtex-Records) sind mustergestuetzt. Nicht ableitbare Opcodes: `0x01, 0x03, 0x06, 0x07, 0x09, 0x10, 0x11, 0x17, 0x18, 0x19, 0x1A, 0x31, 0x36, 0x37, 0x39, 0x40, 0x46, 0x49, 0x4E, 0x6B, 0x7C, 0x7E, 0x7F`. |
| **Gerenderte Textur-Gegenprobe der Boss-TIMs.** Fuer Typ 0x20 existiert `info/re2leon/COMMON/BIN/em_type20_tex.png` als Vorbild; fuer 0x36/0x37 nichts. | — | Verifikation der 3 CLUTs / 512×256 des Bosses. |

## 4.2 Scheinbare Luecken — die Datei ist da, nur falsch benannt oder nie geoeffnet

Das ist der praktisch wichtigere Teil des Berichts.

| Angeblich fehlend | Tatsaechlich | Konsequenz |
|---|---|---|
| "`EMOVL36_S0/S1.BIN`, `EMOVL37_S*.BIN` fehlen" | Die Bytes liegen vollstaendig in `info/re2leon/PL0/PLD/CDEMD0.EMS`. Ausschneiden: `[3444*2048, +23476]`, `[3456*2048, +23476]`, `[3588*2048, +23024]`, `[3600*2048, +23024]`. | **Die Boss-KI ist verfuegbar.** Ich habe sie disassembliert (Abschnitt 1.3). |
| "`CDEMD1.EMS` hat keine Adressierungstabelle" | Zweite TOC `@0x8009B880`, Selektor `0x800CFC00`, Datei-Ids 469/470 (`@0x8001b940-0x8001ba14`). Alle 56 Records validiert, Bereich = exakt 5467 Sektoren = Dateigroesse. | **Szenario B ist byte-true adressierbar.** |
| "`EM036.EMD` / `EM036.TIM`" (in `PL0/PLD/CDEMD0/`) | **Sektorverschobene Fehl-Splits.** `EM036.TIM` liegt bei Sektor 3127 (TOC: 3468), `EM036.EMD` bei 3192 (TOC: 3533) und enthaelt ab 0xB800 einen fremden Overlay-Chunk. | **Wer daraus misst, misst nicht Typ 0x36.** Autoritativ ist ausschliesslich die TOC bzw. `re2_ems_toc.inc`. |
| "Die Tuer-Choreografie ist nicht disassembliert" | `info/re2leon/COMMON/DOOR/DOOR*/` — **498 `.c`-Routinen** in 55 Verzeichnissen, plus MD1/TIM/OBJ/VH/VB/BMP/ini je Tuer. **Nie geoeffnet.** | Die vollstaendige Bild-fuer-Bild-Choreografie liegt im Klartext vor. |
| "Es fehlen extrahierte WAVs der ROOM70x0-Baenke" | Richtig (0 wav unter `room70*/`) — **aber die Boss-Toene liegen gar nicht dort**, sondern in `ENEMSE.VBS`, und davon sind **980 WAVs** bereits extrahiert. | Boss-Zustand → SE-Id {6..14} → Bank 25 → hoerbares Sample ist ohne neue Daten aufloesbar. |
| "Nur die fuenf Endkampf-Raeume haben keine `.c`" | **Falsch. 191 von 245** Raumverzeichnissen haben gar keine `.c`. MIT `.c`: nur room1000-room11D0 (30) + die Untergruppen room?0A0..?0F0 (24) = **54**. Alle Szenario-B-Raeume (A..G) haben keine. | **Jeder Zensus, der ueber `*/scd/*.c` lief, ruht auf 22 % der Raeume.** Das entwertet u.a. den "344-Zeilen-Kind-Zensus". |
| "`RESULT.BIN` / `ENDING.BIN` / `OPENING.BIN`" | Vorhanden (17.360 / 47.194 / 13.524 B) — **nie als Schirm untersucht**, nur als Zensuszeile. RE1.5 hat **kein** Gegenstueck (`info/Re1.5/PSX/BIN` = nur DEBUG, STAGE1-6, TITLE). | Zwei komplette RE2-Schirme mit eigenen Effekten sind ununtersucht. |
| Sprach-/Filmspur des Abspanns | `info/re2leon/PL0/VOICE/` (nach Figuren sortiert) und `info/re2leon/PL0/ZMOVIE/` (STR + bereits konvertierte `.avi`). **Nie geoeffnet.** Bezug: ROOM7040 sub07 `@0x0019EA: 59 00 0d 00` = `xa_on 0x0d`. | Die Tonspur des Endkampf-Abspanns ist dort zu suchen. |

---

# 5. Umsetzbarkeit

## 5.1 Sofort machbar (alle Belege liegen vor)

**A. Zwei fehlende RE1.5-Toene im Port nachruesten.** `core_se(6)` im Titel-Zustand 3 (`@0x80102ac8`) und `core_se(9)` im Debug-Item-Werkzeug (`@0x8004a158`). — *Zwei Zeilen, je eine Adresse im Kommentar; unter einer Stunde.*

**B. Entscheiden, ob die drei RE2-Save-Screen-SEs im Port bleiben.** `main.c:1167-1169` + `menu_common.c:1106/1109/1119` sind dokumentierte RE2-Uebernahmen; RE1.5 ist dort nachweislich stumm (kein `jal 0x80045024` im ganzen Bereich, kein Datenwort `0x80045024`). Das ist eine **Nutzer-Entscheidung**, kein RE-Problem. — *Streichen: Minuten; beibehalten: nichts zu tun.*

**C. Die vier Chunks von Typ 0x36/0x37 unter korrektem Namen aus `CDEMD0.EMS` schneiden.** TIM `[3468*2048,+132640]` / EMD `[3533*2048,+112340]` / AI `[3444*2048,+23476]` + `[3456*2048,+23476]`; analog 0x37. Die TOC ist bereits als `re2_ems_toc.inc` vendored und stimmt. — *Ein Python-Skript, unter einer Stunde; beseitigt dauerhaft die EM036-Falle.*

**D. Den dtex-Zensus (610 Records) und die 498 decompilierten Tuer-Skripte als Dossier ablegen.** Beides ist bereits erhoben bzw. liegt im Repo. — *Ein Tag Dokumentation, kein RE.*

**E. Boss-Ton hoerbar machen.** SE-Ids {6..14} → ENEMSE-Bank-25-EDT `@0x1F2800` → VAG → eine der 980 vorhandenen WAVs. — *Ein Nachmittag, rein datenseitig.*

## 5.2 Erst nach RE (Bytes da, Arbeit noch nicht gemacht)

**F. Die sechs belegten Zustandsfunktionen des Bosses disassemblieren** (`0x801055CC[0,1,2,3,4,7]` im 23.476-B-Chunk). Zu ermitteln: Uebergaenge, `sltiu`-Abstandsschwellen gegen +0x1F0, Clip-Indizes (Anim-Kopplung), Schadensfenster, die beiden weiteren HP-Stores `0x80103004`/`0x80103060` (Phasenwechsel). — *Das ist die Hauptarbeit am Endkampf: geschaetzt 2-4 Tage; der Einstieg ist trivial (Adresse + Tabelle stehen fest), die Breite macht die Zeit.*

**G. Den ENEM-Bank-Resolver `FUN_80052b38` gegen die Paartabelle `0x800a7400` portieren** und die **Paar**-Semantik von `0x800d8cd0` klaeren (`@0x80057274-0x800572b8` fuellt zwei Slots, `@0x80052b7c-cc` matcht in beiden Reihenfolgen). Erst dann ist "ROOM7040 = Bank 25" hart. Heute nagelt der Port die Bank pro Gegnermodul fest (6/7/11/0). — *Ein bis zwei Tage; blockiert alle raumabhaengigen RE2-Gegner-SEs.*

**H. Den RE2-BGM-Tabellensetzer nachbauen**, falls RE2-BGM im Port ueberhaupt gewollt ist: Opcode `0x57` (8 B, `@0x80057e98`) + RAM-Array `0x800d46d0` + Stage-Basis `0x800a80af`/`0x800a80b0` + Datei-Index `0x800a8168`/`0x800a81e8`. Der Port kennt Stage 7 gar nicht (`audio_pc.c:2103 if (stage < 0 || stage > 5) return -1;`, Tabelle = RE1.5-statisch `SS_BGMTBL[106]`, `UNK_80074828`). ⚠ Und: RE2s Opcode `0x57` ist im Port korrekt der **RE1.5-Fade-Schritt** — die Opcodes duerfen nicht vermischt werden. — *Zwei bis drei Tage, und nur sinnvoll, wenn RE2-BGM als Option gewollt ist.*

**I. Die RE1.5-Tuer-Demo byte-true nachbauen** (`FUN_800171f4` / `FUN_800161e0` / `FUN_800164c8` / `FUN_80016518` / `FUN_800170e0` / `FUN_80016664` + die 0x2000000-Warteschleife). Sichtbar aendert sich fast nichts (RE1.5s Skript ist ein einziges END), aber die Zustandsfolge wird byte-true, und die Infrastruktur steht dann fuer alles Weitere. ⚠ Bei der VH-Kopie **0xC38**, nicht 0xC30. — *Drei bis fuenf Tage: DO2-Parser, Koroutine, VAB-Ladepfad und Demo-Renderer sind vier neue Bausteine im Shared-Engine.*

**J. Falls RE2-Tueren als Option gewollt sind:** DO2-Lader mit der Chunk-Tabelle `@0x8009a520`, die 55 Archive, der Schloss-Handler `@0x80051514`, die Ladekaschierung fuer dtex 0x32/0x34. Der SCE-Index-Unterschied (RE1.5=2 vs RE2=1) muss dabei sauber getrennt bleiben. — *Eine Woche plus; grosses Feature, klar abgegrenzt.*

## 5.3 Blockiert (nicht loesbar mit dem, was im Repo liegt)

**K. Wer den Boss scharfschaltet und positioniert.** Steht im STAGE7-Code-Overlay ab ~0x8011A000, das nicht im Repo ist. Ersatzwege: (a) RE2-Savestate aus ROOM7040 ziehen; (b) einen vollstaendigen SCD-Walk von ROOM7040 fahren, sobald eine Opcode-Laengentabelle existiert — ROOM7040 Bank1 hat 16 Subs, und der `work_set kind=3`-Poll ist schon lokalisiert. **Ohne (a) oder (b) ist der Endkampf nicht end-to-end portierbar.**

**L. "Bank 4 = CORE" fuer RE2 belegen.** `0x800d4c48` und `0x800dbb78` sind im statischen Image komplett 0x00. Braucht einen RAM-Abzug.

**M. Der Konsument der Tuer-Zielfelder +22/+23/+24.** Ein EXE-weiter Scan auf `addiu`-Immediate `0xe558` findet nur Setzer. Die zwei Loads `0x80055170`/`0x800551f4` sind der einzige Ansatz — noch nicht verfolgt.

**N. Szenario-Zuordnung ROOM7xx0 vs ROOMGxx0.** Die Namensbildungs-Routine wurde nie gefunden. Aussage bleibt unbelegt.

**O. Der Name des Endgegners.** Existiert in den Daten nicht. Jede Nennung waere eine Erfindung.

---

## Die drei naechsten konkreten RE-Schritte

**1. Die sechs Zustandsfunktionen des Endgegners disassemblieren.**
Chunk schneiden: `info/re2leon/PL0/PLD/CDEMD0.EMS[3444*2048 .. +23476]` (Bank 0, Ladebasis `0x8010D000`) und `[3456*2048 .. +23476]` (Bank 1, Ladebasis `0x80100000`). Einstieg `0x801000BC`, Tabelle `0x801055CC` = `{0x801003CC, 0x80100784, 0x801025BC, 0x80102BBC, 0x80103834, 0, 0, 0x80103878}`. Gesucht: die `sltiu`-Vergleiche gegen `+0x1F0` (Distanz), die Clip-Schreiber auf `+0x94`, die Aufrufe von `0x8002959C` (Anim-Advance) und die Bedeutung der HP-Stores `0x80103004`/`0x80103060`. **Das ist die einzige verbliebene grosse Unbekannte am Endkampf, und die Bytes liegen bereit.**

**2. Den ENEM-Bank-Resolver `FUN_80052b38` vollstaendig disassemblieren.**
Kernfrage: was steht in beiden Slots von `0x800d8cd0`, wenn ROOM7040 seine fuenf `Sce_em_set` abgearbeitet hat (`@0x80057274-0x800572b8` fuellt Slot 0, dann Slot 1). Zeile 25 der Tabelle `0x800a7400` ist `(27,00)` — sie matcht nur bei leerem Slot 1. Ohne diese Klaerung ist "ROOM7040 = ENEM-Bank 25" mustergestuetzt, und damit ist die gesamte Zuordnung Boss-SE-Id → hoerbares Sample nicht hart. Gleichzeitig liefert der Schritt den Resolver, den der Port fuer **alle** RE2-Gegner-SEs braucht.

**3. Den Konsumenten der Tuer-Zielfelder lokalisieren.**
Das ist der einzige nicht disassemblierte Baustein der gesamten Tuer-Kette. Ansatzpunkte: die zwei Loads auf die AOT-Tabelle `0x800ce558` bei `0x80055170` und `0x800551f4` vorwaerts verfolgen, bis der Zugriff auf AOT-Struktur `+20/+21/+22` faellt. Gelingt das, ist die RE2-Tuer-Sequenz von der Trigger-Kante bis zum Zielraum lueckenlos byte-belegt — und der 610-Records-Zensus wird von "mustergestuetzt" zu "belegt".