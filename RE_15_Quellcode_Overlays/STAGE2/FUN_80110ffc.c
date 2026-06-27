/* FUN_80110ffc @ 0x80110ffc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110ffc(void)

{
  short sVar1;
  undefined4 uVar2;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(ushort *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + (*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x3c;
  uVar2 = 0;
  if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
    uVar2 = 0x800;
  }
  func_0x800245d8(uVar2);
  return;
}


