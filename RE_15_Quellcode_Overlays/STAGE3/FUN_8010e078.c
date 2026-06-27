/* FUN_8010e078 @ 0x8010e078  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e078(int param_1)

{
  undefined4 in_v0;
  int iVar1;
  int iVar2;
  int in_v1;
  
  _DAT_800acbd0 = *(undefined4 *)(in_v1 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  _DAT_800acbcc = in_v0;
  func_0x8001a8f8(param_1 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar1;
  iVar2 = func_0x8001f314(iVar2,*(undefined4 *)(iVar1 + 0x16c));
  if (iVar2 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


