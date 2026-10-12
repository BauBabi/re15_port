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

#endif
