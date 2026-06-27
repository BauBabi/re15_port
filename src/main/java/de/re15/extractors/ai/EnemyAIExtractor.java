package de.re15.extractors.ai;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

/**
 * Extrahiert Enemy AI Behavior aus dekompiliertem RE1.5 Quellcode.
 * Liest die Ghidra-generierten C-Dateien und analysiert SCD Spawn-Befehle.
 */
public final class EnemyAIExtractor {

    private final Path sourceDir;
    private final Map<Integer, BehaviorAnalysis> analyzedBehaviors = new LinkedHashMap<>();
    private final Map<String, RoomSpawns> roomSpawns = new LinkedHashMap<>();
    private final Map<Integer, EnemyTypeStats> enemyStats = new LinkedHashMap<>();

    private Path ghidraListingPath;
    private Path extractedDir;

    public EnemyAIExtractor(Path sourceDir) {
        this.sourceDir = sourceDir;
        Path parent = sourceDir.getParent();
        this.ghidraListingPath = (parent != null)
                ? parent.resolve("ghidra1.txt")
                : Path.of("ghidra1.txt");
        this.extractedDir = (parent != null)
                ? parent.resolve("already_extracted")
                : Path.of("already_extracted");
    }

    public void setExtractedDir(Path extractedDir) {
        this.extractedDir = extractedDir;
    }

    /**
     * Extrahiert alle Behavior-Funktionen und SCD-Spawns.
     */
    public void extractAll(Path outputDir) throws IOException {
        Files.createDirectories(outputDir);

        // Phase 1: Behavior-Funktionen analysieren
        System.out.println("[EnemyAI] Phase 1: Analysiere Behavior-Funktionen...");
        if (Files.exists(ghidraListingPath)) {
            System.out.println("[EnemyAI] Lese Assembly aus ghidra1.txt...");
            extractFromGhidraListing();
        } else {
            extractFromCFiles();
        }

        // Phase 2: SCD Spawn-Befehle analysieren
        System.out.println("\n[EnemyAI] Phase 2: Analysiere SCD Spawn-Befehle...");
        if (Files.exists(extractedDir)) {
            extractSpawnsFromSCD();
        } else {
            System.out.println("[EnemyAI] WARNUNG: " + extractedDir + " nicht gefunden, ueberspringe Spawn-Analyse");
        }

        // Generiere Ausgaben
        System.out.println("\n[EnemyAI] Phase 3: Generiere Dokumentation...");

        // JSON Export
        String json = generateJson();
        Files.writeString(outputDir.resolve("enemy_behaviors.json"), json);
        System.out.println("[EnemyAI] Exportiert: enemy_behaviors.json");

        // Markdown Dokumentation
        String markdown = generateMarkdown();
        Files.writeString(outputDir.resolve("enemy_behaviors.md"), markdown);
        System.out.println("[EnemyAI] Exportiert: enemy_behaviors.md");

        // C Header
        String header = generateCHeader();
        Files.writeString(outputDir.resolve("enemy_ai.h"), header);
        System.out.println("[EnemyAI] Exportiert: enemy_ai.h");

        // Spawn-Analyse (NEU)
        if (!roomSpawns.isEmpty()) {
            String spawnJson = generateSpawnJson();
            Files.writeString(outputDir.resolve("enemy_spawns.json"), spawnJson);
            System.out.println("[EnemyAI] Exportiert: enemy_spawns.json");

            String allEnemies = generateAllEnemiesMarkdown();
            Files.writeString(outputDir.resolve("all_enemies.md"), allEnemies);
            System.out.println("[EnemyAI] Exportiert: all_enemies.md");
        }
    }

    /**
     * Scannt SCD-Dateien nach Sce_em_set Befehlen.
     */
    private void extractSpawnsFromSCD() throws IOException {
        // Regex fuer Sce_em_set(slot, type, behavior, p3, p4, p5, p6, x, y, z, p10, rot, p12)
        Pattern spawnPattern = Pattern.compile(
            "Sce_em_set\\s*\\(\\s*" +
            "(-?\\d+)\\s*,\\s*" +           // slot
            "(0x[0-9A-Fa-f]+|\\d+)\\s*,\\s*" + // type
            "(-?\\d+)\\s*,\\s*" +            // behavior
            "(-?\\d+)\\s*,\\s*" +            // unknown1
            "(-?\\d+)\\s*,\\s*" +            // flags
            "(-?\\d+)\\s*,\\s*" +            // unknown2
            "(-?\\d+)\\s*,\\s*" +            // id
            "(-?\\d+)\\s*,\\s*" +            // x
            "(-?\\d+)\\s*,\\s*" +            // y
            "(-?\\d+)\\s*,\\s*" +            // z
            "(-?\\d+)\\s*,\\s*" +            // unknown3
            "(-?\\d+)\\s*,\\s*" +            // rotation
            "(-?\\d+)\\s*\\)"                // unknown4
        );

        List<Path> scdFiles = Files.walk(extractedDir)
            .filter(Files::isRegularFile)
            .filter(p -> p.toString().endsWith(".c"))
            .filter(p -> p.toString().contains("scd") || p.toString().contains("SCD"))
            .sorted()
            .collect(Collectors.toList());

        System.out.println("[EnemyAI] Gefunden: " + scdFiles.size() + " SCD-Dateien");

        int totalSpawns = 0;
        for (Path scdFile : scdFiles) {
            String content = Files.readString(scdFile);
            Matcher m = spawnPattern.matcher(content);

            String roomName = extractRoomName(scdFile);
            RoomSpawns room = roomSpawns.computeIfAbsent(roomName, RoomSpawns::new);

            while (m.find()) {
                EnemySpawn spawn = new EnemySpawn();
                spawn.slot = Integer.parseInt(m.group(1));
                spawn.type = parseHexOrInt(m.group(2));
                spawn.behavior = Integer.parseInt(m.group(3));
                spawn.unknown1 = Integer.parseInt(m.group(4));
                spawn.flags = Integer.parseInt(m.group(5));
                spawn.unknown2 = Integer.parseInt(m.group(6));
                spawn.id = Integer.parseInt(m.group(7));
                spawn.x = Integer.parseInt(m.group(8));
                spawn.y = Integer.parseInt(m.group(9));
                spawn.z = Integer.parseInt(m.group(10));
                spawn.rotation = Integer.parseInt(m.group(12));
                spawn.roomName = roomName;

                room.spawns.add(spawn);

                // Statistiken aktualisieren
                EnemyTypeStats stats = enemyStats.computeIfAbsent(spawn.type, EnemyTypeStats::new);
                stats.totalSpawns++;
                stats.rooms.add(roomName);
                stats.behaviors.add(spawn.behavior);

                totalSpawns++;
            }
        }

        System.out.println("[EnemyAI] Gefunden: " + totalSpawns + " Spawn-Befehle in " + roomSpawns.size() + " Raeumen");
        System.out.println("[EnemyAI] Gegner-Typen: " + enemyStats.size());

        // Zeige Top-Gegner
        enemyStats.entrySet().stream()
            .sorted((a, b) -> Integer.compare(b.getValue().totalSpawns, a.getValue().totalSpawns))
            .limit(5)
            .forEach(e -> System.out.printf("  [0x%02X] %s: %d Spawns, %d Raeume, Behaviors: %s%n",
                e.getKey(),
                getEnemyName(e.getKey()),
                e.getValue().totalSpawns,
                e.getValue().rooms.size(),
                e.getValue().behaviors.stream().sorted().map(String::valueOf).collect(Collectors.joining(", "))));
    }

    private String extractRoomName(Path scdFile) {
        // Extrahiere Raumnamen aus Pfad wie .../room1030/scd/main00.c
        String path = scdFile.toString().replace('\\', '/');
        Pattern roomPattern = Pattern.compile("(room[0-9A-Fa-f]+)");
        Matcher m = roomPattern.matcher(path);
        if (m.find()) {
            return m.group(1);
        }
        return scdFile.getParent().getFileName().toString();
    }

    private int parseHexOrInt(String value) {
        if (value.startsWith("0x") || value.startsWith("0X")) {
            return Integer.parseInt(value.substring(2), 16);
        }
        return Integer.parseInt(value);
    }

    private String getEnemyName(int type) {
        return BehaviorFunctionTable.ENEMY_TYPES.getOrDefault(type, String.format("em%02X", type));
    }

    /**
     * Extrahiert Behaviors aus ghidra1.txt Assembly-Listing.
     */
    private void extractFromGhidraListing() throws IOException {
        String listing = Files.readString(ghidraListingPath);
        String[] lines = listing.split("\n");

        Pattern instructionPattern = Pattern.compile(
                "^\\s+([0-9a-f]{8})\\s+[0-9a-f]{2}\\s+[0-9a-f]{2}\\s+[0-9a-f]{2}\\s+[0-9a-f]{2}\\s+\\w+");

        List<Map.Entry<Integer, BehaviorFunctionTable.BehaviorEntry>> sortedBehaviors =
                new ArrayList<>(BehaviorFunctionTable.BEHAVIORS.entrySet());
        sortedBehaviors.sort(Comparator.comparingInt(e -> e.getValue().address()));

        for (var entry : sortedBehaviors) {
            int index = entry.getKey();
            var behavior = entry.getValue();

            int startAddr = behavior.address();
            String startAddrHex = String.format("%08x", startAddr);
            String labelName = "LAB_" + startAddrHex;

            StringBuilder asmBlock = new StringBuilder();
            boolean inBlock = false;
            boolean foundFirstInstruction = false;
            int instructionCount = 0;

            for (String line : lines) {
                String lineLower = line.toLowerCase();

                if (!inBlock) {
                    if (line.contains(labelName) || lineLower.contains(startAddrHex)) {
                        inBlock = true;
                    }
                    continue;
                }

                asmBlock.append(line).append("\n");

                Matcher m = instructionPattern.matcher(lineLower);
                if (m.find()) {
                    instructionCount++;
                    foundFirstInstruction = true;

                    if (line.contains("jr") && line.contains("ra")) {
                        break;
                    }
                }

                if (foundFirstInstruction && line.trim().startsWith("LAB_")) {
                    break;
                }
            }

            BehaviorAnalysis analysis = analyzeAssembly(index, behavior, asmBlock.toString(), instructionCount);
            analyzedBehaviors.put(index, analysis);

            System.out.printf("  [%02X] %s -> %d Instruktionen, Typ: %s%n",
                    index, behavior.name(), instructionCount, analysis.behaviorType);
        }
    }

    private void extractFromCFiles() throws IOException {
        for (var entry : BehaviorFunctionTable.BEHAVIORS.entrySet()) {
            int index = entry.getKey();
            var behavior = entry.getValue();

            String fileName = behavior.getSourceFileName();
            Path sourcePath = sourceDir.resolve(fileName);

            if (Files.exists(sourcePath)) {
                String source = Files.readString(sourcePath);
                BehaviorAnalysis analysis = analyzeBehavior(index, behavior, source);
                analyzedBehaviors.put(index, analysis);
                System.out.printf("  [%02X] %s -> %d Zeilen%n",
                        index, behavior.name(), analysis.lineCount);
            }
        }
    }

    private BehaviorAnalysis analyzeAssembly(int index, BehaviorFunctionTable.BehaviorEntry behavior, String asm, int instructionCount) {
        BehaviorAnalysis analysis = new BehaviorAnalysis();
        analysis.index = index;
        analysis.name = behavior.name();
        analysis.address = behavior.address();
        analysis.lineCount = instructionCount;
        analysis.rawSource = asm;

        if (asm.contains("beq") || asm.contains("bne") || asm.contains("bgtz") || asm.contains("bltz")) {
            analysis.hasSwitchStatement = true;
            analysis.behaviorType = "CONDITIONAL";
        }

        if (asm.contains("jalr") || asm.contains("jal")) {
            Pattern callPattern = Pattern.compile("(FUN_[0-9a-fA-F]+|LAB_[0-9a-fA-F]+)");
            Matcher m = callPattern.matcher(asm);
            while (m.find()) {
                analysis.functionCalls.add(m.group(1));
            }
            analysis.behaviorType = "COMPLEX";
        }

        Pattern datPattern = Pattern.compile("DAT_([0-9a-fA-F]+)");
        Matcher datMatcher = datPattern.matcher(asm);
        while (datMatcher.find()) {
            analysis.dataReferences.add("DAT_" + datMatcher.group(1));
        }

        if (asm.contains("ori       v0,zero,0x1") || asm.contains("_ori      v0,zero,0x1")) {
            analysis.returnValues.add("1");
            if (analysis.behaviorType.equals("UNKNOWN")) {
                analysis.behaviorType = "CONTINUE";
            }
        }
        if (asm.contains("ori       v0,zero,0x2") || asm.contains("_ori      v0,zero,0x2")) {
            analysis.returnValues.add("2");
            if (analysis.behaviorType.equals("UNKNOWN")) {
                analysis.behaviorType = "DONE";
            }
        }

        if (asm.contains("0x1c(a0)") || asm.contains("1c,0x0(a0)")) {
            analysis.behaviorType = "STACK_OPERATION";
        }

        if (instructionCount <= 8 && analysis.behaviorType.equals("UNKNOWN")) {
            analysis.behaviorType = "SIMPLE";
        } else if (instructionCount > 50) {
            analysis.behaviorType = "STATE_MACHINE";
        }

        return analysis;
    }

    private BehaviorAnalysis analyzeBehavior(int index, BehaviorFunctionTable.BehaviorEntry behavior, String source) {
        BehaviorAnalysis analysis = new BehaviorAnalysis();
        analysis.index = index;
        analysis.name = behavior.name();
        analysis.address = behavior.address();
        analysis.lineCount = source.lines().count();
        analysis.rawSource = source;

        Pattern switchPattern = Pattern.compile("switch\\s*\\([^)]+\\)\\s*\\{([^}]+)\\}", Pattern.DOTALL);
        Matcher switchMatcher = switchPattern.matcher(source);
        while (switchMatcher.find()) {
            analysis.hasSwitchStatement = true;
        }

        Pattern callPattern = Pattern.compile("(FUN_[0-9a-fA-F]+)\\s*\\(");
        Matcher callMatcher = callPattern.matcher(source);
        while (callMatcher.find()) {
            analysis.functionCalls.add(callMatcher.group(1));
        }

        Pattern returnPattern = Pattern.compile("return\\s+(\\d+|0x[0-9a-fA-F]+)");
        Matcher returnMatcher = returnPattern.matcher(source);
        while (returnMatcher.find()) {
            analysis.returnValues.add(returnMatcher.group(1));
        }

        analysis.behaviorType = "UNKNOWN";
        return analysis;
    }

    private String generateJson() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("  \"version\": \"2.0\",\n");
        sb.append("  \"source\": \"RE1.5 Ghidra Decompilation + SCD Analysis\",\n");
        sb.append("  \"behavior_table_address\": \"0x800744a8\",\n");
        sb.append("  \"entity_struct_size\": 368,\n");
        sb.append("  \"behaviors\": [\n");

        boolean first = true;
        for (var entry : analyzedBehaviors.entrySet()) {
            if (!first) sb.append(",\n");
            first = false;
            BehaviorAnalysis a = entry.getValue();
            sb.append("    {\n");
            sb.append(String.format("      \"index\": %d,\n", a.index));
            sb.append(String.format("      \"index_hex\": \"0x%02X\",\n", a.index));
            sb.append(String.format("      \"name\": \"%s\",\n", a.name));
            sb.append(String.format("      \"address\": \"0x%08X\",\n", a.address));
            sb.append(String.format("      \"type\": \"%s\",\n", a.behaviorType));
            sb.append(String.format("      \"instruction_count\": %d\n", a.lineCount));
            sb.append("    }");
        }

        sb.append("\n  ],\n");
        sb.append("  \"enemy_types\": [\n");

        first = true;
        for (var entry : BehaviorFunctionTable.ENEMY_TYPES.entrySet()) {
            if (!first) sb.append(",\n");
            first = false;
            EnemyTypeStats stats = enemyStats.get(entry.getKey());
            int spawns = stats != null ? stats.totalSpawns : 0;
            sb.append(String.format("    {\"id\": \"0x%02X\", \"name\": \"%s\", \"spawn_count\": %d}",
                entry.getKey(), entry.getValue(), spawns));
        }

        sb.append("\n  ]\n");
        sb.append("}\n");
        return sb.toString();
    }

    private String generateSpawnJson() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("  \"version\": \"1.0\",\n");
        sb.append("  \"total_spawns\": " + enemyStats.values().stream().mapToInt(s -> s.totalSpawns).sum() + ",\n");
        sb.append("  \"rooms\": [\n");

        boolean first = true;
        for (var entry : roomSpawns.entrySet()) {
            if (!first) sb.append(",\n");
            first = false;
            RoomSpawns room = entry.getValue();
            sb.append("    {\n");
            sb.append("      \"name\": \"").append(room.name).append("\",\n");
            sb.append("      \"spawn_count\": ").append(room.spawns.size()).append(",\n");
            sb.append("      \"spawns\": [\n");

            boolean firstSpawn = true;
            for (EnemySpawn spawn : room.spawns) {
                if (!firstSpawn) sb.append(",\n");
                firstSpawn = false;
                sb.append(String.format(
                    "        {\"type\": \"0x%02X\", \"name\": \"%s\", \"behavior\": %d, \"pos\": [%d, %d, %d], \"rotation\": %d}",
                    spawn.type, getEnemyName(spawn.type), spawn.behavior,
                    spawn.x, spawn.y, spawn.z, spawn.rotation));
            }
            sb.append("\n      ]\n");
            sb.append("    }");
        }

        sb.append("\n  ],\n");
        sb.append("  \"enemy_stats\": [\n");

        first = true;
        List<Map.Entry<Integer, EnemyTypeStats>> sortedStats = new ArrayList<>(enemyStats.entrySet());
        sortedStats.sort((a, b) -> Integer.compare(b.getValue().totalSpawns, a.getValue().totalSpawns));

        for (var entry : sortedStats) {
            if (!first) sb.append(",\n");
            first = false;
            EnemyTypeStats stats = entry.getValue();
            sb.append(String.format(
                "    {\"type\": \"0x%02X\", \"name\": \"%s\", \"spawns\": %d, \"rooms\": %d, \"behaviors\": %s}",
                entry.getKey(), getEnemyName(entry.getKey()), stats.totalSpawns, stats.rooms.size(),
                stats.behaviors.stream().sorted().map(String::valueOf).collect(Collectors.joining(", ", "[", "]"))));
        }

        sb.append("\n  ]\n");
        sb.append("}\n");
        return sb.toString();
    }

    private String generateMarkdown() {
        StringBuilder sb = new StringBuilder();
        sb.append("# RE1.5 Enemy AI Behavior Analysis\n\n");
        sb.append("Automatisch extrahiert aus Ghidra Decompilation.\n\n");
        sb.append("## Behavior Function Pointer Table\n\n");
        sb.append("Basis-Adresse: `0x800744a8`\n\n");
        sb.append("| Index | Name | Adresse | Typ | Instruktionen |\n");
        sb.append("|-------|------|---------|-----|---------------|\n");

        for (var entry : analyzedBehaviors.entrySet()) {
            BehaviorAnalysis a = entry.getValue();
            sb.append(String.format("| 0x%02X | %s | 0x%08X | %s | %d |\n",
                    a.index, a.name, a.address, a.behaviorType, a.lineCount));
        }

        sb.append("\n## Enemy Types\n\n");
        sb.append("| ID | Name | Spawns | Raeume | Behaviors |\n");
        sb.append("|----|------|--------|--------|----------|\n");

        for (var entry : BehaviorFunctionTable.ENEMY_TYPES.entrySet()) {
            EnemyTypeStats stats = enemyStats.get(entry.getKey());
            String behaviors = stats != null
                ? stats.behaviors.stream().sorted().map(String::valueOf).collect(Collectors.joining(", "))
                : "-";
            int spawns = stats != null ? stats.totalSpawns : 0;
            int rooms = stats != null ? stats.rooms.size() : 0;
            sb.append(String.format("| 0x%02X | %s | %d | %d | %s |\n",
                entry.getKey(), entry.getValue(), spawns, rooms, behaviors));
        }

        return sb.toString();
    }

    private String generateAllEnemiesMarkdown() {
        StringBuilder sb = new StringBuilder();
        sb.append("# RE1.5 Alle Gegner-Typen\n\n");
        sb.append("Automatisch generiert aus SCD-Spawn-Analyse.\n\n");

        // Uebersicht
        sb.append("## Uebersicht\n\n");
        sb.append("| Typ | Name | Spawns | Raeume | Behaviors | Kategorie |\n");
        sb.append("|-----|------|--------|--------|-----------|----------|\n");

        List<Map.Entry<Integer, EnemyTypeStats>> sorted = new ArrayList<>(enemyStats.entrySet());
        sorted.sort((a, b) -> Integer.compare(b.getValue().totalSpawns, a.getValue().totalSpawns));

        for (var entry : sorted) {
            EnemyTypeStats stats = entry.getValue();
            String behaviors = stats.behaviors.stream().sorted().map(String::valueOf).collect(Collectors.joining(", "));
            String category = classifyEnemyType(entry.getKey(), stats);
            sb.append(String.format("| 0x%02X | %s | %d | %d | %s | %s |\n",
                entry.getKey(), getEnemyName(entry.getKey()),
                stats.totalSpawns, stats.rooms.size(), behaviors, category));
        }

        // Behavior-Index Erklaerung
        sb.append("\n## Behavior-Index Bedeutung\n\n");
        sb.append("```\n");
        sb.append("Bit 7 (0x80): Entity startet TOT/LIEGEND\n");
        sb.append("              Beispiel: 135 (0x87), 136 (0x88) = Zombie liegt am Boden\n");
        sb.append("\n");
        sb.append("Bit 6 (0x40): Entity startet WARTEND/VERSTECKT\n");
        sb.append("              Beispiel: 64 (0x40) = Wartet auf Trigger (Decke/Wand)\n");
        sb.append("\n");
        sb.append("Bit 4 (0x10): Alternativer Animations-Start\n");
        sb.append("              Beispiel: 16 (0x10), 144 (0x90)\n");
        sb.append("\n");
        sb.append("Bits 0-3:     Animations-Variante (0-15)\n");
        sb.append("```\n");

        // Details pro Gegner
        sb.append("\n## Gegner-Details\n\n");

        for (var entry : sorted) {
            int type = entry.getKey();
            EnemyTypeStats stats = entry.getValue();

            sb.append(String.format("### %s (0x%02X)\n\n", getEnemyName(type), type));
            sb.append(String.format("- **Spawns**: %d\n", stats.totalSpawns));
            sb.append(String.format("- **Raeume**: %d (%s)\n", stats.rooms.size(),
                stats.rooms.stream().sorted().limit(5).collect(Collectors.joining(", ")) +
                (stats.rooms.size() > 5 ? ", ..." : "")));
            sb.append(String.format("- **Behaviors**: %s\n",
                stats.behaviors.stream().sorted()
                    .map(b -> String.format("%d (0x%02X)", b, b))
                    .collect(Collectors.joining(", "))));
            sb.append(String.format("- **Kategorie**: %s\n", classifyEnemyType(type, stats)));

            // Spawn-Beispiele
            sb.append("\n**Spawn-Beispiele:**\n```c\n");
            roomSpawns.values().stream()
                .flatMap(r -> r.spawns.stream())
                .filter(s -> s.type == type)
                .limit(3)
                .forEach(s -> sb.append(String.format(
                    "// %s\nSce_em_set(%d, 0x%02X, %d, ..., %d, %d, %d, ..., %d, ...);\n",
                    s.roomName, s.slot, s.type, s.behavior, s.x, s.y, s.z, s.rotation)));
            sb.append("```\n\n");
        }

        // Raum-Uebersicht
        sb.append("## Spawn-Statistik pro Raum\n\n");
        sb.append("| Raum | Anzahl | Gegner-Typen |\n");
        sb.append("|------|--------|-------------|\n");

        roomSpawns.entrySet().stream()
            .sorted((a, b) -> Integer.compare(b.getValue().spawns.size(), a.getValue().spawns.size()))
            .limit(20)
            .forEach(entry -> {
                RoomSpawns room = entry.getValue();
                String types = room.spawns.stream()
                    .map(s -> s.type)
                    .distinct()
                    .sorted()
                    .map(t -> String.format("0x%02X", t))
                    .collect(Collectors.joining(", "));
                sb.append(String.format("| %s | %d | %s |\n", room.name, room.spawns.size(), types));
            });

        return sb.toString();
    }

    private String classifyEnemyType(int type, EnemyTypeStats stats) {
        // Klassifiziere basierend auf Behavior-Mustern
        boolean hasDeadBehavior = stats.behaviors.stream().anyMatch(b -> (b & 0x80) != 0);
        boolean hasWaitingBehavior = stats.behaviors.stream().anyMatch(b -> (b & 0x40) != 0 && (b & 0x80) == 0);
        boolean onlyZeroBehavior = stats.behaviors.size() == 1 && stats.behaviors.contains(0);
        int avgSpawnsPerRoom = stats.totalSpawns / Math.max(1, stats.rooms.size());

        if (type >= 0x10 && type <= 0x18) {
            return hasDeadBehavior ? "Zombie (liegend/aufstehend)" : "Zombie (aktiv)";
        }
        if (type == 0x1A && avgSpawnsPerRoom > 5) {
            return "Schwarm-Gegner";
        }
        if (type >= 0x40 && type <= 0x4F && hasWaitingBehavior) {
            return "Decken/Wand-Gegner (Licker?)";
        }
        if (type >= 0x20 && type <= 0x2F) {
            if (hasWaitingBehavior) return "Versteckter Gegner";
            return "Mittlerer Gegner";
        }
        if (onlyZeroBehavior && avgSpawnsPerRoom > 3) {
            return "Schwarm-Gegner";
        }
        if (hasDeadBehavior) {
            return "Kann auferstehen";
        }
        if (hasWaitingBehavior) {
            return "Wartet auf Spieler";
        }
        return "Standard";
    }

    private String generateCHeader() {
        StringBuilder sb = new StringBuilder();
        sb.append("/**\n");
        sb.append(" * RE1.5 Enemy AI Header\n");
        sb.append(" * Automatisch generiert aus Ghidra Decompilation + SCD Analyse\n");
        sb.append(" */\n\n");
        sb.append("#ifndef RE15_ENEMY_AI_H\n");
        sb.append("#define RE15_ENEMY_AI_H\n\n");
        sb.append("#include <stdint.h>\n\n");

        sb.append("// Entity Struktur (0x170 = 368 Bytes)\n");
        sb.append("typedef struct {\n");
        sb.append("    uint8_t  type;              // +0x00: Enemy Type ID\n");
        sb.append("    uint8_t  active;            // +0x01: Active flag\n");
        sb.append("    uint8_t  behavior_depth;    // +0x02: Behavior stack depth\n");
        sb.append("    uint8_t  flags;             // +0x03: Status flags\n");
        sb.append("    uint8_t  reserved_04[4];    // +0x04\n");
        sb.append("    int16_t  pos_x;             // +0x08: Position X\n");
        sb.append("    int16_t  pos_y;             // +0x0A: Position Y\n");
        sb.append("    int16_t  pos_z;             // +0x0C: Position Z\n");
        sb.append("    int16_t  rot_y;             // +0x0E: Rotation Y\n");
        sb.append("    uint8_t  reserved_10[12];   // +0x10\n");
        sb.append("    uint8_t* behavior_ptr;      // +0x1C: Current behavior pointer\n");
        sb.append("    uint8_t  reserved_20[0x150];// +0x20 to +0x170\n");
        sb.append("} Entity;  // sizeof = 0x170\n\n");

        sb.append("// Behavior Return Values\n");
        sb.append("#define BEHAVIOR_CONTINUE  1\n");
        sb.append("#define BEHAVIOR_DONE      2\n\n");

        sb.append("// Behavior Index Flags\n");
        sb.append("#define BEHAVIOR_FLAG_DEAD    0x80  // Entity starts dead/lying\n");
        sb.append("#define BEHAVIOR_FLAG_WAITING 0x40  // Entity starts waiting/hidden\n");
        sb.append("#define BEHAVIOR_FLAG_ALT     0x10  // Alternative start animation\n\n");

        sb.append("// Behavior Function Indices\n");
        for (var entry : BehaviorFunctionTable.BEHAVIORS.entrySet()) {
            sb.append(String.format("#define BEHAVIOR_%s  0x%02X\n",
                    entry.getValue().name(), entry.getKey()));
        }

        sb.append("\n// Enemy Type IDs\n");
        for (var entry : BehaviorFunctionTable.ENEMY_TYPES.entrySet()) {
            sb.append(String.format("#define ENEMY_%s  0x%02X\n",
                    entry.getValue().toUpperCase(), entry.getKey()));
        }

        sb.append("\n// Behavior function type\n");
        sb.append("typedef int (*BehaviorFunc)(Entity* entity);\n\n");

        sb.append("// Function pointer table (at 0x800744a8)\n");
        sb.append("extern BehaviorFunc g_behavior_table[34];\n\n");

        sb.append("#endif // RE15_ENEMY_AI_H\n");
        return sb.toString();
    }

    // === Data Classes ===

    private static class BehaviorAnalysis {
        int index;
        String name;
        int address;
        long lineCount;
        String rawSource;
        String behaviorType = "UNKNOWN";
        boolean hasSwitchStatement;
        List<String> switchCases = new ArrayList<>();
        List<String> returnValues = new ArrayList<>();
        Set<String> functionCalls = new LinkedHashSet<>();
        Set<String> dataReferences = new LinkedHashSet<>();
    }

    private static class EnemySpawn {
        int slot;
        int type;
        int behavior;
        int unknown1;
        int flags;
        int unknown2;
        int id;
        int x, y, z;
        int rotation;
        String roomName;
    }

    private static class RoomSpawns {
        String name;
        List<EnemySpawn> spawns = new ArrayList<>();

        RoomSpawns(String name) {
            this.name = name;
        }
    }

    private static class EnemyTypeStats {
        int type;
        int totalSpawns;
        Set<String> rooms = new LinkedHashSet<>();
        Set<Integer> behaviors = new TreeSet<>();

        EnemyTypeStats(int type) {
            this.type = type;
        }
    }

    public static void main(String[] args) throws IOException {
        Path sourceDir = Path.of("RE_15_Quellcode");
        Path outputDir = Path.of("extracted/AI");

        EnemyAIExtractor extractor = new EnemyAIExtractor(sourceDir);
        extractor.extractAll(outputDir);
    }
}
