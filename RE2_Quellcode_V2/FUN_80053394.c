/* FUN_80053394 @ 0x80053394  (Ghidra headless, raw MIPS overlay) */

void FUN_80053394(void)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  
  bVar1 = *(byte *)(DAT_800ce324 + 2);
  uVar3 = 0;
  if (bVar1 != 0) {
    iVar4 = 0;
    do {
      uVar3 = uVar3 + 1;
      if (((*(uint *)((int)&DAT_800d0324 + iVar4) & 1) != 0) &&
         (((&DAT_800d0469)[iVar4] & 0x80) != 0)) {
        uVar2 = ((byte)(&DAT_800d0469)[iVar4] & 0x60) >> 5;
        local_20 = 0;
        local_1e = 0xffa6;
        local_1c = 0;
        if (uVar2 == 0) {
          uVar2 = 1;
        }
        FUN_8001bf10((uVar2 * 8 + 7) * 0x10000 | 0x5002800,0,&DAT_800d0348 + iVar4,&local_20);
      }
      iVar4 = iVar4 + 0x1f8;
    } while (uVar3 < bVar1);
  }
  return;
}


