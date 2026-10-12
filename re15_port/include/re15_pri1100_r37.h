#ifndef RE15_PRI1100_R37_H
#define RE15_PRI1100_R37_H
/*
 * Runde 37 Spur A (pri1100): Messhaken fuer die Nutzer-Marke befund_1100_F892_marke1.bmp.
 *
 * RE15_GEGNER_POS="<slot>,<x>,<z>,<rot>@<bild>#<raum-hex>" setzt den Akteur g_actors[slot] ab Bild <bild> des
 * Raums <raum-hex> auf die Weltlage (x, 0, z) mit Blickrichtung rot und haelt ihn dort (jedes Bild). Grund: die
 * Marke F892 zeigt Zombie 1 an (-16157,0,-23876) rot 1024, die Spawnlage ist (-16453,0,-23817); ohne Haken
 * laesst sich die Szene des Nutzers nicht bildgleich nachstellen (der liegende Zombie st=1 ss=7/1 bewegt sich
 * in 900 Bildern nicht, Messung m3_lang im Dossier analysis/befunde_runde37/A_pri1100.md).
 * Reiner Messhaken (wie RE15_PLAYER_POS): ohne die Variable kostet er einen Zeigervergleich je Bild.
 */
void re15_r37a_gegner_pos_haken(void);

/*
 * Runde 37 Spur A, Nachbesserung 1 (VERTRAG.md 1.7, Abnahme A#0 M1): MASKEN-ZWILLING Elza -> Leon.
 *
 * Liefert die Raum-Id, deren nachgezeichnete Port-Masken (MASKS/ROOM%04X.MSK und MASKS/ROOM%04X_PRI%02d.TIM)
 * fuer room_id gelten: die gerade Leon-Variante, wenn room_id eine Elza-Variante (ungerade Id) OHNE eigene MSK ist
 * und Kamerasaetze UND Hintergrund je Cut beider Raeume nachweislich gleich sind; sonst room_id selbst.
 * Befund: Elza sah in ROOM1101 (RDT bytegleich zu ROOM1100) Zombie 1 ueber der Westwand, weil der Port die Masken
 * mit der VOLLEN Raum-Id sucht und es in STAGE1 nur Leon-Masken gibt (ausser ROOM1221).
 * Original (warum die Bedingung reicht): Masken = f(RDT-Kamerasatz) @0x80039310/14/1c (RDT+0x24, Cut*0x20,
 * Satz+0x1C); Projektion = f(Kamerasatz) @0x80021e68 (lhu +2 fov, >>7 -> H) und @0x80021e90 -> FUN_80053ca4 (a0 = Satz
 * @0x80053cac addu s3,a0,zero; liest pos +4/+8/+12 und Ziel +16/+20/+24, z.B. @0x80053d1c lw v0,16(s3) / @0x80053d20 lw v1,4(s3));
 * Hintergrund = f(Stage, Raum OHNE Varianten-Ziffer, Cut) @0x80021c88..0x80021cd8 (Stage-Tabelle 0x8007438c).
 * Tabelle: gen/pri_masken_zwilling_r37.inc (erzeugt von tools/maske/zwilling_zensus.py, Riegel
 * unit_r37_pri1100_zwilling rechnet den Zensus aus den Dateien nach).
 */
unsigned re15_pri_masken_raum(unsigned room_id);

#endif
