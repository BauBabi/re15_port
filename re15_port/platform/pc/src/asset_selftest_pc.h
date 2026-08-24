/* =============================================================================================
 * RE1.5 Rebuilt — ASSET-SELBSTTEST (Paket-Gate, 2026-08-24).
 *
 * NUR DIAGNOSE. Dieses Modul liest, es entscheidet nichts: es fasst weder die Aufloeser-Logik
 * (asset_root_pc.c) noch irgendein Spiel-Verhalten an. Es beantwortet genau eine Frage —
 *
 *     "Findet DIESE exe, aus DIESEM Arbeitsverzeichnis gestartet, ihre kritischen Assets,
 *      und aus WELCHEM Verzeichnis kommen sie tatsaechlich?"
 *
 * WARUM ES DAS GIBT: der 0.3.19-Report ("Es fehlen die Optionen in config, die subtitles usw.")
 * war ein Paket-Fehler, den KEIN bestehendes Gate sehen konnte. release/make_package.sh prueft
 * bis heute nur, ob DATEIEN im Paketordner LIEGEN — nicht, ob das Programm sie zur Laufzeit
 * FINDET. Genau dazwischen lag der Fehler: die Dateien lagen korrekt im Paket, aber die exe
 * suchte an einem einkompilierten Pfad, den es beim Nutzer nicht gibt.
 *
 * SCHLIMMER: auf der Entwickler-Maschine zeigt dieser Compile-Default INS REPO. Ein Test, der
 * nur "wurde die Datei gefunden?" fragt, ist deshalb hier IMMER gruen — auch fuer ein voellig
 * kaputtes Paket. Deshalb meldet der Selbsttest zu JEDEM Treffer den GEWINNENDEN VOLLPFAD;
 * das Gate kann so verlangen, dass die Assets aus dem PAKET kommen und nicht heimlich aus dem
 * Repo nebenan. Das ist die eigentliche Absicherung.
 *
 * AUFRUF: RE15_ASSET_SELFTEST=1 re15_pc.exe
 *   -> Bericht nach stderr (also in die debug.log) UND nach stdout, dann exit:
 *      0 = alles gefunden, 2 = mindestens ein kritisches Asset fehlt.
 *   Laeuft VOR jeder Fenster-/Audio-Initialisierung: kein Fenster, kein Ton, ~50 ms.
 * ============================================================================================= */
#ifndef RE15_ASSET_SELFTEST_PC_H
#define RE15_ASSET_SELFTEST_PC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Ist RE15_ASSET_SELFTEST gesetzt (und nicht "0"), Bericht ausgeben und den Prozess beenden
 * (exit 0 / 2). Sonst kehrt die Funktion sofort und wirkungslos zurueck. */
void re15_pc_asset_selftest_maybe_run(void);

#ifdef __cplusplus
}
#endif
#endif /* RE15_ASSET_SELFTEST_PC_H */
