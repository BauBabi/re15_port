# Runde 36 — Spur I "inventar" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_inventar`, Zweig `r36/inventar`, Basis master 78a4a748.
Zuteilung (VERTRAG.md §0/§1.4): I = inventory_common.c + re15_savepoint.c + Debug-Menue; keine Bits,
keine Nachrichten-IDs, keine Ereignis-Nummern.

## Auftrag (woertlich, AUFTRAG.md Z. 28/29)

> - Bei den Debug Menu, bei den man zwischen die Räume springt, schickst du, wenn man mit Elza spielt Elza
>   in Leons Räume. Wenn man mit Elza spielt, sind die Räume mit "1" hinten korrekt, zum Beispiel ROOM 1071.
> - Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards oder
>   Handgranaten 0 sollen sie aus dem Player inventory verschwinden

Punkte:
* **I1** Memory Card: Speichern verbraucht 1 (Anzahl -1).
* **I2** Memory Card bei 0 -> Slot verschwindet aus dem Inventar.
* **I3** Handgranaten bei 0 (letzter Wurf) -> Slot verschwindet aus dem Inventar.
* **I4** Debug-Menue Raumsprung: bei Elza die xxx1-Raeume (z.B. ROOM1071), nicht Leons.

## Stand

(laufend) — Baum sauber auf 78a4a748 gestartet (Fortsetzung: kein Vorgaenger-Commit vorhanden), Baseline
gebaut (`local_build.sh configure` + `build`, LOCAL-BUILD-OK). Messwerkzeug:
`re15_port/tools/r36_i/messlauf.sh <name> <timeout> KEY=WERT ...` (Messkopie `re15_pc_r36i.exe` neben der
exe, Arbeitsordner `re15_port/build/r36i_mess/<name>/`).

## Messung vorher

### I4 Debug-Menue mit Elza (Lauf `i4_vorher`, Stand 78a4a748)
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
RE15_DEBUG_JUMP=1070@250 RE15_EXIT_AT=400` (Spielerwahl schaltet auf Elza, Sprung ueber den Menue-Pfad
re15_debug_menu_tick = Quadrat in der JUMP-Zeile). debug.log:
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[debug-menu] AUTO-JUMP -> ROOM1070 (Frame 250)
[debug-menu] JUMP -> 107 LOBBY OFFICE   (ROOM1070) spawn=(850,0,3600) cut=0
[room] PC loaded room1070.rdt (153328 bytes)
[flow] EXIT_AT: Bild 400 in Raum 1070 erreicht -> exit
```
**REPRODUZIERT:** Elza (Elza-Bit 1) landet in Leons ROOM1070.RDT. Ursache im Code: platform/pc/main.c:7395
`unsigned droom = (unsigned)dm->load_room << 4;` — die Variante (unterste Hex-Ziffer) ist immer 0.

### I1/I2 Memory Card beim Speichern (Lauf `i1_vorher_karte`, Stand 78a4a748)
Spielstand vom Kartenwerkzeug `probe_r33_speichern_karte re15_card.mcr karte` (ROOM1150 vor dem Telefon,
Inventarplatz 0 = Memory Card x1), Weg wie integration_r33_speichern B: `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,1 RE15_MSG_LOG=1 RE15_PAD_AT=150:A,200:A,290:A,370:A
RE15_EXIT_AT=460#1150`. debug.log:
```
[save] room=1150 msg=1 card_slot=0 -> FRAGE (Memory Card benutzen?)
[save] Antwort: JA -> Speicherbildschirm
[save] saved (room 1150) slot n=2 -> next=3; card=kept (slot 0 qty 1)
```
Geschriebener Block (Platz 1, Inventar @Datei 0x4128): `21 01` = Memory Card x1.
**REPRODUZIERT:** Speichern verbraucht die Karte nicht (Runde-33-PORT-WAHL "kein Verbrauch",
main.c:5056-5058 / ee17ca0a), die Anzahl bleibt 1.

## RE-Belege

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
