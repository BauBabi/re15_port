/* FUN_8010ded4 @ 0x8010ded4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ded4(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  short sVar5;
  uint uVar6;
  
  FUN_80111f0c(0x14,0xf0);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 0xf0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0xf) + 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  if (((*(short *)(_DAT_800ac784 + 0x1dc) == 0) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) &&
     (0xef < *(short *)(_DAT_800ac784 + 0x8c))) {
    func_0x8001a9cc(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    FUN_8010e87c();
    return;
  }
  sVar4 = func_0x8001a9cc(&DAT_800aca88,0x10);
  if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
    sVar4 = func_0x8001a9cc(&DAT_800aca88,0x40);
  }
  *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
     (sVar5 = *(short *)(_DAT_800ac784 + 0x9c) + 1, *(short *)(_DAT_800ac784 + 0x9c) = sVar5,
     0x3c < sVar5)) {
    func_0x8001a8f8(&DAT_800aca88,0x40);
  }
  if (*(short *)(_DAT_800ac784 + 0x1d0) == 0) {
    cVar2 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar2;
    if (cVar2 == '\x06') {
      FUN_80111f6c(0x1e,200);
    }
    if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010ea24();
      return;
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
    *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  }
  FUN_80111fcc((int)sVar4);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar6 + (uVar6 / 7 + (uVar6 - uVar6 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}


