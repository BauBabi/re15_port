# Skeptiker-Pruefung: objekte-paket.md (Runde 16, 2026-09-19)

Auftrag: die Kernbehauptungen des Dossiers `objekte-paket.md` (Abschnitt 0/2/3, Messwerte) statisch
(eigene Disassembly, eigene RDT-Parses) und dynamisch (Dossier-Sonde + eigene Gegen-Sonde in
eigenem Build-Verzeichnis `re15_port/build_r16_sk_objekte-paket`) zu widerlegen.

Ergebnis vorab: **keine der sieben Kernbehauptungen faellt.** Zwei Formulierungs-Unschaerfen und
drei unbelegte Annahmen im Fix-Plan (Abschnitt 4), keine davon tragend.

## 1. Tabelle

| # | Befund (Dossier) | haelt / widerlegt | Beleg (eigene Messung / eigene Adressen) |
|---|---|---|---|
| 1 | 2.1 Item_aot_set (LAB_80040644) testet das Taken-Bit in Zone 9 und schreibt `pool[tk_prop].flags = 0x80000000` @0x80040718; Pool @0x800b3f98, Schritt 148 | **haelt** | `re15_disasm.py dis 0x80040644 70`: `800406d8 lw a0,18056(a0)` (0x80074688, Inhalt gelesen = 0x800B1078 = Zone 9, Port-Mapping game_state.c:106 `[9] DAT_800b1078`); `800406dc jal 0x8004efe4` (= `srl v1,a1,5; ... srlv v0,0x80000000,a1&0x1f; and` = Bit-Test); `800406e4 beq v0,zero,0x8004071c`; `800406e8 lui v1,0x8000`; `800406f4 sb zero,0(v0)` (Record.sce=0); `800406f8..80040708` = s1*148; `80040710 addiu at,at,16280` (0x800b3f98); `80040718 sw v1,0(at)`. Unschaerfe: das Dossier schreibt "Bit = Record+4"; tatsaechlich `80040680 lhu a1,18(a2)` (Kurzform) / `80040670 lhu a1,26(a2)` (Langform, `andi 0x80` @0x80040664 auf pc[3]) = pc+18/pc+26 — exakt das, was der Port liest (scd_vm.c:3667). tk_prop = `lbu s1,20(a2)`/`28(a2)`. |
| 2 | 2.2 FUN_8003ea7c nullt den Pool bei jedem Raumladen (@0x8003eab0-cc, 32 Eintraege, Schritt 148) und wird aus FUN_800396fc @0x800399a0 gerufen; es gibt keinen zweiten Speicherort | **haelt** | `dis 0x8003ea7c`: `8003eab4 addiu at,at,16280; 8003eab8 addu at,at,v1; 8003eabc sw zero,0(at); 8003eac0 addiu s0,s0,1; 8003eac4 sltiu v0,s0,0x20; 8003eac8 bne; 8003eacc addiu v1,v1,148`. Xrefs (ghidra1_V2.txt:148489): FUN_8003ea7c hat GENAU EINEN Caller `FUN_800396fc:800399a0(c)`. FUN_800396fc hat genau einen Caller `8001d988` in FUN_8001d600 (ghidra1:95878), und FUN_8001d600 (beide Zweige: Debug-JUMP `DAT_800ac9a8==0` und Tuer-Record) ruft FUN_800396fc unbedingt (Decompile FUN_8001d600.c:51); FUN_8001d600 selbst hat einen Caller `8001ca54` (Raumwechsel-FSM). Ordnung: die SCD-Ausfuehrung (FUN_8003ef6c -> FUN_8003ee3c(0/1) mit `DAT_800b3f70 = RDT+0x40/+0x44`) liegt in FUN_800396fc NACH dem Pool-Clear (Decompile :59 vs :70). Schreib-Xrefs auf DAT_800b3f98 (ghidra1:506190-506203): `(W)` nur FUN_8003ea7c:8003eabc; die `(*)`-Referenzen 80040718/80040978/8004098c sind die im Dossier genannten; `8003850c(R)` ist ein reiner Leser (`lw v0,0(a2); andi 0x1`, Kollision). |
| 3 | 2.3 Obj_model_set (LAB_80040914): flags==0 -> `pc[6..7]|1`, sonst -> 0 (AUS) | **haelt** | `dis 0x80040914`: Index = `lbu a3,1(a2)` (obj_id) *148 @0x80040944-58; `80040978 lw v0,0(a1); 80040980 beq v0,zero,0x80040990; 8004098c sw zero,0(a1)`; Else-Pfad `80040990 lhu v0,6(a2); 80040998 ori v0,v0,0x1; ... 800409a4 sw v0,0(a1)`. |
| 4 | 2.1 Zeichner FUN_8002c18c zeichnet nur bei `flags & 1`, Schleife bis nOmodel; Live-Aufnahme FUN_80021eb4 @0x80021fc8 nullt dasselbe Wort | **haelt** | `dis 0x8002c1e8`: `8002c1f0 addiu v0,v0,16280; 8002c1f8 addiu s6,s6,148; 8002c1fc lw v0,0(s1); 8002c204 andi v0,v0,0x1; 8002c208 beq v0,zero,...`; Decompile FUN_8002c18c.c: `bVar1 = *(DAT_800ac778+2)`, `while (uVar5 < bVar1)`. `dis 0x80021f90`: `80021fa0 lbu v0,6(s0)` (s0 = Record+12/+20 -> tk_prop) *148 -> `80021fc0 addiu at,at,16280; 80021fc8 sw zero,0(at)`, danach `80021fd4 addiu a0,a0,4216` (0x800b1078) `jal 0x8004ef90` (Flag setzen). |
| 5 | 3. Port: `s_prop_taken_hidden` (scd_vm.c:414) nur in `scd_vm_init` (:419) genullt, gesetzt in op_item_aot_set (:3693, Slot-Hide :3695-96), angewandt in op_obj_model_set (:3803-04); `scd_room_reenter` memsettet nur g_scd (scd_room_setup.c:211); `scd_vm_init` nur beim Boot (main.c:3472) | **haelt** | grep `s_prop_taken_hidden`: genau die Zeilen 414/419/3693/3804/3806 — kein weiterer Schreiber. `scd_vm_init(` Aufrufer: platform/pc/main.c:3472, platform/psx/main.c:121 (Boot). `memset(&g_scd, 0, sizeof(g_scd))` = scd_room_setup.c:211. `scd_room_reenter(` Aufrufer: room_common.c:347 (Tuer), game_step_common.c:1824 (Self-Reenter), main.c:4247 (Fallback) — die "drei Ladewege" stimmen. Zeile 3695-3696 `if (tk_prop < g_scd.prop_count) g_scd.props[tk_prop].active = 0` adressiert tatsaechlich den Slot. |
| 6 | 1.1 RDT-Records: 1050 obj 0x01 main00 @0x0BB0 / obj 0x00 sub00 @0x0C36; 1090 obj 0/1 Typ 4 @0x216E/@0x2190; 11F0 obj 2..0xB sub00 @0x0E76..0x0FA8, Cursor obj 0 @0x0E54 Typ 4, obj 1 main00 @0x0D10; 1000 Items sub00 @0x0C3A/@0x0C50 tk_bit 166/167 tk_prop 0/1; 1010 @0x0996/09AC/09C2 tk_bit 140/141/142 tk_prop 0/1/2; Sce_em_set Ada 0x42 @1050:0x0C8E, @1090:0x22CC | **haelt** | Eigener Parse (shared_assets/PSX/STAGE1): 1050 nOmodel=2, main@0xAD8 sub@0xC10; `0BB0: 2d 01 00 ... pos (14775,0,-10125) ry 1199`; `0C36: 2d 00 ... (15432,0,-10424) ry 2048`. 1090 nOmodel=4; `216E: 2d 00 type4 band1 (-8136,-1800,4753)`; `2190: 2d 01 type4 band5 (-5960,-9000,-15482)`. 11F0 nOmodel=12; `0D10 obj01 (11040,-16200,-15084)`; `0E54 obj00 type4 (-19554,0,22684)`; `0E76 obj02 (-25975,10,26000)`; `0E98 obj03`; `0FA8 obj0B (-18775,10,17800)`. 1000: `0C3A: 50 04 09 31 ... 24 00 01 00 a6 00 00` (tk 166, prop 0), `0C50: ... 25 00 01 00 a7 00 01` (tk 167, prop 1). 1010: tk 140/141/142, prop 0/1/2, Items 0x22/0x15/0x16. `1050@0C8E: 44 00 42 40`, `1090@22CC: 44 00 42 40 01` (Opcode 0x44 = SCD_OP_SCE_EM_SET, Typ 0x42). |
| 7 | 1.4 Sonde: 5 Abweichungen (B, C x3, D), rc=1; E gruen | **haelt** | Eigener Build `re15_port/build_r16_sk_objekte-paket` (GCC 16 mingw64, Ninja): `probe_r16_objekte_paket.exe` liefert byte-gleich `[1050 nach 1000] obj01=aus obj00=aus`, `[11F0 nach 1010] obj01/00/02=aus, 03..0B=AN`, `[1090 nach 1000] obj00=aus obj01=aus obj03=AN`, `DEFEKT: 5 Abweichung(en)`, rc=1. Gegen-Sonde `probe_r16_sk_objekte_paket.exe` (rc=0, 0 Widerlegungen): S2 reproduziert; **S3 Isolation**: identische Kette, aber `scd_vm_init()` (+ Flags erneut gesetzt) zwischen 1000 und 1050 -> `obj01=AN obj00=AN` — die Maske ist die alleinige Ursache, nicht g_aot/Flags/Renderer; S4: 1050 vor dem Item-Raum AN, danach dauerhaft AUS (klebt); S5: ohne Taken-Bits bleibt 1000->1050 AN; S6: nur Bit 166 -> nur obj 0 AUS, obj 1 AN (Bit-Kopplung obj_id<->tk_prop). |
| 8 | 1.5 Zensus der tk_prop-Indizes in STAGE1 | **haelt** | Eigener Scan (op 0x50, sce 9, sat 0x31/0x51, tk_bit!=0, tk_prop<32): tk_prop 0: 1000/1010/1011/1070/1071/1110/1111/11D0/11D1/1220/1221; 1: +10F0/11B0/11F0; 2: 1010/10F0/1110/11B0/1220 (+x1); 3: 10F0/1110; 4/5: 1020; 6..14: 1190 — deckungsgleich mit dem Dossier (das die x1-Varianten weglaesst). |

Nicht selbst nachgemessen (kein Widerspruch, aber ausserhalb meiner Mittel in dieser Runde):
1.2/1.3 mit der Paket-exe (SHA-Gleichheit Paket/win_out/wxbuild, Framedumps). Die Sonden-Ebene
(Befund 7) zeigt denselben Zustand `active=0`, den das Dossier fuer `0 [prop-render]` verantwortlich
macht; ein Renderer-seitiger ZUSAETZLICHER Defekt ist damit nicht ausgeschlossen, aber nichts im
Dossier haengt davon ab.

## 2. Formulierungs-Unschaerfen (keine Widerlegung)

- 2.1 "Flag-Test(Zone, Bit = Record+4)": Das Bit kommt aus `lhu a1,18(a2)` bzw. `26(a2)` (pc+18 / pc+26
  in der 0x80-Langform), nicht "Record+4". Port und Dossier-Sonde benutzen die richtigen Offsets.
- 4.5 nennt `probe_item_model_1020` einen "bestehenden Pin": es gibt fuer diese Sonde kein `add_test`
  (tests/unit/CMakeLists.txt:3009-3010 nur add_executable). Sie ist Diagnose, kein Pin.

## 3. Fix-Plan (Abschnitt 4): Annahmen ohne @0x/Messung

1. **4.2 Self-Reenter-Pfad (game_step_common.c:1824)**: Der Plan setzt die Maske in JEDEM
   `scd_room_reenter` zurueck, also auch beim Same-Room-Reenter, den der Port-Kommentar dort als
   "No asset reload (same room)" modelliert. Im Original geht auch eine Selbst-Tuer durch die
   Raumwechsel-FSM (`8001ca54 jal FUN_8001d600` -> FUN_800396fc -> FUN_8003ea7c), also wird der Pool
   dort ebenfalls genullt — das ist im Dossier nicht ausgesprochen, folgt aber aus Befund 2 (ein
   einziger Ladeweg). Kein Risiko, aber der Plan begruendet es nicht.
2. **4.5 "probe_item_model_1020 bleibt erhalten"**: unbelegte Annahme (nicht gemessen) und die Sonde ist
   ohnehin kein Pin (s. Abschnitt 2). Vor dem Commit einmal laufen lassen.
3. **4.6 "Risiko: gering — die Maske war eine reine Port-Konstruktion (2026-08-25)"**: gestuetzt nur auf
   den Code-Kommentar scd_vm.c:405-413, nicht auf eine Messung. Konsistent mit dem Original (Befund 1/3
   decken den Reihenfolge-Fall: 0x80000000 im Pool -> Obj_model_set schreibt 0).
4. **Nicht im Plan**: die Live-Aufnahme (FUN_80021eb4 @0x80021fc8, Befund 4) nullt im Original das
   Pool-Wort direkt; der Port setzt die Maske NUR in op_item_aot_set (einziger Schreiber, Befund 5).
   Ob der Port beim Aufnehmen den Prop ueber einen anderen Weg versteckt, prueft das Dossier nicht —
   fuer den gemeldeten Defekt irrelevant, fuer die Abnahme "Item weg nach Aufnahme im selben Raum"
   aber die zweite Haelfte.
5. **5. Offen**: Kein DuckStation-Gegenlauf. Der statische Beweis (Befund 2: ein Caller, Pool-Clear vor
   SCD-Start, kein zweiter Writer) ist geschlossen; ich sehe keinen Mechanismus, ueber den das Original
   die Sichtbarkeit raumuebergreifend haelt.

## 4. Werkzeuge dieser Pruefung

- `re15_port/tests/unit/probe_r16_sk_objekte_paket.c` + `tests/unit/probes/r16_sk_objekte-paket.cmake`
  (Gegen-Sonde S1-S6, kein add_test).
- Build: `re15_port/build_r16_sk_objekte-paket` (eigenes Verzeichnis, SDL2-Quelle aus build/_deps).
- Disassembly: `python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80040644 70`,
  `dis 0x8003ea7c 40`, `dis 0x80040914 60`, `dis 0x8004efe4 20`, `dis 0x80021f90 20`, `dis 0x8002c1e8 10`,
  `dis 0x800384e8 24`, `read 0x80074688 1`.
- Ghidra-Xrefs: ghidra1_V2.txt:148489 (FUN_8003ea7c), :95878 (FUN_800396fc-Caller), :95647 (FUN_8001d600-Caller),
  :506190-506203 (DAT_800b3f98).
