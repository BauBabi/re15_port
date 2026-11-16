/* re15_ultimate15.h — Runde 41 Optionen: die Port-Option "ULTIMATE 1.5" (OPTIONS-Schirm, Standard AUS).
 *
 * NUTZER-VORGABE (16.11.2026, woertlich): "Zusaetzlich wuerde ich gerne eine weitere Option hinzufuegen wollen:
 * Ultimate 1.5 zum ein oder ausschalten. Wenn das eingeschaltet ist, bekomme alle Items im Player Inventory Menu und
 * auch die Sachen die man in der Welt aufnehmen kann, ihre haesslichen Icons/Models von der unmodifizierten Resident
 * Evil 1.5 Version."
 *
 * ⛔ KEIN Original-Verhalten — eine Port-Option wie OPTIONS->AI. Sie tauscht nur BILDER/MODELLE (Assets), keine Regel,
 * keine Zahl, keinen Ablauf. AUS = der heutige Stand Byte fuer Byte (die Leser nehmen dann denselben Puffer wie vorher).
 *
 * QUELLE = die unveraenderte Capcom-Disc (Version_40%/PSX, Exe-Stempel "CAPCOM-Nov  6 1996"), NICHT der Fan-Mod-Baum
 * shared_assets/PSX. Die gebrauchten Dateien liegen als UNVERAENDERTE Kopien unter shared_assets/PSX/DISC40/
 * (dort, damit make_package.sh und der Android-Bau sie mit dem PSX-Baum mitnehmen):
 *   DISC40/DATA/ITEMALL.PIX   86400 B  sha256 c8cdba045516b34f441954556a163e39861c6933e6e45452e22d0520882c9ebf
 *   DISC40/DATA/MIXITEM.PIX   16800 B  sha256 0bc36593b6908427b5cd9b7bf6d9f09213302553847a0667947a7d9886d7a7b0
 *   DISC40/ITEM/ITPS.ITP     872448 B  sha256 0b6425c0f67f810620e1fc5db5515a0b48724663f98cccbf2e326d2665dba9db
 *   DISC40/STAGE1/ROOM1020.RDT 158400 B sha256 69ea8bc103a7e0b23e077f69022266340ffe279411b72a94b254a5c222bc59ea
 *   DISC40/STAGE1/ROOM11B0.RDT 174256 B sha256 514210cfe2c8011c9ef48095451c8bb6dae8b32187261198e0bbe0bb751ca372
 *   DISC40/STAGE4/ROOM4010.RDT 245312 B sha256 14729b69e4f6d8bf3daa70f973c29dae74f8c80db04fa0d6a8775de5d99340e1
 *
 * AUFBAU (gemessen, Dossier BERICHT_r1.md; beide Exe-Staende lesen dieselben Formate):
 *   ITEMALL.PIX = 72 Kacheln 40x30 8bpp ohne Kopf, Kachel = Item-Id (re15_inv_screen_cache_tile), Farben aus
 *                 ST_00.TIM CLUT-Zeile 0 (Disc und Mod bitgleich: sha256 6519281f...). MIXITEM.PIX = 14 Kacheln,
 *                 Bild k == ITEMALL-Kachel des Misch-Ergebnisses (in BEIDEN Staenden nachgemessen).
 *   ITPS.ITP    = je Item-Id ein 0x3000-Block (Ladeweg LBA = 0x1740 + Id*6 @0x8001e450-58, in Disc- UND Mod-Exe
 *                 bytegleich): TIM 8bpp, CLUT-Daten +0x14 (256 Eintraege), Bild 112x72 ab +0x220, Breitbild 80x30
 *                 ab +0x21A0 (2-Feld-Waffen 0x0e..0x13). Die Disc hat 71 Bloecke (0x00..0x46), der Mod 72.
 *                 Die Disc-Bloecke tragen die Rechtecke crect (0,480) / prect (0,0); der Item-Modal-Lader
 *                 FUN_8004ee78 vergibt den VRAM-Platz selbst (@0x8004eea8-ed4, in beiden Exe bytegleich), die
 *                 eingebetteten Rechtecke zaehlen dort nicht. Der Port setzt deshalb nur CLUT (+0x14..+0x214),
 *                 Bild (+0x220..+0x21A0) und Breitbild (+0x21A0..+0x2B00) ein und laesst den Mod-Kopf stehen.
 *   Weltmodelle = Raum-Props der RDT (Tabelle RDT+0x30, je Prop TIM,MD1), verknuepft ueber Item_aot_set (0x50)
 *                 Nutzlast tk_prop (pc[20] / lang pc[28], scd_vm.c op_item_aot_set).
 *
 * ZUORDNUNG — je Id, nach Bedeutung (Namen: Disc-PSX.EXE Datei-Offset 0x64021.. (Id 1 "コンバットナイフ") = RAM 0x80073821.., je Name ein 0x07 davor
 * getrennt, japanisch, dekodiert mit
 * analysis/story_begegnungen/werkzeuge/jp.py; Mod BIN/DEBUG.BIN @0x4a28 englisch):
 *   - gleiche Bedeutung + Disc-Bild ist gegenstandseigen -> Disc-Bild.
 *   - KEIN Disc-Bild (bleibt beim heutigen Bild):
 *       a) die Disc zeigt dort nur einen allgemeinen PLATZHALTER: ITEMALL-Kachel bytegleich mit Kachel 0x03
 *          (Browning) = 0x2F 0x30 0x31 0x32 0x35 0x42 0x45; ITPS-Bild bytegleich mit der "BOSS"-Karte von Block 0x00
 *          = alle Bloecke ausser 0x08 0x09 0x15 0x16 0x36 0x3E.
 *       b) die Id bedeutet heute etwas anderes: 0x14 (Disc Feuerloescher, Mod Colt Python), 0x33/0x34 (Disc
 *          MD-Medium/-Spieler, Port Kurbel/Wasserschluessel), 0x43 (Disc Kommunikationskarte, Port Speisesaal-
 *          Schluessel), 0x42/0x45 (Port-Schluessel).
 *   - UMZUG nach Bedeutung: Disc-Icon 0x14 (しょうかき = Feuerloescher) -> heutige Id 0x31 (Fire Extinguisher);
 *     Disc-ITPS-Bild 0x3E (zeigt die Taschenuhr, Disc-Name dort Gワクチン) -> heutige Id 0x3F (Pocket Watch).
 */
#ifndef RE15_ULTIMATE15_H
#define RE15_ULTIMATE15_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_U15_DISC_DIR       "DISC40"            /* unter der CD-Wurzel shared_assets/PSX                   */
#define RE15_U15_ENV            "RE15_ULTIMATE15"   /* Messhaken: 0/1 uebersteuert die Options-Datei beim Laden  */
#define RE15_U15_SCHLUESSEL     "ultimate="         /* Zeile in re15_options.cfg (nur bei AN geschrieben)        */

#define RE15_U15_KACHEL_B       1200                /* 40 x 30 x 1                                               */
#define RE15_U15_KACHELN        72                  /* ITEMALL.PIX                                               */
#define RE15_U15_MIX_KACHELN    14                  /* MIXITEM.PIX                                               */
#define RE15_U15_ITPS_BLOCK     0x3000
#define RE15_U15_ITPS_CLUT      0x14                /* .. +0x214                                                 */
#define RE15_U15_ITPS_CLUT_N    0x200
#define RE15_U15_ITPS_BILD      0x220               /* 112 x 72 = 0x1F80 B -> .. +0x21A0                          */
#define RE15_U15_ITPS_BILD_N    0x1F80
#define RE15_U15_ITPS_BREIT     0x21A0              /* 80 x 30 = 0x960 B -> .. +0x2B00                            */
#define RE15_U15_ITPS_BREIT_N   0x960

/* ---- Schalter ------------------------------------------------------------------------------------------------ */
int      re15_ultimate15(void);              /* 1 = AN                                                              */
void     re15_ultimate15_set(int an);        /* 0/1; jede Aenderung zaehlt die Generation hoch                       */
unsigned re15_ultimate15_generation(void);   /* fuer Leser, die beim Wechsel neu aufbauen                            */

/* Options-Datei: liefert 1/0 fuer "ultimate=on|off", -1 wenn der Schluessel fehlt. */
int re15_ultimate15_parse(const char *text);
/* Beim Laden der Options-Datei (lautstaerke_r41.c): Wert aus dem Text (fehlt er -> AUS), danach der Messhaken
 * RE15_ULTIMATE15 (nur Harness/Tests). */
void re15_ultimate15_aus_text(const char *text);

/* ---- Zuordnung je heutiger Id (-1 = kein Disc-Bild, das heutige bleibt) ---------------------------------------- */
int re15_ultimate15_icon_quelle(unsigned id);    /* Disc-ITEMALL-Kachel                                             */
int re15_ultimate15_bild_quelle(unsigned id);    /* Disc-ITPS-Block (Bild 112x72 + CLUT)                            */
int re15_ultimate15_breit_quelle(unsigned id);   /* Disc-ITPS-Block (Breitbild 80x30, 2-Feld-Waffen)                */

/* Weltmodell eines Gegenstands: Disc-RDT (relativ zur CD-Wurzel) + Prop-Index; 0 = keins. */
int re15_ultimate15_modell_quelle(unsigned item, const char **rdt_rel, int *prop);

/* ---- Puffer bauen: dst = basis (heutiger Stand inkl. Port-Einsaetze) + Disc-Bilder nach der Zuordnung -------- */
/* Rueckgabe = Zahl eingesetzter Teile, -1 bei unpassender Groesse. dst darf nicht mit basis/disc ueberlappen. */
int re15_ultimate15_itemall_bauen(uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn);
int re15_ultimate15_mixitem_bauen(uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn);
int re15_ultimate15_itps_bauen   (uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn);

/* ---- Welt: welche Props der Raum-RDT zeigen welchen Gegenstand ------------------------------------------------- */
/* Laeuft main- und sub-SCD der RDT ab (Regionen wie re15_port/tools/scd_dump_room.py, Laengen aus
 * scd_opcode_size_at) und traegt fuer jedes Item_aot_set item -> out[tk_prop] ein (0xFF = kein Gegenstand).
 * Rueckgabe = Zahl der gefundenen Item_aot_set. */
int re15_ultimate15_prop_items(const uint8_t *rdt, int size, uint8_t *out, int n);

#ifdef __cplusplus
}
#endif
#endif /* RE15_ULTIMATE15_H */
