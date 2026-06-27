package de.re15;

import de.re15.extractors.RE15MasterExtractor;

/**
 * Haupteinstiegspunkt fuer den Resident Evil 1.5 Daten Extraktor.
 *
 * <p>Verwendung:</p>
 * <pre>
 *   java -jar re15-extractor.jar [optionen]
 *
 *   Optionen:
 *     --source &lt;pfad&gt;    Quellverzeichnis (Standard: info/Re1.5)
 *     --target &lt;pfad&gt;    Zielverzeichnis (Standard: extracted)
 *     --help             Zeigt diese Hilfe an
 * </pre>
 */
public class Main {

    private static final String DEFAULT_SOURCE = "info/Re1.5";
    private static final String DEFAULT_TARGET = "extracted";

    public static void main(String[] args) {
        System.out.println("=== Resident Evil 1.5 Daten Extraktor ===\n");

        String sourceRoot = DEFAULT_SOURCE;
        String outputRoot = DEFAULT_TARGET;

        // Parse Kommandozeilenargumente
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--source":
                case "-s":
                    if (i + 1 < args.length) {
                        sourceRoot = args[++i];
                    } else {
                        System.err.println("Fehler: --source erwartet einen Pfad");
                        printUsage();
                        return;
                    }
                    break;
                case "--target":
                case "-t":
                    if (i + 1 < args.length) {
                        outputRoot = args[++i];
                    } else {
                        System.err.println("Fehler: --target erwartet einen Pfad");
                        printUsage();
                        return;
                    }
                    break;
                case "--help":
                case "-h":
                    printUsage();
                    return;
                default:
                    if (args[i].startsWith("-")) {
                        System.err.println("Unbekannte Option: " + args[i]);
                        printUsage();
                        return;
                    }
                    break;
            }
        }

        try {
            RE15MasterExtractor.extract(sourceRoot, outputRoot);
        } catch (Exception e) {
            System.err.println("Fehler beim Starten der Extraktion: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void printUsage() {
        System.out.println("Verwendung: java -jar re15-extractor.jar [optionen]");
        System.out.println();
        System.out.println("Optionen:");
        System.out.println("  --source, -s <pfad>   Quellverzeichnis mit RE1.5 Daten");
        System.out.println("                        (Standard: " + DEFAULT_SOURCE + ")");
        System.out.println("  --target, -t <pfad>   Zielverzeichnis fuer extrahierte Dateien");
        System.out.println("                        (Standard: " + DEFAULT_TARGET + ")");
        System.out.println("  --help, -h            Zeigt diese Hilfe an");
        System.out.println();
        System.out.println("Beispiele:");
        System.out.println("  java -jar re15-extractor.jar");
        System.out.println("      Extrahiert von info/Re1.5 nach extracted/");
        System.out.println();
        System.out.println("  java -jar re15-extractor.jar --source D:/RE15/data --target D:/output");
        System.out.println("      Extrahiert von D:/RE15/data nach D:/output/");
    }
}
