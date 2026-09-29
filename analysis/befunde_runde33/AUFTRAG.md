# Runde 33 (2026-09-29) — Auftrag des Nutzers, wörtlich

Stand vor der Runde: master 88217fd3 (v0.8.18, Suite 416).

> Als nächstes:
> - Ich möchte das du eine Message schreibst, wie in Resident Evil 2, wenn man speichern möchte,
>   aber kein Farbband besitzt. Nur statt Farbband eben "Memory Card".  Speichern soll nur möglich
>   sein, wenn man eine Memory Card besitzt.
> - Nach der Cutscene mit Irons und dem Anzeigen des Communication Room, wo man hin soll, muss man
>   hinterher noch in der Lage sein bei der Map zu 2F zu wechseln, um den Raum zu sehen, auch wenn
>   man noch nicht auf 2F war. Außerdem muss der Raum irgendwie angezeigt bleiben, wie bei Resident
>   Evil 2 bei Zielräumen auch.
> - Man soll nicht eher von Room 1130 zu Room 1120 wechseln können, bevor man einmal die 1,
>   Cutscene mit chief Irons getriggert hat in Room 1170. Vorher soll der Text stehen "I have to
>   Report the situation to the chief first..."
> - Ich möchte das du - für alle Türen die jetzt noch fehlen mit der Türanimation die Türen baust
>   und Animationen hinzufügst, das wir da komplett sind.

## Themen / Arbeitsbäume (alle auf master 88217fd3 + diesem Commit)

| Thema | Baum | Zweig | Dossier |
|---|---|---|---|
| S — Speichern nur mit Memory Card (Item 0x21), RE2-Farbband-Meldung | `.claude/worktrees/r33_speichern` | `r33/speichern` | `speichern_memory_card.md` |
| K — Karte nach dem Irons-Hinweis: 2F wählbar, Communication Room bleibt markiert | `.claude/worktrees/r33_karte` | `r33/karte` | `karte_zielraum.md` |
| R — Tür ROOM1130 -> ROOM1120 erst nach der ersten Irons-Szene in ROOM1170 | `.claude/worktrees/r33_tuer1120` | `r33/tuer1120` | `tuer_1130_1120.md` |
| T — Türmodelle + Animationen für die 61 bisher nicht abgedeckten Türen | `.claude/worktrees/r33_tueren` | `r33/tueren` | `tueren_rest_*.md` |
