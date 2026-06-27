/* DS_init @ 0x8008ae68  (Ghidra headless, raw MIPS overlay) */

void DS_init(void)

{
  CD_init();
  CD_initvol();
  DAT_800c3f10 = 0;
  DAT_800c3f0c = 0;
  DAT_800c3f08 = 0;
  DS_reset_members();
  DsReadMode(0);
  DAT_800acdec = DS_sync_system;
  DAT_800acdf0 = DS_ready_system;
  VSyncCallbacks(0,DS_vsync_system);
  DAT_800ace10 = 1;
  DAT_800ad238 = 1;
  return;
}


