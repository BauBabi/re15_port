export const meta = {
  name: 'r36-generator-ruhezweig',
  description: 'Runde 36: Regression nach R-Merge — Generator-Cursor ROOM11F0 steht 15 Bilder (Ruhe-Zweig); RE gegen das Original, Fix, unabhaengige Abnahme',
  phases: [
    { title: 'Bau', detail: 'messen, Original pruefen, beheben' },
    { title: 'Abnahme', detail: 'unabhaengig nachmessen' },
    { title: 'Nachbesserung', detail: 'nur bei Maengeln' },
  ],
}

const BAUM = 'C:/workspace/git/reAi_v2/.claude/worktrees/r36_generator'
const KONTEXT = `
Projekt reAi_v2 (PC-Port des RE1.5-Prototyps). Baum: ${BAUM} (Zweig r36/generator, abgezweigt von r36/integration 7b2e56d9 =
alle Spuren der Runde 36 inkl. Spur R). Build NUR "bash re15_port/tools/local_build.sh configure" + "... build" im Baum; Tests
gezielt PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build -R "<regex>" --timeout 900 -j 4 — KEINE volle Suite.
Commits mit git commit -F <datei>, Abschlusszeile "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>". STOP-GATE aus
CLAUDE.md: MESSEN -> Original-Funktion disassemblieren (Adresse + Instruktionen ins Dossier) -> erst dann Code; jede Konstante
mit @0x im Kommentar und in der Commit-Message. Original-Messung: DuckStation-Savestates (stage_saves/, sauber = NICHT die
PATCHED-EXE_*.sav), GDB-Weg wie in Spur R (analysis/befunde_runde36/R_belege/orig_*.py; danach settings.ini EnableGDBServer
wieder false!). In einer RDP-Sitzung braucht jede exe SDL_ASSERT=always_ignore.

BEFUND (Orchestrator): Nach dem Merge der Spur R (Dach-Kiste; neuer "Ruhe-Zweig" des Objekt-Passes: LAB_8002c0b8 =
FUN_8003b558(obj,2) @0x8002c0d0 + Ausschiebung FUN_8003da78 @0x8003b79c je Bild fuer JEDES aktive Objekt ausser dem
Stand-Objekt; Code engine/src/zellschub_r36.c, Haken engine/src/aot_common.c re15_prop_push_tick und
re15_zellschub_nach_pass in game_step_common.c; Dossier analysis/befunde_runde36/R_dachkiste.md, Abschnitte Nachbesserung 3)
sind zwei Riegel rot, die vorher (Integration 645/645) gruen waren:
- unit_r31_generator (tests/unit/r31_generator.c, Teil D): "Zwischenschalter 7, Fahrt + Ruhe (49 Bilder) HOCH gehalten:
  dz=6548, Stillstandsbilder 15, Sperrbilder 0, Zeiger 20" -> [FAIL] "nach einem Zwischenschalter bewegt sich der Cursor in
  jedem Bild (keine Sperre)".
- unit_r34n_c_generator (tests/unit/probe_r34n_c_generator.c, Teil A): [FAIL] "in JEDEM Bild der Fahrt und der 30 Ruhebilder
  bewegt sich der Cursor".
Der Cursor ist das Pool-Objekt obj 0 von ROOM11F0 (Obj_model_set @0x00E54, Typ 4), bewegt per sub02 @0x012F6 Speed_set(2,+200)
waehrend HOCH gehalten wird. Hintergrund: analysis/befunde_runde31/generator.md (+ .pruefer.md), Runde 34 Nacht Spur C
(analysis/befunde_runde34_nacht/), Nutzer-Vorgaben dort ("nach einem Zwischenschalter frei", Abnahme erst bei Zeiger 80).
FRAGE: Schiebt das ORIGINAL den Generator-Cursor auch aus Zellen (dann pinnten die Riegel eine Port-Annahme und der
Cursor muss im Port genauso stehen bleiben — Riegel mit Beleg anpassen) oder NICHT (dann fehlt dem Port eine Bedingung:
z.B. Flag/Typ/Band des Objekts, Box, Zellklasse in FUN_8003b558 @0x8003b634-794, Reihenfolge Speed_set vs. Objekt-Pass —
finden, belegen, nachbilden)? Weitere Cursor-/Panel-Raeume pruefen, die dasselbe Muster haben koennten (Hebetisch ROOM1150
Runde 34 Nacht Spur B, Fahrstuhl-Panel 1080, ROOM4000 Cursor-Raster, ...): ihre Riegel liefen im gezielten Lauf gruen, aber
pruefe, ob ein Ruhe-Zweig-Schub dort im Spiel auftritt (Zensus ueber die Cursor-Objekte waehrend ihrer Fahrt).
`
const PUNKTE = `PUNKTE:
P1 MESSEN: im Port je Bild der Fahrt protokollieren (RE15_PUSH_LOG "RUHE-AUSSCHIEBUNG"/ZELLE, Lage vorher/nachher), welche
   SCA-Zelle den Cursor schiebt und wann (Bilder, Zellindex, Grenzen, Band). Mit und ohne Ruhe-Zweig vergleichen.
P2 ORIGINAL: dieselbe Fahrt am Original messen (DuckStation, Savestate in ROOM11F0 — erzeugen, falls keiner da ist; Haltepunkt
   im Ruhe-Zweig @0x8002c0d0/@0x8003b79c bzw. Lese der Cursor-Lage je Bild) ODER statisch zwingend belegen, warum das Original
   den Cursor (nicht) schiebt. Ergebnis mit Adressen/Instruktionen ins Dossier analysis/befunde_runde36/G2_generator_ruhezweig.md.
P3 FIX nach P2: entweder Port-Bedingung byte-true nachbilden (Code) oder Riegel mit Original-Beleg anpassen. Die Dach-Kisten-
   Riegel der Spur R (unit_r36_dachkiste_zellschub, unit_r36_dachkiste_zensus, integration_r36_dachkiste) und die Zensus-Aussage
   "nur 2060/2061 obj 2 im ersten Bild bewegt" muessen gruen bzw. gueltig bleiben.
P4 REGRESSION gezielt: ctest -R "generator|11f0|panel|hebetisch|r34n_|r31_|r36_dachkiste|zellschub|klettern|objfall|cursor|
   1080|4000|push|prop_|collision" (Summenzeile ins Dossier), Bau LOCAL-BUILD-OK. Abschluss-Commit fix(r36-generator): ...`

const BAU_SCHEMA = { type: 'object', properties: {
  status: { type: 'string', enum: ['fertig', 'teilweise'] }, zusammenfassung: { type: 'string' },
  original_ergebnis: { type: 'string' }, commits: { type: 'array', items: { type: 'string' } },
  tests: { type: 'string' }, offene_punkte: { type: 'array', items: { type: 'string' } } },
  required: ['status', 'zusammenfassung', 'original_ergebnis', 'commits', 'tests', 'offene_punkte'] }
const ABN_SCHEMA = { type: 'object', properties: {
  bestanden: { type: 'boolean' },
  urteile: { type: 'array', items: { type: 'object', properties: { punkt: { type: 'string' }, urteil: { type: 'string', enum: ['erfuellt', 'teilweise', 'nicht'] }, beleg: { type: 'string' } }, required: ['punkt', 'urteil', 'beleg'] } },
  maengel: { type: 'array', items: { type: 'string' } }, bericht: { type: 'string' } },
  required: ['bestanden', 'urteile', 'maengel', 'bericht'] }

phase('Bau')
let bau = await agent(`${KONTEXT}
${PUNKTE}
Arbeite P1..P4 ab, committe fortlaufend (wip(r36-generator): ...), Dossier fortlaufend schreiben (Sitzungslimit-Gefahr).
Antwort = StructuredOutput.`, { label: 'bau:generator', phase: 'Bau', schema: BAU_SCHEMA })

let abn = null
for (let runde = 0; runde < 2 && bau; runde++) {
  abn = await agent(`${KONTEXT}
Du bist die UNABHAENGIGE ABNAHME ${runde} dieser Spur. Der Bau-Agent meldet: ${JSON.stringify(bau)}
${PUNKTE}
Pruefe jeden Punkt selbst (Diff git -C ${BAUM} diff 7b2e56d9..HEAD lesen, selbst bauen, Riegel selbst fahren, die
Original-Aussage selbst nachpruefen — Disassemblat selbst lesen, bei Bedarf eigene Original-Messung; danach settings.ini
EnableGDBServer = false). Keine volle Suite. Bericht analysis/befunde_runde36/G2_generator_abnahme_${runde}.md, nur ihn
committen. bestanden = true nur, wenn alle Punkte erfuellt sind. Antwort = StructuredOutput.`, { label: `abnahme:generator#${runde}`, phase: 'Abnahme', schema: ABN_SCHEMA })
  if (!abn || abn.bestanden) break
  bau = await agent(`${KONTEXT}
${PUNKTE}
NACHBESSERUNG ${runde + 1}: Maengel der Abnahme ${runde} (Bericht analysis/befunde_runde36/G2_generator_abnahme_${runde}.md):
${JSON.stringify(abn.maengel)}
Jeden beheben oder mit Messung + Adresse widerlegen. Commit fix(r36-generator): Nachbesserung ${runde + 1}. Antwort =
StructuredOutput.`, { label: `nachbesserung:generator#${runde + 1}`, phase: 'Nachbesserung', schema: BAU_SCHEMA })
}
return { bau, abnahme: abn }
