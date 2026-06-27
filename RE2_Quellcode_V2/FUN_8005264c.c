/* FUN_8005264c @ 0x8005264c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005264c(void)

{
  byte bVar1;
  uint *puVar2;
  undefined4 *puVar3;
  
  if (-1 < DAT_800cfd4e) {
    DAT_800cfd04 = 0;
    if ((DAT_800cfbd8 & 0x200000) != 0) {
      FUN_80051088(&DAT_800cfbf8,1,0x10);
    }
    FUN_80051088(&DAT_800cfbf8,1,0);
    puVar3 = &DAT_800cfe1c;
    if (DAT_800ce334 != &DAT_800cfe1c) {
      do {
        puVar2 = (uint *)*puVar3;
        if ((*puVar2 & 1) != 0) {
          *(undefined2 *)(puVar2 + 0x43) = 0;
          FUN_80051088(puVar2,2,0);
        }
        puVar3 = puVar3 + 1;
      } while (puVar3 != DAT_800ce334);
    }
    bVar1 = *(byte *)(DAT_800ce324 + 2);
    for (puVar2 = &DAT_800d0324; puVar2 < &DAT_800d0324 + (uint)bVar1 * 0x7e; puVar2 = puVar2 + 0x7e
        ) {
      if ((*puVar2 & 1) != 0) {
        *(undefined2 *)(puVar2 + 0x43) = 0;
        FUN_80051088(puVar2,4,0);
      }
    }
    if ((*DAT_800cfe18 & 1) != 0) {
      *(undefined2 *)(DAT_800cfe18 + 0x43) = 0;
      FUN_80051088(DAT_800cfe18,8,0);
    }
  }
  return;
}


