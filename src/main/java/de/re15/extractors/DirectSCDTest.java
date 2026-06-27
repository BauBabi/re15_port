package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Direkter SCD Test für die Sce_em_set Parameter-Korrektur
 */
public class DirectSCDTest {

    public static void main(String[] args) {
        System.out.println("=== DIREKTER SCD TEST ===");

        try {
            // Erstelle extracted Verzeichnis
            Files.createDirectories(Paths.get("extracted"));

            // Führe SCD-Disassembly direkt aus
            SCDScriptDisassembler disassembler = new SCDScriptDisassembler();
            disassembler.disassembleAllSCDFiles("info/Re1.5", "extracted");

            System.out.println("SCD-Test abgeschlossen!");

            // Überprüfe Ergebnisse
            checkResults();

        } catch (Exception e) {
            System.err.println("Fehler: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void checkResults() {
        try {
            Path resultFile = Paths.get("extracted/PSX/DATA/ROOM115U.c");
            if (Files.exists(resultFile)) {
                String content = Files.readString(resultFile);
                System.out.println("\n=== ERGEBNIS ===");

                // Suche nach Sce_em_set
                if (content.contains("Sce_em_set")) {
                    String[] lines = content.split("\n");
                    for (String line : lines) {
                        if (line.contains("Sce_em_set")) {
                            System.out.println("Gefunden: " + line.trim());

                            // Vergleiche mit Referenz
                            String expected = "Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480);";
                            String actual = line.trim();

                            if (actual.contains(expected.substring(12, expected.length()-2))) {
                                System.out.println("✅ KORREKTUR ERFOLGREICH!");
                            } else {
                                System.out.println("❌ Parameter noch falsch");
                                System.out.println("Erwartet: " + expected);
                                System.out.println("Aktuell:  " + actual);
                            }
                            break;
                        }
                    }
                } else {
                    System.out.println("❌ Keine Sce_em_set gefunden!");
                }
            } else {
                System.out.println("❌ Ergebnis-Datei nicht gefunden: " + resultFile);
            }
        } catch (Exception e) {
            System.err.println("Fehler beim Überprüfen: " + e.getMessage());
        }
    }
}
