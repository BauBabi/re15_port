/* FUN_80108ad0 @ 0x80108ad0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108ad0(void)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 3) == 0) {
      uVar2 = func_0x8001af20();
      uVar3 = 5;
      if ((uVar2 & 1) != 0) {
        uVar3 = 4;
      }
      func_0x800453d0(uVar3);
    }
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}


