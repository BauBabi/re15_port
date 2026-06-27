/* FUN_80113e90 @ 0x80113e90  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e90(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  byte bVar4;
  char cVar5;
  ushort uVar6;
  undefined2 uVar7;
  byte bVar8;
  int iVar9;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar2 = false;
  bVar3 = false;
  bVar4 = *(byte *)(_DAT_800ac784 + 6);
  bVar8 = 6;
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar4 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x800453d0(9);
  }
  uVar6 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar6 & 0x3f) + 100;
  bVar4 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar4 & 0x10) + 0x40;
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 0xd) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x28 < 0xd)) {
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  cVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar5;
  FUN_80115c3c(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x28);
  if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    iVar9 = *(int *)(_DAT_800ac784 + 0x188);
    local_28 = _DAT_80072d60;
    local_24 = _DAT_80072d64;
    local_20 = _DAT_80072d68;
    local_1c = _DAT_80072d6c;
    bVar4 = 5;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar4 + 0x8011ed5c)) {
        uVar7 = func_0x8001bff8(iVar9 + 0x448,&local_28,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar7;
        if (*(short *)(_DAT_800ac784 + 0x1d8) != 0) {
          bVar2 = true;
        }
      }
      bVar1 = bVar4 != 0;
      bVar4 = bVar4 - 1;
    } while (bVar1);
    while (bVar1 = bVar8 != 0, bVar8 = bVar8 - 1, bVar1) {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar8 + 0x8011ed64)) {
        uVar7 = func_0x8001bff8(iVar9 + 0x6f8,&local_28,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar7;
        if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
          bVar3 = true;
        }
      }
    }
    if ((bVar2) || (bVar3)) {
      _DAT_800acaee = _DAT_800acaee + -10;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
      DAT_800aca58 = 2;
      cVar5 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar5 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
    }
  }
  return;
}


