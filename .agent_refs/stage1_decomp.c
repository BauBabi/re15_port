/* ============ FUN_80100688 @ 0x80100688 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100688(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f790;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_8011f7e4)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011f034 + (uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&DAT_8011f784)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (*(code *)(&PTR_FUN_8011f7ec)[uVar3 & 7])();
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
                    (uint)(byte)(&DAT_8011f7a4)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a5)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a6)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a7)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a8)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a9)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7aa)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7ab)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
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



/* ============ FUN_80101008 @ 0x80101008 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101008(void)

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



/* ============ FUN_80101058 @ 0x80101058 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101058(void)

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



/* ============ FUN_8010113c @ 0x8010113c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010113c(void)

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



/* ============ FUN_80101224 @ 0x80101224 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101224(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a5)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a6)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a7)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_8011f80c)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010168c @ 0x8010168c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010168c(void)

{
  (*(code *)(&PTR_FUN_8011f840)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f890)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101708 @ 0x80101708 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101708(void)

{
  (*(code *)(&PTR_FUN_8011f8e0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f920)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101784 @ 0x80101784 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101784(void)

{
  (*(code *)(&PTR_FUN_8011f960)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f890)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101800 @ 0x80101800 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101800(void)

{
  (*(code *)(&PTR_FUN_8011f9b0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f920)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010187c @ 0x8010187c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010187c(void)

{
  (*(code *)(&PTR_FUN_8011f9c8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9cc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801018f8 @ 0x801018f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801018f8(void)

{
  (*(code *)(&PTR_FUN_8011f9d0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9d4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101974 @ 0x80101974 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101974(void)

{
  (*(code *)(&PTR_LAB_8011f9d8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9d4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801019f0 @ 0x801019f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019f0(void)

{
  (*(code *)(&PTR_FUN_8011f9dc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9d4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101a6c @ 0x80101a6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101a6c(void)

{
  (*(code *)(&PTR_LAB_8011f9e0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9e4)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101ae8 @ 0x80101ae8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ae8(void)

{
  (*(code *)(&PTR_LAB_8011f9e8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011f9ec)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101b64 @ 0x80101b64 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b64(void)

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



/* ============ FUN_80101c7c @ 0x80101c7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c7c(void)

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



/* ============ FUN_80101d08 @ 0x80101d08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101d08(void)

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
    FUN_8010939c(0,1);
  }
  return;
}



/* ============ FUN_80101de4 @ 0x80101de4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101de4(void)

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



/* ============ FUN_80101ef0 @ 0x80101ef0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ef0(void)

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
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80102058 @ 0x80102058 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102058(void)

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



/* ============ FUN_801021f8 @ 0x801021f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801021f8(void)

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
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011fb00)[uVar5 & 0x1f];
    }
    else {
      uVar5 = func_0x8001af20();
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011faf0)[uVar5 & 0x1f];
    }
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_8011f9f0 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011f9f2 +
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
          (&DAT_8011f9f0 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80102548 @ 0x80102548 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102548(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_8011f7d4);
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



/* ============ FUN_80102bd8 @ 0x80102bd8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102bd8(void)

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



/* ============ FUN_80102d20 @ 0x80102d20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102d20(void)

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



/* ============ FUN_80102dc8 @ 0x80102dc8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102dc8(void)

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



/* ============ FUN_80102f1c @ 0x80102f1c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102f1c(void)

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



/* ============ FUN_80103014 @ 0x80103014 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103014(void)

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
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801031e4 @ 0x801031e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801031e4(void)

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
    goto switchD_8010321c_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010321c_default:
  return;
}



/* ============ FUN_801033c8 @ 0x801033c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801033c8(void)

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



/* ============ FUN_801035f8 @ 0x801035f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801035f8(void)

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



/* ============ FUN_801036dc @ 0x801036dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801036dc(void)

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
    FUN_80109470(0,uVar1);
  }
  return;
}



/* ============ FUN_8010385c @ 0x8010385c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010385c(void)

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



/* ============ FUN_80103980 @ 0x80103980 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103980(void)

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



/* ============ FUN_801039fc @ 0x801039fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801039fc(void)

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



/* ============ FUN_80103a58 @ 0x80103a58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a58(void)

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



/* ============ FUN_80103b94 @ 0x80103b94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103b94(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_8011f7d4);
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
        puVar5 = (uint *)((uint)(byte)(&DAT_8011f784)[(byte)_DAT_800ac784[2]] * 0xac +
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



/* ============ FUN_80104178 @ 0x80104178 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104178(void)

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
    puVar3 = (uint *)((uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ============ FUN_80104548 @ 0x80104548 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104548(void)

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



/* ============ FUN_8010466c @ 0x8010466c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010466c(void)

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



/* ============ FUN_80104760 @ 0x80104760 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104760(void)

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



/* ============ FUN_80104808 @ 0x80104808 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104808(void)

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



/* ============ FUN_801048e8 @ 0x801048e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801048e8(void)

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



/* ============ FUN_80104a50 @ 0x80104a50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104a50(void)

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



/* ============ FUN_80104b40 @ 0x80104b40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b40(void)

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
    goto LAB_80104edc;
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
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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
LAB_80104edc:
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



/* ============ FUN_80104f80 @ 0x80104f80 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f80(void)

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



/* ============ FUN_8010512c @ 0x8010512c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010512c(void)

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
    FUN_8010939c(0,0);
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    uVar4 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) =
         (short)*(undefined4 *)(&DAT_8011fb10 + (uVar4 & 0xf) * 4) * 0x1e;
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



/* ============ FUN_801054f4 @ 0x801054f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801054f4(void)

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



/* ============ FUN_8010561c @ 0x8010561c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010561c(void)

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



/* ============ FUN_801057bc @ 0x801057bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801057bc(void)

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
         *(undefined2 *)(&DAT_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011fb52 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&DAT_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80105a8c @ 0x80105a8c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a8c(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0470))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  else if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_80106a38();
  }
  else {
    FUN_801068a0();
  }
  return;
}



/* ============ FUN_80105b7c @ 0x80105b7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105b7c(void)

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
      else if (bVar2 != 3) goto LAB_8010602c;
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
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011fb00)[uVar4 & 0x1f];
        }
        else {
          uVar4 = func_0x8001af20();
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011faf0)[uVar4 & 0x1f];
        }
        if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) =
             *(undefined2 *)
              (&DAT_8011f9f0 +
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
      goto LAB_8010602c;
    }
    if (bVar2 != 0) goto LAB_8010602c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
              *(int *)(_DAT_800ac784 + 0x188);
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0x40,&LAB_8011fe84);
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    (*(code *)(&PTR_LAB_8011fe30)[*(byte *)(_DAT_800ac784 + 5)])();
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
LAB_8010602c:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80106048 @ 0x80106048 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106048(void)

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
      goto LAB_8010626c;
    }
    if (bVar1 != 0) goto LAB_8010626c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar3 = func_0x8001af20();
    *(undefined *)(_DAT_800ac784 + 0x94) = (&DAT_8011fe94)[uVar3 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011fe9c);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
    (*(code *)(&PTR_LAB_8011fe30)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_8010626c:
  FUN_8010939c(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_80106290 @ 0x80106290 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106290(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_8010640c:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80106608;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_80106608;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (*(code *)(&PTR_LAB_8011fe30)[*(byte *)(_DAT_800ac784 + 5)])();
      goto LAB_8010640c;
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
      goto LAB_80106608;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80106608;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  if (cVar2 == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
  }
LAB_80106608:
  FUN_8010939c(0,0);
  return;
}



/* ============ FUN_80106624 @ 0x80106624 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106624(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
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



/* ============ FUN_801068a0 @ 0x801068a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801068a0(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011fe9c);
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



/* ============ FUN_80106a38 @ 0x80106a38 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106a38(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011fe9c);
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



/* ============ FUN_80106ba4 @ 0x80106ba4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106ba4(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0154))();
  }
  else {
    FUN_80107cb0();
  }
  return;
}



/* ============ FUN_80106c18 @ 0x80106c18 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106c18(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80106ea0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8012016c);
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
LAB_80106ea0:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_8010939c(0,0);
  }
  return;
}



/* ============ FUN_80106edc @ 0x80106edc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106edc(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80107200;
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
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
    puVar7 = (uint *)((uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8012016c);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8012016c);
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
LAB_80107200:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_8010939c(0,0);
  }
  return;
}



/* ============ FUN_80107244 @ 0x80107244 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107244(void)

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
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
    puVar7 = (uint *)((uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8012016c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8012016c);
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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80107634 @ 0x80107634 ============ */

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



/* ============ FUN_80107cb0 @ 0x80107cb0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80107cb0(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
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
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8012016c);
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)uVar3;
  return uVar3;
}



/* ============ FUN_80107ee0 @ 0x80107ee0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107ee0(void)

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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar9 + 0x10,&LAB_8012016c);
    func_0x800453d0(9);
    uVar10 = func_0x8001af20();
    if ((uVar10 & 1) != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      FUN_80108d04((uint)(byte)(&DAT_8011f784)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62])
      ;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108d04(uVar6 + 0x560);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x60c) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x5e8) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x5ea) = 0xc18;
        *(undefined2 *)(uVar6 + 0x5ec) = 0x5f4;
        *(uint *)(uVar6 + 0x560) = *(uint *)(uVar6 + 0x560) | 0x400;
        FUN_80108d04(_DAT_800ac784[0x62] + 0x60c);
      }
      *(undefined4 *)(_DAT_800ac784[0x62] + 0x6b8) = 0;
      uVar6 = _DAT_800ac784[0x62];
      uVar10 = func_0x8001af20();
      if ((uVar10 & 1) == 0) {
        FUN_80108d04(uVar6 + 0x764);
        *(undefined4 *)(_DAT_800ac784[0x62] + 0x810) = 0;
      }
      else {
        *(undefined2 *)(uVar6 + 0x7ec) = 0x5f4;
        *(undefined2 *)(uVar6 + 0x7ee) = 0xc18;
        *(undefined2 *)(uVar6 + 0x7f0) = 0x5f4;
        *(uint *)(uVar6 + 0x764) = *(uint *)(uVar6 + 0x764) | 0x400;
        FUN_80108d04(_DAT_800ac784[0x62] + 0x810);
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
    func_0x80019700(0x2800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_8012016c);
    func_0x80019700(0x5002800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,
                    &LAB_8012016c);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f784)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62]
                    + 0x40,&LAB_8012016c);
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
                        &LAB_8012016c);
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
        goto joined_r0x80108750;
      }
      *(uint *)(_DAT_800ac784[0x62] + 0x560) = *(uint *)(_DAT_800ac784[0x62] + 0x560) | 0x200;
      *(uint *)(_DAT_800ac784[0x62] + 0x764) = *(uint *)(_DAT_800ac784[0x62] + 0x764) | 0x200;
    }
    else {
      uVar1 = *(ushort *)(uVar8 + 0x536);
      *(char *)(uVar8 + 0x529) = cVar3 + -1;
joined_r0x80108750:
      if ((uVar1 & 1) != 0) {
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar8 + 0x4f4,
                        &LAB_8012016c);
      }
    }
    cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_80108a48:
    if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
      FUN_8010939c(0,0);
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
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),uVar10 + 0x4f4,&LAB_8012016c);
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
    goto LAB_80108a48;
  case 4:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}



/* ============ FUN_80108abc @ 0x80108abc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108abc(void)

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
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108d04(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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



/* ============ FUN_80108d04 @ 0x80108d04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108d04(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&LAB_8012016c);
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



/* ============ FUN_80108d74 @ 0x80108d74 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108d74(void)

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
      goto LAB_80109164;
    }
    if (bVar1 != 0) goto LAB_80109164;
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a4)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a5)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a6)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a7)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
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
LAB_80109164:
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_8010939c(0,0);
  }
  return;
}



/* ============ FUN_8010919c @ 0x8010919c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010919c(void)

{
  (**(code **)(&DAT_8012018c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801091e4 @ 0x801091e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801091e4(void)

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
  FUN_8010939c(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801092f4 @ 0x801092f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801092f4(void)

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
    FUN_8010939c(0,1);
  }
  return;
}



/* ============ FUN_8010939c @ 0x8010939c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010939c(undefined4 param_1,int param_2)

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



/* ============ FUN_80109470 @ 0x80109470 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109470(undefined4 param_1,int param_2)

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



/* ============ FUN_80109554 @ 0x80109554 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109554(void)

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
    goto LAB_80109d74;
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
    goto LAB_80109d74;
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
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a4)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a5)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a6)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a7)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a8)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a9)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7aa)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7ab)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
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
LAB_80109d74:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar5;
  }
  return;
}



/* ============ FUN_80109d98 @ 0x80109d98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d98(void)

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



/* ============ FUN_80109e4c @ 0x80109e4c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109e4c(void)

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
      func_0x80019700(0x2800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar6 + 0x10,&DAT_8011f7d4);
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



/* ============ FUN_8010a1cc @ 0x8010a1cc ============ */

void FUN_8010a1cc(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a208 @ 0x8010a208 ============ */

void FUN_8010a208(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a244 @ 0x8010a244 ============ */

void FUN_8010a244(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ============ FUN_8010a268 @ 0x8010a268 ============ */

void FUN_8010a268(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ============ FUN_8010a6f8 @ 0x8010a6f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a6f8(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801201c8
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801201c8
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  return;
}



/* ============ FUN_8010a8c8 @ 0x8010a8c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a8c8(void)

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
    (*(code *)(&PTR_FUN_80120208)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0) {
      *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xed;
    }
    func_0x8001b4e4();
    FUN_80104178();
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



/* ============ FUN_8010ab2c @ 0x8010ab2c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ab2c(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f790;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_8011f7e4)[uVar3 & 7];
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
                    (uint)(byte)(&DAT_8011f7a4)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a5)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a6)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a7)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a9)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7aa)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7ab)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011f7d4);
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



/* ============ FUN_8010b274 @ 0x8010b274 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b274(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a5)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a6)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a7)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_80120230)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010b6d4 @ 0x8010b6d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b6d4(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_80120264)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801202a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b784 @ 0x8010b784 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b784(void)

{
  (*(code *)(&PTR_FUN_801202ec)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120308)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b800 @ 0x8010b800 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b800(void)

{
  (*(code *)(&PTR_FUN_80120324)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801202a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b87c @ 0x8010b87c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b87c(void)

{
  (*(code *)(&PTR_FUN_8012035c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120308)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b8f8 @ 0x8010b8f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8f8(void)

{
  (*(code *)(&PTR_FUN_80120374)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120378)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b974 @ 0x8010b974 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b974(void)

{
  (*(code *)(&PTR_FUN_8012037c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120380)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b9f0 @ 0x8010b9f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9f0(void)

{
  (*(code *)(&PTR_LAB_80120384)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120380)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ba6c @ 0x8010ba6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba6c(void)

{
  (*(code *)(&PTR_FUN_80120388)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120380)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bae8 @ 0x8010bae8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bae8(void)

{
  (*(code *)(&PTR_FUN_8012038c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120390)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bb64 @ 0x8010bb64 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb64(void)

{
  (*(code *)(&PTR_LAB_80120394)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120398)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bbe8 @ 0x8010bbe8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bbe8(void)

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



/* ============ FUN_8010be50 @ 0x8010be50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be50(void)

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
    FUN_8010c088(0,uVar1);
  }
  return;
}



/* ============ FUN_8010bf80 @ 0x8010bf80 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf80(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedfc64))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_80106a38();
  }
  else {
    FUN_801068a0();
  }
  return;
}



/* ============ FUN_8010c014 @ 0x8010c014 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c014(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf9c4))();
  }
  else {
    FUN_80107cb0();
  }
  return;
}



/* ============ FUN_8010c088 @ 0x8010c088 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c088(undefined4 param_1,int param_2)

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



/* ============ FUN_8010c1ec @ 0x8010c1ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c1ec(void)

{
  int iVar1;
  int iVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar1 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10
    ;
    iVar2 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10
    ;
    func_0x80065f60(iVar1 * iVar1 + iVar2 * iVar2);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_8012093c)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  }
  return;
}



/* ============ FUN_8010c33c @ 0x8010c33c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c33c(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80120934;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 1;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x74) = (uVar2 & 0x1f) + 0x1e;
  bVar1 = func_0x8001af20();
  *(byte *)((int)_DAT_800ac784 + 0x1d2) = (bVar1 & 3) + 1;
  uVar3 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar3 + 0x6a4) = 0x60;
  *(undefined2 *)(uVar3 + 0x6a6) = 0x30;
  *(undefined2 *)(uVar3 + 0x6a8) = 0x390;
  *(undefined2 *)(uVar3 + 0x6a0) = 0;
  *(undefined2 *)(uVar3 + 0x6a2) = 0;
  *(undefined2 *)(uVar3 + 0x6aa) = 0x138;
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  return;
}



/* ============ FUN_8010c488 @ 0x8010c488 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c488(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  (*(code *)(&PTR_FUN_8012095c)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010c510 @ 0x8010c510 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c510(void)

{
  (*(code *)(&PTR_LAB_80120968)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120984)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c58c @ 0x8010c58c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c58c(void)

{
  (*(code *)(&PTR_LAB_80120968)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120984)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010c678 @ 0x8010c678 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c678(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8010c714 @ 0x8010c714 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c714(void)

{
  short sVar1;
  byte bVar2;
  undefined4 uVar3;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_8010c7bc:
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    }
  }
  else {
    if (bVar2 < 2) {
      if (bVar2 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 800;
      goto LAB_8010c7bc;
    }
    if (bVar2 == 2) {
      sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
      if (sVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      uVar3 = 0x800;
      goto LAB_8010c918;
    }
    if (bVar2 != 3) {
      return;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 5) = (bVar2 & 1) + 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar3 = 0;
LAB_8010c918:
  func_0x800245d8(uVar3);
  return;
}



/* ============ FUN_8010c938 @ 0x8010c938 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c938(void)

{
  short sVar1;
  int iVar2;
  byte bVar3;
  char cVar4;
  ushort uVar5;
  uint uVar6;
  undefined4 uVar7;
  
  bVar3 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar3 == 1) {
LAB_8010c990:
    bVar3 = func_0x8001af20();
    iVar2 = _DAT_800ac784;
    cVar4 = (bVar3 & 3) + 1;
    *(char *)(_DAT_800ac784 + 0x9f) = cVar4;
    *(char *)(iVar2 + 0x9e) = cVar4;
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x7f) + 0x32;
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar6 = func_0x8001af20();
    if ((uVar6 & 1) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    }
  }
  else {
    if (bVar3 < 2) {
      if (bVar3 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      goto LAB_8010c990;
    }
    if (bVar3 != 2) {
      if (bVar3 != 3) {
        return;
      }
      cVar4 = *(char *)(_DAT_800ac784 + 0x9f);
      *(char *)(_DAT_800ac784 + 0x9f) = cVar4 + -1;
      if (cVar4 == '\0') {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        sVar1 = *(short *)(_DAT_800ac784 + 0x1d0);
        *(short *)(_DAT_800ac784 + 0x1d0) = sVar1 + -1;
        if (sVar1 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 3;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          uVar5 = func_0x8001af20();
          *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar5 & 0x1f) + 0x1e;
        }
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      uVar7 = 0;
      goto LAB_8010cb14;
    }
  }
  cVar4 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar4 + -1;
  if (cVar4 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar7 = 0x800;
LAB_8010cb14:
  func_0x800245d8(uVar7);
  return;
}



/* ============ FUN_8010cb34 @ 0x8010cb34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cb34(void)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        if (bVar1 != 3) {
          return;
        }
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) & 0xfffffbff;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) & 0xfffffbff;
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        uVar2 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar2 & 0x1f) + 0x1e;
        return;
      }
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(short *)(iVar4 + 0x28e) = *(short *)(iVar4 + 0x28e) + *(char *)(iVar4 + 0x27a) * -2;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(short *)(iVar4 + 0x1e2) = *(short *)(iVar4 + 0x1e2) + *(char *)(iVar4 + 0x1ce) * -2;
      if (*(short *)(iVar4 + 400) < 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      }
      *(short *)(iVar4 + 400) = *(short *)(iVar4 + 400) + -2;
      goto LAB_8010cda0;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 5;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x38);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x1e0) = 0x1000;
    *(undefined2 *)(iVar4 + 0x1e2) = 0x1000;
    *(undefined2 *)(iVar4 + 0x1e4) = 0x1000;
    *(uint *)(iVar4 + 0x158) = *(uint *)(iVar4 + 0x158) | 0x400;
    uVar2 = func_0x8001af20();
    sVar3 = (uVar2 & 0xff) + 100;
    *(short *)(iVar4 + 400) = sVar3;
    *(short *)(iVar4 + 0x1da) = sVar3;
    bVar1 = func_0x8001af20();
    *(byte *)(iVar4 + 0x1ce) = (bVar1 & 7) + 5;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x28c) = 0x1000;
    *(undefined2 *)(iVar4 + 0x28e) = 0x1000;
    *(undefined2 *)(iVar4 + 0x290) = 0x1000;
    *(uint *)(iVar4 + 0x204) = *(uint *)(iVar4 + 0x204) | 0x400;
    bVar1 = func_0x8001af20();
    *(byte *)(iVar4 + 0x27a) = bVar1 & 7;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x9c) + (int)*(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(short *)(iVar4 + 0x28e) = (short)*(char *)(iVar4 + 0x27a) + *(short *)(iVar4 + 0x28e);
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar3 = *(short *)(iVar4 + 0x1da);
  *(short *)(iVar4 + 0x1e2) = (short)*(char *)(iVar4 + 0x1ce) + *(short *)(iVar4 + 0x1e2);
  *(short *)(iVar4 + 0x1da) = sVar3 + -1;
  if (sVar3 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
LAB_8010cda0:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8010ce54 @ 0x8010ce54 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ce54(void)

{
  char cVar1;
  byte bVar2;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      if (*(char *)(_DAT_800ac784 + 0x9e) == '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x1d2) = (bVar2 & 3) + 1;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    if (*(char *)(_DAT_800ac784 + 5) == '\x04') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  return;
}



/* ============ FUN_8010cfe0 @ 0x8010cfe0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cfe0(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0xff) + 0x1e;
  }
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if (sVar2 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}



/* ============ FUN_8010d0f8 @ 0x8010d0f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d0f8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf660))();
  if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8010d188 @ 0x8010d188 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d188(void)

{
  short sVar1;
  byte bVar2;
  int iVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x93) >> 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&LAB_8011fe84);
    (*(code *)(&PTR_LAB_80120c40)[*(byte *)(_DAT_800ac784 + 5)])();
  case 1:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x400);
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    func_0x800245d8(0x800);
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(short *)(_DAT_800ac784 + 0x8c);
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x14;
    if (*(short *)(_DAT_800ac784 + 0x8c) < 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 2:
    iVar3 = (int)*(short *)(_DAT_800ac784 + 0x9c);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 3;
    }
    *(short *)(_DAT_800ac784 + 0x8c) = (short)(iVar3 >> 2);
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 4;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar2 & 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    func_0x800245d8(0);
    break;
  case 4:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  }
  return;
}



/* ============ FUN_8010d474 @ 0x8010d474 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d474(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf36c))();
  return;
}



/* ============ FUN_8010d4c4 @ 0x8010d4c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d4c4(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&LAB_8011fe84);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_8010d5d0 @ 0x8010d5d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d5d0(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar3 + 0xec,&LAB_8011fe84);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined4 *)(iVar3 + 0x114) = 0x8f;
    *(undefined2 *)(iVar3 + 0x142) = 0xffce;
    *(undefined2 *)(iVar3 + 0x140) = 0;
    *(undefined2 *)(iVar3 + 0x144) = 0;
    *(undefined2 *)(iVar3 + 0x146) = 3;
    *(undefined2 *)(iVar3 + 0x148) = 0;
    *(undefined2 *)(iVar3 + 0x14a) = 0;
    *(uint *)(iVar3 + 0xac) = *(uint *)(iVar3 + 0xac) | 0x4a;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  if (*(short *)(_DAT_800ac784 + 0x8c) < 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  }
  return;
}



/* ============ FUN_8010d7f8 @ 0x8010d7f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d7f8(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_80120f74)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    uVar2 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar2;
    func_0x8002b544();
    uVar2 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
    uVar1 = 1;
    if ((*(byte *)(_DAT_800ac784 + 4) < 2) && (uVar1 = 0x10, 1 < *(byte *)(_DAT_800ac784 + 5) - 1))
    {
      uVar1 = 0x12;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = uVar1;
    func_0x8001b4e4();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_8010d93c @ 0x8010d93c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d93c(void)

{
  uint uVar1;
  
  _DAT_800ac784[1] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 3;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x29c) = 0x48;
  *(undefined2 *)(uVar1 + 0x29e) = 0x30;
  *(undefined2 *)(uVar1 + 0x2a0) = 0x390;
  *(undefined2 *)(uVar1 + 0x298) = 0;
  *(undefined2 *)(uVar1 + 0x29a) = 0;
  *(undefined2 *)(uVar1 + 0x2a2) = 0x138;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x70) = 0;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80120f70;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,1000,500,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011f034 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1d6) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0;
  _DAT_800aca50 = 0;
  if (*(char *)((int)_DAT_800ac784 + 9) == '@') {
    _DAT_800ac784[1] = 4;
  }
  if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  return;
}



/* ============ FUN_8010dbcc @ 0x8010dbcc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dbcc(void)

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
  (*(code *)(&PTR_LAB_80120f94)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80120fd4)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
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



/* ============ FUN_8010dea0 @ 0x8010dea0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dea0(void)

{
  short sVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    sVar1 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + 300;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8010df9c @ 0x8010df9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010df9c(void)

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
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
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
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_8010e0c4 @ 0x8010e0c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e0c4(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800acae7 != '\0') || (iVar1 = func_0x8001a804(0x9c4,0xc0,&DAT_800aca88), -1 < iVar1)) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
       ((_DAT_800aca58 == 0x701 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      goto LAB_8010e244;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) &&
       ((_DAT_800aca58 == 0x201 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      goto LAB_8010e244;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1e6) == 0) &&
       (((0x50 < _DAT_800acaee && (7000 < *(short *)(_DAT_800ac784 + 0x1d4))) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)))) {
      uVar2 = func_0x8001af20();
      uVar3 = func_0x8001af20();
      if ((uVar2 & 1) * (uVar3 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        goto LAB_8010e244;
      }
    }
    if ((_DAT_800aca50 & 1) == 0) {
      if ((*(byte *)(_DAT_800ac784 + 0x90) & 3) == 0) {
        return;
      }
      if (0x3ff < (((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 -
                   (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff)) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1ea) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10)
      ;
      *(ushort *)(_DAT_800ac784 + 0x1e8) = *(byte *)(_DAT_800ac784 + 0x90) & 1;
      *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
      *(undefined2 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 3;
LAB_8010e244:
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ============ FUN_8010e304 @ 0x8010e304 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e304(void)

{
  undefined2 uVar1;
  int iVar2;
  ushort uVar3;
  short sVar4;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 7) + 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar2 = _DAT_800ac784;
  if (*(short *)(_DAT_800ac784 + 0x1dc) < 0x1f) {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar3 & 0xf) + 1;
    func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x9c) == 0) {
      uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
      func_0x8001af20();
      *(char *)(iVar2 + 0x9f) = (char)uVar1;
    }
    sVar4 = func_0x8001aa68((int)*(char *)(_DAT_800ac784 + 0x9f),0x20);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar4;
    sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + 1;
    if (0x5a < sVar4) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(short *)(_DAT_800ac784 + 0x1e6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1e6) = *(short *)(_DAT_800ac784 + 0x1e6) + -1;
  }
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1e')) {
    func_0x800453d0(6);
  }
  FUN_80111870(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x1e);
  func_0x800245d8(8);
  return;
}



/* ============ FUN_8010e568 @ 0x8010e568 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e568(void)

{
  int iVar1;
  uint uVar2;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d6) == 0) &&
     (iVar1 = func_0x8001a804(3000,0x180,&DAT_800aca88), iVar1 < 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') && (*(undefined1 *)(_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      uVar2 = func_0x8001af20();
      if ((uVar2 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      }
    }
  }
  else if (((*(byte *)(_DAT_800ac784 + 0x90) & 3) != 0) &&
          ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)
            ) + 0x200 & 0xfff) < 0x400)) {
    *(undefined2 *)(_DAT_800ac784 + 0x1ea) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10);
    *(ushort *)(_DAT_800ac784 + 0x1e8) = *(byte *)(_DAT_800ac784 + 0x90) & 1;
    *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8010e6d4 @ 0x8010e6d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e6d4(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  short sVar5;
  uint uVar6;
  
  FUN_80111f0c(0x14,0xf0);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 0xf0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0xf) + 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  if (((*(short *)(_DAT_800ac784 + 0x1dc) == 0) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) &&
     (0xef < *(short *)(_DAT_800ac784 + 0x8c))) {
    sVar4 = func_0x8001a9cc(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  else {
    sVar4 = func_0x8001a9cc(&DAT_800aca88,0x10);
    if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
      sVar4 = func_0x8001a9cc(&DAT_800aca88,0x40);
    }
    *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
     (sVar5 = *(short *)(_DAT_800ac784 + 0x9c) + 1, *(short *)(_DAT_800ac784 + 0x9c) = sVar5,
     0x3c < sVar5)) {
    func_0x8001a8f8(&DAT_800aca88,0x40);
  }
  if (*(short *)(_DAT_800ac784 + 0x1d0) == 0) {
    cVar2 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar2;
    if (cVar2 == '\x06') {
      FUN_80111f6c(0x1e,200);
    }
    if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
    *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  }
  FUN_80111fcc((int)sVar4);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar6 + (uVar6 / 7 + (uVar6 - uVar6 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010ea44 @ 0x8010ea44 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea44(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x3c;
    break;
  case 1:
  case 3:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(2);
    _DAT_800aca50 = _DAT_800aca50 | 1;
    break;
  case 4:
    _DAT_800aca50 = _DAT_800aca50 & 0xfffe;
    if (*(short *)(_DAT_800ac784 + 0x9a) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 3;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x2f0,&DAT_80120f54);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  default:
    goto switchD_8010ea7c_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010ea7c_default:
  return;
}



/* ============ FUN_8010ecb4 @ 0x8010ecb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ecb4(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 1;
    break;
  case 1:
  case 3:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(8);
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  default:
    goto switchD_8010ecec_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010ecec_default:
  return;
}



/* ============ FUN_8010ee90 @ 0x8010ee90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee90(void)

{
  byte bVar1;
  char cVar2;
  
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
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x95);
  if (((cVar2 == '\n') || (cVar2 == '\x19')) || (cVar2 == '-')) {
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_8010efbc @ 0x8010efbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010efbc(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1b')) {
    func_0x800453d0(6);
  }
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
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
  }
  uVar3 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar3 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80111870(0,*(byte *)(_DAT_800ac784 + 0x95) < 0xd);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_8010f15c @ 0x8010f15c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f15c(void)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    func_0x800453d0(5);
switchD_8010f198_caseD_1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
      func_0x800245d8(0);
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xdc;
      if ((DAT_800acae7 == 0) && (iVar3 = func_0x8001a804(2000,0x180,&DAT_800aca88), iVar3 < 0)) {
        _DAT_800acaee = _DAT_800acaee - 10;
        if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
          func_0x800453d0(3);
          *(undefined1 *)(_DAT_800ac784 + 6) = 5;
          DAT_800aca58 = 2;
          cVar1 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar1 + 2;
          DAT_800aca5a = 0;
          DAT_800acae7 = DAT_800acae7 | 1;
          uVar4 = func_0x8001af20();
          func_0x80037edc(*(undefined1 *)
                           ((int)&PTR_LAB_80121010 + (uVar4 & 1) + (uint)DAT_800aca59 * 2),0x32);
        }
        else {
LAB_8010f468:
          cVar1 = func_0x8001a780(&DAT_800aca54);
          *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
          *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
        }
      }
    }
switchD_8010f198_default:
    return;
  case 1:
    goto switchD_8010f198_caseD_1;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  case 3:
    if (1 < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
      func_0x800245d8(8);
      *(int *)(_DAT_800ac784 + 0x38) =
           (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xb) * 0x10000 >> 0x10) +
           *(int *)(_DAT_800ac784 + 0x38);
      if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
        *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
        *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      }
LAB_8010f598:
      cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),
                              *(undefined1 *)(_DAT_800ac784 + 7),0x200);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
      return;
    }
    func_0x800245d8(0);
    if ((DAT_800acae7 != 0) || (iVar3 = func_0x8001a804(2000,0x180,&DAT_800aca88), -1 < iVar3))
    goto LAB_8010f598;
    _DAT_800acaee = _DAT_800acaee - 10;
    if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
      func_0x800453d0(3);
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      DAT_800aca58 = 2;
      cVar1 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar1 + 2;
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      goto LAB_8010f598;
    }
    goto LAB_8010f468;
  case 4:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 10;
    uVar2 = 3;
code_r0x8010f7c4:
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
    goto LAB_8010f7c8;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  case 6:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0x10 < *(byte *)(_DAT_800ac784 + 0x95)) {
      return;
    }
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
    func_0x800245d8(0x800);
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xe) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
    if (*(int *)(_DAT_800ac784 + 0x38) <= (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
      return;
    }
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    return;
  case 7:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    if ((short)_DAT_800acaee < 0x32) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    uVar4 = func_0x8001af20();
    uVar2 = 6;
    if ((uVar4 & 1) != 0) goto code_r0x8010f7c4;
LAB_8010f7c8:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  default:
    goto switchD_8010f198_default;
  }
}



/* ============ FUN_8010f80c @ 0x8010f80c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f80c(void)

{
  char cVar1;
  short sVar2;
  undefined1 uVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    func_0x800453d0(3);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x17;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x28) = _DAT_800aca88;
    *(undefined2 *)((int)_DAT_800ac784 + 0xa2) = _DAT_800aca90;
    _DAT_800acaf4 = _DAT_800aca88;
    _DAT_800acaf6 = _DAT_800aca90;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 0;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x18;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      uVar3 = 0xb;
LAB_8010fc34:
      *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    }
    else {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
      if ((short)_DAT_800ac784[0x27] < 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
        DAT_800aca5a = 4;
      }
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x19;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    cVar1 = func_0x8001af20();
    *(byte *)((int)_DAT_800ac784 + 0x9e) = cVar1 + 0x1eU & 0x3f;
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 6) == '\x06') {
      func_0x800453d0(7);
    }
    break;
  case 6:
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    uVar3 = 0xc;
    goto LAB_8010fc34;
  default:
    break;
  }
  return;
}



/* ============ FUN_8010fc60 @ 0x8010fc60 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc60(void)

{
  char cVar1;
  short sVar2;
  undefined1 uVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    func_0x800453d0(3);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x28) = _DAT_800aca88;
    *(undefined2 *)((int)_DAT_800ac784 + 0xa2) = _DAT_800aca90;
    _DAT_800acaf4 = _DAT_800aca88;
    _DAT_800acaf6 = _DAT_800aca90;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 1;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x18;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      uVar3 = 0xb;
LAB_80110088:
      *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    }
    else {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
      if ((short)_DAT_800ac784[0x27] < 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
        DAT_800aca5a = 4;
      }
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x19;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    cVar1 = func_0x8001af20();
    *(byte *)((int)_DAT_800ac784 + 0x9e) = cVar1 + 0x1eU & 0x3f;
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 6) == '\x06') {
      func_0x800453d0(7);
    }
    break;
  case 6:
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    uVar3 = 0xc;
    goto LAB_80110088;
  default:
    break;
  }
  return;
}



/* ============ FUN_801100b4 @ 0x801100b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801100b4(void)

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
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1b;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    DAT_800aca58 = 6;
    DAT_800aca59 = 0;
    DAT_800aca5a = 0;
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  }
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_801101e4 @ 0x801101e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801101e4(void)

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
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_801102dc @ 0x801102dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801102dc(void)

{
  short sVar1;
  int iVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar1 = func_0x8001aa68((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar1 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4;
  case 3:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x12') {
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0xc1c;
      *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
    }
    iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar2 != 0) {
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xc1c;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      func_0x800245d8(0);
      *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
      *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
      *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_801105bc @ 0x801105bc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801105bc(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      }
      else if (bVar1 != 3) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0xe;
        uVar5 = (*(ushort *)(_DAT_800ac784 + 0x1ea) & 0xc) >> 2;
        do {
          *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + 0x708;
          uVar4 = uVar5 & 0xff;
          *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
          uVar5 = uVar5 - 1;
        } while (uVar4 != 0);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x509;
        func_0x800245d8(0);
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3f8;
        *(short *)(_DAT_800ac784 + 0x9c) =
             *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
      }
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0xe < 6) {
        iVar3 = (int)*(short *)(_DAT_800ac784 + 0x9c);
        if (iVar3 < 0) {
          iVar3 = iVar3 + 7;
        }
        *(int *)(_DAT_800ac784 + 0x38) = (iVar3 >> 3) + *(int *)(_DAT_800ac784 + 0x38);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
        func_0x800245d8(0);
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
        *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar2 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  if (sVar2 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}



/* ============ FUN_801108f0 @ 0x801108f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801108f0(void)

{
  if (*(int *)(_DAT_800ac784 + 0x38) < (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(short *)(_DAT_800ac784 + 0x9c) =
           *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xc) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
  }
  else {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  }
  (*(code *)(&PTR_FUN_80121018)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801109e0 @ 0x801109e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801109e0(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      if (*(short *)(_DAT_800ac784 + 0x9a) < 0) {
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 1;
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1c0) = 0x8000;
    func_0x800453d0(1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&DAT_80120f54);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_80110b9c @ 0x80110b9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b9c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
      *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
      if (cVar2 != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x96;
    *(undefined2 *)(_DAT_800ac784 + 0x1c0) = 0x8000;
    cVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = cVar2 + 0x2dU & 0x3f;
    func_0x800453d0(1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80120f54);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -2;
  func_0x800245d8((1 - (*(byte *)(_DAT_800ac784 + 0x93) & 0x80)) * 0x800);
  if (*(char *)(_DAT_800ac784 + 7) == '\x02') {
    func_0x800453d0(7);
  }
  return;
}



/* ============ FUN_80110dc0 @ 0x80110dc0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110dc0(void)

{
  if (*(int *)(_DAT_800ac784 + 0x38) < (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(short *)(_DAT_800ac784 + 0x9c) =
           *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xc) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
  }
  else {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  }
  (*(code *)(&PTR_FUN_80121070)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80110eb0 @ 0x80110eb0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110eb0(void)

{
  short *psVar1;
  int iVar2;
  char cVar3;
  
  iVar2 = _DAT_800ac784;
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x96;
    *(undefined2 *)(_DAT_800ac784 + 0x1c0) = 0x8000;
    func_0x800453d0(4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80120f54);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  case 1:
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -2;
    func_0x800245d8((1 - (*(byte *)(_DAT_800ac784 + 0x93) & 0x80)) * 0x800);
    if (*(char *)(_DAT_800ac784 + 7) == '\x02') {
      func_0x800453d0(7);
    }
    break;
  case 2:
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
    break;
  case 3:
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 8;
    *(short *)(iVar2 + 0xbe) = *psVar1 + 8;
    *(uint *)(iVar2 + 0xc4) = *(uint *)(iVar2 + 0xc4) & 0xff000000 | 0xffff38;
    *(uint *)(iVar2 + 0xec) = *(uint *)(iVar2 + 0xec) & 0xff000000 | 0xffff38;
    cVar3 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar3 + -1;
    if (cVar3 == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
  }
  return;
}



/* ============ FUN_80111120 @ 0x80111120 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111120(void)

{
  short *psVar1;
  int iVar2;
  char cVar3;
  
  iVar2 = _DAT_800ac784;
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    break;
  case 1:
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 2:
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      func_0x800453d0(7);
    }
    break;
  case 3:
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
    break;
  case 4:
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 8;
    *(short *)(iVar2 + 0xbe) = *psVar1 + 8;
    *(uint *)(iVar2 + 0xc4) = *(uint *)(iVar2 + 0xc4) & 0xff000000 | 0xffff38;
    *(uint *)(iVar2 + 0xec) = *(uint *)(iVar2 + 0xec) & 0xff000000 | 0xffff38;
    cVar3 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar3 + -1;
    if (cVar3 == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    }
  }
  return;
}



/* ============ FUN_80111350 @ 0x80111350 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111350(void)

{
  (*(code *)(&PTR_FUN_801210c8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80111398 @ 0x80111398 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111398(void)

{
  char cVar1;
  int iVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    if (*(char *)(_DAT_800ac784 + 9) != 'C') {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    func_0x800453d0(5);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
      func_0x800245d8(0);
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x14;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 3:
    if (*(byte *)(_DAT_800ac784 + 0x95) < 2) {
      func_0x800245d8(0);
    }
    else {
      iVar2 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffe2,
                              (int)*(short *)(_DAT_800ac784 + 0x1ba));
      if (iVar2 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      }
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),
                    *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    }
    break;
  case 4:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x78;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  }
  return;
}



/* ============ FUN_80111658 @ 0x80111658 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111658(void)

{
  short *psVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    iVar2 = _DAT_800ac784;
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 300;
    *(short *)(iVar2 + 0xbe) = *psVar1 + 300;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  if (*(char *)(_DAT_800ac784 + 9) == 'B') {
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
    iVar2 = _DAT_800ac784;
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + -300;
    *(short *)(iVar2 + 0xbe) = *psVar1 + -300;
  }
  return;
}



/* ============ FUN_80111870 @ 0x80111870 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111870(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
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



/* ============ FUN_80111f0c @ 0x80111f0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f0c(short param_1,short param_2)

{
  if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
    *(short *)(_DAT_800ac784 + 0x8c) = param_1 + *(short *)(_DAT_800ac784 + 0x8c);
    if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}



/* ============ FUN_80111f6c @ 0x80111f6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f6c(short param_1,short param_2)

{
  if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) - param_1;
    if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}



/* ============ FUN_80111fcc @ 0x80111fcc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111fcc(short param_1)

{
  char *pcVar1;
  
  pcVar1 = &DAT_801210f8;
  if (param_1 == 0) {
    pcVar1 = &DAT_801210f9;
  }
  else if (0 < param_1) {
    pcVar1 = &DAT_801210fa;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar1) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar1;
  }
  return;
}



/* ============ FUN_80112020 @ 0x80112020 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112020(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  uVar1 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x1d4) = uVar1;
  iVar3 = func_0x8004efe4(0x800b1028,0x1f);
  if (((iVar3 != 0) && (*(char *)(_DAT_800ac784 + 4) == '\x04')) &&
     ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 3;
    FUN_80115d74(0);
    func_0x8004efb8(0x800b1028,0x1f);
  }
  iVar3 = func_0x8004efe4(0x800b1028,0x1d);
  if (((iVar3 != 0) && (DAT_800aca58 == '\x05')) && (DAT_800aca5a < 3)) {
    DAT_800aca5a = 3;
  }
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    *(char *)(_DAT_800ac784 + 0x82) = -(char)(*(int *)(_DAT_800ac784 + 0x38) / 0x708);
    uVar2 = func_0x8001c6e8(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,8,0x400);
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = uVar2;
    (*(code *)(&PTR_FUN_8012111c)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined1 *)(_DAT_800ac784 + 0x1d1) = uVar1;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_8011224c @ 0x8011224c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011224c(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
  _DAT_800aca50 = func_0x8001b768(0x21);
  _DAT_800aca50 = _DAT_800aca50 << 4;
  *(undefined2 *)(_DAT_800ac784 + 0xa0) = *(undefined2 *)(_DAT_800ac784 + 0x34);
  *(undefined2 *)(_DAT_800ac784 + 0xa2) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
  FUN_80115d94(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80121108;
  func_0x8001af5c(0,0,400,400,_DAT_800ac784 + 0xb0,0xa0a0a0);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    *(undefined4 *)(uVar1 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x1ea) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -400;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x1db) = 1;
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x1db) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 4;
  }
  return;
}



/* ============ FUN_80112420 @ 0x80112420 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112420(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  uVar1 = func_0x8001bc08();
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d2) = uVar1;
  *(byte *)((int)_DAT_800ac784 + 0x1d2) = *(byte *)((int)_DAT_800ac784 + 0x1d2) & 1;
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x77) = uVar2;
  *(short *)(_DAT_800ac784 + 0x7b) = _DAT_800aca8c - (short)_DAT_800ac784[0xe];
  if ((_DAT_800aca50 & 0xff00) != 0) {
    FUN_80116068();
  }
  (*(code *)(&PTR_FUN_8012113c)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_LAB_80121184)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((short)_DAT_800ac784[0x7b] < 0xfa1) {
    if ((short)_DAT_800ac784[0x7b] < 800) {
      func_0x80012974(6000);
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
    }
  }
  else {
    func_0x80012a0c(6000);
  }
  FUN_80115f70();
  if ((_DAT_800aca50 & 0x800) != 0) {
    if (*(char *)((int)_DAT_800ac784 + 0x1db) == '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1db) = 1;
    }
  }
  return;
}



/* ============ FUN_80112628 @ 0x80112628 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112628(void)

{
  bool bVar1;
  short sVar2;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + -0x28;
    }
    else {
      sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + 0x28;
    }
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2;
    FUN_80115d74(*(byte *)(_DAT_800ac784 + 0x1d4) & 3);
  }
  if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
    bVar1 = *(ushort *)(_DAT_800ac784 + 0x1dc) < 5000;
  }
  else {
    bVar1 = *(ushort *)(_DAT_800ac784 + 0x1dc) < 10000;
  }
  if ((bVar1) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if (((_DAT_800aca52 & 1) != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}



/* ============ FUN_80112854 @ 0x80112854 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112854(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(1);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  return;
}



/* ============ FUN_801128e4 @ 0x801128e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801128e4(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(2);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  return;
}



/* ============ FUN_80112974 @ 0x80112974 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112974(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    func_0x800453d0(6);
  }
  return;
}



/* ============ FUN_80112a30 @ 0x80112a30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a30(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      goto LAB_80112af4;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 1;
    FUN_80115d94(6);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xffb0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x3c;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_80112af4:
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  FUN_80115e24();
  func_0x800245d8(0);
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    FUN_80115d74(5);
  }
  return;
}



/* ============ FUN_80112bac @ 0x80112bac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112bac(void)

{
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0xe10) {
    FUN_80115d74(9);
  }
  return;
}



/* ============ FUN_80112bec @ 0x80112bec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112bec(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      goto LAB_80112ca0;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_80112ca0:
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  FUN_80115e24();
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_80112d34 @ 0x80112d34 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112d34(void)

{
  char cVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(byte *)(_DAT_800ac784 + 0x1d5) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x32;
    FUN_80115d94(6);
    func_0x800453d0(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    FUN_80115d74(9);
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  cVar1 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_80112e4c @ 0x80112e4c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e4c(void)

{
  if (*(short *)(_DAT_800ac784 + 0x1ba) != *(short *)(_DAT_800ac784 + 0x1ea)) {
    FUN_80115d74(5);
  }
  return;
}



/* ============ FUN_80112e88 @ 0x80112e88 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e88(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 3;
        *(int *)(_DAT_800ac784 + 0x38) =
             (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee < *(int *)(_DAT_800ac784 + 0x38)) {
          *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
          *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0xc;
        }
        FUN_80115e24();
      }
      else {
        if (bVar1 != 3) {
          return;
        }
        *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 9;
        *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + -9;
        *(int *)(_DAT_800ac784 + 0x38) =
             (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
        *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(8);
        }
      }
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x8c;
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x15;
    FUN_80115d94(5);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 3;
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 8;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_801130fc @ 0x801130fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801130fc(void)

{
  if (*(short *)(_DAT_800ac784 + 0x1ba) != *(short *)(_DAT_800ac784 + 0x1ea)) {
    FUN_80115d74(5);
  }
  return;
}



/* ============ FUN_80113138 @ 0x80113138 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113138(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(7);
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 10;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  iVar1 = *(short *)(_DAT_800ac784 + 0x1ba) + -400;
  if (iVar1 <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = iVar1;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    FUN_80115d74(0);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 0;
  }
  return;
}



/* ============ FUN_8011325c @ 0x8011325c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011325c(void)

{
  int iVar1;
  undefined4 uVar2;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) == '\0') goto LAB_80113300;
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
  if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0xf) == 0) {
    iVar1 = _DAT_800ac784;
    if (((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) == 0) &&
       (iVar1 = 6, *(char *)(_DAT_800ac784 + 5) != '\x06')) goto LAB_801132d0;
    FUN_80115f00(iVar1);
  }
  else {
    iVar1 = 9;
LAB_801132d0:
    FUN_80115d74(iVar1);
  }
  if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
    FUN_80115f00();
  }
LAB_80113300:
  if ((((*(char *)(_DAT_800ac784 + 0x1d2) != '\0') && (*(char *)(_DAT_800ac784 + 0x1db) != '\0')) &&
      (*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000)) && (*(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    uVar2 = 10;
    if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
      uVar2 = 0xc;
    }
    FUN_80115d74(uVar2);
  }
  return;
}



/* ============ FUN_80113384 @ 0x80113384 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113384(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
      uVar3 = 4;
    }
    else {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
      uVar3 = 3;
    }
    FUN_80115d94(uVar3);
    cVar1 = FUN_80115dc8();
    *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  if (*(byte *)(_DAT_800ac784 + 0x1d6) < 3) {
    func_0x8001a8f8(&DAT_800aca88,0x1e);
    FUN_80115e24();
  }
  else if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 4) != 0) {
    func_0x8001a8f8(&DAT_800aca88,0x1e);
  }
  func_0x800245d8(0);
  uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar2;
  return;
}



/* ============ FUN_801134f8 @ 0x801134f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801134f8(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1db) == '\0') {
    FUN_80115d74(5);
  }
  if (*(ushort *)(_DAT_800ac784 + 0x1dc) < 0x9c4) {
    FUN_80115d74(0xe);
  }
  if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
  }
  return;
}



/* ============ FUN_80113590 @ 0x80113590 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113590(void)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x8001a8f8(&DAT_800aca88,0x32);
      func_0x800245d8(0);
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      iVar3 = func_0x8001a804(9000,100,&DAT_800aca88);
      if (-1 < iVar3) {
        return;
      }
      FUN_80115d74(0xb);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    FUN_80115d94(6);
    func_0x800453d0(1);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0xe10 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    uVar2 = -*(short *)(_DAT_800ac784 + 0x1e4);
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x1518 <= *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      goto LAB_801136dc;
    }
    uVar2 = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
  }
  *(ushort *)(_DAT_800ac784 + 0x1e4) = uVar2;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
LAB_801136dc:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8011376c @ 0x8011376c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011376c(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 0x1d1) != '\0') {
    FUN_80115d74(0xe);
  }
  iVar1 = func_0x8001b9b4(&DAT_800aca88);
  if (iVar1 != 0) {
    FUN_80115d74(0xe);
  }
  if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
  }
  return;
}



/* ============ FUN_801137fc @ 0x801137fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801137fc(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(5);
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
        }
        *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 2;
        func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
        *(short *)(_DAT_800ac784 + 0x23) =
             (short)_DAT_800ac784[0x23] + *(short *)((int)_DAT_800ac784 + 0x1e6);
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
        if ((((ushort)_DAT_800ac784[0x77] < 600) && ((ushort)_DAT_800ac784[0x7b] - 1 < 0xe0f)) &&
           (DAT_800acae7 == '\0')) {
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 6;
          *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0xffec;
          func_0x800453d0(4);
          DAT_800aca58 = 2;
          _DAT_800acaee = _DAT_800acaee + -4;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          _DAT_800aca5a = 0;
          if (_DAT_800acaee < 0) {
            DAT_800aca58 = 3;
            _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
            DAT_800aca59 = '\0';
            _DAT_800aca5a = 0;
            *(undefined1 *)(_DAT_800ac784 + 0x76) = 1;
          }
          FUN_801161e8();
        }
        if (*(byte *)((int)_DAT_800ac784 + 0x1d7) != 0) {
          func_0x80019700((uint)*(byte *)((int)_DAT_800ac784 + 0x1d7) << 0xb,
                          (int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x198,
                          &DAT_8012110c);
          *(char *)((int)_DAT_800ac784 + 0x1d7) = *(char *)((int)_DAT_800ac784 + 0x1d7) + -1;
        }
      }
      goto code_r0x80113be0;
    }
    if (bVar1 != 0) goto code_r0x80113be0;
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x50;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 200;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 9;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
code_r0x80113be0:
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80113c0c @ 0x80113c0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c0c(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1db) == '\0') {
    FUN_80115d74(0xe);
  }
  if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
  }
  return;
}



/* ============ FUN_80113c7c @ 0x80113c7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c7c(void)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      FUN_80115d94(4);
      uVar2 = 0xb4;
    }
    else {
      FUN_80115d94(3);
      uVar2 = 0xa0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar2;
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar3 != 0) {
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x8001a8f8(&DAT_800aca88,0x32);
    func_0x800245d8(0);
    if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x8000;
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      _DAT_800aca5a = 0;
      DAT_800aca59 = 0;
      _DAT_800acaee = _DAT_800acaee + -8;
      DAT_800aca58 = 5;
      FUN_80115d74(0xd);
    }
    cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
    *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
    if (cVar1 == '\0') {
      FUN_80115d74(5);
    }
  }
  return;
}



/* ============ FUN_80113e94 @ 0x80113e94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e94(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    *(char *)(_DAT_800ac784 + 0x1d6) = *(char *)(_DAT_800ac784 + 0x1d6) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
    FUN_801161e8();
    if (_DAT_800acaee < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar3 != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  }
  if ((uint)*(byte *)(_DAT_800ac784 + 0x1d5) % 10 == 0) {
    func_0x800453d0(0);
  }
  sVar2 = func_0x80037024();
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar2 * -3;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    _DAT_800aca5a = CONCAT11(DAT_800aca5b,DAT_800aca5a + '\x01');
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x4000;
    FUN_80115d74(0xe);
  }
  return;
}



/* ============ FUN_80114100 @ 0x80114100 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114100(void)

{
  if (12000 < *(ushort *)(_DAT_800ac784 + 0x1dc)) {
    FUN_80115d74(9);
  }
  return;
}



/* ============ FUN_8011413c @ 0x8011413c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011413c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(9);
      }
      goto LAB_801142a0;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x20;
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x3c;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) != 0) {
      *(short *)(_DAT_800ac784 + 0x1de) = -*(short *)(_DAT_800ac784 + 0x1de);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1de);
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x5a;
  }
LAB_801142a0:
  cVar2 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar2;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8011431c @ 0x8011431c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011431c(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(char *)(_DAT_800ac784 + 0x1d6) = *(char *)(_DAT_800ac784 + 0x1d6) + '\x01';
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x8001a8f8(&DAT_800aca88,0x32);
    if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
      FUN_80115d74(0x10);
    }
    func_0x800245d8(0);
  }
  return;
}



/* ============ FUN_80114484 @ 0x80114484 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114484(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    FUN_801161e8();
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    _DAT_800acaee = _DAT_800acaee - 4;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_80114594 @ 0x80114594 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114594(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,100);
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) + 0x30;
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee) {
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  }
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(ushort *)(_DAT_800ac784 + 0x1dc) < 1000) &&
     (*(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee < *(int *)(_DAT_800ac784 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}



/* ============ FUN_801146d0 @ 0x801146d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801146d0(void)

{
  func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  (*(code *)(&PTR_FUN_801211cc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80114738 @ 0x80114738 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114738(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 1) = 7;
      FUN_80115d74(0);
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(3);
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x7a) = 0x26;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
  }
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar3 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)uVar3 <= (int)_DAT_800ac784[0xe]) {
    _DAT_800ac784[0xe] = uVar3;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    FUN_80115d94(10);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  return;
}



/* ============ FUN_801149c4 @ 0x801149c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801149c4(void)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  ushort uVar4;
  
  if (*(char *)((int)_DAT_800ac784 + 7) == '\0') {
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
    func_0x800453d0(3);
    uVar4 = 0;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    uVar3 = 0;
    do {
      puVar2 = (uint *)(uVar3 * 0xac + _DAT_800ac784[0x62]);
      puVar2[0x1a] = 0x8f;
      *(undefined2 *)(puVar2 + 0x25) = 0;
      *(undefined2 *)((int)puVar2 + 0x96) = 0xffce;
      *(undefined2 *)(puVar2 + 0x26) = 0;
      *(undefined2 *)((int)puVar2 + 0x9a) = 3;
      *(undefined2 *)(puVar2 + 0x27) = 0;
      *(undefined2 *)((int)puVar2 + 0x9e) = 0;
      *puVar2 = *puVar2 | 0x4a;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x32;
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      uVar4 = uVar4 + 1;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      uVar3 = (uint)uVar4;
    } while (uVar4 < 0xd);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  else if ((*(char *)((int)_DAT_800ac784 + 7) == '\x01') &&
          (cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5),
          *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1, cVar1 == '\0')) {
    uVar4 = 0;
    do {
      uVar3 = (uint)uVar4;
      uVar4 = uVar4 + 1;
      *(undefined4 *)(uVar3 * 0xac + _DAT_800ac784[0x62]) = 0;
    } while (uVar4 < 0xd);
    *(undefined1 *)(_DAT_800ac784 + 1) = 7;
    FUN_80115d74(1);
  }
  return;
}



/* ============ FUN_80114ba4 @ 0x80114ba4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114ba4(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 1) = 7;
      FUN_80115d74(0);
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(3);
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x7a) = 0x26;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784 + 8,
                    &DAT_8012110c);
  }
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar3 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)uVar3 <= (int)_DAT_800ac784[0xe]) {
    _DAT_800ac784[0xe] = uVar3;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    FUN_80115d94(10);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  return;
}



/* ============ FUN_80114e54 @ 0x80114e54 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114e54(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  
  *(char *)(_DAT_800ac784 + 0x82) = -(char)(*(int *)(_DAT_800ac784 + 0x38) / 0x708);
  uVar1 = func_0x8001c6e8(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),8
                          ,0x400);
  *(undefined2 *)(_DAT_800ac784 + 0x1ba) = uVar1;
  iVar2 = _DAT_800aca88 - *(int *)(_DAT_800ac784 + 0x34);
  iVar3 = _DAT_800aca90 - *(int *)(_DAT_800ac784 + 0x3c);
  uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = uVar1;
  *(short *)(_DAT_800ac784 + 0x1ec) = _DAT_800aca8c - *(short *)(_DAT_800ac784 + 0x38);
  iVar2 = func_0x8004efe4(0x800b1028,0x1d);
  if ((iVar2 != 0) && (*(char *)(_DAT_800ac784 + 5) != '\x03')) {
    FUN_80115d74(3);
  }
  (*(code *)(&PTR_FUN_80121220)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80114fb8 @ 0x80114fb8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114fb8(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_80115030:
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_801150f8;
      func_0x800453d0(0);
      FUN_80115d94(4);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      goto LAB_80115030;
    }
    if (bVar1 != 2) goto LAB_801150f8;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001a8f8(&DAT_800aca88,0x14);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
LAB_801150f8:
  if (2000 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(1);
  }
  return;
}



/* ============ FUN_80115130 @ 0x80115130 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115130(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x8001a8f8(&DAT_800aca88,0x32);
    func_0x800245d8(0);
    if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
      if (DAT_800aca58 != '\x05') {
        _DAT_800acbfc = _DAT_800ac784;
        _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
        _DAT_800aca5a = 0;
        _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
        DAT_800aca58 = '\x05';
        DAT_800aca59 = 1;
      }
      FUN_80115d74(2);
      func_0x8004ef90(0x800b1028,0x1c);
    }
  }
  return;
}



/* ============ FUN_801152e0 @ 0x801152e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801152e0(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
  }
  return;
}



/* ============ FUN_801153ac @ 0x801153ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801153ac(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(1);
      }
      goto LAB_801154ec;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(0);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xffe2;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x20;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x3c;
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x28;
  }
LAB_801154ec:
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ============ FUN_8011553c @ 0x8011553c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011553c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffff7;
        }
        *(short *)(_DAT_800ac784 + 0x23) =
             (short)_DAT_800ac784[0x23] + *(short *)((int)_DAT_800ac784 + 0x1e6);
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
      }
      goto LAB_801157b4;
    }
    if (bVar1 != 0) goto LAB_801157b4;
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x50;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 200;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 9;
    iVar3 = func_0x8004efe4(0x800b1028,0x1e);
    if (iVar3 != 0) {
      *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    }
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
LAB_801157b4:
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801157e8 @ 0x801157e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801157e8(void)

{
  (**(code **)(&DAT_80121234 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80115d74 @ 0x80115d74 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d74(undefined1 param_1)

{
  *(undefined1 *)(_DAT_800ac784 + 5) = param_1;
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ============ FUN_80115d94 @ 0x80115d94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d94(undefined1 param_1)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = param_1;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  return;
}



/* ============ FUN_80115dc8 @ 0x80115dc8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_80115dc8(void)

{
  undefined4 uVar1;
  
  uVar1 = 1;
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x1518 <= *(int *)(_DAT_800ac784 + 0x38)) {
    if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      uVar1 = 0xffffffff;
    }
    else {
      uVar1 = 0xffffffff;
      if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) == 0) {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}



/* ============ FUN_80115e24 @ 0x80115e24 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e24(void)

{
  byte bVar1;
  short sVar2;
  
  if (*(ushort *)(_DAT_800ac784 + 0x1dc) < 2000) {
    if (*(char *)(_DAT_800ac784 + 0x1d7) != '\0') goto LAB_80115ea4;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) | 1;
    }
    else {
      if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 1) goto LAB_80115ea4;
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) | 2;
    }
  }
  else {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) & 0xfc;
  }
  *(byte *)(_DAT_800ac784 + 0x1d7) = bVar1;
LAB_80115ea4:
  bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + 0x50;
  }
  else {
    if (bVar1 < 2) {
      return;
    }
    if (bVar1 != 2) {
      return;
    }
    sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + -0x50;
  }
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2;
  return;
}



/* ============ FUN_80115f00 @ 0x80115f00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f00(void)

{
  undefined4 uVar1;
  
  if (((*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0x708) &&
      (uVar1 = 7, (int)*(short *)(_DAT_800ac784 + 0x1ba) == (int)*(short *)(_DAT_800ac784 + 0x1ea)))
     || (uVar1 = 5, *(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    FUN_80115d74(uVar1);
  }
  return;
}



/* ============ FUN_80115f70 @ 0x80115f70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f70(void)

{
  int iVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  undefined2 uVar5;
  
  iVar1 = _DAT_800ac784;
  if (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1450) {
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0;
    iVar3 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 4) + 400;
    uVar5 = (undefined2)iVar3;
    if (iVar3 * 0x10000 >> 0x10 < 0x32) {
      uVar5 = 100;
    }
    *(undefined2 *)(iVar1 + 0xbe) = uVar5;
    *(undefined2 *)(iVar1 + 0xbc) = uVar5;
    iVar3 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 5) + 0x80;
    sVar2 = (short)iVar3;
    if (iVar3 * 0x10000 >> 0x10 < 0x20) {
      sVar2 = 0x20;
    }
    uVar4 = (uint)sVar2;
    uVar4 = uVar4 << 0x10 | uVar4 << 8 | uVar4;
    *(uint *)(iVar1 + 0xc4) = uVar4 | *(uint *)(iVar1 + 0xc4) & 0xff000000;
    *(uint *)(iVar1 + 0xec) = uVar4 | *(uint *)(iVar1 + 0xec) & 0xff000000;
  }
  else {
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0xffff;
  }
  return;
}



/* ============ FUN_80116068 @ 0x80116068 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116068(void)

{
  ushort uVar1;
  
  uVar1 = _DAT_800aca50 & 0x4000;
  if (((_DAT_800aca50 & 0x8000) != 0) &&
     (uVar1 = _DAT_800aca50 & 0x4000, *(byte *)(_DAT_800ac784 + 9) < 0x80)) {
    uVar1 = _DAT_800aca50 & 0xfff;
    if ((*(char *)(_DAT_800ac784 + 0x1d8) == '\0') &&
       (uVar1 = _DAT_800aca50, 4 < *(byte *)(_DAT_800ac784 + 5))) {
      FUN_80115d74(0xf);
      uVar1 = _DAT_800aca50;
    }
    _DAT_800aca50 = uVar1;
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    uVar1 = _DAT_800aca50 & 0x4000;
  }
  if (uVar1 != 0) {
    uVar1 = _DAT_800aca50 & 0xfff;
    if ((*(char *)(_DAT_800ac784 + 0x1d8) != '\0') ||
       (uVar1 = _DAT_800aca50, 3 < *(byte *)(_DAT_800ac784 + 5))) {
      _DAT_800aca50 = uVar1;
      FUN_80115d74(0xe);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
  }
  if ((_DAT_800aca50 & 0x2000) != 0) {
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x1000;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    if ((3 < *(byte *)(_DAT_800ac784 + 5)) && (*(char *)(_DAT_800ac784 + 9) != -0x80)) {
      FUN_80115d74(0x11);
    }
  }
  return;
}



/* ============ FUN_801161e8 @ 0x801161e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161e8(void)

{
  short sVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  sVar1 = *(short *)(_DAT_800ac784 + 0x1ec);
  if (sVar1 < 0x5dc) {
    func_0x80037edc(1,10);
    uVar2 = 2;
    uVar3 = 10;
  }
  else {
    if (sVar1 < 3000) {
      func_0x80037edc(3,10);
      func_0x80037edc(4,0x28);
      func_0x80037edc(6,0x28);
      uVar2 = 5;
    }
    else {
      if (0xe0f < sVar1) {
        return;
      }
      func_0x80037edc(0,8);
      uVar2 = 7;
    }
    uVar3 = 0x28;
  }
  func_0x80037edc(uVar2,uVar3);
  return;
}



/* ============ FUN_80116288 @ 0x80116288 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116288(void)

{
  short sVar1;
  char cVar2;
  int iVar3;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) != 0)) {
    (*(code *)(&PTR_FUN_80121268)[(byte)_DAT_800ac784[1]])();
    func_0x8002b498(_DAT_800ac784);
    if ((byte)_DAT_800ac784[0x74] < 0xd) {
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      iVar3 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
      if (iVar3 != 0) {
        if (DAT_800acae7 == '\0') {
          DAT_800aca58 = 2;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          _DAT_800aca5a = 0;
        }
        if (3 < _DAT_800acaee) {
          _DAT_800acaee = _DAT_800acaee + -2;
        }
      }
    }
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x76];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0x1da);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
      sVar1 = *(short *)((int)_DAT_800ac784 + 0x1d6);
      iVar3 = ((byte)_DAT_800ac784[0x74] - 1) * 0x14;
    }
    else {
      sVar1 = *(short *)((int)_DAT_800ac784 + 0x1d6);
      iVar3 = ((byte)_DAT_800ac784[0x74] - 1) * 0x28;
    }
    _DAT_800ac784[0xe] = sVar1 - iVar3;
  }
  return;
}



/* ============ FUN_801164b0 @ 0x801164b0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801164b0(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 100;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80121264;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,600,_DAT_800ac784 + 0x2c,0xa0a0a0);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    _DAT_800ac784[uVar1 + 0x74] = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  *(uint *)_DAT_800ac784[0x62] = *(uint *)_DAT_800ac784[0x62] & 0xfffffffe;
  *(short *)(_DAT_800ac784 + 0x76) = (short)_DAT_800ac784[0xd];
  *(short *)((int)_DAT_800ac784 + 0x1da) = (short)_DAT_800ac784[0xf];
  *(short *)((int)_DAT_800ac784 + 0x1d6) = (short)_DAT_800ac784[0xe];
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  _DAT_800ac784[1] = 1;
  *(byte *)((int)_DAT_800ac784 + 5) = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    func_0x80019700(0x9031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &DAT_80121248);
  }
  return;
}



/* ============ FUN_801166fc @ 0x801166fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166fc(void)

{
  (*(code *)(&PTR_FUN_8012127c)[*(byte *)(_DAT_800ac784 + 5)])();
  func_0x8001b064(_DAT_800ac784 + 0xb0,0xfffff8f8);
  return;
}



/* ============ FUN_80116758 @ 0x80116758 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116758(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x1d4);
    *(short *)(_DAT_800ac784 + 0x1d4) = sVar2 + -1;
    if (sVar2 != 0) {
      return;
    }
    if (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      return;
    }
    if (*(byte *)(_DAT_800ac784 + 0x1d0) < 8) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      return;
    }
    FUN_80116bec();
    *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (bVar1 < 2) {
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0x28;
    FUN_80116d00();
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    if (bVar1 != 2) {
      return;
    }
    FUN_80116c68();
    cVar3 = '\x01';
  }
  *(char *)(_DAT_800ac784 + 6) = cVar3;
  return;
}



/* ============ FUN_80116870 @ 0x80116870 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116870(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x1d4);
    *(short *)(_DAT_800ac784 + 0x1d4) = sVar2 + -1;
    if (sVar2 != 0) {
      return;
    }
    if (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      return;
    }
    if (7 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      FUN_80116bec();
    }
    *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (bVar1 < 2) {
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0x2c;
    FUN_80116d00();
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    if (bVar1 != 2) {
      return;
    }
    FUN_80116c68();
    cVar3 = '\x01';
  }
  *(char *)(_DAT_800ac784 + 6) = cVar3;
  return;
}



/* ============ FUN_8011697c @ 0x8011697c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011697c(void)

{
  (*(code *)(&PTR_FUN_80121290)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80116a04 @ 0x80116a04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116a04(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
    FUN_80116c68();
  }
  else {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x10001;
      *(byte *)(_DAT_800ac784 + 5) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 3;
    if (((*(byte *)(_DAT_800ac784 + 0x1d0) < 8) ||
        (FUN_80116b70(), *(byte *)(_DAT_800ac784 + 0x1d0) < 8)) &&
       (bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x7f, bVar1 < 3)) {
      func_0x8004ef90(0x800b1028,bVar1 + 0x1d);
    }
    if (1 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + -1;
    }
  }
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
  return;
}



/* ============ FUN_80116b70 @ 0x80116b70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116b70(void)

{
  undefined4 uVar1;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar1 = 8;
    break;
  case 1:
  case 2:
  case 4:
    uVar1 = 0x10;
    break;
  default:
    goto switchD_80116bac_default;
  }
  func_0x80019d50(uVar1,3,0x23,*(int *)(_DAT_800ac784 + 0x188) + 0x40);
switchD_80116bac_default:
  return;
}



/* ============ FUN_80116bec @ 0x80116bec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116bec(void)

{
  undefined4 uVar1;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar1 = 8;
    break;
  case 1:
  case 2:
  case 4:
    uVar1 = 0x10;
    break;
  default:
    goto switchD_80116c28_default;
  }
  func_0x80019d50(uVar1,3,0x22,*(int *)(_DAT_800ac784 + 0x188) + 0x40);
switchD_80116c28_default:
  return;
}



/* ============ FUN_80116c68 @ 0x80116c68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116c68(void)

{
  ushort uVar1;
  undefined4 uVar2;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar2 = 8;
    break;
  case 1:
  case 2:
  case 4:
    uVar2 = 0x10;
    break;
  default:
    goto switchD_80116ca4_default;
  }
  func_0x80019d50(uVar2,3,0x12,*(int *)(_DAT_800ac784 + 0x188) + 0x40);
switchD_80116ca4_default:
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1d4) = (uVar1 & 0x3f) + 0x10;
  return;
}



/* ============ FUN_80116d00 @ 0x80116d00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d00(void)

{
  ushort uVar1;
  uint uVar2;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar2 = 0x8030000;
    break;
  case 1:
  case 2:
  case 4:
    uVar2 = 0x10030000;
    break;
  default:
    goto switchD_80116d3c_default;
  }
  func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d0) << 8 | uVar2,
                  (int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x40,
                  &DAT_80121248);
switchD_80116d3c_default:
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1d4) = (uVar1 & 0x3f) + 0x10;
  return;
}



/* ============ FUN_80116db8 @ 0x80116db8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116db8(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_801213c8)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80116f50 @ 0x80116f50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f50(void)

{
  uint uVar1;
  
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
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 4;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011f034 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_80121368)[*(byte *)((int)_DAT_800ac784 + 0x1e2)];
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



/* ============ FUN_80117254 @ 0x80117254 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117254(void)

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
  (*(code *)(&PTR_LAB_801213e8)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80121428)[*(byte *)((int)_DAT_800ac784 + 5)])();
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
  if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  }
  else {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
  }
  return;
}



/* ============ FUN_80117574 @ 0x80117574 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117574(void)

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



/* ============ FUN_80117764 @ 0x80117764 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117764(void)

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



/* ============ FUN_80117860 @ 0x80117860 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117860(void)

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
    func_0x800453d0(4);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_8011bf50(0,0);
  return;
}



/* ============ FUN_80117a3c @ 0x80117a3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117a3c(void)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  
  if (((DAT_800acae7 == '\0') && (iVar2 = func_0x8001a804(3000,0x180,&DAT_800aca88), iVar2 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    uVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar1;
    if ((*(short *)(_DAT_800ac784 + 0x1da) != 0) && (*(short *)(_DAT_800ac784 + 0x1da) < 1)) {
      return;
    }
    uVar3 = func_0x8001a804(0x9c4,0x100,&DAT_800aca88);
    if ((uVar3 & 0x80000000) == 0) {
      return;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
      return;
    }
    if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    return;
  }
  if (DAT_800acae7 != '\0') {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      goto LAB_80117c68;
    }
    if (DAT_800acae7 != '\0') {
      return;
    }
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
      ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  if (DAT_800acae7 != '\0') {
    return;
  }
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80117c68:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_80117c90 @ 0x80117c90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117c90(void)

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
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x18);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = '\x18';
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x29);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = ')';
  }
  if ((cVar1 == '\x01') || (cVar1 == cVar3)) {
    func_0x800453d0(8);
  }
  return;
}



/* ============ FUN_80118110 @ 0x80118110 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118110(void)

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



/* ============ FUN_80118270 @ 0x80118270 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118270(void)

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
  FUN_8011bf50(0,0);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x8012146c)) {
        bVar3 = true;
      }
      bVar1 = uVar6 != 0;
      uVar6 = uVar6 - 1;
    } while (bVar1);
    if ((bVar3) && (*(short *)(_DAT_800ac784 + 0x1da) != 0)) {
      _DAT_800acaee = _DAT_800acaee + -6;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(6);
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



/* ============ FUN_8011854c @ 0x8011854c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011854c(void)

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
      else if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        uVar5 = 4;
      }
      else {
        uVar5 = 3;
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
    FUN_8011c024(0,0);
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar8 + 0x80121470)) {
        bVar3 = true;
      }
      bVar1 = uVar8 != 0;
      uVar8 = uVar8 - 1;
    } while (bVar1);
    if ((bVar3) &&
       ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
      _DAT_800acaee = _DAT_800acaee + -0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
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



/* ============ FUN_80118908 @ 0x80118908 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118908(void)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
          if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) &&
             (*(char *)(_DAT_800ac784 + 7) == '\0')) {
            *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 2;
            *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            *(undefined1 *)(_DAT_800ac784 + 6) = 3;
            goto LAB_80118dac;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
          local_28 = DAT_801213a8;
          local_24 = DAT_801213ac;
          local_20 = DAT_801213b0;
          local_1c = DAT_801213b4;
          func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                          *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
        if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
        }
        iVar4 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                                (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar4 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(2);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
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
        iVar4 = func_0x8001a804(5000,0xc0,&DAT_800aca88);
        if (-1 < iVar4) {
          return;
        }
        if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        func_0x800453d0(7);
        return;
      }
      if (bVar1 != 3) {
        return;
      }
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar4 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80118dac:
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_80118dbc;
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
  if (*(char *)(_DAT_800ac784 + 7) != '\0') {
    sVar3 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x20);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar3;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 200;
    *(ushort *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) + (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 10;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32a;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
      *(undefined1 *)(_DAT_800ac784 + 0x82) = 1;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
LAB_80118dbc:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80118ddc @ 0x80118ddc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118ddc(void)

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
  if (bVar2 == 1) {
LAB_80118ea4:
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0'))
      {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        goto LAB_80119248;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
      local_48 = DAT_801213a8;
      local_44 = DAT_801213ac;
      local_40 = DAT_801213b0;
      local_3c = DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_48);
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
          if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x80121478)) {
            bVar3 = true;
          }
          bVar1 = uVar6 != 0;
          uVar6 = uVar6 - 1;
        } while (bVar1);
        if ((bVar3) &&
           ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
          _DAT_800acaee = _DAT_800acaee + -600;
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 300;
          func_0x800453d0(1);
          _DAT_800aca58 = 6;
          _DAT_800acbfc = _DAT_800ac784;
          _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
          _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
          DAT_800acae7 = DAT_800acae7 | 1;
        }
      }
    }
    else {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
      func_0x800453d0(2);
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    }
  }
  else {
    if (bVar2 < 2) {
      if (bVar2 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      goto LAB_80118ea4;
    }
    if (bVar2 != 2) {
      return;
    }
    iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar5 == 0) {
      func_0x800245d8(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80119248:
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ============ FUN_80119284 @ 0x80119284 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119284(void)

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
        local_28 = DAT_801213a8;
        local_24 = DAT_801213ac;
        local_20 = DAT_801213b0;
        local_1c = DAT_801213b4;
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



/* ============ FUN_8011936c @ 0x8011936c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011936c(void)

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
      *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8012136c)[*(byte *)(_DAT_800ac784 + 0x1e2)];
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



/* ============ FUN_80119634 @ 0x80119634 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119634(void)

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
        local_28 = DAT_801213a8;
        local_24 = DAT_801213ac;
        local_20 = DAT_801213b0;
        local_1c = DAT_801213b4;
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



/* ============ FUN_8011971c @ 0x8011971c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011971c(void)

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
LAB_80119870:
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 2:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x18') {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x17;
    }
    iVar1 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffb0,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar1 == 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    goto LAB_80119870;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80121368)[*(byte *)(_DAT_800ac784 + 0x1e2)];
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



/* ============ FUN_80119998 @ 0x80119998 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119998(void)

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
      local_28 = DAT_801213a8;
      local_24 = DAT_801213ac;
      local_20 = DAT_801213b0;
      local_1c = DAT_801213b4;
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



/* ============ FUN_80119a6c @ 0x80119a6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119a6c(void)

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



/* ============ FUN_80119c38 @ 0x80119c38 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119c38(void)

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
      local_28 = DAT_801213a8;
      local_24 = DAT_801213ac;
      local_20 = DAT_801213b0;
      local_1c = DAT_801213b4;
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



/* ============ FUN_80119d0c @ 0x80119d0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119d0c(void)

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
    FUN_8011c024(0,0);
    if (*(char *)(_DAT_800ac784 + 0x95) == '2') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
LAB_80119f40:
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
LAB_80119f1c:
    *(char *)(_DAT_800ac784 + 6) = cVar1 + cVar2;
    FUN_8011c024(0,uVar3);
    break;
  case 4:
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    goto LAB_80119f40;
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
    goto LAB_80119f1c;
  default:
    break;
  }
  return;
}



/* ============ FUN_80119f70 @ 0x80119f70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119f70(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  byte bVar5;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  bVar2 = false;
  bVar3 = false;
  bVar5 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar5 & 2) != 0) {
    if ((bVar5 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      goto LAB_8011a1c8;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = DAT_801213a8;
    local_34 = DAT_801213ac;
    local_30 = DAT_801213b0;
    local_2c = DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8012147d)[bVar5]) {
      bVar2 = true;
    }
    bVar1 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar1);
  if ((((bVar2) && (DAT_800acae7 == '\0')) &&
      (iVar4 = func_0x8001a804(5000,0x180,&DAT_800aca88), iVar4 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  else {
    bVar5 = 2;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x8012147c)) {
        bVar3 = true;
      }
      bVar2 = bVar5 != 0;
      bVar5 = bVar5 - 1;
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
  }
LAB_8011a1c8:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_8011a1f8 @ 0x8011a1f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a1f8(void)

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
  FUN_8011c024(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x19);
  return;
}



/* ============ FUN_8011a378 @ 0x8011a378 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a378(void)

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
      local_28 = DAT_801213a8;
      local_24 = DAT_801213ac;
      local_20 = DAT_801213b0;
      local_1c = DAT_801213b4;
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



/* ============ FUN_8011a44c @ 0x8011a44c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a44c(void)

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
  FUN_8011c024(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x28);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar4 + 0x80121480)) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar8 + 0x80121488)) {
        uVar7 = func_0x8001bff8(iVar9 + 0x6f8,&local_28,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar7;
        if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
          bVar3 = true;
        }
      }
    }
    if ((bVar2) || (bVar3)) {
      _DAT_800acaee = _DAT_800acaee + -0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(6);
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



/* ============ FUN_8011a878 @ 0x8011a878 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a878(void)

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
      local_28 = DAT_801213a8;
      local_24 = DAT_801213ac;
      local_20 = DAT_801213b0;
      local_1c = DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
    else {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8011a960 @ 0x8011a960 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a960(void)

{
  char cVar1;
  undefined2 uVar2;
  ushort extraout_var;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) + 0x640;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x800453d0(7);
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
  case 1:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if ((*(char *)((int)_DAT_800ac784 + 0x95) == '\x04') &&
       (iVar3 = func_0x8001a780(&DAT_800aca54), iVar3 == 0)) {
      local_20 = _DAT_80072d60;
      local_1c = _DAT_80072d64;
      local_18 = _DAT_80072d68;
      local_14 = _DAT_80072d6c;
      func_0x8001a804(3000,0x100,&DAT_800aca88);
      *(ushort *)(_DAT_800ac784 + 0x76) = extraout_var >> 0xf;
      if ((DAT_800acae7 != 0) || ((short)_DAT_800ac784[0x76] == 0)) goto LAB_8011aaf0;
LAB_8011abac:
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    }
    else {
LAB_8011aaf0:
      if (*(char *)((int)_DAT_800ac784 + 0x95) != '\x0f') {
        return;
      }
      iVar3 = func_0x8001a780(&DAT_800aca54);
      if (iVar3 != 0) {
        local_20 = _DAT_80072d60;
        local_1c = _DAT_80072d64;
        local_18 = _DAT_80072d68;
        local_14 = _DAT_80072d6c;
        uVar2 = func_0x8001bff8(_DAT_800ac784[0x62] + 0x39c,&local_20,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar2;
        if ((DAT_800acae7 == 0) && ((short)_DAT_800ac784[0x76] != 0)) goto LAB_8011abac;
      }
      if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x0f') {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
        *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x23;
      }
    }
    break;
  case 2:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    func_0x800453d0(10);
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    DAT_800aca59 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca5a = 0;
    DAT_800aca5b = 0;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 4:
    *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x16;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    func_0x800453d0(4);
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  case 5:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '1') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    }
    break;
  case 6:
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0xff;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    break;
  case 7:
    iVar3 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],1,0x200);
    if (iVar3 != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
      _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 6;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x1e;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0x5a;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ============ FUN_8011af5c @ 0x8011af5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011af5c(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80121368)[*(byte *)(_DAT_800ac784 + 0x1e2)];
    func_0x800453d0(2);
  }
  (*(code *)(&PTR_FUN_801214a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011b018 @ 0x8011b018 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b018(void)

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
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar3);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar3);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_8011b1ec @ 0x8011b1ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b1ec(void)

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
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar3);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar3);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_8011bf50(0,0);
  }
  else {
    FUN_8011bf50(0,1);
  }
  return;
}



/* ============ FUN_8011b400 @ 0x8011b400 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b400(void)

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
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar3);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar3);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(3);
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
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ============ FUN_8011b6fc @ 0x8011b6fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b6fc(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80121368)[*(byte *)(_DAT_800ac784 + 0x1e2)];
    func_0x800453d0(2);
  }
  (*(code *)(&PTR_FUN_80121500)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011b7b8 @ 0x8011b7b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b7b8(void)

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
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar3);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar3);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_8011bf50(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) == '=') {
    func_0x800453d0(2);
  }
  return;
}



/* ============ FUN_8011b998 @ 0x8011b998 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b998(void)

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
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar3);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar3);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x13') {
    func_0x800453d0(2);
  }
  return;
}



/* ============ FUN_8011bb9c @ 0x8011bb9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bb9c(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
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
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x1f) + 0x50;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_80121388 + iVar4);
    local_1c = *(undefined4 *)(&DAT_8012138c + iVar4);
    local_18 = *(undefined4 *)(&DAT_80121390 + iVar4);
    local_14 = *(undefined4 *)(&DAT_80121394 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
    func_0x800245d8(0);
  }
  return;
}



/* ============ FUN_8011bdec @ 0x8011bdec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bdec(void)

{
  (*(code *)(&PTR_LAB_80121558)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011bf50 @ 0x8011bf50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bf50(undefined4 param_1,int param_2)

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



/* ============ FUN_8011c024 @ 0x8011c024 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c024(undefined4 param_1,int param_2)

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



/* ============ FUN_8011c414 @ 0x8011c414 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c414(void)

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
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_80121570
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_80121570
                   );
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + cVar1,_DAT_800aca58);
  return;
}



/* ============ FUN_8011c5a0 @ 0x8011c5a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c5a0(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_80121598)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011c6dc @ 0x8011c6dc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c6dc(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80121594;
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



/* ============ FUN_8011c884 @ 0x8011c884 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c884(void)

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
  (*(code *)(&PTR_FUN_801215ac)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011c950 @ 0x8011c950 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c950(void)

{
  (**(code **)(&DAT_801215b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801215d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011c9cc @ 0x8011c9cc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c9cc(void)

{
  (**(code **)(&DAT_801215f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121610 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011ca48 @ 0x8011ca48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ca48(void)

{
  (**(code **)(&DAT_80121630 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011ca90 @ 0x8011ca90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ca90(void)

{
  (**(code **)(&DAT_80121630 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011cad8 @ 0x8011cad8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cad8(void)

{
  (**(code **)(&DAT_80121630 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011cb70 @ 0x8011cb70 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cb70(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_80121668)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011ccac @ 0x8011ccac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ccac(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80121664;
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



/* ============ FUN_8011ce54 @ 0x8011ce54 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce54(void)

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
  (*(code *)(&PTR_FUN_8012167c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011cf20 @ 0x8011cf20 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cf20(void)

{
  (**(code **)(&DAT_80121680 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801216a0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011cf9c @ 0x8011cf9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cf9c(void)

{
  (**(code **)(&DAT_801216c0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801216e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011d018 @ 0x8011d018 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d018(void)

{
  (**(code **)(&DAT_80121700 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011d060 @ 0x8011d060 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d060(void)

{
  (**(code **)(&DAT_80121700 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011d0a8 @ 0x8011d0a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d0a8(void)

{
  (**(code **)(&DAT_80121700 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011d140 @ 0x8011d140 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d140(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar1 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10
    ;
    iVar2 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10
    ;
    uVar3 = func_0x80065f60(iVar1 * iVar1 + iVar2 * iVar2);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar3;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(&DAT_800aca88,0,0);
    (*(code *)(&PTR_FUN_80121738)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011d2b8 @ 0x8011d2b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d2b8(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80121734;
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



/* ============ FUN_8011d460 @ 0x8011d460 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d460(void)

{
  (*(code *)(&PTR_FUN_8012174c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  func_0x80012aa4(3000);
  return;
}



/* ============ FUN_8011d4b4 @ 0x8011d4b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d4b4(void)

{
  (**(code **)(&DAT_80121750 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8012175c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011d530 @ 0x8011d530 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d530(void)

{
  (**(code **)(&DAT_80121768 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121768 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011d5ac @ 0x8011d5ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d5ac(void)

{
  (**(code **)(&DAT_80121768 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011d5f4 @ 0x8011d5f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d5f4(void)

{
  (**(code **)(&DAT_80121768 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011d63c @ 0x8011d63c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d63c(void)

{
  (**(code **)(&DAT_80121768 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011d6d4 @ 0x8011d6d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d6d4(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar1 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10
    ;
    iVar2 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10
    ;
    uVar3 = func_0x80065f60(iVar1 * iVar1 + iVar2 * iVar2);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar3;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(&DAT_800aca88,0,0);
    (*(code *)(&PTR_FUN_801217a0)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011d84c @ 0x8011d84c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d84c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8012179c;
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



/* ============ FUN_8011d9f4 @ 0x8011d9f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d9f4(void)

{
  (*(code *)(&PTR_FUN_801217b4)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  func_0x80012aa4(3000);
  return;
}



/* ============ FUN_8011da48 @ 0x8011da48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011da48(void)

{
  (**(code **)(&DAT_801217b8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801217c4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011dac4 @ 0x8011dac4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dac4(void)

{
  (**(code **)(&DAT_801217d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801217d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011db40 @ 0x8011db40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011db40(void)

{
  (**(code **)(&DAT_801217d0 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011db88 @ 0x8011db88 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011db88(void)

{
  (**(code **)(&DAT_801217d0 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011dbd0 @ 0x8011dbd0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dbd0(void)

{
  (**(code **)(&DAT_801217d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011dc68 @ 0x8011dc68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dc68(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_80121808)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011dd98 @ 0x8011dd98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dd98(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80121804;
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



/* ============ FUN_8011df40 @ 0x8011df40 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011df40(void)

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
  (*(code *)(&PTR_FUN_8012181c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011e00c @ 0x8011e00c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e00c(void)

{
  (**(code **)(&DAT_80121820 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121840 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011e088 @ 0x8011e088 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e088(void)

{
  (**(code **)(&DAT_80121860 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121880 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011e104 @ 0x8011e104 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e104(void)

{
  (**(code **)(&DAT_801218a0 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011e14c @ 0x8011e14c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e14c(void)

{
  (**(code **)(&DAT_801218a0 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011e194 @ 0x8011e194 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e194(void)

{
  (**(code **)(&DAT_801218a0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011e22c @ 0x8011e22c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e22c(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_801218d8)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011e370 @ 0x8011e370 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e370(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_801218d4;
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



/* ============ FUN_8011e518 @ 0x8011e518 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e518(void)

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
    (*(code *)(&PTR_FUN_801218ec)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8011e644 @ 0x8011e644 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e644(void)

{
  (**(code **)(&DAT_801218f4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121914 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011e6c0 @ 0x8011e6c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e6c0(void)

{
  (**(code **)(&DAT_80121934 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121954 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011e73c @ 0x8011e73c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e73c(void)

{
  (**(code **)(&DAT_80121974 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011e784 @ 0x8011e784 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e784(void)

{
  (**(code **)(&DAT_80121988 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011e7cc @ 0x8011e7cc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e7cc(void)

{
  (**(code **)(&DAT_80121988 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011e864 @ 0x8011e864 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e864(void)

{
  _DAT_80072bf8 = FUN_8010a8c8;
  _DAT_80072bec = &LAB_80100424;
  _DAT_80072bf0 = &LAB_80100424;
  _DAT_80072bf4 = &LAB_80100424;
  _DAT_80072c04 = &LAB_80100424;
  _DAT_80072c0c = &LAB_80100424;
  _DAT_80072c1c = &LAB_80100424;
  _DAT_80072c20 = &LAB_80100424;
  _DAT_80072c24 = &LAB_80100424;
  _DAT_80072c28 = &LAB_80100424;
  _DAT_80072c2c = FUN_8010d7f8;
  _DAT_80072c30 = FUN_80112020;
  _DAT_80072c44 = FUN_80116288;
  _DAT_80072c48 = FUN_80116db8;
  _DAT_80072cac = FUN_8011c5a0;
  _DAT_80072cb4 = FUN_8011cb70;
  _DAT_80072cc0 = FUN_8011d140;
  _DAT_80072cc8 = FUN_8011d6d4;
  _DAT_80072cd0 = FUN_8011dc68;
  _DAT_80072cd8 = FUN_8011e22c;
  _DAT_800b52c8 = &DAT_8011eae4;
  _DAT_800ac998 = &DAT_8011efc4;
  _DAT_800ac99c = &DAT_8011f734;
  _DAT_80072c14 = FUN_8010c1ec;
  _DAT_800ac798 = &LAB_8010a28c;
  _DAT_800ac79c = &LAB_8010a28c;
  _DAT_800ac7a0 = &LAB_8010a28c;
  _DAT_800ac7a4 = &LAB_8010c16c;
  _DAT_800ac7b0 = &LAB_8010a28c;
  _DAT_800ac7b8 = &LAB_8010a28c;
  _DAT_800ac7c8 = &LAB_8010a28c;
  _DAT_800ac7cc = &LAB_8010a28c;
  _DAT_800ac7d0 = &LAB_8010a28c;
  _DAT_800ac7d4 = &LAB_8010a28c;
  _DAT_800ac7d8 = &LAB_80111944;
  _DAT_800ac7dc = &LAB_8011597c;
  _DAT_800ac7f4 = &LAB_8011c118;
  _DAT_800ac898 = &LAB_8010a6b8;
  _DAT_800ac89c = &LAB_8010a6b8;
  _DAT_800ac8a0 = &LAB_8010a6b8;
  _DAT_800ac8b0 = &LAB_8010a6b8;
  _DAT_800ac8b8 = &LAB_8010a6b8;
  _DAT_800ac8c8 = &LAB_8010a6b8;
  _DAT_800ac8cc = &LAB_8010a6b8;
  _DAT_800ac8d0 = &LAB_8010a6b8;
  _DAT_800ac8d4 = &LAB_8010a6b8;
  _DAT_800ac8d8 = &LAB_80111cb0;
  _DAT_800ac8dc = &LAB_80115d2c;
  _DAT_800ac8a4 = &LAB_8010c1ac;
  _DAT_800ac8f4 = &LAB_8011c3d4;
  func_0x80029afc();
  return;
}



