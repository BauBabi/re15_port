/* FUN_8010d428 @ 0x8010d428  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d428(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0xb4;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 0x40;
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((uint)*(byte *)(_DAT_800ac784 + 0x95) % 0xd == 1) {
    func_0x800453d0(8);
  }
  func_0x800245d8(0);
  return;
}


