/* DsInit @ 0x8008a110  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int DsInit(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  
  iVar1 = DS_system_active();
  iVar2 = 0;
  if (iVar1 == 0) {
    iVar1 = 0;
    DAT_800dcca8 = 0;
    DAT_800dccac = 0;
    DAT_800c3da8 = 0;
    DAT_800c3d98 = 0;
    DAT_800c3d88 = 0;
    DAT_800c3dac = 0;
    DAT_800c3d9c = 0;
    DAT_800c3d8c = 0;
    do {
      (&DAT_800c3d8d)[iVar1] = 0;
      (&DAT_800c3d9d)[iVar1] = 0;
      (&DAT_800c3dad)[iVar1] = 0;
      iVar1 = iVar1 + 1;
      iVar2 = 0;
    } while (iVar1 < 8);
    puVar3 = &DAT_800c3db8;
    do {
      CQ_clear_queue(puVar3);
      iVar2 = iVar2 + 1;
      puVar3 = puVar3 + 6;
    } while (iVar2 < 8);
    iVar1 = 0x70;
    DAT_800c3e7c = 0;
    DAT_800c3e78 = 0;
    DAT_800c3e80 = 0;
    do {
      *(undefined4 *)((int)&DAT_800c3e84 + iVar1) = 0;
      iVar1 = iVar1 + -0x10;
    } while (-1 < iVar1);
    DAT_800c3f04 = 0;
    DS_init();
    DS_sync_callback(CQ_sync_system);
    DS_ready_callback(CQ_ready_system);
    DS_vsync_callback(CQ_vsync_system);
    iVar2 = 1;
  }
  return iVar2;
}


