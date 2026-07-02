# RE1.5 Room-Correctness Backlog — falsch „byte-true" portierte Mechaniken

Vom Nutzer beim GUI-Test gefunden (2026-07-02). Regel: **jeder Fix byte-true belegt** (Overlay/Ghidra/
Savestate-Adresse oder Datei-Offset), Mechanismus beweisen — „sieht richtig aus" zählt nicht. Einer zu
100% fertig, bevor der nächste dran ist. Status: ⬜ offen / 🔎 in RE / 🔧 fix / ✅ verifiziert.

---

## 1. ROOM 1240 — Intro-Text: Volltext statt Typewriter + Balken über Text  ✅ (1A [FL-observed], 1B [BT])
- **Bug A (KORRIGIERT — Richtung war verkehrt):** Das Original zeigt das 1240-Pre-Intro als **VOLLTEXT**
  (kompletter Block), der Port **typte** es. Ursache: der Narrator (sub02, 6× `Message_on(0x2B)` msg 0..5)
  lief durch den Typewriter-FSM, weil `re15_room_has_voice` nur 0x1170 kannte → 1240 in den else-Zweig.
  **Fix (`scd_vm.c` op_message_on):** neues `re15_room_full_text() = {0x1170,0x1240}` → Volltext-Pfad
  (`msg_show`/legacy `message_display_frames`); Voice bleibt auf {0x1170} (1240 hat keine VOICE##.VAG).
  **[FL-observed]:** der statisch sichtbare EXE-Printer (0x2B→LAB_800404f4 hardcodet 0x300→FUN_80027e68
  setzt DAT_800b8521=0 bedingungslos→FUN_80028134) ist **byte-true Typewriter-only** — Volltext kommt aus
  einem nicht in ghidra1_V2.txt sichtbaren Weg (**DEBUG.BIN @0x800C0000**, savestate-only). Basiert auf der
  Nutzer-Beobachtung. **RESIDUAL zum byte-true-Schließen:** 1240-Pre-Intro-Savestate ziehen, DAT_800b8521/
  8524/852c über Frames lesen (springt das Fenster [start==full] in 1 Frame → echter Volltext-Printer →
  DEBUG.BIN-Overlay disassemblieren; sonst schneller Typewriter). Workflow: wf_f427c5e9-e8c.
- **Bug B [BT]:** Letterbox-**Balken lagen ÜBER dem Text**. **Fix (`render_pc.c` end_frame):** Balken jetzt
  VOR dem Text-Overlay → Untertitel sitzt auf dem schwarzen Balken (Text über Balken), byte-true zum
  Original (Letterbox POLY_F4-Quads `FUN_80020f8c` im OT hinter den Text-Primitiven).

## 2. ROOM 1240/Pre-Intro — Hintergrund  ✅ (verifiziert gegen Original, KEIN Code-Change nötig)
- **Ursprüngliche Beobachtung:** Port zeigt ROOM1170-Kisten während des Pre-Intro-Textes statt des Intros.
- **Byte-true RE (statisch):** ROOM1240s Narrator = `sub[2]` (subScdStart @RDT 0x53c, off 0x1e) macht
  **9× Cut_chg (cut 0→8) + 6× Message_on**, cycled also durch ROOM1240s EIGENE Backgrounds
  **BG00–BG08** (exakt 9 Dateien vorhanden). SCD-Disasm bestätigt (scd_dis.py). Cut 0=schwarz,
  1=Zombie, 2–4=T-Virus-Zellen, 5–7=S.T.A.R.S.-Logo, 8=Heli/Umbrella.
- **Ursache der Kisten:** Der Boot-BG-Load (`re15_bg_load_cut(0)` @main.c:427) läuft VOR
  `g_current_room_id = boot_room` (@main.c:457) → nutzt den Default `0x1170` (room_common.c:31).
  VOR dem Boot-Fix (a883a93f) lud ROOM1240s RDT nicht → sub[2] lief nie → kein Cut_chg → der
  ROOM1170-Boot-BG blieb stehen = die Kisten. Der Boot-Fix hat ROOM1240s SCD zum Laufen gebracht
  → sub[2]s Cut_chg(0) lädt sofort ROOM1240 BG00 → #2 als Seiteneffekt mitgefixt.
- **Dynamische Verifikation (DuckStation, selbst gecaptured 2026-07-02):** Original-Pre-Intro
  Frame-für-Frame gegen den Port verglichen (`shots/COMPARE_port_vs_original.png`,
  `shots/contact_ORIGINAL.png`): **Port == Original** — beide zeigen schwarz→Zombie→T-Virus→
  S.T.A.R.S.→Heli→Umbrella-Logo mit dem Text DARÜBER, alle 6 Captions, danach Handoff zu ROOM1170.
  Die „pur schwarz"-Beobachtung = der lange Schwarz-Vorlauf (in BEIDEN da). Die Kisten erscheinen
  im Port jetzt korrekt ERST NACH dem Intro (Helipad-Gameplay f3180+).
- **Rest:** main.c:424 Kommentar („boot directly into ROOM1170 cut 0 BG") war irreführend → korrigiert.

## 3. Waffe/Inventar — Modell + Anheben + Schuss-Anim + Effekte falsch  ⬜
- Beim Waffen-Auswählen wird das **Waffenmodell an Leon (außerhalb des Inventars) nicht geladen**.
- **Waffe anheben** spielt die **falsche Animation**.
- **Schießen** spielt die **falsche Animation**.
- **Effekte fehlen** (Mündungsfeuer/Treffer?) — evtl. mit den obigen Punkten gekoppelt.
- (Item-Menü-Submenüs sind separat unvollständig — siehe der laufende Submenü-Map-Workflow.)

## 4. ROOM 1140 — Zombie-Trigger + falsche Anim + Hänger  ⬜
- Zombies starten **korrekt** (fressend am Boden). ✅
- Sobald der Spieler den **Trigger-Punkt des ERSTEN Zombies** erreicht, stehen **SOFORT ALLE** Zombies
  und spielen eine **falsche „getroffen"-Animation** (sollten einzeln/anders reagieren).
- **Biss** spielt die **falsche Animation**.
- **Ab einem bestimmten Punkt hängt das Spiel** (Freeze).
- Betrifft die portierte Zombie-AI (@0x8011f7b4-Dispatch, wake/engage/hurt-States) — Verdacht:
  Wake-Trigger weckt global statt per-Zombie, + falsche Clip-Zuordnung, + ein State ohne Exit (Hang).

## 5. ROOM 1070 — Irons-Office-Cutscene endet nicht  ⬜
- In der Irons-Cutscene **hängt Leon am Ende**, die **Cutscene endet nicht** (kein Exit/Handoff).
- Verdacht: fehlender/fälschlicher Cutscene-End-Trigger oder ein Clip/Event ohne Terminierung.

---

### Reihenfolge / Notizen
- Präsentation (1,2) vs. Combat-AI (4) vs. Cutscene-Ende (5) vs. Waffen-Pipeline (3).
- **Hänger (#4, #5) sind funktional am schwersten** (Freeze) — ggf. vorziehen, falls der Nutzer will.
- #1+#2 teilen sich evtl. das Intro-Präsentations-Subsystem; #3 koppelt an Combat/Effekte (#4).
