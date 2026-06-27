/* DsFlush @ 0x8008ab74  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void DsFlush(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  
  DS_stop();
  iVar4 = 0;
  puVar3 = &DAT_800c3db8;
  DAT_800c3e80 = 0;
  DAT_800c3e7c = 0;
  DAT_800c3e78 = 0;
  do {
    iVar2 = 3;
    iVar1 = (int)puVar3 + 3;
    *puVar3 = 0;
    *(undefined1 *)(puVar3 + 1) = 0;
    do {
      *(undefined1 *)(iVar1 + 5) = 0;
      iVar2 = iVar2 + -1;
      iVar1 = iVar1 + -1;
    } while (-1 < iVar2);
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
    iVar4 = iVar4 + 1;
    puVar3 = puVar3 + 6;
  } while (iVar4 < 8);
  DsEndReadySystem();
  DS_restart();
  return;
}


