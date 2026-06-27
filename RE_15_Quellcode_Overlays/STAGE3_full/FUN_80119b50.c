/* FUN_80119b50 @ 0x80119b50  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119b50(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1d4) = 0xaa10;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xd120;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x60
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(uint *)(_DAT_800ac784 + 0x1d0) < 500) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  FUN_8011b5c4(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}


