# RE1.5 Player-Select ("PLEASE SELECT MAIN CAST") — byte-true Draw-Spec

Reverse-engineered aus TITLE.BIN (lädt @0x80100000, KEIN Header, file_off = addr-0x80100000, 11832 B) +
PSX.EXE. Quelle: Workflow wf_d0863bd2 (6 Komponenten-Agenten, adversariell verifiziert). Für den byte-true
Port des LEON/ELZA-Player-Selects. Scene-Struct-Base **`0x80190000`** (Ptr @0x801026c0, gesetzt @0x801010c0-c8).

## Einstieg / Task
- Live-Pfad = Standalone-Task **@0x80101094** (via `FUN_80029ba4(a0=0x80101094)` @0x80102c9c aus dem Menü-
  NEW-GAME-Confirm registriert; REPLACE-TASK, ersetzt den Title-Task). **state6 @0x801023d0 ist TOTER Dev-Code**
  (font-basierter 2D-Selector, `pad&0x5000`-Toggle) — NICHT porten.
- Main-Loop: 3-Level-State-Machine. Dispatch-Tabelle **@0x8010266c** (9 Words), indexiert von `scene+0x390`.
  Pro Frame: render fns 0x80100654 / 0x80100a50 / 0x80100f14 / 0x80101bf0, dann Flip 0x80029ac8.

## State-Machine
- **LEVEL-1** (`scene+0x390`, u16): 0=INIT(sub0 0x8010115c) → 1=SELECT-INPUT(sub1 0x80101258) → 2=CONFIRM-SEQ(sub2 0x801012e4).
  sub0 self-advances 0→1; sub1 auf CONFIRM 1→2.
- **LEVEL-2** (`scene+0x392`, u16, in sub2 via Tabelle+0xC @0x80102678): 0=START-ZOOM(sub3 0x8010132c) → 1=ZOOM-LOOP(sub4 0x80101394) → 2=PAN-SETUP(sub5 0x80101438, 1f) → 3=PAN(sub6 0x801014f0, 40f) → 4=FADE(sub7 0x801015f0, 60f) → 5=HANDOFF(0x8010166c).

## Input (sub1 @0x80101258) — Pad-Wort BYTE-SWAPPED @0x800ac762 (Edge/new-press)
- **CONFIRM (→Zoom→Spiel): `pad & 0x8f0`** @0x80101268 = △(0x10)|○(0x20)|✕(0x40)|□(0x80)|Start(0x800) = jeder Face-Button oder Start. Ungated. Advanct `scene+0x390` 1→2.
- **TOGGLE Leon↔Elza: `pad & 0xa000`** @0x801012c0 = Links(0x8000)|Rechts(0x2000) = D-Pad L/R. GATED auf Pulse `scene+0x32e >= 0x80` (16-Frame-Debounce). `scene+0x394 ^= 1`, reset `scene+0x32e=0`.

## Highlight-Pulse (0x8010080c, jeden Frame via render fn 0x80100654 @0x801006c4)
- Counter `scene+0x32e`: init 0x80, +8/Frame bis 0x80 cap (=16 Frames), reset 0 bei Toggle.
- Schreibt Helligkeits-Byte auf +4 der 2 TILE-Prims (stride 0x24) @`scene+0x32c`, indexiert sel*0x24:
  **SELECTED[+4] = (-0x80 - counter)&0xff** (0x80→0x00 = aufhellen), **OTHER[+4] = counter** (0x00→0x80 = abdunkeln). → 16-Frame-Crossfade der Auswahl-Rechtecke.

## Zeichen-Reihenfolge pro Frame (byte-true)
1. **Background** = SELECTH.TIM (id **0x1d**, 16bpp 320×240, „PLEASE SELECT MAIN CAST"-Header + Dach(Leon,links)/Trümmerraum(Elza,rechts) eingebacken). JEDEN Frame re-geblittet: `FUN_80043850(w=0x140,h=0xf0,mode=1)` registriert Descriptor (@0x800b8544/48/4c); `FUN_80043870` blittet pro Frame nach VRAM(0,0)/(0,240) Doublebuffer (mode=1 skippt 0x14-TIM-Header → Pixel @0x80198014). Staging-Buffer 0x80198000.
2. **Modelle** (0x80101bf0): 2-Actor-Loop, Base **0x800acc2c** (Leon) / **0x800ace20** (Elza), stride **0x1f4**. BEIDE jeden Frame gezeichnet, laufender **Idle-Clip 2** (actor+0x94=2, advance `FUN_8001f314(a3=0x200)`). Elza zusätzlich Spring-Motion `FUN_80024c30` (4× Feder-Ketten). Draw `FUN_8001e8c8` (RotMatrix actor+0x68→+0x20, Lighting FUN_80053fc0, Mesh-Parts stride 0xac).
   - **Positionen:** Leon POS X=**-0x620**, Elza POS X=**+0x620**; POS Y=**0x7f4** (beide); POS Z=**0**; rot_y=**0x404**; color=**0x808080**.
   - **Load (sub0 via FUN_80101720):** 2-iter (s3=0 Leon / s3=1 Elza); CD-file-id-Tabelle @0x80102690 (halfwords) — Leon-Slot(s3=0)=**0x2c**, Elza-Slot(s3=1)=**0x44**. Port lädt PL00.PLD(Leon)/PL01.PLD(Elza). Vertex-Adjust-Consts (FUN_80022150): Leon (0x17,0x10), Elza (0x05,0x13).
3. **Overlay Group A** = 2× TILE (solid rect, GPU 0x60, semi-transparent). Leon `scene+0x330` XY(0,40) 160×200; Elza `scene+0x354` XY(160,40) 160×200; stride 0x24. Init RGB (128,128,128)/(0,0,0). = die Auswahl-Highlight/Dim-Rechtecke (Pulse-moduliert, s.o.). Init 0x801004f8, recolor 0x80100654→0x8010080c/0x801006e4.
4. **Overlay Group B** = 4× SPRT (textured, GPU 0x64, sampelt Selecth3-Atlas). Prims `scene+0x188+i*0x3c`, gebaut im Setup-Loop @0x800108a0 (4 Iter, `sltiu $s3,4`). Geometrie-Tabelle @0x80102624 = **6B/Eintrag `[x, y, w, h, u, v]`** (byte-verifiziert 2026-07-14, alle Bytes gelesen): Zeile0 Leon-Name **screen(16,48)** UV(0,0) 136×24; Zeile1 Leon-Sub **screen(24,216)** UV(8,26) 128×14; Zeile2 Elza-Name **screen(168,48)** UV(0,48) 136×24; Zeile3 Elza-Sub **screen(176,216)** UV(8,74) 128×14. RGB const (0x80,0x80,0x80). **Namen (0/2) = IDLE-sichtbar; Subs (1/3) = CONFIRM-ONLY** — der Zoom-Animator 0x80100a50 (gated `scene+0x15c` state 2/3) holt sie erst im Confirm-Zoom on-screen; das native Idle zeigt bei y=216 NICHTS. Zoom-Ziele @0x8010263c (Leon)/@0x8010264c (Elza) `[0x000B/0x0014/0x00B0/0x00D0]<<16`.
5. **Overlay Group C** = 2× SPRT (textured, GPU 0x64, Selecth3). Prims `scene+0x2ac+i*0x54`. Tabelle @0x8010265c (8B): Leon screen(24,168) UV(0,88); Elza screen(176,168) UV(0,136); WH 120×48. = die 2 PROFILE-Blöcke (HEIGHT/AGE/WEIGHT/BLOOD). Confirm-Flash-recolor 0x80100f14.

## Text = PRE-RENDERED (kein Font-Draw im Live-Screen)
- **Selecth3.tim (id 0x1f, 8bpp 256×256 + 256-CLUT)** = Text-Atlas: „Leon,S,Kennedy"(blau)+„from police department"(grün), „Elza Walker"(rot)+„from college dormitory"(grün), 2 PROFILE-Blöcke. Einmal @VRAM hochgeladen (`FUN_8004ee78`, Slot-Byte @0x800aca4c=0x15, CLUT-Y @0x800aca4d=0x16). Textfarben (Leon blau/Elza rot) sind IM Atlas gebacken, NICHT Laufzeit.
- Header „PLEASE SELECT MAIN CAST" ist im SELECTH.TIM-Background gebacken.
- **SELECT.TIM / SELECTH2.TIM = NICHT verwendet** (keine Referenz in TITLE.BIN). Meine frühere „3 Highlight-Varianten"-Annahme war FALSCH — Highlight = Pulse-Modulation der TILE-Rects, nicht TIM-Swap.

## Kamera / Zoom (0x80100a50) — ANIMIERT die SPRT-Overlays + Modell-Transform (Rotations-Patch)
- State-Machine `scene+0x15c`: 2=ARM, 3=RUN, 4=DONE. Von sub4 @0x80101424 auf 2 gesetzt.
- **Lerp = trunc((target-current)/40)**, EINMAL bei ARM berechnet, 40(41) Frames angewandt (Magic 0x66666667 = /40 round-to-zero, @0x80100ac4). Timer `scene+0x15e`=0x28.
- **Zoom-Ziel-Tabellen** @0x8010263c (Leon) / @0x8010264c (Elza), BEIDE identisch `[0x000B0000, 0x00140000, 0x00B00000, 0x00D00000]` (>>16: 11,20,176,208). **IDLE-Werte** im Byte-Table @0x80102624 (dies IST die Group-B-Geometrie-Tabelle — die 4 SPRT-Positionen werden vom Zoom animiert). Blocks: Leon blk0=scene+0x160, blk1=scene+0x19c; Elza blk2=scene+0x1d8, blk3=scene+0x214 (stride 0x3c).
- Werte sind 16.16, konsumiert als hi-halfword>>16 = SPRT-Pixel-XY. Die Blocks slidern die 2 Namens-Zeilen des GEWÄHLTEN Chars (Zeilen 0/2) — die Untertitel-Zeilen (1/3) werden NICHT gerendert (wie im Idle unsichtbar). Div = trunc/40 (`idiv40`, magic 0x66666667, >>4).

## ZOOM ist eine KAMERA-DOLLY, KEIN Scale/Sin-Cos-Patch (byte-true RE'd 2026-07-15)
- **Der visuelle Zoom = Kamera-Dolly.** sub6 (@0x801014f0) akkumuliert `scene+0x74` (= Kamera-pos.z, Idle -20000) quadratisch: `scene+0x74 += scene+0x04`, `scene+0x04 += 0xA` (Start 0x14=20 in sub5) über 41 Frames → accum ≈ 9020 → pos.z ≈ -10980. Jeden Frame ruft sub6 **`FUN_80053ca4(&scene+0x68)`** (= `setupCameraLookAtMatrix`, EXE @0x80053ca4, RE_15_Quellcode_V2) → baut die View-Matrix `DAT_800b5288` neu aus dem Kamera-Block (pos=scene+0x6c/70/74, target=scene+0x78/7c/80=(0,0,0)). Da nur pos.z sich ändert (target fix am Ursprung), bleibt R=diag konstant und **TR.z schrumpft** von 20039 → ~11000 (via `TR.z = R[8]·(-pos.z)/4096`) = **~1.82× Dolly-in**. Der Idle-Save bestätigt scene+0x68-Block = pos(0,0,-20000)→target(0,0,0) (= die Idle-View selbst wird von FUN_80053ca4 gebaut). **KORREKTUR:** die frühere „Sin/Cos-Rotations-Patch @0x800aca34"-Vermutung war FALSCH; die View-Matrix WIRD während Confirm modifiziert (der Workflow-Befund „View unverändert" galt nur für TITLE.BIN — der Rebuild ist EXE-seitig FUN_80053ca4).
- **PARALLEL** dazu pant sub6 das Modell selbst: `field[0x34]` (world-X) `-= idiv40(field[0x34])`/Frame → von ∓1568 nach ~0 (Zentrum); `field[0x6a]` (rot_y) `+= 8`/Frame (Spin). Zoom-in (Dolly) + Zentrieren (Pan) zusammen = der Charakter füllt den Screen.
- **PORTIERT** (Commit folgt): pc_run_player_select FSM cz=0..5, `re15_idiv40`, `re15_cz_fade_tick`, Kamera `cam.trans[2]=4104·(20000-accum)/4096`, Pan pan_x/pan_ry, sub4-Backdrop-Fade SUBTRAKTIV (ABR2), sub7 Fade-to-black. Video-verifiziert gegen shots/orig_confirm.avi (Fade/Pause/Zoom/Peak decken sich).

## Confirm-Commit + Handoff (sub7/handoff 0x8010166c, inner 5)
- Gated auf `FUN_8002178c()!=0` (Fade fertig). Dann: beide Model-Structs nullen; **`DAT_800aca5c = scene+0x394 << 2`** (0=Leon, 4=Elza) @0x801016ac = der Charakter-Commit; Elza-Flag Bit 0x80000000 in `DAT_800aca3c` setzen/clearen; `FUN_80043850(0x140,0xf0,0)`; **`FUN_80029ba4(a0=0x8001c958)`** @0x80101708 = Fade/Door-Transition-FSM → Spiel.

## Offene dynamische Punkte (Savestate nötig)
- Der exakte Sin/Cos-Rotations-Patch (@0x800aca34-Index) → wie die Block-A/B-Werte in die Modell-/SPRT-Transform-Matrix eingehen (Kamera-Rendering-Detail).
- CD-file-id 0x2c/0x44 → PL00/PL01-Zuordnung bestätigen (Port nutzt eh die PLD-Dateinamen).
- Idle-Clip-2-Frame-Count (aus PL00/PL01 clip-table zur Laufzeit).

## MODELL-KAMERA + TRANSFORM (byte-true, Workflow wf_0aab308c, savestate-verifiziert)

**Projektion (einmal in der Szenen-Prologue @0x8010109c, konstant):** `InitGeom()` (0x80066c40) + `SetGeomScreen(H=1000)` (0x80066c30, @0x801010a4 a0=0x3e8) + `SetGeomOffset(OFX=160, OFY=120)` (0x80066d60, @0x801010b0 a0=0xa0/a1=0x78). → `screen_x = 1000·Xview/Zview + 160`, `screen_y = 1000·Yview/Zview + 120`. (Zoom kommt aus der View-Matrix, NICHT H.)

**Globale View-Matrix @DAT_800b5288 (savestate sub_newgame.sav):** R = rows [4104,0,0][0,4096,0][0,0,4104] (≈Identity, 4104/4096=1.00195 auf X/Z; Q12), TR = (0,0,20039). = Kamera bei Welt (0,0,-20000), Blick +Z. (4104/20039 statt 4096/20000 = LookAt-Approx-Normalize-Overshoot; measured matrix verbatim nehmen.)

**Modelle (Actor-Base 0x800acc2c, stride 0x1f4; Loop FUN_80101bf0 idx0=Leon/idx1=Elza):**
- Leon PL00 @0x800acc2c: POS +0x34/38/3c = (-1568, 2036, 0)=(-0x620,0x7f4,0); rot SVECTOR +0x68 = {vx=0, vy=0x404, vz=0} (pure Y-Yaw, vy überlappt rot_y@+0x6a); part-count +0x83 = 17; Idle-Clip +0x94 = 2.
- Elza PL01 @0x800ace20: POS (+1568, 2036, 0); rot {0,0x404,0}; part-count 21; Clip 2.
- model-root MATRIX +0x20 = RotMatrix(+0x68) = [[-25,0,4096][0,4096,0][-4096,0,-25]] (cos(0x404)=-25, sin(0x404)=4096 via LUT DAT_800794c4); MATRIX.t überlappt POS (PsyQ MATRIX.t @+0x14 = actor+0x34).

**Transform-Kette (FUN_8001e8c8 → FUN_8001e9ec pro Part):** RotMatrix(+0x68 → +0x20 mit t=POS) → bone-compose(part+0x6c parent = entity+0x20 für root, part+0x18 local → part+0x40 world, FUN_80022da0) → compose(globale viewMat 0x800b5288 × part+0x40 → sp+16, FUN_80022da0) → SetRotMatrix(sp+16) + SetTransMatrix(sp+16.t) → gte_RotTransPers pro Vertex. Lighting FUN_80053fc0(POS) + SetLightMatrix(0x80076d14) + SetColorMatrix(0x80076d34, alle 2048=0.5). Idle: ScaleMatrix SKIP (flag bit 0x800 aus).

**Port-Plan:** cam_view.rot = [4104,0,0,0,4096,0,0,0,4104], cam_view.trans = (0,0,20039), cam_view.fov_screen_dist = 1000, Center (160,120). 2 Actors an POS ∓1568/2036/0, rot_y 0x404, motion=clip 2 → re15_skel_compute_pose → re15_camera_compose_view_bone → PROJECT_VERT → queue_textri (die vorhandene Actor-Mesh-Render-Kette).
