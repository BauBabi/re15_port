# Runde 38 — Spur G "tagebuch" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_tagebuch`, Zweig `r38/tagebuch`, Basis e2cc7673. Angelegt 2026-10-16 16:19.

Punkte (Wortlaut AUFTRAG.md, massgeblich):
1. Elza, ROOM1011: zweites Anklicken des liegenden Roy -> "Roys Diary" (Hintergrund FILE16_title_paper) STATT der
   Code-Nachricht; Text woertlich; Code 0513 gruen wie in allen Dokumenten.
2. Danach: Karte markiert bei Elza den Communications Room und den Prison Room, so wie bei Leon der Communications Room.

Ressourcen (VERTRAG §1): Bank-9-Bit 68; Msg-Ids 1011 40-44; Dokument-Id per Zensus.

## Stand
- [ ] Messung vorher
- [ ] RE-Belege
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## Messung vorher

Bau des Baums (Stand e2cc7673 + Dossier), `local_build.sh configure` + `build` = LOCAL-BUILD-OK.

### V1 — Roy (Elza, ROOM1011) nach dem Dialog untersuchen, echter Weg ueber Spielstand + CONTINUE
Lauf `scratchpad/G/v1`: Karte `probe_r36_elza_karte re15_card.mcr 1011 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74`
(Stand NACH dem Dialog sub02: (4,0xF1)/(4,0xFA)/(3,0x74) = sub02 @0x00AFE/@0x00B02/@0x00B06), Elza vor Roy, Blick +z
(Gier 3072: Vorwaertspunkt x + 620 cos, z - 620 sin, aot_common.c:1273 / @0x80042bd0), CONTINUE, Viereck (Aktion,
game_step_common.c:1110) in Bild 60. debug.log:
    [schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)      <- Runde 36 Q: Ereignis 6 -> 7
    [scd] Plc_motion(entity=1, motion=11, flags=0x00)                                 <- sub07 @0x00E5C, Elza beugt sich
    [msg] room=1011 id=18 savepoint=0
    [msgtext] room=1011 id=18 "He has passed out from his injuries.He is holding a slip.The numbers 0513" are printed on the slip."
Bild `G_belege/vorher_1011_msg18.png` (Bild 60 / Bild 120): die Code-Nachricht msg 18, Seite 1 mit Weiter-Pfeil.
Damit Ist-Zustand: JEDES Untersuchen des liegenden Roy zeigt msg 18 (Zettel mit 0513); kein Dokument, keine Karte.

### V2 — Karte bei Elza
Hinweis-Tabelle `map_hint_common.c` s_hints[]: 3 Eintraege, alle Leon (Quelle 0x1150 Ziel 0x10F0; Quelle 0x10F0 Ziele
0x11C0/0x1150). Fuer Elza gibt es keinen Eintrag -> keine Markierung.

## RE-Belege

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
