/* FUN_80052e7c @ 0x80052e7c  (Ghidra headless, raw MIPS overlay) */

void FUN_80052e7c(void)

{
  byte bVar1;
  undefined4 *puVar2;
  uint *puVar3;
  
  puVar2 = &DAT_800cfe1c;
  DAT_800cfc02 = 0xffff;
  if (DAT_800ce334 != &DAT_800cfe1c) {
    do {
      if ((*(uint *)*puVar2 & 1) != 0) {
        *(undefined2 *)((int)*puVar2 + 10) = 0xffff;
      }
      puVar2 = puVar2 + 1;
    } while (puVar2 != DAT_800ce334);
  }
  bVar1 = *(byte *)(DAT_800ce324 + 2);
  for (puVar3 = &DAT_800d0324; puVar3 < &DAT_800d0324 + (uint)bVar1 * 0x7e; puVar3 = puVar3 + 0x7e)
  {
    if ((*puVar3 & 1) != 0) {
      *(undefined2 *)((int)puVar3 + 10) = 0xffff;
    }
  }
  return;
}


