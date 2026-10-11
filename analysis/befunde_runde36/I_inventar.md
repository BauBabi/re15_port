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

### I3 letzte Handgranate (Lauf `i3_vorher`, Stand 78a4a748)
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=-4311,-19289,0
RE15_AI_FLAVOR=re2 RE15_GIVE=9:1 RE15_EQUIP=9 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W6,S0.1,W3 RE15_FRAMEDUMP=300-400/20:inv RE15_EXIT_AT=400#1140`
(Wurf-Rezept von integration_r35_granate "wand", aber EINE Granate; START erst bei F280 > Sprungbild 250,
damit es nicht schon im Startraum als klebrige Anforderung liegen bleibt — Messfalle des ersten Laufs:
"S" im Startraum oeffnete das Inventar in ROOM1140 bei F15 und verschluckte den Wurf).
state.log (ROOM1140, mg = Menge des ausgeruesteten Platzes):
```
F15 pad=0800 ac=8 mg=1     (Zielen)
F19 pad=8800 ac=7 mg=0     (Abzug: FUN_8004eae4 zieht im Abzugsbild ab)
F41 ... fx=1 mg=0          (Projektil Rueckstoss-Bild 22)
F100..F125 fx 2..4..0      (Explosion)
F280 pad=0008 mg=0         (START)
```
Statusschirm F340 (`I_belege/i3_vorher_statusschirm_F340.png`): "Equip Arms" = Handgranate mit **0**,
Item list Platz 2 = Handgranate mit **0**. **REPRODUZIERT:** die leere Granate bleibt im Inventar und
ausgeruestet.

## RE-Belege

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
