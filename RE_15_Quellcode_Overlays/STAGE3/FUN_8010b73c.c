/* FUN_8010b73c @ 0x8010b73c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b73c(void)

{
  int iVar1;
  short sVar2;
  uint uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x10
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x100);
  iVar1 = _DAT_800ac784;
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) == 0) {
        FUN_8010c174(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      if (*(short *)(_DAT_800ac784 + 0x1da) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
        return;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x1dc) + 1;
      *(short *)(_DAT_800ac784 + 0x1dc) = sVar2;
      *(char *)(iVar1 + 5) = (char)sVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      if ((DAT_800acae7 != '\0') && (*(undefined1 *)(_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32))
      {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        uVar3 = func_0x8001af20(0,1);
        if ((uVar3 & 1) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
          FUN_8010d1a4();
          return;
        }
      }
      return;
    }
  }
  return;
}


