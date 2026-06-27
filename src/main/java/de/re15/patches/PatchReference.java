package de.re15.patches;

// Executable reference — run via: ./gradlew run or java PatchReference

/**
 * RE1.5 Patch System — Java Reference Implementation v1.21
 * =========================================================
 * Executable reference of all MIPS assembly patches in patch_save_final.py.
 * Simulates the PSX memory model and all patch functions with descriptive names.
 *
 * <pre>
 * Memory Map:
 *   0x80010000-0x800BFFFF  PSX.EXE loaded code/data
 *   0x800BF000-0x800C0000  EXE expansion (+8KB for BOX_STORAGE, PLAYER_INV_SAVE)
 *   0x800C0000-0x80100000  DEBUG.BIN overlay (runtime loaded)
 *   0x801F0000-0x801F4FFF  PATCH.BIN (loaded from CD, 10 sectors = 20KB)
 *   0x801F1000+            PATCH2.BIN content (loaded inside combined PATCH.BIN, voice subsystem)
 *   0x80070340-0x800703FF  Cave 0 (236B, BOX_LOAD)
 *   0x800719D4-0x80071CD3  Cave 2 (768B, native text triggers + strings + voice_table)
 *   0x80071A30+            VOICE_TABLE (8B/entry: msf+sectors+size, up to 64 voices)
 *   0x8007153C-0x800717FF  Cave 3 (736B, PATCH_LOADER + TEXT_DISMISS_V2)
 * </pre>
 *
 * <p>SPU RAM (Phase 3d voice subsystem):
 * <pre>
 *   0x60000  voice 23 sample buffer (single buffer, voice id baked into op2B arg2)
 * </pre>
 *
 * Usage: {@code new PatchReference().runDiagnostics()}
 */
public class PatchReference {

    // =========================================================================
    // PSX Memory Simulation
    // =========================================================================

    /** Simulated PSX RAM (2MB) mapped from 0x80000000 */
    private final byte[] ram = new byte[2 * 1024 * 1024];

    // --- Address Constants ---
    private static final int RAM_BASE = 0x80000000;

    // State bytes (inventory/item box system)
    private static final int STATE_BASE       = 0x800B25BC;
    private static final int STATE_25BD        = 0x800B25BD;  // cursor position (0-9)
    private static final int STATE_25C0        = 0x800B25C0;  // mode: 0=normal inv, 1=item box
    private static final int STATE_25C1        = 0x800B25C1;  // sub-mode: 0=menu, 2=browse
    private static final int STATE_25C2        = 0x800B25C2;  // CCA4[2] dispatch index
    private static final int STATE_25C8        = 0x800B25C8;  // display cursor
    private static final int STATE_25CF        = 0x800B25CF;  // CHECK cursor
    private static final int STATE_25D6        = 0x800B25D6;  // submenu cursor / CHECK phase
    private static final int STATE_25DC        = 0x800B25DC;  // CHECK overlay X position (hw)
    private static final int STATE_25DE        = 0x800B25DE;  // CHECK overlay Y position (hw)
    private static final int STATE_25E0        = 0x800B25E0;  // panel X position (hw)
    private static final int STATE_25EE        = 0x800B25EE;  // scroll counter (hw)
    private static final int STATE_260E        = 0x800B260E;  // item type for CHECK lookup
    private static final int STATE_2668        = 0x800B2668;  // STPIC state

    // Player data
    private static final int PLAYER_HP         = 0x800B0FAC;  // player health
    private static final int PLAYER_INVENTORY  = 0x800B10AC;  // 10 slots × 4 bytes
    private static final int EQUIPPED_SLOT     = 0x800B25C8;  // equipped weapon slot

    // Custom flags (in code caves — MUST NOT overlap Cave 2 text data at 0x800719D4+)
    private static final int PAGE_FLAG         = 0x800719D0;  // Take/Check active
    private static final int SAVED_PHASE       = 0x800719D1;  // CHECK phase counter (0-5) — v1.12: relocated from 0x800719D5
    private static final int MODE_FLAG         = 0x800719DC;  // MODEL_RESTORE state (0/1/2) — v1.15: relocated from 0x800719D1 (collision with SAVED_PHASE!)
    private static final int CHECK_MODE        = 0x800719D2;  // 0=normal, 2=CHECK (also: deposit_happened flag)
    private static final int SAVED_EQUIPPED    = 0x800719DD;  // v1.15: saved equipped slot (Cave 2, was 0x800719D2 = collision with CHECK_MODE!)
    private static final int TEXT_FLAG         = 0x800719D3;  // native text active flag
    private static final int SAVED_CURSOR      = 0x800719CE;  // saved cursor for CHECK — v1.12: relocated from 0x800719D4

    // Item box storage
    private static final int BOX_STORAGE       = 0x800BFB00;  // 50 slots × 4B = 200B
    private static final int PLAYER_INV_SAVE   = 0x800BFC00;  // backup of player inventory

    // Save system
    private static final int SAVE_BUFFER       = 0x80160000;  // memory card save buffer
    private static final int ITEMALL_PIX_BUFFER = 0x80160000; // v1.15: ITEMALL.PIX loaded here (= save buffer, free during gameplay)
    private static final int POST_GSB          = 0x800B2208;  // post-GSB area for extra data
    private static final int GAME_FLAGS_BASE   = 0x800B08D0;  // game flags array
    private static final int SAVE_SIZE         = 0x1680;      // total save data size

    // CHECK display (in DEBUG.BIN overlay)
    private static final int CHECK_ENABLE      = 0x800C6220;  // panel enable flag (hw)
    private static final int CHECK_X_POS       = 0x800C6222;  // panel X position (hw, signed)
    private static final int CHECK_Y_BASE      = 0x800C6224;  // panel Y base (hw)

    // Text system (native game API)
    private static final int MSG_STATE         = 0x800B8520;  // message state struct (32B)
    private static final int MSG_FLAGS         = 0x800B8520;  // +0x00: flags (0x80=active)
    private static final int MSG_TEXT_PTR      = 0x800B8528;  // +0x08: text data pointer
    private static final int MSG_X_POS         = 0x800B8534;  // +0x14: X position
    private static final int MSG_Y_POS         = 0x800B8536;  // +0x16: Y position
    private static final int ACA40_BACKUP      = 0x800C853C;  // backup of 0x800ACA40 for restore

    // GPU DMA registers
    private static final int D2_MADR           = 0x1F8010A0;  // DMA channel 2 start address
    private static final int D2_BCR            = 0x1F8010A4;  // DMA block size
    private static final int D2_CHCR           = 0x1F8010A8;  // DMA control register
    private static final int DMA_LINKED_LIST   = 0x01000401;  // CHCR: linked-list, RAM→GPU

    // OT system
    private static final int OT_BASE_DB0       = 0x800AA6D8;  // OT buffer double-buffer 0
    private static final int OT_BASE_DB1       = 0x800AB6D8;  // OT buffer double-buffer 1
    private static final int TEXT_OT_FIXED     = 0x800AA6B4;  // text OT (NOT double-buffered)
    private static final int OT_TERMINATOR     = 0x00FFFFFF;  // linked-list terminator
    private static final int PRIM_BUFFER_PTR   = 0x800A7394;  // prim buffer bump pointer
    private static final int PRIM_BUFFER_BASE  = 0x800CC934;  // initial prim buffer address
    private static final int ISOLATED_PRIM_BUF = 0x800BF000;  // v1.12: isolated prim buffer for TEXT_DISMISS_V2

    // Bucket restore values (count=0x02 includes DR_ENV DrawMode+TexWindow)
    private static final int BUCKET_INIT_DB0   = 0x020AA6A0;  // buf0 bkt[3]+0 init value
    private static final int BUCKET_INIT_DB1   = 0x020AA6C0;  // buf1 bkt[3]+0 init value
    private static final int BUF_IDX           = 0x800ACA34;  // double-buffer index (0 or 1)
    private static final int BUCKET_DB0        = 0x800C82E4;  // buf0 bkt[3]+0
    private static final int BUCKET_DB1        = 0x800C83A4;  // buf1 bkt[3]+0

    // Item box model fix
    private static final int MODEL_SAVE_SIZE   = 256;         // animation table save size

    // Voice subsystem (v1.20+v1.21, Phase 3d-3 op2B hook)
    private static final int VOICE_TABLE_BASE       = 0x80071A30;  // Cave 2: 8B/entry (msf[3] + sectors[1] + size[4])
    private static final int VOICE_TABLE_ENTRY_SIZE = 8;
    private static final int LOADED_VOICE_ID        = 0x800719EE;  // Cave 2: cached voice_id (init 0xFF)
    private static final int VOICE_HANDLER_COUNTER  = 0x800719F0;  // Cave 2: legacy SCD 0x59 wrapper hits
    private static final int VOICE_FINAL_MARKER     = 0x800719F6;  // Cave 2: 0xBABE = epilogue reached
    private static final int VOICE_STEP_TRACE       = 0x800719F8;  // Cave 2: step trace 0x0001..0x000A
    private static final int VOICE_MSF_BUFFER       = 0x800719FC;  // Cave 2: CdControlB SetLoc target (4B)
    private static final int SPU_VOICE_BUFFER       = 0x60000;     // SPU RAM: voice 23 sample target
    private static final int SPU_VOICE_INDEX        = 23;
    private static final int OP2B_HANDLER_NATIVE    = 0x8006EA20;  // room-overlay-loaded text-display handler
    private static final int MESSAGE_ON_NATIVE      = 0x800404F4;  // SCD opcode 0x21 (Message_on)
    private static final int SCD_TABLE_BASE_FILE    = 0x064CA8;    // SCD dispatch table (FILE offset)
    private static final int SCD_OP2B               = 0x2B;        // text-display opcode (used in ROOM1240 intro)

    // Item constants
    private static final int ITEM_KNIFE        = 0x01;
    private static final int ITEM_EMPTY        = 0x00;
    private static final int MAX_INVENTORY     = 10;
    private static final int MAX_BOX_SLOTS     = 50;
    private static final int BOX_PAGES         = 5;
    private static final int ITEMS_PER_PAGE    = 10;

    // =========================================================================
    // Memory Access Helpers
    // =========================================================================

    private int addr(int psxAddr) {
        return (psxAddr & 0x001FFFFF);  // strip KSEG0/1 prefix, map to RAM array
    }

    private int readByte(int psxAddr) {
        return ram[addr(psxAddr)] & 0xFF;
    }

    private void writeByte(int psxAddr, int value) {
        ram[addr(psxAddr)] = (byte)(value & 0xFF);
    }

    private int readHalfword(int psxAddr) {
        int a = addr(psxAddr);
        return (ram[a] & 0xFF) | ((ram[a+1] & 0xFF) << 8);  // little-endian
    }

    private void writeHalfword(int psxAddr, int value) {
        int a = addr(psxAddr);
        ram[a]   = (byte)(value & 0xFF);
        ram[a+1] = (byte)((value >> 8) & 0xFF);
    }

    private int readWord(int psxAddr) {
        int a = addr(psxAddr);
        return (ram[a] & 0xFF) | ((ram[a+1] & 0xFF) << 8)
             | ((ram[a+2] & 0xFF) << 16) | ((ram[a+3] & 0xFF) << 24);
    }

    private void writeWord(int psxAddr, int value) {
        int a = addr(psxAddr);
        ram[a]   = (byte)(value & 0xFF);
        ram[a+1] = (byte)((value >> 8) & 0xFF);
        ram[a+2] = (byte)((value >> 16) & 0xFF);
        ram[a+3] = (byte)((value >> 24) & 0xFF);
    }

    private void memcpy(int dst, int src, int size) {
        System.arraycopy(ram, addr(src), ram, addr(dst), size);
    }

    // =========================================================================
    // Inventory Slot Helpers
    // =========================================================================

    /** Each inventory slot is 4 bytes: [type, quantity, flag1, flag2] */
    private int getItemType(int baseAddr, int slotIndex) {
        return readByte(baseAddr + slotIndex * 4);
    }

    private int getItemQuantity(int baseAddr, int slotIndex) {
        return readByte(baseAddr + slotIndex * 4 + 1);
    }

    private void setSlot(int baseAddr, int slotIndex, int type, int qty, int f1, int f2) {
        int a = baseAddr + slotIndex * 4;
        writeByte(a, type);
        writeByte(a + 1, qty);
        writeByte(a + 2, f1);
        writeByte(a + 3, f2);
    }

    private void clearSlot(int baseAddr, int slotIndex) {
        setSlot(baseAddr, slotIndex, 0, 0, 0, 0);
    }

    private void copySlot(int dstBase, int dstIdx, int srcBase, int srcIdx) {
        memcpy(dstBase + dstIdx * 4, srcBase + srcIdx * 4, 4);
    }

    private int countItems(int baseAddr, int maxSlots) {
        int count = 0;
        for (int i = 0; i < maxSlots; i++) {
            if (getItemType(baseAddr, i) != ITEM_EMPTY) count++;
        }
        return count;
    }

    private int findEmptySlot(int baseAddr, int maxSlots) {
        for (int i = 0; i < maxSlots; i++) {
            if (getItemType(baseAddr, i) == ITEM_EMPTY) return i;
        }
        return -1;
    }

    // =========================================================================
    // PART 1: SAVE SYSTEM
    // =========================================================================

    /**
     * [A] SCD_HOOK: Intercepts SCD Message_on(0xFE, 0xFFFF) for save trigger.
     * Also intercepts Message_on(0xFD) for item box trigger.
     * 92 bytes at 0x8006EA20 in EXE.
     */
    public void scdHook_interceptSaveAndItemBoxTrigger(int msgIndex, int msgValue) {
        if (msgIndex == 0xFE && msgValue == 0xFFFF) {
            int flag0 = readWord(GAME_FLAGS_BASE);
            writeWord(GAME_FLAGS_BASE, flag0 | (1 << 18));
            writeByte(0x800B5359, 1);
        } else if (msgIndex == 0xFD) {
            itemBoxCheck_verifySentinel();
        }
    }

    /** [P] EXTENDED_SNAPSHOT: Save flags 0,1,2,11 to post-GSB for memory card. */
    public void extendedSnapshot_saveGameFlags() {
        writeWord(POST_GSB + 0x00, readWord(GAME_FLAGS_BASE + 0 * 4));
        writeWord(POST_GSB + 0x04, readWord(GAME_FLAGS_BASE + 1 * 4));
        writeWord(POST_GSB + 0x08, readWord(GAME_FLAGS_BASE + 2 * 4));
        writeWord(POST_GSB + 0x0C, readWord(GAME_FLAGS_BASE + 11 * 4));
    }

    /** [Q] RESTORE_CODE: On load, restore flag 0 (OR-merge) + flag 2 (direct). */
    public void restoreCode_loadGameFlags(int cardBufferAddr) {
        int src = cardBufferAddr + 0x0200;
        int flag0 = readWord(GAME_FLAGS_BASE) | readWord(src + 0x00);
        writeWord(GAME_FLAGS_BASE, flag0);
        writeWord(GAME_FLAGS_BASE + 2 * 4, readWord(src + 0x08));
    }

    /** [V] HP_SAVE: Save HP + full inventory + weapon to post-GSB. */
    public void hpSave_capturePlayerState() {
        writeHalfword(POST_GSB + 0x10, readHalfword(PLAYER_HP));
        memcpy(POST_GSB + 0x20, PLAYER_INVENTORY, MAX_INVENTORY * 4);
        writeByte(POST_GSB + 0x48, readByte(EQUIPPED_SLOT));
    }

    /** WEAPON_CAPTURE: Before save, capture weapon type + sentinel + save_count. */
    public void weaponCapture_beforeSave() {
        writeByte(POST_GSB + 0x4A, 0xAA);
        writeByte(POST_GSB + 0x4B, readByte(STATE_25C8));
        writeByte(POST_GSB + 0x4C, readByte(PLAYER_INVENTORY + readByte(STATE_25C8) * 4));
        int saveCount = readByte(0x800B0FBD);
        writeByte(0x800B0FBD, saveCount + 1);
    }

    /**
     * [W] CD_FIX: Guard room init CD channel when in item box mode.
     * v1.19 (commit 8807f1f2): made into a no-op for the helicopter SFX channel —
     * the prior aggressive clear was wiping helicopter SFX state on every room
     * transition, regression introduced ≈ a1e07588 (v1.4 era). The item-box guard
     * still applies for non-affected channels.
     */
    public boolean cdFix_shouldSkipRoomInitCD() {
        return readByte(STATE_25C0) == 1;
    }

    /** [Z] SAVE_LOC_FUNC: Per-slot location name index for memory card title. */
    public int saveLocFunc_getLocationIndex(int returnAddress) {
        return (returnAddress == 0x800404F4) ? 0 : 1;
    }

    // =========================================================================
    // PART 2: ITEM BOX — CORE
    // =========================================================================

    /** ITEM_BOX_CHECK: Verify SCD sentinel (0xFD) for item box trigger. */
    public void itemBoxCheck_verifySentinel() {
        itemBoxOpen_enterItemBoxMode();
    }

    /** ITEM_BOX_OPEN: Transition to item box mode (state[25C0]=1). */
    public void itemBoxOpen_enterItemBoxMode() {
        writeByte(STATE_25C0, 1);
        modelSave_backupAnimationTable();
    }

    /**
     * DEPOSIT_HOOK: Auto-deposit equipped item to BOX_STORAGE.
     * Guards: knife → native text, box-full → native text.
     * v1.12: Knife/Boxfull triggers use show_message (native text).
     */
    public boolean depositHook_autoDeposit() {
        int cursor = readByte(STATE_25BD);
        int itemType = getItemType(PLAYER_INVENTORY, cursor);

        if (itemType == ITEM_KNIFE) {
            nativeTextTrigger_knife();
            return false;
        }

        int boxSlot = findEmptySlot(BOX_STORAGE, MAX_BOX_SLOTS);
        if (boxSlot == -1) {
            nativeTextTrigger_boxfull();
            return false;
        }

        copySlot(BOX_STORAGE, boxSlot, PLAYER_INVENTORY, cursor);
        clearSlot(PLAYER_INVENTORY, cursor);
        writeByte(CHECK_MODE, 1);  // deposit_happened
        System.out.printf("  [DEPOSIT] Item 0x%02X -> box slot %d%n", itemType, boxSlot);
        return true;
    }

    // =========================================================================
    // PART 3: MODEL FIX
    // =========================================================================

    /** Save animation table (256B) + set mode_flag=1 to trigger restore on close. */
    public void modelSave_backupAnimationTable() {
        writeByte(CHECK_MODE, 0);  // clear deposit_happened
        writeByte(MODE_FLAG, 1);   // v1.15: trigger MODEL_RESTORE (at 0x800719DC, NOT 0x800719D1!)
        System.out.println("  [MODEL] Animation table saved (256B), mode_flag=1");
    }

    /**
     * Restore animation table + clear state[0] to prevent re-init with bad data.
     * v1.15: mode_flag at 0x800719DC (was 0x800719D1 = collision with saved_phase).
     * 2-stage: mode_flag=1 → clear state[0], set mode_flag=2.
     *          mode_flag=2 → memcpy restore, clear mode_flag.
     */
    public void modelRestore_restoreAnimationTable() {
        int mf = readByte(MODE_FLAG);
        if (mf == 0) return;  // skip
        if (mf == 1) {
            writeByte(STATE_25C0, 0);  // clear state[0]
            writeByte(MODE_FLAG, 2);
            System.out.println("  [MODEL] Stage 1: state[0]=0, mode_flag=2");
        } else if (mf == 2) {
            writeByte(MODE_FLAG, 0);
            System.out.println("  [MODEL] Stage 2: animation table restored (256B), mode_flag=0");
        }
    }

    // =========================================================================
    // PART 4: TAKE / WITHDRAW
    // =========================================================================

    /** PB_TAKE_HANDLER: Enter Take view — copy box items to inventory display. */
    public void takeHandler_enterTakeView() {
        memcpy(PLAYER_INV_SAVE, PLAYER_INVENTORY, MAX_INVENTORY * 4);
        int takePage = readByte(0x800719CD);  // take_page
        int pageOffset = takePage * ITEMS_PER_PAGE;
        for (int i = 0; i < ITEMS_PER_PAGE; i++) {
            copySlot(PLAYER_INVENTORY, i, BOX_STORAGE, pageOffset + i);
        }
        writeByte(PAGE_FLAG, 1);
        System.out.println("  [TAKE] Entered Take view, page " + takePage);
    }

    /** PB_TAKE_RESTORE: Exit Take view — restore player inventory. */
    public void takeRestore_exitTakeView() {
        memcpy(PLAYER_INVENTORY, PLAYER_INV_SAVE, MAX_INVENTORY * 4);
        writeByte(PAGE_FLAG, 0);
        System.out.println("  [TAKE] Exited Take view, inventory restored");
    }

    /**
     * PB_WITHDRAW: Take selected item from box to player inventory.
     * v1.15: After clearing, calls PB_COMPACT_BOX_SYNC instead of compact_inventory.
     * compact_inventory shifts g_inventory + VRAM icons, then we ALSO compact
     * BOX_STORAGE[page] in sync. Without BOX compact, cursor desyncs → item duplication.
     */
    public boolean withdraw_takeItemFromBox(int cursor, int takePage) {
        int boxIndex = takePage * ITEMS_PER_PAGE + cursor;
        int itemType = getItemType(BOX_STORAGE, boxIndex);

        if (itemType == ITEM_EMPTY) {
            return false;
        }

        if (countItems(PLAYER_INV_SAVE, MAX_INVENTORY) >= MAX_INVENTORY) {
            nativeTextTrigger_invfull();
            return false;
        }

        int emptySlot = findEmptySlot(PLAYER_INV_SAVE, MAX_INVENTORY);
        if (emptySlot == -1) return false;

        copySlot(PLAYER_INV_SAVE, emptySlot, BOX_STORAGE, boxIndex);
        clearSlot(BOX_STORAGE, boxIndex);
        clearSlot(PLAYER_INVENTORY, cursor);
        // v1.15: compact_inventory (g_inv + icons) + compact BOX_STORAGE[page]
        compactBoxStoragePage(takePage);
        System.out.printf("  [WITHDRAW] Item 0x%02X -> player slot %d (BOX page %d compacted)%n",
                itemType, emptySlot, takePage);
        return true;
    }

    /**
     * PB_COMPACT_BOX_SYNC: compact BOX_STORAGE[page] — shift non-empty items down, zero remainder.
     * v1.15: Keeps BOX_STORAGE in sync with g_inventory after compact_inventory.
     * Without this, cursor positions desync → WITHDRAW clears wrong slot → item duplication.
     */
    private void compactBoxStoragePage(int page) {
        int pageStart = BOX_STORAGE + page * ITEMS_PER_PAGE * 4;
        int dst = 0;
        int[] temp = new int[ITEMS_PER_PAGE];
        // Collect non-empty items
        for (int i = 0; i < ITEMS_PER_PAGE; i++) {
            int item = readWord(pageStart + i * 4);
            if (item != 0) {
                temp[dst++] = item;
            }
        }
        // Write back compacted + zero remainder
        for (int i = 0; i < ITEMS_PER_PAGE; i++) {
            writeWord(pageStart + i * 4, i < dst ? temp[i] : 0);
        }
    }

    /** PB_PRE_DEPOSIT: Routes to deposit, withdraw, or CHECK based on mode. */
    public void preDepositCheck(int cursor, int takePage) {
        int pageFlag = readByte(PAGE_FLAG);
        int checkMode = readByte(CHECK_MODE);

        if (pageFlag == 0) {
            depositHook_autoDeposit();
        } else if (checkMode == 2) {
            checkAction_startCheckDisplay(cursor);
        } else {
            withdraw_takeItemFromBox(cursor, takePage);
        }
    }

    // =========================================================================
    // PART 5: PAGING (R/L)
    // =========================================================================

    /** PB_TAKE_PAGE_CHECK: Detect L1/R1 edge for page switching. */
    public void takePageCheck_detectPageButton(int buttonsNewPress) {
        boolean l1 = (buttonsNewPress & 0x0004) != 0;
        boolean r1 = (buttonsNewPress & 0x0008) != 0;

        if (l1) takePageDo_switchPage(-1);
        else if (r1) takePageDo_switchPage(1);
    }

    /** PB_TAKE_PAGE_DO: Switch page with wrap (0-4), copy new items to display. */
    public void takePageDo_switchPage(int direction) {
        int takePage = readByte(0x800719CD);
        takePage = (takePage + direction + BOX_PAGES) % BOX_PAGES;
        writeByte(0x800719CD, takePage);

        int pageOffset = takePage * ITEMS_PER_PAGE;
        for (int i = 0; i < ITEMS_PER_PAGE; i++) {
            copySlot(PLAYER_INVENTORY, i, BOX_STORAGE, pageOffset + i);
        }
        System.out.println("  [PAGE] Switched to page " + takePage);
    }

    // =========================================================================
    // PART 6: ICON REFRESH
    // =========================================================================

    /**
     * PB_REFRESH_ICONS: Load ITEMALL.PIX from CD and upload item icons to VRAM.
     * v1.15: ALL loads go to 0x80160000 (save buffer), NOT 0x80100000 (stage data!).
     * 0x80100000 contains stage/camera data needed by rooms with enemies (e.g. room 114).
     * Save buffer at 0x80160000 is free during gameplay (only used during save/load I/O).
     */
    public void refreshIcons_loadFromCD() {
        System.out.printf("  [ICONS] CD_read(11, 0x%08X) + upload_icon_to_slot x 10%n", ITEMALL_PIX_BUFFER);
    }

    // =========================================================================
    // PART 7: NATIVE TEXT SYSTEM (v1.12)
    // =========================================================================

    /**
     * Native text trigger: Uses game's show_message(0x8800) API.
     * show_message(0x8800) skips the corrupted text table and allows manual
     * text_ptr setup. After the call, we set:
     *   - text_ptr at 0x800B8528 to our Cave 2 encoded string
     *   - state[25C1]=2, state[25C2]=5 to block D-pad during display
     *   - speed_flag=1 for button-speedup (like CHECK's Phase 1)
     *   - TEXT_FLAG=1 to enable TEXT_DISMISS_V2 rendering
     *
     * Text strings in Cave 2 use native encoding (A-Z=0x1D-0x36, space=0x00).
     * Terminator: 0x02 0x00 0x01 0x01 (page break arrow + auto-dismiss).
     */
    private void showMessageNative(int textAddr, String label) {
        // jal 0x80027E68 with a0=0x8800 (skip text table)
        writeByte(MSG_FLAGS, 0x80);  // activate text
        writeWord(MSG_TEXT_PTR, textAddr);  // manual text pointer
        writeByte(STATE_25C1, 2);  // block D-pad
        writeByte(STATE_25C2, 5);  // block D-pad
        writeByte(TEXT_FLAG, 1);   // enable TEXT_DISMISS_V2
        System.out.printf("  [TEXT] %s: show_message(0x8800) + text_ptr=0x%08X%n", label, textAddr);
    }

    /** KNIFE_NATIVE (Cave 2): "Knife cannot be stored." */
    public void nativeTextTrigger_knife() {
        showMessageNative(0x800719D4, "Knife");
    }

    /** BOXFULL_NATIVE (Cave 2): "No more space." */
    public void nativeTextTrigger_boxfull() {
        showMessageNative(0x800719F0, "Boxfull");
    }

    /** INVFULL_NATIVE (Cave 2): "Inventory full." */
    public void nativeTextTrigger_invfull() {
        showMessageNative(0x80071A04, "Invfull");
    }

    /**
     * TEXT_DISMISS_V2: Per-frame text renderer in Cave 3 (332B).
     * Hooked at CD18 (0x8004CD18), runs every frame during inventory.
     *
     * <pre>
     * Key innovation: Isolated prim buffer at 0x800BF000.
     * The main loop and inventory renderer use 0x800A7394 as prim pointer.
     * TEXT_DISMISS_V2 saves 0x800A7394, redirects to 0x800BF000, renders,
     * then restores 0x800A7394. This prevents cross-contamination.
     *
     * Flow:
     *   1. Guard: check_mode!=0 → skip (CHECK has own text render)
     *   2. Guard: bit7=0 → go to restore path (text dismissed)
     *   3. Guard: flag=0 → skip (no active text)
     *   4. Save 0x800A7394, redirect to 0x800BF000
     *   5. Restore bucket bkt[3]+0 to init value (per buf_idx)
     *   6. Pre-init count bytes at isolated buffer (0x04000000 × 30 slots)
     *   7. Call 0x800280B4 (FSM + render_commit → text prims to isolated buffer)
     *   8. DMA wait + DMA from 0x800AA6B4 (text OT head)
     *   9. Restore bucket AGAIN (post-DMA, prevents inventory DrawOTag crash)
     *  10. Restore 0x800A7394
     *
     * Restore path (bit7=0, flag=1 → text just dismissed):
     *   - Clear flag, set state[25C2]=1, restore ACA40 from backup
     * </pre>
     */
    public void textDismissV2_perFrame() {
        int checkMode = readByte(CHECK_MODE);
        int bit7 = readByte(MSG_FLAGS) & 0x80;
        int flag = readByte(TEXT_FLAG);

        // Guard: CHECK has own text render
        if (checkMode != 0) return;

        // bit7=0 → text dismissed, check if we need to restore
        if (bit7 == 0) {
            if (flag != 0) {
                // Dismiss: clear flag, restore state
                writeByte(TEXT_FLAG, 0);
                writeByte(STATE_25C2, 1);  // back to browse
                // Restore ACA40 from backup
                writeWord(0x800ACA40, readWord(ACA40_BACKUP));
                System.out.println("  [TEXT_DISMISS] Restored (flag cleared, ACA40 restored)");
            }
            return;
        }

        // bit7=1 but no flag → not our text
        if (flag == 0) return;

        // === Active render with isolated prim buffer ===
        int savedPrimPtr = readWord(PRIM_BUFFER_PTR);
        writeWord(PRIM_BUFFER_PTR, ISOLATED_PRIM_BUF);  // redirect

        // Restore bucket for current double-buffer
        int bufIdx = readByte(BUF_IDX);
        if (bufIdx == 0) {
            writeWord(BUCKET_DB0, BUCKET_INIT_DB0);
        } else {
            writeWord(BUCKET_DB1, BUCKET_INIT_DB1);
        }

        // Pre-init count bytes at isolated buffer
        for (int i = 0; i < 30; i++) {
            writeWord(ISOLATED_PRIM_BUF + i * 20, 0x04000000);
        }

        // Call text render pipeline
        textTypewriterRender_0x800280B4();

        // DMA from text OT head
        startLinkedListDma(TEXT_OT_FIXED);

        // Post-DMA: restore BOTH buckets (prevents inventory DrawOTag crash)
        writeWord(BUCKET_DB0, BUCKET_INIT_DB0);
        writeWord(BUCKET_DB1, BUCKET_INIT_DB1);

        // Restore original prim pointer
        writeWord(PRIM_BUFFER_PTR, savedPrimPtr);

        System.out.println("  [TEXT_DISMISS] Rendered via isolated prim buffer 0x800BF000");
    }

    // =========================================================================
    // PART 8: CHECK DISPLAY
    // =========================================================================

    /** PB_CHECK_ENTER: Browse menu entry [0] — sets check_mode=2, enters browse. */
    public void checkEnter_setCheckModeAndBrowse() {
        writeByte(CHECK_MODE, 2);
        takeHandler_enterTakeView();
        System.out.println("  [CHECK] Entered CHECK mode (check_mode=2)");
    }

    /**
     * PB_CHECK_ACTION: User confirmed item in CHECK — load image + start text.
     * 160 bytes at PATCH.BIN.
     */
    public void checkAction_startCheckDisplay(int cursor) {
        writeByte(SAVED_CURSOR, cursor);
        writeByte(STATE_25C8, cursor);
        int itemType = getItemType(PLAYER_INVENTORY, cursor);
        writeByte(STATE_260E, itemType);

        loadItemCheckImage(itemType);
        showMessage(0x00A00013, 0x8400, itemType, 0xFF000000);

        writeHalfword(CHECK_ENABLE, 1);
        writeHalfword(CHECK_X_POS, -207);
        writeHalfword(CHECK_Y_BASE, 26);

        writeByte(SAVED_PHASE, 1);
        writeByte(STATE_25C1, 2);
        writeByte(STATE_25C2, 2);

        System.out.printf("  [CHECK] Started CHECK for item 0x%02X at cursor %d%n",
                itemType, cursor);
    }

    /**
     * PB_CHECK_STATE3: Per-frame CHECK handler at CCA4[2] entry [2].
     * 420 bytes at PATCH.BIN.
     */
    public void checkState3_perFrameHandler() {
        int checkMode = readByte(CHECK_MODE);
        int savedPhase = readByte(SAVED_PHASE);

        if (checkMode != 2) {
            originalEntry2_panelExpansion();
            return;
        }
        if (savedPhase == 0) {
            writeByte(CHECK_MODE, 0);
            originalEntry2_panelExpansion();
            return;
        }
        if (savedPhase >= 4) {
            exitCheckWithButtonReleaseWait();
            return;
        }

        writeByte(STATE_25D6, savedPhase);
        writeByte(STATE_25BD, readByte(SAVED_CURSOR));
        nativeCheckHandler_0x800C6630(savedPhase);

        int otBase = getOtEntryForCurrentDb();
        clearOtAsLinkedChain(otBase, 16);
        int otLast = otBase + 15 * 4;

        writeWord(PRIM_BUFFER_PTR, PRIM_BUFFER_BASE);
        itemDataPanelRenderer_0x800C6228();
        textTypewriterRender_0x800280B4();

        waitForGpuDmaComplete();
        startLinkedListDma(otLast);
        waitForGpuDmaComplete();
        startLinkedListDma(TEXT_OT_FIXED);

        savedPhase = readByte(STATE_25D6);
        writeByte(SAVED_PHASE, savedPhase);

        if (readByte(STATE_25C2) != 2) {
            exitCheckWithButtonReleaseWait();
        }

        System.out.printf("  [CHECK] Frame: phase=%d, panelX=%d%n",
                savedPhase, (short)readHalfword(CHECK_X_POS));
    }

    private void exitCheckWithButtonReleaseWait() {
        int buttons = readHalfword(0x800BC762) & 0xC000;
        if (buttons == 0) {
            writeByte(STATE_25C2, 1);
            writeByte(SAVED_PHASE, 0);
            System.out.println("  [CHECK] Exit -> browse (buttons released)");
        } else {
            writeByte(SAVED_PHASE, 5);
            System.out.println("  [CHECK] Waiting for button release...");
        }
    }

    /**
     * PB_TAKE_HINT_NATIVE (Cave 2): Clears check_mode on Take entry.
     * Replaces old PB_TAKE_HINT which had chain-based typewriter text.
     * v1.12: simplified to just clear flags and jump to TAKE_HANDLER.
     */
    public void takeHintNative_clearAndEnterTake() {
        writeByte(CHECK_MODE, 0);
        writeByte(MSG_FLAGS, 0);  // clear any active text
        System.out.println("  [TAKE_HINT] Cleared check_mode + text state (Take entry)");
    }

    /**
     * TAKE_RESTORE_AND_REFRESH (PATCH2.BIN): Restores player inventory,
     * refreshes VRAM icons, clears custom flags. Called from TBC2[31].
     * v1.17: REMOVED writes to room-overlay addresses (0x800C8520, 0x800BCA40,
     * 0x800A7394/98) which corrupted SCD script state causing input lockup.
     */
    public void takeRestoreAndRefresh() {
        takeRestore_exitTakeView();
        // Clear our custom flags only (NOT room overlay addresses!)
        writeByte(CHECK_MODE, 0);
        writeByte(SAVED_PHASE, 0);
        writeByte(0x800719CB, 0);  // paging_refresh
        // Reload ITEMALL.PIX + refresh icons
        refreshIcons_loadFromCD();
        System.out.println("  [RESTORE] Full state restore + icon refresh");
    }

    // =========================================================================
    // PART 9: BOX PERSISTENCE
    // =========================================================================

    /** PB_BOX_SAVE: Write BOX_STORAGE to post-GSB area (chained from WEAPON_CAPTURE). */
    public void boxSave_writeToPostGsb() {
        memcpy(POST_GSB + 0x50, BOX_STORAGE, MAX_BOX_SLOTS * 4);
        System.out.println("  [BOX_SAVE] 200B written to post-GSB");
    }

    /** BOX_LOAD: Read BOX_STORAGE from card buffer (in EXE Cave 0, not PATCH.BIN). */
    public void boxLoad_readFromCard(int cardBufferAddr) {
        memcpy(BOX_STORAGE, cardBufferAddr + 0x0200 + 0x50, MAX_BOX_SLOTS * 4);
        System.out.println("  [BOX_LOAD] 200B read from card");
    }

    // =========================================================================
    // PART 10: PATCH LOADER (Cave 3)
    // =========================================================================

    /** PATCH_LOADER: Per-frame from SCD_FRAME. Loads PATCH.BIN on first call. */
    private boolean patchBinLoaded = false;

    public void patchLoader_perFrame() {
        if (!patchBinLoaded) {
            System.out.println("  [LOADER] PATCH.BIN loaded from CD (10 sectors = 20KB -> 0x801F0000, includes PATCH2 at 0x1000)");
            patchBinLoaded = true;
            return;
        }
        // checkExitWrapper + modelRestore run each frame
    }

    // =========================================================================
    // Native Function Stubs (called but not reimplemented)
    // =========================================================================

    private void originalEntry2_panelExpansion() {
        System.out.println("  [NATIVE] 0x8004D28C: panel expansion (original entry [2])");
    }

    private void nativeCheckHandler_0x800C6630(int phase) {
        System.out.printf("  [NATIVE] 0x800C6630: CHECK handler phase %d%n", phase);
    }

    private void itemDataPanelRenderer_0x800C6228() {
        System.out.println("  [NATIVE] 0x800C6228: ITEM DATA panel -> OT primitives");
    }

    private void textTypewriterRender_0x800280B4() {
        System.out.println("  [NATIVE] 0x800280B4: text typewriter -> text OT sprites");
    }

    private void loadItemCheckImage(int itemId) {
        System.out.printf("  [NATIVE] 0x800C6878: load ITPS image for item 0x%02X%n", itemId);
    }

    private void showMessage(int pos, int mode, int itemId, int color) {
        System.out.printf("  [NATIVE] 0x80027E68: start text (item=0x%02X, pos=0x%08X)%n",
                itemId, pos);
    }

    private int getOtEntryForCurrentDb() {
        int dbIndex = readByte(BUF_IDX);
        return OT_BASE_DB0 + dbIndex * 0x1000;
    }

    private void clearOtAsLinkedChain(int otBase, int numEntries) {
        writeWord(otBase, OT_TERMINATOR);
        for (int i = 1; i < numEntries; i++) {
            int prevAddr = otBase + (i - 1) * 4;
            writeWord(otBase + i * 4, prevAddr & 0x00FFFFFF);
        }
    }

    private void waitForGpuDmaComplete() {
        // In real PSX: poll D2_CHCR bit 24 until 0
    }

    private void startLinkedListDma(int otAddr) {
        System.out.printf("  [DMA] Linked-list from 0x%08X%n", otAddr);
    }

    // =========================================================================
    // PART 7: VOICE SUBSYSTEM (v1.20 voice_table, v1.21 op2B hook)
    // =========================================================================
    //
    // Pipeline: synchro/STAGE<N>/room<XXXX>/mainNN.{wav,mp3}
    //   ↳ generate_voice_pak(): transcode .mp3→.wav (ffmpeg or miniaudio),
    //      VAG-encode @ 11025 Hz, pack sector-aligned into VOICE.PAK, populate
    //      VOICE_TABLE @ 0x80071A30 + VOICE_LOOKUP[(room,msg)]→voice_id
    //   ↳ patch_rdt_voices_all(): bake voice_id into op2B opcode arg2
    //      (`2B NN 00 00` → `2B NN VV 00`) inside SCD region of every RDT
    //   ↳ runtime: SCD dispatch[0x2B] → PB_OP2B_WRAPPER reads VV, calls
    //      PB_VOICE_PLAY, tail-jumps to native handler 0x8006EA20

    /** Per-voice metadata stored at VOICE_TABLE_BASE + voiceId*8. */
    public static final class VoiceTableEntry {
        public final int msf;          // 24-bit MSF for CdControlB SetLoc
        public final int sectors;      // sector count for CdRead
        public final int sizeBytes;    // payload size in VAG bytes
        public VoiceTableEntry(int msf, int sectors, int sizeBytes) {
            this.msf = msf; this.sectors = sectors; this.sizeBytes = sizeBytes;
        }
    }

    /** Lookup voice metadata by 1-based voice_id; voice_id 0 is reserved no-op. */
    public VoiceTableEntry voiceTableLookup(int voiceId) {
        if (voiceId == 0) return null;
        int base = VOICE_TABLE_BASE + voiceId * VOICE_TABLE_ENTRY_SIZE;
        int msf = readWord(base) & 0x00FFFFFF;
        int sectors = readByte(base + 3);
        int size = readWord(base + 4);
        return new VoiceTableEntry(msf, sectors, size);
    }

    /**
     * PB_VOICE_PLAY (v1.20): reusable subroutine, voice_id in a0. Loads VAG from CD
     * and streams to SPU voice 23 buffer at 0x60000. Skips the CD/SPU path on a
     * cache hit (loaded_voice_id == voice_id), only re-KONs the voice.
     * Native PATCH.BIN address: 0x801F11FC (428B).
     */
    public void pbVoicePlay(int voiceId) {
        VoiceTableEntry entry = voiceTableLookup(voiceId);
        if (entry == null || entry.sizeBytes == 0) {
            System.out.printf("  [VOICE] play(%d): no entry — skip%n", voiceId);
            return;
        }
        if (readByte(LOADED_VOICE_ID) == (voiceId & 0xFF)) {
            System.out.printf("  [VOICE] play(%d): already loaded — re-KON only%n", voiceId);
            spuKonVoice(SPU_VOICE_INDEX);
            return;
        }
        // CD path
        writeWord(VOICE_MSF_BUFFER, entry.msf);
        cdControlBSetLoc(entry.msf);
        cdRead(entry.sectors, SPU_VOICE_BUFFER);
        cdReadSync();
        // SPU path
        spuSetTransferMode();
        spuSetTransferStartAddr(SPU_VOICE_BUFFER);
        spuTransferRead(entry.sizeBytes);
        spuWaitTransferComplete();
        spuKonVoice(SPU_VOICE_INDEX);
        writeByte(LOADED_VOICE_ID, voiceId & 0xFF);
        System.out.printf(
            "  [VOICE] play(%d): MSF=0x%06X, %d sectors, %dB → SPU 0x%X, voice %d KON%n",
            voiceId, entry.msf, entry.sectors, entry.sizeBytes, SPU_VOICE_BUFFER, SPU_VOICE_INDEX);
    }

    /**
     * PB_OP2B_WRAPPER (v1.21): SCD dispatch[0x2B] redirect.
     * Reads voice_id from {@code script_ptr+2} (baked into RDT by
     * patch_rdt_voices_all), triggers PB_VOICE_PLAY when non-zero, then
     * tail-jumps to the room-overlay native handler 0x8006EA20.
     * Native PATCH.BIN address: 0x801F1438 (68B).
     */
    public void pbOp2bWrapper(int scriptPtr) {
        int opcode = readByte(scriptPtr);
        int msgId = readByte(scriptPtr + 1);
        int voiceId = readByte(scriptPtr + 2);
        // arg3 (scriptPtr + 3) is unused / 0
        if (opcode != SCD_OP2B) {
            throw new IllegalStateException("PB_OP2B_WRAPPER hit non-0x2B opcode: 0x" + Integer.toHexString(opcode));
        }
        if (voiceId != 0) {
            pbVoicePlay(voiceId);
        }
        nativeOp2bHandler_0x8006EA20(msgId);
    }

    /**
     * PB_MSG_TAIL (v1.21, dormant): Message_on (op21) hook. Receives voice_id in t0,
     * preserved through ITEM_BOX_CHECK's `j` from `lbu t0, 2(v0)`. If non-zero,
     * triggers PB_VOICE_PLAY, then tail-calls native Message_on at 0x800404F4.
     * Currently unused (ROOM1240 uses op2B, not op21); kept for future opcode-21 voices.
     * Native PATCH.BIN address: 0x801F13FC (60B).
     */
    public void pbMsgTail(int voiceId, int msgIndex, int msgValue) {
        if (voiceId != 0) {
            pbVoicePlay(voiceId);
        }
        nativeMessageOn_0x800404F4(msgIndex, msgValue);
    }

    /**
     * ITEM_BOX_CHECK voice_id passthrough (v1.21 rewrite).
     * Cave 2 @ 0x800708CC (36B / 9 instrs). Reads {@code lbu t0, 2(v0)} to capture
     * voice_id from the live op2B/op21 opcode, then preserves it through the
     * fall-through {@code j PB_MSG_TAIL}. msg_id (v0+1) routes to either:
     *   - 0xFD → ITEM_BOX_OPEN
     *   - other → PB_MSG_TAIL (which decides voice + Message_on)
     */
    public void itemBoxCheckVoicePassthrough(int scriptPtr) {
        int msgIndex = readByte(scriptPtr + 1);
        int voiceId = readByte(scriptPtr + 2);
        if (msgIndex == 0xFD) {
            itemBoxOpen_enterItemBoxMode();
            return;
        }
        pbMsgTail(voiceId, msgIndex, 0xFFFF);
    }

    /**
     * Build-time mirror of {@code patch_rdt_voices_all()}: bake voice_id into op2B
     * opcode arg2 inside the SCD region of an RDT. SCD bounds are read from the
     * RDT header (offsets 0x40/0x44/0x48 = main/sub/extra Scd start, 0x4C =
     * effectStart). For each {@code 2B NN 00 00} where NN matches an entry in
     * {@code voiceMap} (msg_id → voice_id), arg2 is rewritten to the voice_id.
     *
     * @return number of op2B sites patched
     */
    public static int bakeVoiceIdsIntoRdt(byte[] rdtBytes, java.util.Map<Integer, Integer> voiceMap) {
        if (rdtBytes.length < 0x60) return 0;
        int mainScd  = readLittleEndianInt(rdtBytes, 0x40);
        int subScd   = readLittleEndianInt(rdtBytes, 0x44);
        int extraScd = readLittleEndianInt(rdtBytes, 0x48);
        int effect   = readLittleEndianInt(rdtBytes, 0x4C);
        int scdStart = Integer.MAX_VALUE;
        for (int s : new int[]{mainScd, subScd, extraScd}) {
            if (s > 0 && s < rdtBytes.length) scdStart = Math.min(scdStart, s);
        }
        if (scdStart == Integer.MAX_VALUE) return 0;
        int scdEnd = (scdStart < effect && effect <= rdtBytes.length) ? effect : rdtBytes.length;
        int patched = 0;
        for (int off = scdStart; off + 3 < scdEnd; off++) {
            if ((rdtBytes[off] & 0xFF) == SCD_OP2B
                    && rdtBytes[off + 2] == 0
                    && rdtBytes[off + 3] == 0) {
                int msgId = rdtBytes[off + 1] & 0xFF;
                Integer voiceId = voiceMap.get(msgId);
                if (voiceId != null) {
                    rdtBytes[off + 2] = (byte) (voiceId & 0xFF);
                    patched++;
                }
            }
        }
        return patched;
    }

    private static int readLittleEndianInt(byte[] data, int offset) {
        return (data[offset] & 0xFF)
             | ((data[offset + 1] & 0xFF) << 8)
             | ((data[offset + 2] & 0xFF) << 16)
             | ((data[offset + 3] & 0xFF) << 24);
    }

    // --- Native CD/SPU function placeholders (real targets are libcd/libsnd entries) ---

    private void cdControlBSetLoc(int msf) {
        System.out.printf("  [CD] CdControlB(SetLoc, MSF=0x%06X)%n", msf);
    }
    private void cdRead(int sectors, int destAddr) {
        System.out.printf("  [CD] CdRead(%d sectors → 0x%08X)%n", sectors, destAddr);
    }
    private void cdReadSync() { /* poll until done */ }
    private void spuSetTransferMode() { /* mode = DMA */ }
    private void spuSetTransferStartAddr(int spuAddr) {
        System.out.printf("  [SPU] SetTransferStartAddr(0x%X)%n", spuAddr);
    }
    private void spuTransferRead(int sizeBytes) {
        System.out.printf("  [SPU] TransferRead(%dB)%n", sizeBytes);
    }
    private void spuWaitTransferComplete() { /* IsTransferCompleted poll */ }
    private void spuKonVoice(int voiceIndex) {
        System.out.printf("  [SPU] KON voice %d%n", voiceIndex);
    }
    private void nativeOp2bHandler_0x8006EA20(int msgIndex) {
        System.out.printf("  [NATIVE] 0x8006EA20: op2B text-display(msg=%d)%n", msgIndex);
    }
    private void nativeMessageOn_0x800404F4(int msgIndex, int msgValue) {
        System.out.printf("  [NATIVE] 0x800404F4: Message_on(msg=0x%02X, val=0x%04X)%n",
                msgIndex, msgValue & 0xFFFF);
    }

    // =========================================================================
    // DIAGNOSTICS / MAIN
    // =========================================================================

    /**
     * Run a simulated item box session to verify all patches work together.
     */
    public void runDiagnostics() {
        System.out.println("=== RE1.5 Patch Reference v1.21 — Diagnostics ===\n");

        // Setup: populate some inventory and box items
        setSlot(PLAYER_INVENTORY, 0, 0x01, 1, 0, 0);   // Knife
        setSlot(PLAYER_INVENTORY, 1, 0x04, 15, 0, 0);   // VP70 (15 rounds)
        setSlot(PLAYER_INVENTORY, 2, 0x0B, 50, 0, 0);   // Ammo
        setSlot(BOX_STORAGE, 0, 0x08, 6, 0, 0);         // Shotgun
        setSlot(BOX_STORAGE, 1, 0x09, 252, 0, 0);       // Shotgun shells

        System.out.println("--- 1. SAVE SYSTEM ---");
        extendedSnapshot_saveGameFlags();
        hpSave_capturePlayerState();
        weaponCapture_beforeSave();
        boxSave_writeToPostGsb();

        System.out.println("\n--- 2. ITEM BOX OPEN ---");
        patchLoader_perFrame();
        itemBoxOpen_enterItemBoxMode();

        System.out.println("\n--- 3. DEPOSIT (knife denied) ---");
        writeByte(STATE_25BD, 0);  // cursor on knife
        depositHook_autoDeposit();

        System.out.println("\n--- 4. DEPOSIT (VP70) ---");
        writeByte(STATE_25BD, 1);  // cursor on VP70
        depositHook_autoDeposit();
        System.out.printf("  Box items after deposit: %d%n",
                countItems(BOX_STORAGE, MAX_BOX_SLOTS));

        System.out.println("\n--- 5. NATIVE TEXT (per-frame render) ---");
        textDismissV2_perFrame();
        writeByte(MSG_FLAGS, 0);  // simulate text dismissed
        textDismissV2_perFrame();

        System.out.println("\n--- 6. TAKE MODE ---");
        takeHintNative_clearAndEnterTake();
        takeHandler_enterTakeView();
        takePageCheck_detectPageButton(0x0008);  // R1 press

        System.out.println("\n--- 7. WITHDRAW ---");
        writeByte(STATE_25BD, 0);
        withdraw_takeItemFromBox(0, 0);

        System.out.println("\n--- 8. CHECK MODE ---");
        takeRestoreAndRefresh();
        checkEnter_setCheckModeAndBrowse();
        writeByte(STATE_25BD, 0);
        checkAction_startCheckDisplay(0);

        System.out.println("\n--- 9. CHECK DISPLAY (3 frames) ---");
        for (int frame = 0; frame < 3; frame++) {
            System.out.printf("  -- Frame %d --%n", frame);
            checkState3_perFrameHandler();
        }

        System.out.println("\n--- 10. CHECK EXIT ---");
        writeByte(SAVED_PHASE, 4);
        writeHalfword(0x800BC762, 0x0000);
        checkState3_perFrameHandler();

        System.out.println("\n--- 11. MODEL RESTORE (v1.15: mode_flag at 0x800719DC) ---");
        // After item box close, MODEL_RESTORE runs in 2 stages
        writeByte(MODE_FLAG, 1);  // set by MODEL_SAVE_WRAPPER at item box open
        System.out.println("  mode_flag=" + readByte(MODE_FLAG) + " (should be 1)");
        System.out.println("  saved_phase=" + readByte(SAVED_PHASE) + " (independent, no collision)");
        modelRestore_restoreAnimationTable();  // stage 1: clear state[0], mode_flag→2
        modelRestore_restoreAnimationTable();  // stage 2: restore table, mode_flag→0

        System.out.println("\n--- 12. WITHDRAW DUPLICATION TEST ---");
        // Deposit 2 items, then withdraw both — verify no duplication
        takeHintNative_clearAndEnterTake();
        takeHandler_enterTakeView();
        int boxBefore = countItems(BOX_STORAGE, MAX_BOX_SLOTS);
        withdraw_takeItemFromBox(0, 0);
        withdraw_takeItemFromBox(0, 0);  // cursor 0 again (items shifted by compact)
        int boxAfter = countItems(BOX_STORAGE, MAX_BOX_SLOTS);
        System.out.printf("  BOX items: %d before, %d after (should be %d)%n",
                boxBefore, boxAfter, Math.max(0, boxBefore - 2));
        assert boxAfter == Math.max(0, boxBefore - 2) : "Item duplication detected!";

        System.out.println("\n--- 13. v1.20+v1.21 VOICE SUBSYSTEM ---");
        // Seed a fake VOICE_TABLE entry for voice_id=1 (ROOM1240 main00):
        //   MSF=0x162026 (LBA 73388), 18 sectors, 35440B body
        int voice1Base = VOICE_TABLE_BASE + 1 * VOICE_TABLE_ENTRY_SIZE;
        writeWord(voice1Base, 0x00162026);
        writeByte(voice1Base + 3, 18);
        writeWord(voice1Base + 4, 35440);
        writeByte(LOADED_VOICE_ID, 0xFF);  // sentinel: nothing loaded yet
        VoiceTableEntry e = voiceTableLookup(1);
        System.out.printf("  voiceTableLookup(1): MSF=0x%06X, %d sectors, %dB%n",
                e.msf, e.sectors, e.sizeBytes);
        // First call: cold path (CD + SPU upload)
        pbVoicePlay(1);
        // Second call: warm path (already-loaded fast path)
        pbVoicePlay(1);

        // Simulate ROOM1240@0x057E: opcode 0x2B, msg 0, voice 1 baked into arg2
        int fakeScd = 0x800B0000;       // anywhere in RAM for the simulation
        writeByte(fakeScd, SCD_OP2B);
        writeByte(fakeScd + 1, 0);      // msg_id 0 (main00)
        writeByte(fakeScd + 2, 1);      // voice_id 1 (baked by patch_rdt_voices_all)
        writeByte(fakeScd + 3, 0);
        pbOp2bWrapper(fakeScd);

        // Simulate ITEM_BOX_CHECK voice_id passthrough on op2B path:
        //   msg_id != 0xFD → falls through to PB_MSG_TAIL → Message_on (with voice retrigger)
        itemBoxCheckVoicePassthrough(fakeScd);

        // Build-time RDT baking simulation: synthesize a tiny RDT with one op2B at 0x80
        byte[] fakeRdt = new byte[0x100];
        // Header: mainScdStart=0x60, subScd=0, extraScd=0, effectStart=0xC0
        fakeRdt[0x40] = 0x60;
        fakeRdt[0x4C] = (byte) 0xC0;
        // SCD content @ 0x80: 2B 00 00 00 (op2B, msg 0, arg2 unset, arg3 0)
        fakeRdt[0x80] = (byte) SCD_OP2B;
        fakeRdt[0x81] = 0;
        fakeRdt[0x82] = 0;
        fakeRdt[0x83] = 0;
        java.util.Map<Integer, Integer> voiceMap = java.util.Map.of(0, 1);  // msg 0 → voice 1
        int patchedCount = bakeVoiceIdsIntoRdt(fakeRdt, voiceMap);
        System.out.printf("  bakeVoiceIdsIntoRdt: patched %d site(s); arg2 @0x82 = 0x%02X (expect 0x01)%n",
                patchedCount, fakeRdt[0x82] & 0xFF);
        assert patchedCount == 1 && fakeRdt[0x82] == 1 : "RDT op2B baking failed";

        System.out.println("\n--- 14. v1.15 ADDRESS SAFETY VERIFICATION ---");
        System.out.printf("  ITEMALL.PIX buffer:   0x%08X (save buffer, NOT 0x80100000)%n", ITEMALL_PIX_BUFFER);
        System.out.printf("  MODE_FLAG address:    0x%08X (Cave 2, NOT 0x800719D1)%n", MODE_FLAG);
        System.out.printf("  SAVED_EQUIPPED addr:  0x%08X (Cave 2, NOT 0x800719D2)%n", SAVED_EQUIPPED);
        System.out.printf("  SAVED_PHASE address:  0x%08X (separate from MODE_FLAG)%n", SAVED_PHASE);
        assert MODE_FLAG != SAVED_PHASE : "MODE_FLAG and SAVED_PHASE must not share address!";
        assert SAVED_EQUIPPED != CHECK_MODE : "SAVED_EQUIPPED and CHECK_MODE must not share address!";
        assert ITEMALL_PIX_BUFFER != 0x80100000 : "ITEMALL.PIX must not load to stage data area!";
        System.out.println("  OK: No address collisions");

        System.out.println("\n=== All diagnostics complete ===");
    }

    public static void main(String[] args) {
        new PatchReference().runDiagnostics();
    }
}
