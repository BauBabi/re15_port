/* FUN_80111a4c @ 0x80111a4c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111a4c(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
  _DAT_800aca50 = func_0x8001b768(0x21);
  _DAT_800aca50 = _DAT_800aca50 << 4;
  *(undefined2 *)(_DAT_800ac784 + 0xa0) = *(undefined2 *)(_DAT_800ac784 + 0x34);
  *(undefined2 *)(_DAT_800ac784 + 0xa2) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
  FUN_80115d94(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined4 *)(_DAT_800ac784 + 0x78) = DAT_80121108;
  func_0x8001af5c(0,0,400,400,_DAT_800ac784 + 0xb0,0xa0a0a0);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    *(undefined4 *)(uVar1 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x1ea) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -400;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x1db) = 0;
    FUN_801123ec();
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x1db) = 1;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 4;
  }
  return;
}


