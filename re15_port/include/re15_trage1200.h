/*
 * RE1.5 Rebuilt — ROOM1200: der Zombie von der Trage (Runde 35 Spur H, Punkt 3).
 *
 * Nutzer (AUFTRAG.md Runde 35): "Bei ROOM 1200 nach dem aufnehmen des Minidisc players steht der
 * Zombie von der Trage auf und laeuft durch die Luft, statt wie im Original danach auf die Spieler
 * Ebene runter zu kommen."  Dossier: analysis/befunde_runde35/H_raeume.md, Punkt 3.
 */
#ifndef RE15_TRAGE1200_H
#define RE15_TRAGE1200_H

#include <stdint.h>

/* Messschiene RE15_GEGNER_Y_LOG=<datei> (env-gegatet, kein Spielverhalten): je Spielbild eine Zeile
 * je aktivem Gegner — Typ, Zustand, grid, Clip/Bild, x/y/z, Band (+0x82), Boden-Referenz. Gerufen
 * aus der PC-Hauptschleife hinter re15_game_step. Auf PSX ein No-op. */
void re15_trage1200_mess(void);

#endif /* RE15_TRAGE1200_H */
