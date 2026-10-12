# Runde 37 Spur H "zombieaktiv" — Dossier (laufend)

Baum `.claude/worktrees/r37_zombieaktiv`, Zweig `r37/zombieaktiv`, Basis master 44881ac2.

## Wortlaut des Nutzers (AUFTRAG.md, Nachtrag 2 + Ergaenzung)

> die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach
> Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2
> bewegen sie sich doch auch teilweise aktiv im Gang.

> das soll nicht nur da gelten, das darf generell für die Zombies gelten

> schaue einfach nach wie Resident Evil 2 das macht

Lesart: GENERELL alle Zombies (jeder Raum, jede Spawn-Art) bekommen das RE2-Retail-Verhalten (teilweise aktiv im Gang, auf
Leon zu); die Montage nach Irons' Tod ist das Beispiel.

## Stand

- [x] Schritt 0: Lektuere (Vorrunde F/K/E, Port-KI enemy_ai_re2_zombie.c)
- [ ] Schritt 1: Messung vorher (Montage-Zombies 1130/1040/1030/1070 + allgemein)
- [ ] Schritt 2: RE-Belege RE2 (Zustandsautomat, Weck-/Sichtbedingungen, Wander)
- [ ] Schritt 3: Umsetzung
- [ ] Schritt 4: Messung nachher
- [ ] Schritt 5: Tests

## Messung vorher

### M1 — echte exe, ROOM1130 nach der Montage (Stand master 44881ac2, Messkopie `re15_pc_r37h_mess.exe`)

Weg: Spielstand + CONTINUE + echte Tuer. Karte `probe_r36_gegnerzahl_karte re15_card.mcr 1150 -17150 -11960 0 9:71 3:94
9:73-76` (ROOM1150 vor der Tuer Slot 0 @0x00D5E nach 1130, Montage gesehen: (9,73)-(9,76)), Aktionstaste -> Tuer ->
ROOM1130 (debug.log `Nachspawn 1130 (Programm 10)` = k_p_1130_nach = EM_1130_LISTE, grid 0x02), RE2-KI (Default),
`RE15_STATE_LOG`. Auswerter `scratchpad/h/ausw.py` (je Zombie Zustandsfolge st/ss1/ss2, Weg, Abstand).

**Lauf a (Leon bleibt an der Tuer (-5900,16350), 1800 Bilder = 60 s):** alle fuenf Zombies (Typ 0x16/0x10/0x10/0x11/0x11,
grid 0x02) bleiben in Zustand 1 / Unterzustand 0 (RE2 EXEC[0] Stehen): Folge `0/0 -> 1/0/0 @6 -> 1/0/1 @7` (Stehen P1),
dazwischen RE2-Wander (P3 @544..812, @876..1232, @1412..1592, @1713..) — Wege 293..9044 Einheiten zufaellig im Gang,
Abstand zu Leon am Ende 20680..28524. **Kein einziger geht auf Leon zu (Unterzustand 1 nie erreicht).**

**Lauf w (Leon geht den Gang hinunter, `R0.33,U25`):** Leon passiert die 1120-Tuer (z -3150..-1150) und laeuft weiter;
die Zombies wechseln erst in Bild 672..702 in den Gang (Unterzustand 1), als Leon bei z ~ -4800 ist (Abstand ~6000-7000,
RE2-Selbstwecker dist < 0x1D4C mit Sicht @0x80101544-7C bzw. DECISION[0] dist < 0x1388 @0x80101308-1C). Leon stirbt
danach im Griff (hp 100 -> -20).

**Ursache (Mechanismus, gemessen):** die Montage-Zombies in 1130 tragen grid 0x02 (re15_irons_tod.h EM_1130_LISTE =
"STEHENDE Form" ROOM1140 sub00 @0x00C12..). Der RE2-INIT bildet jeden RE1.5-Deskriptor ausser 0x0d auf Zustandswort
0x1 = Stehen ab (enemy_ai_re2_zombie.c re2z_init, `else re15_ai_set_state_word(e, 0x1)`), und ein stehender RE2-Zombie
wird nur wach bei dist < 5000 im Vorderkegel +-1024 mit Sicht (DECISION[0] @0x80101308-1C) oder dist < 7500 mit Sicht
nach 150..405 Bildern (@0x80101544-7C), sonst nur Zufalls-Wander (@0x8010151C-40). Der Weg 1150 -> 1120 fuehrt in
>= 8800 Abstand an ihnen vorbei (1120-Tuer z -2150, naechster Zombie z -10800) -> sie bleiben stehen. Das ist genau der
Nutzer-Befund "bleiben einfach nur stumpf alle im Gang stehen".

## RE-Belege

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
