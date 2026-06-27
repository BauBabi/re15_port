/* DsReset @ 0x8008a254  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int DsReset(void)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  
  DS_stop();
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
    iVar3 = 0;
  } while (iVar1 < 8);
  puVar2 = &DAT_800c3db8;
  do {
    CQ_clear_queue(puVar2);
    iVar3 = iVar3 + 1;
    puVar2 = puVar2 + 6;
  } while (iVar3 < 8);
  iVar1 = 0x70;
  DAT_800c3e7c = 0;
  DAT_800c3e78 = 0;
  DAT_800c3e80 = 0;
  do {
    *(undefined4 *)((int)&DAT_800c3e84 + iVar1) = 0;
    iVar1 = iVar1 + -0x10;
  } while (-1 < iVar1);
  DAT_800c3f04 = 0;
  DS_restart();
  return 1;
}


