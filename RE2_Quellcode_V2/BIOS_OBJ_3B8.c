/* BIOS_OBJ_3B8 @ 0x800877e4  (Ghidra headless, raw MIPS overlay) */

void BIOS_OBJ_3B8(void)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  int unaff_s0;
  int unaff_s1;
  
  DAT_800ad0cd = 1;
  if (unaff_s1 != 0) {
    if (unaff_s0 == 1) {
      unaff_s1 = 0;
    }
    DAT_800ad0cd = 1;
    if (unaff_s1 != 0) {
      DAT_800ad0cd = 5;
    }
  }
  puVar2 = &DAT_800c3d48;
  puVar4 = &stack0x00000018;
  iVar3 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar3 = iVar3 + -1;
    *puVar2 = uVar1;
    puVar2 = puVar2 + 1;
  } while (iVar3 != -1);
  *PTR_CDROM_REG0_800ad0b4 = 0;
  *PTR_CDROM_REG3_800ad0c0 = 0;
  BIOS_OBJ_570();
  return;
}


