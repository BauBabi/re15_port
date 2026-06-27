package de.re15.extractors;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Stream;
import java.util.stream.Collectors;

/**
 * SCD Script Disassembler fÃ¼r Resident Evil 1.5 - FINALE KORREKTE VERSION
 * Basierend auf der Dokumentation in information93.txt
 */
public class SCDScriptDisassembler {

    private static final int DOOR_OBJ_MOVE_SIZE = 22;

    private static class OpcodeInfo {
        String name;
        int size;
        String description;

        OpcodeInfo(String name, int size, String description) {
            this.name = name;
            this.size = size;
            this.description = description;
        }
    }

    private static class ScriptCommand {
        int offset;
        int opcode;
        String name;
        int size;
        Map<String, Object> parameters = new HashMap<>();

        // Externe Parameter fÃ¼r Ck-Befehle
        int ckFlagId = -1;
        int ckBit = -1;
        int ckValue = -1;
    }

    private static class ConditionalContext {
        int indent;
        boolean awaitingCondition = true;
    }

    private static class PendingWhileContext {
        int indent;
        int blockLength;
    }

    private static class PendingDoWhileContext {
        int indent;
        int conditionLength;
    }

    private static class ParseEvaluation {
        int score;
        int unknownCount;
        int controlFlowCount;
        int commandCount;
        boolean hasEvtEnd;
    }

    private static class RoomContext {
        static final RoomContext EMPTY = new RoomContext(-1, "", -1, List.of(), Map.of(), null);

        final int stage;
        final String roomCode;
        final int messageBlockStart;
        final List<Integer> messagePointers;
        final Map<Integer, String> messageFileNames;
        final Path rdtPath;

        RoomContext(int stage, String roomCode, int messageBlockStart, List<Integer> messagePointers, Map<Integer, String> messageFileNames, Path rdtPath) {
            this.stage = stage;
            this.roomCode = roomCode;
            this.messageBlockStart = messageBlockStart;
            this.messagePointers = messagePointers;
            this.messageFileNames = messageFileNames;
            this.rdtPath = rdtPath;
        }

        MessageEntry resolveMessage(int messageId) {
            if (messagePointers.isEmpty() && messageBlockStart < 0) {
                return null;
            }
            int pointerIndex = -1;
            int pointerValue = -1;
            if (!messagePointers.isEmpty()) {
                if (messageId >= 0 && messageId < messagePointers.size()) {
                    pointerIndex = messageId;
                    pointerValue = messagePointers.get(messageId);
                }
            }
            if (pointerValue < 0 && messageBlockStart >= 0) {
                int absolute = messageBlockStart + (messageId & 0xFFFF);
                pointerValue = absolute;
                if (!messagePointers.isEmpty()) {
                    pointerIndex = messagePointers.indexOf(absolute);
                }
            }
            String fileName = (pointerIndex >= 0)
                    ? messageFileNames.getOrDefault(pointerIndex, null)
                    : null;
            boolean usePointerLiteral = pointerValue >= 0;
            return new MessageEntry(pointerValue, fileName, usePointerLiteral, messageId);
        }

        MessageEntry resolveMessageByIndex(int messageIndex) {
            if (messageIndex < 0) {
                return null;
            }
            String fileName = messageFileNames.getOrDefault(messageIndex, null);
            if (!messagePointers.isEmpty() && messageIndex < messagePointers.size()) {
                int pointer = messagePointers.get(messageIndex);
                return new MessageEntry(pointer, fileName, true, messageIndex);
            }
            if (fileName != null) {
                return new MessageEntry(-1, fileName, false, messageIndex);
            }
            return null;
        }
    }

    private static class MessageEntry {
        final int pointer;
        final String fileName;
        final boolean usePointerLiteral;
        final int literalValue;

        MessageEntry(int pointer, String fileName, boolean usePointerLiteral, int literalValue) {
            this.pointer = pointer;
            this.fileName = fileName;
            this.usePointerLiteral = usePointerLiteral;
            this.literalValue = literalValue;
        }
    }

    private static class MessageTable {
        final int blockStart;
        final List<Integer> pointers;

        MessageTable(int blockStart, List<Integer> pointers) {
            this.blockStart = blockStart;
            this.pointers = pointers;
        }
    }

    private static final Map<Integer, OpcodeInfo> OPCODES = new HashMap<>();
    private static final Set<String> CONTROL_FLOW_COMMANDS = Set.of(
            "Ifel_ck",
            "Else_ck",
            "Endif",
            "For",
            "Next",
            "For2",
            "While",
            "Ewhile",
            "Do",
            "Edwhile",
            "Switch",
            "Case",
            "Default",
            "Eswitch",
            "Goto",
            "Gosub",
            "Return",
            "Break"
    );
    private static final int OBJ_MODEL_HEADER_SIZE = 6;
    private static final int OBJ_MODEL_MAX_WORDS = 15;
    private static final int OBJ_MODEL_BASE_SIZE = 2 + OBJ_MODEL_HEADER_SIZE;
    private static final int OBJ_MODEL_DYNAMIC_MAX_WORDS = 128;
    private static final int OBJ_MODEL_TERMINATOR_ZERO_BYTES = 8;
    private static final int OBJ_MODEL_FAILSAFE_WORDS = 15;
    private static final int RDT_MESSAGE_OFFSET = 60;
    private static final int[] RDT_SECTION_POINTER_OFFSETS = {
            8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 64, 68, 72, 76, 84, 88, 92
    };
    private static final Pattern MSG_FILE_PATTERN = Pattern.compile("(\\w+?)(\\d{2})\\.msg", Pattern.CASE_INSENSITIVE);

    private final Map<String, RoomContext> roomContextCache = new HashMap<>();
    private RoomContext currentRoomContext = RoomContext.EMPTY;
    private BlockMetadata currentBlockMetadata = BlockMetadata.EMPTY;

    static {
        registerOpcode(0x00, "nop", 1, "No operation");
        registerOpcode(0x01, "Evt_end", 2, "Exit current function");
        registerOpcode(0x02, "Evt_next", 1, "Wait for player input");
        registerOpcode(0x03, "Evt_chain", 4, "Reinitialize script execution");
        registerOpcode(0x04, "Evt_exec", 4, "Execute script when condition is met");
        registerOpcode(0x05, "Evt_kill", 2, "Disable event");
        registerOpcode(0x06, "Ifel_ck", 4, "IF block start");
        registerOpcode(0x07, "Else_ck", 4, "ELSE block start");
        registerOpcode(0x08, "Endif", 2, "End IF block");
        registerOpcode(0x09, "Sleep", 4, "Initialize Sleep");
        registerOpcode(0x0A, "Sleeping", 3, "Execute Sleep loop");
        registerOpcode(0x0B, "Wsleep", 1, "Initialize global sleep");
        registerOpcode(0x0C, "Wsleeping", 1, "Execute global sleep");
        registerOpcode(0x0D, "For", 6, "Initialize FOR loop");
        registerOpcode(0x0E, "Next", 2, "End of FOR block");
        registerOpcode(0x0F, "While", 4, "Start WHILE block");
        registerOpcode(0x10, "Ewhile", 2, "End WHILE block");
        registerOpcode(0x11, "Do", 4, "Start DO block");
        registerOpcode(0x12, "Edwhile", 2, "End DO block");
        registerOpcode(0x13, "Switch", 4, "Switch statement");
        registerOpcode(0x14, "Case", 6, "Case label");
        registerOpcode(0x15, "Default", 4, "Default label");
        registerOpcode(0x16, "Eswitch", 2, "End switch block");
        registerOpcode(0x17, "Goto", 6, "Jump");
        registerOpcode(0x18, "Gosub", 2, "Call subroutine");
        registerOpcode(0x19, "Return", 2, "Return from Gosub");
        registerOpcode(0x1A, "Break", 2, "Break out of block");
        registerOpcode(0x1B, "For2", 6, "Secondary loop construct");
        registerOpcode(0x1C, "Break_point", 1, "Breakpoint");
        registerOpcode(0x1D, "Work_copy", 1, "Script padding / work copy stub");
        registerOpcode(0x1E, "nop", 1, "NOP (padding)");
        registerOpcode(0x20, "nop", 1, "NOP (padding)");
        registerOpcode(0x21, "Ck", 4, "Check flag");
        registerOpcode(0x22, "Set", 4, "Set flag");
        registerOpcode(0x23, "Cmp", 6, "Compare value");
        registerOpcode(0x24, "Save", 4, "Assign value");
        registerOpcode(0x25, "Copy", 3, "Copy variable");
        registerOpcode(0x26, "Calc", 6, "Calculate value");
        registerOpcode(0x27, "Calc2", 4, "Calculate using variable operand");
        registerOpcode(0x28, "Sce_rnd", 1, "Randomise scene");
        registerOpcode(0x29, "Cut_chg", 2, "Change camera");
        registerOpcode(0x2A, "Cut_old", 1, "Previous camera");
        registerOpcode(0x2B, "Message_on", 4, "Show message");
        registerOpcode(0x2C, "Aot_set", 20, "Configure action trigger");
        registerOpcode(0x2D, "Obj_model_set", 34, "Configure room object model");  /* byte-true FIXED size (LAB_80040914 pc+=0x22); was 38+heuristic */
        registerOpcode(0x2E, "Work_set", 3, "Select work entity");
        registerOpcode(0x2F, "Speed_set", 4, "Set speed");
        registerOpcode(0x30, "Add_speed", 1, "Apply speed");
        registerOpcode(0x31, "Add_aspeed", 1, "Apply angular speed");
        registerOpcode(0x32, "Pos_set", 8, "Set position");
        registerOpcode(0x33, "Dir_set", 8, "Set direction");
        registerOpcode(0x34, "Member_set", 4, "Set member value");
        registerOpcode(0x35, "Member_set2", 3, "Set member (var)");
        registerOpcode(0x36, "Se_on", 12, "Play sound effect");
        registerOpcode(0x37, "Sca_id_set", 4, "Change camera trigger");
        registerOpcode(0x38, "Flr_set", 3, "Update floor flag");
        registerOpcode(0x39, "Sca_floor_set", 4, "SCA floor-byte set (LAB_80041814: writes value->entry.floor +0xb). Java previously mislabeled this 'Dir_ck'.");
        registerOpcode(0x3A, "Sce_espr_on", 16, "Enable sprite animation");
        registerOpcode(0x3B, "Door_aot_set", 32, "Configure door trigger");
        registerOpcode(0x3C, "Cut_auto", 2, "Toggle automatic camera switch");
        registerOpcode(0x3D, "Member_copy", 4, "Copy member to variable");
        registerOpcode(0x3E, "Member_cmp", 6, "Compare entity member");
        registerOpcode(0x3F, "Plc_motion", 4, "Set motion");
        registerOpcode(0x40, "Plc_dest", 8, "Move player");
        registerOpcode(0x41, "Plc_neck", 10, "Set neck orientation");
        registerOpcode(0x42, "Plc_ret", 1, "Return control");
        registerOpcode(0x43, "Plc_flg", 4, "Set player flag");
        registerOpcode(0x44, "Sce_em_set", 20, "Spawn enemy");
        registerOpcode(0x45, "Col_chg_set", 3, "Set collision region");
        registerOpcode(0x46, "Aot_reset", 10, "Reset action trigger");
        registerOpcode(0x47, "Aot_on", 2, "Activate trigger");
        registerOpcode(0x48, "Super_set", 16, "Configure super effect payload");
        registerOpcode(0x49, "Super_reset", 8, "Reset super effect");
        registerOpcode(0x4A, "Plc_gun", 2, "Set gun state");
        registerOpcode(0x4B, "Cut_replace", 3, "Swap camera transition");
        registerOpcode(0x4C, "Sce_espr_kill", 5, "Kill sprite");
        registerOpcode(0x4D, "Op4D", 22, "Unknown opcode 4D payload");
        registerOpcode(0x4E, "Item_aot_set", 22, "Configure item trigger (legacy)");
        registerOpcode(0x4F, "Sce_key_ck", 4, "Check key state");
        // ====================================================================
        // ⚠️ RE1.5 vs retail RE2 SCD OPCODE DIVERGENCE (BIO 1.5 → 2.0 format)
        // --------------------------------------------------------------------
        // The opcode table is NOT identical between RE1.5 (this prototype) and
        // retail RE2. RE-verified 2026-05-31 from the PSX dispatch jump table at
        // 0x800744a8 (ghidra1_V2.txt) vs the retail-RE2 table in
        // info/.../information293.txt. Two structural differences:
        //
        //  1) RE1.5 has ONLY opcodes 0x00..0x5E (95). The jump table ends at
        //     0x5E — RE1.5 has NO 0x5F..0x8E (no Xa_vol, Kage_set, Cut_be_set,
        //     *_4p AOT setters, Light_* lighting ops, Movie_on, Super_on,
        //     Sce_line_*, Se_vol, Poison_*, ... — those are RE2-only additions).
        //  2) The 0x50+ block is SHIFTED ~1 slot vs RE2, so the same FUNCTION
        //     sits at a different opcode number. Confirmed pairs:
        //
        //     opcode | RE1.5 (this prototype)        | retail RE2
        //     -------+-------------------------------+--------------------------
        //     0x50   | Item_aot_set (var 22/30)      | Sce_trg_ck (4)
        //     0x51   | Sce_key_ck (4)                | Sce_bgm_control (6)
        //     0x52   | flag-AND condition check (4)  | Sce_espr_control (6)
        //     0x53   | (fade-ish) (3)                | Sce_fade_set (6)
        //     0x54   | Sce_bgm_control (6)  ◄ FUN_80044da4 SsSeq play/stop
        //            |                               | Sce_espr3d_on (22)
        //     0x57   | (4)                           | Sce_bgmtbl_set (8)
        //
        //     So RE2's Sce_bgm_control (0x51) == RE1.5's 0x54. The old Java
        //     label "Sce_espr_control2" for 0x54 was simply WRONG (now fixed
        //     below to Sce_bgm_control). NEVER assume RE2 opcode numbers map to
        //     RE1.5 — cross-check the jump table / scd_vm.c.
        //
        //  ✅ FIXED 2026-05-31: the 0x52..0x5E lengths below are now the RE1.5
        //  jump-table-VERIFIED values (each handler's PC-advance read from the
        //  disasm), NOT the retail-RE2 lengths they used to carry. Verified
        //  changes: 0x52=4(was6) 0x53=3(was6) 0x56=6(was4) 0x57=4(was8)
        //  0x5A=6(was2) 0x5B=4(was2) 0x5C=4(was3) 0x5D=4(was2) 0x5E=4(was2).
        //  Re-disassembled all 1920 SCDs: unknown-opcode count unchanged (no new
        //  desync) and every affected script still terminates cleanly. The C
        //  engine (scd_vm.c s_opcode_sizes + op handlers) was fixed to match.
        //  Names for 0x52/0x57 etc. stay tentative (RE2-seeded, not library-
        //  fingerprinted). 0x38/0x4C/0x4F also differ from RE2 (conditional
        //  advances — left for a deeper pass; unused by current scripts).
        //  See memory reference_re15_vs_re2_scd_opcodes.md for the full table.
        // ====================================================================
        registerOpcode(0x50, "Item_aot_set", 22, "Configure item trigger");
        registerOpcode(0x51, "Sce_key_ck", 4, "Check key state");
        registerOpcode(0x52, "Sce_espr_control", 4, "RE1.5 len 4 (disasm: flag-AND check; retail RE2 Sce_espr_control=6). Name tentative.");
        registerOpcode(0x53, "Sce_fade_set", 3, "RE1.5 len 3 (disasm LAB_80040e18; retail RE2=6)");
        registerOpcode(0x54, "Sce_bgm_control", 6, "BGM/SsSeq sequence control: play/stop/pause/fade a sequence slot (PSX FUN_80044da4). NOT a sprite op — the old 'Sce_espr_control2' label was wrong. In RE1.5 this is opcode 0x54 (retail RE2 puts the same function at 0x51).");
        registerOpcode(0x55, "Member_calc", 6, "Entity member calculation");
        registerOpcode(0x56, "Member_calc2", 6, "RE1.5 len 6 (disasm LAB_80042a58 reads b@1/b@2/b@3/h@4; retail RE2=4)");
        registerOpcode(0x57, "Sce_bgmtbl_set", 4, "RE1.5 len 4 (disasm LAB_80042ab4→FUN_800216ec; NOT retail-RE2's Sce_bgmtbl_set=8 — RE1.5 0x57 is an unidentified 4-byte op). Name tentative.");
        registerOpcode(0x58, "Plc_rot", 4, "Rotate entity");
        registerOpcode(0x59, "Xa_on", 4, "Play XA audio");
        registerOpcode(0x5A, "Weapon_chg", 6, "RE1.5 len 6 (disasm LAB_80041474; retail RE2 Weapon_chg=2). Name tentative.");
        registerOpcode(0x5B, "Plc_cnt", 4, "RE1.5 len 4 (disasm LAB_800414e0; retail RE2 Plc_cnt=2). Name tentative.");
        registerOpcode(0x5D, "Mizu_div_set", 4, "RE1.5 len 4 (disasm LAB_8004203c; retail RE2 Mizu_div_set=2). Name tentative.");
        registerOpcode(0x5E, "Keep_Item_ck", 4, "RE1.5 len 4 (disasm LAB_80042b04; retail RE2 Keep_Item_ck=2). Name tentative.");
        registerOpcode(0x5F, "Xa_vol", 2, "Set XA volume");
        registerOpcode(0x62, "Sce_item_lost", 2, "Remove item from inventory");
        registerOpcode(0x5C, "Sce_shake_on", 4, "RE1.5 len 4 (disasm LAB_80042a10; retail RE2 Sce_shake_on=3)");
        registerOpcode(0x60, "Kage_set", 14, "Configure shadow volume");
        registerOpcode(0x61, "Cut_be_set", 4, "Configure camera transition");
        registerOpcode(0x63, "Plc_gun_eff", 1, "Trigger gun effect");
        registerOpcode(0x64, "Sce_espr_on2", 16, "Enable secondary sprite animation");
        registerOpcode(0x65, "Sce_espr_kill2", 2, "Kill secondary sprite animation");
        registerOpcode(0x66, "Plc_stop", 1, "Stop player movement");
        registerOpcode(0x6A, "Light_pos_set", 6, "Adjust light position");
        registerOpcode(0x6B, "Light_kido_set", 4, "Adjust light brightness");
        registerOpcode(0x7C, "Light_color_set", 6, "Adjust light colour");
        registerOpcode(0x80, "Se_vol", 2, "Set sound effect volume scale");
        registerOpcode(0x81, "Op81", 3, "Perform comparison (unknown)");
        registerOpcode(0x82, "Op82", 3, "Unknown opcode 0x82");
        registerOpcode(0x83, "Op83", 1, "Unknown opcode 0x83");
        registerOpcode(0x84, "Op84", 2, "Unknown opcode 0x84");
        registerOpcode(0x85, "Op85", 6, "Unknown opcode 0x85");
        registerOpcode(0x86, "Poison_ck", 1, "Check poison status");
        registerOpcode(0x87, "Poison_clr", 1, "Clear poison status");
        registerOpcode(0x88, "Sce_item_ck_lost", 3, "Check/remove item");
        registerOpcode(0x89, "Op89", 1, "Unknown opcode 0x89");
        registerOpcode(0x8A, "Nop8A", 6, "Padding (6 bytes)");
        registerOpcode(0x8B, "Nop8B", 6, "Padding (6 bytes)");
        registerOpcode(0x8C, "Nop8C", 1, "Padding / alignment (1 byte)");
        registerOpcode(0x8D, "Op8D", 24, "Room object payload");
        registerOpcode(0x8E, "Nop8E", 2, "No-op / alignment");
        registerOpcode(0x67, "Aot_set_4p", 28, "Configure wall trigger (4 point)");
        registerOpcode(0x68, "Door_aot_set_4p", 40, "Configure door trigger (4 point)");
        registerOpcode(0x69, "Item_aot_set_4p", 30, "Configure item trigger (4 point)");
        registerOpcode(0x6C, "Rbj_reset", 1, "Reset RBJ");
        registerOpcode(0x6D, "Sce_scr_move", 4, "Scroll background");
        registerOpcode(0x6E, "Parts_set", 6, "Set parts value");
        registerOpcode(0x6F, "Movie_on", 2, "Play movie");
        registerOpcode(0x70, "Splc_ret", 1, "Return special cut");
        registerOpcode(0x71, "Splc_sce", 1, "Special cut execute");
        registerOpcode(0x72, "Super_on", 16, "Enable super effect");
        registerOpcode(0x73, "Mirror_set", 8, "Configure mirror");
        registerOpcode(0x74, "Sce_fade_adjust", 4, "Adjust fade settings");
        registerOpcode(0x75, "Sce_espr3d_on2", 22, "Enable 3D sprite effect");
        registerOpcode(0x76, "Sce_item_get", 3, "Give item to player");
        registerOpcode(0x77, "Sce_line_start", 4, "Start spline line");
        registerOpcode(0x78, "Sce_line_main", 6, "Configure spline line");
        registerOpcode(0x79, "Sce_line_end", 1, "End spline line");
        registerOpcode(0x7A, "Sce_parts_bomb", 16, "Parts bomb effect");
        registerOpcode(0x7B, "Sce_parts_down", 16, "Parts down effect");
        registerOpcode(0x7D, "Light_pos_set2", 6, "Adjust light position for specific camera");
        registerOpcode(0x7E, "Light_kido_set2", 6, "Adjust light brightness for specific camera");
        registerOpcode(0x7F, "Light_color_set2", 6, "Adjust light colour for specific camera");
    }

    private static void registerOpcode(int opcode, String name, int size, String description) {
        OPCODES.put(opcode, new OpcodeInfo(name, size, description));
    }

    public static void main(String[] args) {
        System.out.println("=== SCD SCRIPT DISASSEMBLER FÃœR RESIDENT EVIL 1.5 ===");

        try {
            SCDScriptDisassembler disassembler = new SCDScriptDisassembler();
            if (args != null && args.length > 0) {
                for (String arg : args) {
                    Path scdPath = Paths.get(arg);
                    disassembler.disassembleInPlace(scdPath);
                }
            } else {
                disassembler.disassembleAllSCDFiles("info/Re1.5", "extracted");
            }
        } catch (Exception e) {
            System.err.println("Fehler: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void disassembleAllSCDFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Suche nach SCD-Dateien in: " + sourceRoot);

        Path sourcePath = Paths.get(sourceRoot);
        Path outputPath = Paths.get(outputRoot);
        Files.createDirectories(outputPath);

        if (!Files.exists(sourcePath)) {
            System.err.println("FEHLER: Quellverzeichnis existiert nicht: " + sourcePath);
            return;
        }

        System.out.println("Durchsuche Verzeichnis: " + sourcePath.toAbsolutePath());

        try (Stream<Path> walk = Files.walk(sourcePath)) {
            List<Path> scdFiles = walk.filter(Files::isRegularFile)
                    .filter(path -> path.toString().toUpperCase().endsWith(".SCD"))
                    .collect(java.util.stream.Collectors.toList());

            System.out.println("Gefundene SCD-Dateien: " + scdFiles.size());
            for (Path scdFile : scdFiles) {
                System.out.println("  - " + scdFile);
                disassembleFileToDirectory(scdFile, sourcePath, outputPath);
            }
        }

        System.out.println("SCD-Disassemblierung abgeschlossen!");
    }

    private void disassembleFileToDirectory(Path scdFile, Path sourceRoot, Path outputRoot) throws IOException {
        System.out.println("Disassembliere SCD-Datei: " + scdFile.getFileName());

        Path relativePath = sourceRoot.relativize(scdFile.getParent());
        Path outputDir = outputRoot.resolve(relativePath);
        Files.createDirectories(outputDir);

        String fileName = scdFile.getFileName().toString();
        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        currentRoomContext = loadRoomContext(scdFile);
        byte[] data = Files.readAllBytes(scdFile);
        System.out.println("  Dateigroesse: " + data.length + " bytes");

        BlockMetadata metadata = BlockMetadata.fromExistingLayout(outputDir, baseName);
        disassembleBlock(data, outputDir, baseName, metadata);
        System.out.println("  -> " + fileName + " Disassemblierung abgeschlossen");
        currentRoomContext = RoomContext.EMPTY;
    }

    public void disassembleInPlace(Path scdFile) throws IOException {
        Path parent = scdFile.getParent();
        if (parent == null) {
            parent = Paths.get(".");
        }
        disassembleFileToDirectory(scdFile, parent, parent);
    }

    public void disassembleBlock(byte[] data, Path outputDir, String baseName, BlockMetadata metadata) throws IOException {
        Objects.requireNonNull(outputDir, "outputDir");
        Objects.requireNonNull(baseName, "baseName");

        BlockMetadata effectiveMetadata = (metadata != null) ? metadata : BlockMetadata.EMPTY;
        currentBlockMetadata = effectiveMetadata;

        if (effectiveMetadata.hasRoomInfo()) {
            currentRoomContext = buildRoomContext(effectiveMetadata.stageName, effectiveMetadata.roomName, effectiveMetadata.roomDir);
        } else if (currentRoomContext == RoomContext.EMPTY) {
            Path potentialScd = outputDir.resolve(baseName + ".scd");
            if (Files.exists(potentialScd)) {
                currentRoomContext = loadRoomContext(potentialScd);
            } else {
                currentRoomContext = RoomContext.EMPTY;
            }
        }

        List<ScriptCommand> commands = parseScript(data);

        if (!commands.isEmpty()) {
            String cCode = generateCCode(commands, baseName);
            Files.createDirectories(outputDir);
            Path cFile = outputDir.resolve(baseName + ".c");
            Files.write(cFile, cCode.getBytes());
            System.out.println("  -> Disassembliert zu " + commands.size() + " Script-Befehlen");
            System.out.println("    - " + baseName + ".c (C Source Code)");
        } else {
            System.out.println("  ! Keine gueltigen Script-Befehle gefunden");
        }

        currentBlockMetadata = BlockMetadata.EMPTY;
        currentRoomContext = RoomContext.EMPTY;
    }

    private RoomContext loadRoomContext(Path scdFile) {
        try {
            Path scdParent = scdFile.getParent();
            if (scdParent == null) {
                return RoomContext.EMPTY;
            }
            Path roomDir = scdParent.getParent();
            if (roomDir == null) {
                return RoomContext.EMPTY;
            }
            Path stageDir = roomDir.getParent();
            if (stageDir == null) {
                return RoomContext.EMPTY;
            }
            String stageName = stageDir.getFileName().toString();
            String roomName = roomDir.getFileName().toString();
            String cacheKey = stageName + "/" + roomName;
            return roomContextCache.computeIfAbsent(cacheKey, key -> buildRoomContext(stageName, roomName, roomDir));
        } catch (Exception ex) {
            System.err.println("Warnung: Konnte RoomContext nicht bestimmen fuer " + scdFile + ": " + ex.getMessage());
            return RoomContext.EMPTY;
        }
    }

    private RoomContext buildRoomContext(String stageName, String roomName, Path roomDir) {
        int stageNumber = parseStageNumber(stageName);
        if (stageNumber < 0) {
            return RoomContext.EMPTY;
        }
        String roomCode = extractRoomCode(roomName, stageNumber);
        if (roomCode == null) {
            return RoomContext.EMPTY;
        }
        Path stageDir = (roomDir != null) ? roomDir.getParent() : null;
        if (stageDir == null) {
            return RoomContext.EMPTY;
        }
        String rdtFileName = String.format(Locale.ROOT, "ROOM%s.RDT", roomCode.toUpperCase(Locale.ROOT));
        Path rdtPath = stageDir.resolve(rdtFileName);
        if (!Files.exists(rdtPath)) {
            Path infoRoot = Paths.get("info", "Re1.5", "PSX", stageDir.getFileName().toString(), rdtFileName);
            if (Files.exists(infoRoot)) {
                rdtPath = infoRoot;
            }
        }
        MessageTable messageTable = readMessageTable(rdtPath);
        Map<Integer, String> messageFiles = collectMessageFileNames(roomDir.resolve("msg"), stageName, roomName);
        return new RoomContext(stageNumber, roomCode, messageTable.blockStart, messageTable.pointers, messageFiles, rdtPath);
    }

    private int parseStageNumber(String stageName) {
        if (stageName == null || !stageName.startsWith("STAGE")) {
            return -1;
        }
        String suffix = stageName.substring(5);
        try {
            return Integer.parseInt(suffix);
        } catch (NumberFormatException ex) {
            return -1;
        }
    }

    private String extractRoomCode(String roomName, int stageNumber) {
        if (roomName == null || !roomName.startsWith("room") || roomName.length() <= 4) {
            return null;
        }
        String suffix = roomName.substring(4);
        if (suffix.length() == 3 && stageNumber >= 0) {
            return String.format(Locale.ROOT, "%d%s", stageNumber, suffix);
        }
        return suffix;
    }

    private MessageTable readMessageTable(Path rdtPath) {
        if (rdtPath == null || !Files.exists(rdtPath)) {
            return new MessageTable(-1, List.of());
        }
        try {
            byte[] data = Files.readAllBytes(rdtPath);
            int messageStart = readLittleEndianInt(data, RDT_MESSAGE_OFFSET);
            if (messageStart <= 0 || messageStart >= data.length) {
                return new MessageTable(-1, List.of());
            }
            int messageEnd = data.length;
            for (int offset : RDT_SECTION_POINTER_OFFSETS) {
                if (offset == RDT_MESSAGE_OFFSET) {
                    continue;
                }
                int candidate = readLittleEndianInt(data, offset);
                if (candidate > messageStart && candidate < messageEnd) {
                    messageEnd = candidate;
                }
            }
            messageEnd = Math.min(messageEnd, data.length);
            int limit = Math.max(0, messageEnd - messageStart);
            List<Integer> pointers = new ArrayList<>();
            int previous = -1;
            for (int rel = 0; rel + 2 <= limit; rel += 2) {
                int value = readLittleEndianUnsignedShort(data, messageStart + rel);
                if (value == 0 || value >= limit || value <= previous) {
                    break;
                }
                pointers.add(messageStart + value);
                previous = value;
            }
            return new MessageTable(messageStart, pointers);
        } catch (IOException ex) {
            System.err.println("Warnung: Konnte RDT-Datei nicht lesen: " + rdtPath + " (" + ex.getMessage() + ")");
            return new MessageTable(-1, List.of());
        }
    }

    private Map<Integer, String> collectMessageFileNames(Path msgDir, String stageName, String roomName) {
        if (msgDir == null || !Files.isDirectory(msgDir)) {
            return Map.of();
        }
        Map<Integer, String> names = new HashMap<>();
        try (Stream<Path> stream = Files.list(msgDir)) {
            stream.filter(Files::isRegularFile).forEach(path -> {
                String name = path.getFileName().toString();
                Matcher matcher = MSG_FILE_PATTERN.matcher(name);
                if (matcher.matches()) {
                    try {
                        int index = Integer.parseInt(matcher.group(2));
                        String relative = "PSX/" + stageName + "/" + roomName + "/msg/" + name;
                        names.put(index, relative);
                    } catch (NumberFormatException ignored) {
                        // ignore malformed filenames
                    }
                }
            });
        } catch (IOException ex) {
            System.err.println("Warnung: Konnte MSG-Verzeichnis nicht lesen: " + msgDir + " (" + ex.getMessage() + ")");
        }
        return names;
    }

    private List<ScriptCommand> parseScript(byte[] data) {
        List<ScriptCommand> zeroParse = parseScriptCommands(data, 0);
        ParseEvaluation zeroEval = evaluateParse(zeroParse);

        int scriptStart = detectScriptStart(data);
        if (scriptStart > 0 && scriptStart < data.length) {
            byte[] scriptBytes = Arrays.copyOfRange(data, scriptStart, data.length);
            List<ScriptCommand> detectedParse = parseScriptCommands(scriptBytes, scriptStart);
            ParseEvaluation detectedEval = evaluateParse(detectedParse);
            if (isBetterParse(detectedEval, zeroEval)) {
                return detectedParse;
            }
        }
        return zeroParse;
    }

    private int detectScriptStart(byte[] data) {
        // Some SCDs start directly with opcodes; others begin with small tables.
        // We ignore very small pointer tables (first pointer < 0x10) to avoid
        // incorrectly skipping the first instructions.
        final int MIN_POINTER_START = 0x10;
        int pos = 0;
        int prev = -1;
        int count = 0;
        int firstPointer = -1;
        while (pos + 1 < data.length) {
            int value = readUnsignedShort(data, pos);
            if (value == 0 || value <= prev || value >= data.length) {
                break;
            }
            if (firstPointer == -1) {
                firstPointer = value;
            }
            prev = value;
            count++;
            pos += 2;
        }
        if (count >= 2
                && firstPointer >= MIN_POINTER_START
                && firstPointer >= pos
                && firstPointer < data.length) {
            return firstPointer;
        }
        return 0;
    }

    private List<ScriptCommand> parseScriptCommands(byte[] data, int baseOffset) {
        List<ScriptCommand> commands = new ArrayList<>();

        int offset = 0;
        while (offset < data.length) {
            if (offset < data.length - 10) {
                int lookahead = Math.min(10, data.length - offset);
                boolean zeroRun = true;
                for (int i = 0; i < lookahead; i++) {
                    if ((data[offset + i] & 0xFF) != 0x00) {
                        zeroRun = false;
                        break;
                    }
                }
                if (zeroRun) {
                    int next = offset + lookahead;
                    while (next < data.length && data[next] == 0) {
                        next++;
                    }
                    if (next >= data.length) {
                        break;
                    }
                    offset = next;
                    continue;
                }
            }

            if (offset >= data.length) break;

            int opcode = data[offset] & 0xFF;

            ScriptCommand command = parseCommand(data, offset, opcode, baseOffset);
            if (command != null) {
                commands.add(command);
                offset += command.size;
                // Do not stop on Return (0x19): early returns are often conditional, so more code can follow.
                if (command.opcode == 0x01) {
                    break;
                }
                if (command.name == null || "UNKNOWN".equals(command.name)) {
                    // Do not allow an endless loop on malformed data
                    if (command.size <= 0) {
                        break;
                    }
                }
            } else {
                offset++;
            }
        }

        return commands;
    }

    private ParseEvaluation evaluateParse(List<ScriptCommand> commands) {
        ParseEvaluation eval = new ParseEvaluation();
        eval.commandCount = commands.size();
        for (ScriptCommand command : commands) {
            if (command == null) {
                continue;
            }
            String name = command.name == null ? "" : command.name.trim();
            if (name.isEmpty()) {
                continue;
            }
            if ("UNKNOWN".equals(name)) {
                eval.unknownCount++;
                eval.score -= 5;
                continue;
            }
            eval.score += 1;
            if (CONTROL_FLOW_COMMANDS.contains(name)) {
                eval.controlFlowCount++;
                eval.score += 2;
            }
            if ("Evt_end".equalsIgnoreCase(name) || command.opcode == 0x01) {
                eval.hasEvtEnd = true;
                eval.score += 1;
            }
        }
        return eval;
    }

    private boolean isBetterParse(ParseEvaluation candidate, ParseEvaluation baseline) {
        if (candidate.commandCount > 0 && baseline.commandCount == 0) {
            return true;
        }
        if (candidate.commandCount == 0 && baseline.commandCount > 0) {
            return false;
        }
        if (candidate.score != baseline.score) {
            return candidate.score > baseline.score;
        }
        if (candidate.unknownCount != baseline.unknownCount) {
            return candidate.unknownCount < baseline.unknownCount;
        }
        if (candidate.controlFlowCount != baseline.controlFlowCount) {
            return candidate.controlFlowCount > baseline.controlFlowCount;
        }
        if (candidate.hasEvtEnd != baseline.hasEvtEnd) {
            return candidate.hasEvtEnd;
        }
        if (candidate.commandCount != baseline.commandCount) {
            return candidate.commandCount > baseline.commandCount;
        }
        return false;
    }

    private ScriptCommand parseCommand(byte[] data, int offset, int opcode, int baseOffset) {
        if (currentBlockMetadata.isDoorScript()) {
            if (opcode == 0x4F) {
                return parseDoorObjModelMoveCommand(data, offset, baseOffset);
            }
            if (opcode == 0x56) {
                return parseMemberCalcCommand(data, offset, baseOffset, opcode);
            }
            if (opcode == 0x57) {
                return parseDoorMemberCalc2Command(data, offset, baseOffset);
            }
        }

        OpcodeInfo opcodeInfo = OPCODES.get(opcode);

        if (opcodeInfo == null) {
            ScriptCommand command = new ScriptCommand();
            command.offset = baseOffset + offset;
            command.opcode = opcode;
            command.name = "UNKNOWN";
            command.size = (offset < data.length) ? 1 : 0;
            command.parameters = new HashMap<>();
            command.parameters.put("comment", "unknown opcode (" + formatHex(opcode, 2) + ") found at 0x" + formatHex(offset, 8));
            return command;
        }

        int requiredSize = opcodeInfo.size;   /* 0x2D is now a byte-true fixed 34 (no heuristic) */
        if (offset + requiredSize > data.length) {
            return null;
        }

        ScriptCommand command = new ScriptCommand();
        command.offset = baseOffset + offset;
        command.opcode = opcode;
        command.name = opcodeInfo.name;
        command.size = opcodeInfo.size;
        command.parameters = new HashMap<>();

        switch (opcode) {
            case 0x01: // return
                if (command.size >= 2) {
                    command.parameters.put("value", data[offset + 1] & 0xFF);
                }
                break;

            case 0x03: // Evt_chain
                command.parameters.put("arg1", data[offset + 1] & 0xFF);
                command.parameters.put("arg2", data[offset + 2] & 0xFF);
                command.parameters.put("arg3", data[offset + 3] & 0xFF);
                break;

            case 0x04: // Evt_exec
                command.parameters.put("condition", data[offset + 1] & 0xFF);
                command.parameters.put("value", readBigEndianUnsignedShort(data, offset + 2));
                break;

            case 0x05: // Evt_kill
                command.parameters.put("event", data[offset + 1] & 0xFF);
                break;

            case 0x06: // Ifel_ck
            case 0x07: // Else_ck
                /* block_length is LITTLE-ENDIAN (2026-06-08): the PSX reads the skip
                 * distance with `lhu` (LE); the engine op_if/op_else read it as
                 * pc[2]|pc[3]<<8 (verified vs scd_vm.c). Java read it BE (readShort),
                 * inflating every if/else skip by up to 256x. Same class as the
                 * already-LE Sleep (0x09) and For (0x0D) fixes. */
                command.parameters.put("block_length", readLittleEndianUnsignedShort(data, offset + 2));
                break;

            case 0x08: // Endif
                break;

            case 0x09: // Sleep
                int sleepTimer = 0;
                int duration = 0;
                if (offset + 1 < data.length) {
                    sleepTimer = data[offset + 1] & 0xFF;
                }
                if (offset + 3 < data.length) {
                    /* BO-round 2026-05-29: Sleep duration is LITTLE-ENDIAN. PSX
                     * LAB_8003f3e0 reads it via `lhu` (LE); the engine confirms
                     * (scd_vm.c op_sleep, Phase 4.5.13-E). The old BE read made
                     * every Sleep show 256× its real tick count. */
                    duration = readLittleEndianUnsignedShort(data, offset + 2);
                }
                command.parameters.put("timerId", sleepTimer);
                command.parameters.put("duration", duration);
                break;

            case 0x0A: // Sleeping
                command.parameters.put("count", readShort(data, offset + 1));
                break;

            case 0x0D: // For
                /* AO4-round 2026-05-26: PSX `lhu` reads block_length and
                 * count as LE u16 (info293.txt spec). Java previously
                 * displayed BE which made sub06 show `for(n < 39936)` —
                 * the actual PSX-canonical count is 0x9C = 156. */
                command.parameters.put("block_length", readLittleEndianUnsignedShort(data, offset + 2));
                command.parameters.put("count",        readLittleEndianUnsignedShort(data, offset + 4));
                break;

            case 0x0F: // While
                if (offset + 1 < data.length) {
                    command.parameters.put("dummy", data[offset + 1] & 0xFF);
                }
                if (offset + 2 < data.length) {
                    command.parameters.put("block_length", readShort(data, offset + 1));
                }
                break;

            case 0x10: // Ewhile
                if (offset + 1 < data.length) {
                    command.parameters.put("dummy", data[offset + 1] & 0xFF);
                }
                break;

            case 0x11: // Do
                if (offset + 1 < data.length) {
                    command.parameters.put("dummy", data[offset + 1] & 0xFF);
                }
                if (offset + 2 < data.length) {
                    command.parameters.put("block_length", readShort(data, offset + 1));
                }
                break;

            case 0x12: // Edwhile
                if (offset + 1 < data.length) {
                    command.parameters.put("condition_length", data[offset + 1] & 0xFF);
                }
                break;

            case 0x13: // Switch
                command.parameters.put("var", data[offset + 1] & 0xFF);
                /* block_length LITTLE-ENDIAN (PSX `lhu`), same class as Case below. */
                command.parameters.put("block_length", readLittleEndianUnsignedShort(data, offset + 2));
                break;

            case 0x14: // Case
                /* block_length AND value are LITTLE-ENDIAN (2026-06-08): the engine
                 * op_case reads skip=pc[2]|pc[3]<<8 and value=pc[4]|pc[5]<<8 (verified).
                 * Java read them BE (readShort), so e.g. ROOM1170 sub00's `case 11`
                 * (bytes 0b 00 = 0x000B LE) was mis-shown as `case 2816` (0x0B00 BE)
                 * and `case 3` as `case 768` — which misled scenario-dispatch RE. */
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("block_length", readLittleEndianUnsignedShort(data, offset + 2));
                command.parameters.put("value", readLittleEndianUnsignedShort(data, offset + 4));
                break;

            case 0x15: // Default
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("block_length", readLittleEndianUnsignedShort(data, offset + 2));
                break;

            case 0x16: // Eswitch
                if (offset + 1 < data.length) {
                    command.parameters.put("dummy", data[offset + 1] & 0xFF);
                }
                break;

            case 0x17: // Goto
                /* AO2-round 2026-05-26: PSX layout per information293.txt:432-438
                 * and PSX disasm of FUN_8001f3bc + sub04.scd raw bytes:
                 *   pc[0]   = opcode 0x17
                 *   pc[1]   = unknown0 (flag/cond field, often 0xFF)
                 *   pc[2]   = unknown1
                 *   pc[3]   = unknown2 (dummy/padding)
                 *   pc[4..5] = rel_jump int16 LE — relative to instruction start
                 * Previous: target = readUnsigned24Bit(offset+3) treated pc[3..5]
                 * as a BE u24, producing misleading C output (`goto(0x0000FEFF)`)
                 * instead of the canonical `goto(-2)`. */
                command.parameters.put("field0",  data[offset + 1] & 0xFF);
                command.parameters.put("field1",  data[offset + 2] & 0xFF);
                command.parameters.put("dummy",   data[offset + 3] & 0xFF);
                command.parameters.put("rel_jump", readLittleEndianSignedShort(data, offset + 4));
                /* keep "target" for backward-compat C-output rendering */
                command.parameters.put("target",  readLittleEndianSignedShort(data, offset + 4));
                break;

            case 0x18: // Gosub
                command.parameters.put("event", data[offset + 1] & 0xFF);
                break;

            case 0x19: // Return
                if (offset + 1 < data.length) {
                    command.parameters.put("dummy", data[offset + 1] & 0xFF);
                }
                break;

            case 0x1B: // For2
                command.parameters.put("block_length", readBigEndianShort(data, offset + 2));
                command.parameters.put("count", readBigEndianShort(data, offset + 4));
                break;

            case 0x21: // Ck
                int flagId = data[offset + 1] & 0xFF;
                int bit = data[offset + 2] & 0xFF;
                int value = data[offset + 3] & 0xFF;
                command.parameters.put("flag_id", flagId);
                command.parameters.put("bit", bit);
                command.parameters.put("value", value & 0x01);
                command.ckFlagId = flagId;
                command.ckBit = bit;
                command.ckValue = value & 0x01;
                break;

            case 0x22: // Set
                command.parameters.put("flag_id", data[offset + 1] & 0xFF);
                command.parameters.put("bit", data[offset + 2] & 0xFF);
                command.parameters.put("mode", data[offset + 3] & 0xFF);
                break;

            case 0x23: // Cmp
                command.parameters.put("targetType", data[offset + 1] & 0xFF);
                command.parameters.put("var", data[offset + 2] & 0xFF);
                command.parameters.put("compare", data[offset + 3] & 0xFF);
                /* 2026-06-17: the Cmp immediate is LITTLE-ENDIAN (PSX `lh`, handler LAB_8003ff68
                 * reads var=pc[2], op=pc[3], imm=lh pc[4..5]). It was read BIG-ENDIAN → e.g.
                 * ROOM1150 sub01 `Cmp(work_vars[0]==2)` printed as `==512` (0x0200 BE vs 0x0002
                 * LE). The runtime VM already reads it LE; this fixes the decompiled .c only. */
                command.parameters.put("value", readLittleEndianSignedShort(data, offset + 4));
                break;

            case 0x24: // Save
                command.parameters.put("target", data[offset + 1] & 0xFF);
                command.parameters.put("value", readShort(data, offset + 2));
                break;

            case 0x25: // Copy
                command.parameters.put("dst", data[offset + 1] & 0xFF);
                command.parameters.put("src", data[offset + 2] & 0xFF);
                break;

            case 0x26: // Calc
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("operation", data[offset + 2] & 0xFF);
                command.parameters.put("var", data[offset + 3] & 0xFF);
                command.parameters.put("value", readBigEndianUnsignedShort(data, offset + 4));
                break;

            case 0x27: // Calc2
                command.parameters.put("operation", data[offset + 1] & 0xFF);
                // Swap operand/target to match observed scripts (operand first, target last)
                command.parameters.put("var", data[offset + 3] & 0xFF);
                command.parameters.put("sourceVar", data[offset + 2] & 0xFF);
                break;

            case 0x28: // Sce_rnd
                break;

            case 0x29: // Cut_chg
                command.parameters.put("camera", data[offset + 1] & 0xFF);
                break;

            case 0x2A: // Cut_old
                break;

            case 0x2B: // Message_on
                /* BO-round 2026-05-29: bytes +2 and +3 are two INDEPENDENT u8
                 * args (arg2 / arg3 — e.g. voice_id at +2), NOT a BE u16. The
                 * engine (scd_vm.c op_message_on) reads them as separate bytes;
                 * the BE u16 read inflated the value 256×. */
                command.parameters.put("messageIndex", data[offset + 1] & 0xFF);
                command.parameters.put("messageArg2", data[offset + 2] & 0xFF);
                command.parameters.put("messageArg3", data[offset + 3] & 0xFF);
                command.parameters.put("messageFlags", readBigEndianUnsignedShort(data, offset + 2)); // legacy (deprecated)
                command.parameters.put("messageBaseOffset", -1);
                break;

            case 0x2C: // Aot_set
                command.parameters.put("id", data[offset + 1] & 0xFF);
                command.parameters.put("type", data[offset + 2] & 0xFF);
                command.parameters.put("floor", data[offset + 3] & 0xFF);
                command.parameters.put("chain", data[offset + 4] & 0xFF);
                command.parameters.put("switchFlag", data[offset + 5] & 0xFF);
                /* AOT rect is LITTLE-ENDIAN (2026-06-04 fix). Was BE-read, which
                 * made the trigger zone huge + mis-placed (see op_door_aot_set in
                 * the C engine + the Plc_dest X/Z LE precedent). */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("w", readLittleEndianSignedShort(data, offset + 10));
                command.parameters.put("h", readLittleEndianSignedShort(data, offset + 12));
                command.parameters.put("act", data[offset + 14] & 0xFF);
                command.parameters.put("priority", data[offset + 15] & 0xFF);
                int commandLimit = Math.min(data.length, offset + command.size);
                int eventKey = (offset + 16 < commandLimit) ? data[offset + 16] & 0xFF : 0;
                int eventId = (offset + 17 < commandLimit) ? data[offset + 17] & 0xFF : 0;
                int eventState = (offset + 19 < commandLimit)
                        ? readLittleEndianUnsignedShort(data, offset + 18)
                        : 0;
                int eventTrigger = (offset + 20 < commandLimit) ? data[offset + 20] & 0xFF : 0;
                command.parameters.put("eventKey", eventKey);
                command.parameters.put("eventId", eventId);
                command.parameters.put("eventState", eventState);
                command.parameters.put("eventTrigger", eventTrigger);
                break;

            case 0x2D: // Obj_model_set
                // BYTE-TRUE (LAB_80040914): a FIXED 34-byte opcode — the handler advances
                // the thread PC by exactly +0x22 in every (type & 0xc0) branch (verified vs
                // the real room1170 main00.scd: consecutive 0x2D at offsets 80 and 114 = 34
                // apart). The old zero-byte-terminator heuristic (determineObjModelEnd /
                // resolveObjModelTerminator) was a size GUESS that could mis-resolve and
                // misalign subsequent opcodes. Operand bytes from the opcode start:
                //   [1]=obj_id  [2]=render TYPE (pool+0x08, the FUN_8002c18c branch byte)
                //   [3]=grid id [4]=band(+0x82)  [6..7]=enable  [8..9]=flags
                //   [10/12/14]=X/Y/Z (LE)  [16/18/20]=rotX/Y/Z (LE)  [22..33]=collision box (6 LE shorts)
                command.size = 34;
                command.parameters.put("obj_id", data[offset + 1] & 0xFF);
                command.parameters.put("type",   data[offset + 2] & 0xFF);   // render type = pc[2], NOT pc[4]
                command.parameters.put("header",
                        readByteSequence(data, offset + 2, OBJ_MODEL_HEADER_SIZE));
                // Payload = the LE shorts at pc[8..33] (13 words). Was readBigEndianSignedShortList
                // — the SCD pos/rot/box operands are LITTLE-ENDIAN (R3000 `lh`); the old BE read
                // also bled 2 words out of the next opcode (it read 15 from an over-long size).
                int objPayloadWords = (command.size - OBJ_MODEL_BASE_SIZE) / 2;   // = 13
                List<Integer> payloadValues = readLittleEndianSignedShortList(
                        data,
                        offset + OBJ_MODEL_BASE_SIZE,
                        objPayloadWords
                );
                command.parameters.put("payload", payloadValues);
                command.parameters.put("payload_words", payloadValues.size());
                break;

            case 0x2E: // Work_set
                command.parameters.put("work_no", data[offset + 1] & 0xFF);
                command.parameters.put("value", data[offset + 2] & 0xFF);
                break;

            case 0x2F: // Speed_set
                /* AO3-round 2026-05-26: PSX LAB_80040f14 reads value via
                 * `lhu a1, 0x2(v0)` (MIPS R3000 LE halfword). Sub04's
                 * `2F 04 00 02` bytes decode LE=0x0200=512 (rotor spin
                 * 3.75 rev/sec), not BE=2 (256× too slow). Previous BE
                 * read made our engine appear to need vel_ry=2 but the
                 * real PSX value is 512. */
                command.parameters.put("axis",  data[offset + 1] & 0xFF);
                command.parameters.put("speed", readLittleEndianSignedShort(data, offset + 2));
                break;

            case 0x30: // Add_speed
                // parameterless in this script variant
                break;

            case 0x31: // Add_aspeed
                // parameterless
                break;

            case 0x32: // Pos_set
                /* X/Y/Z are LITTLE-ENDIAN s16 (R3000 `lh`), same class as
                 * Member_set / Plc_dest / Sce_em_set positions. The old
                 * readSignedShort (=BE) showed e.g. room1170/sub14 x=17106
                 * which places Leon BEHIND cut 12's camera (invisible); the
                 * real LE value is -11710 (door 0's courtyard arrival).
                 * Runtime scd_vm.c::op_pos_set was fixed to LE too. (2026-06-08) */
                int posRegister = data[offset + 1] & 0xFF;
                if (posRegister != 0) {
                    command.parameters.put("register", posRegister);
                }
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 2));
                command.parameters.put("y", readLittleEndianSignedShort(data, offset + 4));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 6));
                break;

            case 0x33: // Dir_set
                /* LITTLE-ENDIAN s16 (R3000 `lh`), mirrors Pos_set. (2026-06-08) */
                int dirRegister = data[offset + 1] & 0xFF;
                if (dirRegister != 0) {
                    command.parameters.put("register", dirRegister);
                }
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 2));
                command.parameters.put("y", readLittleEndianSignedShort(data, offset + 4));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 6));
                break;

            case 0x34: // Member_set
                /* AN-round (2026-05-26): value is LE-signed-16 per PSX
                 * handler LAB_800410b8 which reads via `lh a2, 0x2(v0)`.
                 * `lh` on MIPS R3000 is little-endian. Java's
                 * readSignedShort (= readBigEndianSignedShort) was wrong;
                 * its BE display showed "clean-looking" numbers that were
                 * actually corrupted (e.g. BE -1016 vs LE-canonical 2300
                 * for Leon's "Hey!" pos). Member_set values include actor
                 * X/Y/Z positions (IDs 0-2), rotations (IDs 3-5), motion
                 * (ID 8), and "off-map hide" coordinates like LE -31000
                 * which BE renders as a misleading -6010. Engine in
                 * scd_vm.c::op_member_set was already LE-correct (AG-round);
                 * only the disassembler output was misleading. */
                command.parameters.put("member", data[offset + 1] & 0xFF);
                command.parameters.put("value", readLittleEndianSignedShort(data, offset + 2));
                break;

            case 0x35: // Member_set2
                command.parameters.put("member", data[offset + 1] & 0xFF);
                command.parameters.put("var", data[offset + 2] & 0xFF);
                break;

            case 0x36: // Se_on
                command.parameters.put("bank", data[offset + 1] & 0xFF);
                command.parameters.put("id", data[offset + 2] & 0xFF);
                command.parameters.put("volume", data[offset + 3] & 0xFF);
                command.parameters.put("pan", data[offset + 4] & 0xFF);
                command.parameters.put("unknown", data[offset + 5] & 0xFF);
                /* pos X/Y/Z = LITTLE-ENDIAN s16 (R3000 `lhu`, PSX handler
                 * FUN_80041624) — sound-source world coords. (2026-06-08:
                 * was readSignedShort=BE; STAGE3+ uses nonzero pos.) */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("y", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 10));
                break;

            case 0x37: // Sca_id_set — [op, region, index, value]; byte-true LAB_8004175c
                // writes value -> SCA entry's u0 flag byte (offset +0x9). The two
                // operand bytes are SEPARATE (index, value); the old code fused them
                // into one BE short ("0x0A00"), hiding index=0x0a / value=0x00.
                command.parameters.put("region", data[offset + 1] & 0xFF);
                command.parameters.put("index", data[offset + 2] & 0xFF);
                command.parameters.put("value", data[offset + 3] & 0xFF);
                break;

            case 0x38: // Flr_set
                command.parameters.put("id", data[offset + 1] & 0xFF);
                command.parameters.put("flag", data[offset + 2] & 0xFF);
                break;

            case 0x39: // Sca_floor_set — [op, region, index, value]; byte-true LAB_80041814
                // Same addressing as Sca_id_set but writes value -> SCA entry's floor
                // byte (offset +0xb). NOT a direction check — the old "Dir_ck" name and
                // its id/dummy/angle params were wrong (5 fake args for a 4-byte op).
                command.parameters.put("region", data[offset + 1] & 0xFF);
                command.parameters.put("index", data[offset + 2] & 0xFF);
                command.parameters.put("value", data[offset + 3] & 0xFF);
                break;

            case 0x3A: // Sce_espr_on
                command.parameters.put("arg0", data[offset + 1] & 0xFF);
                command.parameters.put("arg1", data[offset + 2] & 0xFF);
                command.parameters.put("arg2", data[offset + 3] & 0xFF);
                command.parameters.put("arg3", data[offset + 4] & 0xFF);
                command.parameters.put("arg4", data[offset + 5] & 0xFF);
                command.parameters.put("arg5", data[offset + 7] & 0xFF);
                command.parameters.put("x", readBigEndianSignedShort(data, offset + 8));
                command.parameters.put("y", readBigEndianSignedShort(data, offset + 10));
                command.parameters.put("z", readBigEndianSignedShort(data, offset + 12));
                command.parameters.put("w", readBigEndianSignedShort(data, offset + 14));
                break;

            case 0x3B: // Door_aot_set
                command.parameters.put("id", data[offset + 1] & 0xFF);
                command.parameters.put("aot", data[offset + 2] & 0xFF);
                command.parameters.put("floor", data[offset + 3] & 0xFF);
                command.parameters.put("chain", data[offset + 4] & 0xFF);
                command.parameters.put("switchFlag", data[offset + 5] & 0xFF);
                /* All s16 fields LITTLE-ENDIAN (2026-06-04 fix). Were BE-read,
                 * making the door trigger rect huge + mis-placed (door 0 covered
                 * half the room -> spurious firing). Verified vs raw main00.scd +
                 * the C engine op_door_aot_set fix + the Plc_dest X/Z LE precedent.
                 * Entry (next_*) is the same opcode's uniform s16 -> also LE. */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("w", readLittleEndianSignedShort(data, offset + 10));
                command.parameters.put("h", readLittleEndianSignedShort(data, offset + 12));
                command.parameters.put("next_x", readLittleEndianSignedShort(data, offset + 14));
                command.parameters.put("next_y", readLittleEndianSignedShort(data, offset + 16));
                command.parameters.put("next_z", readLittleEndianSignedShort(data, offset + 18));
                command.parameters.put("next_dir", readLittleEndianSignedShort(data, offset + 20));
                command.parameters.put("stage", data[offset + 22] & 0xFF);
                command.parameters.put("room", data[offset + 23] & 0xFF);
                command.parameters.put("camera", data[offset + 24] & 0xFF);
                command.parameters.put("texture", data[offset + 25] & 0xFF);
                command.parameters.put("door_type", data[offset + 26] & 0xFF);
                command.parameters.put("door_lock", data[offset + 27] & 0xFF);
                command.parameters.put("unknown2", data[offset + 28] & 0xFF);
                command.parameters.put("door_locked", data[offset + 29] & 0xFF);
                command.parameters.put("door_key", data[offset + 30] & 0xFF);
                command.parameters.put("unknown3", data[offset + 31] & 0xFF);
                break;

            case 0x3C: // Cut_auto
                command.parameters.put("state", data[offset + 1] & 0xFF);
                break;

            case 0x3D: // Member_copy
                command.parameters.put("var", data[offset + 1] & 0xFF);
                command.parameters.put("member", readLittleEndianUnsignedShort(data, offset + 2));
                break;

            case 0x3E: // Member_cmp
                command.parameters.put("var", data[offset + 2] & 0xFF);
                command.parameters.put("compare", data[offset + 3] & 0xFF);
                command.parameters.put("value", readBigEndianSignedShort(data, offset + 4));
                break;

            case 0x3F: // Plc_motion
                command.parameters.put("arg1", data[offset + 1] & 0xFF);
                command.parameters.put("arg2", data[offset + 2] & 0xFF);
                command.parameters.put("arg3", data[offset + 3] & 0xFF);
                break;

            case 0x40: // Plc_dest
                int plcRegister = data[offset + 1] & 0xFF;
                if (plcRegister != 0) {
                    command.parameters.put("register", plcRegister);
                }
                /* BU-round (2026-06-04): pc[2] is the MOTION MODE, pc[3] a flag
                 * byte — NOT a single big-endian "destination" u16. Proven from
                 * the PSX Plc_dest handler FUN_80041BE4:
                 *   lbu a2,0x2(v1)  → mode  (a2)
                 *   lbu v1,0x3(v1)  → flag  (stored to player+0x1C3 @80041c24)
                 * The handler then switches on (mode-4) and reads a PER-MODE
                 * motion table indexed by the player anim-set (player+0x8):
                 *   mode 4 WALK → DAT_80073EA5 → motion id 0x30 (speed 0x4B/75)
                 *   mode 5 RUN  → DAT_80073F25 → motion id 0x48 (speed 0xC8/200)
                 *   mode 9 TURN → DAT_80073EE5 → motion id 0x60 (speed 0)
                 * So WALK and RUN are DISTINCT motions. The old code merged
                 * mode+flag into one number (e.g. 0x05,0x21 → "1313"), hiding the
                 * mode entirely — which is why the run/walk distinction was
                 * invisible in the decompiled output. */
                command.parameters.put("mode", data[offset + 2] & 0xFF);   /* pc[2] motion mode */
                command.parameters.put("flag", data[offset + 3] & 0xFF);   /* pc[3] flag → player+0x1C3 */
                /* AK-round (2026-05-26): X/Z are LE per PSX.EXE handler at
                 * 0x80041BE4 (`lhu v0, 0x4(v0)` / `lhu v1, 0x6(v0)`). Previous
                 * readSignedShort was BE-aliased — produced literals like
                 * "11255, -24810" for bytes (2B F7 9F 16) whose engine-canonical
                 * LE decode is (-2261, 5791). See memory
                 * bugfix_AK_round_cut2_camera_mismatch_2026_05_26. */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 4));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 6));
                break;

            case 0x41: // Plc_neck
                /* BO-round 2026-05-29: operands are FOUR LE s16 halfwords
                 * (yaw, pitch, roll, speed) read via `lhu` (PSX LAB_80041e98);
                 * the engine (scd_vm.c op_plc_neck) confirms. The old code split
                 * the first s16 into two u8 "angles" and BE-read the rest. */
                command.parameters.put("mode", data[offset + 1] & 0xFF);
                command.parameters.put("yaw", readLittleEndianSignedShort(data, offset + 2));
                command.parameters.put("pitch", readLittleEndianSignedShort(data, offset + 4));
                command.parameters.put("roll", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("speed", readLittleEndianSignedShort(data, offset + 8));
                break;

            case 0x43: // Plc_flg
                command.parameters.put("entity", data[offset + 1] & 0xFF);
                command.parameters.put("flag", data[offset + 2] & 0xFF);
                command.parameters.put("value", data[offset + 3] & 0xFF);
                break;

            case 0x44: // Sce_em_set
                command.parameters.put("state", data[offset + 1] & 0xFF);
                command.parameters.put("enemy_id", data[offset + 2] & 0xFF);
                command.parameters.put("script_id", data[offset + 3] & 0xFF);
                command.parameters.put("sound_bank", data[offset + 4] & 0xFF);
                command.parameters.put("texture", data[offset + 5] & 0xFF);
                command.parameters.put("kill_flag", data[offset + 6] & 0xFF);
                command.parameters.put("spawn_flag", data[offset + 7] & 0xFF);
                /* AK-round (2026-05-26): X/Y/Z + dir are LE per PSX engine
                 * scd_vm.c::op_sce_em_set + memory bugfix_obj_sce_em_positions_LE.
                 * The Java disassembler previously displayed BE-interpretation
                 * literals (e.g. Y=-7965 for bytes e0 e3) which led prior
                 * sessions to false-positive bugs. LE-canonical decode:
                 * bytes e0 e3 → Y=-7200 (engine-canonical). */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("y", readLittleEndianSignedShort(data, offset + 10));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 12));
                /* BO-round 2026-05-29: the real direction (DirectionY = yaw) is
                 * LE @ +16, not +14. PSX/engine op_sce_em_set reads pc[16] LE
                 * (T-SCEEM-DIR forensic: 8/8 NPCs yield valid Q12 yaws only via
                 * +16 LE; +14 is dirX ≈ 0). The old @+14 showed dir=0 for every
                 * enemy and stashed the real yaw in param0. */
                command.parameters.put("dir",    readLittleEndianSignedShort(data, offset + 16)); // DirectionY = yaw
                command.parameters.put("param0", readLittleEndianSignedShort(data, offset + 14)); // dirX (~0)
                command.parameters.put("param1", readLittleEndianSignedShort(data, offset + 18)); // dirZ (~0)
                break;

            case 0x45: // Col_chg_set
                command.parameters.put("fromId", data[offset + 1] & 0xFF);
                command.parameters.put("fromFlag", data[offset + 2] & 0xFF);
                command.parameters.put("toId", 0);
                command.parameters.put("toFlag", 0);
                command.parameters.put("mode", 177);  // PSOUND reference constant
                break;

            case 0x46: // Aot_reset(id, sce, sat, value0, value1, value2) — re-types AOT slot.
                /* value0..2 are LITTLE-ENDIAN (the runtime op_aot_reset / disasm LAB_80040738
                 * read `pc[4]|(pc[5]<<8)` etc.). Were read BE → e.g. ROOM1130 sub01
                 * Aot_reset(3, sce=1, …, value0=0x0001) showed 0x0100=256 instead of msg
                 * index 1. `type` = the SCE byte (1 = examine/MESSAGE AOT, 2 = door); for
                 * sce=1 value0 is the .msg index shown on action. */
                command.parameters.put("id", data[offset + 1] & 0xFF);
                command.parameters.put("type", data[offset + 2] & 0xFF);
                command.parameters.put("floor", data[offset + 3] & 0xFF);
                command.parameters.put("value0", readLittleEndianUnsignedShort(data, offset + 4));
                command.parameters.put("value1", readLittleEndianUnsignedShort(data, offset + 6));
                command.parameters.put("value2", readLittleEndianUnsignedShort(data, offset + 8));
                break;

            case 0x47: // Aot_on
                command.parameters.put("id", data[offset + 1] & 0xFF);
                break;

            case 0x48: // Super_set
                command.parameters.put("payload", readByteSequence(data, offset + 1, 15));
                break;

            case 0x4A: // Plc_gun
                command.parameters.put("mode", data[offset + 1] & 0xFF);
                break;

            case 0x4B: // Cut_replace
                command.parameters.put("from", data[offset + 1] & 0xFF);
                command.parameters.put("to", data[offset + 2] & 0xFF);
                break;

            case 0x4C: // Sce_espr_kill
                command.parameters.put("id", data[offset + 1] & 0xFF);
                command.parameters.put("type", data[offset + 2] & 0xFF);
                command.parameters.put("work", data[offset + 3] & 0xFF);
                command.parameters.put("idx", data[offset + 4] & 0xFF);
                break;

            case 0x4D: // Op4D
                command.parameters.put("payload", readByteSequence(data, offset + 1, 21));
                break;

            case 0x4E: // Item_aot_set (legacy)
                command.parameters.put("aot", data[offset + 1] & 0xFF);
                command.parameters.put("id", data[offset + 2] & 0xFF);
                command.parameters.put("type", data[offset + 3] & 0xFF);
                command.parameters.put("floor", data[offset + 4] & 0xFF);
                command.parameters.put("super", data[offset + 5] & 0xFF);
                /* AOT rect is LITTLE-ENDIAN (2026-06-04 fix, same as Door_aot_set). */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("w", readLittleEndianSignedShort(data, offset + 10));
                command.parameters.put("h", readLittleEndianSignedShort(data, offset + 12));
                command.parameters.put("item", readBigEndianUnsignedShort(data, offset + 14));
                command.parameters.put("amount", readBigEndianUnsignedShort(data, offset + 16));
                command.parameters.put("flag", readBigEndianUnsignedShort(data, offset + 18));
                command.parameters.put("model_act", readBigEndianUnsignedShort(data, offset + 20));
                break;

            case 0x4F: // Sce_key_ck
                command.parameters.put("state", data[offset + 1] & 0xFF);
                command.parameters.put("trigger", readShort(data, offset + 2));
                break;

            case 0x50: // Item_aot_set
                command.parameters.put("aot", data[offset + 1] & 0xFF);
                command.parameters.put("id", data[offset + 2] & 0xFF);
                command.parameters.put("type", data[offset + 3] & 0xFF);
                command.parameters.put("floor", data[offset + 4] & 0xFF);
                command.parameters.put("super", data[offset + 5] & 0xFF);
                /* AOT rect is LITTLE-ENDIAN (2026-06-04 fix, same as Door_aot_set). */
                command.parameters.put("x", readLittleEndianSignedShort(data, offset + 6));
                command.parameters.put("z", readLittleEndianSignedShort(data, offset + 8));
                command.parameters.put("w", readLittleEndianSignedShort(data, offset + 10));
                command.parameters.put("h", readLittleEndianSignedShort(data, offset + 12));
                command.parameters.put("item", readBigEndianUnsignedShort(data, offset + 14));
                command.parameters.put("amount", readBigEndianUnsignedShort(data, offset + 16));
                command.parameters.put("flag", readBigEndianUnsignedShort(data, offset + 18));
                command.parameters.put("model_act", readBigEndianUnsignedShort(data, offset + 20));
                break;

            case 0x51: // Sce_key_ck (alias)
                command.parameters.put("state", data[offset + 1] & 0xFF);
                command.parameters.put("trigger", readShort(data, offset + 2));
                break;

            case 0x53: // Sce_fade_set - RE1.5 = 3 bytes (opcode + 2 operands)
                // disasm LAB_80040e18 reads only b@1 and b@2 then advances PC by 3
                // (retail RE2's 6-byte/5-operand form is a different opcode-table).
                command.parameters.put("param1", data[offset + 1] & 0xFF);
                command.parameters.put("param2", data.length > offset + 2 ? data[offset + 2] & 0xFF : 0);
                break;

            case 0x54: // Sce_bgm_control - SsSeq sequence control (6 bytes)
                // opcode + 5 operands: [slot, ctrl, ...]. ctrl 1=SsSeqPlay(loop),
                // 2=SsSeqStop, 3=SsSeqReplay, 4=SsSeqPause, 5=SsSeqSetDecrescendo.
                command.parameters.put("param1", data[offset + 1] & 0xFF);
                command.parameters.put("param2", data[offset + 2] & 0xFF);
                command.parameters.put("param3", data[offset + 3] & 0xFF);
                command.parameters.put("param4", data.length > offset + 4 ? data[offset + 4] & 0xFF : 0);
                command.parameters.put("param5", data.length > offset + 5 ? data[offset + 5] & 0xFF : 0);
                break;

            case 0x55: // Member_calc
                ScriptCommand memberCalc = parseMemberCalcCommand(data, offset, baseOffset, opcode);
                if (memberCalc == null) {
                    return null;
                }
                return memberCalc;

            case 0x56: // Member_calc2
                command.parameters.put("param0", data[offset + 1] & 0xFF);
                command.parameters.put("param1", data[offset + 2] & 0xFF);
                command.parameters.put("param2", data[offset + 3] & 0xFF);
                break;

            case 0x57: // RE1.5 = 4 bytes (disasm LAB_80042ab4→FUN_800216ec, PC+=4).
                // NOT retail-RE2's 8-byte Sce_bgmtbl_set; RE1.5 0x57 is an
                // unidentified 4-byte op. Read only its 3 in-bounds operand bytes.
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("value0", data.length > offset + 2 ? data[offset + 2] & 0xFF : 0);
                command.parameters.put("value1", data.length > offset + 3 ? data[offset + 3] & 0xFF : 0);
                break;

            case 0x58: // Plc_rot (RE2 name; RE1.5 0x58 is really a Ck-style flag-check,
                       // never used in any room). +2 operand = LITTLE-ENDIAN (lhu, LAB_8003fd54).
                command.parameters.put("axis", data[offset + 1] & 0xFF);
                command.parameters.put("value", readLittleEndianSignedShort(data, offset + 2));
                break;

            case 0x59: // Xa_on
                command.parameters.put("channel", data[offset + 1] & 0xFF);
                command.parameters.put("sample", readShort(data, offset + 2));
                break;

            case 0x5A: // Weapon_chg
                command.parameters.put("item", data[offset + 1] & 0xFF);
                break;

            case 0x5C: // Sce_shake_on
                command.parameters.put("amplitude", (int) (byte) data[offset + 1]);
                command.parameters.put("duration", (int) (byte) data[offset + 2]);
                break;

            case 0x5D: // Mizu_div_set
                command.parameters.put("value", data[offset + 1] & 0xFF);
                break;

            case 0x5E: // Keep_Item_ck
                command.parameters.put("item", data[offset + 1] & 0xFF);
                break;

            case 0x5F: // Xa_vol
                command.parameters.put("value", data[offset + 1] & 0xFF);
                break;

            case 0x60: // Kage_set
                command.parameters.put("b0", data[offset + 1] & 0xFF);
                command.parameters.put("b1", data[offset + 2] & 0xFF);
                command.parameters.put("b2", data[offset + 3] & 0xFF);
                command.parameters.put("b3", data[offset + 4] & 0xFF);
                command.parameters.put("b4", data[offset + 5] & 0xFF);
                command.parameters.put("s0", readShort(data, offset + 6));
                command.parameters.put("s1", readShort(data, offset + 8));
                command.parameters.put("s2", readShort(data, offset + 10));
                command.parameters.put("s3", readShort(data, offset + 12));
                break;

            case 0x61: // Cut_be_set
                command.parameters.put("value0", data[offset + 1] & 0xFF);
                command.parameters.put("value1", data[offset + 2] & 0xFF);
                command.parameters.put("value2", data[offset + 3] & 0xFF);
                break;

            case 0x62: // Sce_item_lost
                command.parameters.put("item", data[offset + 1] & 0xFF);
                break;

            case 0x63: // Plc_gun_eff
                break;

            case 0x64: // Sce_espr_on2
                List<Integer> esprPayload = new ArrayList<>(15);
                for (int i = 0; i < 15; i++) {
                    esprPayload.add(data[offset + 1 + i] & 0xFF);
                }
                command.parameters.put("payload", esprPayload);
                break;

            case 0x65: // Sce_espr_kill2
                command.parameters.put("value", data[offset + 1] & 0xFF);
                break;

            case 0x66: // Plc_stop
                break;

            case 0x67: // Aot_set_4p
                command.parameters.put("id", data[offset + 1] & 0xFF);
                List<Integer> aot4pUnknown = new ArrayList<>();
                for (int i = 0; i < 2; i++) {
                    aot4pUnknown.add(readShort(data, offset + 2 + (i * 2)));
                }
                List<Integer> aot4pCoords = new ArrayList<>();
                for (int i = 0; i < 8; i++) {
                    aot4pCoords.add(readShort(data, offset + 6 + (i * 2)));
                }
                List<Integer> aot4pTail = new ArrayList<>();
                for (int i = 0; i < 3; i++) {
                    aot4pTail.add(readShort(data, offset + 22 + (i * 2)));
                }
                command.parameters.put("unknown", aot4pUnknown);
                command.parameters.put("coords", aot4pCoords);
                command.parameters.put("tail", aot4pTail);
                break;

            case 0x68: // Door_aot_set_4p
                command.parameters.put("id", data[offset + 1] & 0xFF);
                List<Integer> door4pData = new ArrayList<>();
                for (int i = 0; i < 19; i++) {
                    door4pData.add(readShort(data, offset + 2 + (i * 2)));
                }
                command.parameters.put("data", door4pData);
                break;

            case 0x69: // Item_aot_set_4p
                command.parameters.put("id", data[offset + 1] & 0xFF);
                List<Integer> item4pData = new ArrayList<>();
                for (int i = 0; i < 14; i++) {
                    item4pData.add(readShort(data, offset + 2 + (i * 2)));
                }
                command.parameters.put("data", item4pData);
                break;

            case 0x6A: // Light_pos_set
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("light", data[offset + 2] & 0xFF);
                command.parameters.put("param", data[offset + 3] & 0xFF);
                command.parameters.put("value", readShort(data, offset + 4));
                break;

            case 0x6B: // Light_kido_set
                command.parameters.put("light", data[offset + 1] & 0xFF);
                command.parameters.put("brightness", readShort(data, offset + 2));
                break;

            case 0x6C: // Rbj_reset
                break;

            case 0x6D: // Sce_scr_move
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("value", readShort(data, offset + 2));
                break;

            case 0x6E: // Parts_set
                command.parameters.put("dummy", data[offset + 1] & 0xFF);
                command.parameters.put("param0", data[offset + 2] & 0xFF);
                command.parameters.put("param1", data[offset + 3] & 0xFF);
                command.parameters.put("value", readShort(data, offset + 4));
                break;

            case 0x6F: // Movie_on
                command.parameters.put("id", data[offset + 1] & 0xFF);
                break;

            case 0x70: // Splc_ret
            case 0x71: // Splc_sce
                break;

            case 0x72: // Super_on
                command.parameters.put("payload", readByteSequence(data, offset + 1, 15));
                break;

            case 0x73: // Mirror_set
                command.parameters.put("payload", readByteSequence(data, offset + 1, 7));
                break;

            case 0x74: // Sce_fade_adjust
                command.parameters.put("payload", readByteSequence(data, offset + 1, 3));
                break;

            case 0x75: // Sce_espr3d_on2
                command.parameters.put("payload", readByteSequence(data, offset + 1, 21));
                break;

            case 0x76: // Sce_item_get
                command.parameters.put("item", data[offset + 1] & 0xFF);
                command.parameters.put("amount", data[offset + 2] & 0xFF);
                break;

            case 0x77: // Sce_line_start
                command.parameters.put("payload", readByteSequence(data, offset + 1, 3));
                break;

            case 0x78: // Sce_line_main
                command.parameters.put("payload", readByteSequence(data, offset + 1, 5));
                break;

            case 0x79: // Sce_line_end
                break;

            case 0x7A: // Sce_parts_bomb
                command.parameters.put("payload", readByteSequence(data, offset + 1, 15));
                break;

            case 0x7B: // Sce_parts_down
                command.parameters.put("payload", readByteSequence(data, offset + 1, 15));
                break;

            case 0x7C: // Light_color_set
                command.parameters.put("light", data[offset + 1] & 0xFF);
                command.parameters.put("r", data[offset + 2] & 0xFF);
                command.parameters.put("g", data[offset + 3] & 0xFF);
                command.parameters.put("b", data[offset + 4] & 0xFF);
                command.parameters.put("extra", data[offset + 5] & 0xFF);
                break;

            case 0x7D: // Light_pos_set2
                command.parameters.put("camera", data[offset + 1] & 0xFF);
                command.parameters.put("light", data[offset + 2] & 0xFF);
                command.parameters.put("param", data[offset + 3] & 0xFF);
                command.parameters.put("value", readSignedShort(data, offset + 4));
                break;

            case 0x7E: // Light_kido_set2
                command.parameters.put("camera", data[offset + 2] & 0xFF);
                command.parameters.put("light", data[offset + 3] & 0xFF);
                command.parameters.put("brightness", readSignedShort(data, offset + 4));
                break;

            case 0x7F: // Light_color_set2
                command.parameters.put("camera", data[offset + 1] & 0xFF);
                command.parameters.put("light", data[offset + 2] & 0xFF);
                command.parameters.put("r", data[offset + 3] & 0xFF);
                command.parameters.put("g", data[offset + 4] & 0xFF);
                command.parameters.put("b", data[offset + 5] & 0xFF);
                break;

            case 0x80: // Se_vol
                command.parameters.put("value", data[offset + 1] & 0xFF);
                break;

            case 0x81: // Op81
                command.parameters.put("payload", readByteSequence(data, offset + 1, 2));
                break;

            case 0x82: // Op82
                command.parameters.put("payload", readByteSequence(data, offset + 1, 2));
                break;

            case 0x83: // Op83
                command.parameters.put("payload", Collections.emptyList());
                break;

            case 0x84: // Op84
                command.parameters.put("payload", readByteSequence(data, offset + 1, 1));
                break;

            case 0x85: // Op85
                command.parameters.put("payload", readByteSequence(data, offset + 1, 5));
                break;

            case 0x86: // Poison_ck
                break;

            case 0x87: // Poison_clr
                break;

            case 0x88: // Sce_item_ck_lost
                command.parameters.put("item", data[offset + 1] & 0xFF);
                command.parameters.put("mode", data[offset + 2] & 0xFF);
                break;

            case 0x89: // Op89
                break;

            case 0x8A: // Nop8A
            case 0x8B: // Nop8B
                command.parameters.put("payload", readByteSequence(data, offset + 1, command.size - 1));
                break;

            case 0x8C: // Nop8C
                break;

            case 0x8D: // Op8D
                command.parameters.put("payload", readByteSequence(data, offset + 1, command.size - 1));
                break;

            case 0x8E: // Nop8E
                if (offset + 1 < data.length) {
                    command.parameters.put("value", data[offset + 1] & 0xFF);
                }
                break;

            default:
                break;
        }

        return command;
    }

    private ScriptCommand parseDoorObjModelMoveCommand(byte[] data, int offset, int baseOffset) {
        if (offset + DOOR_OBJ_MOVE_SIZE > data.length) {
            return null;
        }
        ScriptCommand command = new ScriptCommand();
        command.offset = baseOffset + offset;
        command.opcode = 0x4F;
        command.name = "Obj_model_move";
        command.size = DOOR_OBJ_MOVE_SIZE;
        command.parameters = new HashMap<>();

        List<Integer> args = new ArrayList<>(13);
        args.add(Byte.toUnsignedInt(data[offset + 1]));
        args.add(Byte.toUnsignedInt(data[offset + 2]));
        args.add(Byte.toUnsignedInt(data[offset + 3]));
        args.add(Byte.toUnsignedInt(data[offset + 4]));
        args.add(Byte.toUnsignedInt(data[offset + 5]));
        for (int i = 0; i < 8; i++) {
            boolean unsigned = (i == 0);
            args.add(readDoorShort(data, offset + 6 + (i * 2), unsigned));
        }
        command.parameters.put("args", args);
        return command;
    }

    private ScriptCommand parseMemberCalcCommand(byte[] data, int offset, int baseOffset, int opcode) {
        if (offset + 6 > data.length) {
            return null;
        }
        ScriptCommand command = new ScriptCommand();
        command.offset = baseOffset + offset;
        command.opcode = opcode;
        command.name = "Member_calc";
        command.size = 6;
        command.parameters = new HashMap<>();
        command.parameters.put("work", data[offset + 1] & 0xFF);
        command.parameters.put("member", readBigEndianShort(data, offset + 2));
        command.parameters.put("value", readBigEndianShort(data, offset + 4));
        return command;
    }

    private ScriptCommand parseDoorMemberCalc2Command(byte[] data, int offset, int baseOffset) {
        if (offset + 4 > data.length) {
            return null;
        }
        ScriptCommand command = new ScriptCommand();
        command.offset = baseOffset + offset;
        command.opcode = 0x57;
        command.name = "Member_calc2";
        command.size = 4;
        command.parameters = new HashMap<>();
        command.parameters.put("param0", data[offset + 1] & 0xFF);
        command.parameters.put("param1", data[offset + 2] & 0xFF);
        command.parameters.put("param2", data[offset + 3] & 0xFF);
        return command;
    }

    private String generateCCode(List<ScriptCommand> commands, String fileName) {
        StringBuilder c = new StringBuilder();
        c.append("int ").append(fileName).append("(void) {\n\n");

        int indent = 1;
        Stack<Integer> ifBlocks = new Stack<>();
        Stack<Integer> conditionalIfBlocks = new Stack<>();
        Stack<Integer> loopBlocks = new Stack<>();
        Stack<Integer> doWhileBlocks = new Stack<>();
        Stack<Integer> whileBlocks = new Stack<>();
        Stack<Integer> switchBlocks = new Stack<>();
        Deque<ConditionalContext> conditionalStack = new ArrayDeque<>();
        PendingWhileContext pendingWhileContext = null;
        PendingDoWhileContext pendingDoWhileContext = null;
        Map<Integer, Integer> scaFlagState = new HashMap<>();
        Map<Integer, Integer> knownVarValues = new HashMap<>();
        int lastForCount = 0;
        int lastForBlockLength = 0;
        boolean lastForWasFor2 = false;
        int lastPlcDestZ = 0;

        for (ScriptCommand command : commands) {
            String commandName = command.name;

            if ("Next".equals(commandName)) {
                if (!loopBlocks.isEmpty()) {
                    indent = loopBlocks.pop();
                } else if (indent > 1) {
                    indent--;
                }
                c.append("\t".repeat(Math.max(0, indent))).append("}\n");
                continue;
            }

            ConditionalContext pendingIfContext = null;
            boolean commandStartsConditional =
                    "Ck".equals(commandName) ||
                            "Cmp".equals(commandName) ||
                            "Member_cmp".equals(commandName) ||
                            "Sce_key_ck".equals(commandName);
            if (commandStartsConditional && !conditionalStack.isEmpty() && conditionalStack.peek().awaitingCondition) {
                pendingIfContext = conditionalStack.peek();
            }

            int effectiveIndent = indent;
            if (pendingIfContext != null) {
                effectiveIndent = pendingIfContext.indent;
            } else if (("Else_ck".equals(commandName) || "Endif".equals(commandName)) && !ifBlocks.isEmpty()) {
                effectiveIndent = ifBlocks.peek();
            }

            boolean skipIndent =
                    "Ifel_ck".equals(commandName) ||
                            "Next".equals(commandName) ||
                            "Sleeping".equals(commandName) ||
                            "Switch".equals(commandName) ||
                            "Case".equals(commandName) ||
                            "Default".equals(commandName) ||
                            "Eswitch".equals(commandName) ||
                            "Break".equals(commandName) ||
                            "Break_point".equals(commandName) ||
                            "Edwhile".equals(commandName) ||
                            "Ewhile".equals(commandName) ||
                            "Work_copy".equals(commandName) ||
                            ("Ck".equals(commandName) && (pendingWhileContext != null || pendingDoWhileContext != null)) ||
                            ("Cmp".equals(commandName) && (pendingWhileContext != null || pendingDoWhileContext != null));
            if (!skipIndent) {
                c.append("\t".repeat(Math.max(0, effectiveIndent)));
            }

            switch (commandName) {
                case "Ifel_ck":
                    ConditionalContext ctx = new ConditionalContext();
                    ctx.indent = indent;
                    conditionalStack.push(ctx);
                    break;

                case "Else_ck":
                    Integer elseIndentRef = !conditionalIfBlocks.isEmpty()
                            ? conditionalIfBlocks.peek()
                            : (!ifBlocks.isEmpty() ? ifBlocks.peek() : null);
                    if (elseIndentRef != null) {
                        c.append("} else {\n");
                        indent = elseIndentRef + 1;
                    } else {
                        c.append("// else ohne zugehoeriges if\n");
                    }
                    break;

                case "Endif":
                    if (!ifBlocks.isEmpty()) {
                        int ifIndent = ifBlocks.pop();
                        c.append("}\n");
                        indent = ifIndent;
                        if (!conditionalIfBlocks.isEmpty() && conditionalIfBlocks.peek().equals(ifIndent)) {
                            conditionalIfBlocks.pop();
                            if (!conditionalStack.isEmpty()) {
                                conditionalStack.pop();
                            }
                        }
                    } else {
                        c.append("// stray Endif\n");
                    }
                    break;

                case "nop":
                    c.append("nop();\n");
                    break;

                case "Evt_end":
                    int retVal = command.parameters.containsKey("value") ? (Integer) command.parameters.get("value") : 0;
                    c.append("return ").append(retVal).append(";\n");
                    break;


                case "Evt_next":
                    c.append("Evt_next();\n");
                    break;

                case "Evt_chain":
                    c.append("Evt_chain(")
                            .append(command.parameters.get("arg1")).append(", ")
                            .append(command.parameters.get("arg2")).append(", ")
                            .append(command.parameters.get("arg3")).append(");\n");
                    break;

                case "Evt_exec": {
                    int evtCondition = (Integer) command.parameters.getOrDefault("condition", 0);
                    int evtValue = (Integer) command.parameters.getOrDefault("value", 0);
                    c.append("Evt_exec(")
                            .append(evtCondition).append(", ")
                            .append(formatUShortHexLiteral(evtValue)).append(");\n");
                    break;
                }

                case "Evt_kill":
                    c.append("Evt_kill(")
                            .append(command.parameters.get("event")).append(");\n");
                    break;

                case "While": {
                    PendingWhileContext whileCtx = new PendingWhileContext();
                    whileCtx.indent = effectiveIndent;
                    int blockLength = 0;
                    if (command.parameters.containsKey("block_length")) {
                        blockLength = (Integer) command.parameters.get("block_length");
                    }
                    whileCtx.blockLength = blockLength;
                    pendingWhileContext = whileCtx;
                    break;
                }

                case "Sleep":
                    c.append("Sleep(").append(command.parameters.get("duration")).append(");\n");
                    break;
                case "Sleeping":
                    break;


                case "Wsleep":
                    c.append("Wsleep();\n");
                    break;

                case "Wsleeping":
                    c.append("Wsleeping();\n");
                    break;

                case "Work_set":
                    int workValue = (Integer) command.parameters.getOrDefault("value", 0);
                    c.append("Work_set(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("work_no", 0))).append(", ")
                            .append(formatByteHex(workValue)).append(");\n");
                    break;

                case "Plc_ret":
                    c.append("Plc_ret();\n");
                    break;

                case "Gosub":
                    int gosubEvent = (Integer) command.parameters.getOrDefault("event", 0);
                    c.append("goto Sub")
                            .append(String.format("%02d", gosubEvent))
                            .append("();\n");
                    break;

                case "Goto":
                    /* AO2-round 2026-05-26: Goto's "target" is a signed
                     * relative jump (LE int16 from pc[4..5]). Print as signed
                     * decimal so the C-output reflects the actual semantic
                     * (e.g. `goto(-2)` for sub04 rotor-spin loop) instead of
                     * the misleading BE-u24 `goto(0x0000FEFF)` we used to emit. */
                    int gotoTarget = (Integer) command.parameters.getOrDefault("target", 0);
                    c.append("goto(").append(gotoTarget).append(");\n");
                    break;

                case "Return": {
                    int returnTarget = (Integer) command.parameters.getOrDefault("dummy", 0);
                    c.append("return Sub")
                            .append(String.format("%02d", returnTarget))
                            .append("();\n");
                    break;
                }


                case "Pos_set": {
                    int posReg = command.parameters.containsKey("register") ? (Integer) command.parameters.get("register") : 0;
                    c.append("Pos_set(")
                            .append(posReg).append(", ")
                            .append(command.parameters.get("x")).append(", ")
                            .append(command.parameters.get("y")).append(", ")
                            .append(command.parameters.get("z")).append(");\n");
                    break;
                }

                case "Dir_set": {
                    int dirReg = command.parameters.containsKey("register") ? (Integer) command.parameters.get("register") : 0;
                    c.append("Dir_set(")
                            .append(dirReg).append(", ")
                            .append(command.parameters.get("x")).append(", ")
                            .append(command.parameters.get("y")).append(", ")
                            .append(command.parameters.get("z")).append(");\n");
                    break;
                }



                case "Se_on":
                    c.append("Se_on(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("bank", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("id", 0))).append(", ")
                            .append(command.parameters.getOrDefault("volume", 0)).append(", ")
                            .append(command.parameters.getOrDefault("pan", 0)).append(", ")
                            .append(command.parameters.getOrDefault("unknown", 0)).append(", ")
                            .append(command.parameters.getOrDefault("x", 0)).append(", ")
                            .append(command.parameters.getOrDefault("y", 0)).append(", ")
                            .append(command.parameters.getOrDefault("z", 0)).append(");\n");
                    break;
                case "Switch":
                    int switchIndent = effectiveIndent;
                    c.append("\t".repeat(Math.max(0, switchIndent)))
                            .append("switch(")
                            .append(command.parameters.getOrDefault("var", 0))
                            .append(") {\n");
                    switchBlocks.push(switchIndent);
                    indent = switchIndent + 1;
                    break;
                case "Case":
                    int caseIndent = switchBlocks.isEmpty() ? effectiveIndent : switchBlocks.peek();
                    c.append("\t".repeat(Math.max(0, caseIndent)))
                            .append("case ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("value", 0))).append(":\n");
                    indent = caseIndent + 1;
                    break;
                case "Default":
                    int defaultIndent = switchBlocks.isEmpty() ? effectiveIndent : switchBlocks.peek();
                    c.append("\t".repeat(Math.max(0, defaultIndent))).append("default:\n");
                    indent = defaultIndent + 1;
                    break;
                case "Eswitch":
                    if (!switchBlocks.isEmpty()) {
                        indent = switchBlocks.pop();
                        c.append("\t".repeat(Math.max(0, indent))).append("}\n");
                    }
                    break;
                case "Sca_id_set": {
                    // [op, region, index, value] -> writes value to SCA entry's u0 flag
                    // byte (+0x9). The two operand bytes are SEPARATE; the old renderer
                    // fused them into one BE short ("0x0A00"), hiding index/value.
                    int scaRegion = (Integer) command.parameters.getOrDefault("region", 0);
                    int scaIndex  = (Integer) command.parameters.getOrDefault("index", 0);
                    int scaValue  = (Integer) command.parameters.getOrDefault("value", 0);
                    c.append("Sca_id_set(")
                            .append(scaRegion).append(", ")
                            .append(scaIndex).append(", ")
                            .append(scaValue).append(");\n");
                    break;
                }
                case "Flr_set":
                    c.append("FlrSet(")
                            .append(command.parameters.getOrDefault("id", 0)).append(", ")
                            .append(formatOnOff((Integer) command.parameters.getOrDefault("flag", 0))).append(");\n");
                    break;
                case "Sca_floor_set": {
                    // [op, region, index, value] -> writes value to SCA entry's floor
                    // byte (+0xb). (Was mislabeled "Dir_ck" with bogus id/angle params.)
                    int sfRegion = (Integer) command.parameters.getOrDefault("region", 0);
                    int sfIndex  = (Integer) command.parameters.getOrDefault("index", 0);
                    int sfValue  = (Integer) command.parameters.getOrDefault("value", 0);
                    c.append("Sca_floor_set(")
                            .append(sfRegion).append(", ")
                            .append(sfIndex).append(", ")
                            .append(sfValue).append(");\n");
                    break;
                }
                case "Kage_set":
                    c.append("KageSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b2", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b3", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b4", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("s0", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("s1", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("s2", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("s3", 0))).append(");\n");
                    break;
                case "Cut_be_set":
                    c.append("CutBeSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value2", 0))).append(");\n");
                    break;
                case "Aot_set_4p":
                    List<String> aotArgs = new ArrayList<>();
                    aotArgs.add(formatByteHex((Integer) command.parameters.getOrDefault("id", 0)));
                    @SuppressWarnings("unchecked")
                    List<Integer> aotUnknown = (List<Integer>) command.parameters.getOrDefault("unknown", Collections.emptyList());
                    aotUnknown.forEach(v -> aotArgs.add(formatUShortHex(v)));
                    @SuppressWarnings("unchecked")
                    List<Integer> aotCoords = (List<Integer>) command.parameters.getOrDefault("coords", Collections.emptyList());
                    aotCoords.forEach(v -> aotArgs.add(formatUShortHex(v)));
                    @SuppressWarnings("unchecked")
                    List<Integer> aotTail = (List<Integer>) command.parameters.getOrDefault("tail", Collections.emptyList());
                    aotTail.forEach(v -> aotArgs.add(formatUShortHex(v)));
                    c.append("AotSet4p(").append(String.join(", ", aotArgs)).append(");\n");
                    break;
                case "Door_aot_set_4p":
                    List<String> doorArgs = new ArrayList<>();
                    doorArgs.add(formatByteHex((Integer) command.parameters.getOrDefault("id", 0)));
                    @SuppressWarnings("unchecked")
                    List<Integer> doorData = (List<Integer>) command.parameters.getOrDefault("data", Collections.emptyList());
                    doorData.forEach(v -> doorArgs.add(formatUShortHex(v)));
                    c.append("DoorSet4p(").append(String.join(", ", doorArgs)).append(");\n");
                    break;
                case "Obj_model_move":
                    @SuppressWarnings("unchecked")
                    List<Integer> moveArgs = (List<Integer>) command.parameters.getOrDefault("args", Collections.emptyList());
                    List<String> formattedArgs = moveArgs.stream()
                            .map(String::valueOf)
                            .collect(Collectors.toList());
                    c.append("Obj_model_move(").append(String.join(", ", formattedArgs)).append(");\n");
                    break;
                case "Super_set":
                    @SuppressWarnings("unchecked")
                    List<Integer> superPayload = (List<Integer>) command.parameters.get("payload");
                    appendBytePayload(c, "SuperSet", superPayload);
                    break;
                case "Item_aot_set_4p":
                    List<String> itemArgs = new ArrayList<>();
                    itemArgs.add(formatByteHex((Integer) command.parameters.getOrDefault("id", 0)));
                    @SuppressWarnings("unchecked")
                    List<Integer> itemData = (List<Integer>) command.parameters.getOrDefault("data", Collections.emptyList());
                    itemData.forEach(v -> itemArgs.add(formatUShortHex(v)));
                    c.append("ItemAotSet4p(").append(String.join(", ", itemArgs)).append(");\n");
                    break;
                case "Op4D":
                    appendBytePayload(c, "Op4D", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Obj_model_set":
                    StringBuilder objArgs = new StringBuilder();
                    objArgs.append(formatByteHexLiteral((Integer) command.parameters.getOrDefault("obj_id", 0)));
                    @SuppressWarnings("unchecked")
                    List<Integer> objHeader = (List<Integer>) command.parameters.getOrDefault("header", Collections.emptyList());
                    for (int i = 0; i < OBJ_MODEL_HEADER_SIZE; i++) {
                        int value = i < objHeader.size() ? objHeader.get(i) : 0;
                        objArgs.append(", ").append(value);
                    }
                    @SuppressWarnings("unchecked")
                    List<Integer> objPayload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    for (int i = 0; i < OBJ_MODEL_MAX_WORDS; i++) {
                        int value = i < objPayload.size() ? objPayload.get(i) : 0;
                        objArgs.append(", ").append(value);
                    }
                    c.append("Obj_model_set(").append(objArgs).append(");\n");
                    break;
                case "Cut_auto":
                    c.append("Cut_auto(")
                            .append(command.parameters.getOrDefault("state", 0)).append(");\n");
                    break;
                case "Member_copy":
                    c.append("Member_copy(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("var", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("member", 0))).append(");\n");
                    break;
                case "Member_calc":
                    c.append("Member_calc(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("work", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("member", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("value", 0))).append(");\n");
                    break;
                case "Member_calc2":
                    c.append("Member_calc2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param2", 0))).append(");\n");
                    break;
                case "Member_set":
                    c.append("Member_set(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("member", 0))).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;
                case "Member_set2":
                    c.append("Member_set2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("member", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("var", 0))).append(");\n");
                    break;
                case "Member_cmp": {
                    int cmpIndent = (pendingIfContext != null) ? pendingIfContext.indent : effectiveIndent;
                    c.append("if(Member_cmp(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("var", 0))).append(" ")
                            .append(formatComparison((Integer) command.parameters.getOrDefault("compare", 0))).append(" ")
                            .append(command.parameters.getOrDefault("value", 0)).append(")) {\n");
                    ifBlocks.push(cmpIndent);
                    indent = cmpIndent + 1;
                    if (pendingIfContext != null) {
                        pendingIfContext.awaitingCondition = false;
                    }
                    break;
                }
                case "Cmp": {
                    int cmpTargetType = (Integer) command.parameters.getOrDefault("targetType", 0);
                    String cmpComment = null;
                    if (cmpTargetType != 0) {
                        cmpComment = "// Cmp targetType " + formatByteHex(cmpTargetType);
                    }
                    String cmpExpression = buildCmpExpression(command);

                    if (pendingDoWhileContext != null) {
                        int loopIndent = pendingDoWhileContext.indent;
                        String indentStr = "\t".repeat(Math.max(0, loopIndent));
                        if (cmpComment != null) {
                            c.append(indentStr).append(cmpComment).append("\n");
                        }
                        c.append(indentStr)
                                .append("} while(")
                                .append(cmpExpression)
                                .append(");\n");
                        pendingDoWhileContext = null;
                        break;
                    }

                    if (pendingWhileContext != null) {
                        int loopIndent = pendingWhileContext.indent;
                        String indentStr = "\t".repeat(Math.max(0, loopIndent));
                        if (cmpComment != null) {
                            c.append(indentStr).append(cmpComment).append("\n");
                        }
                        c.append(indentStr)
                                .append("while(")
                                .append(cmpExpression)
                                .append(") {\n");
                        whileBlocks.push(loopIndent);
                        indent = loopIndent + 1;
                        pendingWhileContext = null;
                        break;
                    }

                    int cmpIndent = (pendingIfContext != null) ? pendingIfContext.indent : effectiveIndent;
                    String indentStr = "\t".repeat(Math.max(0, cmpIndent));
                    if (cmpComment != null) {
                        c.append(indentStr).append(cmpComment).append("\n");
                    }
                    if (pendingIfContext != null) {
                        c.append(indentStr)
                                .append("if(")
                                .append(cmpExpression)
                                .append(") {\n");
                        ifBlocks.push(cmpIndent);
                        indent = cmpIndent + 1;
                        pendingIfContext.awaitingCondition = false;
                    } else {
                        c.append(indentStr)
                                .append(cmpExpression)
                                .append(";\n");
                    }
                    break;
                }
                case "Sce_rnd":
                    c.append("SceRnd();\n");
                    break;
                case "Cut_chg":
                    c.append("Cut_chg(")
                            .append(formatByteHexLiteral((Integer) command.parameters.getOrDefault("camera", 0))).append(");\n");
                    break;
                case "Cut_old":
                    c.append("Cut_old();\n");
                    break;
                case "Message_on": {
                    int messageIndex = (Integer) command.parameters.getOrDefault("messageIndex", 0);
                    /* Show arg2/arg3 as SEPARATE bytes (the engine reads them separately):
                     * arg2 bit 0x80 = YES/NO query prompt (e.g. ROOM1130 switch arg2=0x80).
                     * The old combined BE messageFlags (e.g. 33023=0x80FF) hid that bit. */
                    int messageArg2 = (Integer) command.parameters.getOrDefault("messageArg2", 0);
                    int messageArg3 = (Integer) command.parameters.getOrDefault("messageArg3", 0);
                    int messageBaseOffset = (Integer) command.parameters.getOrDefault("messageBaseOffset", -1);
                    c.append("Message_on(")
                            .append(messageIndex).append(", ")
                            .append(formatByteHex(messageArg2)).append(", ")
                            .append(formatByteHex(messageArg3)).append(");");
                    String msgComment = formatMessageComment(messageIndex, messageBaseOffset);
                    if (!msgComment.isEmpty()) {
                        c.append(" // ").append(msgComment);
                    }
                    c.append("\n");
                    break;
                }
                case "Sce_key_ck": {
                    int keyIndent = (pendingIfContext != null) ? pendingIfContext.indent : effectiveIndent;
                    c.append("if(Sce_key_ck(")
                            .append(command.parameters.getOrDefault("state", 0)).append(", ")
                            .append(command.parameters.getOrDefault("trigger", 0)).append(");) {\n");
                    ifBlocks.push(keyIndent);
                    indent = keyIndent + 1;
                    if (pendingIfContext != null) {
                        pendingIfContext.awaitingCondition = false;
                    }
                    break;
                }

                case "Do":
                    c.append("do {\n");
                    doWhileBlocks.push(effectiveIndent);
                    indent = effectiveIndent + 1;
                    break;

                case "Edwhile": {
                    int loopIndent = !doWhileBlocks.isEmpty() ? doWhileBlocks.pop() : Math.max(0, indent - 1);
                    indent = loopIndent;
                    PendingDoWhileContext doCtx = new PendingDoWhileContext();
                    doCtx.indent = loopIndent;
                    doCtx.conditionLength = (Integer) command.parameters.getOrDefault("condition_length", 0);
                    pendingDoWhileContext = doCtx;
                    break;
                }

                case "Ewhile": {
                    int loopIndent = !whileBlocks.isEmpty() ? whileBlocks.pop() : Math.max(0, indent - 1);
                    indent = loopIndent;
                    c.append("\t".repeat(Math.max(0, loopIndent))).append("}\n");
                    break;
                }


                case "Ck":
                    if (pendingDoWhileContext != null) {
                        int loopIndent = pendingDoWhileContext.indent;
                        indent = loopIndent;
                        c.append("\t".repeat(Math.max(0, loopIndent)))
                                .append("} while(Ck(")
                                .append(command.ckFlagId).append(", ")
                                .append(formatByteHex(command.ckBit)).append(", ")
                                .append(command.ckValue & 0x01).append("));\n");
                        pendingDoWhileContext = null;
                        break;
                    }
                    if (pendingWhileContext != null) {
                        int loopIndent = pendingWhileContext.indent;
                        String lineIndent = "\t".repeat(Math.max(0, loopIndent));
                        c.append(lineIndent)
                                .append("while(Ck(")
                                .append(command.ckFlagId).append(", ")
                                .append(formatByteHex(command.ckBit)).append(", ")
                                .append(command.ckValue & 0x01).append(")) {\n");
                        whileBlocks.push(loopIndent);
                        indent = loopIndent + 1;
                        pendingWhileContext = null;
                        break;
                    }
                    if (pendingIfContext != null) {
                        int condIndent = pendingIfContext.indent;
                        c.append("if(Ck(")
                                .append(command.ckFlagId).append(", ")
                                .append(formatByteHex(command.ckBit)).append(", ")
                                .append(command.ckValue & 0x01).append(")) {\n");
                        ifBlocks.push(condIndent);
                        conditionalIfBlocks.push(condIndent);
                        pendingIfContext.awaitingCondition = false;
                        indent = condIndent + 1;
                    } else {
                        c.append("Ck(")
                                .append(command.ckFlagId).append(", ")
                                .append(formatByteHex(command.ckBit)).append(", ")
                                .append(command.ckValue & 0x01).append(");\n");
                    }
                    break;

                case "For":
                    int loopCount = (Integer) command.parameters.getOrDefault("count", 0);
                    lastForCount = loopCount;
                    lastForBlockLength = (Integer) command.parameters.getOrDefault("block_length", 0);
                    lastForWasFor2 = false;
                    c.append("for(n = 0; n < ")
                            .append(loopCount)
                            .append("; n++) {\n");
                    loopBlocks.push(effectiveIndent);
                    indent = effectiveIndent + 1;
                    break;
                case "For2":
                    int loopCount2 = (Integer) command.parameters.getOrDefault("count", 0);
                    lastForCount = loopCount2;
                    lastForBlockLength = (Integer) command.parameters.getOrDefault("block_length", 0);
                    lastForWasFor2 = true;
                    c.append("for(n = 0; n < ")
                            .append(loopCount2)
                            .append("; n++) {\n");
                    loopBlocks.push(effectiveIndent);
                    indent = effectiveIndent + 1;
                    break;

                case "Next":
                    int loopIndent = !loopBlocks.isEmpty() ? loopBlocks.pop() : Math.max(0, indent - 1);
                    indent = loopIndent;
                    c.append("\t".repeat(Math.max(0, indent))).append("}\n");
                    break;

                case "Set":
                    c.append("Set(")
                            .append(command.parameters.get("flag_id")).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("bit"))).append(", ")
                            .append(formatSetMode((Integer) command.parameters.getOrDefault("mode", 0))).append(");\n");
                    break;
                case "Rbj_reset":
                    c.append("RbjReset();\n");
                    break;
                case "Sce_scr_move":
                    c.append("ScrMove(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("dummy", 0))).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;
                case "Parts_set":
                    c.append("PartsSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("dummy", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param1", 0))).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;
                case "Movie_on":
                    c.append("MovieOn(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("id", 0))).append(");\n");
                    break;
                case "Splc_ret":
                    c.append("SplcRet();\n");
                    break;
                case "Splc_sce":
                    c.append("SplcSce();\n");
                    break;
                case "Super_on":
                    appendBytePayload(c, "SuperOn", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Mirror_set":
                    appendBytePayload(c, "MirrorSet", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_fade_adjust":
                    appendBytePayload(c, "FadeAdjust", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_espr3d_on2":
                    appendBytePayload(c, "SceEspr3dOn2", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_item_get":
                    c.append("ItemGet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("item", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("amount", 0))).append(");\n");
                    break;
                case "Sce_line_start":
                    appendBytePayload(c, "LineStart", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_line_main":
                    appendBytePayload(c, "LineMain", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_line_end":
                    c.append("LineEnd();\n");
                    break;
                case "Sce_parts_bomb":
                    appendBytePayload(c, "PartsBomb", (List<Integer>) command.parameters.get("payload"));
                    break;
                case "Sce_parts_down":
                    appendBytePayload(c, "PartsDown", (List<Integer>) command.parameters.get("payload"));
                    break;

                case "Save":
                    int saveTarget = (Integer) command.parameters.get("target");
                    int saveValue = (Integer) command.parameters.get("value");
                    c.append("Save(")
                            .append(formatByteHex(saveTarget)).append(", ")
                            .append(saveValue).append(");\n");
                    knownVarValues.put(saveTarget & 0xFF, saveValue);
                    break;

                case "Copy":
                    c.append("Copy(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("dst", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("src", 0)))
                            .append(");\n");
                    break;

                case "Calc": {
                    int calcOperation = (Integer) command.parameters.getOrDefault("operation", 0);
                    int targetVar = (Integer) command.parameters.getOrDefault("var", 0);
                    int operand = (Integer) command.parameters.getOrDefault("value", 0);
                    knownVarValues.remove(targetVar & 0xFF);
                    c.append("Calc(")
                            .append(formatCalcExpression(targetVar, calcOperation, operand, false, null, false))
                            .append(");\n");
                    break;
                }

                case "Calc2": {
                    int calcOperation = (Integer) command.parameters.getOrDefault("operation", 0);
                    int targetVar = (Integer) command.parameters.getOrDefault("var", 0);
                    int operandVar = (Integer) command.parameters.getOrDefault("sourceVar", 0);
                    knownVarValues.remove(targetVar & 0xFF);
                    String operandString = null;
                    if (knownVarValues.containsKey(operandVar & 0xFF)) {
                        operandString = Integer.toString(knownVarValues.get(operandVar & 0xFF));
                    }
                    c.append("Calc2(")
                            .append(formatCalcExpression(targetVar, calcOperation, operandVar, true, operandString, true))
                            .append(");\n");
                    break;
                }


                case "Speed_set":
                    c.append("Speed_set(")
                            .append(command.parameters.get("axis")).append(", ")
                            .append(command.parameters.get("speed")).append(");\n");
                    break;

                case "Add_speed":
                    c.append("Add_speed();\n");
                    break;
                case "Add_aspeed":
                    c.append("Add_aspeed();\n");
                    break;

                case "Sce_espr_on":
                    c.append("EsprOn(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg2", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg3", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg4", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("arg5", 0))).append(", ")
                            .append(command.parameters.getOrDefault("x", 0)).append(", ")
                            .append(command.parameters.getOrDefault("y", 0)).append(", ")
                            .append(command.parameters.getOrDefault("z", 0)).append(", ")
                            .append(command.parameters.getOrDefault("w", 0)).append(");\n");
                    break;


                case "Plc_motion":
                    c.append("Plc_motion(")
                            .append(command.parameters.get("arg1")).append(", ")
                            .append(command.parameters.get("arg2")).append(", ")
                            .append(command.parameters.get("arg3")).append(");\n");
                    break;
                case "Plc_dest": {
                    lastPlcDestZ = (Integer) command.parameters.getOrDefault("z", 0);
                    int plcMode = (Integer) command.parameters.getOrDefault("mode", 0);
                    /* mode → distinct motion via PSX Plc_dest handler FUN_80041BE4
                     * per-mode tables (WALK 0x30 / RUN 0x48 / TURN 0x60). */
                    String plcModeName;
                    switch (plcMode) {
                        case 0x04: plcModeName = "WALK"; break;
                        case 0x05: plcModeName = "RUN";  break;
                        case 0x06: plcModeName = "STOP"; break;
                        case 0x09: plcModeName = "TURN"; break;
                        default:   plcModeName = "mode" + plcMode; break;
                    }
                    c.append("Plc_dest(")
                            .append(command.parameters.getOrDefault("register", 0)).append(", ")
                            .append(plcMode).append(" /*").append(plcModeName).append("*/, ")
                            .append(command.parameters.getOrDefault("flag", 0)).append(", ")
                            .append(command.parameters.getOrDefault("x", 0)).append(", ")
                            .append(lastPlcDestZ).append(");\n");
                    break;
                }

                case "Plc_neck":
                    c.append("Plc_neck(")
                            .append(command.parameters.getOrDefault("mode", 0)).append(", ")
                            .append(command.parameters.getOrDefault("yaw", 0)).append(", ")
                            .append(command.parameters.getOrDefault("pitch", 0)).append(", ")
                            .append(command.parameters.getOrDefault("roll", 0)).append(", ")
                            .append(command.parameters.getOrDefault("target", 0)).append(", ")
                            .append(command.parameters.getOrDefault("speed", 0)).append(");\n");
                    break;

                case "Plc_flg":
                    c.append("Plc_flg(")
                            .append(command.parameters.getOrDefault("entity", 0)).append(", ")
                            .append(command.parameters.getOrDefault("flag", 0)).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;

                case "Aot_set":
                    c.append("Aot_set(")
                            .append(command.parameters.get("id")).append(", ")
                            .append(command.parameters.get("type")).append(", ")
                            .append(command.parameters.get("floor")).append(", ")
                            .append(command.parameters.get("chain")).append(", ")
                            .append(command.parameters.get("switchFlag")).append(", ")
                            .append(command.parameters.get("x")).append(", ")
                            .append(command.parameters.get("z")).append(", ")
                            .append(command.parameters.get("w")).append(", ")
                            .append(command.parameters.get("h")).append(", ")
                            .append(command.parameters.get("act")).append(", ")
                            .append(command.parameters.get("priority")).append(", ")
                            .append(command.parameters.get("eventKey")).append(", ")
                            /* 2026-06-13: also emit eventId (pc[17]) + eventState — the
                             * eventId is the sub_scd[] the AOT runs on action (e.g. the
                             * ROOM1130 switch's eventId=2 → sub02). Was omitted, hiding
                             * the field that actually drives event-AOTs/switches. */
                            .append(command.parameters.get("eventId")).append(", ")
                            .append(command.parameters.get("eventState")).append(");\n");
                    break;

                case "Door_aot_set":
                    c.append("Door_aot_set(")
                            .append(command.parameters.get("id")).append(", ")
                            .append(command.parameters.get("aot")).append(", ")
                            .append(command.parameters.get("floor")).append(", ")
                            .append(command.parameters.get("chain")).append(", ")
                            .append(command.parameters.get("switchFlag")).append(", ")
                            .append(command.parameters.get("x")).append(", ")
                            .append(command.parameters.get("z")).append(", ")
                            .append(command.parameters.get("w")).append(", ")
                            .append(command.parameters.get("h")).append(", ")
                            .append(command.parameters.get("next_x")).append(", ")
                            .append(command.parameters.get("next_y")).append(", ")
                            .append(command.parameters.get("next_z")).append(", ")
                            .append(command.parameters.get("next_dir")).append(", ")
                            .append(command.parameters.get("stage")).append(", ")
                            .append(command.parameters.get("room")).append(", ")
                            .append(command.parameters.get("camera")).append(", ")
                            .append(command.parameters.get("texture")).append(", ")
                            .append(command.parameters.get("door_type")).append(", ")
                            .append(command.parameters.get("door_lock")).append(", ")
                            .append(command.parameters.get("unknown2")).append(", ")
                            .append(command.parameters.get("door_locked")).append(", ")
                            .append(command.parameters.get("door_key")).append(", ")
                            .append(command.parameters.get("unknown3")).append(");\n");
                    break;

                case "Sce_em_set":
                    int enemyId = (Integer) command.parameters.getOrDefault("enemy_id", 0);
                    c.append("Sce_em_set(")
                            .append(command.parameters.get("state")).append(", ")
                            .append(formatByteHexLiteral(enemyId)).append(", ")
                            .append(command.parameters.get("script_id")).append(", ")
                            .append(command.parameters.get("sound_bank")).append(", ")
                            .append(command.parameters.get("texture")).append(", ")
                            .append(command.parameters.get("kill_flag")).append(", ")
                            .append(command.parameters.get("spawn_flag")).append(", ")
                            .append(command.parameters.get("x")).append(", ")
                            .append(command.parameters.get("y")).append(", ")
                            .append(command.parameters.get("z")).append(", ")
                            .append(command.parameters.get("dir")).append(", ")
                            .append(command.parameters.get("param0")).append(", ")
                            .append(command.parameters.get("param1")).append(");\n");
                    break;
                case "Break":
                case "Break_point":
                    int breakIndent = indent;
                    if (!switchBlocks.isEmpty() && indent > switchBlocks.peek()) {
                        breakIndent = switchBlocks.peek();
                    }
                    c.append("\t".repeat(Math.max(0, breakIndent))).append("break;\n");
                    break;
                case "Col_chg_set":
                    c.append("Col_chg_set(")
                            .append(command.parameters.getOrDefault("fromId", 0)).append(", ")
                            .append(command.parameters.getOrDefault("fromFlag", 0)).append(", ")
                            .append(command.parameters.getOrDefault("toId", 0)).append(", ")
                            .append(command.parameters.getOrDefault("toFlag", 0)).append(", ")
                            .append(command.parameters.getOrDefault("mode", 177)).append(");\n");
                    break;
                case "Cut_replace":
                    c.append("Cut_replace(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("from", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("to", 0))).append(");\n");
                    break;
                case "Aot_reset":
                    c.append("Aot_reset(")
                            .append(formatByteHex((Integer) command.parameters.get("id"))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("type"))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("floor"))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("value0", 0))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.getOrDefault("value1", 0))).append(", ")
                            .append(formatHexCompact((Integer) command.parameters.getOrDefault("value2", 0))).append(");\n");
                    break;
                case "Aot_on":
                    c.append("Aot_on(")
                            .append(formatByteHex((Integer) command.parameters.get("id"))).append(");\n");
                    break;
                case "Plc_gun":
                    c.append("Plc_gun(")
                            .append(formatByteHex((Integer) command.parameters.get("mode"))).append(");\n");
                    break;
                case "Sce_espr_kill":
                    c.append("EsprKill(")
                            .append(formatByteHex((Integer) command.parameters.get("id"))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("type"))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("work"))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.get("idx"))).append(");\n");
                    break;
                case "Sce_trg_ck":
                    c.append("TrgCk(")
                            .append(formatOnOff((Integer) command.parameters.get("state"))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.get("trigger"))).append(");\n");
                    break;
                case "Sce_bgm_control":
                    // 0x54: SsSeq sequence control (play/stop/pause/fade a slot).
                    c.append("Sce_bgm_control(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param2", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param3", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param4", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param5", 0))).append(");\n");
                    break;
                // Note: 0x53 is Sce_fade_set, NOT Sce_bgm_control (0x54, handled above)
                case "Sce_bgmtbl_set":
                    // RE1.5 0x57 = 4 bytes (3 operand bytes); name tentative.
                    c.append("Sce_bgmtbl_set(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("dummy", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value0", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value1", 0))).append(");\n");
                    break;
                case "Sce_fade_set":
                    // 0x53: RE1.5 = 3 bytes → 2 operands (disasm LAB_80040e18).
                    c.append("Sce_fade_set(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param1", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param2", 0))).append(");\n");
                    break;
                case "Plc_rot":
                    c.append("PlcRot(")
                            .append(formatByteHex((Integer) command.parameters.get("axis"))).append(", ")
                            .append(command.parameters.get("value")).append(");\n");
                    break;
                case "Xa_on":
                    c.append("XaOn(")
                            .append(formatByteHex((Integer) command.parameters.get("channel"))).append(", ")
                            .append(formatUShortHex((Integer) command.parameters.get("sample"))).append(");\n");
                    break;
                case "Sce_espr_on2":
                    @SuppressWarnings("unchecked")
                    List<Integer> esprPayload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    String esprArgs = esprPayload.stream()
                            .map(this::formatByteHex)
                            .collect(Collectors.joining(", "));
                    c.append("SceEsprOn2(").append(esprArgs).append(");\n");
                    break;
                case "Sce_espr_kill2":
                    c.append("SceEsprKill2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value", 0))).append(");\n");
                    break;
                case "Plc_gun_eff":
                    c.append("PlcGunEff();\n");
                    break;
                case "Plc_stop":
                    c.append("PlcStop();\n");
                    break;
                case "Xa_vol":
                    c.append("XaVol(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value", 0))).append(");\n");
                    break;
                case "Se_vol":
                    c.append("SeVol(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value", 0))).append(");\n");
                    break;
                case "Op81":
                    @SuppressWarnings("unchecked")
                    List<Integer> op81Payload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Op81", op81Payload);
                    break;
                case "Op82":
                    @SuppressWarnings("unchecked")
                    List<Integer> op82Payload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Op82", op82Payload);
                    break;
                case "Op83":
                    c.append("Op83();\n");
                    break;
                case "Op84":
                    @SuppressWarnings("unchecked")
                    List<Integer> op84Payload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Op84", op84Payload);
                    break;
                case "Op85":
                    @SuppressWarnings("unchecked")
                    List<Integer> op85Payload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Op85", op85Payload);
                    break;
                case "Weapon_chg":
                    c.append("WeaponChg(")
                            .append(formatByteHex((Integer) command.parameters.get("item"))).append(");\n");
                    break;
                case "Sce_shake_on":
                    c.append("ShakeOn(")
                            .append(command.parameters.getOrDefault("amplitude", 0)).append(", ")
                            .append(command.parameters.getOrDefault("duration", 0)).append(");\n");
                    break;
                case "Light_pos_set":
                    c.append("LightPosSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("dummy", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("light", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("param", 0))).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;
                case "Light_kido_set":
                    c.append("LightKidoSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("light", 0))).append(", ")
                            .append(command.parameters.getOrDefault("brightness", 0)).append(");\n");
                    break;
                case "Light_pos_set2":
                    c.append("LightPosSet2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("camera", 0))).append(", ")
                            .append(command.parameters.getOrDefault("light", 0)).append(", ")
                            .append(command.parameters.getOrDefault("param", 0)).append(", ")
                            .append(command.parameters.getOrDefault("value", 0)).append(");\n");
                    break;
                case "Light_kido_set2":
                    c.append("LightKidoSet2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("camera", 0))).append(", ")
                            .append(command.parameters.getOrDefault("light", 0)).append(", ")
                            .append(command.parameters.getOrDefault("brightness", 0)).append(");\n");
                    break;
                case "Light_color_set2":
                    c.append("LightColorSet2(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("camera", 0))).append(", ")
                            .append(command.parameters.getOrDefault("light", 0)).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("r", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("g", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b", 0))).append(");\n");
                    break;
                case "Light_color_set":
                    c.append("LightColorSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("light", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("r", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("g", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("b", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("extra", 0))).append(");\n");
                    break;
                case "Mizu_div_set":
                    c.append("MizuDivSet(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("value", 0))).append(");\n");
                    break;
                case "Poison_ck":
                    c.append("PoisonCk();\n");
                    break;
                case "Poison_clr":
                    c.append("PoisonClr();\n");
                    break;
                case "Keep_Item_ck":
                    c.append("KeepItemCk(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("item", 0))).append(");\n");
                    break;
                case "Sce_item_ck_lost":
                    c.append("ItemCkLost(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("item", 0))).append(", ")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("mode", 0))).append(");\n");
                    break;
                case "Op89":
                    c.append("Op89();\n");
                    break;
                case "Nop8A":
                    @SuppressWarnings("unchecked")
                    List<Integer> nop8aPayload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Nop8A", nop8aPayload);
                    break;
                case "Nop8B":
                    @SuppressWarnings("unchecked")
                    List<Integer> nop8bPayload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Nop8B", nop8bPayload);
                    break;
                case "Nop8C":
                    c.append("nop();\n");
                    break;
                case "Op8D":
                    @SuppressWarnings("unchecked")
                    List<Integer> op8dPayload = (List<Integer>) command.parameters.getOrDefault("payload", Collections.emptyList());
                    appendBytePayload(c, "Op8D", op8dPayload);
                    break;
                case "Nop8E":
                    c.append("nop();\n");
                    break;
                case "Sce_item_lost":
                    c.append("ItemLost(")
                            .append(formatByteHex((Integer) command.parameters.getOrDefault("item", 0))).append(");\n");
                    break;
                case "Item_aot_set":
                    int aotId = ((Integer) command.parameters.getOrDefault("aot", 0));
                    int itemId = ((Integer) command.parameters.getOrDefault("item", 0));
                    int flag = ((Integer) command.parameters.getOrDefault("flag", 0));
                    int modelAct = ((Integer) command.parameters.getOrDefault("model_act", 0));
                    c.append("Item_aot_set(")
                            .append(aotId).append(", ")
                            .append(command.parameters.getOrDefault("id", 0)).append(", ")
                            .append(command.parameters.getOrDefault("type", 0)).append(", ")
                            .append(command.parameters.getOrDefault("floor", 0)).append(", ")
                            .append(command.parameters.getOrDefault("super", 0)).append(", ")
                            .append(command.parameters.getOrDefault("x", 0)).append(", ")
                            .append(command.parameters.getOrDefault("z", 0)).append(", ")
                            .append(command.parameters.getOrDefault("w", 0)).append(", ")
                            .append(command.parameters.getOrDefault("h", 0)).append(", ")
                            .append(formatHexCompact(itemId)).append(", ")
                            .append(command.parameters.getOrDefault("amount", 0)).append(", ")
                            .append(flag).append(", ")
                            .append(formatHexCompact(modelAct)).append(");\n");
                    break;

                case "unknown_4D":
                    c.append("// unknown opcode (4D) found at 0x").append(formatHex(command.offset, 8)).append("\n");
                    break;

                case "Work_copy":
                    // Observed in retail scripts as padding between commands; treat as noop.
                    break;

                case "UNKNOWN":
                    c.append("// ").append(command.parameters.get("comment")).append("\n");
                    break;

                default:
                    c.append("// ").append(commandName);
                    if (!command.parameters.isEmpty()) {
                        c.append(" - ");
                        boolean first = true;
                        for (Map.Entry<String, Object> param : command.parameters.entrySet()) {
                            if (!first) c.append(", ");
                            c.append(param.getKey()).append("=").append(param.getValue());
                            first = false;
                        }
                    }
                    c.append("\n");
                    break;
            }
        }

        if (pendingDoWhileContext != null) {
            int loopIndent = pendingDoWhileContext.indent;
            c.append("\t".repeat(Math.max(0, loopIndent)))
                    .append("} while(/* TODO: unresolved do-while condition */);\n");
        }

        if (pendingWhileContext != null) {
            int loopIndent = pendingWhileContext.indent;
            c.append("\t".repeat(Math.max(0, loopIndent)))
                    .append("// TODO: unresolved while condition (block_length=")
                    .append(pendingWhileContext.blockLength)
                    .append(")\n");
        }

        while (!whileBlocks.isEmpty()) {
            indent = whileBlocks.pop();
            c.append("\t".repeat(Math.max(0, indent))).append("}\n");
        }

        while (!loopBlocks.isEmpty()) {
            indent = loopBlocks.pop();
            c.append("\t".repeat(Math.max(0, indent))).append("}\n");
        }

        while (!ifBlocks.isEmpty()) {
            indent = ifBlocks.pop();
            c.append("\t".repeat(Math.max(0, indent))).append("}\n");
        }

        c.append("}");
        return c.toString();
    }

    private String buildCmpExpression(ScriptCommand command) {
        int var = (Integer) command.parameters.getOrDefault("var", 0);
        int compare = (Integer) command.parameters.getOrDefault("compare", 0);
        int value = (Integer) command.parameters.getOrDefault("value", 0);
        return "Cmp(" + formatByteHex(var) + " " + formatComparison(compare) + " " + value + ")";
    }

    private int readShort(byte[] data, int offset) {
        return readBigEndianShort(data, offset);
    }

    private int readDoorShort(byte[] data, int offset, boolean unsigned) {
        if (unsigned) {
            return readBigEndianShort(data, offset);
        }
        return readBigEndianSignedShort(data, offset);
    }

    private int readBigEndianShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        return ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
    }

    private int readBigEndianSignedShort(byte[] data, int offset) {
        int value = readBigEndianShort(data, offset);
        if (value > 32767) {
            value -= 65536;
        }
        return value;
    }

    private int readLittleEndianSignedShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) {
            return 0;
        }
        int value = (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
        if (value > 32767) {
            value -= 65536;
        }
        return value;
    }

    private int readLittleEndianUnsignedShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) {
            return 0;
        }
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private int readLittleEndianInt(byte[] data, int offset) {
        if (offset < 0 || offset + 3 >= data.length) {
            return 0;
        }
        return ((data[offset] & 0xFF))
                | ((data[offset + 1] & 0xFF) << 8)
                | ((data[offset + 2] & 0xFF) << 16)
                | ((data[offset + 3] & 0xFF) << 24);
    }

    private int readSignedShort(byte[] data, int offset) {
        return readBigEndianSignedShort(data, offset);
    }

    /* AK-round note: readLittleEndianSignedShort is the existing LE helper
     * (defined later in the file). Use it for opcodes whose PSX handler
     * reads via `lh`/`lhu` (LE on MIPS3000). Verified for opcode 0x40
     * (Plc_dest) per PSX.EXE 0x80041BE4 and memory
     * bugfix_plc_dest_le_endian_2026_05_21 + bugfix_AK_round_cut2_camera. */

    private int readBigEndianUnsignedShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) {
            return 0;
        }
        return ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
    }

    private int readUnsignedShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) {
            return 0;
        }
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private int readUnsigned24Bit(byte[] data, int offset) {
        if (offset < 0 || offset + 2 >= data.length) {
            return 0;
        }
        return ((data[offset] & 0xFF) << 16)
                | ((data[offset + 1] & 0xFF) << 8)
                | (data[offset + 2] & 0xFF);
    }

    private List<Integer> readByteSequence(byte[] data, int offset, int length) {
        List<Integer> bytes = new ArrayList<>(length);
        for (int i = 0; i < length; i++) {
            int idx = offset + i;
            if (idx >= 0 && idx < data.length) {
                bytes.add(data[idx] & 0xFF);
            } else {
                bytes.add(0);
            }
        }
        return bytes;
    }

    // Opcodes that are unlikely to be valid terminators for Obj_model_set
    // These values often appear as high bytes of signed shorts in payload data
    private static final Set<Integer> OBJ_MODEL_EXCLUDED_TERMINATORS = Set.of(
            0x7C, // Light_color_set - 0x7CFC = 31996 is common
            0x7D, // Light_pos_set2
            0x7E, // Light_kido_set2
            0x7F, // Light_color_set2
            0x83, // Op83
            0x84, // Op84 - 0x8403 = -31741 is common
            0x85, // Op85
            0x86, // Plc_gun_eff
            0x87, // Plc_stop
            0x88, // Plc_rot
            0x89  // Plc_cnt
    );

    private int determineObjModelEnd(byte[] data, int offset) {
        int searchStart = offset + OBJ_MODEL_BASE_SIZE;
        if (searchStart >= data.length) {
            return data.length;
        }
        int pos = searchStart;
        int zeroBytes = 0;
        int dynamicLimit = Math.min(
                data.length,
                offset + OBJ_MODEL_BASE_SIZE + (OBJ_MODEL_DYNAMIC_MAX_WORDS * 2)
        );
        while (pos + 1 < data.length) {
            if (data[pos] == 0 && data[pos + 1] == 0) {
                zeroBytes += 2;
                pos += 2;
                if (zeroBytes >= OBJ_MODEL_TERMINATOR_ZERO_BYTES) {
                    int nextOpcodePos = pos;
                    while (nextOpcodePos < data.length && data[nextOpcodePos] == 0) {
                        nextOpcodePos++;
                    }
                    if (nextOpcodePos >= data.length) {
                        return data.length;
                    }
                    int nextOpcode = data[nextOpcodePos] & 0xFF;
                    // Only terminate on valid opcodes that are likely to follow Obj_model_set
                    // Exclude opcodes that often appear as data values (high bytes of shorts)
                    if (OPCODES.containsKey(nextOpcode) && !OBJ_MODEL_EXCLUDED_TERMINATORS.contains(nextOpcode)) {
                        return nextOpcodePos;
                    }
                    // If we found an excluded opcode, continue scanning but look for next valid opcode
                    // (like another Obj_model_set 0x2D) at word boundaries
                    if (OBJ_MODEL_EXCLUDED_TERMINATORS.contains(nextOpcode)) {
                        return resolveObjModelTerminator(data, offset, nextOpcodePos);
                    }
                }
            } else {
                zeroBytes = 0;
                pos += 2;
            }
            if (pos >= dynamicLimit) {
                return resolveObjModelTerminator(data, offset, pos);
            }
        }
        return resolveObjModelTerminator(data, offset, searchStart);
    }

    // Opcodes that are likely to follow Obj_model_set and can be trusted as terminators
    private static final Set<Integer> OBJ_MODEL_VALID_TERMINATORS = Set.of(
            0x2D, // Obj_model_set - another object model
            0x01, // Evt_end/return
            0x3B, // Door_aot_set
            0x44, // Sce_em_set
            0x2C, // Aot_set
            0x50, // Item_aot_set
            0x0E  // Evt_end (alternate)
    );

    private int findNextValidObjModelTerminator(byte[] data, int baseOffset, int start) {
        // Scan from start looking for a structural opcode that's likely to follow Obj_model_set
        // We can't trust arbitrary opcodes because single-byte values like 0x00 (nop) appear as data
        for (int pos = start; pos < data.length; pos += 2) {
            int opcode = data[pos] & 0xFF;
            if (OBJ_MODEL_VALID_TERMINATORS.contains(opcode)) {
                // For 0x2D (Obj_model_set), validate that the header looks plausible
                // Real headers typically have multiple zero bytes; data values like 0x2D00
                // appearing as "opcodes" will have non-zero garbage in the header position
                if (opcode == 0x2D && pos + 8 <= data.length) {
                    int zeroCount = 0;
                    for (int i = 2; i < 8; i++) {  // Check header bytes (positions 2-7)
                        if (data[pos + i] == 0) zeroCount++;
                    }
                    // Real Obj_model_set headers have at least 3-4 zero bytes
                    if (zeroCount < 3) continue;  // Skip this false positive
                }
                return pos;
            }
        }
        return data.length;
    }

    private int resolveObjModelTerminator(byte[] data, int offset, int start) {
        int alignedStart = Math.max(start, offset + OBJ_MODEL_BASE_SIZE);
        int trusted = findNextValidObjModelTerminator(data, offset, alignedStart);
        if (trusted > offset && trusted < data.length) {
            return trusted;
        }
        int fallback = findNextOpcodeBoundary(data, offset, alignedStart);
        if (fallback > offset && fallback <= data.length) {
            return fallback;
        }
        return data.length;
    }

    private int findNextOpcodeBoundary(byte[] data, int offset, int start) {
        int candidateStart = Math.max(start, offset + OBJ_MODEL_BASE_SIZE);
        for (int candidate = candidateStart; candidate < data.length; candidate++) {
            if (((candidate - offset) & 1) != 0) {
                continue;
            }
            int opcode = data[candidate] & 0xFF;
            OpcodeInfo info = OPCODES.get(opcode);
            if (info == null) {
                continue;
            }
            int requiredSize = info.size;
            if (opcode == 0x2D) {
                requiredSize = OBJ_MODEL_BASE_SIZE;
            }
            if (candidate + requiredSize <= data.length) {
                return candidate;
            }
        }
        return data.length;
    }

    private List<Integer> readBigEndianSignedShortList(byte[] data, int offset, int count) {
        List<Integer> values = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int idx = offset + (i * 2);
            if (idx + 1 >= data.length) {
                break;
            }
            values.add(readBigEndianSignedShort(data, idx));
        }
        return values;
    }

    /* LITTLE-ENDIAN twin: SCD operand words are LE (R3000 `lh`). Used by
     * Obj_model_set (0x2D) — its pos/rot/box operands are little-endian, not BE. */
    private List<Integer> readLittleEndianSignedShortList(byte[] data, int offset, int count) {
        List<Integer> values = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int idx = offset + (i * 2);
            if (idx + 1 >= data.length) {
                break;
            }
            values.add(readLittleEndianSignedShort(data, idx));
        }
        return values;
    }

    private void appendBytePayload(StringBuilder builder, String name, List<Integer> payload) {
        if (payload == null || payload.isEmpty()) {
            builder.append(name).append("();\n");
            return;
        }
        String joined = payload.stream()
                .map(this::formatByteHex)
                .collect(Collectors.joining(", "));
        builder.append(name).append("(").append(joined).append(");\n");
    }

    private String formatHex(int value, int digits) {
        String hex = Integer.toHexString(value).toUpperCase();
        while (hex.length() < digits) {
            hex = "0" + hex;
        }
        return hex;
    }

    private String formatByteHex(int value) {
        return Integer.toString(value & 0xFF);
    }

    private String formatUShortHex(int value) {
        return Integer.toString(value & 0xFFFF);
    }

    private String formatByteHexLiteral(int value) {
        return String.format("0x%02X", value & 0xFF);
    }

    private String formatUShortHexLiteral(int value) {
        return String.format("0x%04X", value & 0xFFFF);
    }

    private String formatHexCompact(int value) {
        int v = value & 0xFFFF;
        if (v <= 0xFF) {
            return String.format("0x%02X", v);
        }
        return String.format("0x%X", v);
    }

    private String formatMessageArgument(int messageIndex, int messageBaseOffset) {
        if (currentRoomContext != null && currentRoomContext != RoomContext.EMPTY) {
            if (messageIndex >= 0) {
                MessageEntry byIndex = currentRoomContext.resolveMessageByIndex(messageIndex);
                if (byIndex != null) {
                    return byIndex.usePointerLiteral
                            ? formatPointerLiteral(byIndex.pointer)
                            : formatUShortHexLiteral(byIndex.literalValue);
                }
            }
            if (messageBaseOffset >= 0) {
                MessageEntry entry = currentRoomContext.resolveMessage(messageBaseOffset);
                if (entry != null) {
                    return entry.usePointerLiteral
                            ? formatPointerLiteral(entry.pointer)
                            : formatUShortHexLiteral(entry.literalValue);
                }
            }
        }
        if (messageBaseOffset >= 0) {
            return formatUShortHexLiteral(messageBaseOffset);
        }
        return formatByteHexLiteral(messageIndex);
    }

    private String formatMessageComment(int messageIndex, int messageBaseOffset) {
        if (currentRoomContext == null || currentRoomContext == RoomContext.EMPTY) {
            return "";
        }
        if (messageIndex >= 0) {
            MessageEntry byIndex = currentRoomContext.resolveMessageByIndex(messageIndex);
            if (byIndex != null && byIndex.fileName != null && !byIndex.fileName.isEmpty()) {
                return byIndex.fileName;
            }
        }
        if (messageBaseOffset >= 0) {
            MessageEntry resolved = currentRoomContext.resolveMessage(messageBaseOffset);
            if (resolved != null && resolved.fileName != null && !resolved.fileName.isEmpty()) {
                return resolved.fileName;
            }
        }
        return "";
    }

    private String formatPointerLiteral(int value) {
        int digits = value <= 0xFFFF ? 4 : 8;
        return "0x" + formatHex(value & 0xFFFFFFFF, digits);
    }

    private String formatOnOff(int value) {
        return (value & 0xFF) == 0 ? "OFF" : "ON";
    }

    private int[] deriveDirParameters(int angle, int lastForCount, int lastPlcDestZ) {
        // If there was a for-loop before, use lastForCount and the last Plc_dest Z coordinate
        if (lastForCount > 0) {
            return new int[] {lastForCount, lastPlcDestZ, 0};
        }
        // Derive based on angle's low byte
        int lowByte = angle & 0xFF;
        if (lowByte == 0x40) {
            return new int[] {0, 512, 16};  // angles ending in 0x40 (e.g., 0x0C40, 0x0440)
        }
        if (lowByte == 0x00) {
            return new int[] {0, 295, 192};  // angles ending in 0x00 (e.g., 0x0600, 0x0200)
        }
        return new int[] {0, 0, 0};
    }

    private String formatComparison(int mode) {
        switch (mode & 0xFF) {
            case 0:
                return "=";
            case 1:
                return ">";
            case 2:
                return ">=";
            case 3:
                return "<";
            case 4:
                return "<=";
            case 5:
                return "!=";
            default:
                return "?";
        }
    }

    private String formatSetMode(int mode) {
        int value = mode & 0xFF;
        switch (value) {
            case 0:
            case 1:
            case 7:
                return Integer.toString(value);
            default:
                return formatByteHex(value);
        }
    }

    private String formatCalcExpression(
            int targetVar,
            int operation,
            int operand,
            boolean operandIsVar,
            String operandStringOverride,
            boolean preferLogicalSymbols
    ) {
        String varString = formatByteHex(targetVar);
        String operator = preferLogicalSymbols
                ? formatCalc2Operator(operation)
                : formatCalcOperator(operation);
        int opFlags = operation & 0xF0;
        if ("~".equals(operator)) {
            String expr = operator + varString;
            if (opFlags != 0) {
                expr += " /* flags 0x" + formatHex(opFlags, 2) + " */";
            }
            return expr;
        }
        String operandString = operandStringOverride != null
                ? operandStringOverride
                : (operandIsVar ? formatByteHex(operand) : Integer.toString(operand));
        if (preferLogicalSymbols) {
            if ("|".equals(operator)) {
                operator = "||";
            } else if ("&".equals(operator)) {
                operator = "&&";
            }
        }
        if ("?".equals(operator)) {
            return varString + " /* op 0x" + formatHex(operation, 2) + " */ " + operandString;
        }
        String expr = varString + " " + operator + " " + operandString;
        if (opFlags != 0) {
            expr += " /* flags 0x" + formatHex(opFlags, 2) + " */";
        }
        return expr;
    }

    private String formatCalcOperator(int op) {
        switch (op & 0x0F) {
            case 0:
                return "+";
            case 1:
                return "-";
            case 2:
                return "*";
            case 3:
                return "/";
            case 4:
                return "%";
            case 5:
                return "||";
            case 6:
                return "&&";
            case 7:
                return "^";
            case 8:
                return "~";
            case 9:
                return "<<";
            case 10:
                return ">>";
            case 11:
                return ">>";
            default:
                return "?";
        }
    }

    public static final class BlockMetadata {
        enum ScriptKind { DEFAULT, DOOR }

        static final BlockMetadata EMPTY = new BlockMetadata(null, null, null, null, null, -1, -1, -1, List.of(), ScriptKind.DEFAULT);

        final String stageName;
        final String roomName;
        final Path roomDir;
        final Path rdtPath;
        final String blockType;
        final int blockIndex;
        final int sectionStart;
        final int blockRelativeOffset;
        final List<Integer> pointerTable;
        final ScriptKind scriptKind;

        private BlockMetadata(String stageName,
                              String roomName,
                              Path roomDir,
                              Path rdtPath,
                              String blockType,
                              int blockIndex,
                              int sectionStart,
                              int blockRelativeOffset,
                              List<Integer> pointerTable,
                              ScriptKind scriptKind) {
            this.stageName = stageName;
            this.roomName = roomName;
            this.roomDir = roomDir;
            this.rdtPath = rdtPath;
            this.blockType = blockType;
            this.blockIndex = blockIndex;
            this.sectionStart = sectionStart;
            this.blockRelativeOffset = blockRelativeOffset;
            this.pointerTable = pointerTable != null ? Collections.unmodifiableList(new ArrayList<>(pointerTable)) : List.of();
            this.scriptKind = scriptKind == null ? ScriptKind.DEFAULT : scriptKind;
        }

        static BlockMetadata forBlock(String stageName,
                                      String roomName,
                                      Path roomDir,
                                      Path rdtPath,
                                      String blockType,
                                      int blockIndex,
                                      int sectionStart,
                                      int blockRelativeOffset,
                                      List<Integer> pointerTable) {
            return new BlockMetadata(stageName, roomName, roomDir, rdtPath, blockType, blockIndex, sectionStart, blockRelativeOffset, pointerTable, ScriptKind.DEFAULT);
        }

        static BlockMetadata forDoorScript() {
            return new BlockMetadata(null, null, null, null, "door", -1, -1, -1, List.of(), ScriptKind.DOOR);
        }

        static BlockMetadata fromExistingLayout(Path scdDir, String baseName) {
            if (scdDir == null) {
                return EMPTY;
            }
            Path roomDir = scdDir.getParent();
            if (roomDir == null) {
                return EMPTY;
            }
            Path stageDir = roomDir.getParent();
            if (stageDir == null) {
                return EMPTY;
            }
            String stageName = stageDir.getFileName().toString();
            String roomName = roomDir.getFileName().toString();
            String blockType = extractPrefix(baseName);
            int blockIndex = extractNumericSuffix(baseName);
            return new BlockMetadata(stageName, roomName, roomDir, null, blockType, blockIndex, -1, -1, List.of(), ScriptKind.DEFAULT);
        }

        boolean hasRoomInfo() {
            return stageName != null && roomDir != null;
        }

        boolean isDoorScript() {
            return scriptKind == ScriptKind.DOOR;
        }

        private static String extractPrefix(String baseName) {
            if (baseName == null || baseName.isBlank()) {
                return "";
            }
            int pos = 0;
            while (pos < baseName.length() && !Character.isDigit(baseName.charAt(pos))) {
                pos++;
            }
            return baseName.substring(0, pos);
        }

        private static int extractNumericSuffix(String baseName) {
            if (baseName == null) {
                return -1;
            }
            int pos = baseName.length() - 1;
            while (pos >= 0 && Character.isDigit(baseName.charAt(pos))) {
                pos--;
            }
            if (pos == baseName.length() - 1) {
                return -1;
            }
            try {
                return Integer.parseInt(baseName.substring(pos + 1));
            } catch (NumberFormatException ex) {
                return -1;
            }
        }
    }

    private String formatCalc2Operator(int op) {
        // Calc2 seems to use the same operation codes, but in observed scripts
        // operation code 0x01 behaves like a bitwise AND instead of subtraction.
        if ((op & 0x0F) == 0x01) {
            return "&";
        }
        return formatCalcOperator(op);
    }
}
