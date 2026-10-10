# Runde 36 — Spur J "szenenleon" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_szenenleon` (Zweig `r36/szenenleon`, Basis master 78a4a748).
Auftrag (AUFTRAG.md, woertlich, inkl. Nachtrag 14:00) — Spur J:

1. ROOM11B0-Cutscene Leon/Ada/Marvin: Animation + gemeinsames Wegschieben des Containers mit Marvin klappt nicht.
2. ROOM11B0-Cutscene am Ende erweitern (geaendert 14:00): Marvin "Hold on" -> rennt zur Innenseite der
   Werkstatt-Tuer in 11B0 -> Zuschliess-Animation + Zuschliess-Ton (RE2 Robert Kendo Spielanfang) -> Tuer
   bleibt zu (Elzas Meldung ROOM11B1 msg 12 "The door is locked.") -> zurueck zu Leon/Ada -> Marvin: "I just
   want to make sure we don't have any unwanted followers." Parkplatz-Tuer 11B0->11C0 NICHT abschliessen.
3. ROOM10F0-Szene: Kamera bleibt auf Leon, wenn Marvin/Ada den Raum verlassen (kein "Ploppen"), nur
   Tuer-schliessen-Ton zum Schluss.
4. Irons: sobald tot (gegen Ende Stage 1), kein Kopf-Tracking hinter Leon mehr.

Ressourcen (VERTRAG.md): Bank-9-Bits 84 (Werkstatt-Tuer verschlossen), 85 Reserve; Nachrichten-IDs ROOM11B0
20..27; Ereignisse 22, 23 in ROOM11B0; Eigentuemer cut_10f0.c (Kamera/Abgang) + 11B0-Szene.

## Stand / Fortschritt

- [ ] Punkt 1 Container-Schieben
- [ ] Punkt 2 Erweiterung Werkstatt-Tuer
- [ ] Punkt 3 10F0 Kamera/Abgang
- [ ] Punkt 4 Irons-Kopf nach Tod

(Abschnitte je Punkt folgen: Messung vorher / RE-Belege / Umsetzung / Messung nachher / Tests.)
