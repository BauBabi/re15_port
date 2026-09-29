# Runde 34 (Granaten) — Auftrag

Nutzer, 2026-09-29, woertlich:

> Super, als nächstes mache mir die Handgranate aus Room 1170 komplett Lauffähig, inklusive
> Gegner Reaktion, korrekter Animation, Schaden etc. mit allen. Mache das gleich für alle 3
> Granatenarten.

## Einordnung (vor der RE-Runde)

* "Room 1170" = die Handgranate im Hebetisch von Irons' Buero. Der Hebetisch steht in
  **ROOM1150/1151**, nicht in 1170 (Memory reai-v2-runde33: der Nutzer verwechselt 1150/1170).
  Prop, Aufnahme und Modal liefert `engine/src/granate_1150.c` seit Runde 30 (Nachtrag K).
* Die drei Granatenarten = Item 0x09 Hand Grenade, 0x0A Acid Grenade, 0x0B Incendiary Grenade
  (Namensleser FUN_80028840, Dossier `analysis/befunde_runde30/nachtrag-granate.md` §1).
* Stand v0.8.19 (Dossier Runde 30 §2): Ausruesten, Wurfanimation (W09 Clip 7, 35 Bilder),
  Munitionsabzug und der Spawn von Effekt 4 sub 0x0D laufen. Es FEHLEN der Flug (ESP-Routinen
  30 @0x8001843c und 29 @0x80018320), die Explosion (Routine 31 @0x8001854c), die Kind-Effekte,
  die SEs und der Flaechenschaden `jal 0x80012d60` @0x800185b8. Den Schaden liefert die
  Port-Bruecke ENT[9].resolve SOFORT beim Abzug. 0x0A/0x0B sind im Auslieferungsstand reine
  Munitions-Stubs (@0x80033b58/78), der Spawn ist hart auf Id 9 gegatet (@0x8003368c).
* Richtung fuer 0x0A/0x0B: RE1.5 ist dort nachweislich UNFERTIG -> RE2 Retail ist das Ziel
  (Memory reai-v2-beta-zu-retail); der Beleg kommt dann aus RE2 (GL-Saeure/-Brand-Runden).

## Nachtrag des Nutzers (2026-09-29, waehrend der RE-Runde), woertlich

> Nein, Resident Evil 1.5 hat auch 2 andere Granaten. Wenn die auch vollkommen kaputt sind in der
> Resident Evil 1.5 Beta. Auch der Schaden von Resident Evil 1.5 für die normale Granate bin ich mir
> nicht sicher ob der existiert, da das Spiel im original unter Nutzung der Granate - zum Beispiel
> gegen Zombies - abstürzt.

Folgerungen fuer die Runde:
* Alle DREI RE1.5-Granaten (0x09/0x0A/0x0B) werden lauffaehig gemacht, auch wenn sie im Beta-
  Auslieferungsstand kaputt sind.
* NUTZER-MESSUNG: Das Original STUERZT AB, wenn die Granate (z.B. gegen Zombies) benutzt wird. Der
  Absturz ist zu reproduzieren und bis zur abstuerzenden Instruktion zu disassemblieren (eigenes
  Dossier `re_absturz_original.md`). Je Teil der Kette (Wurf, Flug, Abprall, Explosionsbild, Ton,
  Schaden, Gegnerreaktion) ist dann belegt einzuordnen: laeuft im Original (-> RE1.5 byte-true) oder
  kaputt/unfertig (-> RE2 Retail als Ziel, Beleg aus RE2 — GL-Explosiv-/Saeure-/Brand-Runde).
* Den RE1.5-Schadenswert der Granate NICHT ungeprueft uebernehmen: existiert er, wird er im Original je
  erreicht (ohne Absturz)?

## Nutzer-Antwort zur Absturz-Beobachtung (2026-09-29), woertlich

> Bei der Explosion wenn sie einen Zombie erwischt. Und ich kam zur Granate, indem ich im player
> Inventory select und so lange r drücke, bis die Granate im Inventar ausgewählt ist. Es gibt in der
> Original Version ein weiteres Debug Menu zur Item Auswahl.

* Absturzbedingung laut Nutzer: **die Explosion erwischt einen Zombie** (Wurf und Flug laufen also).
* Weg zur Granate im Original OHNE Savestate-Patch: im Spieler-Inventar **Select** druecken und dann
  **R (R1)** wiederholt, bis die Granate ausgewaehlt ist — ein weiteres Debug-Menue zur Item-Auswahl
  (im Original vorhanden). Fuer die Reproduktion nutzbar (vgamepad: BACK = Select, RB/RIGHT_SHOULDER = R1).
