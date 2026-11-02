# Stufe 3 Bahnsteig-Form: Bewegung, Angriffe, Einbau (laufendes Dossier)

Auftrag (Orchestrator, nach E19): J bekommt Gerippe-Pruefung, eigene Clips nach dem Muster RE2-G4, das G4-Verhalten aus Spur R auf die
neuen Clips, Einbau in 5080 und 5090 unter eigenem Asset-Namen, Tests, Sichtbelege. Bindend: `NUTZER_ENTSCHEIDE.md` E1-E19.
Arbeitsbaum `.claude/worktrees/r41_form`, Zweig `r41/form`. Dieses Dossier wird fortlaufend geschrieben; wer nach einem Abbruch
weitermacht, liest zuerst Abschnitt 0.

## 0. Stand und naechster Schritt

| Schritt | Stand |
|---|---|
| 1 Sichern + `git merge master` | erledigt: 8423920d4 (Formstand, `.gitignore` im Ordner), f89159cf4 (Merge, keine Konflikte) |
| 2 Gerippe pruefen | laeuft |
| 3 Clips | offen |
| 4 Angriffsmuster | offen |
| 5 Einbau, Tests | offen |
| 6 Sichtbelege | offen |

Generator dieser Stufe: `stufe3/gen/` (`s3_rig.py` Gerippe/Posen/IK, `s3_g4.py` G4-Clipdaten, `s3_blatt.py` Kontaktblaetter,
`s3_lesen.py` Gerippe-Ausgabe). Zwischenbilder unter `stufe3/roh/` (nicht fuer den Nutzer).

## 1. Bestand vor dieser Stufe (gemessen im Quelltext nach dem Merge)

**5080/5081 (Bahnsteig).** RDT sub00 @0x00746 `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00 ..` = Typ 0x30, grid 0x33, Lage
(-18100,-5600,200), solange Flag (3,48) = 0 (@0x0072A). grid 0x33 friert die Wurzel ein (`re15_birkin_root`, Bit 0x20), bis sub03
nach der Generator-Folge Pos_set (-18100,-5600,-17800) @0x00822, Speed_set/Add_speed (Fall auf y 0) und Member_set(0x0C,0x13)
@0x0083E faehrt. Port heute: Original-EM30 kaempft (`re15_birkin_ai_tick`), Tod -> Todesmaschine Phase 2 setzt Flag (5,0x1c)
(@0x8011aecc STAGE5), sub01 @0x0078A sieht das, setzt (3,48)/(3,49), Musik, sub04. **Die Bahnsteig-Form kommt in 5080 nicht vor.**

**5090/5091 (Zug).** sub00 @0x0124A Typ 0x30 grid 0x33, sub04 Member_set(0x0C,0x13) @0x0130A. Port heute: EM30 kaempft, toedlicher
Treffer -> Mutation (Sub 6, Clip 0x14) -> `re15_bf_mutation_fertig` laedt die Bank neu -> RE2/BAHNSTEIG_G4.EMD (G4 umgeformt,
27 Knochen) -> `re15_bb_tick` (engine/src/birkin_bahnsteig.c, Spur R) -> beim Tod Rueckstoss-Szene (`re15_bf_rueckstoss_start`,
birkin_formen.c) -> G5 (Typ 0x36). Belegt in R_bahnsteig.md 8.1 (Leon 5090, Elza 5091 gleich).

**Soll nach E5/E6/E17 (Nutzer):** 5080 Riesenform, Kampf endet mit Flucht; 5090 Riesenform, wird aus dem Zug geschossen; dann G5.
Die Mutation zur Riesenform liegt nach E6/E17 am Ende des letzten Sichel-Form-Kampfs (Spur SF, Baum r41_sichel), also VOR dem Bahnsteig.
Folge fuer diese Stufe: in 5080 und 5090 traegt der 0x30 die neue Form ab der Raumladung; die EM30-Runde samt Mutation in 5090 entfaellt.

## 2. Gerippe J

Gelesen aus `varianten/J_dicker/FORM.EMD` (`stufe3/gen/s3_lesen.py`): 32 Knochen (Port-Grenze RE15_EMD_MAX_BONES = 32, voll), 32 Meshes,
Mesh i an Knochen i, Eltern vor dem Kind.

| Knochen | Eltern | Rolle |
|---|---|---|
| 0 | - | Huefte, Wurzel |
| 1-6 | 0,1,..,5 | Schwanz, Kette von sechs Gliedern nach -X (Versaetze 592..678) |
| 7, 10 | 0, 7 | Wirbel, Brust (= BB_TEIL_RUMPF 10) |
| 8/24/25, 9/26/27 | 0/8/24, 0/9/26 | Bein links/rechts: Oberschenkel, Unterschenkel, Fuss (kantig, Sohle) |
| 11/12/13 | 10/11/12 | Arm links: Oberarm, Unterarm, Hand (= Teil 13, Hieb Seite 1); Klauen 14, 15, 28, 30 |
| 16/17/18 | 10/16/17 | Arm rechts: Oberarm, Unterarm, Hand (= Teil 18, Hieb Seite 0); Klauen 19, 20, 29, 31 |
| 21/22/23 | 10/21/22 | Hals, Kopf, Kiefer |

Alle Teile, die eine Bewegung braucht, haben schon einen eigenen Knochen; Teil 10/13/18 sitzen dort, wo der Tick sie erwartet. Neue Knochen
gingen ohnehin nicht (32 von 32). Posen von J: Clip 0 = Bild 1, Clip 1 = Bild 2, Clip 2 = Ruhe (`form.json`).
