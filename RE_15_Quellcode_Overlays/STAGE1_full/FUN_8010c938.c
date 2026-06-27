/* FUN_8010c938 @ 0x8010c938  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c938(void)

{
  short sVar1;
  int iVar2;
  byte bVar3;
  char cVar4;
  ushort uVar5;
  uint uVar6;
  undefined4 uVar7;
  
  bVar3 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar3 == 1) {
LAB_8010c990:
    bVar3 = func_0x8001af20();
    iVar2 = _DAT_800ac784;
    cVar4 = (bVar3 & 3) + 1;
    *(char *)(_DAT_800ac784 + 0x9f) = cVar4;
    *(char *)(iVar2 + 0x9e) = cVar4;
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x7f) + 0x32;
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar6 = func_0x8001af20();
    if ((uVar6 & 1) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    }
  }
  else {
    if (bVar3 < 2) {
      if (bVar3 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      goto LAB_8010c990;
    }
    if (bVar3 != 2) {
      if (bVar3 != 3) {
        return;
      }
      cVar4 = *(char *)(_DAT_800ac784 + 0x9f);
      *(char *)(_DAT_800ac784 + 0x9f) = cVar4 + -1;
      if (cVar4 == '\0') {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        sVar1 = *(short *)(_DAT_800ac784 + 0x1d0);
        *(short *)(_DAT_800ac784 + 0x1d0) = sVar1 + -1;
        if (sVar1 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 3;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          uVar5 = func_0x8001af20();
          *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar5 & 0x1f) + 0x1e;
        }
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      uVar7 = 0;
      goto LAB_8010cb14;
    }
  }
  cVar4 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar4 + -1;
  if (cVar4 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar7 = 0x800;
LAB_8010cb14:
  func_0x800245d8(uVar7);
  return;
}


