/* FUN_80111844 @ 0x80111844  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111844(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  byte bVar5;
  undefined2 uVar6;
  int extraout_v1;
  int iVar7;
  ushort uVar8;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      bVar5 = 2;
      if (bVar2 == 2) {
        if ((DAT_800acae7 == 0) && (iVar7 = func_0x8001a804(4000,0x180,&DAT_800aca88), iVar7 < 0)) {
          if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
            *(undefined1 *)(_DAT_800ac784 + 7) = 0;
            bVar5 = 2;
            if (-1 < _DAT_800acaee) {
              return;
            }
            goto LAB_801123d0;
          }
        }
        else if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 3;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_801123d0;
    }
    bVar5 = 1;
    if (bVar2 != 0) goto LAB_801123d0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_80115c3c(0,0);
    *(undefined1 *)(extraout_v1 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0();
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    return;
  }
  FUN_80115b68(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 != 0) {
    return;
  }
  iVar7 = *(int *)(_DAT_800ac784 + 0x188);
  local_28 = _DAT_80072d60;
  local_24 = _DAT_80072d64;
  local_20 = _DAT_80072d68;
  local_1c = _DAT_80072d6c;
  uVar6 = func_0x8001bff8(iVar7 + 0x448,&local_28,800,&DAT_800aca88);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
  uVar6 = func_0x8001bff8(iVar7 + 0x6f8,&local_28,800,&DAT_800aca88);
  uVar8 = 5;
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&LAB_8011ed4c)[uVar8]) {
      bVar3 = true;
    }
    bVar1 = uVar8 != 0;
    uVar8 = uVar8 - 1;
  } while (bVar1);
  if (!bVar3) {
    return;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d8) == 0) && (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
    return;
  }
  _DAT_800acaee = _DAT_800acaee + -10;
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
  func_0x800453d0(4);
  DAT_800aca58 = 2;
  cVar4 = func_0x8001a780(&DAT_800aca54);
  DAT_800aca59 = cVar4 + '\x04';
  DAT_800aca5a = 0;
  if (_DAT_800acaee < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  bVar5 = DAT_800acae7;
LAB_801123d0:
  *(byte *)(_DAT_800ac784 + 5) = bVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}


