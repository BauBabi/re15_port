/* FUN_80037eac @ 0x80037eac  (Ghidra headless, raw MIPS overlay) */

uint * FUN_80037eac(int param_1)

{
  int iVar1;
  uint *puVar2;
  
  puVar2 = &DAT_800d0324;
  if (&DAT_800d0324 < DAT_800d4224) {
    do {
      if (((((*puVar2 & 0x101) == 0x101) &&
           (*(char *)(param_1 + 0x106) == *(char *)((int)puVar2 + 0x106))) &&
          (*(uint **)(param_1 + 0x1e4) != puVar2)) &&
         (iVar1 = FUN_80037d48(param_1,puVar2), iVar1 != 0)) {
        return puVar2;
      }
      puVar2 = puVar2 + 0x7e;
    } while (puVar2 < DAT_800d4224);
  }
  return (uint *)0x0;
}


