/* FUN_80107634 @ 0x80107634  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107634(void)

{
  uint uVar1;
  uint *puVar2;
  undefined1 uVar3;
  char cVar4;
  ushort uVar5;
  short sVar6;
  int iVar7;
  undefined4 uVar8;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
  case 0:
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x27) = (uVar5 & 0x7f) + 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    if (_DAT_800b0fe0 < 3) {
      uVar3 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar8 = 0x800b1038;
    }
    else {
      uVar3 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar8 = 0x800b1058;
    }
    func_0x8004ef90(uVar8,uVar3);
  case 1:
    uVar1 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar1 + -1;
    if ((short)uVar1 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
    uVar3 = 0x12;
    if ((char)_DAT_800ac784[0x25] == '\r') {
      uVar3 = 0x13;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x25) = uVar3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  case 3:
switchD_80107670_caseD_3:
    cVar4 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar4;
    break;
  case 4:
    *(char *)(_DAT_800ac784 + 0x25) = (char)_DAT_800ac784[0x75];
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 5;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) & 0xfd;
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x27) = (uVar5 & 7) + 2;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 4;
  case 5:
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 2) != 0) {
      func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x4f4,
                      &DAT_8012017c);
      *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) & 0xfd;
      cVar4 = *(char *)((int)_DAT_800ac784 + 0x9e);
      *(char *)((int)_DAT_800ac784 + 0x9e) = cVar4 + -1;
      if (cVar4 == '\0') {
        *(undefined1 *)((int)_DAT_800ac784 + 7) = 6;
        return;
      }
    }
    if ((((DAT_800acae7 == 0) && (_DAT_800ac784[0x74] < 0x4b0)) &&
        (iVar7 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar7 == 0)) &&
       (DAT_800acad6 == *(char *)((int)_DAT_800ac784 + 0x82))) {
      uVar3 = func_0x8001a780(&DAT_800aca54);
      *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = uVar3;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 9;
    }
    sVar6 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] - sVar6;
    if ((short)_DAT_800ac784[0x27] == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 6;
    }
    func_0x8001a8f8(&DAT_800aca88,0x20);
    if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
      FUN_8010939c(0,0);
    }
    else {
      FUN_8010939c(0,1);
    }
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  case 7:
    cVar4 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar4;
    FUN_8010939c(0,1);
    break;
  case 8:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
    break;
  case 9:
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 10;
    *(char *)(_DAT_800ac784 + 0x25) = *(char *)((int)_DAT_800ac784 + 0x9f) * '\x03';
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800aca58 = (int)*(char *)((int)_DAT_800ac784 + 0x9f) << 8 | 5;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 10:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    goto switchD_80107670_caseD_3;
  case 0xb:
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    _DAT_800aca58 = 0x101;
    DAT_800acae8 = 0;
    DAT_800acae9 = 0;
    DAT_800acae3 = 0;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 6;
    puVar2 = _DAT_800ac784;
    if (*(char *)((int)_DAT_800ac784 + 0x9f) == '\0') {
      _DAT_800acabe = _DAT_800acabe + 0x800;
    }
    _DAT_800ac784 = (uint *)&DAT_800aca54;
    func_0x8001f314(_DAT_800acbc4,_DAT_800acbc8,0,0x200);
    _DAT_800ac784 = puVar2;
  }
  return;
}


