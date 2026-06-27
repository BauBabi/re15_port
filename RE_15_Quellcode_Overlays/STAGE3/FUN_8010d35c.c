/* FUN_8010d35c @ 0x8010d35c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d35c(void)

{
  char cVar1;
  ushort in_v0;
  int iVar2;
  undefined8 uVar3;
  
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (in_v0 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0xd) {
    FUN_8010dbf8(0,1);
    return;
  }
  FUN_80110350(0,0);
  iVar2 = 0x800;
  uVar3 = func_0x800245d8(0x800);
  _DAT_800acbcc = (undefined4)uVar3;
  _DAT_800acbd0 = *(undefined4 *)((int)((ulonglong)uVar3 >> 0x20) + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(iVar2 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}


