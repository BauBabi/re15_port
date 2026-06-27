/* BIOS_OBJ_26C @ 0x80087698  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800877dc) */

void BIOS_OBJ_26C(void)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  int unaff_s1;
  
  if (unaff_s1 != 0) {
    DAT_800ad0cc = 5;
    puVar2 = &DAT_800c3d40;
    puVar4 = &stack0x00000018;
    iVar3 = 7;
    do {
      uVar1 = *puVar4;
      puVar4 = puVar4 + 1;
      iVar3 = iVar3 + -1;
      *puVar2 = uVar1;
      puVar2 = puVar2 + 1;
    } while (iVar3 != -1);
    BIOS_OBJ_570();
    return;
  }
  if (*(int *)(&DAT_800aceb4 + (uint)DAT_800ace0d * 4) != 0) {
    DAT_800ad0cc = 3;
    puVar2 = &DAT_800c3d40;
    puVar4 = &stack0x00000018;
    iVar3 = 7;
    do {
      uVar1 = *puVar4;
      puVar4 = puVar4 + 1;
      iVar3 = iVar3 + -1;
      *puVar2 = uVar1;
      puVar2 = puVar2 + 1;
    } while (iVar3 != -1);
    BIOS_OBJ_570();
    return;
  }
  DAT_800ad0cc = 2;
  puVar2 = &DAT_800c3d40;
  puVar4 = &stack0x00000018;
  iVar3 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar3 = iVar3 + -1;
    *puVar2 = uVar1;
    puVar2 = puVar2 + 1;
  } while (iVar3 != -1);
  BIOS_OBJ_570();
  return;
}


