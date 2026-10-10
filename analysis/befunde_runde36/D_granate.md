# Runde 36 — Spur D: Granate (HE ohne Feuer an Zombies; Wurf-Aussetzer)

Stand: 2026-10-10, Zweig `r36/granate`, Baum `.claude/worktrees/r36_granate`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz). Auftrag: `AUFTRAG.md` (woertlich), Vertrag: `VERTRAG.md`.
Vorrunde: `analysis/befunde_runde35/A_granate.md` (Wand/Reichweite/Sound/Brutalitaet), `B_werfer.md` (Granatwerfer).

## 0. Auftrag (Wortlaut des Nutzers)
1. "Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der Körperteile.
   [Feuer] würde ich nur bei den incendiary Granaten erwarten."
2. "Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen."

## 1. Ausgangslage (Stand master 5a9dbb7b, selbst nachgelesen)
* Handgranate Art 2 (Item 9), Saeure Art 3 (0x0A), Brand Art 4 (0x0B). Routinen 30/29/31 in re15_esp.c; Wand/Reichweite/
  Explosion in granate_r35.c (Runde 35 Spur A); Explosion stempelt RE2-Zeile 9, Klammer 0, Zone 1 -> DEATH[9][1] =
  0x80108BEC `re2z_death_rip` (Zerreissen) bzw. 0x80109610 (Wegschleudern).
* `re2z_death_rip` Phase 0 ruft fuer Zeile 9/17 ohne +0x10E&0x80 `re2z_gore_soot` = FUN_8010640C ("Spreng-Russ",
  `jal 0x8010640C` @0x80108CEC). Diese spawnt RE2-Effekt-Ids 0x05032710 (Part 0, v={0,200,0}, @0x80106418-44) und
  0x050313E8 (Part 3 R-Unterarm, @0x80106450-70) und setzt die Russ-Tinten (@0x8010648C-F8).
* Port-Dekoder `re2z_gore_fx_ex` (enemy_ai_re2_zombie.c:1107): RE2-Klasse 5 -> RE1.5-CORE00-Id 8 (= FEUER), "Klassen-
  Zuordnung, Sichtpruefung offen" (Runde 4, gore-vollausbau.md §2.1). Dieselbe Klasse 5 nutzt die Brand-Verkohlung
  FUN_80106128 (Zeile 10/16).

(Fortsetzung: Messung vorher / RE-Belege / Umsetzung / Messung nachher / Tests / OFFEN / Fuer den Nutzer)
