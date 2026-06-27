/* BIOS_OBJ_43C @ 0x80087868  (Ghidra headless, raw MIPS overlay) */

void BIOS_OBJ_43C(void)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  puVar3 = &DAT_800c3d50;
  DAT_800ad0ce = 4;
  puVar4 = &stack0x00000018;
  DAT_800ad0cd = 4;
  iVar2 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar2 = iVar2 + -1;
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != -1);
  puVar3 = &DAT_800c3d48;
  puVar4 = &stack0x00000018;
  iVar2 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar2 = iVar2 + -1;
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != -1);
  BIOS_OBJ_570();
  return;
}


