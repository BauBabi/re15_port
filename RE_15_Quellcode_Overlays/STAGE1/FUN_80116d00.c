/* FUN_80116d00 @ 0x80116d00  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d00(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  char cVar3;
  undefined2 uVar4;
  int iVar5;
  int extraout_v1;
  undefined1 uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined1 *puVar9;
  ushort unaff_s3;
  ushort uVar10;
  short unaff_s4;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  undefined4 in_stack_00000020;
  undefined4 in_stack_00000024;
  undefined4 in_stack_00000028;
  undefined4 in_stack_0000002c;
  
  *(undefined1 *)(param_1 + 5) = 4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  func_0x8001f314();
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    if (0x17 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_80117db0(0,0);
      return;
    }
    FUN_8011bf50(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x01') {
LAB_80117628:
      func_0x800453d0(8);
      return;
    }
  }
  else {
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x29) {
      FUN_8011bf50(0,1);
      if ((*(char *)(_DAT_800ac784 + 0x95) != '\x01') && (*(char *)(_DAT_800ac784 + 0x95) != ')')) {
        return;
      }
      goto LAB_80117628;
    }
    cVar3 = func_0x8001f314(0,0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  }
  uVar6 = 0;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_8011c024(0,0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = uVar6;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0'))
      {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        iVar5 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        func_0x80019700(3,(int)*(short *)(iVar5 + 0x6a),*(int *)(iVar5 + 0x188) + 0x40,
                        &stack0x00000010);
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
      in_stack_00000010 = _DAT_801213a8;
      in_stack_00000014 = _DAT_801213ac;
      in_stack_00000018 = _DAT_801213b0;
      in_stack_0000001c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
    }
    iVar5 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                            (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar5 != 0) {
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
    puVar7 = &stack0x00000020;
    if (DAT_800acae7 != 0) {
      return;
    }
    puVar8 = (undefined4 *)&DAT_800aca88;
    puVar9 = *(undefined1 **)(_DAT_800ac784 + 0x188);
    in_stack_00000020 = _DAT_80072d60;
    in_stack_00000024 = _DAT_80072d64;
    in_stack_00000028 = _DAT_80072d68;
    in_stack_0000002c = _DAT_80072d6c;
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(9);
    }
    puVar8 = &stack0x00000010;
    if (DAT_800acae7 != 0) {
      return;
    }
    puVar9 = &DAT_800aca88;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    in_stack_00000010 = _DAT_80072d60;
    in_stack_00000014 = _DAT_80072d64;
    in_stack_00000018 = _DAT_80072d68;
    in_stack_0000001c = _DAT_80072d6c;
    uVar4 = func_0x8001bff8(iVar5 + 0x448,puVar8,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar4;
    uVar4 = func_0x8001bff8(iVar5 + 0x6f8,puVar8,800,&DAT_800aca88);
    uVar10 = unaff_s3 - 1;
    bVar1 = unaff_s3 != 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar4;
    unaff_s3 = uVar10;
    if (bVar1) {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s3 + 0x80121470)) {
          unaff_s4 = 1;
        }
        bVar1 = unaff_s3 != 0;
        unaff_s3 = unaff_s3 - 1;
      } while (bVar1);
    }
    if (unaff_s4 == 0) {
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d8) == 0) && (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
      return;
    }
    puVar7 = (undefined4 *)&DAT_800acaee;
    _DAT_800acaee = _DAT_800acaee + -0xc;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(5);
    _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
    cVar3 = func_0x8001a780(&DAT_800aca54);
    uVar2 = _DAT_800aca58;
    DAT_800aca5b = SUB41(uVar2,3);
    _DAT_800aca58 = (uint3)CONCAT11(cVar3 + '\x04',DAT_800aca58);
    if (_DAT_800acaee < 0) {
      _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
    }
    DAT_800acae7 = DAT_800acae7 | 1;
  }
  uVar4 = func_0x8001bff8(puVar9 + 0x448);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar4;
  uVar4 = func_0x8001bff8(puVar9 + 0x6f8,puVar7,800,puVar8);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar4;
  if (unaff_s3 != 0) {
    do {
      unaff_s3 = unaff_s3 - 1;
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s3 + 0x80121478)) {
        unaff_s4 = 1;
      }
    } while (unaff_s3 != 0);
  }
  if ((unaff_s4 != 0) &&
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


