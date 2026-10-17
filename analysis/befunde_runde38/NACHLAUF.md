# Runde 38 — NACHLAUF (vorbestehende Abweichungen, NICHT beauftragt)

Von den Spuren dieser Runde nebenbei gefunden und belegt, aber nicht Teil des Auftrags. Jeder Punkt nennt die Spur, die ihn
gefunden hat, und die Stelle im Dossier mit den Belegen. Kandidaten fuer eine eigene Runde. Laufend ergaenzt.

| # | Fund | Spur / Beleg | Wirkung im Spiel |
|---|---|---|---|
| 1 | Das Bewegungs-Gate des Spielers (player_common.c re15_player_tick) ignoriert Spieler+0x4. Im Original schaltet der Dispatcher @0x80031c8c bei +0x4 == 4 auf den Executor @0x80073f90[4] = 0x80030660 statt auf den Pad-Zweig [1] = 0x80031de8. | B, `B_szenenschutz.md` N1.OFFEN | Bei kurzen Skript-Bewegungen ohne Text (ROOM1230 sub21: 30 Bilder) kann Leon im Port laufen, im Original nicht. Korrektur braucht einen Zensus aller Plc-Subs (jedes state 4 braucht ein erreichbares Plc_ret). |
| 2 | Leons Modellteil 15 (6 Ecken, an Teil 2 mit Versatz (-9,0x66,-0x94), Flag 1) ist im Original an (@0x80031a00-74), der Port zeichnet es nicht. | D, `D_zopf.md` §2.3 N1 | Kleines Teil an Leons Modell fehlt. |
| 3 | Die Port-RotMatrix (skeleton_common.c mat3_from_euler) hat keinen Negativ-Zweig wie das Original @0x80068098. | D, `D_zopf.md` §2.3 N2 | Alle Knochen weichen bei negativen Winkeln um einen Tabellenschritt ab. |
| 4 | Gehtempo: Port gleichmaessig 74-75 Einheiten/Bild (WALK_SPEED_PER_FRAME 75), Original 55..90 je nach Schritt (D) bzw. 76..80 (S, Messung o10). | D N3, S | Zopf-Winkel 3-4 Grad anders; Bahnen laufen auseinander. |
| 5 | Nacken-Gier in ROOM1121: Port dreht auf +-512 (Kraehe EM021), in der GDB-Messung des Originals blieb sie 0 (ob die Kraehe dort aktiv war, ist nicht gemessen). | D, N4 | Elza dreht den Kopf zur Kraehe. |
| 6 | Zufallsquelle FUN_8001af20 haengt im Original am a0 des Aufrufers; der Port nimmt einen Ersatz. | S, `S_affen3.md` OFFEN | Nach Zufallsentscheiden laufen Gegnerbahnen auseinander. |
| 7 | Rueckstoss nach einem Biss beginnt im Port ein Bild spaeter als im Original. | S (Messung o10) | Kaum sichtbar. |
| 8 | Anzeige-Bits aca3c des Spieler-Hakens cmd 5 (@0x80115df8 / @0x80116020) haben im Port kein Gegenstueck (wie beim Gorilla-Finisher). | S | Unbekannt (Anzeige waehrend Griffen). |
| 9 | RE1.5-KI-Schatten der Zombie-Wurzel: Original zeichnet ihn in jedem Aufruf auf +0x1ba (@0x801004fc/@0x80100500), Port nur waehrend 0x0A. | U, `U_wasserzombie.md` | Schatten nach Abbruch des Hochziehens / beim Schwerkraft-Sturz an falscher Hoehe. |
| 10 | CONTINUE in ROOM2000: main.c s_preintro haelt sub00 zurueck, wenn main00 beim Boot ein Ereignis startet (@0x0177c Evt_exec sub07). | U | Nach CONTINUE in ROOM2000 keine Gegner und keine Ada/John-Szene (ueber Tuer korrekt). |
| 11 | RE2-Partner-Domino der Griffphase P8 (sw 0x901 @0x80102E90-98) liest contact_slot ohne Aktualitaetspruefung. | U | Zweiter Kletterer in ROOM2000 wird unterbrochen (F408). |
| 12 | NPC-Schritt-Ton: NPC-Zweig von FUN_80045630 (`addiu s3,a0,-8` @0x8004575c) nicht abgebildet. | M, `M_npckante.md` OFFEN (Adresse dort falsch zitiert, richtig @0x8004575c) | NPC-Schritte stumm. |
| 13 | Der PC teilt Vierecke an der Diagonale 0-3, die PSX an 1-2. | W, `W_rolltor20a0.md` §9.6 | Texturverzerrung an nicht ebenen Vierecken. |
| 14 | Kommentar enemy_ai_common.c (Schwerkraft 0x8001bd60 "hier noch nicht portiert") veraltet, seit M sie verdrahtet. | M Abnahme 0 | Nur Doku. |
| 15 | RE2-Spuck-Entscheid liest kein Spieler-Tor (@0x80100784-8c0): eine freie RE2-Spinne spuckt auch waehrend einer Szene, der Klumpen wird erst am Spieler abgefangen. | R, `R_spinnen.md` N1.M3 | Original-Verhalten, nur vermerkt. |
