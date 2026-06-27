/* FUN_801125dc @ 0x801125dc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801125dc(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x82;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar3 + (uVar3 / 7 + (uVar3 - uVar3 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(5);
  }
  FUN_80115bec(0,6 < *(byte *)(_DAT_800ac784 + 0x95) - 2);
  return;
}


