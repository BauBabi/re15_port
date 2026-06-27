/* FUN_8010dbf8 @ 0x8010dbf8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dbf8(int param_1)

{
  char cVar1;
  int in_v0;
  
  DAT_800aca59 = 0;
  DAT_800aca5a = 0;
  _DAT_800aca54 = *(uint *)(in_v0 + -0x35ac) | 0x1000;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(param_1 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}


