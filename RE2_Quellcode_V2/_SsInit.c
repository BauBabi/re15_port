/* _SsInit @ 0x8007a344  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void _SsInit(void)

{
  undefined2 uVar1;
  word wVar2;
  undefined4 *puVar3;
  undefined2 *puVar4;
  word *pwVar5;
  undefined *puVar6;
  int iVar7;
  int iVar8;
  dword *pdVar9;
  word *pwVar10;
  
  pdVar9 = &VOICE_00_LEFT_RIGHT;
  iVar7 = 0;
  do {
    iVar8 = 0;
    puVar4 = &DAT_800ab9f8;
    do {
      uVar1 = *puVar4;
      puVar4 = puVar4 + 1;
      iVar8 = iVar8 + 1;
      *(undefined2 *)pdVar9 = uVar1;
      pdVar9 = (dword *)((int)pdVar9 + 2);
    } while (iVar8 < 8);
    iVar7 = iVar7 + 1;
  } while (iVar7 < 0x18);
  pwVar10 = &SPU_MAIN_VOL_L;
  iVar7 = 0;
  pwVar5 = &DAT_800aba08;
  do {
    wVar2 = *pwVar5;
    pwVar5 = pwVar5 + 1;
    iVar7 = iVar7 + 1;
    *pwVar10 = wVar2;
    pwVar10 = pwVar10 + 1;
  } while (iVar7 < 0x10);
  FUN_8008239c(0x18);
  iVar7 = 0;
  puVar6 = &UNK_800ea2d0;
  do {
    iVar8 = 0xf;
    puVar3 = (undefined4 *)(puVar6 + 0x3c);
    do {
      *puVar3 = 0;
      iVar8 = iVar8 + -1;
      puVar3 = puVar3 + -1;
    } while (-1 < iVar8);
    iVar7 = iVar7 + 1;
    puVar6 = puVar6 + 0x40;
  } while (iVar7 < 0x20);
  DAT_800ea228 = 0x3c;
  DAT_800dcbd4 = 0;
  DAT_800d7658 = 0;
  return;
}


