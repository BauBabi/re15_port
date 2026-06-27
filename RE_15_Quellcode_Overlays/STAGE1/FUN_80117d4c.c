/* FUN_80117d4c @ 0x80117d4c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d4c(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  undefined4 uVar4;
  char cVar5;
  undefined2 uVar6;
  int iVar7;
  int extraout_v1;
  undefined1 uVar8;
  undefined1 *unaff_s0;
  undefined4 *unaff_s1;
  undefined1 *unaff_s2;
  ushort uVar9;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  uVar9 = 6;
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        if ((DAT_800acae7 == 0) && (iVar7 = func_0x8001a804(4000,0x180,&DAT_800aca88), iVar7 < 0)) {
          if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
LAB_801180b8:
            *(undefined1 *)(_DAT_800ac784 + 5) = 3;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 0;
            return;
          }
        }
        else {
          if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) goto LAB_801180b8;
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
      }
      goto LAB_801188dc;
    }
    if (bVar2 != 0) goto LAB_801188dc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar5;
  uVar8 = 0;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_8011c024(0,0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = uVar8;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0'))
      {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        iVar7 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        func_0x80019700(3,(int)*(short *)(iVar7 + 0x6a),*(int *)(iVar7 + 0x188) + 0x40,&uStack_28);
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
      uStack_28 = _DAT_801213a8;
      uStack_24 = _DAT_801213ac;
      uStack_20 = _DAT_801213b0;
      uStack_1c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
    }
    iVar7 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                            (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar7 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
      func_0x800453d0(2);
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
      func_0x800453d0(9);
    }
    unaff_s0 = &stack0xffffffe8;
    if (DAT_800acae7 != 0) {
      return;
    }
    unaff_s1 = (undefined4 *)&DAT_800aca88;
    unaff_s2 = *(undefined1 **)(_DAT_800ac784 + 0x188);
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(9);
    }
    unaff_s1 = &uStack_28;
    if (DAT_800acae7 != 0) {
      return;
    }
    unaff_s2 = &DAT_800aca88;
    iVar7 = *(int *)(_DAT_800ac784 + 0x188);
    uStack_28 = _DAT_80072d60;
    uStack_24 = _DAT_80072d64;
    uStack_20 = _DAT_80072d68;
    uStack_1c = _DAT_80072d6c;
    uVar6 = func_0x8001bff8(iVar7 + 0x448,unaff_s1,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
    uVar6 = func_0x8001bff8(iVar7 + 0x6f8,unaff_s1,800,&DAT_800aca88);
    uVar9 = 5;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80121470)) {
        bVar3 = true;
      }
      bVar1 = uVar9 != 0;
      uVar9 = uVar9 - 1;
    } while (bVar1);
    if (!bVar3) {
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d8) == 0) && (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
      return;
    }
    unaff_s0 = &DAT_800acaee;
    _DAT_800acaee = _DAT_800acaee + -0xc;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(5);
    _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
    cVar5 = func_0x8001a780(&DAT_800aca54);
    uVar4 = _DAT_800aca58;
    DAT_800aca5b = SUB41(uVar4,3);
    _DAT_800aca58 = (uint3)CONCAT11(cVar5 + '\x04',DAT_800aca58);
    if (_DAT_800acaee < 0) {
      _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
    }
    DAT_800acae7 = DAT_800acae7 | 1;
  }
LAB_801188dc:
  uVar6 = func_0x8001bff8(unaff_s2 + 0x448);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
  uVar6 = func_0x8001bff8(unaff_s2 + 0x6f8,unaff_s0,800,unaff_s1);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
  if (uVar9 != 0) {
    do {
      uVar9 = uVar9 - 1;
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80121478)) {
        bVar3 = true;
      }
    } while (uVar9 != 0);
  }
  if ((bVar3) &&
     ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
    _DAT_800acaee = _DAT_800acaee + -600;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 300;
    func_0x800453d0(1);
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    DAT_800acae7 = DAT_800acae7 | 1;
    _DAT_800aca58 = 6;
    return;
  }
  return;
}


