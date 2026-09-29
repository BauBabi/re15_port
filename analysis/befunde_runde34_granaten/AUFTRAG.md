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
