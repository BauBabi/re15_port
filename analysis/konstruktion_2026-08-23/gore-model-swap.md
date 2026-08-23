# Gore-Modelltausch + Part-Scatter

Alle drei RE-Stränge sind aufgelöst. Hier die Rohdaten für das Dossier.

---

# GORE-MODELLTAUSCH + PART-SCATTER — RE-Ergebnis (Infrastruktur-Blocker #2)

## 1) MECHANISMUS

### 1A. RE1.5 — Die Part-Record-Maschine der EXE (der komplette +0x188-Konsument; Krähen-Scatter-Antwort)

**Allokator/Init (Xref-Jagd abgeschlossen):**
- `FUN_8001e56c` (RE_15_Quellcode_V2): `+0x83 = *(model+0x10 hdr[-4])>>1` (Bone-Zahl), `*(ent+0x188)=buf`, return `buf + (+0x83)*0xac`. Caller: Player-Init `@0x80031614`, **Sce_em_set `@0x80042524`** (ghidra1_V2.txt Z.96887/126330/152797).
- `FUN_8001e5b0` = Record-INITIALIZER, Caller `@0x80031624` (Player) und **`@0x80042530`** (Sce_em_set, direkt nach dem Allokator; ghidra1_V2.txt Z.96972-73). Pro Record i: `word0=1`; `+0x8 = model+i*0x38` (MD1-Tri-Block), `+0x10 = model+i*0x38+0x1c` (Quad-Block); Prim-Aufbau `FUN_80025940`/`FUN_80025a98` (Doppelpuffer, Cursor läuft als param_2 durch); EMR-Bone-Offset → `+0x2c/+0x30/+0x34` (int) und `+0x38/+0x3a/+0x3c` (short); Part-Index-Byte `+0x74`; Eltern-Verkabelung: rec0 `+0x6C=&ent+0x20`, `+0x90=ent`; Kind `+0x6C=&parent_rec+0x40` (Welt-Matrix des Elternteils!), `+0x90=parent_rec`; Template enthält Farb-Default `0x808080` (local_54).

**Record-Layout RE1.5 (Stride 0xac), Bits von word0 — jede Aussage aus den Konsumenten:**

| Feld | Bedeutung | Beleg |
|---|---|---|
| +0x00 | Flag-Wort | s.u. |
| +0x04/+0x0C, +0x14 | Tri-/Quad-Prims (Doppelpuffer `DAT_800aca34*0x28` bzw. `*0x34`) | `FUN_8001e9ec`: `FUN_800254a0(rec[2], aca34*0x28+rec[3], rec+0x68,0)` / `FUN_800256b0(rec[4], aca34*0x34+rec[5],…)` |
| +0x08/+0x10 | Mesh-Zeiger Tri/Quad | `FUN_8001e5b0` |
| +0x18 | lokale Matrix | `FUN_8001ef54`: `FUN_80022da0(rec[0x1b], rec+0x18, rec+0x40)` |
| +0x38/3a/3c | Wurf-Velocity (Modus 0x20) | `FUN_8001f024`: `t[0]+= (s16)rec[0xe]; +0x3a += (s8)+0x75; t[1]+=+0x3a; t[2]+=(s16)rec[0xf]` |
| +0x40 | Welt-MATRIX (t bei +0x54/+0x58/+0x5c) | dito |
| +0x60 / +0x78 | aktive Rot / Pose-Rot (Crossfade `FUN_80020510`) | `FUN_8001f3bc` |
| **+0x68** | **PAKET-FARBWORT (24-bit RGB, Modulation)** | `FUN_80023410`: `uVar5 = *(rec+0x68)&0xffffff \| 0x34000000`; `FUN_80023708`: `…\| 0x3c000000` |
| +0x6C | Eltern-Koordblock-Zeiger | `FUN_8001ef54`/init |
| +0x75/+0x76 | Gravity-Byte / Spin-Rate (Modus 0x20) | `FUN_8001f024`, `FUN_8001f220` (setzt 0x14/8) |
| +0x82 | Spin-Winkel (`RotMatrixZ`) | `FUN_8001f024` |
| +0x88/8a/8c | Scale (Bits 0x400/0x800/0x2000) | `FUN_8001e9ec` |
| +0x90 | Eltern-Record | init |
| +0x94/+0x98 | Modus 0x80: Ziel-FARBWÖRTER | `FUN_8002441c(rec+0x4, rec[0x25], rec[0x26])` — pro Byte `(prim+ziel)>>1` |
| +0x96/+0x9a/+0x9c | Modus 0x8: vel-y / grav / Tick-Zähler | `FUN_8001f1e0`: `+0x96 += +0x9a; t[1](+0x58) += +0x96; if(+0x9c > 0x1c) word0=0` |
| +0x9a | Modus 0x10: Countdown | `FUN_8001f220` (bei 0: XOR `word0^=0x1062`, +0x75=0x14, +0x76=8, +0x82=0, vel=0) |

**word0-Bits** (aus `FUN_8001e9ec`/`FUN_8001ef54`/`FUN_8001f3bc`): 0x1 aktiv/zeichnen; 0x8 **EXPLODE-SCATTER** (Zeichner `FUN_80023410`/`FUN_80023708` + Physik `FUN_8001f1e0`); 0x10 Timer-Modus (`FUN_80023a58`/`FUN_80023e90` + `FUN_8001f220`); 0x20 Fall/Bounce-Physik `FUN_8001f024` (Boden = `player+0x1ba`, Bounce `+0x3a = -+0x3a>>3`, Ruhe → `+0x54=1`, Bit0 löschen = tot; mit Bit 0x10 am Boden → `|0x2000` Squash 10000/8000); 0x40 **DETACH** (Eltern-Concat übersprungen → Welt-Matrix friert auf Todespose ein); 0x80 Farb-Blend `FUN_8002441c`; 0x200 Rot-Freeze; 0x400/0x800 Scale; 0x1000 eigenes Licht (`FUN_80053fc0(rec+0x54)`); 0x2000 Squash. **Bit 0x2: von KEINER Maschinen-Funktion gelesen** (nur im XOR 0x1062 von f220) — s. OFFEN.

**Draw-Walk = Integrator/Renderer der armierten Records:** `FUN_8001e8c8` (EINZIGER Caller von `FUN_8001e9ec`/`FUN_8001ef54`, XREF 8001e990/8001e9b4), gerufen aus dem Entity-Render-Dispatcher `@0x8001d09c`/`@0x8001d108`; Entity-Cursor `DAT_800ac784` ist der PER-ENTITY-Schleifenzeiger (`FUN_8001a50c`: `&DAT_800acc2c`, Stride 0x7d Wörter) — **die Maschine läuft also für Player UND alle NPC/Gegner, auch für die tote Krähe in State 7**. On-/Off-screen via `FUN_80014368`.

**Scatter-Zeichner:** pro Polygon Verschiebung `dir = (vx<<16>>21, (vy+300)>>6, vz<<16>>21) * curve[+0x9c]` (vertex-radial), rotiert per `gte_rt`, dann `rtpt` + flach getönter GT3/GT4 (Farbwort in alle 3/4 RGB-Slots → Textur-MODULATION mit +0x68). `FUN_80023708` inkrementiert `+0x9c` am Ende (einmal pro Frame). **Kurven-Tabelle `@0x80073454`, 30 shorts (selbst gedumpt):** 1E 3B 57 72 8C A5 BD D4 EA FF 113 126 138 149 159 168 176 183 18F 19A 1A4 1AD 1B5 1BC 1C2 1C7 1CB 1CE 1D0 1D1 (Ease-out, Endauslenkung 465). OT-Einfügung `@&DAT_800aa6d8 + aca34*0x1000`, Slot `otz>>4`.

**Krähen-GIB-Armierung (byte-verifiziert `@0x80114a2c-aa4`):** `ori t1,zero,0x8f` @0x80114a30 / `addiu t0,zero,-50` @0x80114a34 / `ori a3,zero,0x3` @0x80114a38; Schleife i<13: `sw t1,104(v1)` (+0x68=0x8f → RGB(0x8f,0,0) = dunkelrot-moduliert), `sh zero,148` / `sh t0,150` / `sh zero,152` / `sh a3,154` / `sh zero,156/158` (+0x94=0, +0x96=−50, +0x98=0, +0x9a=3, +0x9c=0, +0x9e=0), `ori v0,v0,0x4a; sw v0,0(v1)` @0x80114aa0-a4. **Lebenslauf: Teil steigt (−50, +3/Tick), Tris fliegen radial per Kurve auseinander, Selbst-Kill bei Tick 29 (`+0x9c>0x1c`) — der 50-Tick-Wipe `sw zero,0(part)` @0x80114b78 ist nur Backstop.** Der „Feder-Burst" ist in Wahrheit ein roter Chunk-Burst der 13 texturierten Bone-Meshes.

### 1B. RE2-Zombie (EMOVL10_S0.BIN) — Dressing-Helfer, Abriss, Stumpf-Tausch

**0x80106128 tauscht KEIN Modell.** Ablauf: `+0x10E|=0x80` (@0x8010613C-48, Einweg-Latch, kein EXE-Leser — Grep über alle RE2_Quellcode_V2-0x10e-Leser negativ); dann rnd (`FUN_80015fe8`, LCG auf `DAT_800ce318`) &1 → Variante A: fx `0x05032710` an rec0+0x48 mit Vektor (0,200,0); rec8 aktiv → `0x05031388`; rec3 aktiv → `0x050313E8` (0,200,0). Variante B: rec0 ohne Vektor; rec3 → (0,200,0); rec6 → (0,−200,0). fx-Spawner `FUN_8001bf10` = RE2-Effekt-Slot-Allokator (0x60 Slots à 0x7c @0x800D8D08-Region), a1=`+0x76` yaw, a2=&rec+0x48 (RE2-Welt-Matrix des Parts). Danach, außer `+0x5==16`: **Grau-TINTEN in `rec+0x70`** (RE2-Farbwort, = RE1.5 +0x68+8): rec0/11/13/14=0x404040, rec1/10=0x484848, rec2/5=0x707070, rec8/9/12=0x505050 (@0x8010627C-F4) — NCCT-Modulations-Verkohlung, keine Geometrie. Geschwister: `0x8010640C` (Ruß, ohne rec8), `0x80106310` (Ätzung: `+0x21A|=0x1800`, Tinten 0x304040/0x405050/0x506060, fx 0x040F1770/0x040F0FA0), `0x80106510` (Elektro: 4× fx `0x0600xxxx` mit Zufallsvektoren `128−rnd` je Achse). Die 14 Aufrufer sind Zeilen-gegatete Death/Hurt-Stellen (Zeile=+0x5: 9→640C, 10→6128, 11→6310, 14→6510, 16→6128 ab `+0x23A>=9`); Muster an 6 der 14 Sites instruktionsgeprüft (@0x801010B8-D0, @0x8010519C-C4, @0x80105234-64, @0x80105528-64, @0x80105DD4-FC, @0x80107970-A18) — deckungsgleich mit der bereits im Port dokumentierten Karte.

**Der eigentliche ABRISS (neu byte-belegt):**
- **Arm** (`Knockdown 0x80107438` P0, @0x80107514-7C): rnd&1 → `rec3.word0|=0x4A` @0x80107544-48; Drift-Felder `+0x98=yaw+0x800` (fliegt nach HINTEN), `+0x9A=−10`, `+0x9C/+0x9E/+0xA0/+0xA4=0` (@0x8010754C-74); **Farbe `rec+0x70=0x0010104F`** (dunkelrot) @0x80107568; Blut-Cleanup `FUN_8001cefc(5,3,rec3+0x48)`; **Kind `rec4.word0=0`** @0x80107588 (Unterteil verschwindet ersatzlos); SE 2. Verschachtelt rnd&1: rec5 und rec6 identisch armiert + `rec7=0` @0x8010767C.
- **Bein/Stumpf** (@0x80108E40-F94): `rec9.word0|=0x1062` (Wurf-Physik-Modus), Felder `+0x38=yaw+0x800, +0x3A=0, +0x3C=100, +0x3E=0, +0x40=yaw, +0x42=1024`, Byte `+0x79=30`, `+0x86=0`; `rec10.word0|=0x80` + `+0x98=64` / `+0x9C=0x00101010` (Fade-nach-Dunkel). Alternativpfad (`+0x21A&0x20`): rec9/10/11 = 0 + Boden-Decals `FUN_8001cefc(5,3,rec+0x48)`. Zweites Bein (`+0x21A&0x40`, rnd): rec12|=0x1062, rec13|=0x80; danach rec11|=0x80.
- **STUMPF-TAUSCH — die Antwort auf die Stumpf-Frage:** `@0x8010531C-50` kopiert das Geometrie-Quartett `+0x8/+0xC/+0x10/+0x14` von **rec15** in **rec9** (Oberschenkel zeigt Stumpf; Gate rnd&1 && Typ!=30); `@0x80108FC0-DC` kopiert **rec16** → **rec0** (Hüft-Stumpf, nach `rec0|=0x80` + `+0x98=32/+0x9C=32` @0x80108FA0-B4). **Die Stumpf-Meshes sind RESERVE-PARTS 15/16 DESSELBEN EM010-MD1** — dieselbe Datei, kein eigenes dir[], kein separater Blob. **Selbst nachgemessen:** `CDEMD0.EMS` @0x2A800, dir_cnt=8, `D[7]=0x1EE38`, MD1 `object_count=34` → **17 Parts** (15 Skelett + 2 Stümpfe). INIT versteckt sie: `sw zero,2580/2752` @0x801010DC/E8 (rec15/rec16 word0=0; wiederholt @0x80104D38/44).
- **Kinder-Parts:** flacher Walk ohne Baum (`FUN_80027160`, Delay-Slot-Advance @0x800273A4/F8) — Kinder verschwinden nur über die Flag-KASKADE `(Eltern&0x21)==0x20` (`FUN_80027434` @0x80027470-94); ein fliegendes Elternteil behält Bit 0, die Kinder zeichnen an der eingefrorenen Matrix weiter, bis die Wurf-Physik `FUN_80028ad8` beim zweiten Bodenkontakt Bit 0 löscht.
- **NEU — der fehlende Konsument des „Stumpf-Markers" Bit 0x80 (Port-Grenze (3) GELÖST):** `FUN_80027434` (RE2_Quellcode_V2, Z.162-166): nach dem Normal-Zeichnen `if (param_3 & 0x80) FUN_8002940c(rec+0x4, rec[0x26](+0x98), rec[0x27](+0x9C))` — **per-Frame-Prim-Farb-Blend Richtung (+0x98, +0x9C)**, RE1.5-Zwilling `FUN_8002441c` (`(prim+ziel)>>1` pro Byte, Tri-Prims byte-weise, Quad ebenso). Rec10/11/13 faden also exponentiell nach ~0x101010 (fast schwarz), rec0 nach 32/32. Bits 0x8000→Cull-Test `FUN_8002c820`→0x4000=Skip (das `|=0x9000` @0x80106DC0-E14 auf rec1-7 im Sturz-Handler). `FUN_80017054` (INIT-Preamble @0x80101040-B4 auf rec10/12/13/16) = per-Part-UV/CLUT/TPage-Umschaltung (Gore-Textur-Variante), kein Mesh-Tausch.

## 2) PORT-STAND

Der Port ist hier weiter als der Auftragstext annahm — die RE2-Seite ist seit „Welle G" weitgehend gebaut:
- `re15_port/engine/src/enemy_ai_re2_zombie.c`: Modellblock-Doku+Reset :3350-3433 (`re15_re2z_part_reset`, 16 Records — **eins zu wenig, s. Plan**); Anzeige-Brücke `re15_re2z_gore_active/resolve` :3567-3620 (Bit-0-Draw + 0x21/0x20-Kaskade + Stumpf-Index in `out_mesh`); `re15_re2z_gore_part_matrix` :3787; Dressing-Zwillinge burn/acid/soot/spark :3832-3930 + Zeilen-Dispatch :3937-3948; `re2z_leg_gore` :3971 (`part_mesh[thigh]=15` :4005); Hüft-Split (`part_mesh[0]=16`) :4888; Arm-Drift :5382ff; Physik-Zwillinge FUN_80028AD8/DAC. Benannte Grenzen :4410-4427: (1) Stumpf unsichtbar im RE1.5-Import (Bank hat nur 15 Meshes), (2) Kinder folgen fliegendem Teil nicht, (3) Bit-0x80-Konsument unbekannt — **(3) ist mit `FUN_8002940c` jetzt beantwortet**.
- `re15_port/platform/pc/main.c`: Brücken-Konsum :6933-7010 (Draw/Tint/part_matrix), Mesh-Klammer :7046-47 (`gore_mesh[nbi] < mesh_count`), Krähen-Skip :6729-32 (`crow_hide`).
- `re15_port/engine/src/re15_damage.c`:1384/1572 (`re15_re15_re2z_gore_hit`).
- `re15_port/engine/src/re2_ems.c`:95-103 lädt EM010 aus CDEMD0.EMS, `re15_md1_parse` (md1_common.c:51-54, Cap `MD1_MAX_MESHES=32`) → **mesh_count=17 kommt im RE2-MODELS-Modus vollständig an**; die Klammer main.c:7046 ließe 15/16 also bereits durch.
- Krähe: `enemy_ai_common.c`:5440-5460 GIB-Lane mit `crow_hide=1` + ESP-Feder-Stand-in; `enemy_ai_re2_crow.c`:1283 analog. Die RE1.5-Part-Maschine existiert im Port für den RE1.5-Flavor NICHT.

## 3) IMPLEMENTIERUNGS-PLAN

**(a) Sichtbare Stümpfe:**
1. `re15_re2z_part_reset` und alle `re2z_part_*`-Arrays von 16 auf **17** Records erweitern (rec16 existiert: Stores @0x80108FC0-DC, INIT-Null @0x801010E8) — enemy_ai_re2_zombie.c + re15_actor.h.
2. RE2-MODELS-Modus: per Probe (`test_re2_gore_render`/`probe_re2_gore` erweitern) verifizieren, dass nach `re2z_leg_gore` bzw. Hüft-Split `out_mesh=15/16` durch die Klammer geht und das Stumpf-Mesh zeichnet (mesh_count=17 gemessen); visuell per Skill `re15-port-visual-verify` (gdigrab).
3. Bit-0x80-Blend implementieren (Konsument jetzt belegt): pro gerendertem Frame `part_tint = ((part_tint + ziel)>>1)` je Kanal (Ziele liegen byte-true in den schon gestempelten Feldern `+0x98/+0x9C`); Einbau in `re15_re2z_gore_resolve` bzw. einen Frame-Step neben `gore_part_matrix` (@FUN_80027434:162 / FUN_8002940c; RE1.5-Zwilling FUN_8002441c). Test: Fade-Folge 0x808080→0x101010 geometrisch pinnen.
4. RE1.5-Import-Modus („AI RE2"): Stumpf bleibt wie vom Nutzer akzeptiert aus (RE1.5-Bank hat 15 Meshes); Alternative dokumentieren: EM010-Parts 15/16 beim Hybrid-Bank-Bau anhängen (re2_ems.c) — nur mit Nutzer-Entscheid.

**(b) Fliegende Körperteile (Krähe, RE1.5-Maschine):**
1. Generische RE1.5-Part-Schicht (Storage wie `re2z_part_*`, aber RE1.5-Feldsemantik): pro Part word0-Flags, Farbe (+0x68), `+0x96/+0x9a/+0x9c`, eingefrorene Welt-Matrix (Snapshot beim Armieren = Bit-0x40-Semantik, Muster `re2z_part_m/t`).
2. Physik-Step = `FUN_8001f1e0`-Zwilling: `vel += 3; y += vel; tick++ (FUN_80023708-Ende); if (tick > 0x1c) part tot` — läuft einmal pro Engine-Frame (PSX: Render==Logik-Frame).
3. Renderer (main.c NPC-Loop, statt `crow_hide`-Skip): armierte Parts als EXPLODE zeichnen — pro Tri/Quad Versatz `((vx<<16>>21), ((vy+300)>>6), (vz<<16>>21)) * curve[tick]`, rotiert mit der eingefrorenen Part-Rotation, auf die Translation addiert; Farben aller Ecken = 0x00008F-Modulation (Textur bleibt, G/B ausgelöscht → dunkelrote Chunks); Kurve = die 30 Shorts @0x80073454 verbatim.
4. Armierung: `enemy_ai_common.c:5452` `crow_hide=1` ersetzen durch die 13er-Schleife (+0x68=0x8f, +0x96=−50, +0x9a=3, word0|=0x4a @0x80114a2c-aa4); 50-Tick-Wipe @0x80114b78 als Backstop behalten; ESP-Feder-Stand-in entfernen (Original-GIB-Lane hat KEINEN ESP-Spawn — die „Federn" sind die roten Chunks).
5. Verifikation: `probe_crow_death` erweitern (Waffe 7 → 13 Parts armiert, alle tot ≤ Tick 29, Körper-Skip erst danach); gdigrab-Vergleich gegen DuckStation-Capture (Skill `re15-room-capture`, ROOM10C0/1170).

## 4) OFFEN (ehrlich)

1. **RE1.5 word0-Bit 0x2**: kein Leser in der gesamten Maschine (e8c8/e9ec/ef54/f024/f1e0/f220/23410/23708/2441c/254a0/256b0); Teil des f220-XOR 0x1062. Nächster Schritt: EXE-weiter Census `lw word0`+`andi 0x2` oder als Write-only akzeptieren.
2. **+0x68-Init-Wert**: Leser beweist +0x68=Farbe; der Template-Wert 0x808080 (FUN_8001e5b0 local_54) ist im Block-Copy nicht instruktionsgenau auf +0x68 gemappt. Nächster Schritt: Roh-Disasm der Store-Schleife @0x8001e6xx.
3. **RE2-Part↔Körperteil-Namen** (rec3/5/6 = welcher Arm, rec9/12 = Schulter vs. Oberschenkel): Bind-Offset-Messung des Ports widerspricht den Welle-E-Namen; Mechanismus (Indizes/Flags) davon unberührt. Nächster Schritt: Render-Katalog der EM010-Parts.
4. **RE2 0x1062-Wurffelder** (+0x38 yaw, +0x3C=100, +0x42=1024, +0x79=30): Interpreter `FUN_80028AD8` laut Port-Kommentaren byte-true portiert (Welle G); in dieser Session nicht re-verifiziert.
5. **fx-Identitäten** (0x05032710 usw., Bank/Gruppe): visuell nicht gemappt; Port-Stand-in (RE1.5-Raumbank-Blut) bleibt dokumentierte Annäherung.
6. **8 der 14 Aufruf-Sites** (0x80107C18/C70, 0x80108468/708/800/B24, 0x80109BB4) nur über das Port-Dokument bestätigt, nicht in dieser Session einzeln disassembliert — Muster an 6 Sites deckungsgleich.
7. **`|=0x9000` auf rec1-7** (@0x80106DC0-E14, Sturz-Handler, Gate frame≥0x14 && rtn2==1): Zweck (Cull-Test-Freigabe 0x8000 + eigenes Licht 0x1000) belegt, Handler-Kontext nicht vollständig gemappt.

Relevante Pfade: `c:\workspace\git\reAi_v2\RE_15_Quellcode_V2\FUN_8001e5b0.c` / `FUN_8001e9ec.c` / `FUN_8001ef54.c` / `FUN_80023410.c` / `FUN_80023708.c` / `FUN_8001f024.c` / `FUN_8001f1e0.c` / `FUN_8001f220.c` / `FUN_8002441c.c`; `RE2_Quellcode_V2\FUN_80027434.c` / `FUN_80017054.c` / `FUN_8001bf10.c`; `re15_port\engine\src\enemy_ai_re2_zombie.c` (:3350-4030, :4395-4900), `re15_port\platform\pc\main.c` (:6729, :6933-7047), `re15_port\engine\src\enemy_ai_common.c` (:5440-5460, :10804-10821); Overlay-Disasm-Dump: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\e15d7177-43f5-413e-8f99-d5e5f369c973\scratchpad\emovl10_s0.asm`.
