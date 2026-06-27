/* ============ FUN_8010051c @ 0x8010051c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010051c(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80117894;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_801178e8)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011717c + (uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&DAT_80117888)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (*(code *)(&PTR_FUN_801178f0)[uVar3 & 7])();
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
                    (uint)(byte)(&DAT_801178a8)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178a9)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178aa)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ab)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ac)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ad)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ae)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178af)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
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



/* ============ FUN_80100e9c @ 0x80100e9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e9c(void)

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



/* ============ FUN_80100eec @ 0x80100eec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100eec(void)

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



/* ============ FUN_80100fd0 @ 0x80100fd0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fd0(void)

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



/* ============ FUN_801010b8 @ 0x801010b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010b8(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178aa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ab)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ac)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ad)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ae)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178af)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_80117910)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80101520 @ 0x80101520 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101520(void)

{
  (*(code *)(&PTR_FUN_80117944)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117994)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010159c @ 0x8010159c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010159c(void)

{
  (*(code *)(&PTR_FUN_801179e4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117a24)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101618 @ 0x80101618 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101618(void)

{
  (*(code *)(&PTR_FUN_80117a64)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117994)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101694 @ 0x80101694 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101694(void)

{
  (*(code *)(&PTR_FUN_80117ab4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117a24)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101710 @ 0x80101710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101710(void)

{
  (*(code *)(&PTR_FUN_80117acc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117ad0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010178c @ 0x8010178c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010178c(void)

{
  (*(code *)(&PTR_FUN_80117ad4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117ad8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101808 @ 0x80101808 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101808(void)

{
  (*(code *)(&PTR_LAB_80117adc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117ad8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101884 @ 0x80101884 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101884(void)

{
  (*(code *)(&PTR_FUN_80117ae0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117ad8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101900 @ 0x80101900 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101900(void)

{
  (*(code *)(&PTR_LAB_80117ae4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117ae8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010197c @ 0x8010197c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010197c(void)

{
  (*(code *)(&PTR_LAB_80117aec)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80117af0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801019f8 @ 0x801019f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019f8(void)

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



/* ============ FUN_80101b10 @ 0x80101b10 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b10(void)

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



/* ============ FUN_80101b9c @ 0x80101b9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b9c(void)

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
    FUN_80109230(0,1);
  }
  return;
}



/* ============ FUN_80101c78 @ 0x80101c78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c78(void)

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



/* ============ FUN_80101d84 @ 0x80101d84 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101d84(void)

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
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80101eec @ 0x80101eec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101eec(void)

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



/* ============ FUN_8010208c @ 0x8010208c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010208c(void)

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
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80117c04)[uVar5 & 0x1f];
    }
    else {
      uVar5 = func_0x8001af20();
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80117bf4)[uVar5 & 0x1f];
    }
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_80117af4 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_80117af6 +
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
          (&DAT_80117af4 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801023dc @ 0x801023dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801023dc(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_801178d8);
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



/* ============ FUN_80102a6c @ 0x80102a6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102a6c(void)

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



/* ============ FUN_80102bb4 @ 0x80102bb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102bb4(void)

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



/* ============ FUN_80102c5c @ 0x80102c5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102c5c(void)

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



/* ============ FUN_80102db0 @ 0x80102db0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102db0(void)

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



/* ============ FUN_80102ea8 @ 0x80102ea8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102ea8(void)

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
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80103078 @ 0x80103078 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103078(void)

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
    goto switchD_801030b0_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_801030b0_default:
  return;
}



/* ============ FUN_8010325c @ 0x8010325c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010325c(void)

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



/* ============ FUN_8010348c @ 0x8010348c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010348c(void)

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



/* ============ FUN_80103570 @ 0x80103570 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103570(void)

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
    FUN_80109304(0,uVar1);
  }
  return;
}



/* ============ FUN_801036f0 @ 0x801036f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801036f0(void)

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



/* ============ FUN_80103814 @ 0x80103814 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103814(void)

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



/* ============ FUN_80103890 @ 0x80103890 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103890(void)

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



/* ============ FUN_801038ec @ 0x801038ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801038ec(void)

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



/* ============ FUN_80103a28 @ 0x80103a28 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a28(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_801178d8);
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
        puVar5 = (uint *)((uint)(byte)(&DAT_80117888)[(byte)_DAT_800ac784[2]] * 0xac +
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



/* ============ FUN_8010400c @ 0x8010400c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010400c(void)

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
    puVar3 = (uint *)((uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ============ FUN_801043dc @ 0x801043dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801043dc(void)

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



/* ============ FUN_80104500 @ 0x80104500 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104500(void)

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



/* ============ FUN_801045f4 @ 0x801045f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801045f4(void)

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



/* ============ FUN_8010469c @ 0x8010469c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010469c(void)

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



/* ============ FUN_8010477c @ 0x8010477c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010477c(void)

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



/* ============ FUN_801048e4 @ 0x801048e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801048e4(void)

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



/* ============ FUN_801049d4 @ 0x801049d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801049d4(void)

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
    goto LAB_80104d70;
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
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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
LAB_80104d70:
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



/* ============ FUN_80104e14 @ 0x80104e14 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104e14(void)

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



/* ============ FUN_80104fc0 @ 0x80104fc0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104fc0(void)

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
    FUN_80109230(0,0);
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    uVar4 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) =
         (short)*(undefined4 *)(&DAT_80117c14 + (uVar4 & 0xf) * 4) * 0x1e;
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



/* ============ FUN_80105388 @ 0x80105388 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105388(void)

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



/* ============ FUN_801054b0 @ 0x801054b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801054b0(void)

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



/* ============ FUN_80105650 @ 0x80105650 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105650(void)

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
         *(undefined2 *)(&DAT_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_80117c56 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&DAT_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80105920 @ 0x80105920 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105920(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee836c))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  else if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801068cc();
  }
  else {
    FUN_80106734();
  }
  return;
}



/* ============ FUN_80105a10 @ 0x80105a10 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a10(void)

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
      else if (bVar2 != 3) goto LAB_80105ec0;
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
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80117c04)[uVar4 & 0x1f];
        }
        else {
          uVar4 = func_0x8001af20();
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_80117bf4)[uVar4 & 0x1f];
        }
        if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) =
             *(undefined2 *)
              (&DAT_80117af4 +
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
      goto LAB_80105ec0;
    }
    if (bVar2 != 0) goto LAB_80105ec0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
              *(int *)(_DAT_800ac784 + 0x188);
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0x40,&LAB_80117f88);
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    (*(code *)(&PTR_LAB_80117f34)[*(byte *)(_DAT_800ac784 + 5)])();
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
LAB_80105ec0:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80105edc @ 0x80105edc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105edc(void)

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
      goto LAB_80106100;
    }
    if (bVar1 != 0) goto LAB_80106100;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar3 = func_0x8001af20();
    *(undefined *)(_DAT_800ac784 + 0x94) = (&DAT_80117f98)[uVar3 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_80117fa0);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
    (*(code *)(&PTR_LAB_80117f34)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_80106100:
  FUN_80109230(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_80106124 @ 0x80106124 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106124(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_801062a0:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_8010649c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_8010649c;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (*(code *)(&PTR_LAB_80117f34)[*(byte *)(_DAT_800ac784 + 5)])();
      goto LAB_801062a0;
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
      goto LAB_8010649c;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_8010649c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  if (cVar2 == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
  }
LAB_8010649c:
  FUN_80109230(0,0);
  return;
}



/* ============ FUN_801064b8 @ 0x801064b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801064b8(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
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



/* ============ FUN_80106734 @ 0x80106734 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106734(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_80117fa0);
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



/* ============ FUN_801068cc @ 0x801068cc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801068cc(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80117fa0);
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



/* ============ FUN_80106a38 @ 0x80106a38 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106a38(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee8050))();
  }
  else {
    FUN_80107b44();
  }
  return;
}



/* ============ FUN_80106aac @ 0x80106aac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106aac(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80106d34;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80118270);
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
LAB_80106d34:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109230(0,0);
  }
  return;
}



/* ============ FUN_80106d70 @ 0x80106d70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106d70(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80107094;
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
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    puVar7 = (uint *)((uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
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
LAB_80107094:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109230(0,0);
  }
  return;
}



/* ============ FUN_801070d8 @ 0x801070d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801070d8(void)

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
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    puVar7 = (uint *)((uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_80118280);
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
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801074c8 @ 0x801074c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801074c8(void)

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
switchD_80107504_caseD_3:
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
                      &DAT_80118280);
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
      FUN_80109230(0,0);
    }
    else {
      FUN_80109230(0,1);
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
    FUN_80109230(0,1);
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
    goto switchD_80107504_caseD_3;
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



/* ============ FUN_80107b44 @ 0x80107b44 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80107b44(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
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
                    (uint)(byte)(&DAT_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)uVar3;
  return uVar3;
}



/* ============ FUN_80107d74 @ 0x80107d74 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107d74(void)

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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar9 + 0x10,&LAB_80118270);
    func_0x800453d0(9);
    uVar10 = func_0x8001af20();
    if ((uVar10 & 1) != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      FUN_80108b98((uint)(byte)(&DAT_80117888)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62])
      ;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108b98(uVar6 + 0x560);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x60c) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x5e8) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x5ea) = 0xc18;
        *(undefined2 *)(uVar6 + 0x5ec) = 0x5f4;
        *(uint *)(uVar6 + 0x560) = *(uint *)(uVar6 + 0x560) | 0x400;
        FUN_80108b98(_DAT_800ac784[0x62] + 0x60c);
      }
      *(undefined4 *)(_DAT_800ac784[0x62] + 0x6b8) = 0;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108b98(uVar6 + 0x764);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x810) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x7ec) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x7ee) = 0xc18;
        *(undefined2 *)(uVar6 + 0x7f0) = 0x5f4;
        *(uint *)(uVar6 + 0x764) = *(uint *)(uVar6 + 0x764) | 0x400;
        FUN_80108b98(_DAT_800ac784[0x62] + 0x810);
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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_80118270);
    func_0x80019700(0x5002800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,
                    &LAB_80118270);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80117888)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62]
                    + 0x40,&LAB_80118270);
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
                        &LAB_80118270);
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
        goto joined_r0x801085e4;
      }
      *(uint *)(_DAT_800ac784[0x62] + 0x560) = *(uint *)(_DAT_800ac784[0x62] + 0x560) | 0x200;
      *(uint *)(_DAT_800ac784[0x62] + 0x764) = *(uint *)(_DAT_800ac784[0x62] + 0x764) | 0x200;
    }
    else {
      uVar1 = *(ushort *)(uVar8 + 0x536);
      *(char *)(uVar8 + 0x529) = cVar3 + -1;
joined_r0x801085e4:
      if ((uVar1 & 1) != 0) {
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar8 + 0x4f4,
                        &LAB_80118270);
      }
    }
    cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_801088dc:
    if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
      FUN_80109230(0,0);
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
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_80118270);
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
    goto LAB_801088dc;
  case 4:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}



/* ============ FUN_80108950 @ 0x80108950 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108950(void)

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
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108b98(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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



/* ============ FUN_80108b98 @ 0x80108b98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b98(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&LAB_80118270);
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



/* ============ FUN_80108c08 @ 0x80108c08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108c08(void)

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
      goto LAB_80108ff8;
    }
    if (bVar1 != 0) goto LAB_80108ff8;
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a8)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a9)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178aa)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ab)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ac)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ad)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ae)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178af)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
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
LAB_80108ff8:
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109230(0,0);
  }
  return;
}



/* ============ FUN_80109030 @ 0x80109030 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109030(void)

{
  (**(code **)(&DAT_80118290 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80109078 @ 0x80109078 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109078(void)

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
  FUN_80109230(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80109188 @ 0x80109188 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109188(void)

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
    FUN_80109230(0,1);
  }
  return;
}



/* ============ FUN_80109230 @ 0x80109230 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109230(undefined4 param_1,int param_2)

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



/* ============ FUN_80109304 @ 0x80109304 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109304(undefined4 param_1,int param_2)

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



/* ============ FUN_801093e8 @ 0x801093e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801093e8(void)

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
    goto LAB_80109c08;
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
    goto LAB_80109c08;
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
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178a8)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178a9)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178aa)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178ab)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178ac)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178ad)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178ae)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_801178af)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
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
LAB_80109c08:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar5;
  }
  return;
}



/* ============ FUN_80109c2c @ 0x80109c2c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109c2c(void)

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



/* ============ FUN_80109ce0 @ 0x80109ce0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109ce0(void)

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
      func_0x80019700(0x2800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar6 + 0x10,&DAT_801178d8);
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



/* ============ FUN_8010a060 @ 0x8010a060 ============ */

void FUN_8010a060(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a09c @ 0x8010a09c ============ */

void FUN_8010a09c(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a0d8 @ 0x8010a0d8 ============ */

void FUN_8010a0d8(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ============ FUN_8010a0fc @ 0x8010a0fc ============ */

void FUN_8010a0fc(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ============ FUN_8010a58c @ 0x8010a58c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a58c(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801182cc
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801182cc
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  return;
}



/* ============ FUN_8010a75c @ 0x8010a75c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a75c(void)

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
    (*(code *)(&PTR_FUN_8011830c)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0) {
      *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xed;
    }
    func_0x8001b4e4();
    FUN_8010400c();
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



/* ============ FUN_8010a9c0 @ 0x8010a9c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a9c0(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80117894;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_801178e8)[uVar3 & 7];
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
                    (uint)(byte)(&DAT_801178a8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178a9)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178aa)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ab)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ac)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ad)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ae)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178af)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_801178d8);
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



/* ============ FUN_8010b108 @ 0x8010b108 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b108(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178a9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178aa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ab)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ac)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ad)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178ae)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_801178af)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_80118334)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010b568 @ 0x8010b568 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b568(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_80118368)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801183ac)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b618 @ 0x8010b618 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b618(void)

{
  (*(code *)(&PTR_FUN_801183f0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011840c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b694 @ 0x8010b694 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b694(void)

{
  (*(code *)(&PTR_FUN_80118428)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801183ac)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b710 @ 0x8010b710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b710(void)

{
  (*(code *)(&PTR_FUN_80118460)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011840c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b78c @ 0x8010b78c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b78c(void)

{
  (*(code *)(&PTR_FUN_80118478)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011847c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b808 @ 0x8010b808 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b808(void)

{
  (*(code *)(&PTR_FUN_80118480)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118484)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b884 @ 0x8010b884 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b884(void)

{
  (*(code *)(&PTR_LAB_80118488)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118484)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b900 @ 0x8010b900 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b900(void)

{
  (*(code *)(&PTR_FUN_8011848c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118484)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b97c @ 0x8010b97c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b97c(void)

{
  (*(code *)(&PTR_FUN_80118490)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118494)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b9f8 @ 0x8010b9f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9f8(void)

{
  (*(code *)(&PTR_LAB_80118498)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011849c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ba7c @ 0x8010ba7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba7c(void)

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



/* ============ FUN_8010bce4 @ 0x8010bce4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bce4(void)

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
    FUN_8010bf1c(0,uVar1);
  }
  return;
}



/* ============ FUN_8010be14 @ 0x8010be14 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be14(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee7b60))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_801068cc();
  }
  else {
    FUN_80106734();
  }
  return;
}



/* ============ FUN_8010bea8 @ 0x8010bea8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bea8(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee78c0))();
  }
  else {
    FUN_80107b44();
  }
  return;
}



/* ============ FUN_8010bf1c @ 0x8010bf1c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf1c(undefined4 param_1,int param_2)

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



/* ============ FUN_8010c080 @ 0x8010c080 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c080(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_LAB_80118a20)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}



/* ============ FUN_8010c0fc @ 0x8010c0fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c0fc(void)

{
  (*(code *)(&PTR_LAB_80118a40)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_LAB_80118a80)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c278 @ 0x8010c278 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c278(void)

{
  (*(code *)(&PTR_LAB_80118ac0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c2d8 @ 0x8010c2d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c2d8(void)

{
  (*(code *)(&PTR_LAB_80118b18)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c338 @ 0x8010c338 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c338(void)

{
  (*(code *)(&PTR_LAB_80118b70)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c448 @ 0x8010c448 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c448(void)

{
  undefined2 uVar1;
  
  func_0x800279c8(0x280010,0,s_R0__x_80100194,*(undefined4 *)(_DAT_800ac784 + 4));
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_80118bc8)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_8010c56c @ 0x8010c56c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c56c(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  _DAT_800ac784[1] = 0x601;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 4;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 1;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  _DAT_800ac784[0xe] = 0xfffffb50;
  *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar2 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011717c + (uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80118bb0;
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 10) + 200,_DAT_800ac784 + 0x2c,0x808080);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
    _DAT_800ac784[1] = 0x1000001;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
    _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
    *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
  }
  iVar4 = 0xe1c;
  iVar5 = 0x15;
  do {
    iVar3 = iVar4 + _DAT_800ac784[0x62];
    iVar4 = iVar4 + -0xac;
    *(int *)(iVar3 + 0x2c) = (*(int *)(iVar3 + 0x2c) * 5) / 2;
    *(int *)(iVar3 + 0x30) = (*(int *)(iVar3 + 0x30) * 5) / 2;
    bVar1 = iVar5 != 0;
    *(int *)(iVar3 + 0x34) = (*(int *)(iVar3 + 0x34) * 5) / 2;
    iVar5 = iVar5 + -1;
  } while (bVar1);
  return;
}



/* ============ FUN_8010c860 @ 0x8010c860 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c860(void)

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
  (*(code *)(&PTR_LAB_80118be8)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118c28)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(0x9c4);
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e2) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e2) = *(char *)((int)_DAT_800ac784 + 0x1e2) + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e3) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e3) = *(char *)((int)_DAT_800ac784 + 0x1e3) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  }
  else {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
  }
  return;
}



/* ============ FUN_8010cb30 @ 0x8010cb30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cb30(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  ushort uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
      if (sVar2 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  return;
}



/* ============ FUN_8010cc70 @ 0x8010cc70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc70(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_8010cd70 @ 0x8010cd70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cd70(void)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    goto LAB_8010ce6c;
  }
  if (DAT_800acae7 == '\0') {
LAB_8010ce34:
    iVar3 = func_0x8001a804(6000,0x180,&DAT_800aca88);
    if (-1 < iVar3) {
      return;
    }
    uVar2 = 3;
    if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
      return;
    }
  }
  else if (((4999 < *(ushort *)(_DAT_800ac784 + 0x1d4)) ||
           ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) ||
          (uVar2 = 1, *(char *)(_DAT_800ac784 + 0x1e2) != '\0')) {
    if (DAT_800acae7 != '\0') {
      return;
    }
    goto LAB_8010ce34;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
LAB_8010ce6c:
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_8010ce9c @ 0x8010ce9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ce9c(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      bVar1 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x3c;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010cfbc @ 0x8010cfbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cfbc(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  undefined1 uVar5;
  undefined2 uVar6;
  ushort uVar7;
  int iVar8;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  bVar3 = false;
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        iVar8 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),100,0xfffffff6,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar8 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x21;
          return;
        }
        if (DAT_800acae7 == 0) {
          local_14 = _DAT_80072d64;
          local_10 = _DAT_80072d68;
          local_c = _DAT_80072d6c;
          local_18 = 700;
          uVar6 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0xa54,&local_18,800,
                                  &DAT_800aca88);
          uVar7 = 2;
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
          do {
            if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar7 + 0x80118c68)) {
              bVar3 = true;
            }
            bVar1 = uVar7 != 0;
            uVar7 = uVar7 - 1;
          } while (bVar1);
          if ((bVar3) && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
            *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
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
          if (DAT_800acae7 == 0) {
            return;
          }
        }
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0xa54,&DAT_80118bb8);
        return;
      }
      if (bVar2 != 3) {
        return;
      }
      iVar8 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar8 != 0) {
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
        }
        uVar5 = 4;
        if (*(char *)(_DAT_800ac784 + 0x1e0) == '\0') {
          uVar5 = 2;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = uVar5;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_8010d388;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    uVar6 = func_0x8001a9cc(&DAT_800aca88,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x1da),0);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\f') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar7 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar7 & 0x1f) + 0x78;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  }
LAB_8010d388:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010d3a4 @ 0x8010d3a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d3a4(void)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\x01') {
    if (((DAT_800acae7 == '\0') && (iVar4 = func_0x8001a804(6000,0x180,&DAT_800aca88), iVar4 < 0))
       && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1389) ||
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
LAB_8010d4a0:
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        return;
      }
      bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
      if (((bVar1 & 3) == 0) ||
         (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))
         ) goto LAB_8010d5a4;
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    }
    else {
      uVar5 = func_0x8001af20();
      uVar6 = func_0x8001af20();
      if ((uVar5 & 1) * (uVar6 & 1) == 0) goto LAB_8010d4a0;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  else {
LAB_8010d5a4:
    sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
    *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
    if ((sVar3 == 0) ||
       ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
        (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
      }
    }
    else {
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
    }
  }
  return;
}



/* ============ FUN_8010d684 @ 0x8010d684 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d684(void)

{
  char cVar1;
  byte bVar2;
  ushort uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar3 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 0x3c;
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(char *)(_DAT_800ac784 + 0x1e0) == '\x02') &&
     (cVar1 = *(char *)(_DAT_800ac784 + 0x1e1), *(char *)(_DAT_800ac784 + 0x1e1) = cVar1 + -1,
     cVar1 == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
    if (*(int *)(_DAT_800ac784 + 0x38) < -0x4b0) {
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    }
  }
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010d868 @ 0x8010d868 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d868(void)

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
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 100;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if ((3 < *(byte *)(_DAT_800ac784 + 0x95)) &&
     (iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffba,
                              (int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar3 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}



/* ============ FUN_8010db50 @ 0x8010db50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010db50(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  ushort uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
      if (sVar2 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  return;
}



/* ============ FUN_8010dc90 @ 0x8010dc90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dc90(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_8010dd98 @ 0x8010dd98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dd98(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),1);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  if (iVar1 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x800;
    *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
    if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    }
  }
  return;
}



/* ============ FUN_8010df34 @ 0x8010df34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010df34(void)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),2);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  if (iVar3 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x800;
    if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x3c;
    }
  }
  if (DAT_800acae7 == 0) {
    local_20 = _DAT_80072d60;
    local_1c = _DAT_80072d64;
    local_18 = _DAT_80072d68;
    local_14 = _DAT_80072d6c;
    uVar2 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar2;
    if ((*(byte *)(_DAT_800ac784 + 0x95) - 7 < 9) && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x78;
      DAT_800aca58 = 2;
      cVar1 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar1 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
    }
    if (DAT_800acae7 == 0) {
      return;
    }
  }
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20);
  return;
}



/* ============ FUN_8010e1c0 @ 0x8010e1c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e1c0(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  short sVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        return;
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      sVar4 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
      *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
      if (sVar4 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      return;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        return;
      }
      func_0x8001c0dc(0x898,800,3000);
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0xfb50;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
      return;
    }
  }
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x32;
    if (-0x4b0 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    }
  }
  else {
    cVar3 = func_0x8001c0dc(800,400,0xfffffff6);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
    iVar2 = (*(byte *)(_DAT_800ac784 + 0x82) + 1) * -0x708;
    if (*(int *)(_DAT_800ac784 + 0x38) < iVar2) {
      *(int *)(_DAT_800ac784 + 0x38) = iVar2;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010e570 @ 0x8010e570 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e570(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\0') {
    (*(code *)(&PTR_FUN_80118c6c)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  else {
    FUN_8010e91c();
  }
  return;
}



/* ============ FUN_8010e5d8 @ 0x8010e5d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e5d8(void)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      if (*(char *)(_DAT_800ac784 + 0x1e0) == '\0') {
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        uVar3 = 2;
      }
      else {
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        uVar3 = 4;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,&DAT_80118bb8);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_8010e748 @ 0x8010e748 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e748(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
    *(ushort *)(_DAT_800ac784 + 0x9c) = _DAT_800acabe - *(short *)(_DAT_800ac784 + 0x6a) & 0xfff;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x12) {
    func_0x800245d8((int)*(short *)(_DAT_800ac784 + 0x9c));
  }
  return;
}



/* ============ FUN_8010e91c @ 0x8010e91c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e91c(void)

{
  undefined2 uVar1;
  
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
  *(ushort *)(_DAT_800ac784 + 0x9c) = _DAT_800acabe - *(short *)(_DAT_800ac784 + 0x6a) & 0xfff;
  *(undefined1 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar1;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
  return;
}



/* ============ FUN_8010e9e8 @ 0x8010e9e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e9e8(void)

{
  (*(code *)(&PTR_FUN_80118cc4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ea30 @ 0x8010ea30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea30(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if ((*(char *)(_DAT_800ac784 + 0x1e0) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x14')) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x95)) {
    func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffb0,
                    (int)*(short *)(_DAT_800ac784 + 0x1ba));
  }
  return;
}



/* ============ FUN_8010ec3c @ 0x8010ec3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec3c(void)

{
  (*(code *)(&PTR_LAB_80118d1c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ee40 @ 0x8010ee40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee40(short param_1,uint param_2)

{
  int iVar1;
  char *pcVar2;
  
  iVar1 = (param_2 & 0xff) * 4;
  pcVar2 = &DAT_80118d44 + iVar1;
  if (param_1 == 0) {
    pcVar2 = &DAT_80118d45 + iVar1;
  }
  else if (0 < param_1) {
    pcVar2 = &DAT_80118d46 + iVar1;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar2) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar2;
  }
  return;
}



/* ============ FUN_8010ee9c @ 0x8010ee9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee9c(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    (*(code *)(&PTR_FUN_80118d50)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}



/* ============ FUN_8010ef1c @ 0x8010ef1c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ef1c(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  FUN_8011089c(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    *(undefined4 *)(uVar1 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  DAT_80119318 = *(uint **)(_DAT_800ac784 + 0x188);
  *DAT_80119318 = *DAT_80119318 & 0xfffffffe;
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = *(undefined1 *)(_DAT_800ac784 + 9);
  return;
}



/* ============ FUN_8010f130 @ 0x8010f130 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f130(void)

{
  (*(code *)(&PTR_FUN_80118d88)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_8010f178 @ 0x8010f178 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f178(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(0xfffffc18,0xa8c,0xfffffb50);
    FUN_80110988(0x4000,0,7);
    FUN_801108cc(0xfffff830,0xa8c,0x4b0);
    FUN_80110988(0x4000,0,7);
    FUN_801108cc(0xfffffc18,0xa8c,0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffff830,0x514,0xfffffb50);
    FUN_80110988(0x4000,0,7);
    FUN_801108cc(0xfffffc18,0x514,0x4b0);
    FUN_80110988(0x4000,0,7);
    FUN_801108cc(0xfffff830,0x514,0);
    FUN_80110988(0x4000,0,7);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (10 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_8010f2c0 @ 0x8010f2c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f2c0(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(0xfffff830,0x9c4,0xfffffb50);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x9c4,0x4b0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffff830,0x9c4,0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x44c,0xfffffb50);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffff830,0x44c,0x4b0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x44c,0);
    FUN_80110988(0x3000,0,7);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (10 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8010f3fc @ 0x8010f3fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f3fc(void)

{
  (*(code *)(&PTR_FUN_80118d90)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_8010f444 @ 0x8010f444 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f444(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x708,0);
    FUN_80110988(0x2000,0xffffff38,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x708,0xfffffda8);
    FUN_80110988(0x2600,0xffffff9c,0);
    FUN_801108cc(400,0x960,0);
    FUN_80110988(0x2600,0xffffff9c,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x708,0xfffffb50);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x960,0xfffffda8);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,3000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,0x960,0xfffffb50);
    FUN_80110988(0x3200,0,0);
    FUN_801108cc(800,3000,0xfffffda8);
    FUN_80110988(0x3200,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,3000,0xfffffb50);
    FUN_80110988(0x3800,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x19 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_8010f6b0 @ 0x8010f6b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f6b0(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x708,0xfffffb50);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x708,0xfffffda8);
    FUN_80110988(0x2600,100,0);
    FUN_801108cc(400,0x960,0xfffffb50);
    FUN_80110988(0x2600,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x708,0);
    FUN_80110988(0x2000,200,0);
    FUN_801108cc(600,0x960,0xfffffda8);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,3000,0xfffffb50);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,600,0);
    FUN_80110988(0x3200,100,0);
    FUN_801108cc(800,3000,0xfffffda8);
    FUN_80110988(0x3200,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,3000,0);
    FUN_80110988(0x3800,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x19 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_8010f910 @ 0x8010f910 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f910(void)

{
  (*(code *)(&PTR_LAB_80118da0)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_8010f9bc @ 0x8010f9bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f9bc(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x708,0);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x708,600);
    FUN_80110988(0x2600,100,0);
    FUN_801108cc(400,0x960,0);
    FUN_80110988(0x2600,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x708,0x4b0);
    FUN_80110988(0x2000,200,0);
    FUN_801108cc(600,0x960,600);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,3000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,600,0x4b0);
    FUN_80110988(0x3200,100,0);
    FUN_801108cc(800,3000,600);
    FUN_80110988(0x3200,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,3000,0x4b0);
    FUN_80110988(0x3800,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x19 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_8010fc28 @ 0x8010fc28 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc28(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x708,0x4b0);
    FUN_80110988(0x2000,200,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x708,600);
    FUN_80110988(0x2600,100,0);
    FUN_801108cc(400,600,0x4b0);
    FUN_80110988(0x2600,100,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x708,0);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x960,600);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,3000,0x4b0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,0x960,0);
    FUN_80110988(0x3200,0,0);
    FUN_801108cc(800,3000,600);
    FUN_80110988(0x3200,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,3000,0);
    FUN_80110988(0x3800,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x19 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8010fe24 @ 0x8010fe24 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe24(void)

{
  (*(code *)(&PTR_FUN_80118db0)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_8010fe6c @ 0x8010fe6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe6c(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x640,0);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x640,0xfffffda8);
    FUN_80110988(0x2000,0,0);
    FUN_801108cc(400,1000,0);
    FUN_80110988(0x2600,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x640,0xfffffb50);
    FUN_80110988(0x2000,0,0);
    FUN_801108cc(600,1000,0xfffffda8);
    FUN_80110988(0x2600,0,0);
    FUN_801108cc(600,400,0);
    FUN_80110988(0x3000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,400,0xfffffda8);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(800,1000,0xfffffb50);
    FUN_80110988(0x2600,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,400,0xfffffb50);
    FUN_80110988(0x3000,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_801100d8 @ 0x801100d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801100d8(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x640,0xfffffb50);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,1000,0xfffffb50);
    FUN_80110988(0x2400,0,0);
    FUN_801108cc(400,0x640,0xfffffda8);
    FUN_80110988(0x2400,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,400,0xfffffb50);
    FUN_80110988(0x2800,0,0);
    FUN_801108cc(600,1000,0xfffffda8);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x640,0);
    FUN_80110988(0x2800,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,400,0xfffffda8);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(800,1000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,400,0);
    FUN_80110988(0x3000,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_80110338 @ 0x80110338 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110338(void)

{
  (*(code *)(&PTR_LAB_80118dc0)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_801103e4 @ 0x801103e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801103e4(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x640,0);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,1000,0);
    FUN_80110988(0x2600,0,0);
    FUN_801108cc(400,0x640,600);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,400,0);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,1000,600);
    FUN_80110988(0x2600,0,0);
    FUN_801108cc(600,0x640,0x4b0);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,400,600);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(800,1000,0x4b0);
    FUN_80110988(0x2600,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,400,0x4b0);
    FUN_80110988(0x3000,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}



/* ============ FUN_80110650 @ 0x80110650 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110650(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x640,0x4b0);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,1000,0x4b0);
    FUN_80110988(0x2400,0,0);
    FUN_801108cc(400,0x640,600);
    FUN_80110988(0x2400,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,400,0x4b0);
    FUN_80110988(0x2800,0,0);
    FUN_801108cc(600,1000,600);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x640,0);
    FUN_80110988(0x2800,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,400,600);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(800,1000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,400,0);
    FUN_80110988(0x3000,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8011089c @ 0x8011089c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011089c(undefined1 param_1)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = param_1;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  return;
}



/* ============ FUN_801108cc @ 0x801108cc ============ */

void FUN_801108cc(short param_1,short param_2,short param_3)

{
  DAT_8011931c = (int)param_1;
  DAT_80119320 = (int)param_2;
  DAT_80119324 = (int)param_3;
  return;
}



/* ============ FUN_80110904 @ 0x80110904 ============ */

void FUN_80110904(void)

{
  int iVar1;
  
  iVar1 = func_0x8001af20();
  DAT_8011931c = DAT_8011931c + -0x180 + iVar1 * 3;
  iVar1 = func_0x8001af20();
  DAT_80119320 = DAT_80119320 + -0x200 + iVar1 * 4;
  iVar1 = func_0x8001af20();
  DAT_80119324 = DAT_80119324 + -0x100 + iVar1 * 2;
  return;
}



/* ============ FUN_80110988 @ 0x80110988 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110988(uint param_1,short param_2,uint param_3)

{
  DAT_80119318 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80019700((param_3 & 0xff) << 0x10 | param_1 & 0xffff | 0xf000000,(int)param_2,
                  DAT_80119318 + 0x40,&DAT_8011931c);
  return;
}



/* ============ FUN_801109e4 @ 0x801109e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801109e4(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_80118e24)[(byte)_DAT_800ac784[1]])();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)((int)_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0xd,*(undefined2 *)(_DAT_800ac784[0x1e] + 6),4);
    *(undefined2 *)((int)_DAT_800ac784 + 0x1da) = uVar1;
  }
  if ((*_DAT_800ac784 & 2) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x2d) = *(undefined2 *)(_DAT_800ac784[0x1e] + 6);
    *(short *)((int)_DAT_800ac784 + 0xb6) = (short)_DAT_800ac784[0x2d] + 100;
  }
  if ((int)_DAT_800ac784[0xe] < *(short *)((int)_DAT_800ac784 + 0x1ba) + -3000) {
    *(undefined2 *)(_DAT_800ac784 + 0x2d) = 0;
    *(undefined2 *)((int)_DAT_800ac784 + 0xb6) = 0;
  }
  func_0x8001b064(_DAT_800ac784 + 0x2c,(int)*(short *)((int)_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_80110b6c @ 0x80110b6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b6c(void)

{
  uint uVar1;
  char cVar2;
  
  _DAT_800ac784[1] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x10;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0;
  if (*(byte *)((int)_DAT_800ac784 + 9) < 2) {
    *(undefined2 *)(_DAT_800ac784 + 0x1a) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
  }
  if (*(byte *)((int)_DAT_800ac784 + 9) - 2 < 0x1e) {
    *(undefined2 *)(_DAT_800ac784 + 0x1a) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x800;
    _DAT_800ac784[0xe] = 0xffffd5d0;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 2;
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80118e04;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,0x514,0x578,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011717c + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0xccc;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1d6) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ============ FUN_80110e50 @ 0x80110e50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110e50(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x78) = uVar1;
  if ((ushort)((ushort)_DAT_800ac784[0x78] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x78] & 1;
  }
  uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
  if (*(char *)((int)_DAT_800ac784 + 5) == '\0') {
    (*(code *)(&PTR_FUN_80118e44)[*(byte *)((int)_DAT_800ac784 + 9)])();
  }
  (*(code *)(&PTR_FUN_80118e64)[*(byte *)((int)_DAT_800ac784 + 6)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((*(byte *)((int)_DAT_800ac784 + 9) < 2) || (*(byte *)((int)_DAT_800ac784 + 9) == 5)) {
    func_0x80012aa4(4000);
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  }
  else {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + 1;
  }
  return;
}



/* ============ FUN_80111040 @ 0x80111040 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111040(void)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0))
  {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 3;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) == 0) {
    cVar1 = func_0x8001af20();
    if ((cVar1 == '\0') && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 6;
    }
    if (((*(short *)(_DAT_800ac784 + 0x1d4) < 7000) &&
        (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar3 == 0)) &&
       (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 6) = (uVar2 & 1) + 6;
    }
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar4 = func_0x8001af20(), (uVar4 & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if ((DAT_800acae7 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
  }
  return;
}



/* ============ FUN_80111240 @ 0x80111240 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111240(void)

{
  undefined2 uVar1;
  ushort uVar2;
  uint uVar3;
  
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x80);
  *(undefined2 *)(_DAT_800ac784 + 0x1e2) = uVar1;
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x100);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = uVar1;
  if (((_DAT_800aca58 == 0x701) && (*(short *)(_DAT_800ac784 + 0x1e2) != 0)) &&
     (DAT_800aca5a == '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 3;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 8000) && (*(short *)(_DAT_800ac784 + 0x1e2) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 3;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) == 0) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 0x1d4c) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 6) = (uVar2 & 1) + 6;
    }
    if (((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) && (*(short *)(_DAT_800ac784 + 0x1e2) == 0)) &&
       (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 7;
    }
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar3 = func_0x8001af20(), (uVar3 & 0xf) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  return;
}



/* ============ FUN_80111488 @ 0x80111488 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111488(void)

{
  undefined2 uVar1;
  int iVar2;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 3000) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 8;
  }
  else if ((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 8;
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x1d4) < 0x1389) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 2;
    if (*(short *)(_DAT_800ac784 + 0x1d4) < 0x1389) {
      return;
    }
    iVar2 = func_0x8001a9cc(&DAT_800aca88,0x80);
    if (iVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 0xb;
  }
  *(undefined2 *)(_DAT_800ac784 + 6) = uVar1;
  return;
}



/* ============ FUN_801115e4 @ 0x801115e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801115e4(void)

{
  (*(code *)(&PTR_FUN_80118eb4)[*(byte *)(_DAT_800ac784 + 9)])();
  return;
}



/* ============ FUN_8011162c @ 0x8011162c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011162c(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) =
         (uVar2 & 0x3f) - ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * 0x50 + -0x50);
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    uVar3 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 6) =
         *(undefined1 *)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * 8 + -0x7fee715c + (uVar3 & 7));
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ============ FUN_80111768 @ 0x80111768 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111768(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_801117fc @ 0x801117fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801117fc(void)

{
  short sVar1;
  undefined4 uVar2;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(ushort *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + (*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x3c;
  uVar2 = 0;
  if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
    uVar2 = 0x800;
  }
  func_0x800245d8(uVar2);
  return;
}



/* ============ FUN_801118a8 @ 0x801118a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801118a8(void)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  ushort uVar4;
  undefined4 uVar5;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if ((((bVar1 & 3) != 0) && (*(byte *)(_DAT_800ac784 + 9) < 2)) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      uVar3 = 9;
      goto LAB_80111c48;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
    if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0x800;
    }
    if (((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 -
         ((int)*(short *)(_DAT_800ac784 + 0x6a) + (int)*(short *)(_DAT_800ac784 + 0x1e2))) + 0x200 &
        0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 2) +
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      if ((*(byte *)(_DAT_800ac784 + 0x90) & 0x40) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 4);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           **(short **)(_DAT_800ac784 + 0x1b4) + (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      return;
    }
  }
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) * (ushort)*(byte *)(_DAT_800ac784 + 6) + 0x20
    ;
    if (*(byte *)(_DAT_800ac784 + 6) >> 1 == 0) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x50;
    }
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(byte *)(_DAT_800ac784 + 0x95) == 0) || (uVar5 = 1, 8 < *(byte *)(_DAT_800ac784 + 0x95))) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x166);
    uVar5 = 0;
  }
  else {
    sVar2 = *(short *)(_DAT_800ac784 + 0x166);
  }
  FUN_8011388c(uVar5,(int)sVar2);
  *(ushort *)(_DAT_800ac784 + 0x8c) = (ushort)*(byte *)(_DAT_800ac784 + 0x8c);
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if (sVar2 != 0) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 4) = 1;
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  uVar3 = 0xd;
  if (1 < *(byte *)(_DAT_800ac784 + 9) - 3) {
    return;
  }
LAB_80111c48:
  *(undefined2 *)(_DAT_800ac784 + 6) = uVar3;
  return;
}



/* ============ FUN_80111c5c @ 0x80111c5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c5c(void)

{
  short sVar1;
  ushort uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = uVar2 & 0x3f;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x18;
    if ((*(ushort *)(_DAT_800ac784 + 0x9c) & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffe8;
    }
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e6);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) ||
     (iVar3 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6)), iVar3 != 0)) {
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_80111e04 @ 0x80111e04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e04(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x80;
  FUN_80111e40();
  return;
}



/* ============ FUN_80111e40 @ 0x80111e40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e40(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  undefined2 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(byte *)(_DAT_800ac784 + 6) = ((byte)*(undefined2 *)(_DAT_800ac784 + 0x1d0) & 1) * '\v';
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
      *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) + 0x50;
  }
  uVar4 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6));
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = uVar4;
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if ((sVar2 == 0) || (*(short *)(_DAT_800ac784 + 0x1e0) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e0);
  return;
}



/* ============ FUN_80112004 @ 0x80112004 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112004(void)

{
  short sVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x30);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 100;
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8011207c @ 0x8011207c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011207c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x20;
  FUN_80111e40();
  return;
}



/* ============ FUN_801120b8 @ 0x801120b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801120b8(void)

{
  byte bVar1;
  char cVar2;
  undefined2 uVar3;
  ushort uVar4;
  short sVar5;
  int iVar6;
  uint uVar7;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 2) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) != 0) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 9;
      return;
    }
    if (((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
         0x200 & 0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 2) +
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      if ((*(byte *)(_DAT_800ac784 + 0x90) & 0x40) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 4);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           **(short **)(_DAT_800ac784 + 0x1b4) + (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      return;
    }
  }
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    break;
  case 1:
  case 7:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) + 0x14;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar3 = func_0x8001a9cc(&DAT_800aca88,0x40);
    *(undefined2 *)(_DAT_800ac784 + 0x1e0) = uVar3;
    *(short *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e0);
    if (*(short *)(_DAT_800ac784 + 0x1e0) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
    if (sVar5 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) + 10;
  case 5:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar5 = func_0x8001a9cc(&DAT_800aca88,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar5;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0);
    sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
    if ((sVar5 == 0) || (iVar6 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar6 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d2) == 0) {
      return;
    }
    if (DAT_800acae7 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    func_0x800453d0(2);
    DAT_800aca58 = 2;
    cVar2 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar2 + 2;
    DAT_800aca5a = 0;
    DAT_800acae7 = DAT_800acae7 | 1;
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 1;
    }
    uVar7 = func_0x8001af20();
    func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar7 & 1)),0x32);
    return;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 7) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  default:
    goto switchD_801122c4_default;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800245d8(0);
switchD_801122c4_default:
  return;
}



/* ============ FUN_801126d4 @ 0x801126d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801126d4(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 4) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
      *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xf;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    func_0x80019700(0xd002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118ee8);
    func_0x800453d0(4);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_801127f0 @ 0x801127f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801127f0(void)

{
  int *piVar1;
  short sVar2;
  int iVar3;
  byte bVar4;
  char cVar5;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    iVar3 = _DAT_800ac784;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    piVar1 = (int *)(_DAT_800ac784 + 0x38);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + 1;
    *(int *)(iVar3 + 0x38) = (int)sVar2 * (int)sVar2 * 8 + *piVar1;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + 0x155;
    if (-1 < *(short *)(_DAT_800ac784 + 0x6c)) {
      *(undefined2 *)(_DAT_800ac784 + 0x6c) = 0;
    }
    if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x6c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      sVar2 = *(short *)(_DAT_800ac784 + 0x1ba);
      *(char *)(_DAT_800ac784 + 0x82) =
           (char)(sVar2 >> 0xf) - ((char)((int)sVar2 / 0x708) + (char)(sVar2 >> 7));
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
      func_0x800453d0(2);
      DAT_800aca58 = 2;
      cVar5 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar5 + '\x02';
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      if (_DAT_800acaee < 0) {
        _DAT_800acaee = 1;
      }
      func_0x80037edc(0,10);
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    func_0x800453d0(6);
    bVar4 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 9) = bVar4 & 1;
  case 3:
    cVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar5;
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80112af8 @ 0x80112af8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112af8(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80112b80:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar2 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    goto LAB_80112b80;
  }
  if (bVar1 == 2) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    if (*(short *)(_DAT_800ac784 + 0x1ec) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
  }
  else if (bVar1 != 3) {
    return;
  }
  if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) {
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x15') {
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0xa8c;
      *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + 0x708;
    }
    if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) goto LAB_80112d00;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x1a') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x904;
    *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
  }
LAB_80112d00:
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar3 == 0) {
    return;
  }
  *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
  *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x708;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 3000;
  if (*(short *)(_DAT_800ac784 + 0x1ec) != 0) {
    *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x02';
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xe10;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -200;
  }
  *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
  func_0x800245d8(0);
  *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
  *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
  *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  *(undefined1 *)(_DAT_800ac784 + 5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ============ FUN_80112e58 @ 0x80112e58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e58(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  ushort uVar4;
  short sVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar4 & 1) * -0x658 + 0x32c;
    iVar2 = _DAT_800ac784;
    uVar4 = func_0x8001af20();
    sVar5 = *(short *)(_DAT_800ac784 + 0x1e6);
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      sVar5 = sVar5 + (uVar4 & 0xff);
    }
    *(short *)(iVar2 + 0x1e6) = sVar5;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 200;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x800245d8((int)*(short *)(_DAT_800ac784 + 0x1e6));
  return;
}



/* ============ FUN_8011302c @ 0x8011302c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011302c(void)

{
  short sVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x14;
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801130a4 @ 0x801130a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801130a4(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e0c;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
      iVar2 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) * 0x10 + 0x800;
    }
    else {
      iVar2 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4;
    }
    sVar1 = func_0x8001aa68(iVar2,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar1 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      func_0x800245d8(0);
      break;
    case 2:
      func_0x800245d8(0x800);
      break;
    case 3:
      if (-0x2a30 < *(int *)(_DAT_800ac784 + 0x38)) {
        uVar3 = func_0x8001af20();
        iVar2 = (*(int *)(_DAT_800ac784 + 0x38) + -100) - (uVar3 & 0xf);
LAB_80113364:
        *(int *)(_DAT_800ac784 + 0x38) = iVar2;
      }
      break;
    case 4:
      if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -500) {
        uVar3 = func_0x8001af20();
        iVar2 = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar3 & 0xf);
        goto LAB_80113364;
      }
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x1f') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    break;
  case 4:
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      *(undefined1 *)(_DAT_800ac784 + 9) = 3;
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x5d0;
      break;
    case 2:
      *(undefined1 *)(_DAT_800ac784 + 9) = 4;
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x5d0;
      break;
    case 3:
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
      *(undefined1 *)(_DAT_800ac784 + 9) = 2;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xffffd5d0;
      uVar3 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar3 == 1) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
LAB_80113500:
        *(int *)(_DAT_800ac784 + 0x3c) = iVar2;
      }
      else if (uVar3 < 2) {
        if (uVar3 == 0) {
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
        }
      }
      else if (uVar3 == 2) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
      }
      else if (uVar3 == 3) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
        goto LAB_80113500;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x3c;
      break;
    case 4:
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
      *(undefined1 *)(_DAT_800ac784 + 9) = 5;
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      uVar3 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar3 == 1) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
LAB_801135f8:
        *(int *)(_DAT_800ac784 + 0x3c) = iVar2;
      }
      else if (uVar3 < 2) {
        if (uVar3 == 0) {
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
        }
      }
      else if (uVar3 == 2) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
      }
      else if (uVar3 == 3) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
        goto LAB_801135f8;
      }
    }
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
    case 2:
    case 5:
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0;
      break;
    case 3:
    case 4:
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xd;
    }
  }
  return;
}



/* ============ FUN_80113734 @ 0x80113734 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113734(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  FUN_801118a8();
  if (*(char *)(_DAT_800ac784 + 9) == '\x03') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = (*(int *)(_DAT_800ac784 + 0x38) + -100) - (uVar1 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) < -9000) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
  }
  if (*(char *)(_DAT_800ac784 + 9) == '\x04') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar1 & 0xf);
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
  }
  return;
}



/* ============ FUN_8011388c @ 0x8011388c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011388c(uint param_1,int param_2)

{
  undefined2 uVar1;
  byte bVar2;
  int iVar3;
  int local_48;
  int local_44;
  int local_40;
  undefined1 auStack_38 [20];
  int local_24 [3];
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,auStack_38);
  if (param_2 != 0) {
    local_48 = param_2;
    local_44 = param_2;
    local_40 = param_2;
    func_0x80065ff0(auStack_38,&local_48);
  }
  iVar3 = iVar3 + (param_1 & 0xff) * 0x560 + 0x2b0;
  bVar2 = 2;
  if ((*(uint *)(iVar3 + -0xac) & 1) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  }
  else {
    do {
      func_0x80022da0(auStack_38,iVar3 + (uint)bVar2 * -0xac + 0xc4,auStack_38);
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    local_24[1] = 0;
    local_24[0] = local_24[0] - *(int *)(iVar3 + 0x54);
    local_24[2] = local_24[2] - *(int *)(iVar3 + 0x5c);
    func_0x800662e8(local_24,&local_48);
    uVar1 = func_0x80065f60(local_48 + local_40);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  }
  return;
}



/* ============ FUN_80113a0c @ 0x80113a0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113a0c(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
  }
  (*(code *)(&PTR_FUN_80118ef8)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}



/* ============ FUN_80113aa4 @ 0x80113aa4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113aa4(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  uint *puVar6;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 4) = 1;
      *(byte *)(_DAT_800ac784 + 6) = (*(byte *)(_DAT_800ac784 + 9) & 1) + 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
      *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 10;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    func_0x800453d0(2);
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) {
      uVar4 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e0) = uVar4 & 7;
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 8;
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
      sVar2 = *(short *)(_DAT_800ac784 + 0x1e4);
      *(short *)(_DAT_800ac784 + 0x1e4) = sVar2 + -1;
      while (sVar2 != 0) {
        if ((&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] ==
            (&DAT_80118f30)[*(short *)(_DAT_800ac784 + 0x1e4)]) {
          *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 1;
        }
        sVar2 = *(short *)(_DAT_800ac784 + 0x1e4);
        *(short *)(_DAT_800ac784 + 0x1e4) = sVar2 + -1;
      }
      if (*(short *)(_DAT_800ac784 + 0x1e2) == 0) {
        (&DAT_80118f30)[*(short *)(_DAT_800ac784 + 0x1de)] =
             (&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)];
        puVar6 = (uint *)((uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                         *(int *)(_DAT_800ac784 + 0x188));
        puVar6[0x1a] = 0x207f60;
        *(undefined2 *)(puVar6 + 0x25) = 0;
        *(undefined2 *)((int)puVar6 + 0x96) = 0xffce;
        *(undefined2 *)(puVar6 + 0x26) = 0;
        *(undefined2 *)((int)puVar6 + 0x9a) = 3;
        *(undefined2 *)(puVar6 + 0x27) = 0;
        *(undefined2 *)((int)puVar6 + 0x9e) = 0;
        *puVar6 = *puVar6 | 0x4a;
        iVar5 = (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                *(int *)(_DAT_800ac784 + 0x188);
        *(undefined4 *)(iVar5 + 0x114) = 0x207f60;
        *(undefined2 *)(iVar5 + 0x140) = 0;
        *(undefined2 *)(iVar5 + 0x142) = 0xffce;
        *(undefined2 *)(iVar5 + 0x144) = 0;
        *(undefined2 *)(iVar5 + 0x146) = 3;
        *(undefined2 *)(iVar5 + 0x148) = 0;
        *(undefined2 *)(iVar5 + 0x14a) = 0;
        *(uint *)(iVar5 + 0xac) = *(uint *)(iVar5 + 0xac) | 0x4a;
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                        0x198,&DAT_80118f18);
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                        0x198,&DAT_80118f18);
        func_0x800453d0(5);
        *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
      }
    }
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  return;
}



/* ============ FUN_80113f40 @ 0x80113f40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f40(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  (*(code *)(&PTR_FUN_80118f38)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80113ff8 @ 0x80113ff8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113ff8(void)

{
  short *psVar1;
  short sVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  
  iVar4 = _DAT_800ac784;
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    puVar7 = *(uint **)(_DAT_800ac784 + 0x188);
    puVar7[0x1a] = 0x207f60;
    *(undefined2 *)(puVar7 + 0x25) = 0;
    *(undefined2 *)((int)puVar7 + 0x96) = 0xffce;
    *(undefined2 *)(puVar7 + 0x26) = 0;
    *(undefined2 *)((int)puVar7 + 0x9a) = 3;
    *(undefined2 *)(puVar7 + 0x27) = 0;
    *(undefined2 *)((int)puVar7 + 0x9e) = 0;
    *puVar7 = *puVar7 | 0x4a;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0xd2c) = 0x207f60;
    *(undefined2 *)(iVar4 + 0xd58) = 0;
    *(undefined2 *)(iVar4 + 0xd5a) = 0xffce;
    *(undefined2 *)(iVar4 + 0xd5c) = 0;
    *(undefined2 *)(iVar4 + 0xd5e) = 3;
    *(undefined2 *)(iVar4 + 0xd60) = 0;
    *(undefined2 *)(iVar4 + 0xd62) = 0;
    *(uint *)(iVar4 + 0xcc4) = *(uint *)(iVar4 + 0xcc4) | 0x4a;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xd04,&DAT_80118f18);
    func_0x800453d0(5);
    iVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
    sVar2 = *(short *)(_DAT_800ac784 + 0x1e0);
    iVar4 = *(int *)(iVar4 + 0x188);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar2 + -1;
    while (sVar2 != 1) {
      iVar6 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4;
      uVar5 = *(uint *)(iVar6 + 0xac);
      if ((uVar5 & 0x41) == 1) {
        *(uint *)(iVar6 + 0xac) = uVar5 | 0x1062;
        iVar6 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4;
        *(uint *)(iVar6 + 0x158) = *(uint *)(iVar6 + 0x158) | 0x1062;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x121) = 0x14;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x12e) = 0xff80;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x122) = 8;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe4) = 0;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe6) = 0xff9c;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe8) = 0;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1cd) = 0x14;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1da) = 0xff80;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1ce) = 8;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 400) = 0;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x192) = 0xff9c;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x194) = 0;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x1e0);
      *(short *)(_DAT_800ac784 + 0x1e0) = sVar2 + -1;
    }
  case 1:
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    break;
  case 2:
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
    break;
  case 3:
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 8;
    *(short *)(iVar4 + 0xbe) = *psVar1 + 8;
    *(uint *)(iVar4 + 0xc4) = *(uint *)(iVar4 + 0xc4) & 0xff000000 | 0xffff38;
    *(uint *)(iVar4 + 0xec) = *(uint *)(iVar4 + 0xec) & 0xff000000 | 0xffff38;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 1) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
  }
  return;
}



/* ============ FUN_8011461c @ 0x8011461c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011461c(void)

{
  short *psVar1;
  short sVar2;
  undefined1 uVar3;
  char cVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint *puVar8;
  
  if ((*(char *)((int)_DAT_800ac784 + 7) == '\0') &&
     (func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                      &DAT_80118f18), (short)_DAT_800ac784[0x76] != 0)) {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
  }
  if (((*(char *)((int)_DAT_800ac784 + 7) == '\x01') ||
      (*(char *)((int)_DAT_800ac784 + 7) == '\x03')) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x93) & 1) != 0)) {
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x40) == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 4;
      goto LAB_80114b3c;
    }
    puVar8 = (uint *)_DAT_800ac784[0x62];
    puVar8[0x1a] = 0x207f60;
    *(undefined2 *)(puVar8 + 0x25) = 0;
    *(undefined2 *)((int)puVar8 + 0x96) = 0xffce;
    *(undefined2 *)(puVar8 + 0x26) = 0;
    *(undefined2 *)((int)puVar8 + 0x9a) = 3;
    *(undefined2 *)(puVar8 + 0x27) = 0;
    *(undefined2 *)((int)puVar8 + 0x9e) = 0;
    *puVar8 = *puVar8 | 0x4a;
    uVar5 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar5 + 0xd2c) = 0x207f60;
    *(undefined2 *)(uVar5 + 0xd58) = 0;
    *(undefined2 *)(uVar5 + 0xd5a) = 0xffce;
    *(undefined2 *)(uVar5 + 0xd5c) = 0;
    *(undefined2 *)(uVar5 + 0xd5e) = 3;
    *(undefined2 *)(uVar5 + 0xd60) = 0;
    *(undefined2 *)(uVar5 + 0xd62) = 0;
    *(uint *)(uVar5 + 0xcc4) = *(uint *)(uVar5 + 0xcc4) | 0x4a;
    func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &DAT_80118f18);
    func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0xd04,
                    &DAT_80118f18);
    puVar8 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x78) = 9;
    sVar2 = (short)_DAT_800ac784[0x78];
    uVar5 = puVar8[0x62];
    *(short *)(_DAT_800ac784 + 0x78) = sVar2 + -1;
    while (sVar2 != 1) {
      iVar7 = (short)_DAT_800ac784[0x78] * 0x158 + uVar5;
      uVar6 = *(uint *)(iVar7 + 0xac);
      if ((uVar6 & 0x41) == 1) {
        *(uint *)(iVar7 + 0xac) = uVar6 | 0x1062;
        iVar7 = (short)_DAT_800ac784[0x78] * 0x158 + uVar5;
        *(uint *)(iVar7 + 0x158) = *(uint *)(iVar7 + 0x158) | 0x1062;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x121) = 0x14;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x12e) = 0xff80;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x122) = 8;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe4) = 0;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe6) = 0xff9c;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe8) = 0;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1cd) = 0x14;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1da) = 0xff80;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1ce) = 8;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 400) = 0;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x192) = 0xff9c;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x194) = 0;
      }
      sVar2 = (short)_DAT_800ac784[0x78];
      *(short *)(_DAT_800ac784 + 0x78) = sVar2 + -1;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
    uVar3 = 2;
code_r0x80114e00:
    *(undefined1 *)((int)_DAT_800ac784 + 7) = uVar3;
  }
  else {
LAB_80114b3c:
    puVar8 = _DAT_800ac784;
    switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
    case 0:
      *(undefined2 *)(_DAT_800ac784 + 0x76) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 4;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    case 1:
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      if (0xd < *(byte *)((int)_DAT_800ac784 + 0x95)) {
        *_DAT_800ac784 = *_DAT_800ac784 | 0x20000000;
      }
      cVar4 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar4;
      break;
    case 2:
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 5;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
    case 3:
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x20000000;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      break;
    case 4:
      func_0x800453d0(7);
      _DAT_800ac784[1] = 0x3000007;
      break;
    case 5:
      psVar1 = (short *)((int)_DAT_800ac784 + 0xbe);
      *(short *)(_DAT_800ac784 + 0x2f) = (short)_DAT_800ac784[0x2f] + 8;
      *(short *)((int)puVar8 + 0xbe) = *psVar1 + 8;
      puVar8[0x31] = puVar8[0x31] & 0xff000000 | 0xffff38;
      puVar8[0x3b] = puVar8[0x3b] & 0xff000000 | 0xffff38;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      uVar5 = _DAT_800ac784[0x27];
      *(short *)(_DAT_800ac784 + 0x27) = (short)uVar5 + -1;
      uVar3 = 6;
      if ((short)uVar5 != 1) {
        return;
      }
      goto code_r0x80114e00;
    }
  }
  return;
}



/* ============ FUN_80114e18 @ 0x80114e18 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114e18(void)

{
  (*(code *)(&PTR_LAB_80118f90)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80114ec8 @ 0x80114ec8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114ec8(void)

{
  int iVar1;
  uint uVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    if (*(char *)(_DAT_800ac784 + 9) == 'A') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e0c;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  case 1:
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0xb) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
      func_0x800245d8(0);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x1f') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 1000;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0x800);
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + 0x400;
    goto LAB_80115314;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 4:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar2 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar2 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) <= *(short *)(_DAT_800ac784 + 0x1ba) + -0x708) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    return;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 6:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -500) {
      uVar2 = func_0x8001af20();
      *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar2 & 0xf);
    }
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x1f') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 7;
    return;
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 7) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 9) = 5;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    uVar2 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
    if (uVar2 == 1) {
      iVar1 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
LAB_80115310:
      *(int *)(_DAT_800ac784 + 0x3c) = iVar1;
    }
    else if (uVar2 < 2) {
      if (uVar2 == 0) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
      }
    }
    else if (uVar2 == 2) {
      *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
    }
    else if (uVar2 == 3) {
      iVar1 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
      goto LAB_80115310;
    }
LAB_80115314:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(undefined2 *)(_DAT_800ac784 + 4) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  default:
    break;
  }
  return;
}



/* ============ FUN_801153d4 @ 0x801153d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801153d4(void)

{
  uint *puVar1;
  char cVar2;
  uint *puVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  case 1:
    puVar3 = _DAT_800ac784;
    puVar1 = _DAT_800ac784 + 0x3b;
    _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xdf809f;
    puVar3[0x3b] = *puVar1 & 0xff000000 | 0xdf809f;
    *(short *)(puVar3 + 0x2f) = (short)puVar3[0x2f] + 8;
    *(short *)((int)puVar3 + 0xbe) = *(short *)((int)puVar3 + 0xbe) + 8;
    break;
  default:
    goto switchD_8011540c_caseD_2;
  case 3:
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 4;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  case 4:
    puVar3 = _DAT_800ac784;
    puVar1 = _DAT_800ac784 + 0x3b;
    _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xdf809f;
    puVar3[0x3b] = *puVar1 & 0xff000000 | 0xdf809f;
    *(short *)(puVar3 + 0x2f) = (short)puVar3[0x2f] + 8;
    *(short *)((int)puVar3 + 0xbe) = *(short *)((int)puVar3 + 0xbe) + 8;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 2;
    func_0x800453d0(9);
  }
switchD_8011540c_caseD_2:
  return;
}



/* ============ FUN_801155e4 @ 0x801155e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801155e4(undefined4 param_1,int param_2)

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



/* ============ FUN_80115750 @ 0x80115750 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115750(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_80118fd0)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011588c @ 0x8011588c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011588c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80118fcc;
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



/* ============ FUN_80115a34 @ 0x80115a34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a34(void)

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
  (*(code *)(&PTR_FUN_80118fe4)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80115b00 @ 0x80115b00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115b00(void)

{
  (**(code **)(&DAT_80118fe8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80119008 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80115b7c @ 0x80115b7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115b7c(void)

{
  (**(code **)(&DAT_80119028 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80119048 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80115bf8 @ 0x80115bf8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115bf8(void)

{
  (**(code **)(&DAT_80119068 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80115c40 @ 0x80115c40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115c40(void)

{
  (**(code **)(&DAT_80119068 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80115c88 @ 0x80115c88 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115c88(void)

{
  (**(code **)(&DAT_80119068 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80115d20 @ 0x80115d20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d20(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_801190a0)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80115e5c @ 0x80115e5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e5c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011909c;
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



/* ============ FUN_80116004 @ 0x80116004 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116004(void)

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
  (*(code *)(&PTR_FUN_801190b4)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_801160d0 @ 0x801160d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801160d0(void)

{
  (**(code **)(&DAT_801190b8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801190d8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011614c @ 0x8011614c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011614c(void)

{
  (**(code **)(&DAT_801190f8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80119118 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801161c8 @ 0x801161c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161c8(void)

{
  (**(code **)(&DAT_80119138 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80116210 @ 0x80116210 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116210(void)

{
  (**(code **)(&DAT_80119138 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80116258 @ 0x80116258 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116258(void)

{
  (**(code **)(&DAT_80119138 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_801162f0 @ 0x801162f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801162f0(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_80119170)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80116420 @ 0x80116420 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116420(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011916c;
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



/* ============ FUN_801165c8 @ 0x801165c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801165c8(void)

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
  (*(code *)(&PTR_FUN_80119184)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80116694 @ 0x80116694 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116694(void)

{
  (**(code **)(&DAT_80119188 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801191a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80116710 @ 0x80116710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116710(void)

{
  (**(code **)(&DAT_801191c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801191e8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011678c @ 0x8011678c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011678c(void)

{
  (**(code **)(&DAT_80119208 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801167d4 @ 0x801167d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801167d4(void)

{
  (**(code **)(&DAT_80119208 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011681c @ 0x8011681c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011681c(void)

{
  (**(code **)(&DAT_80119208 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_801168b4 @ 0x801168b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801168b4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_80119240)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_801169f8 @ 0x801169f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801169f8(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011923c;
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



/* ============ FUN_80116ba0 @ 0x80116ba0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116ba0(void)

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
    (*(code *)(&PTR_FUN_80119254)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80116ccc @ 0x80116ccc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116ccc(void)

{
  (**(code **)(&DAT_8011925c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011927c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80116d48 @ 0x80116d48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d48(void)

{
  (**(code **)(&DAT_8011929c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801192bc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80116dc4 @ 0x80116dc4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116dc4(void)

{
  (**(code **)(&DAT_801192dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80116e0c @ 0x80116e0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116e0c(void)

{
  (**(code **)(&DAT_801192f0 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80116e54 @ 0x80116e54 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116e54(void)

{
  (**(code **)(&DAT_801192f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80116eec @ 0x80116eec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116eec(void)

{
  _DAT_80072bec = &LAB_801002b8;
  _DAT_80072bf0 = &LAB_801002b8;
  _DAT_80072bf4 = &LAB_801002b8;
  _DAT_80072c04 = &LAB_801002b8;
  _DAT_80072c0c = &LAB_801002b8;
  _DAT_80072c1c = &LAB_801002b8;
  _DAT_80072c20 = &LAB_801002b8;
  _DAT_80072c24 = &LAB_801002b8;
  _DAT_80072c28 = &LAB_801002b8;
  _DAT_80072c34 = FUN_8010c080;
  _DAT_80072c38 = FUN_8010c448;
  _DAT_80072c3c = FUN_8010ee9c;
  _DAT_80072c40 = FUN_801109e4;
  _DAT_80072cac = FUN_80115750;
  _DAT_80072cb4 = FUN_80115d20;
  _DAT_80072cd0 = FUN_801162f0;
  _DAT_80072cd8 = FUN_801168b4;
  _DAT_800b52c8 = &DAT_8011713c;
  _DAT_80072bf8 = FUN_8010a75c;
  _DAT_800ac998 = &DAT_801172bc;
  _DAT_800ac7a0 = &LAB_8010a120;
  _DAT_800ac7b0 = &LAB_8010a120;
  _DAT_800ac7b8 = &LAB_8010a120;
  _DAT_800ac7c8 = &LAB_8010a120;
  _DAT_800ac7cc = &LAB_8010a120;
  _DAT_800ac7d0 = &LAB_8010a120;
  _DAT_800ac7d4 = &LAB_8010a120;
  _DAT_800ac7e0 = &LAB_8010c3a8;
  _DAT_800ac7e4 = &LAB_8010eda0;
  _DAT_800ac798 = &LAB_8010c000;
  _DAT_800ac79c = &LAB_8010c000;
  _DAT_800ac7a4 = &LAB_8010c000;
  _DAT_800ac7ec = &LAB_801156b8;
  _DAT_800ac898 = &LAB_8010c040;
  _DAT_800ac8a0 = &LAB_8010a54c;
  _DAT_800ac8b0 = &LAB_8010a54c;
  _DAT_800ac8b8 = &LAB_8010a54c;
  _DAT_800ac8c8 = &LAB_8010a54c;
  _DAT_800ac8cc = &LAB_8010a54c;
  _DAT_800ac8d0 = &LAB_8010a54c;
  _DAT_800ac8d4 = &LAB_8010a54c;
  _DAT_800ac8e0 = &LAB_8010c3f8;
  _DAT_800ac8e4 = &LAB_8010edf0;
  _DAT_800ac8e8 = &LAB_80110854;
  _DAT_800ac89c = &LAB_8010c040;
  _DAT_800ac8a4 = &LAB_8010c040;
  _DAT_800ac8ec = &LAB_80115700;
  func_0x80029afc();
  return;
}



