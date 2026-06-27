package de.re15.extractors.ai;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 * Function Pointer Table für Enemy AI Behaviors.
 * Extrahiert aus RE1.5 Binary via Ghidra.
 *
 * Die Tabelle befindet sich bei Adresse 0x800744a8.
 * Jeder Eintrag (4 Bytes) zeigt auf eine Behavior-Funktion.
 */
public final class BehaviorFunctionTable {

    // Basis-Adresse der Function Pointer Table
    public static final int TABLE_ADDRESS = 0x800744a8;

    // Entity-Struktur Konstanten
    public static final int ENTITY_SIZE = 0x170;  // 368 bytes pro Entity
    public static final int MAX_ENTITIES = 10;
    public static final int ENTITY_ARRAY_ADDRESS = 0x800b2b4c;
    public static final int ENEMY_TYPE_ARRAY_ADDRESS = 0x800b10ac;

    /**
     * Behavior Function Pointer Table.
     * Index -> Funktionsadresse
     */
    public static final Map<Integer, BehaviorEntry> BEHAVIORS = new LinkedHashMap<>();

    static {
        // Extrahiert aus ghidra1.txt bei 0x800744a8
        // Adressen verifiziert gegen PTR_LAB_800744a8
        BEHAVIORS.put(0x00, new BehaviorEntry(0x8003f1d8, "NOP", "Inkrementiert behavior_ptr, return 1 (CONTINUE)"));
        BEHAVIORS.put(0x01, new BehaviorEntry(0x8003f1f0, "POP_STACK", "Pop vom Behavior-Stack, deaktiviert Entity wenn Stack leer"));
        BEHAVIORS.put(0x02, new BehaviorEntry(0x8003f258, "BEHAVIOR_02", "Unbekannt"));
        BEHAVIORS.put(0x03, new BehaviorEntry(0x8003f270, "BEHAVIOR_03", "Unbekannt"));
        BEHAVIORS.put(0x04, new BehaviorEntry(0x8003f2a0, "BEHAVIOR_04", "Unbekannt"));
        BEHAVIORS.put(0x05, new BehaviorEntry(0x8003f2dc, "BEHAVIOR_05", "Unbekannt"));
        BEHAVIORS.put(0x06, new BehaviorEntry(0x8003f328, "BEHAVIOR_06", "Unbekannt"));
        BEHAVIORS.put(0x07, new BehaviorEntry(0x8003f368, "BEHAVIOR_07", "Unbekannt"));
        BEHAVIORS.put(0x08, new BehaviorEntry(0x8003f3a4, "BEHAVIOR_08", "Unbekannt"));
        BEHAVIORS.put(0x09, new BehaviorEntry(0x8003f3e0, "BEHAVIOR_09", "Unbekannt"));
        BEHAVIORS.put(0x0A, new BehaviorEntry(0x8003f428, "BEHAVIOR_0A", "Unbekannt"));
        BEHAVIORS.put(0x0B, new BehaviorEntry(0x8003f490, "INC_DEPTH", "Inkrementiert Stack-Tiefe"));
        BEHAVIORS.put(0x0C, new BehaviorEntry(0x8003f4c4, "BEHAVIOR_0C", "Prueft DAT_800b8520 Flag"));
        BEHAVIORS.put(0x0D, new BehaviorEntry(0x8003f540, "PUSH_STATE", "em16 Start-Behavior, pusht State auf Stack"));
        BEHAVIORS.put(0x0E, new BehaviorEntry(0x8003f674, "BEHAVIOR_0E", "Unbekannt"));
        BEHAVIORS.put(0x0F, new BehaviorEntry(0x8003f6f4, "STATE_MACHINE", "Komplexe State-Machine mit Conditions"));
        BEHAVIORS.put(0x10, new BehaviorEntry(0x8003f878, "BEHAVIOR_10", "Unbekannt"));
        BEHAVIORS.put(0x11, new BehaviorEntry(0x8003f8bc, "BEHAVIOR_11", "Unbekannt"));
        BEHAVIORS.put(0x12, new BehaviorEntry(0x8003f930, "CONDITION_CHECK", "Prueft Bedingungen, springt zu State bei Erfolg"));
        BEHAVIORS.put(0x13, new BehaviorEntry(0x8003fa5c, "BEHAVIOR_13", "Unbekannt"));
        BEHAVIORS.put(0x14, new BehaviorEntry(0x8003fb38, "BEHAVIOR_14", "Unbekannt"));
        BEHAVIORS.put(0x15, new BehaviorEntry(0x8003fb50, "BEHAVIOR_15", "Unbekannt"));
        BEHAVIORS.put(0x16, new BehaviorEntry(0x8003fb68, "BEHAVIOR_16", "Unbekannt"));
        BEHAVIORS.put(0x17, new BehaviorEntry(0x8003fb9c, "BEHAVIOR_17", "Unbekannt"));
        BEHAVIORS.put(0x18, new BehaviorEntry(0x8003fbe8, "BEHAVIOR_18", "Unbekannt"));
        BEHAVIORS.put(0x19, new BehaviorEntry(0x8003fc50, "BEHAVIOR_19", "Unbekannt"));
        BEHAVIORS.put(0x1A, new BehaviorEntry(0x8003fca8, "BEHAVIOR_1A", "Unbekannt"));
        BEHAVIORS.put(0x1B, new BehaviorEntry(0x8003f5d0, "PUSH_STATE_ALT", "Alternative PUSH_STATE Variante"));
        BEHAVIORS.put(0x1C, new BehaviorEntry(0x8003f1c0, "INC_PTR", "Inkrementiert behavior_ptr um 1"));
        BEHAVIORS.put(0x1D, new BehaviorEntry(0x8003f1d8, "NOP_1D", "Alias fuer NOP (Index 0x00)"));
        BEHAVIORS.put(0x1E, new BehaviorEntry(0x8003f1d8, "NOP_1E", "Alias fuer NOP (Index 0x00)"));
        BEHAVIORS.put(0x1F, new BehaviorEntry(0x8003f1d8, "NOP_1F", "Alias fuer NOP (Index 0x00)"));
        BEHAVIORS.put(0x20, new BehaviorEntry(0x8003f1d8, "NOP_20", "Alias fuer NOP (Index 0x00)"));
        BEHAVIORS.put(0x21, new BehaviorEntry(0x8003fcf4, "BEHAVIOR_21", "Unbekannt"));
    }

    /**
     * Bekannte Enemy-Typen aus der EMS-Datei.
     */
    public static final Map<Integer, String> ENEMY_TYPES = new LinkedHashMap<>();

    static {
        ENEMY_TYPES.put(0x10, "em10");  // Zombie
        ENEMY_TYPES.put(0x11, "em11");
        ENEMY_TYPES.put(0x12, "em12");
        ENEMY_TYPES.put(0x13, "em13");
        ENEMY_TYPES.put(0x16, "em16");
        ENEMY_TYPES.put(0x18, "em18");
        ENEMY_TYPES.put(0x1A, "em1A");
        ENEMY_TYPES.put(0x20, "em20");
        ENEMY_TYPES.put(0x21, "em21");
        ENEMY_TYPES.put(0x24, "em24");
        ENEMY_TYPES.put(0x25, "em25");
        ENEMY_TYPES.put(0x26, "em26");
        ENEMY_TYPES.put(0x27, "em27");
        ENEMY_TYPES.put(0x29, "em29");
        ENEMY_TYPES.put(0x2B, "em2B");  // Ziel-Enemy
        ENEMY_TYPES.put(0x2D, "em2D");
        ENEMY_TYPES.put(0x30, "em30");
        ENEMY_TYPES.put(0x36, "em36");
        ENEMY_TYPES.put(0x40, "em40");
        ENEMY_TYPES.put(0x42, "em42");
        ENEMY_TYPES.put(0x45, "em45");
        ENEMY_TYPES.put(0x47, "em47");
        ENEMY_TYPES.put(0x49, "em49");
        ENEMY_TYPES.put(0x4B, "em4B");
        ENEMY_TYPES.put(0x4D, "em4D");
    }

    public record BehaviorEntry(int address, String name, String description) {
        public String getSourceFileName() {
            return String.format("FUN_%08x.c", address);
        }

        public String getAddressHex() {
            return String.format("0x%08X", address);
        }
    }

    private BehaviorFunctionTable() {}
}
