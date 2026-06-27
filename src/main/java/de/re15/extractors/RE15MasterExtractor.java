package de.re15.extractors;

import de.re15.converters.EffectAnalyzer;
import de.re15.extractors.bss.BssExtractor;
import de.re15.extractors.emd.EmdGltfExtractor;
import de.re15.extractors.pld.PldBlenderExporter;
import de.re15.extractors.pld.PldExtractor;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.file.StandardCopyOption;
import java.util.*;

/**
 * RESIDENT EVIL 1.5 MASTER EXTRACTOR
 *
 * Finale Lösung für die komplette Extraktion aller RE1.5 Dateiformate.
 * Verwendet die korrekten Step 1-4 Konverter für pixel-perfekte Extraktion.
 */
public class RE15MasterExtractor {

    private static final Map<String, String> FILE_DESCRIPTIONS = new HashMap<>();

    static {
        FILE_DESCRIPTIONS.put("PIX", "PlayStation Textur-Archive");
        FILE_DESCRIPTIONS.put("TIM", "PlayStation Textur-Dateien");
        FILE_DESCRIPTIONS.put("SCD", "Audio-Container");
        FILE_DESCRIPTIONS.put("ESP", "Effekt-Archive");
        FILE_DESCRIPTIONS.put("EMD", "3D-Modell-Dateien");
        FILE_DESCRIPTIONS.put("RDT", "Raum-Definition-Dateien");
        FILE_DESCRIPTIONS.put("EXE", "PlayStation Executable");
        FILE_DESCRIPTIONS.put("PLD", "Player-Modell-Container");
        FILE_DESCRIPTIONS.put("PLW", "Player-Waffen-Container");
        FILE_DESCRIPTIONS.put("BIN", "Binär-Daten");
        FILE_DESCRIPTIONS.put("DAT", "Daten-Container");
        FILE_DESCRIPTIONS.put("EMS", "EMS-Archive (EMD-Container)");
        FILE_DESCRIPTIONS.put("CNF", "Konfigurationsdateien");
        FILE_DESCRIPTIONS.put("BSS", "MDEC-Video-Hintergrund-Dateien");
    }

    private static final int EMS_ALIGNMENT = 2048;
    private static final int EMS_DIRECTORY_FOOTER_SIZE = 36;
    private static final boolean ENABLE_MD1_EXPORT = false;
    private static final boolean ENABLE_EMD_GLTF_EXPORT = true;
    private static final String[] DEFAULT_EMS_ORDER = {
        "em10", "em11", "em12", "em13", "em16", "em18", "em1A", "em20", "em21", "em24",
        "em25", "em26", "em27", "em29", "em2B", "em2D", "em30", "em36", "em40", "em42",
        "em45", "em47", "em49", "em4B", "em4D"
    };




    private static final String DEFAULT_SOURCE = "info/Re1.5";
    private static final String DEFAULT_TARGET = "extracted";

    /**
     * Statische Methode fuer den Aufruf mit benutzerdefinierten Pfaden.
     *
     * @param sourceRoot Quellverzeichnis mit RE1.5 Daten
     * @param outputRoot Zielverzeichnis fuer extrahierte Dateien
     */
    public static void extract(String sourceRoot, String outputRoot) {
        System.out.println("======================================================");
        System.out.println("RESIDENT EVIL 1.5 MASTER EXTRACTOR");
        System.out.println("======================================================");
        System.out.println("Finale Loesung fuer pixel-perfekte RE1.5 Extraktion");
        System.out.println("Verwendet Step 1-6 Konverter fuer optimale Ergebnisse");
        System.out.println();

        try {
            // Loesche vorherige Extraktion
            if (Files.exists(Paths.get(outputRoot))) {
                deleteDirectory(Paths.get(outputRoot));
            }

            RE15MasterExtractor extractor = new RE15MasterExtractor();
            extractor.extractAll(sourceRoot, outputRoot);

            System.out.println("\n======================================================");
            System.out.println("RESIDENT EVIL 1.5 EXTRAKTION ABGESCHLOSSEN!");
            System.out.println("======================================================");
            System.out.println("Alle Dateien wurden erfolgreich extrahiert.");
            System.out.println("Ausgabe: " + outputRoot + "/");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    /**
     * Main-Methode mit Standard-Pfaden (Fallback).
     */
    public static void main(String[] args) {
        System.out.println("[DEBUG] MAIN-METHODE GESTARTET");
        extract(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    public void extractAll(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Quelle: " + sourceRoot);
        System.out.println("Ziel: " + outputRoot);
        System.out.println();

        // Erstelle Basis-Ausgabestruktur
        Files.createDirectories(Paths.get(outputRoot));

        // Erstelle Extraktionsprotokoll
        StringBuilder extractionLog = new StringBuilder();
        extractionLog.append("RESIDENT EVIL 1.5 EXTRAKTION\n");
        extractionLog.append("Datum: ").append(new Date()).append("\n\n");

        // ============================================================
        // PHASE 1: QUELLDATEIEN KOPIEREN
        // ============================================================
        printPhaseHeader(1, "QUELLDATEIEN KOPIEREN");
        copyRemainingFiles(sourceRoot, outputRoot, extractionLog);
        extractionLog.append("PHASE 1 ABGESCHLOSSEN: Quelldateien kopiert\n\n");

        // ============================================================
        // PHASE 2: RAUMDATEN PARSEN (RDT, BSS)
        // ============================================================
        printPhaseHeader(2, "RAUMDATEN PARSEN");

        System.out.println("  2.1 RDT Raumextraktion...");
        RDTExtractor rdtExtractor = new RDTExtractor();
        rdtExtractor.extractStages(sourceRoot, outputRoot);
        extractionLog.append("  2.1 RDT Raumdateien extrahiert\n");

        System.out.println("  2.2 BSS-Hintergruende extrahieren...");
        BssExtractor bssExtractor = new BssExtractor();
        bssExtractor.extract(Paths.get(sourceRoot), Paths.get(outputRoot));
        extractionLog.append("  2.2 BSS-Bloecke, TIMs und BMPs erzeugt\n");

        extractionLog.append("PHASE 2 ABGESCHLOSSEN: Raumdaten geparst\n\n");

        // ============================================================
        // PHASE 3: SKRIPTE PARSEN (SCD -> C)
        // ============================================================
        printPhaseHeader(3, "SKRIPTE PARSEN");

        System.out.println("  3.1 SCD-Dateien zu C disassemblieren...");
        SCDScriptDisassembler scdDisassembler = new SCDScriptDisassembler();
        scdDisassembler.disassembleAllSCDFiles(sourceRoot, outputRoot);
        extractionLog.append("  3.1 SCD-Dateien zu C-Code disassembliert\n");

        extractionLog.append("PHASE 3 ABGESCHLOSSEN: Skripte geparst\n\n");

        // ============================================================
        // PHASE 4: MODELL-CONTAINER ZERLEGEN (PLD, PLW, EMS -> Komponenten)
        // ============================================================
        printPhaseHeader(4, "MODELL-CONTAINER ZERLEGEN");

        System.out.println("  4.1 PLD/PLW zu bin, edd, emr, md1, tim...");
        int pldCount = extractPldFiles(sourceRoot, outputRoot, extractionLog);
        extractionLog.append("  4.1 ").append(pldCount).append(" PLD/PLW-Dateien zerlegt\n");

        System.out.println("  4.2 EMS-Archive zu EMD...");
        extractEmsArchives(sourceRoot, outputRoot, extractionLog);
        extractionLog.append("  4.2 EMS-Archive zu EMD extrahiert\n");

        extractionLog.append("PHASE 4 ABGESCHLOSSEN: Modell-Container zerlegt\n\n");

        // ============================================================
        // PHASE 5: TEXTUREN KONVERTIEREN (TIM, PIX -> BMP)
        // ============================================================
        printPhaseHeader(5, "TEXTUREN KONVERTIEREN");

        System.out.println("  5.1 Direkte TIM-Dateien extrahieren...");
        Step1TIMExtractor step1 = new Step1TIMExtractor();
        step1.extractDirectTIMFiles(sourceRoot, outputRoot);
        extractionLog.append("  5.1 Direkte TIM-Dateien extrahiert\n");

        System.out.println("  5.2 MAP-PIX zu TIM...");
        Step2PIXToTIMConverter step2 = new Step2PIXToTIMConverter();
        step2.convertMapPIXFiles(sourceRoot, outputRoot);
        extractionLog.append("  5.2 MAP-PIX zu TIM konvertiert\n");

        System.out.println("  5.3 Item-PIX zu TIM...");
        Step2bItemPIXToTIMConverter step2b = new Step2bItemPIXToTIMConverter();
        step2b.convertItemPIXFiles(sourceRoot, outputRoot);
        extractionLog.append("  5.3 Item-PIX zu TIM konvertiert\n");

        System.out.println("  5.4 Korrigierte Item-PIX...");
        Step3CorrectItemPIXToTIMConverter step3 = new Step3CorrectItemPIXToTIMConverter();
        step3.convertItemPIXContainers(sourceRoot, outputRoot);
        extractionLog.append("  5.4 Korrigierte Item-PIX konvertiert\n");

        System.out.println("  5.5 ITP zu BMP (Item-Icons)...");
        extractItpFiles(sourceRoot, outputRoot, extractionLog);
        extractionLog.append("  5.5 ITP zu BMP Item-Icons extrahiert\n");

        System.out.println("  5.6 TIM zu BMP (MAP)...");
        MapTIMToBMPConverter mapConverter = new MapTIMToBMPConverter();
        mapConverter.convertMapTIMFiles(outputRoot);
        extractionLog.append("  5.6 MAP TIM zu BMP konvertiert\n");

        System.out.println("  5.7 TIM zu BMP (ITEMALL/MIXITEM)...");
        ItemTIMToBMPConverter itemConverter = new ItemTIMToBMPConverter();
        itemConverter.convertItemTIMFiles(outputRoot);
        extractionLog.append("  5.7 Item TIM zu BMP konvertiert\n");

        System.out.println("  5.8 TIM zu BMP (Standalone)...");
        StandaloneTIMToBMPConverter standaloneConverter = new StandaloneTIMToBMPConverter();
        standaloneConverter.convertStandaloneTIMFiles(outputRoot);
        extractionLog.append("  5.8 Standalone TIM zu BMP konvertiert\n");

        extractionLog.append("PHASE 5 ABGESCHLOSSEN: Texturen konvertiert\n\n");

        // ============================================================
        // PHASE 6: 3D-MODELLE EXPORTIEREN (OBJ, glTF)
        // ============================================================
        printPhaseHeader(6, "3D-MODELLE EXPORTIEREN");

        System.out.println("  6.1 PLD/PLW zu OBJ (Blender-Export)...");
        int blenderCount = exportPldForBlender(outputRoot, extractionLog);
        extractionLog.append("  6.1 ").append(blenderCount).append(" PLD/PLW Modelle zu OBJ exportiert\n");

        if (ENABLE_EMD_GLTF_EXPORT) {
            System.out.println("  6.2 EMD/PLD zu glTF...");
            extractEmdModels(sourceRoot, outputRoot, extractionLog);
            extractionLog.append("  6.2 EMD/PLD zu glTF exportiert\n");
        } else {
            System.out.println("  6.2 EMD zu glTF (deaktiviert)");
            extractionLog.append("  6.2 EMD zu glTF deaktiviert\n");
        }

        if (ENABLE_MD1_EXPORT) {
            System.out.println("  6.3 MD1 zu OBJ...");
            extractMd1ToObj(sourceRoot, outputRoot, extractionLog);
            extractionLog.append("  6.3 MD1 zu OBJ konvertiert\n");
        } else {
            System.out.println("  6.3 MD1 zu OBJ (deaktiviert)");
            extractionLog.append("  6.3 MD1 zu OBJ deaktiviert\n");
        }

        System.out.println("  6.4 DO2 zu MD1, TIM, glTF (Tueren)...");
        DO2Extractor doorExtractor = new DO2Extractor();
        doorExtractor.extractDoors(sourceRoot, outputRoot);
        extractionLog.append("  6.4 DO2 zu MD1, TIM, glTF (Tueren) extrahiert\n");

        extractionLog.append("PHASE 6 ABGESCHLOSSEN: 3D-Modelle exportiert\n\n");

        // ============================================================
        // PHASE 7: AUDIO & EFFEKTE
        // ============================================================
        printPhaseHeader(7, "AUDIO & EFFEKTE");

        System.out.println("  7.1 ESP-Dateien zu EFF...");
        Step5ESPExtractor step5 = new Step5ESPExtractor();
        step5.extractESPFiles(sourceRoot, outputRoot);
        extractionLog.append("  7.1 ESP-Dateien zu EFF extrahiert\n");

        System.out.println("  7.2 SOUND BGM/ARMS...");
        SoundExtractor soundExtractor = new SoundExtractor();
        soundExtractor.extract(sourceRoot, outputRoot);
        extractionLog.append("  7.2 SOUND-Baenke und BGM-Dateien extrahiert\n");

        System.out.println("  7.3 EFF-Dateien analysieren...");
        analyzeExtractedEFFFiles(outputRoot);
        extractionLog.append("  7.3 EFF-Dateien analysiert und JSON generiert\n");

        extractionLog.append("PHASE 7 ABGESCHLOSSEN: Audio & Effekte verarbeitet\n\n");

        // Schreibe Extraktionsprotokoll
        Files.write(Paths.get(outputRoot, "EXTRACTION_LOG.txt"), extractionLog.toString().getBytes());
        System.out.println("\nExtraktionsprotokoll gespeichert: EXTRACTION_LOG.txt");
    }

    private void printPhaseHeader(int phase, String title) {
        System.out.println();
        System.out.println("==================================================");
        System.out.println("PHASE " + phase + ": " + title);
        System.out.println("==================================================");
    }

    private void extractMd1ToObj(String sourceRoot, String outputRoot, StringBuilder log) throws IOException {
        List<Path> md1Files = new ArrayList<>();
        Files.walk(Paths.get(sourceRoot))
            .filter(p -> p.toString().toLowerCase(Locale.ROOT).endsWith(".md1"))
            .forEach(md1Files::add);
        MD1Extractor md1Extractor = new MD1Extractor();
        File logFile = new File(outputRoot, "EXTRACTION_LOG.txt");
        for (Path md1Path : md1Files) {
            try {
                Path rel = Paths.get(sourceRoot).relativize(md1Path);
                Path objPath = Paths.get(outputRoot, rel.toString().replaceAll("(?i)\\.md1$", ".obj"));
                Files.createDirectories(objPath.getParent());
                md1Extractor.extractMD1ToOBJ(md1Path.toFile(), objPath.toFile(), logFile);
                log.append("    MD1->OBJ: ").append(rel).append("\n");
            } catch (Exception e) {
                log.append("    FEHLER MD1->OBJ: ").append(md1Path).append(": ").append(e.getMessage()).append("\n");
            }
        }

        Path exampleRoot = Paths.get("info/example");
        if (Files.exists(exampleRoot)) {
            List<Path> md1ExampleFiles = new ArrayList<>();
            Files.walk(exampleRoot)
                .filter(p -> p.toString().toLowerCase(Locale.ROOT).endsWith(".md1"))
                .forEach(md1ExampleFiles::add);
            for (Path md1Path : md1ExampleFiles) {
                try {
                    Path rel = exampleRoot.relativize(md1Path);
                    Path objPath = Paths.get(outputRoot, "example", rel.toString().replaceAll("(?i)\\.md1$", ".obj"));
                    Files.createDirectories(objPath.getParent());
                    md1Extractor.extractMD1ToOBJ(md1Path.toFile(), objPath.toFile(), logFile);
                    log.append("    MD1->OBJ (example): ").append(rel).append("\n");
                } catch (Exception e) {
                    log.append("    FEHLER MD1->OBJ (example): ").append(md1Path).append(": ").append(e.getMessage()).append("\n");
                }
            }
        }
    }

    /**
     * Extrahiert EMS-Archive in einzelne EMD-Dateien.
     */
    private void extractEmsArchives(String sourceRoot, String outputRoot, StringBuilder log) {
        Path sourcePath = Paths.get(sourceRoot);
        try {
            Files.walk(sourcePath)
                    .filter(Files::isRegularFile)
                    .filter(path -> path.getFileName().toString().toLowerCase().endsWith(".ems"))
                    .sorted()
                    .forEach(ems -> {
                        try {
                            Path relative = sourcePath.relativize(ems);
                            byte[] data = Files.readAllBytes(ems);
                            List<EmsEntry> entries = parseEmsEntries(data);
                            if (entries.isEmpty()) {
                                log.append("EMS FEHLER: ").append(relative).append(" - keine Eintraege gefunden").append("\n");
                                System.err.println("Warnung: Keine EMD-Bloecke in " + ems);
                                return;
                            }

                            Path targetBase = Paths.get(outputRoot);
                            if (relative.getParent() != null) {
                                targetBase = targetBase.resolve(relative.getParent());
                            }
                            String stem = stripExtension(ems.getFileName().toString());
                            Path extractionDir = targetBase.resolve(stem);
                            Files.createDirectories(extractionDir);

                            for (EmsEntry entry : entries) {
                                String emdName = resolveEmsEntryName(stem, entry.index);
                                Path emdPath = extractionDir.resolve(emdName + ".emd");
                                Files.write(emdPath, Arrays.copyOfRange(data, entry.offset, entry.offset + entry.length));
                            }

                            Path iniPath = extractionDir.resolve(stem + ".ini");
                            Files.writeString(iniPath, "[ems]\n150\n", StandardCharsets.US_ASCII);

                            log.append("EMS extrahiert: ").append(relative)
                               .append(" -> ").append(extractionDir).append(" (")
                               .append(entries.size()).append(" Dateien)").append("\n");
                            System.out.println("  EMS extrahiert: " + relative + " (" + entries.size() + " EMD)");
                        } catch (Exception e) {
                            System.err.println("Fehler beim EMS-Export " + ems + ": " + e.getMessage());
                            log.append("FEHLER EMS: ").append(ems).append(" - ").append(e.getMessage()).append("\n");
                        }
                    });
        } catch (IOException e) {
            System.err.println("Fehler beim Durchsuchen nach EMS-Dateien: " + e.getMessage());
            log.append("FEHLER beim EMS-Scan: ").append(e.getMessage()).append("\n");
        }
    }

    private int extractPldFiles(String sourceRoot, String outputRoot, StringBuilder log) {
        PldExtractor extractor = new PldExtractor();
        return extractor.extract(Paths.get(sourceRoot), Paths.get(outputRoot), log);
    }

    private int exportPldForBlender(String outputRoot, StringBuilder log) {
        Path pldRoot = Paths.get(outputRoot, "PSX", "PLD");
        if (!Files.exists(pldRoot)) {
            return 0;
        }
        PldBlenderExporter exporter = new PldBlenderExporter();
        return exporter.export(pldRoot, Paths.get(outputRoot), log);
    }

    private static List<EmsEntry> parseEmsEntries(byte[] data) {
        List<EmsEntry> entries = new ArrayList<>();
        int cursor = 0;
        int limit = data.length;
        int index = 0;

        while (cursor + 4 <= limit) {
            int dirOffset = readIntLE(data, cursor);
            if (dirOffset < EMS_DIRECTORY_FOOTER_SIZE || (dirOffset & 0x03) != 0) {
                break;
            }
            int length = dirOffset + EMS_DIRECTORY_FOOTER_SIZE;
            if (length <= 0 || cursor + length > limit) {
                break;
            }
            entries.add(new EmsEntry(index++, cursor, length));

            int nextPos = cursor + length;
            if (nextPos >= limit) {
                break;
            }
            int alignedNext = alignUp(nextPos, EMS_ALIGNMENT);
            if (alignedNext <= cursor) {
                break;
            }
            cursor = alignedNext;

            int skipped = 0;
            while (cursor + 4 <= limit && readIntLE(data, cursor) == 0) {
                cursor += 4;
                skipped += 4;
            }
            if (skipped > 0) {
                cursor = alignUp(cursor, EMS_ALIGNMENT);
            }
        }

        return entries;
    }

    private static int readIntLE(byte[] data, int offset) {
        return (data[offset] & 0xFF)
                | ((data[offset + 1] & 0xFF) << 8)
                | ((data[offset + 2] & 0xFF) << 16)
                | ((data[offset + 3] & 0xFF) << 24);
    }

    private static int alignUp(int value, int alignment) {
        int remainder = value % alignment;
        if (remainder == 0) {
            return value;
        }
        return value + (alignment - remainder);
    }

    private static String stripExtension(String fileName) {
        int dot = fileName.lastIndexOf('.');
        return dot >= 0 ? fileName.substring(0, dot) : fileName;
    }

    private static String resolveEmsEntryName(String stem, int index) {
        if (index < DEFAULT_EMS_ORDER.length) {
            return DEFAULT_EMS_ORDER[index];
        }
        return String.format(Locale.ROOT, "%s_%02d", stem.toLowerCase(Locale.ROOT), index);
    }

    private static final class EmsEntry {
        final int index;
        final int offset;
        final int length;

        EmsEntry(int index, int offset, int length) {
            this.index = index;
            this.offset = offset;
            this.length = length;
        }
    }

    /**
     * Kopiert alle nicht-PIX/TIM Dateien in die Ausgabestruktur
     */
    /**
     * Kopiert alle nicht-PIX/TIM Dateien in die Ausgabestruktur
     */
    private void copyRemainingFiles(String sourceRoot, String outputRoot, StringBuilder log) throws IOException {
        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .sorted()
             .forEach(file -> {
                 try {
                     copyOtherFile(file, sourceRoot, outputRoot, log);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler beim Kopieren von " + file.getFileName() + ": " + e.getMessage());
                     log.append("FEHLER: ").append(file).append(" - ").append(e.getMessage()).append("\n");
                 }
             });
    }

    private void copyOtherFile(Path file, String sourceRoot, String outputRoot, StringBuilder log) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(file);
        String fileName = file.getFileName().toString().toUpperCase();

        // Überspringe PIX, TIM, ESP, SCD und BSS - diese werden von den Step-Konvertern behandelt
        if (fileName.endsWith(".PIX") || fileName.endsWith(".TIM") || fileName.endsWith(".ESP")
                || fileName.endsWith(".SCD") || fileName.endsWith(".BSS")) {
            return;
        }

        // Erstelle entsprechende Ausgabestruktur
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Files.createDirectories(outputPath);

        // Kopiere Datei
        Path targetFile = Paths.get(outputPath.toString(), file.getFileName().toString());
        if (!Files.exists(targetFile)) {
            Files.copy(file, targetFile, StandardCopyOption.REPLACE_EXISTING);

            String fileType = getFileTypeDescription(fileName);
            log.append("KOPIERT: ").append(relativePath).append(" (").append(fileType).append(")\n");
            System.out.println("  " + fileType + " kopiert: " + relativePath);
        }
    }

    private String getFileTypeDescription(String fileName) {
        for (Map.Entry<String, String> entry : FILE_DESCRIPTIONS.entrySet()) {
            if (fileName.endsWith("." + entry.getKey())) {
                return entry.getValue();
            }
        }
        return "Unbekannter Dateityp";
    }

    /**
     * Löscht Verzeichnis rekursiv
     */
    private static void deleteDirectory(Path path) throws IOException {
        if (Files.exists(path)) {
            Files.walk(path)
                 .sorted(java.util.Comparator.reverseOrder())
                 .map(Path::toFile)
                 .forEach(File::delete);
        }
    }

    /**
     * Extrahiert ITP-Dateien (Item Texture Pack) zu BMP Item-Icons.
     */
    private void extractItpFiles(String sourceRoot, String outputRoot, StringBuilder log) {
        ItpParser itpParser = new ItpParser();
        Path sourcePath = Paths.get(sourceRoot);

        try {
            Files.walk(sourcePath)
                    .filter(Files::isRegularFile)
                    .filter(path -> path.getFileName().toString().toUpperCase().endsWith(".ITP"))
                    .sorted()
                    .forEach(itpFile -> {
                        try {
                            Path relative = sourcePath.relativize(itpFile);
                            String baseName = itpFile.getFileName().toString();
                            baseName = baseName.substring(0, baseName.lastIndexOf('.'));

                            Path targetDir = Paths.get(outputRoot);
                            if (relative.getParent() != null) {
                                targetDir = targetDir.resolve(relative.getParent());
                            }
                            targetDir = targetDir.resolve(baseName);

                            int count = itpParser.extractToBmp(itpFile, targetDir);

                            log.append("ITP extrahiert: ").append(relative)
                               .append(" -> ").append(count).append(" Item-Icons\n");
                            System.out.println("  ITP extrahiert: " + relative + " (" + count + " Icons)");

                        } catch (Exception e) {
                            System.err.println("Fehler beim ITP-Export " + itpFile + ": " + e.getMessage());
                            log.append("FEHLER ITP: ").append(itpFile).append(" - ").append(e.getMessage()).append("\n");
                        }
                    });
        } catch (IOException e) {
            System.err.println("Fehler beim Durchsuchen nach ITP-Dateien: " + e.getMessage());
            log.append("FEHLER beim ITP-Scan: ").append(e.getMessage()).append("\n");
        }
    }

    /**
     * Analysiert extrahierte EFF-Dateien und generiert JSON-Beschreibungen
     */
    private void analyzeExtractedEFFFiles(String outputRoot) {
        try {
            // Finde alle Verzeichnisse mit EFF-Dateien und analysiere sie
            Files.walk(Paths.get(outputRoot))
                 .filter(Files::isDirectory)
                 .forEach(dir -> {
                     try {
                         // Prüfe ob das Verzeichnis EFF-Dateien enthält
                         if (Files.list(dir).anyMatch(file -> file.toString().toLowerCase().endsWith(".eff"))) {
                             System.out.println("Analysiere EFF-Dateien in: " + dir);
                             EffectAnalyzer.analyzeEffectFilesInPlace(dir.toString());
                         }
                     } catch (IOException e) {
                         System.err.println("Fehler beim Analysieren von " + dir + ": " + e.getMessage());
                     }
                 });
        } catch (IOException e) {
            System.err.println("FEHLER bei der EFF-Analyse: " + e.getMessage());
        }
    }

    private void extractEmdModels(String sourceRoot, String outputRoot, StringBuilder log) {
        EmdGltfExtractor extractor = new EmdGltfExtractor();
        Set<Path> processed = new HashSet<>();

        Path outputRootPath = Paths.get(outputRoot).toAbsolutePath().normalize();
        processEmdDirectory(extractor, Paths.get(outputRoot), outputRootPath, log, processed);
        processEmdDirectory(extractor, Paths.get(sourceRoot), outputRootPath, log, processed);
    }

    private void processEmdDirectory(EmdGltfExtractor extractor,
                                     Path basePath,
                                     Path outputRootPath,
                                     StringBuilder log,
                                     Set<Path> processed) {
        Path normalizedBase = basePath.toAbsolutePath().normalize();
        if (!Files.exists(normalizedBase)) {
            return;
        }
        try {
            Files.walk(normalizedBase)
                    .filter(Files::isRegularFile)
                    .filter(path -> {
                        String lower = path.getFileName().toString().toLowerCase(Locale.ROOT);
                        return lower.endsWith(".emd") || lower.endsWith(".pld");
                    })
                    .sorted()
                    .forEach(modelFile -> {
                        Path absoluteModel = modelFile.toAbsolutePath().normalize();
                        if (!processed.add(absoluteModel)) {
                            return;
                        }
                        try {
                            Path displayRelative;
                            Path gltfDir;
                            String modelStem = stripExtension(absoluteModel.getFileName().toString());
                            if (absoluteModel.startsWith(outputRootPath)) {
                                displayRelative = outputRootPath.relativize(absoluteModel);
                                // Export directly into the model's folder (no gltf_export subfolder)
                                gltfDir = absoluteModel.getParent();
                            } else {
                                Path relativeToBase = normalizedBase.relativize(absoluteModel);
                                Path parent = relativeToBase.getParent();
                                Path targetBase = parent == null ? outputRootPath : outputRootPath.resolve(parent);
                                // Export into a folder named after the model (matching PldExtractor output)
                                gltfDir = targetBase.resolve(modelStem);
                                displayRelative = relativeToBase;
                            }

                            Path gltfAbsolute = gltfDir.toAbsolutePath().normalize();
                            extractor.extract(absoluteModel, gltfAbsolute);
                            Path gltfRelative;
                            try {
                                gltfRelative = outputRootPath.relativize(gltfAbsolute);
                            } catch (IllegalArgumentException ex) {
                                gltfRelative = gltfAbsolute;
                            }
                            log.append("EMD/PLD zu glTF: ")
                               .append(displayRelative.toString().replace('\\', '/'))
                               .append(" -> ")
                               .append(gltfRelative.toString().replace('\\', '/'))
                               .append(System.lineSeparator());
                            System.out.println("  Modell exportiert: " + displayRelative);
                        } catch (Exception e) {
                            System.err.println("Fehler beim Modell-Export " + absoluteModel + ": " + e.getMessage());;
                            log.append("FEHLER EMD/PLD: ").append(absoluteModel).append(" - ").append(e.getMessage()).append(System.lineSeparator());
                        }
                    });
        } catch (IOException e) {
            System.err.println("Fehler beim Durchsuchen nach EMD/PLD-Dateien: " + e.getMessage());
            log.append("FEHLER beim EMD/PLD-Scan: ").append(e.getMessage()).append(System.lineSeparator());
        }
    }
}

