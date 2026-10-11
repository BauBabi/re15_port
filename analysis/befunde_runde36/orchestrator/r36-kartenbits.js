export const meta = {
  name: 'r36-kartenbits',
  description: 'Runde 36: Bank-9-Kollision der Lageplan-Bits 90/91 mit Original-Item-Taken-Bits beheben + Zensus-Riegel, dann unabhaengige Abnahme',
  phases: [
    { title: 'Bau', detail: 'messen, Zensus, Bits umlegen, Riegel, gezielte Tests, Commit' },
    { title: 'Abnahme', detail: 'unabhaengig nachmessen' },
    { title: 'Nachbesserung', detail: 'nur bei Maengeln' },
  ],
}

const BAUM = 'C:/workspace/git/reAi_v2/.claude/worktrees/r36_kartenbits'
const KONTEXT = `
Projekt reAi_v2 (PC-Port des RE1.5-Prototyps). Baum: ${BAUM} (Zweig r36/kartenbits, abgezweigt von r36/integration 7a0fd453,
in dem alle Spuren der Runde 36 ausser R zusammengefuehrt sind). Build NUR ueber "bash re15_port/tools/local_build.sh configure"
und "... build" im Baum; Tests gezielt mit PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build -R "<regex>"
--timeout 900 -j 4 — KEINE volle Suite (laeuft spaeter einmal in der Integration). Commits mit git commit -F <datei>
(Backticks!), Abschlusszeile "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>". STOP-GATE aus CLAUDE.md gilt: jede
Konstante mit @0x-Adresse oder Datei-Byte-Offset im Code-Kommentar UND in der Commit-Message.

BEFUND (Orchestrator, aus R_abnahme_3.md H3 und eigenem Zensus):
Spur N ("Lageplaene zum Aufsammeln", engine/src/kartenitems_r36.c, Tabelle engine/src/gen/kartenitems_r36.inc Zeilen
k_r36k_lage, erzeugt von tools/r36_kartenitems/karten_export.py) speichert "Karte genommen" in Bank-9-Bits 87..91
(3F=87 ROOM1140, 2F=88 ROOM10D0, 1F=89 ROOM1030, B2=90 ROOM1200, B1=91 ROOM11D0). Bank 9 ist im Original die Zone der
Item-TAKEN-Bits: op_item_aot_set (scd_vm.c, Kommentar "TAKEN-BIT persistence", byte-true @0x800406d4-0x80040718:
FUN_8004efe4(DAT_800b1078 = flag zone 9, bit = payload[2] = pc[18])). Die Bits 90 und 91 sind Taken-Bits ORIGINALER Items:
ROOM2090/2091.RDT @0x00A12 Item_aot_set Item 0x3F floor 1 -> Bit 90; ROOM2040/2041.RDT @0x01364 Item_aot_set Item 0x32
"Head of Akuma" floor 3 -> Bit 91. Folge: wer in Stage 1 den Lageplan B2/B1 nimmt, findet diese Items in Stage 2 nicht
(moeglicher Softlock); umgekehrt meldet ein alter Spielstand mit genommenem Item die Karte als besessen.
Eigener Zensus des Orchestrators (re15_port/tools/scd_dump_room.py ueber alle shared_assets/PSX/STAGE*/ROOM*.RDT):
Original-Item-Taken-Bits {2,6,10,11,12,14,15,16,18,19,20,22,52,85,86,90,91,92,98,99,104,105,108,109,134..253 (viele)},
Skript-Ck/Set auf Bank 9 {6,10,14,15,16,21,52,85,86,92,99,104,134,136,138,154,155,165,180}. Port-Belegung Bank 9 laut
analysis/befunde_runde36/VERTRAG.md 1.1 + Code: 53-63 (Runde 34), 65, 71-76, 79-81 (Runde 35), 82/83 (Q Schluessel),
84 (J Werkstatt-Tuer), 87-91 (N), 93 (Q Parkplatz-Tuer). Einzige Kollisionen: 90, 91 (und die VERTRAG-Reserven 85/86/92,
die kein Code benutzt). NACHPRUEFEN, nicht glauben.
`

const BAU_SCHEMA = {
  type: 'object',
  properties: {
    status: { type: 'string', enum: ['fertig', 'teilweise'] },
    zusammenfassung: { type: 'string' },
    neue_bits: { type: 'string', description: 'gewaehlte Bits fuer B2/B1 mit Begruendung' },
    commits: { type: 'array', items: { type: 'string' } },
    tests: { type: 'string', description: 'gezielte ctest-Laeufe mit Summenzeile' },
    offene_punkte: { type: 'array', items: { type: 'string' } },
  },
  required: ['status', 'zusammenfassung', 'neue_bits', 'commits', 'tests', 'offene_punkte'],
}
const ABN_SCHEMA = {
  type: 'object',
  properties: {
    bestanden: { type: 'boolean' },
    urteile: { type: 'array', items: { type: 'object', properties: { punkt: { type: 'string' }, urteil: { type: 'string', enum: ['erfuellt', 'teilweise', 'nicht'] }, beleg: { type: 'string' } }, required: ['punkt', 'urteil', 'beleg'] } },
    maengel: { type: 'array', items: { type: 'string' } },
    bericht: { type: 'string' },
  },
  required: ['bestanden', 'urteile', 'maengel', 'bericht'],
}

const PUNKTE = `PUNKTE (jeder wird abgenommen):
P1 MESSEN VORHER: die Kollision an der Engine zeigen (z.B. Unit-Test mit scd_vm_init/scd_room_reenter wie
   tests/unit/test_r36_dachkiste_zellschub.c: Bit (9,91) gesetzt -> ROOM2040 laedt den Item-AOT "Head of Akuma" nicht;
   Bit geloescht -> geladen; ebenso (9,90)/ROOM2090 Item 0x3F). Ergebnis + Adresse im Dossier.
P2 ZENSUS: alle Bank-9-Bits des Originals (Item_aot_set pc[18] in allen RDTs aller Stages, Leon- und Elza-RDTs; dazu Ck/Set/
   Cmp mit Bank 9 in allen Skripten) und alle Bank-9-Bits des Ports (grep in include/ und engine/src nach Bank-9-Konstanten
   und re15_game_flag_*(9,...), auch Runde 34/35). Ergebnis als Datei unter analysis/befunde_runde36/N_belege/ (oder neuem
   Ordner) einchecken.
P3 FIX: B2/B1 auf zwei Bits legen, die weder das Original noch der Port benutzt (Vorschlag aus dem VERTRAG-Zensus: 77/78 —
   selbst pruefen; Begruendung im Kommentar). Aendern: tools/r36_kartenitems/karten_export.py, engine/src/gen/
   kartenitems_r36.inc (gleiche Zeilen, nur Bit), Kommentare (include/re15_kartenitems_r36.h "87..91", engine/src/
   re15_map_owned.c), Tests (tests/unit/test_r36_kartenitems.c bits[], tests/integration/test_r36_kartenitems_bild.cmake
   Feld 7 der Zeilen b2/b1 und Kommentare), analysis/befunde_runde36/VERTRAG.md 1.1 (N-Zeile + Vermerk: 85/86/90/91/92 sind
   Original-Taken-Bits, nicht frei), Dossier analysis/befunde_runde36/N_kartenitems.md (Nachtrag "Bank-9-Kollision").
P4 RIEGEL: neuer Unit-Test (eigene Datei tests/unit/probes/r36_kartenbits.cmake, GLOB-erfasst wie die anderen probes) der
   (a) alle ausgelieferten RDTs einliest und die Original-Bank-9-Bits wie in P2 sammelt (dafuer darf scd_vm.c eine winzige
   Lese-Funktion fuer die Opcode-Groessen s_opcode_sizes exportieren — kein Verhalten), (b) prueft, dass der Zensus die
   bekannten Bits 85,86,90,91,92 findet (sonst ist der Zensus selbst kaputt), (c) prueft, dass KEIN Bank-9-Bit des Ports
   (explizite Liste im Test, je Bit Quelle) im Original-Satz liegt, und (d) mit der alten N-Belegung (90/91) ROT wird
   (Gegenprobe im Dossier zeigen). Dazu der Verhaltens-Pin: Lageplan-Bits gesetzt -> Item-AOTs in ROOM2040/2090 bleiben da.
P5 ALT-SPIELSTAENDE: belegen, dass die neuen Bits in keinem bisherigen Spielstand gesetzt sein koennen (kein Code hat sie je
   gesetzt; git log -S), und was ein v0.8.23-Spielstand mit genommenem Item 0x32/0x3F jetzt zeigt (Karte nicht besessen).
P6 REGRESSION gezielt: ctest -R "r36_kartenitems|kartenitem|r36_kartenbits|map_|karte|r33_karte|r35_karte|unit_r36_|item"
   (Summenzeile ins Dossier), Bau LOCAL-BUILD-OK.`

phase('Bau')
let bau = await agent(`${KONTEXT}
${PUNKTE}
Arbeite P1..P6 der Reihe nach ab, committe nach jedem Schritt (Zwischenstaende als wip(r36-kartenbits): ...), am Ende
fix(r36-kartenbits): ... mit allen Adressen. Schreibe das Dossier fortlaufend (falls das Sitzungslimit zuschlaegt, muss ein
Nachfolger weitermachen koennen). Antwort = StructuredOutput.`, { label: 'bau:kartenbits', phase: 'Bau', schema: BAU_SCHEMA })

let abn = null
for (let runde = 0; runde < 2 && bau; runde++) {
  abn = await agent(`${KONTEXT}
Du bist die UNABHAENGIGE ABNAHME ${runde} der Spur "kartenbits" (Bank-9-Kollision). Der Bau-Agent meldet:
${JSON.stringify(bau)}
${PUNKTE}
Pruefe JEDEN Punkt selbst: eigener Zensus (eigenes Skript oder scd_dump_room.py, nicht die Datei des Bau-Agenten
uebernehmen), Diff lesen (git -C ${BAUM} diff 7a0fd453..HEAD), selbst bauen (local_build.sh configure + build), die neuen und
geaenderten Tests selbst fahren, die Gegenprobe (alte Bits -> Riegel rot) selbst nachstellen (z.B. temporaer, danach
zuruecksetzen), RE-Zitate selbst disassemblieren (.claude/skills/re15-psx-disasm/scripts/re15_disasm.py bzw. die
Decompilate). Keine volle Suite. Schreibe den Bericht nach analysis/befunde_runde36/kartenbits_abnahme_${runde}.md und
committe NUR diese Datei. bestanden = true nur, wenn alle Punkte erfuellt sind. Antwort = StructuredOutput.`, { label: `abnahme:kartenbits#${runde}`, phase: 'Abnahme', schema: ABN_SCHEMA })
  if (!abn || abn.bestanden) break
  bau = await agent(`${KONTEXT}
${PUNKTE}
NACHBESSERUNG ${runde + 1}: die unabhaengige Abnahme ${runde} fand diese Maengel (Bericht
analysis/befunde_runde36/kartenbits_abnahme_${runde}.md):
${JSON.stringify(abn.maengel)}
Behebe jeden Mangel oder widerlege ihn mit Messung + Adresse. Gezielte Tests, Commit fix(r36-kartenbits): Nachbesserung
${runde + 1}. Antwort = StructuredOutput.`, { label: `nachbesserung:kartenbits#${runde + 1}`, phase: 'Nachbesserung', schema: BAU_SCHEMA })
}
return { bau, abnahme: abn }
