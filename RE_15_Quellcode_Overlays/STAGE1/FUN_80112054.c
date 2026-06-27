/* FUN_80112054 @ 0x80112054  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112054(int param_1)

{
  undefined1 uVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar2 == 0) {
    FUN_80115d94(1);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar2 != 1) {
    *(undefined1 *)(uVar2 + 0x1d5) = 1;
    if (param_1 == 0) {
      FUN_80115d74(8);
    }
    func_0x800245d8(0);
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  return;
}


