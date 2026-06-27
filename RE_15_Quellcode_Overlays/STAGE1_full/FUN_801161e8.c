/* FUN_801161e8 @ 0x801161e8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161e8(void)

{
  short sVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  sVar1 = *(short *)(_DAT_800ac784 + 0x1ec);
  if (sVar1 < 0x5dc) {
    func_0x80037edc(1,10);
    uVar2 = 2;
    uVar3 = 10;
  }
  else {
    if (sVar1 < 3000) {
      func_0x80037edc(3,10);
      func_0x80037edc(4,0x28);
      func_0x80037edc(6,0x28);
      uVar2 = 5;
    }
    else {
      if (0xe0f < sVar1) {
        return;
      }
      func_0x80037edc(0,8);
      uVar2 = 7;
    }
    uVar3 = 0x28;
  }
  func_0x80037edc(uVar2,uVar3);
  return;
}


