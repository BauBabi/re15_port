/* ============ FUN_8010063c @ 0x8010063c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010063c(void)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  byte bVar5;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  local_38 = DAT_80100004;
  local_34 = DAT_80100008;
  local_30 = DAT_8010000c;
  local_2c = DAT_80100010;
  local_28 = DAT_80100014;
  local_24 = DAT_80100018;
  local_20 = DAT_8010001c;
  local_1c = DAT_80100020;
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80118cb8;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_80118d0c)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_801185a0 + (uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&DAT_80118cac)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (*(code *)(&PTR_FUN_80118d14)[uVar3 & 7])();
  }
  if ((_DAT_800aca3c & 1) != 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 0x100;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar3 & 1) << 9);
    uVar1 = func_0x8001af20();
    uVar2 = func_0x8001af20();
    iVar4 = (uVar3 & 1) * 8;
    *(ushort *)((int)_DAT_800ac784 + 0x1da) = (uVar2 & 0xff) + (uVar1 & 0xff) + 600;
    func_0x80019700(0x8031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccc)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccd)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cce)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccf)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd0)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd1)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd2)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd3)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
  }
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
  bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if ((bVar5 == 1) || (bVar5 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x12;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    }
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 6) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x27;
    _DAT_800ac784[1] = 0x20c01;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xb) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xd) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x27;
    _DAT_800ac784[1] = 0x201;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xe) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x2a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    _DAT_800ac784[1] = 0x1201;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  func_0x8001ee68(_DAT_800ac784);
  return;
}



/* ============ FUN_80100fbc @ 0x80100fbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fbc(void)

{
  int iVar1;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x1e2) = 0x1000;
  *(uint *)(iVar1 + 0x158) = *(uint *)(iVar1 + 0x158) | 0x400;
  *(undefined2 *)(iVar1 + 0x1e0) = 0x7e8;
  *(undefined2 *)(iVar1 + 0x1e4) = 0x7e8;
  *(undefined4 *)(iVar1 + 0x1ec) = 0x80;
  *(undefined4 *)(iVar1 + 0x1f0) = 0x8080e0;
  *(uint *)(iVar1 + 0x158) = *(uint *)(iVar1 + 0x158) | 0x80;
  return;
}



/* ============ FUN_8010100c @ 0x8010100c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010100c(void)

{
  int iVar1;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x89a) = 0x1000;
  *(uint *)(iVar1 + 0x810) = *(uint *)(iVar1 + 0x810) | 0x400;
  *(undefined2 *)(iVar1 + 0x898) = 0x5f4;
  *(undefined2 *)(iVar1 + 0x89c) = 0x5f4;
  *(undefined4 *)(iVar1 + 0x8a4) = 0x80;
  *(undefined4 *)(iVar1 + 0x8a8) = 0x8080e0;
  *(uint *)(iVar1 + 0x810) = *(uint *)(iVar1 + 0x810) | 0x80;
  return;
}



/* ============ FUN_801010f0 @ 0x801010f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010f0(void)

{
  int iVar1;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x3e6) = 0x1000;
  *(uint *)(iVar1 + 0x35c) = *(uint *)(iVar1 + 0x35c) | 0x400;
  *(undefined2 *)(iVar1 + 0x3e4) = 0x7e8;
  *(undefined2 *)(iVar1 + 1000) = 0x7e8;
  *(undefined4 *)(iVar1 + 0x3f0) = 0x80;
  *(undefined4 *)(iVar1 + 0x3f4) = 0x8080e0;
  *(uint *)(iVar1 + 0x35c) = *(uint *)(iVar1 + 0x35c) | 0x80;
  return;
}



/* ============ FUN_801011d8 @ 0x801011d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011d8(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccc)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccd)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cce)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccf)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd0)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd1)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd2)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd3)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      return;
    }
  }
  uVar1 = func_0x8001bc08();
  if ((uVar1 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar1 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
    func_0x80012974(5000);
  }
  (*(code *)(&PTR_FUN_80118d34)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80101640 @ 0x80101640 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101640(void)

{
  (*(code *)(&PTR_FUN_80118d68)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118db8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801016bc @ 0x801016bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016bc(void)

{
  (*(code *)(&PTR_FUN_80118e08)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118e48)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101738 @ 0x80101738 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101738(void)

{
  (*(code *)(&PTR_FUN_80118e88)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118db8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801017b4 @ 0x801017b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801017b4(void)

{
  (*(code *)(&PTR_FUN_80118ed8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118e48)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101830 @ 0x80101830 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101830(void)

{
  (*(code *)(&PTR_FUN_80118ef0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118ef4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801018ac @ 0x801018ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801018ac(void)

{
  (*(code *)(&PTR_FUN_80118ef8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118efc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101928 @ 0x80101928 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101928(void)

{
  (*(code *)(&PTR_LAB_80118f00)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118efc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801019a4 @ 0x801019a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019a4(void)

{
  (*(code *)(&PTR_FUN_80118f04)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118efc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101a20 @ 0x80101a20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101a20(void)

{
  (*(code *)(&PTR_LAB_80118f08)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118f0c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101a9c @ 0x80101a9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101a9c(void)

{
  (*(code *)(&PTR_LAB_80118f10)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118f14)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101b18 @ 0x80101b18 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b18(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  uint uVar4;
  
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,0x5f4);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
  }
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar2 != 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  if ((((sVar3 == 0) && ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0)) &&
      (*(undefined4 *)(_DAT_800ac784 + 4) = 0x201, 10000 < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
     (uVar4 = func_0x8001af20(), (uVar4 & 7) == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
  }
  return;
}



/* ============ FUN_80101c30 @ 0x80101c30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c30(void)

{
  short sVar1;
  uint uVar2;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x5f4);
  if (((sVar1 == 0) && ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0)) &&
     (*(uint *)(_DAT_800ac784 + 0x1d0) < 4000)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 7) == 0) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
    }
  }
  return;
}



/* ============ FUN_80101cbc @ 0x80101cbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101cbc(void)

{
  ushort uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar1 & 0x3f) + 300;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,1);
  }
  return;
}



/* ============ FUN_80101d98 @ 0x80101d98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101d98(void)

{
  short sVar1;
  short sVar2;
  uint uVar3;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x5f4);
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar1 != 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  if ((((sVar2 == 0) && ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0)) &&
      (*(undefined4 *)(_DAT_800ac784 + 4) = 0x201, 10000 < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
     (uVar3 = func_0x8001af20(), (uVar3 & 3) == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x1000) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1001;
  }
  return;
}



/* ============ FUN_80101ea4 @ 0x80101ea4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ea4(void)

{
  short sVar1;
  undefined1 uVar2;
  ushort uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0xff) + 600;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar2 = func_0x8003a07c();
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 8;
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_8010200c @ 0x8010200c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010200c(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) == 0) ||
     (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))) {
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) &&
       (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x2c8), iVar2 != 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
    }
    if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
        (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar2 == 0)) &&
       (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
      iVar2 = func_0x8001a780(&DAT_800aca54);
      *(uint *)(_DAT_800ac784 + 4) = (iVar2 + 3) * 0x100 | 1;
    }
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 0x5dc) && (_DAT_800acaee < 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x1000) != 0) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x1001;
    }
  }
  else {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
  }
  return;
}



/* ============ FUN_801021ac @ 0x801021ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801021ac(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 uVar6;
  short sVar7;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0xf) + 8;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar5 = func_0x8001af20();
    if ((uVar5 & 3) == 0) {
      uVar5 = func_0x8001af20();
      uVar6 = 5;
      if ((uVar5 & 1) != 0) {
        uVar6 = 4;
      }
      func_0x800453d0(uVar6);
    }
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
    if (DAT_800aca4e < 5) {
      uVar5 = func_0x8001af20();
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80119028)[uVar5 & 0x1f];
    }
    else {
      uVar5 = func_0x8001af20();
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80119018)[uVar5 & 0x1f];
    }
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_80118f18 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_80118f1a +
                    *(char *)(_DAT_800ac784 + 0x9f) * 4 +
                    (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                  (int)sVar7);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x1f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_80118f18 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801024fc @ 0x801024fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801024fc(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03';
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800aca58 = (*(byte *)((int)_DAT_800ac784 + 5) - 3) * 0x100 | 5;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 1;
    func_0x800453d0(4);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03' + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x6e;
    if ((_DAT_800aca50 & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 5;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 100;
    _DAT_800acaee = _DAT_800acaee + -10;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    iVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    if (iVar3 != 0) {
      _DAT_800acaee = _DAT_800acaee + -5;
      iVar3 = _DAT_800ac784[0x62] + 0x6b8;
      if ((byte)_DAT_800ac784[2] - 0x13 < 2) {
        iVar3 = _DAT_800ac784[0x62] + 0x764;
      }
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_80118cfc);
    }
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -5;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      _DAT_800aca58 = CONCAT12(4,_DAT_800aca58);
    }
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      _DAT_800ac784[1] = (*(byte *)((int)_DAT_800ac784 + 5) + 2) * 0x100 | 1;
    }
    break;
  case 4:
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03' + '\x02';
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(7);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    _DAT_800aca50 = _DAT_800aca50 | 1;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xfffe;
    func_0x800453d0(7);
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x18') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    }
    if (((((*(byte *)((int)_DAT_800ac784 + 0x1c2) & 2) != 0) &&
         (7 < *(byte *)((int)_DAT_800ac784 + 0x95))) &&
        (uVar4 = _DAT_800ac784[0x6b], -1 < *(short *)(uVar4 + 0x9a))) &&
       (((*(byte *)(uVar4 + 9) & 0x80) == 0 && ((*(ushort *)(uVar4 + 0x1d8) & 1) == 0)))) {
      *(undefined4 *)(uVar4 + 4) = 0xb01;
    }
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x11;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x32;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 2;
  case 7:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],1,0x100);
    *(char *)((int)_DAT_800ac784 + 0x9f) = *(char *)((int)_DAT_800ac784 + 0x9f) - cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x9f) == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 8;
    }
    func_0x800245d8(0x800);
    *(short *)(_DAT_800ac784 + 0x23) =
         (short)_DAT_800ac784[0x23] - (short)*(char *)((int)_DAT_800ac784 + 0x9e);
    if ((short)_DAT_800ac784[0x23] < 10) {
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 10;
    }
    break;
  case 8:
    _DAT_800ac784[1] = 0x201;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  }
  return;
}



/* ============ FUN_80102b8c @ 0x80102b8c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102b8c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 5) + '\x04';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
    _DAT_800aca58 = (*(byte *)(_DAT_800ac784 + 5) - 5) * 0x100 | 6;
    func_0x800453d0(4);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
    func_0x800453d0(3);
  }
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x170),
                  *(undefined4 *)(_DAT_800ac784 + 0x174));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80102cd4 @ 0x80102cd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102cd4(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar1 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar1 + 3) * 0x100 | 1;
  }
  return;
}



/* ============ FUN_80102d7c @ 0x80102d7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102d7c(void)

{
  byte bVar1;
  short sVar2;
  uint uVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 3) == 0) {
      uVar3 = func_0x8001af20();
      uVar4 = 5;
      if ((uVar3 & 1) != 0) {
        uVar4 = 4;
      }
      func_0x800453d0(uVar4);
    }
  }
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x80);
  if (sVar2 == 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    func_0x8001a8f8(&DAT_800aca88,0x80);
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 8;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar1 & 1) + 1;
  }
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_80102ed0 @ 0x80102ed0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102ed0(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) == 0) ||
     (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))) {
    if ((DAT_800acae7 == '\0') &&
       (((*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0 &&
         (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar2 == 0)) &&
        (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))))) {
      iVar2 = func_0x8001a780(&DAT_800aca54);
      *(uint *)(_DAT_800ac784 + 4) = (iVar2 + 3) * 0x100 | 1;
    }
  }
  else {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
  }
  return;
}



/* ============ FUN_80102fc8 @ 0x80102fc8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102fc8(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x19;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar2 & 0x3f) + 0x96;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 3) == 0) {
      uVar3 = func_0x8001af20();
      uVar4 = 5;
      if ((uVar3 & 1) != 0) {
        uVar4 = 4;
      }
      func_0x800453d0(uVar4);
    }
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x40
                 );
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80103198 @ 0x80103198 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103198(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 1;
    break;
  case 1:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x800453d0(0);
    break;
  case 6:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 3;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
  default:
    goto switchD_801031d0_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_801031d0_default:
  return;
}



/* ============ FUN_8010337c @ 0x8010337c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010337c(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x474;
        *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4;
      }
      else if (bVar1 != 3) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x474;
        *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
      *(undefined4 *)(_DAT_800ac784 + 4) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x200);
  sVar2 = func_0x8001aa68((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  if (sVar2 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}



/* ============ FUN_801035ac @ 0x801035ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801035ac(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar1 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar1 + 1) * 0x100 | 1;
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x2000) != 0) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x80) == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
  }
  return;
}



/* ============ FUN_80103690 @ 0x80103690 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103690(void)

{
  undefined4 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x10000) != 0) {
    func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                    0x10);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x100);
  if ((*(char *)(_DAT_800ac784 + 0x95) == '#') || (*(char *)(_DAT_800ac784 + 0x95) == 'P')) {
    func_0x800453d0(2);
  }
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) != 0) {
        return;
      }
      uVar1 = 0;
    }
    else {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) != 0) {
        return;
      }
      uVar1 = 1;
    }
    FUN_80109424(0,uVar1);
  }
  return;
}



/* ============ FUN_80103810 @ 0x80103810 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103810(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x32;
    func_0x800453d0(0);
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar1 != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
  }
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) =
       *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
  if (*(short *)(_DAT_800ac784 + 0x8c) < 10) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
  }
  return;
}



/* ============ FUN_80103934 @ 0x80103934 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103934(void)

{
  ushort uVar1;
  uint uVar2;
  
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 4000) && (*(char *)(_DAT_800ac784 + 6) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = uVar1 & 0xf;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 3) == 0) {
      func_0x800453d0(5);
    }
  }
  return;
}



/* ============ FUN_801039b0 @ 0x801039b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801039b0(void)

{
  uint uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar1 = func_0x8001af20();
    if ((uVar1 & 3) == 0) {
      func_0x800453d0(5);
    }
  }
  return;
}



/* ============ FUN_80103a0c @ 0x80103a0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a0c(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    }
  }
  else if (bVar1 < 2) {
    if (bVar1 == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    }
  }
  else if (bVar1 == 2) {
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  }
  else if (bVar1 == 3) {
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  }
  return;
}



/* ============ FUN_80103b48 @ 0x80103b48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103b48(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100068;
  local_14 = DAT_8010006c;
  local_10 = DAT_80100070;
  local_c = DAT_80100074;
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1b;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800aca58 = (*(byte *)((int)_DAT_800ac784 + 5) + 1) * 0x100 | 5;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 1;
    func_0x800453d0(4);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    if ((_DAT_800aca50 & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 10;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x5a;
    _DAT_800acaee = _DAT_800acaee + -5;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    iVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    if (iVar3 != 0) {
      _DAT_800acaee = _DAT_800acaee + -1;
      iVar3 = _DAT_800ac784[0x62] + 0x6b8;
      if ((byte)_DAT_800ac784[2] - 0x13 < 2) {
        iVar3 = _DAT_800ac784[0x62] + 0x764;
      }
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_80118cfc);
    }
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -5;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      _DAT_800aca58 = CONCAT12(4,_DAT_800aca58);
    }
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      *(char *)((int)_DAT_800ac784 + 5) = *(char *)((int)_DAT_800ac784 + 5) + '\x02';
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    _DAT_800aca50 = _DAT_800aca50 | 1;
    func_0x800453d0(7);
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if ((DAT_800aca5c & 4) == 0) {
      if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x19') {
        *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
        puVar5 = (uint *)((uint)(byte)(&DAT_80118cac)[(byte)_DAT_800ac784[2]] * 0xac +
                         _DAT_800ac784[0x62]);
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar5 + 0x10,&local_18);
        puVar5[0x1a] = 0x6f;
        *(undefined2 *)(puVar5 + 0x25) = 0;
        *(undefined2 *)((int)puVar5 + 0x96) = 0;
        *(undefined2 *)(puVar5 + 0x26) = 0;
        *(undefined2 *)((int)puVar5 + 0x9a) = 0;
        *(undefined2 *)(puVar5 + 0x27) = 0;
        *(undefined2 *)((int)puVar5 + 0x9e) = 0;
        *puVar5 = *puVar5 | 0x4a;
        func_0x800453d0(9);
      }
    }
    else if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x19') {
      *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 2;
      func_0x800453d0(9);
      uVar4 = func_0x8001af20();
      if ((uVar4 & 1) == 0) {
        func_0x800453d0(8);
      }
      else {
        func_0x800453d0(5);
      }
    }
    break;
  case 6:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}



/* ============ FUN_8010412c @ 0x8010412c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010412c(void)

{
  char cVar1;
  short sVar2;
  uint *puVar3;
  uint *puVar4;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  char local_28 [8];
  
  local_40 = DAT_80100098;
  local_3c = DAT_8010009c;
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 6) != 0) {
    puVar3 = (uint *)((uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 2) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      *puVar3 = *puVar3 | 0x40;
      *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xfffd;
      *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 4;
      local_60 = _DAT_80072d4c;
      local_5c = _DAT_80072d50;
      local_58 = _DAT_80072d54;
      local_54 = _DAT_80072d58;
      local_50 = _DAT_80072d5c;
      local_4c = _DAT_80072d60;
      local_48 = _DAT_80072d64;
      local_44 = _DAT_80072d68;
      func_0x800659d0(*(short *)(_DAT_800ac784 + 0x6a) + 0x800,&local_60);
      func_0x80067a28(&local_60,&local_40,puVar3 + 0xe);
      *(undefined1 *)((int)puVar3 + 0x75) = 6;
    }
    local_60 = _DAT_80072d4c;
    local_5c = _DAT_80072d50;
    local_58 = _DAT_80072d54;
    local_54 = _DAT_80072d58;
    local_50 = _DAT_80072d5c;
    local_4c = _DAT_80072d60;
    local_48 = _DAT_80072d64;
    local_44 = _DAT_80072d68;
    func_0x80065b70(0x80,&local_60);
    func_0x8006575c(0x40,&local_60);
    puVar4 = puVar3 + 0x10;
    func_0x80022da0(puVar4,&local_60,puVar4);
    local_38 = puVar3[0x15];
    local_34 = puVar3[0x16];
    local_30 = puVar3[0x17];
    local_2c = puVar3[0x18];
    puVar3[0x15] = (int)(short)puVar3[0xe] + puVar3[0x15];
    puVar3[0x16] = (int)*(short *)((int)puVar3 + 0x3a) + puVar3[0x16];
    puVar3[0x17] = (int)(short)puVar3[0xf] + puVar3[0x17];
    sVar2 = func_0x8001c2dc(puVar3 + 0x15,100,local_28);
    if (local_28[0] != '\0') {
      puVar3[0x15] = local_38;
      puVar3[0x16] = local_34;
      puVar3[0x17] = local_30;
      puVar3[0x18] = local_2c;
      sVar2 = func_0x8001c2dc(puVar3 + 0x15,100,local_28);
      *(short *)(puVar3 + 0xe) = -(short)puVar3[0xe];
      *(short *)(puVar3 + 0xf) = -(short)puVar3[0xf];
      local_30 = 0;
      local_34 = 0;
      local_38 = 0;
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar4,&local_38);
      func_0x800453d0(1);
    }
    *(short *)((int)puVar3 + 0x3a) = *(short *)((int)puVar3 + 0x3a) + 0x32;
    if ((int)sVar2 < (int)puVar3[0x16]) {
      puVar3[0x16] = (int)sVar2;
      *(short *)(puVar3 + 0xe) = (short)puVar3[0xe] >> 1;
      *(short *)(puVar3 + 0xf) = (short)puVar3[0xf] >> 1;
      *(short *)((int)puVar3 + 0x3a) = -(*(short *)((int)puVar3 + 0x3a) >> 2);
      local_30 = 0;
      local_34 = 0;
      local_38 = 0;
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar4,&local_38);
      func_0x800453d0(1);
      cVar1 = *(char *)((int)puVar3 + 0x75);
      *(char *)((int)puVar3 + 0x75) = cVar1 + -1;
      if (cVar1 == '\0') {
        *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xfffb;
      }
    }
  }
  return;
}



/* ============ FUN_801044fc @ 0x801044fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801044fc(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 5) + '\x06';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    _DAT_800aca58 = (*(byte *)(_DAT_800ac784 + 5) - 1) * 0x100 | 6;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
    func_0x800453d0(3);
  }
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x170),
                  *(undefined4 *)(_DAT_800ac784 + 0x174));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80104620 @ 0x80104620 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104620(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar1 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar1 + 1) * 0x100 | 1;
  }
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 3000) &&
     (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  }
  return;
}



/* ============ FUN_80104714 @ 0x80104714 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104714(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar1 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar1 + 1) * 0x100 | 1;
  }
  return;
}



/* ============ FUN_801047bc @ 0x801047bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801047bc(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xffef;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x200);
  return;
}



/* ============ FUN_8010489c @ 0x8010489c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010489c(void)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x94) = (bVar2 & 1) + 0x27;
      }
      else if (bVar2 != 3) {
        return;
      }
      *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xffef;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x29;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  return;
}



/* ============ FUN_80104a04 @ 0x80104a04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104a04(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x29;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 3) == 0) {
      func_0x800453d0(5);
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80104af4 @ 0x80104af4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104af4(void)

{
  char cVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = DAT_80100068;
  local_1c = DAT_8010006c;
  local_18 = DAT_80100070;
  local_14 = DAT_80100074;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x9c) + *(int *)(_DAT_800ac784 + 0x38);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x1e;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      uVar2 = func_0x8001af20();
      if ((uVar2 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      }
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 1;
    func_0x800453d0(0);
  case 3:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x170);
    uVar4 = *(undefined4 *)(_DAT_800ac784 + 0x174);
    uVar5 = 0x200;
    goto LAB_80104e90;
  case 4:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
    func_0x80019700(0x5002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&local_20);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar6 + 0x140) = 0x80;
    *(undefined4 *)(iVar6 + 0x144) = 0x20;
    *(uint *)(iVar6 + 0xac) = *(uint *)(iVar6 + 0xac) | 0x80;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 1) != 0) {
      *(undefined2 *)(iVar6 + 0x134) = 0x5f4;
      *(undefined2 *)(iVar6 + 0x136) = 0xc18;
      *(undefined2 *)(iVar6 + 0x138) = 0x5f4;
      *(uint *)(iVar6 + 0xac) = *(uint *)(iVar6 + 0xac) | 0x400;
    }
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar6 + 0x344) = 0x80;
    *(undefined4 *)(iVar6 + 0x348) = 0x20;
    *(uint *)(iVar6 + 0x2b0) = *(uint *)(iVar6 + 0x2b0) | 0x80;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 1) != 0) {
      *(undefined2 *)(iVar6 + 0x338) = 0x5f4;
      *(undefined2 *)(iVar6 + 0x33a) = 0xc18;
      *(undefined2 *)(iVar6 + 0x33c) = 0x5f4;
      *(uint *)(iVar6 + 0x2b0) = *(uint *)(iVar6 + 0x2b0) | 0x400;
    }
  case 6:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x170);
    uVar4 = *(undefined4 *)(_DAT_800ac784 + 0x174);
    uVar5 = 0x100;
LAB_80104e90:
    cVar1 = func_0x8001f314(uVar3,uVar4,0,uVar5);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 7:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
  }
  return;
}



/* ============ FUN_80104f34 @ 0x80104f34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f34(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 9) = 0;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 4;
      if (*(char *)(_DAT_800ac784 + 0x9f) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
        *(undefined4 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 1;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),
                          (int)*(char *)(_DAT_800ac784 + 0x9f),0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_801050e0 @ 0x801050e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801050e0(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  uint uVar4;
  undefined4 uVar5;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 3) == 0) {
      uVar4 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar4 & 1) != 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x80;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
  case 1:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_80109350(0,0);
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    uVar4 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) =
         (short)*(undefined4 *)(&DAT_80119038 + (uVar4 & 0xf) * 4) * 0x1e;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  case 3:
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      uVar4 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar4 & 1) != 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
    }
  case 5:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    break;
  case 6:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1dc) = (uVar3 & 3) + 4;
  }
  return;
}



/* ============ FUN_801054a8 @ 0x801054a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801054a8(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x2a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 4:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xd01;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  }
  return;
}



/* ============ FUN_801055d0 @ 0x801055d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801055d0(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) == 0) ||
     (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))) {
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) &&
       (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x2c8), iVar2 != 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
    }
    if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
        (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar2 == 0)) &&
       (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
      iVar2 = func_0x8001a780(&DAT_800aca54);
      *(uint *)(_DAT_800ac784 + 4) = (iVar2 + 3) * 0x100 | 1;
    }
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 0x5dc) && (_DAT_800acaee < 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x1000) != 0) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x1001;
    }
  }
  else {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
  }
  return;
}



/* ============ FUN_80105770 @ 0x80105770 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105770(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 uVar6;
  short sVar7;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 7) + 8;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar5 = func_0x8001af20();
    if ((uVar5 & 3) == 0) {
      uVar5 = func_0x8001af20();
      uVar6 = 5;
      if ((uVar5 & 1) != 0) {
        uVar6 = 4;
      }
      func_0x800453d0(uVar6);
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&DAT_80119078 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011907a + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                  (int)sVar7);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&DAT_80119078 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80105a40 @ 0x80105a40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a40(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee6f48))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  else if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801069ec();
  }
  else {
    FUN_80106854();
  }
  return;
}



/* ============ FUN_80105b30 @ 0x80105b30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105b30(void)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  bVar2 = *(byte *)(_DAT_800ac784 + 7);
  iVar5 = iVar3 + 0x4b4;
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 7) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
      }
      else if (bVar2 != 3) goto LAB_80105fe0;
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      *(short *)(iVar3 + 0x518) = *(short *)(iVar3 + 0x518) + *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x80;
      func_0x80068098(iVar3 + 0x514,iVar3 + 0x4cc);
      cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
      *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
      if (cVar1 == '\0') {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x10201;
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 8;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        if (DAT_800aca4e < 5) {
          uVar4 = func_0x8001af20();
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80119028)[uVar4 & 0x1f];
        }
        else {
          uVar4 = func_0x8001af20();
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80119018)[uVar4 & 0x1f];
        }
        if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) =
             *(undefined2 *)
              (&DAT_80118f18 +
              *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        uVar4 = func_0x8001af20();
        if ((uVar4 & 0xf) == 0) {
          func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
          *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
        }
        iVar3 = func_0x8001a780(&DAT_800aca54);
        if (iVar3 != 0) {
          *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
          func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
        }
      }
      goto LAB_80105fe0;
    }
    if (bVar2 != 0) goto LAB_80105fe0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
              *(int *)(_DAT_800ac784 + 0x188);
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0x40,&LAB_801193ac);
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    (*(code *)(&PTR_LAB_80119358)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(iVar5 + 100) = *(short *)(iVar5 + 100) + *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x80;
  func_0x80068098(iVar5 + 0x60,iVar5 + 0x18);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
LAB_80105fe0:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80105ffc @ 0x80105ffc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105ffc(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        uVar3 = func_0x8001af20();
        if ((uVar3 & 7) == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        }
        iVar4 = func_0x8001a780(&DAT_800aca54);
        if (iVar4 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        }
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
      goto LAB_80106220;
    }
    if (bVar1 != 0) goto LAB_80106220;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar3 = func_0x8001af20();
    *(undefined *)(_DAT_800ac784 + 0x94) = (&DAT_801193bc)[uVar3 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801193c4);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
    (*(code *)(&PTR_LAB_80119358)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_80106220:
  FUN_80109350(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_80106244 @ 0x80106244 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106244(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_801063c0:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_801065bc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_801065bc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (*(code *)(&PTR_LAB_80119358)[*(byte *)(_DAT_800ac784 + 5)])();
      goto LAB_801063c0;
    }
    if (bVar1 != 2) {
      if ((bVar1 == 3) &&
         (iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                                  *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200), iVar3 != 0)) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        uVar4 = func_0x8001af20();
        if ((uVar4 & 7) == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        }
        iVar3 = func_0x8001a780(&DAT_800aca54);
        if (iVar3 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        }
      }
      goto LAB_801065bc;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_801065bc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  if (cVar2 == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
  }
LAB_801065bc:
  FUN_80109350(0,0);
  return;
}



/* ============ FUN_801065d8 @ 0x801065d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801065d8(void)

{
  uint uVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x93) >> 7;
    *(char *)(_DAT_800ac784 + 0x94) = '\x10' - *(char *)(_DAT_800ac784 + 0x9f);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
    uVar1 = func_0x8001af20();
    if ((uVar1 & 1) == 0) {
      func_0x800453d0(6);
    }
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x400);
  if (iVar2 != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    uVar1 = func_0x8001af20();
    if ((uVar1 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    }
    iVar2 = func_0x8001a780(&DAT_800aca54);
    if (iVar2 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    }
  }
  func_0x800245d8(*(char *)(_DAT_800ac784 + 0x9f) * -0x800 + 0x800);
  *(short *)(_DAT_800ac784 + 0x8c) =
       *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
  if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
  }
  return;
}



/* ============ FUN_80106854 @ 0x80106854 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106854(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 1;
      if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 3) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801193c4);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    if (*(short *)(_DAT_800ac784 + 0x1dc) == 0x80) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x400);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_801069ec @ 0x801069ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801069ec(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 1;
      if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 3) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar1 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_801193c4);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_80106b58 @ 0x80106b58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106b58(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee6c2c))();
  }
  else {
    FUN_80107c64();
  }
  return;
}



/* ============ FUN_80106bcc @ 0x80106bcc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106bcc(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80106e54;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80119694);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1038;
    }
    else {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1058;
    }
    func_0x8004ef90(uVar4,uVar1);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
LAB_80106e54:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,0);
  }
  return;
}



/* ============ FUN_80106e90 @ 0x80106e90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106e90(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_801071b4;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    puVar7 = (uint *)((uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80119694);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80119694);
    puVar7[0x1a] = 0x8f;
    *(undefined2 *)((int)puVar7 + 0x96) = 0xffce;
    *(undefined2 *)(puVar7 + 0x25) = 0;
    *(undefined2 *)(puVar7 + 0x26) = 0;
    *(undefined2 *)((int)puVar7 + 0x9a) = 3;
    *(undefined2 *)(puVar7 + 0x27) = 0;
    *(undefined2 *)((int)puVar7 + 0x9e) = 0;
    *puVar7 = *puVar7 | 0x4a;
    func_0x800453d0(9);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar5 + 0x548) = 0x80;
    *(undefined4 *)(iVar5 + 0x54c) = 0x20;
    *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
    if (_DAT_800b0fe0 < 3) {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1038;
    }
    else {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1058;
    }
    func_0x8004ef90(uVar6,uVar1);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
LAB_801071b4:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,0);
  }
  return;
}



/* ============ FUN_801071f8 @ 0x801071f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801071f8(void)

{
  undefined1 uVar1;
  char cVar2;
  short sVar3;
  byte bVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x28;
    bVar4 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar4 & 0x3f) + 0x3c;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar4 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar4 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    puVar7 = (uint *)((uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80119694);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80119694);
    puVar7[0x1a] = 0x8f;
    *(undefined2 *)((int)puVar7 + 0x96) = 0xffce;
    *(undefined2 *)(puVar7 + 0x25) = 0;
    *(undefined2 *)(puVar7 + 0x26) = 0;
    *(undefined2 *)((int)puVar7 + 0x9a) = 3;
    *(undefined2 *)(puVar7 + 0x27) = 0;
    *(undefined2 *)((int)puVar7 + 0x9e) = 0;
    *puVar7 = *puVar7 | 0x4a;
    func_0x800453d0(9);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar5 + 0x548) = 0x80;
    *(undefined4 *)(iVar5 + 0x54c) = 0x20;
    *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    if (_DAT_800b0fe0 < 3) {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1038;
    }
    else {
      uVar1 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1058;
    }
    func_0x8004ef90(uVar6,uVar1);
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar3 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar3 + -1;
    if (sVar3 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar2 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801075e8 @ 0x801075e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801075e8(void)

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
switchD_80107624_caseD_3:
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
                      &DAT_801196a4);
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
      FUN_80109350(0,0);
    }
    else {
      FUN_80109350(0,1);
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
    FUN_80109350(0,1);
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
    goto switchD_80107624_caseD_3;
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



/* ============ FUN_80107c64 @ 0x80107c64 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80107c64(void)

{
  byte bVar1;
  undefined1 uVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (bVar1 != 0) {
      return (uint)(bVar1 < 2);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1058;
    }
    func_0x8004ef90(uVar4,uVar2);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
    uVar3 = func_0x8001af20();
    uVar4 = 8;
    if ((uVar3 & 1) != 0) {
      uVar4 = 5;
    }
    func_0x800453d0(uVar4);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)uVar3;
  return uVar3;
}



/* ============ FUN_80107e94 @ 0x80107e94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107e94(void)

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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar9 + 0x10,&LAB_80119694);
    func_0x800453d0(9);
    uVar10 = func_0x8001af20();
    if ((uVar10 & 1) != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      FUN_80108cb8((uint)(byte)(&DAT_80118cac)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62])
      ;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108cb8(uVar6 + 0x560);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x60c) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x5e8) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x5ea) = 0xc18;
        *(undefined2 *)(uVar6 + 0x5ec) = 0x5f4;
        *(uint *)(uVar6 + 0x560) = *(uint *)(uVar6 + 0x560) | 0x400;
        FUN_80108cb8(_DAT_800ac784[0x62] + 0x60c);
      }
      *(undefined4 *)(_DAT_800ac784[0x62] + 0x6b8) = 0;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108cb8(uVar6 + 0x764);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x810) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x7ec) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x7ee) = 0xc18;
        *(undefined2 *)(uVar6 + 0x7f0) = 0x5f4;
        *(uint *)(uVar6 + 0x764) = *(uint *)(uVar6 + 0x764) | 0x400;
        FUN_80108cb8(_DAT_800ac784[0x62] + 0x810);
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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_80119694);
    func_0x80019700(0x5002800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,
                    &LAB_80119694);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62]
                    + 0x40,&LAB_80119694);
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
                        &LAB_80119694);
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
        goto joined_r0x80108704;
      }
      *(uint *)(_DAT_800ac784[0x62] + 0x560) = *(uint *)(_DAT_800ac784[0x62] + 0x560) | 0x200;
      *(uint *)(_DAT_800ac784[0x62] + 0x764) = *(uint *)(_DAT_800ac784[0x62] + 0x764) | 0x200;
    }
    else {
      uVar1 = *(ushort *)(uVar8 + 0x536);
      *(char *)(uVar8 + 0x529) = cVar3 + -1;
joined_r0x80108704:
      if ((uVar1 & 1) != 0) {
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar8 + 0x4f4,
                        &LAB_80119694);
      }
    }
    cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_801089fc:
    if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
      FUN_80109350(0,0);
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
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_80119694);
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
    goto LAB_801089fc;
  case 4:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}



/* ============ FUN_80108a70 @ 0x80108a70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108a70(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
      *(undefined4 *)(_DAT_800ac784 + 4) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x1e;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108cb8(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0x140) = 0x80;
    *(undefined4 *)(iVar4 + 0x144) = 0x20;
    *(uint *)(iVar4 + 0xac) = *(uint *)(iVar4 + 0xac) | 0x80;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0x344) = 0x80;
    *(undefined4 *)(iVar4 + 0x348) = 0x20;
    *(uint *)(iVar4 + 0x2b0) = *(uint *)(iVar4 + 0x2b0) | 0x80;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) != 0) {
      *(undefined4 *)(iVar4 + 0x2b0) = 0;
    }
    uVar3 = func_0x8001af20();
    uVar5 = 6;
    if ((uVar3 & 1) == 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_80108cb8 @ 0x80108cb8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108cb8(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&LAB_80119694);
  param_1[0x1a] = 0x8f;
  *(undefined2 *)(param_1 + 0x25) = 0;
  *(undefined2 *)((int)param_1 + 0x96) = 0;
  *(undefined2 *)(param_1 + 0x26) = 0;
  *(undefined2 *)((int)param_1 + 0x9a) = 0;
  *(undefined2 *)(param_1 + 0x27) = 0;
  *(undefined2 *)((int)param_1 + 0x9e) = 0;
  *param_1 = *param_1 | 0x4a;
  return;
}



/* ============ FUN_80108d28 @ 0x80108d28 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108d28(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  uint uVar4;
  undefined4 uVar5;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *(undefined2 *)(_DAT_800ac784 + 1) = 7;
      }
      goto LAB_80109118;
    }
    if (bVar1 != 0) goto LAB_80109118;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    uVar4 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar4 & 1) != 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
    *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
    uVar4 = (ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccc)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccd)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cce)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccf)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd0)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd1)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd2)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd3)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1058;
    }
    func_0x8004ef90(uVar5,uVar2);
  }
  cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_80109118:
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,0);
  }
  return;
}



/* ============ FUN_80109150 @ 0x80109150 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109150(void)

{
  (**(code **)(&DAT_801196b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80109198 @ 0x80109198 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109198(void)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 3) == 0) {
      uVar2 = func_0x8001af20();
      uVar3 = 5;
      if ((uVar2 & 1) != 0) {
        uVar3 = 4;
      }
      func_0x800453d0(uVar3);
    }
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_80109350(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801092a8 @ 0x801092a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801092a8(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,1);
  }
  return;
}



/* ============ FUN_80109350 @ 0x80109350 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109350(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0x204;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_80109424 @ 0x80109424 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109424(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x4cc,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0x6b8;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_80109508 @ 0x80109508 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109508(void)

{
  short *psVar1;
  char cVar2;
  uint *puVar3;
  byte bVar4;
  undefined1 uVar5;
  ushort uVar6;
  int iVar7;
  uint *puVar8;
  undefined4 uVar9;
  uint uVar10;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 5)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    func_0x80039a74(_DAT_800ac784);
    puVar3 = _DAT_800ac784;
    puVar8 = _DAT_800ac784 + 0x3b;
    _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xffff38;
    puVar3[0x3b] = *puVar8 & 0xff000000 | 0xffff38;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 1;
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
    }
    if ((char)_DAT_800ac784[0x25] == '\v') {
      *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x25) = *(char *)((int)_DAT_800ac784 + 0x9f) + '\x14';
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
    uVar9 = 0x800b1038;
    if (_DAT_800b0fe0 < 3) {
      uVar5 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
    }
    else {
      uVar5 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar9 = 0x800b1058;
    }
    func_0x8004ef90(uVar9,uVar5);
  case 1:
    puVar8 = _DAT_800ac784;
    psVar1 = (short *)((int)_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0x2f) = (short)_DAT_800ac784[0x2f] + 8;
    *(short *)((int)puVar8 + 0xbe) = *psVar1 + 8;
    cVar2 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar2 + -1;
    if (cVar2 == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
      bVar4 = func_0x8001af20();
      *(byte *)((int)_DAT_800ac784 + 0x9e) = (bVar4 & 0x1f) + 1;
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 500;
      if ((_DAT_800ac784[0x76] & 0x100) != 0) {
        *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
        *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1000;
      }
    }
    if ((short)_DAT_800ac784[0x27] == 0) {
      iVar7 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
      if (iVar7 != 0) {
        uVar6 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x27) = (uVar6 & 7) + 1;
      }
    }
    else {
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1;
    }
    break;
  case 2:
    cVar2 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar2 + -1;
    if (cVar2 == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    }
    uVar10 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar10 + -1;
    if ((short)uVar10 != 0) {
      return;
    }
    uVar5 = 4;
    goto LAB_80109d28;
  case 3:
    iVar7 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x400);
    if (iVar7 != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
      bVar4 = func_0x8001af20();
      *(byte *)((int)_DAT_800ac784 + 0x9e) = (bVar4 & 0x1f) + 1;
    }
    if ((_DAT_800ac784[0x76] & 0x100) != 0) {
      *(short *)((int)_DAT_800ac784 + 0x166) = *(short *)((int)_DAT_800ac784 + 0x166) + -2;
    }
    break;
  case 4:
    if ((_DAT_800ac784[0x76] & 0x100) == 0) {
      return;
    }
    uVar6 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x27) = (uVar6 & 0xff) + 100;
    uVar5 = 5;
    goto LAB_80109d28;
  case 5:
    uVar10 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar10 + -1;
    if ((short)uVar10 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 6;
    }
    *(short *)((int)_DAT_800ac784 + 0x166) = *(short *)((int)_DAT_800ac784 + 0x166) + -2;
    if (*(short *)((int)_DAT_800ac784 + 0x166) < 100) {
      *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 100;
    }
    break;
  case 6:
    uVar10 = (ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8;
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118ccc)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118ccd)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118cce)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118ccf)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118cd0)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118cd1)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118cd2)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_80118cd3)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    puVar8 = (uint *)_DAT_800ac784[0x62];
    puVar8[0x25] = 0;
    puVar8[0x26] = 0;
    *puVar8 = *puVar8 | 0x80;
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x548) = 0;
    *(undefined4 *)(uVar10 + 0x54c) = 0;
    *(uint *)(uVar10 + 0x4b4) = *(uint *)(uVar10 + 0x4b4) | 0x80;
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x5f4) = 0;
    *(undefined4 *)(uVar10 + 0x5f8) = 0;
    *(uint *)(uVar10 + 0x560) = *(uint *)(uVar10 + 0x560) | 0x80;
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x9fc) = 0;
    *(undefined4 *)(uVar10 + 0xa00) = 0;
    *(uint *)(uVar10 + 0x968) = *(uint *)(uVar10 + 0x968) | 0x80;
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x7f8) = 0;
    *(undefined4 *)(uVar10 + 0x7fc) = 0;
    *(uint *)(uVar10 + 0x764) = *(uint *)(uVar10 + 0x764) | 0x80;
    uVar10 = _DAT_800ac784[0x62];
    *(uint *)(uVar10 + 0x60c) = *(uint *)(uVar10 + 0x60c) | 0x80;
    *(undefined4 *)(uVar10 + 0x6a0) = 0;
    *(undefined4 *)(uVar10 + 0x6a4) = 0;
    uVar10 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar10 + 0x8a4) = 0;
    *(undefined4 *)(uVar10 + 0x8a8) = 0;
    *(uint *)(uVar10 + 0x810) = *(uint *)(uVar10 + 0x810) | 0x80;
    uVar5 = 7;
LAB_80109d28:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar5;
  }
  return;
}



/* ============ FUN_80109d4c @ 0x80109d4c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d4c(void)

{
  byte bVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x180),
                          *(undefined4 *)(_DAT_800ac784 + 0x184),0,0x200);
  if (iVar2 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80109e00 @ 0x80109e00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109e00(void)

{
  char cVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  uint *puVar6;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar2 & 1) + 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar3 & 1) == 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x46;
    }
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x180),
                            *(undefined4 *)(_DAT_800ac784 + 0x184),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (*(char *)(_DAT_800ac784 + 0x95) == 'K') {
      uVar3 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar3 & 1) == 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined4 *)(iVar4 + 0x548) = 0x80;
      *(undefined4 *)(iVar4 + 0x54c) = 0x20;
      *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
      puVar6 = *(uint **)(_DAT_800ac784 + 0x188);
      puVar6[0x26] = 0x20;
      puVar6[0x25] = 0x80;
      *(undefined2 *)(puVar6 + 0x22) = 0x5f4;
      *(undefined2 *)(puVar6 + 0x23) = 0x5f4;
      *puVar6 = *puVar6 | 0x80;
      *(undefined2 *)((int)puVar6 + 0x8a) = 0xc18;
      *puVar6 = *puVar6 | 0x400;
      func_0x80019700(0x2800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar6 + 0x10,&DAT_80118cfc);
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0xac) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0xac) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x2b0) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x2b0) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x35c) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x35c) | 0x40;
    }
    break;
  case 2:
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar2 & 0x1f) + 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
  case 3:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9f);
    *(char *)(_DAT_800ac784 + 0x9f) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 3) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 9) = 0x83;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x501;
    }
  }
  return;
}



/* ============ FUN_8010a180 @ 0x8010a180 ============ */

void FUN_8010a180(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a1bc @ 0x8010a1bc ============ */

void FUN_8010a1bc(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a1f8 @ 0x8010a1f8 ============ */

void FUN_8010a1f8(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ============ FUN_8010a21c @ 0x8010a21c ============ */

void FUN_8010a21c(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ============ FUN_8010a6ac @ 0x8010a6ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a6ac(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801196f0
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801196f0
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  return;
}



/* ============ FUN_8010a87c @ 0x8010a87c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a87c(void)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar2 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10
    ;
    iVar3 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10
    ;
    uVar4 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar4;
    if ((*(char *)(_DAT_800ac784 + 0x1d5) != '\0') &&
       (cVar1 = *(char *)(_DAT_800ac784 + 0x1d5) + -1, *(char *)(_DAT_800ac784 + 0x1d5) = cVar1,
       cVar1 == '\0')) {
      _DAT_800aca50 = _DAT_800aca50 & 0xfffe;
    }
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(&DAT_800aca88,*(undefined1 *)(_DAT_800ac784 + 0x1d6),
                    *(ushort *)(_DAT_800ac784 + 0x1d8) & 8);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xfff7;
    (*(code *)(&PTR_FUN_80119730)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0) {
      *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xed;
    }
    func_0x8001b4e4();
    FUN_8010412c();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),
                    *(undefined1 *)(_DAT_800ac784 + 0x1d7));
    func_0x8001b38c();
    func_0x800437d4(*(undefined4 *)(_DAT_800ac784 + 0x34),*(undefined4 *)(_DAT_800ac784 + 0x3c));
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_8010aae0 @ 0x8010aae0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aae0(void)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  byte bVar4;
  int iVar5;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  local_38 = DAT_80100004;
  local_34 = DAT_80100008;
  local_30 = DAT_8010000c;
  local_2c = DAT_80100010;
  local_28 = DAT_80100014;
  local_24 = DAT_80100018;
  local_20 = DAT_8010001c;
  local_1c = DAT_80100020;
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  uVar1 = func_0x8001af20();
  *(ushort *)((int)_DAT_800ac784 + 0x9a) = (uVar1 & 0x1f) + 0x32;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80118cb8;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_80118d0c)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 8;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar3 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar3 + 0x5f8) = 0x60;
  *(undefined2 *)(uVar3 + 0x5fa) = 0x30;
  *(undefined2 *)(uVar3 + 0x5fc) = 0x390;
  *(undefined2 *)(uVar3 + 0x5f4) = 0;
  *(undefined2 *)(uVar3 + 0x5f6) = 0;
  *(undefined2 *)(uVar3 + 0x5fe) = 0x138;
  if ((_DAT_800aca3c & 1) != 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 0x100;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar3 & 1) << 9);
    uVar1 = func_0x8001af20();
    uVar2 = func_0x8001af20();
    iVar5 = (uVar3 & 1) * 8;
    *(ushort *)((int)_DAT_800ac784 + 0x1da) = (uVar2 & 0xff) + (uVar1 & 0xff) + 600;
    func_0x80019700(0x8031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccc)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccd)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cce)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118ccf)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd0)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_80118cfc);
  }
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  bVar4 = *(byte *)((int)_DAT_800ac784 + 9) & 0xf;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar4 == 4) || (bVar4 == 7)) || (bVar4 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
    }
    bVar4 = *(byte *)((int)_DAT_800ac784 + 9) & 0xf;
    if (((bVar4 == 5) || (bVar4 == 8)) || (bVar4 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    }
    bVar4 = *(byte *)((int)_DAT_800ac784 + 9) & 0xf;
    if ((bVar4 == 1) || (bVar4 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    bVar4 = *(byte *)((int)_DAT_800ac784 + 9) & 0xf;
    if (((bVar4 == 4) || (bVar4 == 7)) || (bVar4 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x12;
    }
    bVar4 = *(byte *)((int)_DAT_800ac784 + 9) & 0xf;
    if (((bVar4 == 5) || (bVar4 == 8)) || (bVar4 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x14;
    }
  }
  if (*(char *)((int)_DAT_800ac784 + 9) == '\x06') {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x27;
    _DAT_800ac784[1] = 0x20c00;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  return;
}



/* ============ FUN_8010b228 @ 0x8010b228 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b228(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccc)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccd)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cce)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118ccf)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd0)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd1)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd2)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_80118cd3)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      return;
    }
  }
  uVar1 = func_0x8001bc08();
  if ((uVar1 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar1 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
  }
  (*(code *)(&PTR_FUN_80119758)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010b688 @ 0x8010b688 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b688(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_8011978c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801197d0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b738 @ 0x8010b738 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b738(void)

{
  (*(code *)(&PTR_FUN_80119814)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80119830)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b7b4 @ 0x8010b7b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b7b4(void)

{
  (*(code *)(&PTR_FUN_8011984c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801197d0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b830 @ 0x8010b830 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b830(void)

{
  (*(code *)(&PTR_FUN_80119884)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80119830)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b8ac @ 0x8010b8ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8ac(void)

{
  (*(code *)(&PTR_FUN_8011989c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198a0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b928 @ 0x8010b928 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b928(void)

{
  (*(code *)(&PTR_FUN_801198a4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b9a4 @ 0x8010b9a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9a4(void)

{
  (*(code *)(&PTR_LAB_801198ac)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ba20 @ 0x8010ba20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba20(void)

{
  (*(code *)(&PTR_FUN_801198b0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ba9c @ 0x8010ba9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba9c(void)

{
  (*(code *)(&PTR_FUN_801198b4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198b8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bb18 @ 0x8010bb18 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb18(void)

{
  (*(code *)(&PTR_LAB_801198bc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801198c0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bb9c @ 0x8010bb9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb9c(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x474;
        *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4;
      }
      else if (bVar1 != 3) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x474;
        *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar2 = func_0x8001aa68((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  if (sVar2 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}



/* ============ FUN_8010be04 @ 0x8010be04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be04(void)

{
  undefined4 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x10
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x100);
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) != 0) {
        return;
      }
      uVar1 = 0;
    }
    else {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) != 0) {
        return;
      }
      uVar1 = 1;
    }
    FUN_8010c03c(0,uVar1);
  }
  return;
}



/* ============ FUN_8010bf34 @ 0x8010bf34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf34(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee673c))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_801069ec();
  }
  else {
    FUN_80106854();
  }
  return;
}



/* ============ FUN_8010bfc8 @ 0x8010bfc8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bfc8(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee649c))();
  }
  else {
    FUN_80107c64();
  }
  return;
}



/* ============ FUN_8010c03c @ 0x8010c03c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c03c(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x4cc,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0x764;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_8010c1a0 @ 0x8010c1a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c1a0(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_80119ed4)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) == 0) {
    *(short *)(_DAT_800ac784 + 0x1d8) =
         *(short *)(*(int *)(_DAT_800ac784 + 0x78) + 6) + 100 +
         (short)(*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 3);
    if (*(short *)(_DAT_800ac784 + 0x1d8) < 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 100;
    }
    *(undefined2 *)(_DAT_800ac784 + 0xb4) = *(undefined2 *)(_DAT_800ac784 + 0x1d8);
    *(short *)(_DAT_800ac784 + 0xb6) = *(short *)(_DAT_800ac784 + 0xb4) + 100;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_8010c338 @ 0x8010c338 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c338(void)

{
  uint uVar1;
  char cVar2;
  
  _DAT_800ac784[1] = 1;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 1;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 2) != 0) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 1;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 1;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 4) != 0) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x79) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 8) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x79) = 4;
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_801185a0 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_80119e74)[(byte)_DAT_800ac784[0x79]];
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = _DAT_800ac784[0x62];
  *(uint *)(uVar1 + 0xc84) = uVar1 + 0xec;
  *(uint *)(uVar1 + 0xca8) = uVar1 + 0xac;
  *(undefined4 *)(uVar1 + 0xc44) = 0x66;
  *(undefined4 *)(uVar1 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar1 + 0xc4c) = 0;
  *(undefined2 *)(uVar1 + 0xc78) = 0;
  *(undefined2 *)(uVar1 + 0xc7a) = 0;
  *(undefined2 *)(uVar1 + 0xc7c) = 0;
  func_0x80068098(uVar1 + 0xc78,uVar1 + 0xc30);
  return;
}



/* ============ FUN_8010c6bc @ 0x8010c6bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c6bc(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar1;
  if ((ushort)((ushort)_DAT_800ac784[0x76] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x76] & 1;
  }
  uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
  (*(code *)(&PTR_LAB_80119ef4)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80119f34)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(4000);
  if ((char)_DAT_800ac784[0x78] != '\0') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    func_0x80012a0c(5000);
  }
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  }
  else {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
  }
  return;
}



/* ============ FUN_8010c9b0 @ 0x8010c9b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c9b0(void)

{
  short sVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    sVar1 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8010cba0 @ 0x8010cba0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cba0(void)

{
  short sVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    sVar1 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8010cc9c @ 0x8010cc9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc9c(void)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      bVar1 = *(byte *)(_DAT_800ac784 + 7);
      if (bVar1 == 1) {
        uVar3 = 4;
      }
      else {
        if (1 < bVar1) {
          if (bVar1 != 2) {
            return;
          }
          *(undefined1 *)(_DAT_800ac784 + 5) = 3;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 1;
          return;
        }
        uVar3 = 3;
        if (bVar1 != 0) {
          return;
        }
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    bVar1 = *(byte *)(_DAT_800ac784 + 7);
    if (bVar1 == 1) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    }
    else if (bVar1 < 2) {
      if (bVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      }
    }
    else if (bVar1 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    }
  }
  if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80111208(0,0);
  return;
}



/* ============ FUN_8010ce78 @ 0x8010ce78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ce78(void)

{
  undefined1 uVar1;
  int iVar2;
  
  if (((DAT_800acae7 != '\0') || (iVar2 = func_0x8001a804(3000,0x180,&DAT_800aca88), -1 < iVar2)) ||
     (uVar1 = 5, *(short *)(_DAT_800ac784 + 0x1dc) != 0)) {
    if (DAT_800acae7 != '\0') {
      return;
    }
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    if (DAT_800acae7 != '\0') {
      return;
    }
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
      return;
    }
    uVar1 = 4;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar1;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_8010cfb0 @ 0x8010cfb0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cfb0(void)

{
  char cVar1;
  byte bVar2;
  char cVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    FUN_80111208(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x18);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = '\x18';
  }
  else {
    FUN_80111208(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x29);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = ')';
  }
  if ((cVar1 == '\x01') || (cVar1 == cVar3)) {
    func_0x800453d0(8);
  }
  return;
}



/* ============ FUN_8010d160 @ 0x8010d160 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d160(void)

{
  int iVar1;
  uint uVar2;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(4000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  else {
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      return;
    }
    if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
      return;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
      return;
    }
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
      return;
    }
    iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
    if (iVar1 != 0) {
      return;
    }
    if ((_DAT_800aca58 != 0x701) && (uVar2 = func_0x8001af20(), (uVar2 & 1) == 0)) {
      return;
    }
    iVar1 = func_0x8001a780(&DAT_800aca54);
    if (iVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_8010d2a8 @ 0x8010d2a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d2a8(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0xb4;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 0x40;
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((uint)*(byte *)(_DAT_800ac784 + 0x95) % 0xd == 1) {
    func_0x800453d0(8);
  }
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010d408 @ 0x8010d408 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d408(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  undefined2 uVar5;
  ushort uVar6;
  int iVar7;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        return;
      }
      if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  FUN_80111208(0,0);
  iVar7 = *(int *)(_DAT_800ac784 + 0x188);
  local_24 = _DAT_80072d64;
  local_20 = _DAT_80072d68;
  local_1c = _DAT_80072d6c;
  local_28 = 100;
  if (*(char *)(_DAT_800ac784 + 0x95) == '\v') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    uVar5 = func_0x8001bff8(iVar7 + 0x64c,&local_28,1000,&DAT_800aca88);
    uVar6 = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x80119f78)) {
        bVar3 = true;
      }
      bVar1 = uVar6 != 0;
      uVar6 = uVar6 - 1;
    } while (bVar1);
    if ((bVar3) && (*(short *)(_DAT_800ac784 + 0x1da) != 0)) {
      _DAT_800acaee = _DAT_800acaee + -5;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
      DAT_800aca58 = 2;
      cVar4 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar4 + '\x02';
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



/* ============ FUN_8010d6e4 @ 0x8010d6e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d6e4(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  undefined1 uVar5;
  undefined2 uVar6;
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
      if (bVar2 != 2) {
        return;
      }
      if ((DAT_800acae7 == 0) && (iVar7 = func_0x8001a804(4000,0x180,&DAT_800aca88), iVar7 < 0)) {
        uVar5 = 3;
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          uVar5 = 5;
        }
      }
      else {
        uVar5 = 3;
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
          uVar5 = 3;
        }
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_801112dc(0,0);
  }
  else {
    FUN_80111208(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar8 + 0x80119f7c)) {
        bVar3 = true;
      }
      bVar1 = uVar8 != 0;
      uVar8 = uVar8 - 1;
    } while (bVar1);
    if ((bVar3) &&
       ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
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
    }
  }
  return;
}



/* ============ FUN_8010da9c @ 0x8010da9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010da9c(void)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
        if ((bVar1 & 2) != 0) {
          if ((bVar1 & 0x40) != 0) {
            *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 2;
            *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            *(undefined1 *)(_DAT_800ac784 + 6) = 3;
            goto LAB_8010ddf4;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
          local_28 = DAT_80119eb4;
          local_24 = DAT_80119eb8;
          local_20 = DAT_80119ebc;
          local_1c = DAT_80119ec0;
          func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                          *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
        iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                                (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar3 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(1);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
          return;
        }
        if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
          return;
        }
        if (0x31 < _DAT_800acaee) {
          return;
        }
        if (DAT_800acae7 != '\0') {
          return;
        }
        iVar3 = func_0x8001a804(5000,0xc0,&DAT_800aca88);
        if (-1 < iVar3) {
          return;
        }
        if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        func_0x800453d0(6);
        return;
      }
      if (bVar1 != 3) {
        return;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_8010ddf4:
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_8010de04;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0xb4;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 200;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
LAB_8010de04:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010de24 @ 0x8010de24 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010de24(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  undefined2 uVar4;
  int iVar5;
  ushort uVar6;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        return;
      }
      iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar5 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (-1 < _DAT_800acaee) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
        return;
      }
      func_0x800245d8(0);
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
  bVar2 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar2 & 2) != 0) {
    if ((bVar2 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 3;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar2 & 0xfd;
    local_48 = DAT_80119eb4;
    local_44 = DAT_80119eb8;
    local_40 = DAT_80119ebc;
    local_3c = DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_48);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
  iVar5 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                          (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                          (int)*(short *)(_DAT_800ac784 + 0x1ba));
  if (iVar5 == 0) {
    if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
      func_0x800453d0(9);
    }
    if (DAT_800acae7 == 0) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      local_38 = _DAT_80072d60;
      local_34 = _DAT_80072d64;
      local_30 = _DAT_80072d68;
      local_2c = _DAT_80072d6c;
      uVar4 = func_0x8001bff8(iVar5 + 0x448,&local_38,800,&DAT_800aca88);
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar4;
      uVar4 = func_0x8001bff8(iVar5 + 0x6f8,&local_38,800,&DAT_800aca88);
      uVar6 = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar4;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x80119f84)) {
          bVar3 = true;
        }
        bVar1 = uVar6 != 0;
        uVar6 = uVar6 - 1;
      } while (bVar1);
      if ((bVar3) &&
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
      }
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}



/* ============ FUN_8010e300 @ 0x8010e300 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e300(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  if (1 < *(byte *)(_DAT_800ac784 + 6)) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
    if ((bVar1 & 2) != 0) {
      if ((bVar1 & 0x40) == 0) {
        *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
        local_28 = DAT_80119eb4;
        local_24 = DAT_80119eb8;
        local_20 = DAT_80119ebc;
        local_1c = DAT_80119ec0;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
      }
      else {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 2;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      }
    }
  }
  return;
}



/* ============ FUN_8010e3e8 @ 0x8010e3e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e3e8(void)

{
  int iVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x10') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
      *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e78)[*(byte *)(_DAT_800ac784 + 0x1e4)];
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    }
    break;
  case 2:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),400,0xffffffec,
                    (int)*(short *)(_DAT_800ac784 + 0x1ba));
    iVar1 = *(short *)(_DAT_800ac784 + 0x1ba) + -0xa8c;
    if (*(int *)(_DAT_800ac784 + 0x38) < iVar1) {
      *(int *)(_DAT_800ac784 + 0x38) = iVar1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1c;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 4:
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8010e6b0 @ 0x8010e6b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e6b0(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  if (*(byte *)(_DAT_800ac784 + 6) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
    if ((bVar1 & 2) != 0) {
      if ((bVar1 & 0x40) == 0) {
        *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
        local_28 = DAT_80119eb4;
        local_24 = DAT_80119eb8;
        local_20 = DAT_80119ebc;
        local_1c = DAT_80119ec0;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
      }
      else {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 2;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      }
    }
  }
  return;
}



/* ============ FUN_8010e798 @ 0x8010e798 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e798(void)

{
  int iVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_8010e8ec:
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 2:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x18') {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x17;
    }
    iVar1 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffba,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar1 == 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    goto LAB_8010e8ec;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e74)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  case 4:
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    break;
  default:
    break;
  }
  return;
}



/* ============ FUN_8010ea14 @ 0x8010ea14 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea14(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_80119eb4;
      local_24 = DAT_80119eb8;
      local_20 = DAT_80119ebc;
      local_1c = DAT_80119ec0;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
    else {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8010eae8 @ 0x8010eae8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eae8(void)

{
  byte bVar1;
  undefined2 uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1b;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x78;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = 1;
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 1 < 7) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x11 < 4)) {
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 0x50;
    uVar2 = func_0x8001a9cc(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
    *(short *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1da);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(short *)(_DAT_800ac784 + 0x1da) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) ||
       (5000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    }
  }
  return;
}



/* ============ FUN_8010ecb4 @ 0x8010ecb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ecb4(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_80119eb4;
      local_24 = DAT_80119eb8;
      local_20 = DAT_80119ebc;
      local_1c = DAT_80119ec0;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
    else {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8010ed88 @ 0x8010ed88 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ed88(void)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    FUN_801112dc(0,0);
    if (*(char *)(_DAT_800ac784 + 0x95) == '2') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
LAB_8010efbc:
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x32;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 3:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    cVar1 = *(char *)(_DAT_800ac784 + 6);
    uVar3 = 1;
LAB_8010ef98:
    *(char *)(_DAT_800ac784 + 6) = cVar1 + cVar2;
    FUN_801112dc(0,uVar3);
    break;
  case 4:
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    goto LAB_8010efbc;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 6:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    cVar1 = *(char *)(_DAT_800ac784 + 6);
    uVar3 = 0;
    goto LAB_8010ef98;
  default:
    break;
  }
  return;
}



/* ============ FUN_8010efec @ 0x8010efec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010efec(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  undefined1 uVar4;
  int iVar5;
  byte bVar6;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  bVar2 = false;
  bVar3 = false;
  bVar6 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar6 & 2) != 0) {
    if ((bVar6 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      goto LAB_8010f2ac;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar6 & 0xfd;
    local_38 = DAT_80119eb4;
    local_34 = DAT_80119eb8;
    local_30 = DAT_80119ebc;
    local_2c = DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar5 = func_0x8001a780(&DAT_800aca54), iVar5 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    uVar4 = 0xf;
  }
  else {
    bVar6 = 1;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_80119f89)[bVar6]) {
        bVar2 = true;
      }
      bVar1 = bVar6 != 0;
      bVar6 = bVar6 - 1;
    } while (bVar1);
    if (((!bVar2) || (DAT_800acae7 != '\0')) ||
       ((iVar5 = func_0x8001a804(5000,0x180,&DAT_800aca88), -1 < iVar5 ||
        (*(short *)(_DAT_800ac784 + 0x1dc) != 0)))) {
      bVar6 = 2;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar6 + 0x80119f88)) {
          bVar3 = true;
        }
        bVar2 = bVar6 != 0;
        bVar6 = bVar6 - 1;
      } while (bVar2);
      if (!bVar3) {
        return;
      }
      if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0xfa1) {
        return;
      }
      if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      if (*(byte *)(_DAT_800ac784 + 0x95) - 1 < 0x19) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      }
      goto LAB_8010f2ac;
    }
    uVar4 = 0xe;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_8010f2ac:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_8010f2dc @ 0x8010f2dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f2dc(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 8) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x21 < 5)) {
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x50;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x30) + 0x30;
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
      *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
    }
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  FUN_801112dc(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x19);
  return;
}



/* ============ FUN_8010f45c @ 0x8010f45c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f45c(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_80119eb4;
      local_24 = DAT_80119eb8;
      local_20 = DAT_80119ebc;
      local_1c = DAT_80119ec0;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
    else {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8010f530 @ 0x8010f530 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f530(void)

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
  FUN_801112dc(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x28);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar4 + 0x80119f8c)) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar8 + 0x80119f94)) {
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



/* ============ FUN_8010f964 @ 0x8010f964 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f964(void)

{
  char cVar1;
  short sVar2;
  undefined4 uVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) + 400;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    _DAT_800acbfc = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x28) = _DAT_800aca88;
    *(undefined2 *)((int)_DAT_800ac784 + 0xa2) = _DAT_800aca90;
    _DAT_800acaf4 = _DAT_800aca88;
    _DAT_800acaf6 = _DAT_800aca90;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    DAT_800acae7 = DAT_800acae7 | 1;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    _DAT_800aca58 = 5;
    uVar3 = 6;
    *(undefined2 *)((int)_DAT_800ac784 + 0x6a) = _DAT_800acabe;
    goto LAB_8010fc44;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\0') {
      func_0x800453d0(10);
    }
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      _DAT_800aca58 = CONCAT12(4,_DAT_800aca58);
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1e;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  case 5:
    uVar3 = 5;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x1d') {
LAB_8010fc44:
      func_0x800453d0(uVar3);
    }
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 6:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    func_0x800453d0(3);
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
    _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_80119e74)[(byte)_DAT_800ac784[0x79]];
    *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  case 7:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '1') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 8;
    }
    break;
  case 8:
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
  }
  return;
}



/* ============ FUN_8010fe30 @ 0x8010fe30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe30(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e74)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    func_0x800453d0(1);
  }
  (*(code *)(&PTR_FUN_80119fb4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010feec @ 0x8010feec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010feec(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e3) == '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar3);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar3);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar3);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_801100e0 @ 0x801100e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801100e0(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      if (0x31 < *(short *)(_DAT_800ac784 + 0x9a)) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e3) == '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar3);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar3);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar3);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80111208(0,0);
  }
  else {
    FUN_80111208(0,1);
  }
  return;
}



/* ============ FUN_80110358 @ 0x80110358 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110358(void)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0x50;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar3);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar3);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar3);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
    func_0x800245d8(0x800);
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      func_0x800245d8(0);
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x11;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ============ FUN_80110654 @ 0x80110654 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110654(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e74)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    func_0x800453d0(1);
  }
  (*(code *)(&PTR_FUN_8011a00c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80110710 @ 0x80110710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110710(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,0);
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar4);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar4);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar4);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1058;
    }
    func_0x8004ef90(uVar5,uVar2);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  FUN_80111208(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) == '=') {
    func_0x800453d0(1);
  }
  return;
}



/* ============ FUN_80110920 @ 0x80110920 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110920(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar4);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar4);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar4);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1058;
    }
    func_0x8004ef90(uVar5,uVar2);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x13') {
    func_0x800453d0(1);
  }
  return;
}



/* ============ FUN_80110b54 @ 0x80110b54 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b54(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x1f) + 0x50;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar5 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80119e94 + iVar5);
    local_1c = *(undefined4 *)(&DAT_80119e98 + iVar5);
    local_18 = *(undefined4 *)(&DAT_80119e9c + iVar5);
    local_14 = *(undefined4 *)(&DAT_80119ea0 + iVar5);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar6 = 0x800b1058;
    }
    func_0x8004ef90(uVar6,uVar2);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
    func_0x800245d8(0);
  }
  return;
}



/* ============ FUN_80110dd4 @ 0x80110dd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110dd4(void)

{
  (*(code *)(&PTR_FUN_8011a064)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80110e1c @ 0x80110e1c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110e1c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0xbf;
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80110f88 @ 0x80110f88 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f88(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e78)[*(byte *)(_DAT_800ac784 + 0x1e4)];
  *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0xbf;
  *(undefined1 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = 10;
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x13;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  return;
}



/* ============ FUN_80111208 @ 0x80111208 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111208(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0x968;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_801112dc @ 0x801112dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801112dc(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0xc4,auStack_30);
  iVar1 = iVar1 + param_2 * 0x2b0 + 0x408;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_80111744 @ 0x80111744 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111744(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        return;
      }
      func_0x80037edc(0,10);
      func_0x80037edc(5,0x32);
      func_0x80037edc(7,0x32);
      _DAT_800aca58 = 7;
      return;
    }
    if (DAT_800aca5a != 0) {
      return;
    }
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011a07c
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011a07c
                   );
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + cVar1,_DAT_800aca58);
  return;
}



/* ============ FUN_801118d0 @ 0x801118d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801118d0(void)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  uVar2 = 4;
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_8011a0b4)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    if (*(char *)(_DAT_800ac784 + 9) == '\x10') {
      uVar2 = 8;
    }
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,uVar2);
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0xb4) = 1000;
    *(short *)(_DAT_800ac784 + 0xb6) = *(short *)(_DAT_800ac784 + 0xb4) + 100;
    if (*(int *)(_DAT_800ac784 + 0x38) < (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
      *(undefined2 *)(_DAT_800ac784 + 0xb4) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb6) = 0;
    }
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_80111a64 @ 0x80111a64 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111a64(void)

{
  uint uVar1;
  char cVar2;
  
  _DAT_800ac784[1] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0x19;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  if (*(char *)((int)_DAT_800ac784 + 9) == 'C') {
    _DAT_800ac784[1] = 0x204;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_801185a0 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011a0ac;
  func_0x8001af5c(0,0,1000,0x44c,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ============ FUN_80111c98 @ 0x80111c98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c98(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar1;
  if ((ushort)((ushort)_DAT_800ac784[0x76] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x76] & 1;
  }
  uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
  (*(code *)(&PTR_LAB_8011a0d4)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011a114)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x18) == 0) {
    if (1 < (byte)_DAT_800ac784[0x25]) {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
    }
    func_0x80012974(4000);
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1de) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  }
  else {
    *(short *)(_DAT_800ac784 + 0x78) = (short)_DAT_800ac784[0x78] + 1;
  }
  return;
}



/* ============ FUN_80111ee4 @ 0x80111ee4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111ee4(void)

{
  short sVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    sVar1 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (((int)*(short *)(_DAT_800ac784 + 0x9c) % 0xc) * 0x10000 >> 0x10 == 0xb) {
    func_0x800453d0(6);
  }
  return;
}



/* ============ FUN_80112024 @ 0x80112024 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112024(void)

{
  int iVar1;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(8000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  else if ((((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
           ((*(byte *)(_DAT_800ac784 + 9) & 0x19) == 0)) &&
          ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)
            ) + 0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80112124 @ 0x80112124 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112124(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0xc;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 8;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((uint)*(byte *)(_DAT_800ac784 + 0x95) % 0xc == 0xb) {
    func_0x800453d0(5);
  }
  func_0x800245d8(0);
  FUN_80115bec(0,0x11 < *(byte *)(_DAT_800ac784 + 0x95));
  return;
}



/* ============ FUN_80112294 @ 0x80112294 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112294(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
    func_0x800453d0(1);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80115bec(0,0);
  return;
}



/* ============ FUN_801123e8 @ 0x801123e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123e8(void)

{
  undefined1 uVar1;
  int iVar2;
  
  if ((((DAT_800acae7 == '\0') && (iVar2 = func_0x8001a804(3000,0x100,&DAT_800aca88), iVar2 < 0)) &&
      (*(short *)(_DAT_800ac784 + 0x1de) == 0)) &&
     ((0x50 < _DAT_800acaee && (0x9c4 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
    uVar1 = 7;
LAB_801124c0:
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  else {
    if (DAT_800acae7 == '\0') {
      iVar2 = func_0x8001a804(0x708,0x180,&DAT_800aca88);
      if ((iVar2 < 0) && (uVar1 = 4, *(short *)(_DAT_800ac784 + 0x1de) == 0)) goto LAB_801124c0;
      if ((DAT_800acae7 == '\0') &&
         (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0 &&
          (7000 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 1;
        return;
      }
    }
    if ((((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
        ((*(byte *)(_DAT_800ac784 + 9) & 0x19) == 0)) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
         0x200 & 0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  return;
}



/* ============ FUN_801125dc @ 0x801125dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801125dc(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x82;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar3 + (uVar3 / 7 + (uVar3 - uVar3 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(5);
  }
  FUN_80115bec(0,6 < *(byte *)(_DAT_800ac784 + 0x95) - 2);
  return;
}



/* ============ FUN_8011275c @ 0x8011275c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011275c(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(3);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_80115bec(0,0);
    if (((9 < *(byte *)(_DAT_800ac784 + 0x95)) && (DAT_800acae7 == 0)) &&
       (iVar2 = func_0x8001a804(0x898,0x180,&DAT_800aca88), iVar2 < 0)) {
      _DAT_800acaee = _DAT_800acaee + -10;
      if (_DAT_800acaee < 0x32) {
        cVar1 = func_0x8001a780(&DAT_800aca54);
        cVar1 = cVar1 * '\x04' + '\x05';
LAB_80112a34:
        *(char *)(_DAT_800ac784 + 5) = cVar1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      else {
        func_0x800453d0(9);
        *(undefined1 *)(_DAT_800ac784 + 6) = 2;
        DAT_800aca58 = 2;
        cVar1 = func_0x8001a780(&DAT_800aca54);
        DAT_800aca59 = cVar1 + 2;
        DAT_800aca5a = 0;
        DAT_800acae7 = DAT_800acae7 | 1;
        uVar3 = func_0x8001af20();
        func_0x80037edc(*(undefined1 *)
                         ((int)&PTR_FUN_8011a150 + (uVar3 & 1) + (uint)DAT_800aca59 * 2),0x32);
      }
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 10;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_80115bec(0,0);
    break;
  case 4:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    cVar1 = '\x03';
    goto LAB_80112a34;
  default:
    break;
  }
  return;
}



/* ============ FUN_80112a64 @ 0x80112a64 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a64(void)

{
  char cVar1;
  short sVar2;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 0;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    break;
  case 1:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 != '\0') && (-1 < _DAT_800acaee)) {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
      if (-1 < (short)_DAT_800ac784[0x27]) {
        return;
      }
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      DAT_800aca5a = 4;
      return;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 8;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x10;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  case 5:
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x18') {
      func_0x800453d0(0);
    }
    break;
  case 6:
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  default:
    goto switchD_80112aa0_default;
  }
  func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
switchD_80112aa0_default:
  return;
}



/* ============ FUN_80112e20 @ 0x80112e20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e20(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x11;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    DAT_800aca58 = 6;
    DAT_800aca59 = 0;
    DAT_800aca5a = 0;
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
    func_0x800453d0(8);
  }
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80112f58 @ 0x80112f58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f58(void)

{
  char cVar1;
  short sVar2;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x12;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 1;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    break;
  case 1:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      DAT_800aca5a = 4;
    }
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 10;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x14;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  case 5:
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x18') {
      func_0x800453d0(0);
    }
    break;
  case 6:
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  default:
    goto switchD_80112f98_default;
  }
  func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
switchD_80112f98_default:
  return;
}



/* ============ FUN_80113300 @ 0x80113300 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113300(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    DAT_800aca58 = 6;
    DAT_800aca59 = 1;
    DAT_800aca5a = 0;
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
    func_0x800453d0(8);
  }
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_80113438 @ 0x80113438 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113438(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  undefined1 uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) == 0) {
      iVar5 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4;
    }
    else {
      iVar5 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) * 0x10 + 0x800;
    }
    sVar2 = func_0x8001aa68(iVar5,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9);
    if (bVar1 == 8) {
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x2f;
    }
    else if (bVar1 < 9) {
      if (bVar1 == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x2d;
        uVar6 = 0;
LAB_80113674:
        func_0x800245d8(uVar6);
      }
    }
    else if (bVar1 == 9) {
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x2f;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x640;
    }
    else if (bVar1 == 0x10) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x2f;
      uVar6 = 0x800;
      goto LAB_80113674;
    }
    cVar3 = *(char *)(_DAT_800ac784 + 0x95);
    if (cVar3 == '\n') {
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0b0;
      cVar3 = *(char *)(_DAT_800ac784 + 0x95);
    }
    if (cVar3 == '#') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x12) {
      FUN_80115bec(0,0);
    }
    else {
      FUN_80115bec(0,1);
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9);
    if (bVar1 == 8) {
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0ac;
      *(undefined1 *)(_DAT_800ac784 + 9) = 0x10;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xffffeae8;
      uVar7 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar7 != 1) {
        if (uVar7 < 2) {
          if (uVar7 != 0) {
            return;
          }
LAB_80113968:
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x671;
          return;
        }
        if (uVar7 == 2) goto LAB_80113940;
        if (uVar7 != 3) {
          return;
        }
        goto LAB_80113954;
      }
    }
    else {
      if (bVar1 < 9) {
        if (bVar1 != 0) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 9) = 8;
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x7de;
        return;
      }
      if (bVar1 != 9) {
        if (bVar1 != 0x10) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 9) = 9;
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x671;
        return;
      }
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0ac;
      *(undefined1 *)(_DAT_800ac784 + 9) = 1;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      uVar7 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar7 == 1) {
LAB_80113954:
        *(int *)(_DAT_800ac784 + 0x3c) = *(int *)(_DAT_800ac784 + 0x3c) + -0x671;
        return;
      }
      if (uVar7 < 2) {
        if (uVar7 != 0) {
          return;
        }
LAB_80113940:
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x671;
        return;
      }
      if (uVar7 == 2) goto LAB_80113968;
      if (uVar7 != 3) {
        return;
      }
    }
    *(int *)(_DAT_800ac784 + 0x3c) = *(int *)(_DAT_800ac784 + 0x3c) + 0x671;
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      uVar4 = 1;
      break;
    default:
      goto switchD_80113470_default;
    case 8:
    case 9:
      uVar4 = 0xc;
      break;
    case 0x10:
      uVar4 = 0xd;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar4;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
switchD_80113470_default:
  return;
}



/* ============ FUN_80113a78 @ 0x80113a78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113a78(void)

{
  uint uVar1;
  
  if (*(char *)(_DAT_800ac784 + 9) == '\b') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = (*(int *)(_DAT_800ac784 + 0x38) + -0x3c) - (uVar1 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) < -0xe10) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  if (*(char *)(_DAT_800ac784 + 9) == '\t') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3c + (uVar1 & 0xf);
    if (-0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_80113b9c @ 0x80113b9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113b9c(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((uint)*(byte *)(_DAT_800ac784 + 0x95) % 0xc == 0xb) {
    func_0x800453d0(5);
  }
  return;
}



/* ============ FUN_80113d3c @ 0x80113d3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113d3c(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0xc;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 8;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((uint)*(byte *)(_DAT_800ac784 + 0x95) % 0xc == 0xb) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_80113e48 @ 0x80113e48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e48(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(3);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    local_1c = _DAT_80072d64;
    local_18 = _DAT_80072d68;
    local_14 = _DAT_80072d6c;
    local_20 = 100;
    uVar3 = func_0x8001bff8(iVar4 + 0x244,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar3;
    uVar3 = func_0x8001bff8(iVar4 + 0x9a8,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
    if (((10 < *(byte *)(_DAT_800ac784 + 0x95)) && (DAT_800acae7 == '\0')) &&
       ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
      _DAT_800acaee = _DAT_800acaee + -10;
      uVar2 = 0xf;
LAB_801140e4:
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 10;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),1,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 4:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x1e;
    uVar2 = 0xd;
    goto LAB_801140e4;
  default:
    break;
  }
  return;
}



/* ============ FUN_8011411c @ 0x8011411c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011411c(void)

{
  char cVar1;
  short sVar2;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x18;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    *(short *)(_DAT_800ac784 + 0x28) = (short)_DAT_800ac784[0xd];
    *(short *)((int)_DAT_800ac784 + 0xa2) = (short)_DAT_800ac784[0xf];
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x66c;
    func_0x800245d8(0x800);
    _DAT_800aca88 = _DAT_800ac784[0xd];
    _DAT_800aca90 = _DAT_800ac784[0xf];
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x28];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0xa2);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    cVar1 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    break;
  case 1:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x19;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
  case 3:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 0;
    }
    else {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -8;
      if (-1 < (short)_DAT_800ac784[0x27]) {
        return;
      }
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
    DAT_800aca5a = 4;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    break;
  case 6:
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x1e;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 0xd;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  default:
    goto switchD_80114158_default;
  }
  cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
switchD_80114158_default:
  return;
}



/* ============ FUN_80114494 @ 0x80114494 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114494(void)

{
  short sVar1;
  ushort uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar2 & 0xf) + 0x78;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = *(undefined2 *)(_DAT_800ac784 + 0x9a);
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if ((sVar1 == 0) || (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x400), iVar3 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_801145c8 @ 0x801145c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801145c8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    if ((*(short *)(_DAT_800ac784 + 0x1dc) == 100) &&
       (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x400), iVar3 == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\x05') {
      func_0x80019700(0xd002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011a158);
      *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80115bec(0,1);
  return;
}



/* ============ FUN_80114770 @ 0x80114770 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114770(void)

{
  if ((((*(short *)(_DAT_800ac784 + 0x1dc) == 0) ||
       ((&DAT_8011a1a8)[*(byte *)(_DAT_800ac784 + 5)] == '\0')) ||
      (*(char *)(_DAT_800ac784 + 6) != '\x01')) || ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0))
  {
    (*(code *)(&PTR_FUN_8011a1c0)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  else {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
    func_0x800453d0(0xb);
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = *(undefined2 *)(_DAT_800ac784 + 0x1dc);
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}



/* ============ FUN_80114850 @ 0x80114850 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114850(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
      if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011a168 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011a16c + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011a170 + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011a174 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80115bec(0,1);
  return;
}



/* ============ FUN_80114a40 @ 0x80114a40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114a40(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011a168 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011a16c + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011a170 + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011a174 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (((*(byte *)(_DAT_800ac784 + 0x95) < 0xf) || (*(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
     ((*(byte *)(_DAT_800ac784 + 0x95) < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
    FUN_80115bec(0,1);
  }
  else {
    FUN_80115bec(0,0);
  }
  return;
}



/* ============ FUN_80114c68 @ 0x80114c68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114c68(void)

{
  (*(code *)(&PTR_FUN_8011a218)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80114cb0 @ 0x80114cb0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114cb0(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    iVar2 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011a168 + iVar2);
    local_1c = *(undefined4 *)(&DAT_8011a16c + iVar2);
    local_18 = *(undefined4 *)(&DAT_8011a170 + iVar2);
    local_14 = *(undefined4 *)(&DAT_8011a174 + iVar2);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    if (((*(byte *)(_DAT_800ac784 + 0x95) < 0xf) || (*(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
       ((*(byte *)(_DAT_800ac784 + 0x95) < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
      uVar3 = 1;
    }
    else {
      uVar3 = 0;
    }
    FUN_80115bec(0,uVar3);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x18') {
      func_0x800453d0(7);
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    break;
  case 4:
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011a188);
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  }
  return;
}



/* ============ FUN_80114fe4 @ 0x80114fe4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114fe4(void)

{
  (*(code *)(&PTR_FUN_8011a270)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011502c @ 0x8011502c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011502c(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  byte bVar4;
  
  bVar4 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        return;
      }
      goto LAB_801151cc;
    }
    if (bVar4 != 0) {
      return;
    }
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 0;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 & 0xfffffffe;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 9) != 'B') {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_801151cc:
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 | 1;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '2') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar2 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}



/* ============ FUN_801152ec @ 0x801152ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801152ec(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  byte bVar4;
  
  bVar4 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        return;
      }
      goto LAB_8011548c;
    }
    if (bVar4 != 0) {
      return;
    }
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 0;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 & 0xfffffffe;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 9) != 'B') {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_8011548c:
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 | 1;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '!') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar2 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}



/* ============ FUN_801155ac @ 0x801155ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801155ac(void)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_40 = DAT_80100358;
  local_3c = DAT_8010035c;
  local_38 = DAT_80100360;
  local_34 = DAT_80100364;
  local_30 = DAT_80100368;
  local_2c = DAT_8010036c;
  local_28 = DAT_80100370;
  local_24 = DAT_80100374;
  local_20 = DAT_80100014;
  local_1c = DAT_80100018;
  local_18 = DAT_8010001c;
  local_14 = DAT_80100020;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  uVar1 = func_0x8001af20();
  iVar3 = (uVar1 & 1) * 8;
  func_0x80019700(0x8031800,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a288)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a289)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_30);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28a)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28b)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28c)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28d)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28e)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011a28f)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  puVar2 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar2[0x25] = 0;
  puVar2[0x26] = 0;
  *puVar2 = *puVar2 | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x3f0) = 0;
  *(undefined4 *)(iVar3 + 0x3f4) = 0;
  *(uint *)(iVar3 + 0x35c) = *(uint *)(iVar3 + 0x35c) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xd58) = 0;
  *(undefined4 *)(iVar3 + 0xd5c) = 0;
  *(uint *)(iVar3 + 0xcc4) = *(uint *)(iVar3 + 0xcc4) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xc00) = 0;
  *(undefined4 *)(iVar3 + 0xc04) = 0;
  *(uint *)(iVar3 + 0xb6c) = *(uint *)(iVar3 + 0xb6c) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x8a4) = 0;
  *(undefined4 *)(iVar3 + 0x8a8) = 0;
  *(uint *)(iVar3 + 0x810) = *(uint *)(iVar3 + 0x810) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar3 + 0x60c) = *(uint *)(iVar3 + 0x60c) | 0x80;
  *(undefined4 *)(iVar3 + 0x6a0) = 0;
  *(undefined4 *)(iVar3 + 0x6a4) = 0;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xf5c) = 0;
  *(undefined4 *)(iVar3 + 0xf60) = 0;
  *(uint *)(iVar3 + 0xec8) = *(uint *)(iVar3 + 0xec8) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x1160) = 0;
  *(undefined4 *)(iVar3 + 0x1164) = 0;
  *(uint *)(iVar3 + 0x10cc) = *(uint *)(iVar3 + 0x10cc) | 0x80;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),1,
                  0x200);
  *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  return;
}



/* ============ FUN_80115bec @ 0x80115bec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115bec(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0xe1c;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ============ FUN_801168c4 @ 0x801168c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801168c4(void)

{
  if ((_DAT_800aca40 & 0x20000000) == 0) {
    (*(code *)(&PTR_FUN_8011a2c0)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}



/* ============ FUN_80116920 @ 0x80116920 ============ */

void FUN_80116920(uint *param_1)

{
  uint *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  DAT_800acc0c = 1;
  *(undefined1 *)((int)param_1 + 0x93) = 3;
  *(undefined1 *)(param_1 + 0x6e) = 0;
  *(undefined1 *)((int)param_1 + 0x1b9) = 0;
  *(undefined2 *)((int)param_1 + 0x9a) = 100;
  puVar1 = param_1;
  do {
    puVar1[0x74] = 0;
    uVar2 = uVar2 + 1;
    puVar1 = puVar1 + 1;
  } while (uVar2 < 8);
  param_1[1] = 1;
  *(undefined1 *)(param_1 + 0x25) = 0;
  *(undefined1 *)((int)param_1 + 0x95) = 0;
  *(undefined1 *)((int)param_1 + 0x8f) = 0;
  *param_1 = *param_1 & 0x1fffffff;
  func_0x8001f314(param_1[0x21],param_1[0x5b],0,0x200);
  return;
}



/* ============ FUN_801169b8 @ 0x801169b8 ============ */

void FUN_801169b8(int param_1)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(param_1 + 5);
  if (bVar1 == 1) {
    *(undefined1 *)(param_1 + 0x94) = 0;
    *(undefined1 *)(param_1 + 0x95) = 0;
    *(undefined1 *)(param_1 + 0x8f) = 0;
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        return;
      }
      iVar2 = func_0x8004efe4(0x800b1028,0x1f);
      if (iVar2 == 0) {
        return;
      }
      *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
      func_0x8004efb8(0x800b1028,0x1f);
      return;
    }
    if (bVar1 != 2) {
      return;
    }
  }
  iVar2 = *(int *)(param_1 + 0x188);
  if (*(byte *)(param_1 + 0x95) < 10) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xb;
  }
  if (*(byte *)(param_1 + 0x95) - 10 < 0xd) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x17 < 7) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xb;
  }
  if (*(char *)(param_1 + 0x95) == '(') {
    *(int *)(iVar2 + 0x188) = *(int *)(iVar2 + 0x188) + -0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x29 < 7) {
    *(int *)(iVar2 + 0x188) = *(int *)(iVar2 + 0x188) + -0xd;
  }
  if (*(byte *)(param_1 + 0x95) - 0x2f < 7) {
    *(int *)(iVar2 + 0x2e0) = *(int *)(iVar2 + 0x2e0) + 0xc;
    *(int *)(iVar2 + 0x438) = *(int *)(iVar2 + 0x438) + 0xc;
    *(int *)(iVar2 + 0x590) = *(int *)(iVar2 + 0x590) + 0xc;
    *(int *)(iVar2 + 0x6e8) = *(int *)(iVar2 + 0x6e8) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x36 < 6) {
    *(int *)(iVar2 + 0x2e0) = *(int *)(iVar2 + 0x2e0) + 0xb;
    *(int *)(iVar2 + 0x438) = *(int *)(iVar2 + 0x438) + 0xb;
    *(int *)(iVar2 + 0x590) = *(int *)(iVar2 + 0x590) + 0xb;
    *(int *)(iVar2 + 0x6e8) = *(int *)(iVar2 + 0x6e8) + 0xb;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  return;
}



/* ============ FUN_80116be4 @ 0x80116be4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116be4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011a2d8)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_80116d20 @ 0x80116d20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d20(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011a2d4;
  _DAT_800ac784[0x6a] = (uint)&DAT_800aca54;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 8;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(uVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(uVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(uVar1 + 0x5f4) = 0;
  *(undefined2 *)(uVar1 + 0x5f6) = 0;
  *(undefined2 *)(uVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 2;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 1) = 4;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ============ FUN_80116ec8 @ 0x80116ec8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116ec8(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x5dc,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  (*(code *)(&PTR_FUN_8011a2ec)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80116f94 @ 0x80116f94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f94(void)

{
  (**(code **)(&DAT_8011a2f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a310 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80117010 @ 0x80117010 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117010(void)

{
  (**(code **)(&DAT_8011a330 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a350 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011708c @ 0x8011708c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011708c(void)

{
  (**(code **)(&DAT_8011a370 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801170d4 @ 0x801170d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801170d4(void)

{
  (**(code **)(&DAT_8011a370 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011711c @ 0x8011711c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011711c(void)

{
  (**(code **)(&DAT_8011a370 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_801171b4 @ 0x801171b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801171b4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011a3a8)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_801172f0 @ 0x801172f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801172f0(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011a3a4;
  _DAT_800ac784[0x6a] = (uint)&DAT_800aca54;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 8;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(uVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(uVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(uVar1 + 0x5f4) = 0;
  *(undefined2 *)(uVar1 + 0x5f6) = 0;
  *(undefined2 *)(uVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 2;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 1) = 4;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ============ FUN_80117498 @ 0x80117498 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117498(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x800,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  (*(code *)(&PTR_FUN_8011a3bc)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80117564 @ 0x80117564 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117564(void)

{
  (**(code **)(&DAT_8011a3c0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a3e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801175e0 @ 0x801175e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801175e0(void)

{
  (**(code **)(&DAT_8011a400 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a420 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011765c @ 0x8011765c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011765c(void)

{
  (**(code **)(&DAT_8011a440 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801176a4 @ 0x801176a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801176a4(void)

{
  (**(code **)(&DAT_8011a440 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801176ec @ 0x801176ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801176ec(void)

{
  (**(code **)(&DAT_8011a440 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80117784 @ 0x80117784 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117784(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011a478)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_801178b4 @ 0x801178b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801178b4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011a474;
  _DAT_800ac784[0x6a] = (uint)&DAT_800aca54;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 8;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(uVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(uVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(uVar1 + 0x5f4) = 0;
  *(undefined2 *)(uVar1 + 0x5f6) = 0;
  *(undefined2 *)(uVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 2;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 1) = 4;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ============ FUN_80117a5c @ 0x80117a5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117a5c(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x800,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  (*(code *)(&PTR_FUN_8011a48c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80117b28 @ 0x80117b28 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117b28(void)

{
  (**(code **)(&DAT_8011a490 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a4b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80117ba4 @ 0x80117ba4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117ba4(void)

{
  (**(code **)(&DAT_8011a4d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a4f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80117c20 @ 0x80117c20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117c20(void)

{
  (**(code **)(&DAT_8011a510 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80117c68 @ 0x80117c68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117c68(void)

{
  (**(code **)(&DAT_8011a510 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80117cb0 @ 0x80117cb0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117cb0(void)

{
  (**(code **)(&DAT_8011a510 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80117d48 @ 0x80117d48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d48(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_8011a548)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  *(undefined2 *)(_DAT_800ac784 + 0x1c8) = 0;
  return;
}



/* ============ FUN_80117e8c @ 0x80117e8c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117e8c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011a544;
  _DAT_800ac784[0x6a] = (uint)&DAT_800aca54;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 8;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(uVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(uVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(uVar1 + 0x5f4) = 0;
  *(undefined2 *)(uVar1 + 0x5f6) = 0;
  *(undefined2 *)(uVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 2;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 1) = 4;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ============ FUN_80118034 @ 0x80118034 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118034(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(700,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x578,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  if (((*(byte *)(_DAT_800ac784 + 0x1c2) & 1) == 0) || (_DAT_800acae0 < 0x65)) {
    (*(code *)(&PTR_FUN_8011a55c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80118160 @ 0x80118160 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118160(void)

{
  (**(code **)(&DAT_8011a564 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a584 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801181dc @ 0x801181dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801181dc(void)

{
  (**(code **)(&DAT_8011a5a4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a5c4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80118258 @ 0x80118258 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118258(void)

{
  (**(code **)(&DAT_8011a5e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801182a0 @ 0x801182a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801182a0(void)

{
  (**(code **)(&DAT_8011a5f8 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801182e8 @ 0x801182e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801182e8(void)

{
  (**(code **)(&DAT_8011a5f8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80118380 @ 0x80118380 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118380(void)

{
  _DAT_80072bec = &LAB_801003d8;
  _DAT_80072bf0 = &LAB_801003d8;
  _DAT_80072bf4 = &LAB_801003d8;
  _DAT_80072c04 = &LAB_801003d8;
  _DAT_80072c0c = &LAB_801003d8;
  _DAT_80072c1c = &LAB_801003d8;
  _DAT_80072c20 = &LAB_801003d8;
  _DAT_80072c24 = &LAB_801003d8;
  _DAT_80072c28 = &LAB_801003d8;
  _DAT_80072c50 = FUN_8010c1a0;
  _DAT_80072c58 = FUN_801118d0;
  _DAT_80072c60 = FUN_801168c4;
  _DAT_80072cac = FUN_80116be4;
  _DAT_80072cb4 = FUN_801171b4;
  _DAT_80072cd0 = FUN_80117784;
  _DAT_80072cd8 = FUN_80117d48;
  _DAT_800b52c8 = &DAT_80118590;
  _DAT_800ac998 = &DAT_801186f0;
  _DAT_80072bf8 = FUN_8010a87c;
  _DAT_800ac798 = &LAB_8010a240;
  _DAT_800ac79c = &LAB_8010a240;
  _DAT_800ac7a0 = &LAB_8010a240;
  _DAT_800ac7b0 = &LAB_8010a240;
  _DAT_800ac7b8 = &LAB_8010a240;
  _DAT_800ac7c8 = &LAB_8010a240;
  _DAT_800ac7cc = &LAB_8010a240;
  _DAT_800ac7d0 = &LAB_8010a240;
  _DAT_800ac7d4 = &LAB_8010a240;
  _DAT_800ac7fc = &LAB_801113d0;
  _DAT_800ac804 = &LAB_80115cc0;
  _DAT_800ac7a4 = &LAB_8010c120;
  _DAT_800ac898 = &LAB_8010a66c;
  _DAT_800ac89c = &LAB_8010a66c;
  _DAT_800ac8a0 = &LAB_8010a66c;
  _DAT_800ac8b0 = &LAB_8010a66c;
  _DAT_800ac8b8 = &LAB_8010a66c;
  _DAT_800ac8c8 = &LAB_8010a66c;
  _DAT_800ac8cc = &LAB_8010a66c;
  _DAT_800ac8d0 = &LAB_8010a66c;
  _DAT_800ac8d4 = &LAB_8010a66c;
  _DAT_800ac8fc = &LAB_80111704;
  _DAT_800ac8a4 = &LAB_8010c160;
  _DAT_800ac904 = &LAB_801161dc;
  func_0x80029afc();
  return;
}



