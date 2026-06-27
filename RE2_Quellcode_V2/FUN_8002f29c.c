/* FUN_8002f29c @ 0x8002f29c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002f29c(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  uint *puVar2;
  undefined4 *puVar3;
  
  FUN_8002f3fc();
  if ((*DAT_800cfe18 & 1) != 0) {
    FUN_8002f3fc(param_1,param_2,param_3);
  }
  bVar1 = *(byte *)(DAT_800ce324 + 2);
  for (puVar2 = &DAT_800d0324; puVar2 < &DAT_800d0324 + (uint)bVar1 * 0x7e; puVar2 = puVar2 + 0x7e)
  {
    if (((*puVar2 & 1) != 0) && ((char)puVar2[2] == '\x02')) {
      FUN_8002f3fc(param_1,param_2,param_3,puVar2);
    }
  }
  puVar3 = &DAT_800cfe1c;
  if (DAT_800ce334 != &DAT_800cfe1c) {
    do {
      if ((*(uint *)*puVar3 & 1) != 0) {
        FUN_8002f3fc(param_1,param_2,param_3,(uint *)*puVar3);
      }
      puVar3 = puVar3 + 1;
    } while (puVar3 != DAT_800ce334);
  }
  return;
}


