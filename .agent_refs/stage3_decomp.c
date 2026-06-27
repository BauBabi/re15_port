/* ============ FUN_80100774 @ 0x80100774 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100774(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011d8e0;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_8011d934)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011d1c8 + (uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&DAT_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (*(code *)(&PTR_FUN_8011d93c)[uVar3 & 7])();
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
                    (uint)(byte)(&DAT_8011d8f4)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f5)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f6)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f7)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f8)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f9)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fa)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fb)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
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



/* ============ FUN_801010f4 @ 0x801010f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010f4(void)

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



/* ============ FUN_80101144 @ 0x80101144 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101144(void)

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



/* ============ FUN_80101228 @ 0x80101228 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101228(void)

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



/* ============ FUN_80101310 @ 0x80101310 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101310(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f5)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f6)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f7)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fb)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_8011d95c)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80101778 @ 0x80101778 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101778(void)

{
  (*(code *)(&PTR_FUN_8011d990)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011d9e0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801017f4 @ 0x801017f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801017f4(void)

{
  (*(code *)(&PTR_FUN_8011da30)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011da70)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101870 @ 0x80101870 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101870(void)

{
  (*(code *)(&PTR_FUN_8011dab0)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011d9e0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801018ec @ 0x801018ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801018ec(void)

{
  (*(code *)(&PTR_FUN_8011db00)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011da70)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101968 @ 0x80101968 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101968(void)

{
  (*(code *)(&PTR_FUN_8011db18)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db1c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_801019e4 @ 0x801019e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019e4(void)

{
  (*(code *)(&PTR_FUN_8011db20)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db24)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101a60 @ 0x80101a60 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101a60(void)

{
  (*(code *)(&PTR_LAB_8011db28)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db24)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101adc @ 0x80101adc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101adc(void)

{
  (*(code *)(&PTR_FUN_8011db2c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db24)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101b58 @ 0x80101b58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b58(void)

{
  (*(code *)(&PTR_LAB_8011db30)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db34)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101bd4 @ 0x80101bd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101bd4(void)

{
  (*(code *)(&PTR_LAB_8011db38)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011db3c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80101c50 @ 0x80101c50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c50(void)

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



/* ============ FUN_80101d68 @ 0x80101d68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101d68(void)

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



/* ============ FUN_80101df4 @ 0x80101df4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101df4(void)

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
    FUN_80109488(0,1);
  }
  return;
}



/* ============ FUN_80101ed0 @ 0x80101ed0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ed0(void)

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



/* ============ FUN_80101fdc @ 0x80101fdc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101fdc(void)

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
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80102144 @ 0x80102144 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102144(void)

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



/* ============ FUN_801022e4 @ 0x801022e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801022e4(void)

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
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011dc50)[uVar5 & 0x1f];
    }
    else {
      uVar5 = func_0x8001af20();
      *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011dc40)[uVar5 & 0x1f];
    }
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_8011db40 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011db42 +
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
          (&DAT_8011db40 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80102634 @ 0x80102634 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102634(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_8011d924);
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



/* ============ FUN_80102cc4 @ 0x80102cc4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102cc4(void)

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



/* ============ FUN_80102e0c @ 0x80102e0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102e0c(void)

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



/* ============ FUN_80102eb4 @ 0x80102eb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102eb4(void)

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



/* ============ FUN_80103008 @ 0x80103008 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103008(void)

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



/* ============ FUN_80103100 @ 0x80103100 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103100(void)

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
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801032d0 @ 0x801032d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801032d0(void)

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
    goto switchD_80103308_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_80103308_default:
  return;
}



/* ============ FUN_801034b4 @ 0x801034b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801034b4(void)

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



/* ============ FUN_801036e4 @ 0x801036e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801036e4(void)

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



/* ============ FUN_801037c8 @ 0x801037c8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801037c8(void)

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
    FUN_8010955c(0,uVar1);
  }
  return;
}



/* ============ FUN_80103948 @ 0x80103948 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103948(void)

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



/* ============ FUN_80103a6c @ 0x80103a6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a6c(void)

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



/* ============ FUN_80103ae8 @ 0x80103ae8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103ae8(void)

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



/* ============ FUN_80103b44 @ 0x80103b44 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103b44(void)

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



/* ============ FUN_80103c80 @ 0x80103c80 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103c80(void)

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
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_8011d924);
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
        puVar5 = (uint *)((uint)(byte)(&DAT_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac +
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



/* ============ FUN_80104264 @ 0x80104264 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104264(void)

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
    puVar3 = (uint *)((uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ============ FUN_80104634 @ 0x80104634 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104634(void)

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



/* ============ FUN_80104758 @ 0x80104758 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104758(void)

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



/* ============ FUN_8010484c @ 0x8010484c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010484c(void)

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



/* ============ FUN_801048f4 @ 0x801048f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801048f4(void)

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



/* ============ FUN_801049d4 @ 0x801049d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801049d4(void)

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



/* ============ FUN_80104b3c @ 0x80104b3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b3c(void)

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



/* ============ FUN_80104c2c @ 0x80104c2c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104c2c(void)

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
    goto LAB_80104fc8;
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
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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
LAB_80104fc8:
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



/* ============ FUN_8010506c @ 0x8010506c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010506c(void)

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



/* ============ FUN_80105218 @ 0x80105218 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105218(void)

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
    FUN_80109488(0,0);
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    uVar4 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) =
         (short)*(undefined4 *)(&DAT_8011dc60 + (uVar4 & 0xf) * 4) * 0x1e;
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



/* ============ FUN_801055e0 @ 0x801055e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801055e0(void)

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



/* ============ FUN_80105708 @ 0x80105708 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105708(void)

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



/* ============ FUN_801058a8 @ 0x801058a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801058a8(void)

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
         *(undefined2 *)(&DAT_8011dca0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011dca2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&DAT_8011dca0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80105b78 @ 0x80105b78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105b78(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee2320))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  else if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_80106b24();
  }
  else {
    FUN_8010698c();
  }
  return;
}



/* ============ FUN_80105c68 @ 0x80105c68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105c68(void)

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
      else if (bVar2 != 3) goto LAB_80106118;
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
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011dc50)[uVar4 & 0x1f];
        }
        else {
          uVar4 = func_0x8001af20();
          *(undefined *)(_DAT_800ac784 + 0x1de) = (&DAT_8011dc40)[uVar4 & 0x1f];
        }
        if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) =
             *(undefined2 *)
              (&DAT_8011db40 +
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
      goto LAB_80106118;
    }
    if (bVar2 != 0) goto LAB_80106118;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
              *(int *)(_DAT_800ac784 + 0x188);
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0x40,&LAB_8011dfd4);
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    (*(code *)(&PTR_LAB_8011df80)[*(byte *)(_DAT_800ac784 + 5)])();
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
LAB_80106118:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80106134 @ 0x80106134 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106134(void)

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
      goto LAB_80106358;
    }
    if (bVar1 != 0) goto LAB_80106358;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar3 = func_0x8001af20();
    *(undefined *)(_DAT_800ac784 + 0x94) = (&DAT_8011dfe4)[uVar3 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011dfec);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
    (*(code *)(&PTR_LAB_8011df80)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_80106358:
  FUN_80109488(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_8010637c @ 0x8010637c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010637c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_801064f8:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_801066f4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_801066f4;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (*(code *)(&PTR_LAB_8011df80)[*(byte *)(_DAT_800ac784 + 5)])();
      goto LAB_801064f8;
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
      goto LAB_801066f4;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_801066f4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
  }
  if (cVar2 == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
  }
LAB_801066f4:
  FUN_80109488(0,0);
  return;
}



/* ============ FUN_80106710 @ 0x80106710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106710(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
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



/* ============ FUN_8010698c @ 0x8010698c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010698c(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011dfec);
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



/* ============ FUN_80106b24 @ 0x80106b24 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106b24(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011dfec);
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



/* ============ FUN_80106c90 @ 0x80106c90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106c90(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee2004))();
  }
  else {
    FUN_80107d9c();
  }
  return;
}



/* ============ FUN_80106d04 @ 0x80106d04 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106d04(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  undefined4 uVar4;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80106f8c;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011e2bc);
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
LAB_80106f8c:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109488(0,0);
  }
  return;
}



/* ============ FUN_80106fc8 @ 0x80106fc8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106fc8(void)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_801072ec;
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
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    puVar7 = (uint *)((uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8011e2bc);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8011e2bc);
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
LAB_801072ec:
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109488(0,0);
  }
  return;
}



/* ============ FUN_80107330 @ 0x80107330 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107330(void)

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
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    puVar7 = (uint *)((uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8011e2bc);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_8011e2bc);
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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011e2cc);
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
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_80107720 @ 0x80107720 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107720(void)

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
switchD_8010775c_caseD_3:
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
                      &DAT_8011e2cc);
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
      FUN_80109488(0,0);
    }
    else {
      FUN_80109488(0,1);
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
    FUN_80109488(0,1);
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
    goto switchD_8010775c_caseD_3;
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



/* ============ FUN_80107d9c @ 0x80107d9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80107d9c(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
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
                    (uint)(byte)(&DAT_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)uVar3;
  return uVar3;
}



/* ============ FUN_80107fcc @ 0x80107fcc ============ */

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



/* ============ FUN_80108ba8 @ 0x80108ba8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108ba8(void)

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
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
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



/* ============ FUN_80108df0 @ 0x80108df0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108df0(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&LAB_8011e2bc);
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



/* ============ FUN_80108e60 @ 0x80108e60 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108e60(void)

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
      goto LAB_80109250;
    }
    if (bVar1 != 0) goto LAB_80109250;
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f4)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f5)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f6)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f7)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f8)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f9)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fa)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fb)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
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
LAB_80109250:
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109488(0,0);
  }
  return;
}



/* ============ FUN_80109288 @ 0x80109288 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109288(void)

{
  (**(code **)(&DAT_8011e2dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801092d0 @ 0x801092d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801092d0(void)

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
  FUN_80109488(0,(**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0);
  return;
}



/* ============ FUN_801093e0 @ 0x801093e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801093e0(void)

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
    FUN_80109488(0,1);
  }
  return;
}



/* ============ FUN_80109488 @ 0x80109488 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109488(undefined4 param_1,int param_2)

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



/* ============ FUN_8010955c @ 0x8010955c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010955c(undefined4 param_1,int param_2)

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



/* ============ FUN_80109640 @ 0x80109640 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109640(void)

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
    goto LAB_80109e60;
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
    goto LAB_80109e60;
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
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f4)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f5)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f6)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f7)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f8)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8f9)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8fa)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
                   );
    func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011d8fb)[uVar10] * 0xac + _DAT_800ac784[0x62] + 0x40
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
LAB_80109e60:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar5;
  }
  return;
}



/* ============ FUN_80109e84 @ 0x80109e84 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109e84(void)

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



/* ============ FUN_80109f38 @ 0x80109f38 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109f38(void)

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
      func_0x80019700(0x2800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar6 + 0x10,&DAT_8011d924);
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



/* ============ FUN_8010a2b8 @ 0x8010a2b8 ============ */

void FUN_8010a2b8(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a2f4 @ 0x8010a2f4 ============ */

void FUN_8010a2f4(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ============ FUN_8010a330 @ 0x8010a330 ============ */

void FUN_8010a330(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ============ FUN_8010a354 @ 0x8010a354 ============ */

void FUN_8010a354(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ============ FUN_8010a7e4 @ 0x8010a7e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a7e4(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011e318
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011e318
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  return;
}



/* ============ FUN_8010a9b4 @ 0x8010a9b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a9b4(void)

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
    (*(code *)(&PTR_FUN_8011e358)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0) {
      *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xed;
    }
    func_0x8001b4e4();
    FUN_80104264();
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



/* ============ FUN_8010ac18 @ 0x8010ac18 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ac18(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011d8e0;
  uVar3 = func_0x8001af20();
  *(undefined *)(_DAT_800ac784 + 0x75) = (&DAT_8011d934)[uVar3 & 7];
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
                    (uint)(byte)(&DAT_8011d8f4)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f5)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f6)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f7)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f9)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fa)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fb)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &DAT_8011d924);
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



/* ============ FUN_8010b360 @ 0x8010b360 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b360(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f5)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f6)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f7)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fb)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (*(code *)(&PTR_FUN_8011e380)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8010b7c0 @ 0x8010b7c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b7c0(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_8011e3b4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e3f8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b870 @ 0x8010b870 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b870(void)

{
  (*(code *)(&PTR_FUN_8011e43c)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e458)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b8ec @ 0x8010b8ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8ec(void)

{
  (*(code *)(&PTR_FUN_8011e474)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e3f8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b968 @ 0x8010b968 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b968(void)

{
  (*(code *)(&PTR_FUN_8011e4ac)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e458)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010b9e4 @ 0x8010b9e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9e4(void)

{
  (*(code *)(&PTR_FUN_8011e4c4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4c8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010ba60 @ 0x8010ba60 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba60(void)

{
  (*(code *)(&PTR_FUN_8011e4cc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4d0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010badc @ 0x8010badc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010badc(void)

{
  (*(code *)(&PTR_LAB_8011e4d4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4d0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bb58 @ 0x8010bb58 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb58(void)

{
  (*(code *)(&PTR_FUN_8011e4d8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4d0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bbd4 @ 0x8010bbd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bbd4(void)

{
  (*(code *)(&PTR_FUN_8011e4dc)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4e0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bc50 @ 0x8010bc50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc50(void)

{
  (*(code *)(&PTR_LAB_8011e4e4)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011e4e8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010bcd4 @ 0x8010bcd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bcd4(void)

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



/* ============ FUN_8010bf3c @ 0x8010bf3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf3c(void)

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
    FUN_8010c174(0,uVar1);
  }
  return;
}



/* ============ FUN_8010c06c @ 0x8010c06c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c06c(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1b14))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_80106b24();
  }
  else {
    FUN_8010698c();
  }
  return;
}



/* ============ FUN_8010c100 @ 0x8010c100 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c100(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1874))();
  }
  else {
    FUN_80107d9c();
  }
  return;
}



/* ============ FUN_8010c174 @ 0x8010c174 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c174(undefined4 param_1,int param_2)

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



/* ============ FUN_8010c2d8 @ 0x8010c2d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c2d8(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_8011ea8c)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8010c41c @ 0x8010c41c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c41c(void)

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
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011ea88;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,1000,500,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)(&DAT_8011d1c8 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
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



/* ============ FUN_8010c6ac @ 0x8010c6ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c6ac(void)

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
  (*(code *)(&PTR_LAB_8011eaac)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011eaec)[*(byte *)((int)_DAT_800ac784 + 5)])();
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



/* ============ FUN_8010c980 @ 0x8010c980 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c980(void)

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



/* ============ FUN_8010ca7c @ 0x8010ca7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ca7c(void)

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



/* ============ FUN_8010cba4 @ 0x8010cba4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cba4(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800acae7 != '\0') || (iVar1 = func_0x8001a804(0x9c4,0xc0,&DAT_800aca88), -1 < iVar1)) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
       ((_DAT_800aca58 == 0x701 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      goto LAB_8010cd24;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) &&
       ((_DAT_800aca58 == 0x201 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      goto LAB_8010cd24;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1e6) == 0) &&
       (((0x50 < _DAT_800acaee && (7000 < *(short *)(_DAT_800ac784 + 0x1d4))) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)))) {
      uVar2 = func_0x8001af20();
      uVar3 = func_0x8001af20();
      if ((uVar2 & 1) * (uVar3 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        goto LAB_8010cd24;
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
LAB_8010cd24:
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ============ FUN_8010cde4 @ 0x8010cde4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cde4(void)

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
  FUN_80110350(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x1e);
  func_0x800245d8(8);
  return;
}



/* ============ FUN_8010d048 @ 0x8010d048 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d048(void)

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



/* ============ FUN_8010d1b4 @ 0x8010d1b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d1b4(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  short sVar5;
  uint uVar6;
  
  FUN_801109ec(0x14,0xf0);
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
      FUN_80110a4c(0x1e,200);
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
  FUN_80110aac((int)sVar4);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar6 + (uVar6 / 7 + (uVar6 - uVar6 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}



/* ============ FUN_8010d524 @ 0x8010d524 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d524(void)

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
                      *(int *)(_DAT_800ac784 + 0x188) + 0x2f0,&DAT_8011ea6c);
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
    goto switchD_8010d55c_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010d55c_default:
  return;
}



/* ============ FUN_8010d794 @ 0x8010d794 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d794(void)

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
    goto switchD_8010d7cc_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010d7cc_default:
  return;
}



/* ============ FUN_8010d970 @ 0x8010d970 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d970(void)

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



/* ============ FUN_8010da9c @ 0x8010da9c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010da9c(void)

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
  FUN_80110350(0,*(byte *)(_DAT_800ac784 + 0x95) < 0xd);
  func_0x800245d8(0x800);
  return;
}



/* ============ FUN_8010dc3c @ 0x8010dc3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dc3c(void)

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
switchD_8010dc78_caseD_1:
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
                           ((int)&PTR_LAB_8011eb28 + (uVar4 & 1) + (uint)DAT_800aca59 * 2),0x32);
        }
        else {
LAB_8010df48:
          cVar1 = func_0x8001a780(&DAT_800aca54);
          *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
          *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
        }
      }
    }
switchD_8010dc78_default:
    return;
  case 1:
    goto switchD_8010dc78_caseD_1;
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
LAB_8010e078:
      cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),
                              *(undefined1 *)(_DAT_800ac784 + 7),0x200);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
      return;
    }
    func_0x800245d8(0);
    if ((DAT_800acae7 != 0) || (iVar3 = func_0x8001a804(2000,0x180,&DAT_800aca88), -1 < iVar3))
    goto LAB_8010e078;
    _DAT_800acaee = _DAT_800acaee - 10;
    if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
      func_0x800453d0(3);
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      DAT_800aca58 = 2;
      cVar1 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar1 + 2;
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      goto LAB_8010e078;
    }
    goto LAB_8010df48;
  case 4:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 10;
    uVar2 = 3;
code_r0x8010e2a4:
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
    goto LAB_8010e2a8;
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
    if ((uVar4 & 1) != 0) goto code_r0x8010e2a4;
LAB_8010e2a8:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  default:
    goto switchD_8010dc78_default;
  }
}



/* ============ FUN_8010e2ec @ 0x8010e2ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e2ec(void)

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
LAB_8010e714:
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
    goto LAB_8010e714;
  default:
    break;
  }
  return;
}



/* ============ FUN_8010e740 @ 0x8010e740 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e740(void)

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
LAB_8010eb68:
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
    goto LAB_8010eb68;
  default:
    break;
  }
  return;
}



/* ============ FUN_8010eb94 @ 0x8010eb94 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eb94(void)

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



/* ============ FUN_8010ecc4 @ 0x8010ecc4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ecc4(void)

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



/* ============ FUN_8010edbc @ 0x8010edbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010edbc(void)

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



/* ============ FUN_8010f09c @ 0x8010f09c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f09c(void)

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



/* ============ FUN_8010f3d0 @ 0x8010f3d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f3d0(void)

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
  (*(code *)(&PTR_FUN_8011eb30)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010f4c0 @ 0x8010f4c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f4c0(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&DAT_8011ea6c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ============ FUN_8010f67c @ 0x8010f67c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f67c(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011ea6c);
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



/* ============ FUN_8010f8a0 @ 0x8010f8a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f8a0(void)

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
  (*(code *)(&PTR_FUN_8011eb88)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010f990 @ 0x8010f990 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f990(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011ea6c);
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



/* ============ FUN_8010fc00 @ 0x8010fc00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc00(void)

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



/* ============ FUN_8010fe30 @ 0x8010fe30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe30(void)

{
  (*(code *)(&PTR_FUN_8011ebe0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8010fe78 @ 0x8010fe78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe78(void)

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



/* ============ FUN_80110138 @ 0x80110138 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110138(void)

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



/* ============ FUN_80110350 @ 0x80110350 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110350(undefined4 param_1,int param_2)

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



/* ============ FUN_801109ec @ 0x801109ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801109ec(short param_1,short param_2)

{
  if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
    *(short *)(_DAT_800ac784 + 0x8c) = param_1 + *(short *)(_DAT_800ac784 + 0x8c);
    if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}



/* ============ FUN_80110a4c @ 0x80110a4c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a4c(short param_1,short param_2)

{
  if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) - param_1;
    if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}



/* ============ FUN_80110aac @ 0x80110aac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110aac(short param_1)

{
  char *pcVar1;
  
  pcVar1 = &DAT_8011ec10;
  if (param_1 == 0) {
    pcVar1 = &DAT_8011ec11;
  }
  else if (0 < param_1) {
    pcVar1 = &DAT_8011ec12;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar1) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar1;
  }
  return;
}



/* ============ FUN_80110b00 @ 0x80110b00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b00(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_FUN_8011eca4)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80110c98 @ 0x80110c98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110c98(void)

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
       *(undefined2 *)(&DAT_8011d1c8 + (uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_8011ec44)[(byte)_DAT_800ac784[0x79]];
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



/* ============ FUN_8011101c @ 0x8011101c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011101c(void)

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
  (*(code *)(&PTR_LAB_8011ecc4)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011ed04)[*(byte *)((int)_DAT_800ac784 + 5)])();
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



/* ============ FUN_80111310 @ 0x80111310 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111310(void)

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



/* ============ FUN_80111500 @ 0x80111500 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111500(void)

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



/* ============ FUN_801115fc @ 0x801115fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801115fc(void)

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
  FUN_80115b68(0,0);
  return;
}



/* ============ FUN_801117d8 @ 0x801117d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801117d8(void)

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



/* ============ FUN_80111910 @ 0x80111910 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111910(void)

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
    FUN_80115b68(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x18);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = '\x18';
  }
  else {
    FUN_80115b68(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x29);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = ')';
  }
  if ((cVar1 == '\x01') || (cVar1 == cVar3)) {
    func_0x800453d0(8);
  }
  return;
}



/* ============ FUN_80111ac0 @ 0x80111ac0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111ac0(void)

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



/* ============ FUN_80111c08 @ 0x80111c08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c08(void)

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



/* ============ FUN_80111d68 @ 0x80111d68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111d68(void)

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
  FUN_80115b68(0,0);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x8011ed48)) {
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



/* ============ FUN_80112044 @ 0x80112044 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112044(void)

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
    FUN_80115c3c(0,0);
  }
  else {
    FUN_80115b68(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar8 + 0x8011ed4c)) {
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



/* ============ FUN_801123fc @ 0x801123fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123fc(void)

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
            goto LAB_80112754;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
          local_28 = DAT_8011ec84;
          local_24 = DAT_8011ec88;
          local_20 = DAT_8011ec8c;
          local_1c = DAT_8011ec90;
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
LAB_80112754:
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_80112764;
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
LAB_80112764:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80112784 @ 0x80112784 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112784(void)

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
    local_48 = DAT_8011ec84;
    local_44 = DAT_8011ec88;
    local_40 = DAT_8011ec8c;
    local_3c = DAT_8011ec90;
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x8011ed54)) {
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



/* ============ FUN_80112c60 @ 0x80112c60 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112c60(void)

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
        local_28 = DAT_8011ec84;
        local_24 = DAT_8011ec88;
        local_20 = DAT_8011ec8c;
        local_1c = DAT_8011ec90;
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



/* ============ FUN_80112d48 @ 0x80112d48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112d48(void)

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
      *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8011ec48)[*(byte *)(_DAT_800ac784 + 0x1e4)];
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



/* ============ FUN_80113010 @ 0x80113010 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113010(void)

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
        local_28 = DAT_8011ec84;
        local_24 = DAT_8011ec88;
        local_20 = DAT_8011ec8c;
        local_1c = DAT_8011ec90;
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



/* ============ FUN_801130f8 @ 0x801130f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801130f8(void)

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
LAB_8011324c:
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
    goto LAB_8011324c;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8011ec44)[*(byte *)(_DAT_800ac784 + 0x1e4)];
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



/* ============ FUN_80113374 @ 0x80113374 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113374(void)

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
      local_28 = DAT_8011ec84;
      local_24 = DAT_8011ec88;
      local_20 = DAT_8011ec8c;
      local_1c = DAT_8011ec90;
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



/* ============ FUN_80113448 @ 0x80113448 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113448(void)

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



/* ============ FUN_80113614 @ 0x80113614 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113614(void)

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
      local_28 = DAT_8011ec84;
      local_24 = DAT_8011ec88;
      local_20 = DAT_8011ec8c;
      local_1c = DAT_8011ec90;
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



/* ============ FUN_801136e8 @ 0x801136e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801136e8(void)

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
    FUN_80115c3c(0,0);
    if (*(char *)(_DAT_800ac784 + 0x95) == '2') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
LAB_8011391c:
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
LAB_801138f8:
    *(char *)(_DAT_800ac784 + 6) = cVar1 + cVar2;
    FUN_80115c3c(0,uVar3);
    break;
  case 4:
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    goto LAB_8011391c;
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
    goto LAB_801138f8;
  default:
    break;
  }
  return;
}



/* ============ FUN_8011394c @ 0x8011394c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011394c(void)

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
      goto LAB_80113c0c;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar6 & 0xfd;
    local_38 = DAT_8011ec84;
    local_34 = DAT_8011ec88;
    local_30 = DAT_8011ec8c;
    local_2c = DAT_8011ec90;
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
      if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed59)[bVar6]) {
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar6 + 0x8011ed58)) {
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
      goto LAB_80113c0c;
    }
    uVar4 = 0xe;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80113c0c:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ============ FUN_80113c3c @ 0x80113c3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c3c(void)

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
  FUN_80115c3c(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x19);
  return;
}



/* ============ FUN_80113dbc @ 0x80113dbc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113dbc(void)

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
      local_28 = DAT_8011ec84;
      local_24 = DAT_8011ec88;
      local_20 = DAT_8011ec8c;
      local_1c = DAT_8011ec90;
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



/* ============ FUN_80113e90 @ 0x80113e90 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e90(void)

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
  FUN_80115c3c(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x28);
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar4 + 0x8011ed5c)) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar8 + 0x8011ed64)) {
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



/* ============ FUN_801142c4 @ 0x801142c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801142c4(void)

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
    goto LAB_801145a4;
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
LAB_801145a4:
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
    _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_8011ec44)[(byte)_DAT_800ac784[0x79]];
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



/* ============ FUN_80114790 @ 0x80114790 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114790(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8011ec44)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    func_0x800453d0(1);
  }
  (*(code *)(&PTR_FUN_8011ed84)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011484c @ 0x8011484c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011484c(void)

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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80115b68(0,0);
  }
  else {
    FUN_80115b68(0,1);
  }
  return;
}



/* ============ FUN_80114cb8 @ 0x80114cb8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114cb8(void)

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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar3);
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



/* ============ FUN_80114fb4 @ 0x80114fb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114fb4(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8011ec44)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    func_0x800453d0(1);
  }
  (*(code *)(&PTR_FUN_8011eddc)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80115070 @ 0x80115070 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115070(void)

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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar4);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar4);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar4);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar4);
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
  FUN_80115b68(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) == '=') {
    func_0x800453d0(1);
  }
  return;
}



/* ============ FUN_80115280 @ 0x80115280 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115280(void)

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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar4);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar4);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar4);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar4);
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



/* ============ FUN_801154b4 @ 0x801154b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801154b4(void)

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
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar5);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar5);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar5);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar5);
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



/* ============ FUN_80115734 @ 0x80115734 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115734(void)

{
  (*(code *)(&PTR_FUN_8011ee34)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_8011577c @ 0x8011577c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011577c(void)

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



/* ============ FUN_801158e8 @ 0x801158e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801158e8(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_8011ec48)[*(byte *)(_DAT_800ac784 + 0x1e4)];
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



/* ============ FUN_80115b68 @ 0x80115b68 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115b68(undefined4 param_1,int param_2)

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



/* ============ FUN_80115c3c @ 0x80115c3c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115c3c(undefined4 param_1,int param_2)

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



/* ============ FUN_801160a4 @ 0x801160a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801160a4(void)

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
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011ee4c
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011ee4c
                   );
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + cVar1,_DAT_800aca58);
  return;
}



/* ============ FUN_80116230 @ 0x80116230 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116230(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  short sVar6;
  uint local_28;
  undefined4 local_24;
  uint local_20;
  undefined4 local_1c;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar3 = (int)(((_DAT_800aca88 & 0xffff) - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >>
            0x10;
    iVar4 = (int)(((_DAT_800aca90 & 0xffff) - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >>
            0x10;
    uVar5 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar5;
    func_0x8001bd60(0xfffffff6,0x14);
    local_28 = _DAT_800aca88;
    local_24 = _DAT_800aca8c;
    local_20 = _DAT_800aca90;
    local_1c = _DAT_800aca94;
    if ((*(byte *)(_DAT_800ac784 + 0x1dd) & 1) != 0) {
      local_28 = (uint)(short)*(ushort *)(_DAT_800ac784 + 0x1d4);
      local_20 = (uint)(short)*(ushort *)(_DAT_800ac784 + 0x1d6);
      iVar3 = (int)(((uint)*(ushort *)(_DAT_800ac784 + 0x1d4) -
                    (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10;
      iVar4 = (int)(((uint)*(ushort *)(_DAT_800ac784 + 0x1d6) -
                    (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10;
      uVar5 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
      *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar5;
    }
    func_0x80039e7c(&local_28,0,0);
    if (*(char *)(_DAT_800ac784 + 0x1de) != '\0') {
      *(char *)(_DAT_800ac784 + 0x1de) = *(char *)(_DAT_800ac784 + 0x1de) + -1;
      if (*(char *)(_DAT_800ac784 + 0x1de) == '\0') {
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
    }
    if (*(char *)(_DAT_800ac784 + 0x1df) != '\0') {
      *(char *)(_DAT_800ac784 + 0x1df) = *(char *)(_DAT_800ac784 + 0x1df) + -1;
    }
    (*(code *)(&PTR_FUN_8011ee84)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8001b4e4();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xfd;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x8001b38c();
    if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) == 0) && (*(short *)(_DAT_800ac784 + 0x9a) < 200)) {
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1dd);
      *(byte *)(_DAT_800ac784 + 0x1dd) = bVar1 ^ 4;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar6 = -(ushort)((bVar1 & 4) != 0);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x736) = sVar6 * sVar2 * -3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x6b8,1);
      *(undefined2 *)(iVar3 + 0x736) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x7e2) = sVar6 * sVar2 * 3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x764,1);
      *(undefined2 *)(iVar3 + 0x7e2) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x88e) = sVar6 * sVar2 * -2 + 0x1000;
      func_0x8003a95c(iVar3 + 0x810,1);
      *(undefined2 *)(iVar3 + 0x88e) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) = sVar6 * sVar2 * 3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) = sVar6 * sVar2 * -2 + 0x1000;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0xfff;
      if (*(short *)(_DAT_800ac784 + 0x9a) < 100) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
      }
    }
    *(undefined4 *)(_DAT_800ac784 + 0x1d8) = *(undefined4 *)(_DAT_800ac784 + 4);
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ============ FUN_801166e0 @ 0x801166e0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166e0(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011ee7c;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 9;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x6a4) = 0x60;
  *(undefined2 *)(uVar1 + 0x6a6) = 0x30;
  *(undefined2 *)(uVar1 + 0x6a0) = 0;
  *(undefined2 *)(uVar1 + 0x6a2) = 0;
  *(undefined2 *)(uVar1 + 0x6a8) = 0x200;
  *(undefined2 *)(uVar1 + 0x6aa) = 100;
  _DAT_800ac784[0x76] = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1dd) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x77) = 9;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1de) = 0;
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 3) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x10;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 9;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 1) {
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 10;
    *_DAT_800ac784 = *_DAT_800ac784 | 8;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x400);
  func_0x8001ee68(_DAT_800ac784);
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 300;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 4) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x5de) = 0xfff;
    func_0x8003a95c(uVar1 + 0x560,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x68a) = 0xfff;
    func_0x8003a95c(uVar1 + 0x60c,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x93a) = 0xfff;
    func_0x8003a95c(uVar1 + 0x8bc,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x9e6) = 0xfff;
    func_0x8003a95c(uVar1 + 0x968,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0xa92) = 0xfff;
    func_0x8003a95c(uVar1 + 0xa14,1);
    return;
  }
  *(undefined4 *)(_DAT_800ac784[0x62] + 0xac0) = 0;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0xa92) = 0xfff;
  func_0x8003a95c(uVar1 + 0xa14,1);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) != 0) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar1 + 0xa1c) = *(undefined4 *)(uVar1 + 0xac8);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar1 + 0xad0);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar1 + 0xacc);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar1 + 0xad4);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x88e) = 0;
    func_0x8003a95c(uVar1 + 0x810,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x68a) = 0;
    func_0x8003a95c(uVar1 + 0x60c,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x736) = 0;
    func_0x8003a95c(uVar1 + 0x6b8,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x7e2) = 0;
    func_0x8003a95c(uVar1 + 0x764,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x532) = 0;
    func_0x8003a95c(uVar1 + 0x4b4,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x5de) = 0;
    func_0x8003a95c(uVar1 + 0x560,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x93a) = 0;
    func_0x8003a95c(uVar1 + 0x8bc,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x9e6) = 0;
    func_0x8003a95c(uVar1 + 0x968,1);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) != 0) goto LAB_80116c08;
  }
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x88e) = 0xfff;
  func_0x8003a95c(uVar1 + 0x810,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x68a) = 0xfff;
  func_0x8003a95c(uVar1 + 0x60c,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x736) = 0xfff;
  func_0x8003a95c(uVar1 + 0x6b8,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x7e2) = 0xfff;
  func_0x8003a95c(uVar1 + 0x764,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x532) = 0xfff;
  func_0x8003a95c(uVar1 + 0x4b4,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5de) = 0xfff;
  func_0x8003a95c(uVar1 + 0x560,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x93a) = 0xfff;
  func_0x8003a95c(uVar1 + 0x8bc,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x9e6) = 0xfff;
  func_0x8003a95c(uVar1 + 0x968,1);
LAB_80116c08:
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 0) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar1 + 0xa1c) = *(undefined4 *)(uVar1 + 0xac8);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar1 + 0xad0);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar1 + 0xacc);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar1 + 0xad4);
  }
  return;
}



/* ============ FUN_80116d38 @ 0x80116d38 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d38(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012a0c(2000);
  func_0x80012974(4000);
  (*(code *)(&PTR_FUN_8011eea4)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80116dcc @ 0x80116dcc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116dcc(void)

{
  (*(code *)(&PTR_LAB_8011eeb8)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011eef0)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80116e48 @ 0x80116e48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116e48(void)

{
  (*(code *)(&PTR_LAB_8011ef28)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011ef2c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ============ FUN_80116ed8 @ 0x80116ed8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116ed8(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  return;
}



/* ============ FUN_80116f6c @ 0x80116f6c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f6c(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x338);
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x10);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,500);
  sVar4 = func_0x8001a9cc(&DAT_800aca88,0x464);
  if ((*_DAT_800ac784 & 0x20) == 0) {
    if ((((*(char *)((int)_DAT_800ac784 + 0x1df) == '\0') && (_DAT_800ac784[0x74] < 0xc80)) &&
        (DAT_800aca58 != '\x05')) && (sVar1 == 0)) {
      _DAT_800ac784[1] = 0x301;
    }
    if (((_DAT_800ac784[0x74] < 0x9c4) && (DAT_800aca58 != '\x05')) && (sVar3 == 0)) {
      _DAT_800ac784[1] = 0x401;
    }
    if ((_DAT_800ac784[0x74] < 0xed8) && (sVar2 != 0)) {
      if (DAT_800aca58 == '\x05') goto LAB_8011712c;
      if (((sVar4 == 0) && (100 < _DAT_800acae0)) && (*(char *)((int)_DAT_800ac784 + 0x1df) == '\0')
         ) {
        uVar5 = 0x701;
LAB_80117114:
        _DAT_800ac784[1] = uVar5;
      }
    }
  }
  else if (_DAT_800ac784[0x74] < 0x9c4) {
    if (DAT_800aca58 == '\x05') goto LAB_8011712c;
    if (sVar3 == 0) {
      uVar5 = 0x401;
      goto LAB_80117114;
    }
  }
  if (DAT_800aca58 != '\x05') {
    return;
  }
LAB_8011712c:
  if ((_DAT_800ac784[0x74] < 3000) && (sVar2 == 0)) {
    _DAT_800ac784[1] = 0x501;
  }
  if (_DAT_800ac784[0x74] < 2000) {
    _DAT_800ac784[1] = 0x801;
  }
  if (((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) == 0) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x1dd) & 4) != 0)) {
    _DAT_800ac784[1] = 0x601;
  }
  return;
}



/* ============ FUN_801171d4 @ 0x801171d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801171d4(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),
                        (int)*(short *)(_DAT_800ac784 + 0x1be),0x20);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
          *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
        }
        cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
        *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
        if (cVar2 == '\0') {
          *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        }
      }
      goto LAB_801173cc;
    }
    if (bVar1 != 0) goto LAB_801173cc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_8011b5c4(1,1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x14;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
  }
  iVar3 = func_0x8001a9cc(&DAT_800aca88,0x400);
  if ((iVar3 != 0) && (*(char *)(_DAT_800ac784 + 0x8f) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
  }
LAB_801173cc:
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801173ec @ 0x801173ec ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801173ec(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') goto LAB_80117490;
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x30
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
LAB_80117490:
  FUN_8011b5c4(1,1);
  iVar1 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar1 >> 0x10) - (iVar1 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ============ FUN_801174fc @ 0x801174fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801174fc(void)

{
  undefined2 uVar1;
  char cVar2;
  undefined1 uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  int local_28;
  int local_24;
  int local_20;
  char local_18 [8];
  
  local_38 = DAT_80100390;
  local_34 = DAT_80100394;
  local_30 = DAT_80100398;
  local_2c = DAT_8010039c;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
switchD_801175a8_caseD_1:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    iVar5 = func_0x8001a9cc(&DAT_800aca88,0x400);
    if (((iVar5 == 0) && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0xfa1)) ||
       (0x1d < *(byte *)(_DAT_800ac784 + 0x95))) {
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0x24 < 8) {
        iVar5 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_38,&local_30,
                                &DAT_800aca54);
        if ((iVar5 != 0) && (DAT_800acae7 == '\0')) {
          sVar4 = func_0x8001a6d4(*(undefined4 *)(_DAT_800ac784 + 0x34),
                                  *(undefined4 *)(_DAT_800ac784 + 0x3c),_DAT_800aca88,_DAT_800aca90)
          ;
          uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
          *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + -0x400;
          uVar6 = func_0x8001a780(&DAT_800aca54);
          *(undefined2 *)(_DAT_800ac784 + 0x6a) = uVar1;
          _DAT_800acabe = sVar4 - ((short)(uVar6 & 0xff) * 0x800 + -0x400);
          _DAT_800acbfc = _DAT_800ac784;
          _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
          _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
          _DAT_800aca58 = (uVar6 & 0xff) << 8 | 5;
          func_0x800453d0(7);
          _DAT_800acaee = _DAT_800acaee - 10;
          if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
            _DAT_800acaee = 1;
          }
        }
        local_28 = (int)(short)local_30;
        local_24 = (int)local_30._2_2_;
        local_20 = (int)(short)local_2c;
        func_0x8001c2dc(&local_28,400,local_18);
        if (local_18[0] != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        }
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
        func_0x800453d0(4);
      }
switchD_801175a8_default:
      FUN_8011b5c4(0,0);
    }
    else {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    }
    return;
  case 1:
    goto switchD_801175a8_caseD_1;
  case 2:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    uVar3 = 0x1e;
    goto LAB_801179a8;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x800453d0(6);
  case 4:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x400);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) && (*(char *)(_DAT_800ac784 + 0x95) == '\x1e'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    }
    goto switchD_801175a8_default;
  case 5:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    uVar3 = 0x3c;
LAB_801179a8:
    *(undefined1 *)(_DAT_800ac784 + 0x1df) = uVar3;
  default:
    goto switchD_801175a8_default;
  }
}



/* ============ FUN_801179d8 @ 0x801179d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801179d8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_801003bc;
  local_14 = DAT_801003c0;
  local_10 = DAT_801003c4;
  local_c = DAT_801003c8;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      }
      goto LAB_80117ce0;
    }
    if (bVar1 != 0) goto LAB_80117ce0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    }
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  iVar3 = func_0x8001a9cc(&DAT_800aca88,800);
  if (((iVar3 != 0) || (0xdac < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
     (*(byte *)(_DAT_800ac784 + 0x95) < 0xf)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    return;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if (((*(byte *)(_DAT_800ac784 + 0x95) - 0x23 < 8) &&
      (iVar3 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                               &DAT_800aca54), iVar3 != 0)) && (DAT_800acae7 == '\0')) {
    _DAT_800aca58 = 0x205;
    if (0x25 < *(byte *)(_DAT_800ac784 + 0x95)) {
      _DAT_800aca58 = 0x202;
    }
    _DAT_800acbfc = _DAT_800ac784;
    _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
    _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
    func_0x800453d0(7);
    _DAT_800acaee = _DAT_800acaee - 10;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      _DAT_800acaee = 1;
    }
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
  }
LAB_80117ce0:
  FUN_8011b5c4(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x32);
  return;
}



/* ============ FUN_80117d30 @ 0x80117d30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d30(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_801003cc;
  local_14 = DAT_801003d0;
  local_10 = DAT_801003d4;
  local_c = DAT_801003d8;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      }
      goto LAB_80117f60;
    }
    if (bVar1 != 0) goto LAB_80117f60;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) == ',') {
    iVar3 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                            &DAT_800aca54);
    uVar4 = 2;
    if (iVar3 == 0) {
      uVar4 = 6;
    }
    else {
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      _DAT_800aca58 = 6;
      _DAT_800acc1e = (undefined2)local_c;
      _DAT_800acc1c = (undefined2)local_10;
    }
    func_0x800453d0(uVar4);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
  }
LAB_80117f60:
  FUN_8011b5c4(0,1);
  return;
}



/* ============ FUN_80117f80 @ 0x80117f80 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117f80(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x1dc) = 9;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
        *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) & 0xf7;
      }
      goto LAB_801188cc;
    }
    if (bVar1 != 0) goto LAB_801188cc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    func_0x8004ef90(0x800b1028,0x1d);
    func_0x800453d0(1);
    func_0x800453d0(8);
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x96;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 8;
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x95);
  if (((cVar2 == '`') || (cVar2 == '<')) || (cVar2 == 'x')) {
    func_0x800453d0(8);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == -0x6a) {
    func_0x800453d0(9);
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x6e) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x736) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 3;
    func_0x8003a95c(iVar4 + 0x6b8,1);
    *(undefined2 *)(iVar4 + 0x736) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x7e2) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -3;
    func_0x8003a95c(iVar4 + 0x764,1);
    *(undefined2 *)(iVar4 + 0x7e2) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x810,1);
    *(undefined2 *)(iVar4 + 0x88e) = 0xfff;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0xb < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x3d < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x61 < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0xca;
    }
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == 'n') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x736) = 0;
    func_0x8003a95c(iVar4 + 0x6b8,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x7e2) = 0;
    func_0x8003a95c(iVar4 + 0x764,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x88e) = 0;
    func_0x8003a95c(iVar4 + 0x810,1);
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x532) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x4b4,1);
    *(undefined2 *)(iVar4 + 0x532) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x5de) =
         *(short *)(iVar4 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x560,1);
    *(undefined2 *)(iVar4 + 0x5de) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x68a) =
         *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x60c,1);
    *(undefined2 *)(iVar4 + 0x68a) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x93a) =
         *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x8bc,1);
    *(undefined2 *)(iVar4 + 0x93a) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x9e6) =
         *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x968,1);
    *(undefined2 *)(iVar4 + 0x9e6) = 0xfff;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x8003a95c(iVar4 + 0x4b4,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x5de) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar5 + 0x560,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x68a) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar5 + 0x60c,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x93a) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar5 + 0x8bc,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x9e6) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar5 + 0x968,1);
    *(short *)(iVar4 + 0x532) = *(short *)(iVar4 + 0x532) + -0x88;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x532) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x4b4,1);
    *(undefined2 *)(iVar4 + 0x532) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x5de) = *(short *)(iVar4 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x560,1);
    *(undefined2 *)(iVar4 + 0x5de) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x68a) = *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x60c,1);
    *(undefined2 *)(iVar4 + 0x68a) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x93a) =
         *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x8bc,1);
    *(undefined2 *)(iVar4 + 0x93a) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x9e6) = *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x968,1);
    *(undefined2 *)(iVar4 + 0x9e6) = 0;
  }
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
LAB_801188cc:
  FUN_8011b5c4(0,1);
  return;
}



/* ============ FUN_801188f8 @ 0x801188f8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801188f8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_801003bc;
  local_14 = DAT_801003c0;
  local_10 = DAT_80100398;
  local_c = DAT_8010039c;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
        *(undefined1 *)(_DAT_800ac784 + 0x1df) = 10;
      }
      goto LAB_80118a70;
    }
    if (bVar1 != 0) goto LAB_80118a70;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
LAB_80118a70:
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\x01') || (*(char *)(_DAT_800ac784 + 0x95) == '\x12')) {
    func_0x800453d0(4);
  }
  if ((((*(byte *)(_DAT_800ac784 + 0x95) - 4 < 10) || (*(byte *)(_DAT_800ac784 + 0x95) - 0x12 < 10))
      && (iVar3 = func_0x8001a5e0(iVar3 + 0xa14,&local_18,&local_10,&DAT_800aca54), iVar3 != 0)) &&
     (DAT_800acae7 == '\0')) {
    _DAT_800aca58 = 0x202;
    func_0x800453d0(7);
    _DAT_800acaee = _DAT_800acaee - 5;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      _DAT_800acaee = 1;
    }
  }
  return;
}



/* ============ FUN_80118d00 @ 0x80118d00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118d00(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x800453d0(3);
    func_0x800453d0(6);
  case 1:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 2:
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x800453d0(8);
  case 3:
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x5de) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x5de) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x5de) = 0;
    }
    func_0x8003a95c(iVar4 + 0x560,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x68a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x68a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x68a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x60c,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x93a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x93a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x93a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x8bc,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x9e6) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x9e6) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    func_0x8003a95c(iVar4 + 0x968,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0xa92) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0xa92) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0xa92) = 0;
    }
    func_0x8003a95c(iVar4 + 0xa14,1);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
    if (*(short *)(iVar4 + 0xa92) < 1) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    }
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x800453d0(9);
  case 5:
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x5de) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x5de) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x5de) = 0;
    }
    func_0x8003a95c(iVar4 + 0x560,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x68a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x68a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x68a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x60c,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x93a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x93a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x93a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x8bc,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x9e6) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x9e6) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    func_0x8003a95c(iVar4 + 0x968,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0xa92) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0xa92) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0xa92) = 0;
    }
    func_0x8003a95c(iVar4 + 0xa14,1);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
    if (*(short *)(iVar4 + 0xa92) < 1) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    return;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 6) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    func_0x8004ef90(0x800b1028,0x1d);
  case 7:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(4);
    }
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 6) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x8004ef90(0x800b1028,0x1f);
    uVar3 = 0;
    goto LAB_80119258;
  case 9:
  case 0xb:
    goto switchD_80118d58_caseD_9;
  case 10:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x800453d0(0);
    uVar3 = 1;
LAB_80119258:
    func_0x800453d0(uVar3);
switchD_80118d58_caseD_9:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x400;
    break;
  case 0xc:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x8004ef90(0x800b1028,0x1e);
    func_0x800453d0(6);
  case 0xd:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),1,0x400);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x32;
    func_0x800245d8(0x800);
    return;
  default:
    goto switchD_80118d58_caseD_e;
  }
  cVar1 = func_0x8001f314(uVar3,uVar5,0,uVar6);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_80118d58_caseD_e:
  return;
}



/* ============ FUN_80119378 @ 0x80119378 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119378(void)

{
  char cVar1;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x800453d0(3);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x800453d0(10);
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_8011b5c4(0,1);
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0xfc;
  }
  return;
}



/* ============ FUN_80119524 @ 0x80119524 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119524(void)

{
  char cVar1;
  uint uVar2;
  
  *(byte *)(_DAT_800ac784 + 0x6e) = (byte)_DAT_800ac784[0x6e] | 0x12;
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x78;
  case 1:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    if (0x46 < (short)_DAT_800ac784[0x27]) {
      FUN_8011b5c4(1,1);
      *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
      func_0x800245d8(0);
    }
    if ((short)_DAT_800ac784[0x27] == 0x46) {
      func_0x8004ef90(0x800b1028,0x1f);
    }
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x96;
  case 3:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '[') {
      func_0x800453d0(6);
      uVar2 = _DAT_800ac784[0x62];
      *(undefined4 *)(uVar2 + 0xa1c) = *(undefined4 *)(uVar2 + 0xac8);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar2 + 0xad0);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar2 + 0xacc);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar2 + 0xad4);
      func_0x8004ef90(0x800b1028,0x1e);
    }
    break;
  case 4:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x46;
  case 5:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    FUN_8011b5c4(1,1);
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
    func_0x800245d8(0);
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x28;
      func_0x800453d0(10);
    }
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xd;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '<') {
      func_0x800453d0(3);
    }
  case 7:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    FUN_8011b5c4(0,1);
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 9;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
  case 9:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    if (0x11 < *(byte *)((int)_DAT_800ac784 + 0x95)) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 10;
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 2;
    }
    break;
  case 10:
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xb;
    }
    break;
  case 0xb:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x1e;
  case 0xc:
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x40);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    FUN_8011b5c4(1,1);
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
    func_0x800245d8(0);
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xd;
    }
    break;
  case 0xd:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *(byte *)((int)_DAT_800ac784 + 9) = *(byte *)((int)_DAT_800ac784 + 9) & 0xfe;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffff7;
  }
  return;
}



/* ============ FUN_80119b50 @ 0x80119b50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119b50(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1d4) = 0xaa10;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xd120;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x60
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(uint *)(_DAT_800ac784 + 0x1d0) < 500) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  FUN_8011b5c4(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ============ FUN_80119cb4 @ 0x80119cb4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119cb4(void)

{
  short sVar1;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar1 == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  return;
}



/* ============ FUN_80119d08 @ 0x80119d08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119d08(void)

{
  char cVar1;
  int iVar2;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x14;
  }
  else if (*(char *)((int)_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x10);
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_8011b5c4(1,1);
  iVar2 = (uint)(ushort)_DAT_800ac784[0x23] << 0x10;
  *(short *)(_DAT_800ac784 + 0x23) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0xa0;
  func_0x800245d8(0);
  if (((*_DAT_800ac784 & 0x10) != 0) &&
     (cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e),
     *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1, cVar1 == '\0')) {
    _DAT_800ac784[1] = 0x701;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
  }
  return;
}



/* ============ FUN_80119e78 @ 0x80119e78 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119e78(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(short *)(_DAT_800ac784 + 0x6a) =
           *(short *)(_DAT_800ac784 + 0x6a) +
           (short)((int)((uint)*(byte *)(_DAT_800ac784 + 0x9e) << 0x18) >> 0x13);
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x20101;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 8;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x400;
    sVar3 = func_0x8001a9cc(&DAT_800aca88,0x400);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + -0x400;
    if (sVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xf8;
    }
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       (short)*(char *)(_DAT_800ac784 + 0x9e) + *(short *)(_DAT_800ac784 + 0x6a);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}



/* ============ FUN_8011a060 @ 0x8011a060 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a060(void)

{
  short sVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = DAT_80100068;
  local_1c = DAT_8010006c;
  local_18 = DAT_80100070;
  local_14 = DAT_80100074;
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1de) = 9;
    sVar1 = func_0x8001a6d4(*(undefined4 *)(_DAT_800ac784 + 0x34),
                            *(undefined4 *)(_DAT_800ac784 + 0x3c),_DAT_800aca88,_DAT_800aca90);
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x20);
    func_0x800453d0(0);
    uVar3 = 0x3000;
    if (4000 < *(uint *)(_DAT_800ac784 + 0x1d0)) {
      uVar3 = 0x2400;
      *(short *)(_DAT_800ac784 + 0x9a) = *(short *)(_DAT_800ac784 + 0x9a) + 5;
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      func_0x80019700(uVar3,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x198,&local_20);
    }
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(uVar3,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x5a0,&local_20);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\x02') {
      func_0x80019700(uVar3,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20);
    }
    cVar2 = *(char *)(_DAT_800ac784 + 0x1dc);
    *(char *)(_DAT_800ac784 + 0x1dc) = cVar2 + -1;
    if (cVar2 != '\0') {
      *(undefined4 *)(_DAT_800ac784 + 4) = *(undefined4 *)(_DAT_800ac784 + 0x1d8);
      return;
    }
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee10bc))();
  return;
}



/* ============ FUN_8011a284 @ 0x8011a284 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a284(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x1dc) = (bVar1 & 3) + 3;
    uVar3 = func_0x8001af20();
    uVar4 = 1;
    if ((uVar3 & 1) != 0) {
      uVar4 = 3;
    }
    func_0x800453d0(uVar4);
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  return;
}



/* ============ FUN_8011a3f0 @ 0x8011a3f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a3f0(void)

{
  short sVar1;
  char cVar2;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = DAT_80100068;
  local_1c = DAT_8010006c;
  local_18 = DAT_80100070;
  local_14 = DAT_80100074;
  if ((*(char *)(_DAT_800ac784 + 7) == '\0') && ((*(byte *)(_DAT_800ac784 + 0x1dd) & 8) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1de) = 9;
    sVar1 = func_0x8001a6d4(*(undefined4 *)(_DAT_800ac784 + 0x34),
                            *(undefined4 *)(_DAT_800ac784 + 0x3c),_DAT_800aca88,_DAT_800aca90);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x198,&local_20);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x5a0,&local_20);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\x02') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20);
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x32;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined4 *)(_DAT_800ac784 + 4) = *(undefined4 *)(_DAT_800ac784 + 0x1d8);
  }
  else {
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0e1c))();
  }
  return;
}



/* ============ FUN_8011a5d8 @ 0x8011a5d8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5d8(void)

{
  char cVar1;
  char cVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(1);
  case 1:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    cVar1 = 'P';
    goto LAB_8011a73c;
  case 2:
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 300;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    func_0x8004ef90(0x800b1028,0x1c);
  case 3:
    if ((*(byte *)(_DAT_800ac784 + 9) & 0xf) == 2) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  case 5:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    cVar1 = '<';
LAB_8011a73c:
    if (cVar2 == cVar1) {
      func_0x800453d0(10);
    }
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    break;
  case 6:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xb01;
  }
  FUN_8011b5c4(0,0);
  return;
}



/* ============ FUN_8011b1a8 @ 0x8011b1a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b1a8(void)

{
  uint *puVar1;
  char cVar2;
  int iVar3;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  local_30 = DAT_80100068;
  local_2c = DAT_8010006c;
  local_28 = DAT_80100070;
  local_24 = DAT_80100074;
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        return;
      }
      _DAT_800aca58 = 7;
      return;
    }
    if (DAT_800aca5a != 0) {
      return;
    }
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 3;
    DAT_800acae8 = 1;
    DAT_800acc0c = 1;
    DAT_800acae9 = 0;
    _DAT_800acae0 = 0;
    _DAT_800aca3c = _DAT_800aca3c | 0x40;
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x80045024(0x4030001,&DAT_800aca88);
    puVar1 = _DAT_800acbdc;
    iVar3 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]));
    if (300 < iVar3) {
      *(undefined2 *)((int)puVar1 + 0x8a) = 3000;
      *(undefined2 *)(puVar1 + 0x22) = 5000;
      *(undefined2 *)(puVar1 + 0x23) = 5000;
      puVar1[0x25] = 0x70;
      *puVar1 = *puVar1 | 0x2000;
      puVar1[0x26] = 0x20;
      *puVar1 = *puVar1 | 0x80;
      func_0x80019700(0x3000,0,puVar1 + 0x10,&local_30);
    }
    puVar1 = _DAT_800acbdc;
    iVar3 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]));
    if (300 < iVar3) {
      puVar1[0x56] = puVar1[0x56] | 0x80;
      puVar1[0x7b] = 0x70;
      puVar1[0x7c] = 0x20;
      *(undefined2 *)(puVar1 + 0x78) = 5000;
      *(undefined2 *)((int)puVar1 + 0x1e2) = 3000;
      *(undefined2 *)(puVar1 + 0x79) = 5000;
      puVar1[0x56] = puVar1[0x56] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0x66,&local_30);
      puVar1[0xd7] = puVar1[0xd7] | 0x80;
      puVar1[0xfc] = 0x70;
      puVar1[0xfd] = 0x20;
      *(undefined2 *)(puVar1 + 0xf9) = 5000;
      *(undefined2 *)((int)puVar1 + 0x3e6) = 3000;
      *(undefined2 *)(puVar1 + 0xfa) = 5000;
      puVar1[0xd7] = puVar1[0xd7] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0xe7,&local_30);
    }
    puVar1 = _DAT_800acbdc;
    iVar3 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]));
    if (300 < iVar3) {
      puVar1[0x50] = 0x70;
      puVar1[0x51] = 0x20;
      *(undefined2 *)(puVar1 + 0x4d) = 6000;
      *(undefined2 *)(puVar1 + 0x4e) = 6000;
      puVar1[0x2b] = puVar1[0x2b] | 0x80;
      *(undefined2 *)((int)puVar1 + 0x136) = 3000;
      puVar1[0x2b] = puVar1[0x2b] | 0x2000;
      func_0x80019700(0x3000,0,puVar1 + 0x3b,&local_30);
    }
    puVar1 = _DAT_800acbdc;
    iVar3 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]));
    if (300 < iVar3) {
      puVar1[0x17d] = 0x70;
      puVar1[0x17e] = 0x20;
      *(undefined2 *)(puVar1 + 0x17a) = 6000;
      *(undefined2 *)(puVar1 + 0x17b) = 6000;
      puVar1[0x158] = puVar1[0x158] | 0x80;
      *(undefined2 *)((int)puVar1 + 0x5ea) = 3000;
      puVar1[0x158] = puVar1[0x158] | 0x2000;
      func_0x80019700(0x3000,0,puVar1 + 0x168,&local_30);
    }
  }
  cVar2 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + cVar2,_DAT_800aca58);
  return;
}



/* ============ FUN_8011b5c4 @ 0x8011b5c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b5c4(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined1 auStack_38 [20];
  uint local_24;
  uint local_1c;
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,auStack_38);
  iVar3 = iVar3 + param_2 * 0x204 + 0x204;
  func_0x80022da0(auStack_38,iVar3 + -0x140,auStack_38);
  func_0x80022da0(auStack_38,iVar3 + -0x94,auStack_38);
  func_0x80022da0(auStack_38,iVar3 + 0x18,auStack_38);
  if (param_1 == 0) {
    *(uint *)(_DAT_800ac784 + 0x34) =
         *(int *)(_DAT_800ac784 + 0x34) - (local_24 - *(int *)(iVar3 + 0x54));
    *(uint *)(_DAT_800ac784 + 0x3c) =
         *(int *)(_DAT_800ac784 + 0x3c) - (local_1c - *(int *)(iVar3 + 0x5c));
  }
  else {
    iVar2 = (int)(((local_24 & 0xffff) - (uint)*(ushort *)(iVar3 + 0x54)) * 0x10000) >> 0x10;
    iVar3 = (int)(((local_1c & 0xffff) - (uint)*(ushort *)(iVar3 + 0x5c)) * 0x10000) >> 0x10;
    uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  }
  return;
}



/* ============ FUN_8011b724 @ 0x8011b724 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b724(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011f4c4)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011b860 @ 0x8011b860 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b860(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f4c0;
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



/* ============ FUN_8011ba08 @ 0x8011ba08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ba08(void)

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
  (*(code *)(&PTR_FUN_8011f4d8)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011bad4 @ 0x8011bad4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bad4(void)

{
  (**(code **)(&DAT_8011f4dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f4fc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011bb50 @ 0x8011bb50 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bb50(void)

{
  (**(code **)(&DAT_8011f51c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f53c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011bbcc @ 0x8011bbcc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bbcc(void)

{
  (**(code **)(&DAT_8011f55c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011bc14 @ 0x8011bc14 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc14(void)

{
  (**(code **)(&DAT_8011f55c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011bc5c @ 0x8011bc5c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc5c(void)

{
  (**(code **)(&DAT_8011f55c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011bcf4 @ 0x8011bcf4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bcf4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011f594)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011be30 @ 0x8011be30 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011be30(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f590;
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



/* ============ FUN_8011bfd8 @ 0x8011bfd8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bfd8(void)

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
  (*(code *)(&PTR_FUN_8011f5a8)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011c0a4 @ 0x8011c0a4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c0a4(void)

{
  (**(code **)(&DAT_8011f5ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f5cc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011c120 @ 0x8011c120 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c120(void)

{
  (**(code **)(&DAT_8011f5ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f60c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011c19c @ 0x8011c19c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c19c(void)

{
  (**(code **)(&DAT_8011f62c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011c1e4 @ 0x8011c1e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c1e4(void)

{
  (**(code **)(&DAT_8011f62c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011c22c @ 0x8011c22c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c22c(void)

{
  (**(code **)(&DAT_8011f62c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011c2c4 @ 0x8011c2c4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c2c4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011f664)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011c3f4 @ 0x8011c3f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c3f4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f660;
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



/* ============ FUN_8011c59c @ 0x8011c59c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c59c(void)

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
  (*(code *)(&PTR_FUN_8011f678)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_8011c668 @ 0x8011c668 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c668(void)

{
  (**(code **)(&DAT_8011f67c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f69c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011c6e4 @ 0x8011c6e4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c6e4(void)

{
  (**(code **)(&DAT_8011f6bc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f6dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011c760 @ 0x8011c760 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c760(void)

{
  (**(code **)(&DAT_8011f6fc + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011c7a8 @ 0x8011c7a8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c7a8(void)

{
  (**(code **)(&DAT_8011f6fc + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011c7f0 @ 0x8011c7f0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c7f0(void)

{
  (**(code **)(&DAT_8011f6fc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011c888 @ 0x8011c888 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c888(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_8011f734)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_8011c9cc @ 0x8011c9cc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c9cc(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011f730;
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



/* ============ FUN_8011cb74 @ 0x8011cb74 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cb74(void)

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
    (*(code *)(&PTR_FUN_8011f748)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_8011cca0 @ 0x8011cca0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cca0(void)

{
  (**(code **)(&DAT_8011f750 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f770 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011cd1c @ 0x8011cd1c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cd1c(void)

{
  (**(code **)(&DAT_8011f790 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f7b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011cd98 @ 0x8011cd98 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cd98(void)

{
  (**(code **)(&DAT_8011f7d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_8011cde0 @ 0x8011cde0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cde0(void)

{
  (**(code **)(&DAT_8011f7e4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8011ce28 @ 0x8011ce28 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce28(void)

{
  (**(code **)(&DAT_8011f7e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_8011cec0 @ 0x8011cec0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cec0(void)

{
  _DAT_80072bec = &LAB_80100510;
  _DAT_80072bf0 = &LAB_80100510;
  _DAT_80072bf4 = &LAB_80100510;
  _DAT_80072c04 = &LAB_80100510;
  _DAT_80072c0c = &LAB_80100510;
  _DAT_80072c1c = &LAB_80100510;
  _DAT_80072c20 = &LAB_80100510;
  _DAT_80072c24 = &LAB_80100510;
  _DAT_80072c28 = &LAB_80100510;
  _DAT_80072c2c = FUN_8010c2d8;
  _DAT_80072c50 = FUN_80110b00;
  _DAT_80072c6c = FUN_80116230;
  _DAT_80072c84 = FUN_80116230;
  _DAT_80072cac = FUN_8011b724;
  _DAT_80072cb4 = FUN_8011bcf4;
  _DAT_80072cd0 = FUN_8011c2c4;
  _DAT_80072cd8 = FUN_8011c888;
  _DAT_800b52c8 = &DAT_8011d0f8;
  _DAT_800ac998 = &DAT_8011d2d8;
  _DAT_80072bf8 = FUN_8010a9b4;
  _DAT_800ac798 = &LAB_8010a378;
  _DAT_800ac79c = &LAB_8010a378;
  _DAT_800ac7a0 = &LAB_8010a378;
  _DAT_800ac7b0 = &LAB_8010a378;
  _DAT_800ac7b8 = &LAB_8010a378;
  _DAT_800ac7c8 = &LAB_8010a378;
  _DAT_800ac7cc = &LAB_8010a378;
  _DAT_800ac7d0 = &LAB_8010a378;
  _DAT_800ac7d4 = &LAB_8010a378;
  _DAT_800ac7d8 = &LAB_80110424;
  _DAT_800ac7fc = &LAB_80115d30;
  _DAT_800ac7a4 = &LAB_8010c258;
  _DAT_800ac818 = &LAB_8011a7c4;
  _DAT_800ac898 = &LAB_8010a7a4;
  _DAT_800ac89c = &LAB_8010a7a4;
  _DAT_800ac8a0 = &LAB_8010a7a4;
  _DAT_800ac8b0 = &LAB_8010a7a4;
  _DAT_800ac8b8 = &LAB_8010a7a4;
  _DAT_800ac8c8 = &LAB_8010a7a4;
  _DAT_800ac8cc = &LAB_8010a7a4;
  _DAT_800ac8d0 = &LAB_8010a7a4;
  _DAT_800ac8d4 = &LAB_8010a7a4;
  _DAT_800ac8d8 = &LAB_80110790;
  _DAT_800ac8fc = &LAB_80116064;
  _DAT_800ac8a4 = &LAB_8010c298;
  _DAT_800ac918 = &LAB_8011b168;
  func_0x80029afc();
  return;
}



