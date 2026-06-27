/* FUN_8010d29c @ 0x8010d29c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d29c(void)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1b')) {
    func_0x800453d0(6);
  }
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar3 != 1) {
    iVar4 = _DAT_800ac784;
    if (1 < uVar3) {
      uVar5 = CONCAT44(uVar3,3);
      if (uVar3 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 3;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        return;
      }
      goto code_r0x8010dc28;
    }
    uVar5 = CONCAT44(uVar3,1);
    if (uVar3 != 0) goto code_r0x8010dc28;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar2 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0xd) {
    FUN_8010dbf8(0,1);
    return;
  }
  FUN_80110350(0,0);
  iVar4 = 0x800;
  uVar5 = func_0x800245d8(0x800);
code_r0x8010dc28:
  _DAT_800acbcc = (undefined4)uVar5;
  _DAT_800acbd0 = *(undefined4 *)((int)((ulonglong)uVar5 >> 0x20) + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(iVar4 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}


