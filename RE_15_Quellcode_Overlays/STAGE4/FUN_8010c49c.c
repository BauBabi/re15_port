/* FUN_8010c49c @ 0x8010c49c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c49c(void)

{
  bool bVar1;
  undefined4 uVar2;
  undefined1 uVar3;
  char cVar4;
  undefined2 uVar5;
  uint uVar6;
  int iVar7;
  uint in_v1;
  byte bVar8;
  short unaff_s0;
  ushort uVar9;
  ushort unaff_s3;
  short unaff_s4;
  undefined8 uVar10;
  undefined4 uStackX_8;
  undefined4 uStackX_c;
  
  bVar8 = *(byte *)(_DAT_800ac784 + 6);
  uVar6 = (uint)(bVar8 < 2);
  if (bVar8 == 1) {
LAB_8010c56c:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(3);
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    bVar8 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    uVar10 = FUN_80111208(0,0);
    in_v1 = (uint)((ulonglong)uVar10 >> 0x20);
    uVar6 = (uint)uVar10;
  }
  else {
    if (uVar6 == 0) {
      uVar6 = 2;
      if (bVar8 != 2) goto LAB_8010ce68;
      in_v1 = (uint)*(byte *)(_DAT_800ac784 + 7);
      if (in_v1 == 1) {
        uVar3 = 4;
      }
      else {
        if (1 < in_v1) {
          uVar6 = 3;
          if (in_v1 == 2) {
            *(undefined1 *)(_DAT_800ac784 + 5) = 3;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 1;
            return;
          }
          goto LAB_8010ce68;
        }
        uVar6 = 3;
        uVar3 = 3;
        if (in_v1 != 0) goto LAB_8010ce68;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_8010ce68;
    }
    if (bVar8 != 0) goto LAB_8010ce68;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    bVar8 = *(byte *)(_DAT_800ac784 + 7);
    if (bVar8 == 1) {
      uVar5 = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    }
    else if (bVar8 < 2) {
      uVar5 = 3;
      if (bVar8 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      }
    }
    else {
      uVar5 = 2;
      if (bVar8 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
        goto LAB_8010c56c;
      }
    }
    uVar9 = unaff_s0 - 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    if (unaff_s0 != 0) {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80119f78)) {
          unaff_s3 = 1;
        }
        bVar1 = uVar9 != 0;
        uVar9 = uVar9 - 1;
      } while (bVar1);
    }
    if ((unaff_s3 == 0) || (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
      return;
    }
    _DAT_800acaee = _DAT_800acaee + -5;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(5);
    _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
    bVar8 = 0x54;
    cVar4 = func_0x8001a780();
    uVar2 = _DAT_800aca58;
    in_v1 = (uint)_DAT_800acaee;
    DAT_800aca58_1._2_1_ = SUB41(uVar2,3);
    _DAT_800aca58 = (uint3)CONCAT11(cVar4 + '\x02',DAT_800aca58);
    if ((int)in_v1 < 0) {
      _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
    }
    uVar6 = DAT_800acae7 | 1;
  }
LAB_8010ce68:
  DAT_800acae7 = (byte)uVar6;
  *(byte *)(in_v1 + 0x8f) = DAT_800acae7;
  *(byte *)(_DAT_800ac784 + 0x1e0) = bVar8;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  bVar8 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar8 & 2) != 0) {
    if ((bVar8 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 3;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar8 & 0xfd;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0xfffffff8);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
  iVar7 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                          (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                          (int)*(short *)(_DAT_800ac784 + 0x1ba));
  if (iVar7 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    return;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    iVar7 = *(int *)(_DAT_800ac784 + 0x188);
    uStackX_8 = _DAT_80072d60;
    uStackX_c = _DAT_80072d64;
    uVar5 = func_0x8001bff8(iVar7 + 0x448,&uStackX_8,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
    uVar5 = func_0x8001bff8(iVar7 + 0x6f8,&uStackX_8,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    if (unaff_s3 != 0) {
      do {
        unaff_s3 = unaff_s3 - 1;
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s3 + 0x80119f84)) {
          unaff_s4 = 1;
        }
      } while (unaff_s3 != 0);
    }
    if ((unaff_s4 != 0) &&
       ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
      _DAT_800acaee = _DAT_800acaee + -500;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 300;
      func_0x800453d0(0);
      _DAT_800aca58 = 6;
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      DAT_800acae7 = DAT_800acae7 | 1;
      func_0x80037edc(0,0x32);
      return;
    }
  }
  return;
}


