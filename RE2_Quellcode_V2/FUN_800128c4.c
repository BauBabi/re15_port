/* FUN_800128c4 @ 0x800128c4  (Ghidra headless, raw MIPS overlay) */

void FUN_800128c4(void)

{
  int iVar1;
  int iVar2;
  undefined2 *puVar3;
  
  iVar1 = 0xc;
  puVar3 = &DAT_800d5318;
  do {
    iVar2 = iVar1 + -1;
    (&DAT_800d5b5b)[iVar1] = 0;
    puVar3[0x433] = 0;
    iVar1 = iVar2;
    puVar3 = puVar3 + -1;
  } while (iVar2 != 0);
  do {
    iVar1 = DsInit();
  } while (iVar1 == 0);
  DAT_800cfbd8 = DAT_800cfbd8 & 0xffffffdf;
  DsSetDebug(0);
  DsReadyCallback((DslCB)0x0);
  DsSyncCallback((DslCB)0x0);
  do {
    iVar1 = DsCommand('\x0e',&DAT_8009a414,(DslCB)0x0,-1);
  } while (iVar1 == 0);
  DAT_800d5300 = 0;
  DAT_800d5301 = 0;
  DAT_800d5334 = 0;
  DAT_800d5b50 = 0x78;
  FUN_80013a7c(1);
  return;
}


