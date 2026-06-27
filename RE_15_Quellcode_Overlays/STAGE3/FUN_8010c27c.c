/* FUN_8010c27c @ 0x8010c27c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c27c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  short unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      iVar3 = 2;
      if (bVar1 == 2) {
        if (*(char *)(_DAT_800ac784 + 7) == '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 2;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        iVar3 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      }
      goto LAB_8010cb94;
    }
    iVar3 = 1;
    if (bVar1 != 0) goto LAB_8010cb94;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar3;
LAB_8010cb94:
  if (0x3c < iVar3) {
    func_0x8001a8f8(&DAT_800aca88,0x40);
  }
  if (*(short *)(_DAT_800ac784 + 0x1d0) == 0) {
    cVar2 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar2;
    if (cVar2 == '\x06') {
      FUN_80110a4c(0x1e,200);
    }
    if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010d504();
      return;
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
    *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  }
  FUN_80110aac((int)unaff_s1);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar4 + (uVar4 / 7 + (uVar4 - uVar4 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}


