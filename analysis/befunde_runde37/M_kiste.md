# Runde 37 — Spur M "kiste" (Item-Kiste: Symbole rechts, Rahmen ueber den Items)

Baum `.claude/worktrees/r37_kiste`, Zweig `r37/kiste`, Basis 70030492. Wortlaut: AUFTRAG.md Nachtrag 9.

## Wortlaut des Nutzers (Nachtrag 9, woertlich)

> In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden.
> Außerdem ist mir aufgefallen, das beim scrollen/bewegen in der Kiste währendessen die Items oberhalb des Kastens liegen.
> Das darf nicht sein, der Kasten muss layertechnisch über den Items liegen.

Punkte:
- **M1** Symbole so weit rechts wie moeglich, kein Name ueberdeckt ein Symbol.
- **M2** Beim Scrollen liegt der Rahmen ("Kasten") layertechnisch ueber den gleitenden Item-Zeilen.

## 1. Messung vorher (exe dieses Baums, Stand 70030492)

Weg: echter Spielstand + echte Aktionstaste, kein Sprung. Kartenwerkzeug `probe_r37_kiste_karte` (neu, s. Tests)
schreibt ROOM1150, Leon bei (-25650,-12100) Gier 0 (Blick +x; Aktions-Vorwaertspunkt 620 vor der Figur,
aot_common.c "FORWARD-620", liegt im Kisten-AOT main00+0x70 `2C 05 03 31 00 00 F4 9D C8 CE 40 06 E8 03`
= x -25100..-23500 / z -12600..-11600). Kiste: Ring 63 = 0x15 x100, 0 = 0x44 "Minidisc Player w/ Disc",
1 = 0x0F Grenade Launcher x6 (2-Feld, Size 3), 2 = 0x43, 3 = 0x1D x12, 4 = 0x47, 5 = 0x46, 6 = 0x0B x3.
exe: `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=1 RE15_PAD_AT=40:A,90:A,100:D,120:U
RE15_FRAMEDUMP=95-130/1:f` (A = Viereck: F40 Kiste oeffnen ueber das AOT, F90 Box-Seite, F100 RUNTER, F120 HOCH).

**M1 vorher** (Bild `M_belege/vorher_1150_zeilen_zoom.png`, F90, 4-fach): Symbol-Spalte ICON_X 158, Namen ab NAME_X 16.
"Minidisc Player w/ Disc" laeuft mit dem "c" IN das Symbol (Name liegt oben, Op-Liste: Name vor Symbol);
"Empty Grenade Shells" beruehrt es mit dem "s". Das Grenade-Launcher-Symbol (2-Feld, 49 px) reicht bis x 206 —
ueber die innere Rahmenlinie (x 204) an die Scrollschiene (x 206). Die Symbole haben einen DECKENDEN Grund
(ITEMALL-Kachel, CLUT-Index != 0), Ueberlappung = Rechteck-Ueberlappung.

Zensus ALLER 101 Namen der Namensbank (gen/inv_name_bank.inc, Glyphen aus DATA/TEX.TIM Spalten 256..511 wie
inv_render_pc.c, Vorschub include/font_width.h, Start NAME_X 16), rechtestes gemaltes Pixel:

| Id | Name | rechtes Pixel | vorher (Symbol ab 158) |
|---|---|---|---|
| 0x44 | Minidisc Player w/ Disc | 173 | ueberdeckt 16 px |
| 0x5B | K. Sullivan - R. Aiken | 171 | ueberdeckt 14 px |
| 0x59 | E. Dewey - K. Dooley | 164 | ueberdeckt 7 px |
| 0x5A | J. Frost - F. Speyer | 159 | ueberdeckt 2 px |
| 0x1D | Empty Grenade Shells | 158 | ueberdeckt 1 px |
| 0x43 | Communications Card | 155 | frei |
| 0x47 | Blue Master Keycard | 153 | frei |
| (2-Feld 0x0E..0x13) | Grenade Launcher | 137 | frei |

Dazu (andere Spur, nach der Integration im Spiel): Spur I "Empty Fire Extinguisher" (Id 0x31 bei Leon nach dem
Loeschen, r37/feuerloescher feuerloescher_r37.c k_name_leer) = 177.

**M2 vorher** (Bilder `M_belege/vorher_1150_runter_F100-107.png`, `vorher_1150_gleiten_kopf_fuss.png`):
RUNTER (RE2 Zustand 3, Versatz -3 je Bild): die Zeilen gleiten nach oben, "Minidisc Player w/ Disc" samt Symbol
liegt UEBER dem Kopfband "ITEM BOX"; unten waechst eine leere Luecke (die einlaufende Zeile fehlt). HOCH: "Red
Master Keycard" liegt ueber dem Fussband und dem grauen Pfeil. Pixelzaehlung `glide_check.py` (Pixel AUSSERHALB
des Listenfensters W = x 13..203 / y 52..152, innerhalb des Listen-Panels x 6..219 / y 25..180, Griffspalte
208..213 ausgenommen, gegen das Ruhebild F99):

| Bild | anders | davon oben | unten | seitlich |
|---|---|---|---|---|
| F102..F107 (runter, Versatz -3..-18) | 98 / 260 / 521 / 915 / 1319 / 1509 | bis 1417 | 0 | 48..92 |
| F108..F121 (Ruhe nach dem Schritt) | 98 | 0 | 0 | 98 (2-Feld-Symbol x 204..207 ueber dem Rahmen) |
| F122..F127 (hoch, +3..+18) | 136 / 295 / 691 / 1097 / 1317 / 1433 | 0 | bis 1391 | 42..86 |

(Anzeige 1 Bild versetzt: Taste in F100 -> Gleiten sichtbar F102..F107, Commit F108.)

Ursache im Code: re15_inv_screen.c build_box_mode — die Op-Liste wird von HINTEN gerastert (frueher Eintrag liegt
oben). Block 7b (Zeilen) kommt VOR dem Panel-Op (BOXPANEL.BIN mit Rahmen, Kopf-/Fussband UND Listengrund), also
liegen die Zeilen ueber dem ganzen Panel. Ein Fenster/Abschneiden gibt es nicht.
