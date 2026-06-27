/* BIOS_OBJ_36C @ 0x80087798  (Ghidra headless, raw MIPS overlay) */

void BIOS_OBJ_36C(void)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  int unaff_s1;
  
  DAT_800ad0cc = 2;
  if (unaff_s1 != 0) {
    DAT_800ad0cc = 5;
  }
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


