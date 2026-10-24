export const meta = {
  name: 'r41-integration',
  description: 'Runde 41: bestandene Spuren in r41/integration mergen, bauen, gezielt + unit/prop/probe testen, Querbezuege und verlorene Hunks unabhaengig pruefen, Kette A+C messen',
  phases: [
    { title: 'Merge+Bau', detail: 'Spurzweige mergen (Konfliktregeln), configure + build' },
    { title: 'Pruefung', detail: 'Tests + Querbezugs-Riegel; Kette A+C an der exe' },
    { title: 'Gegenpruefung', detail: 'unabhaengig: jeder Hunk jeder Spur im Ergebnis, Konfliktloesungen korrekt' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const BAUM = ROOT + '/.claude/worktrees/r41_integration'
const CFG = args || {}
const SPUREN = CFG.spuren || []          // [{id:'C', thema:'ventilkette'}, ...]
const KETTE_AC = !!CFG.kette_ac
const LABEL = CFG.label || SPUREN.map(s => s.id).join('')

const BEREITS = CFG.bereits || []      // schon gemergt, Tests/Gegenpruefung stehen noch aus: [{id, thema, merge}]
const liste = SPUREN.map(s => `- Spur ${s.id} "${s.thema}": Zweig r41/${s.thema}, Dossier analysis/befunde_runde41/${s.id}_${s.thema}.md, letzte Abnahme analysis/befunde_runde41/${s.id}_abnahme_*.md (hoechste Nummer)`).join('\n')

const NL = String.fromCharCode(10)
const bereitsText = BEREITS.length ? NL + BEREITS.map(b => `- BEREITS GEMERGT (Merge ${b.merge}), Tests und Gegenpruefung stehen noch aus: Spur ${b.id} "${b.thema}": Zweig r41/${b.thema}, Dossier analysis/befunde_runde41/${b.id}_${b.thema}.md, letzte Abnahme ${b.id}_abnahme_*.md (hoechste Nummer)`).join(NL) : ''
const GEMEINSAM = `
## Rahmen (bindend)
- Integrationsbaum ${BAUM} (Zweig r41/integration). ALLE Aenderungen nur dort; Spurbaeume und Hauptbaum ${ROOT} nur lesen. Kein push, kein master-Merge (macht der Orchestrator).
- Pflichtlektuere: ${BAUM}/analysis/befunde_runde41/UEBERGABE.md §3 + §3a (Integrations-Checkliste mit allen bekannten Querbezuegen), AUFTRAG.md (Orchestrator-Entscheide), VERTRAG.md (gemeinsame Dateien, Bits, Ids). Memory-Notizen C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/reai-v2-gezielte-integration-querbezug.md, reai-v2-paralleler-ninja-luegt.md, reai-v2-gui-tests-flattern-bei-parallelen-agenten.md, reai-v2-build-path-shadowing.md (nur lesen).
- Bauen NUR: cd "${BAUM}" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). Nie zwei Bauten gleichzeitig im selben build/. Bau-/Testausgaben in Dateien unter dem Scratchpad umleiten, nur Ende/Fehler lesen.
- Tests: ctest --test-dir "${BAUM}/re15_port/build" ... (KEINE volle Suite; die kommt vor dem Release). exe-Laeufe mit timeout, Messkopie der exe unter eigenem Namen, NIE taskkill /IM re15_pc.exe. Fenster-Riegel flattern unter Last (andere Agenten bauen parallel): rote Fenster-Riegel einzeln 2-3x nachfahren, bevor sie als Regression gelten.
- Commits per git commit -F <datei>, Nachricht endet mit "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>". Jede Code-Aenderung im Integrationsbaum (Konfliktloesung, Querbezugs-Fix) braucht einen Beleg (Adresse/Byte-Offset oder Messung) im Commit; Konstanten ohne @0x... sind verboten (CLAUDE.md RE-Gate).
`

const MERGE_SCHEMA = {
  type: 'object',
  properties: {
    merge_commits: { type: 'array', items: { type: 'string' } },
    konflikte: { type: 'array', items: { type: 'string' } },
    bau: { type: 'string' },
    kopf: { type: 'string' },
    bericht: { type: 'string' },
  },
  required: ['merge_commits', 'konflikte', 'bau', 'kopf', 'bericht'],
}
const TEST_SCHEMA = {
  type: 'object',
  properties: {
    gruen: { type: 'boolean' },
    summenzeilen: { type: 'array', items: { type: 'string' } },
    rot: { type: 'array', items: { type: 'string' } },
    fixes: { type: 'array', items: { type: 'string' } },
    kopf: { type: 'string' },
    bericht: { type: 'string' },
  },
  required: ['gruen', 'summenzeilen', 'rot', 'fixes', 'kopf', 'bericht'],
}
const PRUEF_SCHEMA = {
  type: 'object',
  properties: {
    in_ordnung: { type: 'boolean' },
    befunde: { type: 'array', items: { type: 'string' } },
    bericht: { type: 'string' },
  },
  required: ['in_ordnung', 'befunde', 'bericht'],
}

phase('Merge+Bau')
const merge = await agent(`Du bist der INTEGRATOR der Runde 41 im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Fuehre diese bestandenen Spuren in den Integrationsbaum zusammen:
${liste}${bereitsText ? NL + '(Bereits gemergt, NICHT erneut mergen:' + bereitsText + ')' : ''}
${GEMEINSAM}
## Vorgehen
1. git -C "${BAUM}" status (muss sauber sein), git -C "${BAUM}" log --oneline -3. Vorher master in r41/integration mergen (git -C "${BAUM}" merge --no-edit master), Konflikte in AUFTRAG.md/VERTRAG.md/UEBERGABE.md immer mit der master-Fassung loesen (git checkout --theirs), dann pruefen, dass keine Zeile der vorigen Fassung fehlt.
2. Je Spur in der angegebenen Reihenfolge: git -C "${BAUM}" merge --no-ff r41/<thema> -m "merge(r41): Spur <X> <thema> - Abnahme bestanden". Konfliktregeln: (a) Dokumente AUFTRAG/VERTRAG/UEBERGABE -> master-Fassung; (b) gemeinsame Code-Dateien (main.c, aot_common.c, scd_vm.c, scd_room_setup.c, enemy_ai_common.c, audio_pc.c, re15_actor.h, Test-cmake ...) -> BEIDE Seiten erhalten, jede Seite vollstaendig, Reihenfolge fachlich pruefen (Haken-Ketten am selben Ort: Reihenfolge so, dass beide Haken wirken; lies dazu beide Spur-Diffs an der Stelle). Hilfsmittel fuer reine Doppel-Einfuegungen: python -I ${ROOT}/analysis/befunde_runde36/orchestrator/konflikt_beide.py <datei> (behaelt ours dann theirs) — danach die Stelle LESEN und kompilierbar/fachlich richtig machen. (c) Dossiers/Belege sind spureigene Dateien (keine Konflikte erwartet).
3. Nach allen Merges: configure + build. Kompilierfehler aus Konfliktloesungen beheben (Commit "fix(r41-integration): ..."). Warnungen neu gegenueber den Spuren nennen.
4. Antwort: Merge-Commits, jede Konfliktdatei mit Loesung (welche Hunks beider Seiten, warum die Reihenfolge), Bau-Ergebnis (Abschlusszeile), git-Kopf.`, { label: `merge:${LABEL}`, phase: 'Merge+Bau', schema: MERGE_SCHEMA })

if (!merge) { log('Merge OHNE Ergebnis — Abbruch'); return { merge: null } }
log(`Merge: ${merge.merge_commits.length} Commits, Konflikte ${merge.konflikte.length}, Bau: ${merge.bau}`)

phase('Pruefung')
const testPrompt = `Du bist der TEST-INTEGRATOR der Runde 41. Im Integrationsbaum wurden diese Spuren gemergt und gebaut (Kopf ${merge.kopf}):
${liste}${bereitsText}
Merge-Bericht: ${merge.bericht}
${GEMEINSAM}
## Vorgehen
1. Bau pruefen (local_build.sh build -> "ninja: no work to do" oder sauber).
2. ctest -j 6 --timeout 600 -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)" — Summenzeile woertlich. Die ausgeschlossenen Release-/Android-Skripttests pruefen release/ und platform/android/ (von den Spuren nicht beruehrt) und laufen unter der Last paralleler Agenten in Zeitueberschreitungen; sie laufen in der vollen Suite vor dem Release ohne Last — hier NICHT fahren und NICHT nachfahren. Jede rote: Ursache mit Beleg (Querbezug zwischen Spuren? alter Riegel durch neue Regel widerlegt? Memory reai-v2-gezielte-integration-querbezug), im Integrationsbaum beheben (kleinster belegter Eingriff; widerlegte alte Riegel nur mit Beleg anpassen) oder als echter Spielfehler melden.
3. Je Spur die gezielten Testgruppen aus ihrem Dossier/ihrer letzten Abnahme (dort steht der -R-Ausdruck woertlich) erneut fahren — Unit-Gruppen und die Integrationsriegel der Spur (integration_r41_<thema>*), dazu die Querbezugs-Riegel aus UEBERGABE §3a (z. B. alle Gator-Riegel nach RE15_ACTOR_MAX 36: test_r35_granate, integration_r36_schuss, integration_r41_re2gator ...; alle Riegel, die dieselben Bank-Bits/Raeume setzen wie die gemergten Spuren: git grep in re15_port/tests nach den Bits/Raeumen der VERTRAG-Zuteilung). Integrationsriegel seriell oder -j 2 mit --timeout 1800.
4. Antwort: alle Summenzeilen woertlich mit -R-Ausdruck, rote Tests mit Ursache, Fixes (Commit + Beleg), git-Kopf.`

const kettePrompt = `Du misst im Integrationsbaum (Kopf ${merge.kopf}) die GANZE STAGE2-Kette der Spuren A (wasser20b0) und C (ventilkette) an der echten exe — Entscheid C-E1 in AUFTRAG.md: "Ventile -> Ost-Tuer zu Fuss -> 20B0-Szene bis Szenen-Ende (letterbox zu, (3,235)=1, Spieler frei) -> Leiter -> 2050".
${GEMEINSAM}
## Vorgehen
1. Warte, bis "${BAUM}/re15_port/build/platform/pc/re15_pc.exe" neuer ist als der Kopf-Commit (der Test-Integrator baut ggf.; selbst NICHT bauen). Kopiere die exe unter eigenem Namen (re15_pc_r41int_kette.exe).
2. Fahre den Riegel integration_r41_ventilkette_kette (falls registriert) UND einen eigenen exe-Lauf mit Framedumps (RE15_WINDOW_SCALE=1, RE15_STATE_LOG, RE15_SCD_TRACE) fuer Leon (2030 -> 20B0) und Elza (2031 -> 20B1): Ventile, Warnung, Ost-Tuer zu Fuss, Szene sub02 -> Zone Slot 7 -> sub05 (Auswasch, EM024-Flut der Spur A sichtbar) -> Set(3,235) -> Spieler frei -> Leiter -> ROOM2050. Belege (Logzeilen + Bildbogen) in ${BAUM}/analysis/befunde_runde41/INT_belege/ ablegen und committen ("doc(r41-integration): Kette A+C gemessen").
3. Antwort: in_ordnung = die Kette laeuft fuer Leon UND Elza ohne Haenger bis 2050; befunde = jede Abweichung mit Bild/Logzeile.`

const [test, kette] = await parallel([
  () => agent(testPrompt, { label: `tests:${LABEL}`, phase: 'Pruefung', schema: TEST_SCHEMA }),
  () => KETTE_AC ? agent(kettePrompt, { label: `kette-ac:${LABEL}`, phase: 'Pruefung', schema: PRUEF_SCHEMA }) : Promise.resolve(null),
])
log(`Tests: ${test ? (test.gruen ? 'GRUEN' : 'ROT ' + test.rot.length) : 'OHNE Ergebnis'}${KETTE_AC ? ', Kette A+C: ' + (kette ? (kette.in_ordnung ? 'OK' : 'Befunde ' + kette.befunde.length) : 'OHNE Ergebnis') : ''}`)

phase('Gegenpruefung')
const gegen = await agent(`Du bist die UNABHAENGIGE GEGENPRUEFUNG der Integration der Runde 41. Glaube dem Integrator nichts. Integrationsbaum ${BAUM}, Kopf jetzt: git -C "${BAUM}" log -1.
Gemergte Spuren:
${liste}${bereitsText}
Bericht des Integrators: ${merge.bericht}
Fixes des Test-Integrators: ${test ? test.fixes.join(' | ') : '(keine Angabe)'}
${GEMEINSAM}
## Pruefe (nur lesen, nichts aendern ausser deinem Bericht)
1. VERLORENE HUNKS: Fuer jede Spur: git -C "${BAUM}" diff <merge-base der Spur mit master>..r41/<thema> -- re15_port (die Spur-Aenderung) — jeder hinzugefuegte/geaenderte Hunk muss im Integrationskopf enthalten sein (git -C "${BAUM}" show HEAD:<datei> bzw. grep der markanten Zeilen). Liste jeden fehlenden oder veraenderten Hunk mit Datei/Zeile.
2. KONFLIKTSTELLEN: Jede Datei mit Konflikt (git log --merges, git show <merge> --cc) fachlich lesen: wirken beide Haken (Reihenfolge, fruehes return/continue, doppelte Deklarationen, doppelte Installer-Zeilen)?
3. QUERBEZUEGE aus UEBERGABE §3a: jeder Punkt erledigt (Beleg: Datei/Zeile/Testname)?
4. Fixes des Test-Integrators: belegt (Adresse/Messung), kein Riegel nur "gruen gemacht"?
Antwort: in_ordnung nur, wenn 1-4 ohne Befund; sonst jeden Befund konkret.`, { label: `gegenpruefung:${LABEL}`, phase: 'Gegenpruefung', schema: PRUEF_SCHEMA })

return { merge, test, kette, gegen }
