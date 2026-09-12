# BEFUND C — Gruppe „Abtrennen" (Gore-Kette hinter den Trefferzonen)

Nutzer 2026-09-12: „die Brutalitaet — das Abtrennen der Koerperteile — funktioniert noch nicht
wie in Resident Evil 2." Dieses Dossier klaert (1) den IST-Zustand des Ports, (2) die
Original-Mechanik (RE2 EMZ0.BIN, selbst gedumpt/disassembliert), (3) den Plan fuer die vier
bekannten Blocker.

Alle Tabellen-Dumps in diesem Dossier sind FRISCH gezogen mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py {table|dis|bytes} … --bin EMZ0.BIN`
(Overlay laedt RAW @0x80100000) bzw. gegen `info/re2leon/PSX.EXE`.

---

## 0. Unter welchem Flavor laeuft der Nutzer?

* Der AI-Schalter hat DREI Zustaende (platform/pc/main.c:2015-2021):
  `0 = RE1.5`, `1 = RE2 (RE2-Gehirn + RE1.5-Modelle, Hybrid-Rig)`, `2 = MIXED`.
* **Default = RE2** — `static re15_ai_flavor_t s_flavor = RE15_AI_FLAVOR_RE2;`
  (enemy_ai_re2_zombie.c:102, Nutzer-Entscheidung 2026-08-22 woertlich im Kommentar).
  Der Zustand wird NICHT persistiert (kein Save/Load des AI-Modus in main.c) — jeder
  Spielstart beginnt also im RE2-Flavor, ausser der Nutzer stellt im Menue um.
* Konsequenz: Der Nutzer sieht im Normalfall das RE2-Gehirn auf RE1.5-MODELLEN
  (`re2_hybrid_apply`, re2_ems.c:323-375). Spielt er RE1.5-AI, greift stattdessen die
  Import-Bruecke `re15_re15_re2z_gore_hit` (enemy_ai_re2_zombie.c:6145, Option
  `RE15_RE15_RE2Z_IMPORT`, Default AN, enemy_ai_re2_zombie.c:215-236).

Die Trefferzonen-Neuerung von heute (`+0x1D2` aus der Ziel-Elevation,
re15_damage.c:1698-1701) laeuft NUR im RE2-/MIXED-Flavor: `re15_re2_stamp_hit` verlaesst
sich auf `re15_ai_re2_for_type(e->type)` (re15_damage.c:1662) und kehrt im RE1.5-Flavor
sofort um. Die RE1.5-Bruecke stempelt weiterhin HART `e->re2z_hits1d2 = 1u`
(enemy_ai_re2_zombie.c:6169) — im RE1.5-Flavor gibt es also bis heute KEINE Kopf-/Bein-Spalte.

---

## 1. IST — was der Port hat, was gated ist, warum der Pin gruen ist und der Nutzer nichts sieht

### 1.1 Implementiert (vollstaendig, byte-true zu RE2, in enemy_ai_re2_zombie.c)

| Baustein | Ort (Port) | Original |
|---|---|---|
| Modellblock +0x198 (16 Part-Records, Flags/Tinte/Mesh/Wurf) | `re15_re2z_part_reset` :3928 | Stride 0xAC, Belege @0x8010627C-F4 u.a. |
| Anzeige-Bruecke (Draw-Enable, Eltern-Kaskade, Tinte, Stumpf-Mesh) | `re15_re2z_gore_resolve` :4101 | FUN_80027160, `andi 0x1` @0x8002737C, `andi 0x21/li 0x20` @0x80027480-94 |
| Freifliegendes Teil (eigene Matrix + zwei Physiken) | `re15_re2z_gore_part_matrix` :4312, `re2z_part_phys_ad8/_dac` | Bit 0x40 @0x80027498; FUN_80028AD8 / FUN_80028DAC |
| DER ZERLEGER (Bein ab im HURT) | `re2z_leg_gore` :4496 | @0x80105288-3D8, Stumpf `part_mesh=15` @0x8010531C-50 |
| Knockdown-Arm-Abriss | `re2z_knockdown_gore` :5929 | @0x80107514-708 |
| Zerreiss-Tod (Kopf + Bein(e) + Rumpf-Stumpf) | `re2z_death_rip` :5251 | FUN_80108BEC, Stempel @0x80108DD4-FDC |
| Magnum-Tod (Kopf platzt) | `re2z_death_magnum` | FUN_801092C4, Kopf @0x80109460-98 |
| Wegschleudern + Fleischbrocken | `re2z_death_burst` :5000 | FUN_80109610 |
| Renderer-Konsument (Draw/Tint/Mesh-Tausch/Flug-Matrix) | platform/pc/main.c:8166-8290 | s. dort, jede Zeile mit @0x |

### 1.2 Die vier Blocker — verifiziert am heutigen Stand

**(1) Todes-Gore blockt im RE1.5-Flavor.**
`re15_re15_re2z_gore_hit`: `if (e->state == 3) return;` (enemy_ai_re2_zombie.c:6149).
Begruendet mit „(c) Der DEATH-Zweig @0x80108250 hat seinen eigenen Gore" (:6127) — nur:
im RE1.5-Flavor LAEUFT diese DEATH-Wurzel nie (das RE1.5-Gehirn enemy_ai_common.c uebernimmt
den Tod), es gibt dort also GAR KEINEN Todes-Gore. Genau dieser Zustand ist sogar gepinnt:
test_re2_zombie_teardeath.c PIN 8 (:643-672) prueft, dass im RE1.5-Flavor NICHTS abgerissen
wird („PIN8: RE1.5-Modus hat Part %d abgerissen — REGRESSION"). Der Pin ist als
Positiv-Beleg des ALTEN Bauzustands gruen — nicht als Beleg, dass der Nutzer Gore sieht.
Nutzer-Auftrag 2026-08-20 (woertlich im Optionsblock :192-196): Gliedmassen-Abriss soll AUCH
in RE1.5 AI — PIN 8 pinnt also inzwischen das Gegenteil des Auftrags (Fixture-Verschiebung
faellig, s. Plan).

**(2) `re2_hybrid_apply` ersetzt das RE2-MD1 → Stumpf-Geometrie weg.**
re2_ems.c:372 `eb->md1 = *md15;` — das RE2-MD1 (EM010: 17 Meshes bei 15 Bones, Meshes 15/16 =
Gore-Reserven) wird durch das RE1.5-MD1 (mesh_count == bone_count == 15, selbst gemessen fuer
EM10/11/12/13/16/18, Kommentar enemy_ai_re2_zombie.c:6138-6141) ersetzt. Der Zerleger stempelt
`part_mesh = 15` (@0x8010531C-50) bzw. `part_mesh[0] = 16` (@0x80108FB8-FDC); die Klammer
`gore_mesh[nbi] < mesh_count` (platform/pc/main.c:8283) klemmt beides weg → Oberschenkel und
Rumpf bleiben INTAKT, obwohl Schienbein/Fuss bzw. Kopf wegfliegen. Betrifft **beide**
Nutzer-Modi: das RE2-Flavor-Hybrid UND den RE1.5-Import (dort ist ueberhaupt keine RE2-Bank
geladen). Der gruene Pin misst dagegen die PURE RE2-Bank („GELADENE RE2-Bank EM010, 17
Meshes", test_re2_zombie_teardeath.c:41) — deshalb Pin gruen, Spiel ohne Stumpf.

**(3) `re2z_gore_fx` verwirft `packed_id`.**
enemy_ai_re2_zombie.c:3916-3921: `(void)packed_id; re2z_blood_fx_at(e, part, rot_y);`
Jeder RE2-Gore-Emitter (FUN_8001BF10, 96 Slots @0x800D8CF0) wird auf EINEN generischen
RE1.5-Raumbank-Blutspawn reduziert. Verloren gehen damit die unterscheidbaren Effekte:
Fontaene 8000, Spray 3000/6000/7000, Landeklecks 2000, Brand-Tinten-Emitter 0x0503xxxx,
Aetz-Emitter 0x040Fxxxx, FLEISCHBROCKEN `0x08000000|((r+4000)*4)` (@0x8010902C-44). Das ist
der Hauptgrund, warum selbst die funktionierenden Zellen (Burst, Magnum-Kopf) „nicht wie
RE2" aussehen.

**(4) FUN_8001CEFC = No-op + fehlende Kinder-Kaskade im FLUG.**
FUN_8001CEFC(gruppe, sub, anker) loescht im Original alle FX-Slots am abgerissenen Teil
(`lbu 28/lbu 30/lw 108/sb zero,0` @0x8001CF14-4C); der Port hat keine ankergebundenen FX →
No-op (Blockkopf :3907-3912). Und: im Original haengt jedes Kind ueber `rec+0x74` an der
MATRIX des Eltern-Records (FUN_80028368.c:106-109) — ein wegfliegender Oberschenkel nimmt
Schienbein+Fuss im FLUG mit. Der Port posiert die Kinder aus der Bank-Skelettkette
(Kommentar „Grenze (2)" :4941-4948): Schienbein/Fuss bleiben an der Leiche stehen, bis die
LANDE-Kaskade greift (Bit-0-Loeschung @0x80028CA0 → `(Eltern&0x21)==0x20` → Kind unsichtbar,
re15_re2z_gore_resolve :4139-4144). Sichtbares Symptom: abgerissene Teile „teleportieren"
statt zusammenhaengend zu fliegen.

### 1.3 Der NEUE fuenfte Blocker (heute eingebaut): die Elevation-Spalte zeigt in NULL-Zellen

re15_damage.c:1698-1701 stempelt seit heute `+0x1D2 = UP→2 / LEVEL→1 / DOWN→0`.
Fuer die SCHROT-Zeilen ist die Kopf-Spalte im Original aber LEER (Beweis §2.1) — und der
Dispatch hat KEINEN NULL-Wächter. Folgen im Port (RE2-Flavor, Schrot W8 → Zeile 7):

* **HURT + Kopf (col 2):** `re2z_row_guard` (:3676-3683) weicht bei NULL auf Zeile 8 aus,
  aber `re2z_hit_tbl[8][2]` ist AUCH 0 → Dispatch-default → `0x101` zurueck in den Gang
  (:6363-6368). **Ein ueberlebter Kopf-Schuss mit Schrot erzeugt seit heute GAR KEINE
  Reaktion.**
* **DEATH + Kopf (col 2):** `re2z_death_tbl[7][2] = NULL` → Port-Fallback
  `re2z_death_crawler` (:6900-6903). **Wer mit der Schrotflinte auf den Kopf zielt, bekommt
  den SCHWAECHSTEN Tod (Kriecher-Konvulsion)** — level gezielt gaebe es Burst/Fleischbrocken.
  Die natuerliche Nutzer-Geste „auf den Kopf halten = Brutalitaet" produziert also seit heute
  WENIGER Gore als vorher. Das erklaert das heutige „funktioniert noch nicht"-Feedback
  unmittelbar mit.

### 1.4 Warum die Bein-Abtrennung mit Schrot praktisch nie feuert

Zerleger-Gate (@0x80105288-B0, Port :4499-4501): `+0x21A&0x60==0` UND `pool152 < 0` UND
`+0x1D0&0xC0` (Seitentreffer). Die Reserve startet bei 13 (@0x8010081C); Schrot-Kosten =
`re2z_pool_cost_w1[7] & 7` = `0x078EFC0A & 7` = **2** pro Rumpf-Treffer (:6051-6058,
Formel `>>bracket*3 &7` @0x80041954-70) → 7 Treffer noetig. Die Schrotflinte toetet mit
Bracket-0-Schaden 200 (Teilbox-Zensus re15_damage.c, Zeile 7 = 200/60/40) einen Zombie
(HP-Wuerfel-Tabelle 0x8010C670, gemessen hp0 75/80) aber in 1-2 Treffern. **Der
Pump-Schrot-Gore kommt im Original fast ausschliesslich aus den DEATH-Zellen, nicht aus dem
Zerleger** — Pistolen (Kosten 4, viele Treffer) sind die Bein-Abriss-Waffe.

---

## 2. ORIGINAL RE2 — die Abtrenn-Zellen fuer Schrot + Kopf/Bein

### 2.1 Die Tabellen, frisch gedumpt (EMZ0.BIN)

Spaltenindex `+0x1D2 = Zone + 3*Bracket`; Zone 0=BEINE, 1=RUMPF, 2=KOPF
(Applier @0x80047294-330; Hitscan `3*s1+s7` @0x80041A6C-9C). Der Port erreicht mit
Bracket 0 die Spalten 0/1/2.

**HURT @0x8010C940, Zeile 7 (Schrot) @0x8010CA3C** (eigener Dump, s. oben):

| col | 0 (Beine) | 1 (Rumpf) | 2 (Kopf) | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|---|
| Zelle | 0x80107438 | 0x801066FC | **0x00000000** | 7438 | 5BC0 | **0** | 7438 | 5438 | **0** |

**DEATH @0x8010CC24, Zeile 7 (Schrot) @0x8010CD20:**

| col | 0 (Beine) | 1 (Rumpf) | 2 (Kopf) |
|---|---|---|---|
| Zelle | 0x80107438 (Knockdown-Tod: **Arm ab**/Kriecher) | 0x801066FC (Ragdoll-Tod: **Burst**/Kriecher) | **0x00000000** |

**DEATH Zeile 8 (SPAS, RE1.5-W13) @0x8010CD44:** col0 = 0x80107438, col1 = **0x80108BEC
(VOLLER ZERREISS: Kopf + Bein(e) + Rumpf-Stumpf)**, col2 = **0x00000000**; col4 = 0x80109610.

**DEATH Zeile 9 (GL Explosiv) @0x8010CD68:** col0 = 7438, col1 = **8BEC**, col2 = **8BEC** —
die EINZIGEN Zeilen mit belegter Kopf-Spalte sind die Granat-Zeilen 9-11 (ihr Erzeuger ist
der AoE-Applier FUN_800470C0 mit echter Schuss-Y-Regel @0x800472D8-30C).

**Der Dispatch hat KEINEN NULL-Waechter** (selbst disassembliert):
HURT @0x801053E0-414: `lbu v1,5(a0) … lbu v1,466(a0) … lw v0,0(v1) / jalr v0` — kein
`beq v0,zero`. DEATH @0x801084E0-518 wortgleich (`jalr v0` @0x80108514). Eine NULL-Zelle
waere `jalr 0` = Absturz → **die Kombination Schrot+Kopf-Spalte ENTSTEHT im Original nie.**
Der Hitscan-Zonen-Erzeuger (Maskentabelle 0x800A6DB4, `lbu v1,2(a0) / and v0,v1,s6 /
srl s7,v1,1` @0x80041488-9C; Bytes {04,02,01,02,01,04,…} selbst gedumpt) ist waffendaten-
getrieben; welche Zeile der Tabelle die Schrotflinte adressiert, ist nicht zu Ende RE'd —
die NULL-Spalten BEWEISEN aber datenseitig, dass Zone 2 fuer die Zeilen 1-8/12-18 nie beim
Dispatch ankommt. **Kopf-Gore laeuft bei Schusswaffen ausschliesslich ueber die
Rumpf-Spalte der DEATH-Tabelle** (Zeile 5/6 → 92C4, Zeile 8/17 → 8BEC), nicht ueber Zone 2.

### 2.2 Welche Zelle bei welchem HP-Stand?

`+0x4` wird vom Applier als WORT geschrieben: `2` (HURT) bzw. bei `HP < 0` `3` (DEATH)
(`lh v1,342 / bgez / sw 2,4 / sw 3,4` @0x8004727C-90). Also:

* **Zombie ueberlebt** → HURT-Wurzel @0x80104F40: erst Zerleger (Bein ab, Gate §1.4), dann
  Zelle `[Zeile][Spalte]` der HURT-Tabelle. Schrot: col0 → Knockdown (P0 reisst ARME ab,
  @0x80107514-708, nur Zeile 12 ausgenommen `beq v1,12` @0x8010750C), col1 → Ragdoll 66FC.
* **HP < 0** → DEATH-Wurzel @0x80108250, Zweig 4 = 2D-Dispatch: Schrot W8 col0 →
  7438-Todeszweig (`rand&3 != 0` → HP=10-Wiederbelebung @0x8010778C-B0, sonst entscheidet
  +0x16B: **Arm ab → CORPSE** `sw 7,4` @0x801077E4); col1 → 66FC-Todeszweig
  (`rand&3 != 0` → `+0x231=1` → **Burst 0x80109610** @0x801067F4-834, sonst P2 **HP=10 →
  Kriecher** @0x80106ACC-E8). SPAS W13 col1 → **0x80108BEC voller Zerreiss**.
  Magnum W7 (Zeilen 5/6) col1 → **0x801092C4 Kopf platzt**.

### 2.3 Was der Port fuer die Zerreiss-Zelle 0x80108BEC zeichnen muss (Stempel-Kontrakt P0)

Alle Adressen selbst disassembliert; der Port-Zwilling `re2z_death_rip` (:5251) stempelt das
bereits — es scheitert nur an Blockern (2)/(3)/(4):

| Was | Felder | Beleg |
|---|---|---|
| KOPF AB (Part 8) | `flags|=0x4A` (fliegt mit Drift-Physik, verschwindet nach 29 Frames `sltiu 0x1d` @0x80028DC8) | @0x80108DD4-E0 |
| Kopf-Kinetik | w9c=400, w9a=-100, life=0, w9e=10, wa4=-50, yaw98=+0x76+2048 | @0x80108DEC-E14 |
| Kopf-Tinte | +0x70 = 0x00101030 | @0x80108E08 |
| Kopf-Blut | FUN_8001BF10 **Id 3000**, Winkel rand*16, Anker Part 8 | @0x80108E18-28 |
| Anker-Kill | FUN_8001CEFC(5,3,Part 8) | @0x80108E38 |
| R-BEIN AB (Part 9) | `flags|=0x1062` (Wurf-Physik) + Wurffelder | @0x80108E54-98 |
| Stumpf-Marker Schienbein (Part 10) | `flags|=0x80`, +0x98=64, +0x9C=0x00101010 | @0x80108EAC-BC |
| L-BEIN AB (Part 12, 50%) | wie Part 9 | @0x80108EE8-F70 |
| Stumpf-Marker Fuss (Part 11) + Part 0 | `flags|=0x80` + Zerfallsfelder | @0x80108F78-94 / @0x80108F98-B4 |
| **RUMPF-STUMPF** | Geometrie-Woerter von **Record 16** → Part 0 (`lw 2760/2764/2768/2772 → sw 8/12/16/20`) | @0x80108FB8-FDC |
| Rumpf-Blut | Id **8000**, Offset {0,-500,0} + **Fleischbrocken** `0x08000000|((r+4000)*4)` | @0x80108FE4-9054 |
| SE | ENEMSE 2 | @0x8010905C-60 |
| Nachbluten P1 | 2 Emitter Id 8000 je geradem +0x15A-Stand | @0x801090D4-140 |

Beim Bein-Zerleger analog: Stumpf = Reserve-**Mesh 15** in den Oberschenkel (@0x8010531C-50),
Blut-Ids 8000 (Oberschenkel) + 7000 (Schienbein) @0x80105354-D4. Beide Reserve-Meshes (15
Bein-Stumpf, 16 Rumpf-Stumpf) existieren **nur im RE2-MD1** (EM010: 17 Meshes / 15 Bones).

---

## 3. PLAN — die Blocker in Auslöse-Reihenfolge

Reihenfolge nach Sichtbarkeit-pro-Aufwand und Abhaengigkeit; (a)-(c) sind reine Logik/FX,
(d) haengt am RE2-MD1, (e) ist Fixture-Pflege.

**Schritt 1 — den heutigen Spalten-Regressor fixen (reine Logik, ZUERST).**
Ort: re15_damage.c:1698-1701 (Elevation-Stempel) bzw. re2z-Dispatch. Regel byte-true aus den
Tabellen: eine Spalte, deren Zelle in der gewaehlten Zeile NULL ist, ENTSTEHT im Original
nie (kein NULL-Waechter, `jalr v0` @0x80105410/@0x80108514) — der Port darf sie also nicht
stempeln. Konkret: nach der Zeilenwahl die Spalte klemmen `wenn tbl[row][col]==NULL → col=1`
(fuer HURT UND DEATH; Zeilen 9-11 behalten col 0/1/2). Damit liefert „Kopf gezielt + Schrot"
wieder die STAERKSTE belegte Zelle (col1: Burst/Zerreiss) statt gar nichts. Beleg: §2.1.

**Schritt 2 — Blocker (1): Todes-Gore in den RE1.5-Flavor bringen (reine Logik).**
Ort: enemy_ai_re2_zombie.c:6149. Die Bruecke darf bei `state == 3` nicht mehr umkehren,
sondern spiegelt die STEMPEL der Death-Zelle (`re2z_death_tbl[row][col]`): 92C4-P0-Stempel
(Kopf 0x4A + Tinte 0x00101040 @0x80109460-98), 8BEC-P0-Stempel (§2.3), 7438-Arm-Zweig,
66FC/9610 als Fling-Stempel — als reine Part-Feld-Schreiber, waehrend die RE1.5-Todesanim
weiterlaeuft (dasselbe Muster wie die vier bestehenden Import-Haken enemy_ai_common.c:1849/
2961/3082/4880 und wie die HURT-Bruecke selbst). Die Anzeige-Kette (resolve/part_matrix/
Renderer) ist im RE1.5-Flavor bereits scharf (re15_re2z_gore_active :4092 erlaubt
import_owns). Nutzer-Mandat: Zitat 2026-08-20 im Optionsblock :192-196.

**Schritt 3 — Blocker (3): `packed_id` konsumieren (FX, reine Logik).**
Ort: `re2z_gore_fx` :3916. Die tatsaechlich benutzte Id-Menge ist klein und liegt komplett
in diesem Modul: 2000/3000/4000/5000/6000/7000/8000, 0x0503xxxx (Brand), 0x040Fxxxx (Aetz),
0x0600xxxx/0x040Cxxxx (Spark), 0x08xxxxxx (Brocken). Mindest-Aufloesung in drei Klassen —
Fontaene (8000), Spray (3000/6000/7000, Winkel-Argument), Brocken (0x08…) — und auf die
Port-Effekt-VM mappen (`re15_esp_fx_spawn_rows`, CORE00.ESP; derselbe Weg, den die
Schwester-Gruppe fuer Muendung rowblk 0x0FC8 / Huelse rowblk 0x18C0 geht). Jede Zuordnung
traegt die Spawn-Adresse aus §2.3. Erst damit sehen Burst/Kopf-Tod nach RE2 aus.

**Schritt 4 — Blocker (2): Stumpf-Geometrie retten (haengt am RE2-MD1).**
Ort: re2_ems.c:372. Das gesicherte `re2_md1` (liegt in `re2_hybrid_apply` bereits als lokale
Kopie vor, :341-346) nicht wegwerfen, sondern als Gore-Seitenbank an der Bank fuehren
(z.B. `eb->md1_gore` + zugehoerige RE2-TIM-Seite aus dem gecachten CDEMD0.EMS,
platform/pc/main.c:501). Renderer: die Klammer main.c:8283 wird zur Weiche — `gore_mesh >= 15`
zeichnet aus der Gore-Bank (RE2-Mesh 15/16 mit RE2-Textur), sonst wie bisher. Fuer den
RE1.5-Flavor dieselbe Quelle (EMS-Cache laden, auch wenn das Brain RE1.5 ist). ZU KLAEREN
vor dem Bau: ob der Enemy-Textur-Pfad zwei TIMs je Aktor traegt (s. „unklar").

**Schritt 5 — Blocker (4): Kinder-Kaskade im Flug + Anker-Kill (reine Logik).**
Ort: `re15_re2z_gore_part_matrix` :4312. Traegt der ELTERN-Part Bit 0x40, muss das Kind
seine Pose relativ zur Eltern-RECORD-Matrix bekommen (Original: `child+0x74 = &parent+0x48`,
FUN_80028368.c:106-109) statt aus der Bank-Skelettkette — dann fliegen Schienbein+Fuss mit
dem Oberschenkel. Die Lande-Kaskade existiert schon (@0x80028CA0 + @0x80027480-94,
resolve :4139-44). FUN_8001CEFC wird implementierbar, sobald Schritt 3 den FX einen
(gruppe,sub,anker)-Schluessel gibt (@0x8001CF14-4C): beim Abriss die am Part haengenden
Spawns beenden.

**Schritt 6 — Fixtures nachziehen (Pflicht, Memory `reai-v2-pin-fixture-verschiebung`).**
test_re2_zombie_teardeath.c PIN 8 (:643-672) pinnt heute „RE1.5 reisst nichts ab" — nach
Schritt 2 ist das die erwartete Verschiebung: PIN 8 wird zum Positiv-Pin (dieselben
Part-Stempel wie PIN 2, nur RE1.5-Todesanim). Neu dazu: (i) ein Spalten-Pin (Schrot + UP-Aim
→ col wird auf 1 geklemmt, Zelle 66FC/8BEC statt NULL), (ii) ein Hybrid-Pin (RE2-Flavor MIT
`re2_hybrid_apply`, prueft dass `gore_mesh=15/16` gezeichnet statt geklemmt wird — genau die
Luecke, durch die der heutige Pin am Nutzer vorbei gruen war).

### Abhaengigkeitsbild

* **Reine Logik (kein Modell):** Schritte 1, 2, 5, 6 und der Mapping-Teil von 3.
* **Am RE2-MD1 haengend:** nur Schritt 4 (Reserve-Meshes 15/16 existieren ausschliesslich
  dort; kein RE1.5-Zombie-EMD traegt ein 16. Mesh — Messung :6138-41). Ohne Schritt 4 gibt
  es Kopf-/Bein-Flug und Blut korrekt, aber keinen sichtbaren Stumpf.

---

## Unklar / vor dem Bau zu klaeren

1. **Hitscan-Zonen-Erzeuger:** Die Indizierung `s1*9+s0` der Maskentabelle 0x800A6DB4 in
   FUN_800410CC (wer ist s1: Haltungsklasse oder Aim-Band?) ist nicht zu Ende RE'd. Fuer den
   Plan nicht tragend (die NULL-Spalten klemmen datenseitig), aber fuer einen spaeteren
   byte-true Zonen-Erzeuger offen.
2. **Zwei Texturen je Aktor:** ob der PC-Enemy-Renderpfad neben dem RE1.5-TIM eine zweite
   (RE2-)TIM-Seite fuer die zwei Stumpf-Meshes binden kann, oder ob die Stumpf-UVs in den
   bestehenden Atlas umgerechnet werden muessen (platform/pc/main.c, Texturbindung der
   Enemy-Bank).
3. **Stumpf-Marker 0x80:** Konsument der Felder +0x98/+0x9C der 0x80-markierten Parts
   (@0x80108EAC-BC u.a.) weiterhin OFFEN — vermutlich ein Zerfalls-/Dunkel-Effekt im
   Zeichner; aendert nichts an Meshtausch/Flug.
4. **66FC-Seiteneingaenge:** `0x800CFBF8 < 0` und `+0x10C/0x800CFBD8&0x10000000`
   (@0x8010679C-EC) haben keinen Port-Produzenten (OPEN, dokumentiert :5616-5618).
