# Story-Texte umsetzen (Spur ST80-T, Runde 41)

Stand 2026-11-02, Zweig `r41/story80`. Grundlage: `ABGLEICH.md` (A1, f.2, f.3), `PORT_ZENSUS.md` (§e), Nutzerentscheide
und Nachträge des Koordinators (30E1, 30C1, 1151, 6030 Wagen und Führerstand). Dieses Dossier wird fortlaufend geschrieben.

Keine Datei unter `synchro/`, `pri/`, `Version_80%/` verändert (synchro nur gelesen). Kein Asset-Patch: die RDTs bleiben
byte-true, Texte und Szenen legt der Port beim Laden bzw. beim Start des Original-Subs ab.

## 1. f.2 — Text-Id 20 in ROOM1151 doppelt belegt (gemessen, behoben)

**Messung vor der Behebung** (`unit_r41_story80_hebetisch1151`, echter Engine-Code: Raumstart ROOM1151 mit (3,0x9D)=1 wie
nach der Hinbeuge-Szene `ROOM1151 sub01 @0x00EA8`, Aktion am Tisch = Ereignis 4, Cursor, SQUARE daneben, Text zu, CROSS,
dann echter Aktionsdruck an der Gesprächslage (-20538,-25147) Gier 1500):

```
== hebetisch ROOM1151 ==
  ok: ROOM1151 (3,0x9D)=1: Irons-Platz Slot 2 = Text 20
  ok: Hebetisch-Cursor aktiv nach 10 Bildern
  ok: Fehldruck: Text Id 20 "Nothing happened."
  ok: Fehldruck-Text geschlossen
  ok: Cursor mit CROSS verlassen (Aufraeumbytes)
   Text nach Irons-Klick: Id 20 "Nothing happened."
  FEHLER: Irons nach dem Fehldruck: Id 20 "Nothing happened." (soll 20 "He has already passed...")
  FEHLER: Hebetisch-Id 20 != Irons-Id 20
```

Ursache wie im Abgleich gelesen: `hebetisch_cursor_1150.c:314` setzt „Nothing happened.“ unter `RE15_HC_TEXT_ID` = 20,
`elza_r36.c:272` legt Irons' Text unter `RE15_ELZA_IRONS_MSG_ID` = 20 ab, der Untersuchen-Weg (`re15_scd_show_message`)
liest die Tabelle ohne neu einzusetzen.

**Behebung:** eigene Id 21 für den Hebetisch (`include/re15_hebetisch_cursor.h`). Belegung geprüft: ROOM1150 0..14 (RDT,
Sektion @0x012F8), 20 (bisher Hebetisch), 22..29 (`irons_tod_1150.c` OP_MSG), 30/31 (`re15_kartenitems_r36.h`); ROOM1151
0..3 (RDT @0x010EC), 20 (`re15_elza_r36.h`), 30/31. Id 21 kommt in keinem Arbeitsbaum für 1150/1151 vor (nur 1110, 2060-Satz
in 1050, 11B0). < 32 (PSX `MSG_TABLE_N`).

**Gegenprobe nach der Behebung:**

```
== hebetisch ROOM1151 ==
  ok: Fehldruck: Text Id 21 "Nothing happened."
   Text nach Irons-Klick: Id 20 "He has already passed..."
  ok: Irons nach dem Fehldruck: Id 20 "He has already passed..." (soll 20 "He has already passed...")
  ok: Hebetisch-Id 21 != Irons-Id 20
== hebetisch ROOM1150 ==
  ok: Fehldruck: Text Id 21 "Nothing happened."
   Text nach Irons-Klick: Id 2 "Irons: I'll be fine. Just worry about yourself for now."
```

Dazu grün: `unit_r34n_b_*` (Hebetisch, Prüfung 6 jetzt Id 21), `unit_r36_elza_*`, `unit_r38_kleinkram_*`.
