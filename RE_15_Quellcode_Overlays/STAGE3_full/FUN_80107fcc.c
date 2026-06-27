/* FUN_80107fcc @ 0x80107fcc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107fcc(void)

{
  ushort uVar1;
  byte bVar2;
  char cVar3;
  short sVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint *puVar9;
  uint uVar10;
  int local_78;
  undefined4 local_74;
  int local_70;
  undefined4 local_6c;
  uint local_68;
  uint local_64;
  uint local_60;
  uint local_5c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  local_78 = DAT_80100068;
  local_74 = DAT_8010006c;
  local_70 = DAT_80100070;
  local_6c = DAT_80100074;
  switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)((int)_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x40) == 0) {
      *(undefined2 *)((int)_DAT_800ac784 + 6) = 6;
      return;
    }
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x548) = 0x80;
    *(undefined4 *)(uVar10 + 0x54c) = 0x20;
    *(uint *)(uVar10 + 0x4b4) = *(uint *)(uVar10 + 0x4b4) | 0x80;
    puVar9 = (uint *)_DAT_800ac784[0x62];
    puVar9[0x25] = 0x80;
    puVar9[0x26] = 0x20;
    *puVar9 = *puVar9 | 0x80;
    uVar10 = func_0x8001af20();
    if (((uVar10 & 1) != 0) && ((char)_DAT_800ac784[2] != '\x13')) {
      *(undefined2 *)(puVar9 + 0x22) = 0x5f4;
      *(undefined2 *)((int)puVar9 + 0x8a) = 0x1000;
      *(undefined2 *)(puVar9 + 0x23) = 0x5f4;
      *puVar9 = *puVar9 | 0x400;
    }
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar9 + 0x10,&LAB_8011e2bc);
    func_0x800453d0(9);
    uVar10 = func_0x8001af20();
    if ((uVar10 & 1) != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      FUN_80108df0((uint)(byte)(&DAT_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62])
      ;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108df0(uVar6 + 0x560);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x60c) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x5e8) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x5ea) = 0xc18;
        *(undefined2 *)(uVar6 + 0x5ec) = 0x5f4;
        *(uint *)(uVar6 + 0x560) = *(uint *)(uVar6 + 0x560) | 0x400;
        FUN_80108df0(_DAT_800ac784[0x62] + 0x60c);
      }
      *(undefined4 *)(_DAT_800ac784[0x62] + 0x6b8) = 0;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108df0(uVar6 + 0x764);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x810) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x7ec) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x7ee) = 0xc18;
        *(undefined2 *)(uVar6 + 0x7f0) = 0x5f4;
        *(uint *)(uVar6 + 0x764) = *(uint *)(uVar6 + 0x764) | 0x400;
        FUN_80108df0(_DAT_800ac784[0x62] + 0x810);
      }
      *(undefined4 *)(_DAT_800ac784[0x62] + 0x8bc) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 200;
      if (_DAT_800b0fe0 < 3) {
        func_0x8004ef90(0x800b1038,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
        return;
      }
      func_0x8004ef90(0x800b1058,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
      return;
    }
    uVar10 = func_0x8001af20();
    if ((uVar10 & 3) == 0) {
      uVar10 = func_0x8001af20();
      uVar5 = 6;
      if ((uVar10 & 1) == 0) {
        uVar5 = 8;
      }
      func_0x800453d0(uVar5);
    }
    uVar10 = _DAT_800ac784[0x62];
    *(uint *)(uVar10 + 0x4b4) = *(uint *)(uVar10 + 0x4b4) | 0x40;
    local_74 = 0;
    local_78 = _DAT_800ac784[0xd] - _DAT_800aca88;
    local_70 = _DAT_800ac784[0xf] - _DAT_800aca90;
    func_0x80066a1c(&local_78,uVar10 + 0x4ec);
    *(undefined2 *)(uVar10 + 0x4ee) = 0x50;
    *(undefined1 *)(uVar10 + 0x529) = 3;
    *(undefined1 *)(uVar10 + 0x52a) = 10;
    *(undefined2 *)(uVar10 + 0x536) = 8;
    *(short *)(uVar10 + 0x4ec) = *(short *)(uVar10 + 0x4ec) >> 3;
    *(short *)(uVar10 + 0x4f0) = *(short *)(uVar10 + 0x4f0) >> 3;
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_8011e2bc);
    func_0x80019700(0x5002800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,
                    &LAB_8011e2bc);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62]
                    + 0x40,&LAB_8011e2bc);
  case 1:
    uVar8 = _DAT_800ac784[0x62];
    *(int *)(uVar8 + 0x508) = (int)*(short *)(uVar8 + 0x4ec) + *(int *)(uVar8 + 0x508);
    *(int *)(uVar8 + 0x510) = (int)*(short *)(uVar8 + 0x4f0) + *(int *)(uVar8 + 0x510);
    puVar9 = _DAT_800ac784;
    local_68 = _DAT_800ac784[0xd];
    local_64 = _DAT_800ac784[0xe];
    local_60 = _DAT_800ac784[0xf];
    local_5c = _DAT_800ac784[0x10];
    uVar10 = *(uint *)(uVar8 + 0x50c);
    uVar6 = *(uint *)(uVar8 + 0x510);
    uVar7 = *(uint *)(uVar8 + 0x514);
    _DAT_800ac784[0xd] = *(uint *)(uVar8 + 0x508);
    puVar9[0xe] = uVar10;
    puVar9[0xf] = uVar6;
    puVar9[0x10] = uVar7;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0xd,*(undefined2 *)(_DAT_800ac784[0x1e] + 6),4);
    uVar10 = _DAT_800ac784[0xe];
    uVar6 = _DAT_800ac784[0xf];
    uVar7 = _DAT_800ac784[0x10];
    *(uint *)(uVar8 + 0x508) = _DAT_800ac784[0xd];
    *(uint *)(uVar8 + 0x50c) = uVar10;
    *(uint *)(uVar8 + 0x510) = uVar6;
    *(uint *)(uVar8 + 0x514) = uVar7;
    puVar9 = _DAT_800ac784;
    _DAT_800ac784[0xd] = local_68;
    puVar9[0xe] = local_64;
    puVar9[0xf] = local_60;
    puVar9[0x10] = local_5c;
    cVar3 = *(char *)(uVar8 + 0x529);
    *(short *)(uVar8 + 0x4f0) = *(short *)(uVar8 + 0x4f0) >> 1;
    *(short *)(uVar8 + 0x4ec) = *(short *)(uVar8 + 0x4ec) >> 1;
    if (cVar3 < '\x04') {
      if (cVar3 == '\0') {
        *(undefined1 *)(uVar8 + 0x52a) = 0;
        *(undefined2 *)(uVar8 + 0x4ee) = 0;
      }
      sVar4 = (short)*(char *)(uVar8 + 0x52a) + *(short *)(uVar8 + 0x4ee);
      *(int *)(uVar8 + 0x50c) = (int)*(short *)(uVar8 + 0x4ee) + *(int *)(uVar8 + 0x50c);
      *(short *)(uVar8 + 0x4ee) = sVar4;
      if (*(short *)((int)_DAT_800ac784 + 0x1ba) + -200 < *(int *)(uVar8 + 0x50c)) {
        *(short *)(uVar8 + 0x4ee) = -(sVar4 >> 2);
        *(int *)(uVar8 + 0x50c) = *(short *)((int)_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar8 + 0x4f4,
                        &LAB_8011e2bc);
        if (*(char *)(uVar8 + 0x529) != '\0') {
          *(char *)(uVar8 + 0x529) = *(char *)(uVar8 + 0x529) + -1;
        }
      }
      if (*(short *)(uVar8 + 0x536) != 0) {
        *(short *)(uVar8 + 0x536) = *(short *)(uVar8 + 0x536) + -1;
        local_48 = _DAT_80072d4c;
        local_44 = _DAT_80072d50;
        local_40 = _DAT_80072d54;
        local_3c = _DAT_80072d58;
        local_38 = _DAT_80072d5c;
        local_34 = _DAT_80072d60;
        local_30 = _DAT_80072d64;
        local_2c = _DAT_80072d68;
        func_0x80065b70(0x80,&local_48);
        func_0x80022da0(uVar8 + 0x4f4,&local_48,uVar8 + 0x4f4);
        uVar1 = *(ushort *)(uVar8 + 0x536);
        goto joined_r0x8010883c;
      }
      *(uint *)(_DAT_800ac784[0x62] + 0x560) = *(uint *)(_DAT_800ac784[0x62] + 0x560) | 0x200;
      *(uint *)(_DAT_800ac784[0x62] + 0x764) = *(uint *)(_DAT_800ac784[0x62] + 0x764) | 0x200;
    }
    else {
      uVar1 = *(ushort *)(uVar8 + 0x536);
      *(char *)(uVar8 + 0x529) = cVar3 + -1;
joined_r0x8010883c:
      if ((uVar1 & 1) != 0) {
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar8 + 0x4f4,
                        &LAB_8011e2bc);
      }
    }
    cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_80108b34:
    if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
      FUN_80109488(0,0);
    }
    break;
  case 2:
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) == 0) {
      uVar10 = _DAT_800ac784[0x62];
      *(ushort *)(uVar10 + 0x7c) = *(short *)(uVar10 + 0x7c) + 0x800U & 0xfff;
      *(ushort *)(uVar10 + 100) = *(short *)(uVar10 + 100) + 0x800U & 0xfff;
      func_0x80068098(uVar10 + 0x60,uVar10 + 0x18);
    }
    *(uint *)(_DAT_800ac784[0x62] + 0xac) = *(uint *)(_DAT_800ac784[0x62] + 0xac) | 0x40;
    *(uint *)(_DAT_800ac784[0x62] + 0x158) = *(uint *)(_DAT_800ac784[0x62] + 0x158) | 0x40;
    *(uint *)(_DAT_800ac784[0x62] + 0x2b0) = *(uint *)(_DAT_800ac784[0x62] + 0x2b0) | 0x40;
    *(uint *)(_DAT_800ac784[0x62] + 0x35c) = *(uint *)(_DAT_800ac784[0x62] + 0x35c) | 0x40;
    uVar10 = _DAT_800ac784[0x62];
    _DAT_800ac784[0xd] = *(uint *)(uVar10 + 0x508);
    _DAT_800ac784[0xf] = *(uint *)(uVar10 + 0x510);
    *(uint *)(uVar10 + 0x4b4) = *(uint *)(uVar10 + 0x4b4) & 0xffffffbf;
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_8011e2bc);
    *(uint *)(_DAT_800ac784[0x62] + 0x560) = *(uint *)(_DAT_800ac784[0x62] + 0x560) & 0xfffffdff;
    *(uint *)(_DAT_800ac784[0x62] + 0x764) = *(uint *)(_DAT_800ac784[0x62] + 0x764) & 0xfffffdff;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0x81;
    _DAT_800ac784[1] = 1;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0x1e;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) & 0xfe;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 0x80;
    break;
  case 3:
    cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
    func_0x800245d8(((int)~(uint)*(byte *)((int)_DAT_800ac784 + 0x93) >> 7) << 0xb);
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + -0x14;
    if ((short)_DAT_800ac784[0x23] < 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    }
    goto LAB_80108b34;
  case 4:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}


