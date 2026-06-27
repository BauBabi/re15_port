/* FUN_8010cc08 @ 0x8010cc08  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc08(void)

{
  bool bVar1;
  undefined4 uVar2;
  char cVar3;
  byte bVar4;
  undefined2 uVar5;
  int iVar6;
  uint uVar7;
  undefined1 *puVar8;
  ushort uVar9;
  ushort uVar10;
  short unaff_s4;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  uVar10 = 0;
  uVar7 = (uint)(byte)_DAT_800ac784[6];
  if (uVar7 != 1) {
    puVar8 = _DAT_800ac784;
    if (1 < uVar7) {
      bVar4 = 2;
      if (uVar7 == 2) {
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
        _DAT_800ac784[5] = 3;
        _DAT_800ac784[6] = 0;
        _DAT_800ac784[7] = 0;
        return;
      }
      goto LAB_8010d6bc;
    }
    bVar4 = 1;
    if (uVar7 != 0) goto LAB_8010d6bc;
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 0x12;
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  _DAT_800ac784[6] = _DAT_800ac784[6] + cVar3;
  FUN_80111208(0,0);
  iVar6 = *(int *)(_DAT_800ac784 + 0x188);
  local_24 = _DAT_80072d64;
  local_20 = _DAT_80072d68;
  local_1c = _DAT_80072d6c;
  local_28 = 100;
  if (_DAT_800ac784[0x95] == '\v') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 != 0) {
    return;
  }
  uVar5 = func_0x8001bff8(iVar6 + 0x64c,&local_28,1000,&DAT_800aca88);
  uVar9 = 3;
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
  do {
    if (_DAT_800ac784[0x95] == *(char *)(uVar9 + 0x80119f78)) {
      uVar10 = 1;
    }
    bVar1 = uVar9 != 0;
    uVar9 = uVar9 - 1;
  } while (bVar1);
  if (uVar10 == 0) {
    return;
  }
  if (*(short *)(_DAT_800ac784 + 0x1da) == 0) {
    return;
  }
  _DAT_800acaee = _DAT_800acaee + -5;
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
  func_0x800453d0(5);
  _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
  puVar8 = &DAT_800aca54;
  cVar3 = func_0x8001a780();
  uVar2 = _DAT_800aca58;
  uVar7 = (uint)_DAT_800acaee;
  DAT_800aca58_1._2_1_ = SUB41(uVar2,3);
  _DAT_800aca58 = (uint3)CONCAT11(cVar3 + '\x02',DAT_800aca58);
  if ((int)uVar7 < 0) {
    _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  bVar4 = DAT_800acae7;
LAB_8010d6bc:
  *(byte *)(uVar7 + 0x8f) = bVar4;
  _DAT_800ac784[0x1e0] = (char)puVar8;
  _DAT_800ac784[0x93] = _DAT_800ac784[0x93] | 1;
  bVar4 = _DAT_800ac784[0x93];
  if ((bVar4 & 2) != 0) {
    if ((bVar4 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      _DAT_800ac784[4] = 3;
      _DAT_800ac784[5] = 9;
      _DAT_800ac784[6] = 3;
      _DAT_800ac784[7] = 0;
      return;
    }
    _DAT_800ac784[0x93] = bVar4 & 0xfd;
    local_28 = _DAT_80119eb4;
    local_24 = _DAT_80119eb8;
    local_20 = _DAT_80119ebc;
    local_1c = _DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
  iVar6 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                          (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                          (int)*(short *)(_DAT_800ac784 + 0x1ba));
  if (iVar6 == 0) {
    if (_DAT_800ac784[0x95] == '\a') {
      func_0x800453d0(9);
    }
    if (DAT_800acae7 == 0) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      uVar5 = func_0x8001bff8(iVar6 + 0x448,&stack0xffffffe8,800,&DAT_800aca88);
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
      uVar5 = func_0x8001bff8(iVar6 + 0x6f8,&stack0xffffffe8,800,&DAT_800aca88);
      *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
      if (uVar10 != 0) {
        do {
          uVar10 = uVar10 - 1;
          if (_DAT_800ac784[0x95] == *(char *)(uVar10 + 0x80119f84)) {
            unaff_s4 = 1;
          }
        } while (uVar10 != 0);
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
  _DAT_800ac784[6] = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800453d0(1);
  _DAT_800ac784[0x1e0] = 0;
  _DAT_800ac784[0x82] = 0;
  _DAT_800ac784[0x93] = 0;
  return;
}


