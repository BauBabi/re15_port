/*
 * RE1.5 Rebuilt — Runde 42 Strang "namen", Stufe 6: VERALTETE AUFNAHMEN bleiben stumm, bis die neue Aufnahme da ist.
 *
 * BEFUND (AUFNAHMELISTE "Warum diese 15 Saetze zuerst?", am 19.11.2026 am Bestand nachgemessen): die Story v0.8.31 gibt 15
 * Saetzen einen neuen Untertitel, unter Raum und Nummer liegt aber noch die Aufnahme des ALTEN Satzes (Dateien bytegleich zum
 * Stand des Tags v0.8.30). Die Sprachausgabe spielt jede Datei unter synchro/STAGE<n>/room<RAUM>/main<NN>.wav, ohne auf den
 * Text zu achten (audio_pc.c re15_voice_load_clip) - man hoerte also den alten Satz unter dem neuen Text, in ROOM6030 sogar mit
 * dem falschen Sprecher (Sherrys alte Stimme unter Leons "Where is she now?", Johns alte unter Sherrys "Elza's still out
 * there...!"). Betroffen: 3020 main00/01/02; 30E0 main03/10/11/13; 6030 main02/03/04/05/21/22; 6031 main01/02.
 *
 * PORT-WAHL (Entscheid des Orchestrators 19.11.2026, keine Original-Frage: RE1.5 hat keine englische Sprachausgabe, synchro/
 * ist Eigenproduktion des Projekts - scd_vm.c scd_queue_voice): eine Datei, die NACHWEISLICH noch die alte Aufnahme ist, gilt
 * wie eine FEHLENDE Datei. Der Satz laeuft stumm mit Untertitel und Textdauer, auf demselben Weg wie jeder Satz ohne Datei
 * (re15_voice_load_clip gibt 0 zurueck, der Sprachkanal startet nicht, g_re15_voice_laeuft bleibt 0). Sobald der Nutzer die
 * Datei durch seine neue Aufnahme ersetzt, spielt sie von selbst - sie hat einen anderen Fingerabdruck.
 * Das loest den Entscheid vom 16.11.2026 ab (Story STAGE3b Nachbesserung 2, R2-1: "alte Aufnahme spielt unter neuem Text").
 *
 * NACHWEIS = Fingerabdruck: Pfad der Datei + Groesse in Byte + FNV-1a 64 bit ueber den ganzen Inhalt, gemessen am Stand des
 * Tags v0.8.30 (tools/r42_namen/altaufnahmen_bauen.py -> gen/altaufnahme_r42_tabelle.inc; dort je Datei auch git-Blob und
 * SHA-256). Aendert sich ein einziges Byte, ist die Datei nicht mehr "alt". Keine andere Datei ist betroffen: der Pfad gehoert
 * zum Abdruck. Umleitungen und Spiegelraeume (stimmen_r39.c 1050/1150/11B0/1171, elza_r36.c 1011, Rekorder r38 1100/1101,
 * Hilferufe r39 1141/1041/1131, Elza-Montage r38 1241 <- 1240) fuehren nicht auf diese 15 Dateien (Riegel
 * unit_r42_namen_altaufnahme).
 */
#ifndef RE15_ALTAUFNAHME_R42_H
#define RE15_ALTAUFNAHME_R42_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t    raum;      /* gepackte Raum-Id wie der Ordnername (0x30E0 -> room30E0)          */
    uint8_t     datei;     /* Nummer der Datei main<datei>.wav = Message-Id der Zeile            */
    uint32_t    groesse;   /* Dateigroesse in Byte (Stand Tag v0.8.30)                           */
    uint64_t    fnv;       /* FNV-1a 64 bit ueber die ganze Datei                                */
    const char *pfad;      /* "synchro/stage<n>/room<raum>/main<nn>.wav", klein geschrieben      */
} re15_altaufnahme_r42_t;

int re15_altaufnahme_r42_anzahl(void);
const re15_altaufnahme_r42_t *re15_altaufnahme_r42_eintrag(int i);

/* FNV-1a 64 bit (Startwert 0xcbf29ce484222325, Faktor 0x100000001b3). */
uint64_t re15_altaufnahme_r42_summe(const uint8_t *bytes, size_t n);

/* 1 = die Datei <relpfad> (ab "synchro/", Gross/Klein und '\' egal) mit genau diesen Bytes ist die alte Aufnahme eines Satzes
 * mit neuem Text -> der Lader behandelt sie wie "keine Datei". Sonst 0. */
int re15_altaufnahme_r42_ist(const char *relpfad, const uint8_t *bytes, size_t n);

#endif
