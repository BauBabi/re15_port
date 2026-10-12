export const meta = {
  name: 'r36-pruefungen-vor-release',
  description: 'Runde 36: Elzas neuer Pflichtweg zum Prison Key Ende-zu-Ende und den exe-Haenger ROOM2060 an der Integrations-exe nachpruefen (nur messen)',
  phases: [{ title: 'Pruefen', detail: 'D1 Elza-Pflichtweg, D2 Haenger 2060 — parallel' }],
}

const INT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration'
const GEMEINSAM = `
Projekt reAi_v2 (PC-Port des RE1.5-Prototyps), Runde 36 kurz vor dem Release v0.8.24. Integrationsbaum: ${INT} (Zweig
r36/integration, HEAD ba384fac = alle 17 Spuren + Spur R). Die exe ist gebaut: ${INT}/re15_port/build/platform/pc/re15_pc.exe.
REGELN:
- NUR MESSEN UND BERICHTEN, keinen Code aendern, nicht bauen, kein ctest (im Baum laeuft gerade ein Testlauf, spaeter wird
  neu gebaut). Arbeite mit einer eigenen MESSKOPIE der exe im selben Ordner (re15_pc_<name>.exe, am Ende loeschen) und
  eigenen Arbeitsordnern unter dem Sitzungs-Scratchpad oder ${INT}/re15_port/build/<name>_wd/.
- Harness-Haken sind getenv("RE15_...") in platform/pc/main.c + engine/src (u.a. RE15_DEBUG_JUMP, RE15_PLAYER_POS,
  RE15_INPUT_SCRIPT/RE15_INPUT_SCRIPT_START, RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 (= Elza), RE15_EXIT_AT,
  RE15_WEGPLAN (geschlossener Wegplan-Treiber der Spur R: engine/src/dachkiste_r36.c — Befehle GEHE/SCHIEB/FLAG/AOTS/...,
  Beispiele analysis/befunde_runde36/R_belege/wegplan_*.txt), RE15_MSG_LOG, RE15_FIRE_AOT, Item-/Flag-Setzer). Semantik im
  Code nachlesen. In einer RDP-Sitzung braucht jede exe SDL_ASSERT=always_ignore. Laeufe ohne RE15_INPUT_SCRIPT lesen das
  echte Pad (angesteckter Controller verfaelscht) — immer skriptgesteuert fahren. Mess-exes nach dem Lauf beenden (taskkill
  /PID, nie /IM).
- Jede Aussage mit Beleg (Logzeile mit Bildnummer, RDT-Offset, @0x-Adresse). Kein "sieht gut aus".
- Antwort auf Deutsch als StructuredOutput.`

const SCHEMA = {
  type: 'object',
  properties: {
    ergebnis: { type: 'string', enum: ['gangbar', 'blockiert', 'reproduziert', 'nicht reproduziert', 'unklar'] },
    zusammenfassung: { type: 'string' },
    schritte: { type: 'array', items: { type: 'object', properties: { schritt: { type: 'string' }, ergebnis: { type: 'string' }, beleg: { type: 'string' } }, required: ['schritt', 'ergebnis', 'beleg'] } },
    blocker: { type: 'array', items: { type: 'object', properties: { was: { type: 'string' }, ursache: { type: 'string' }, beleg: { type: 'string' }, vorschlag: { type: 'string' } }, required: ['was', 'ursache', 'beleg', 'vorschlag'] } },
    messdateien: { type: 'string' },
  },
  required: ['ergebnis', 'zusammenfassung', 'schritte', 'blocker', 'messdateien'],
}

phase('Pruefen')
const [d1, d2] = await parallel([
  () => agent(`${GEMEINSAM}
AUFGABE D1 — Elzas neuer Pflichtweg zum Prison Key (Q_abnahme_1.md §8 H7, ${INT}/analysis/befunde_runde36/Q_schluessel.md).
Hintergrund: Im Original bekommt Elza den Prison Key von Roy in ROOM1011 (analysis/story_leon_elza/ablauf_elza.md E7: sub06
@0x00E2C Set(4,0xEC)=1). Runde 36 Spur Q hat ihn auf Nutzerwunsch in den Funkraum ROOM10F1 gelegt (Konsole links neben der
Tuer von 10D1; Taken-Bit (9,83); Kartenleser ROOM10D1 (3,0x32) braucht die Blue Keycard aus dem Beweisraum 1111 hinter dem
Stimmschloss 1101, das den Minidisc Player aus 1201 braucht — ablauf_elza.md "STAGE1-Nebenwege"). Ohne Prison Key bleibt die
Zelle ROOM1211 zu (E12) und Elzas Pflichtkette (John befreien -> Roy -> Briefing -> Dach -> Kanal) haengt.
1. STATISCH: Erreichbarkeit — wo liegt jedes Glied (Minidisc 1201, Stimmschloss 1101, Blue Keycard 1111, Kartenleser 10D1,
   Prison Key 10F1, Zelle 1211), welche Tueren/Flags/Items sperren die Wege dorthin, und ist die Kette fuer Elza in der
   Reihenfolge ihres Hauptwegs erreichbar, BEVOR sie den Schluessel an der Zelle braucht (ablauf_elza.md E1-E12; Leons
   Fortschritt spielt fuer Elza keine Rolle). Belege mit RDT-Offsets (re15_port/tools/scd_dump_room.py, story_view.py,
   flag_xref.txt).
2. DYNAMISCH an der exe (Elza: RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1): den Weg 1201 -> 1101 -> 1111 -> 10D1 -> 10F1
   -> 11E1 -> 1211 mit ECHTEN Interaktionen fahren (Minidisc nehmen, am Stimmschloss benutzen, Blue Keycard nehmen, am
   Kartenleser 10D1 benutzen, Prison Key in 10F1 aufnehmen, zur Zelle, Befreiungsszene bis "Elza: John. Wait!" und die
   Wegwerf-Frage). Startzustand so nah wie moeglich am echten Spiel (z.B. Elza nach E11 mit dem, was sie bis dahin hat; nur
   was sich nicht anders herstellen laesst per Flag-/Item-Haken, und dann im Bericht nennen). Je Glied: Logzeilen mit
   Bildnummer (Item genommen, Tuer offen, Flag gesetzt, Raum geladen).
3. Blockiert ein Glied: Ursache mit Adresse/Offset bestimmen (Original-Verhalten aus dem RDT/SCD gegen den Port), Vorschlag.
Bericht: analysis/befunde_runde36/D1_elza_pflichtweg.md im Integrationsbaum schreiben, NICHT committen (der Orchestrator
committet).`, { label: 'D1:elza-pflichtweg', phase: 'Pruefen', schema: SCHEMA }),
  () => agent(`${GEMEINSAM}
AUFGABE D2 — exe-Haenger ROOM2060. Beobachtet einmal in der R-Abnahme 1 (${INT}/../r36_dachkiste/analysis/befunde_runde36/
R_abnahme_1.md, Abschnitt "Nachtrag": Leon in ROOM2060 bei (-2328,0,-7336) rot 3104 (mo=7 af=112), debug.log endete bei
[walk] F870, der Prozess lebte weiter, alle Logdateien eingefroren). Die Reproduktion (Skript/Kommandozeile) steht dort.
Seitdem ist Spur R in den Integrationsbaum gekommen (Ruhe-Zweig der Objekt-Ausschiebung, Hoehentest Spieler<->Objekt —
beides aendert die Kollision in 2060).
1. Die dokumentierte Reproduktion an der Integrations-exe exakt nachfahren (3 Wiederholungen), dazu Varianten um die Stelle
   (gleiche Lage/Blickrichtung, Gehen/Laufen in die Ecke, mit und ohne verschobenen Tritt obj 2, Leon und Elza ROOM2061).
2. Haengt es: Prozess NICHT sofort beenden — mit gdb (C:\\msys64\\mingw64\\bin\\gdb.exe -p <PID> -batch -ex "thread apply all bt")
   den Stapel aller Threads holen (die exe hat Symbole, wenn mit -g gebaut; sonst Adressen + nm), Ursache eingrenzen
   (Endlosschleife in welcher Funktion, welche Bedingung), Vorschlag. Haengt es nicht: festhalten, wie oft/mit welchen
   Laeufen gemessen, und ob der damalige Stand (R-Baum vor Nachbesserung 3) noch relevant ist.
Bericht: analysis/befunde_runde36/D2_haenger_2060.md im Integrationsbaum schreiben, NICHT committen.`, { label: 'D2:haenger-2060', phase: 'Pruefen', schema: SCHEMA }),
])
return { d1, d2 }
