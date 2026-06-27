/* ======= FUN_80100000 @ 0x80100000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(void)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  byte bVar5;
  undefined4 uStack00000028;
  undefined4 uStack0000002c;
  undefined4 uStack00000030;
  undefined4 uStack00000034;
  
  uStack00000028 = DAT_80100014;
  uStack0000002c = DAT_80100018;
  uStack00000030 = DAT_8010001c;
  uStack00000034 = DAT_80100020;
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = DAT_8011e820;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011e874)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee1ef8);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&LAB_8011e814)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (**(code **)((uVar3 & 7) * 4 + -0x7fee1784))();
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
                    (uint)(byte)(&LAB_8011e834)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000028);
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_1)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_2)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_3)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e838)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e839)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e83a)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e83b)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
  }
  func_0x8001af5c(0,0,600,700);
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



/* ======= FUN_8010093c @ 0x8010093c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010093c(void)

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



/* ======= FUN_8010098c @ 0x8010098c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010098c(void)

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



/* ======= FUN_80100a70 @ 0x80100a70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a70(void)

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



/* ======= FUN_80100b58 @ 0x80100b58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100b58(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_1)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_2)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_3)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e838)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e839)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83a)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83b)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      if ((10000 < *(uint *)(_DAT_ffffc7a3 + 0x1d0)) &&
         (uVar1 = func_0x8001af20(), (uVar1 & 3) == 0)) {
        _DAT_800ac784[1] = 0x801;
      }
      if ((_DAT_800ac784[0x71] & 0x1000) != 0) {
        _DAT_800ac784[1] = 0x1001;
      }
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
  (**(code **)(&LAB_8011e89c + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80100fc0 @ 0x80100fc0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fc0(void)

{
  (**(code **)(&LAB_8011e8d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e920 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010103c @ 0x8010103c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010103c(void)

{
  (**(code **)(&LAB_8011e970 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e9b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801010b8 @ 0x801010b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010b8(void)

{
  (**(code **)(&LAB_8011e9f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e920 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101134 @ 0x80101134 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101134(void)

{
  (**(code **)(&LAB_8011ea40 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e9b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801011b0 @ 0x801011b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011b0(void)

{
  (**(code **)(&LAB_8011ea58 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ea5c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010122c @ 0x8010122c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010122c(void)

{
  (**(code **)(&LAB_8011ea60 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ea64 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801012a8 @ 0x801012a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801012a8(void)

{
  (**(code **)(&LAB_8011ea68 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ea64 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101324 @ 0x80101324 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101324(void)

{
  (**(code **)(&LAB_8011ea6c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ea64 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801013a0 @ 0x801013a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801013a0(void)

{
  (**(code **)(&LAB_8011ea70 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ea74 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010141c @ 0x8010141c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010141c(void)

{
  (**(code **)(&LAB_8011ea78 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011ea7c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101498 @ 0x80101498 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101498(void)

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



/* ======= FUN_801015b0 @ 0x801015b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801015b0(void)

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



/* ======= FUN_8010163c @ 0x8010163c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010163c(void)

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
    FUN_801094d0(0,1);
  }
  return;
}



/* ======= FUN_80101718 @ 0x80101718 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101718(void)

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



/* ======= FUN_80101824 @ 0x80101824 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101824(void)

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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80102170(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_8010198c @ 0x8010198c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010198c(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
    FUN_80102318();
    return;
  }
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
  return;
}



/* ======= FUN_80101b2c @ 0x80101b2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b2c(void)

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
    if (4 < DAT_800aca4e) {
      uVar5 = func_0x8001af20();
      *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011eb80)[uVar5 & 0x1f];
      FUN_80102464();
      return;
    }
    uVar5 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x1de) =
         *(undefined1 *)((int)&PTR_LAB_8011eb90 + (uVar5 & 0x1f));
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)
          (&DAT_8011ea80 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&DAT_8011ea82 +
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
          (&DAT_8011ea80 +
          *(char *)(_DAT_800ac784 + 0x9f) * 4 + (uint)*(byte *)(_DAT_800ac784 + 0x1de) * 0x80);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80102654(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80101e7c @ 0x80101e7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101e7c(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 9) {
                    /* WARNING: Jumptable at 0x80101eb8 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_80100024 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80102170 @ 0x80102170 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102170(short param_1,int param_2)

{
  char cVar1;
  short in_v1;
  
  *(short *)(param_2 + 0x9c) = (param_1 + -1) - in_v1;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    DAT_800aca5a = 4;
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if ((cVar1 != '\0') && (-1 < _DAT_800acaee)) {
    return;
  }
  *(uint *)(_DAT_800ac784 + 4) = (*(byte *)(_DAT_800ac784 + 5) + 2) * 0x100 | 1;
  FUN_80102cf0();
  return;
}



/* ======= FUN_80102318 @ 0x80102318 ======= */

void FUN_80102318(void)

{
  int in_v1;
  int iVar1;
  
  iVar1 = *(int *)(in_v1 + -0x387c);
  if (((((*(byte *)(iVar1 + 0x1c2) & 2) != 0) && (7 < *(byte *)(iVar1 + 0x95))) &&
      (iVar1 = *(int *)(iVar1 + 0x1ac), -1 < *(short *)(iVar1 + 0x9a))) &&
     (((*(byte *)(iVar1 + 9) & 0x80) == 0 && ((*(ushort *)(iVar1 + 0x1d8) & 1) == 0)))) {
    *(undefined4 *)(iVar1 + 4) = 0xb01;
    FUN_80102cf0();
    return;
  }
  return;
}



/* ======= FUN_80102464 @ 0x80102464 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102464(void)

{
  int in_v0;
  int in_v1;
  
  if (in_v0 == 0) {
    *(undefined1 *)(in_v1 + 6) = 8;
  }
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) =
       *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
  if (*(short *)(_DAT_800ac784 + 0x8c) < 10) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    FUN_80102cf0();
    return;
  }
  return;
}



/* ======= FUN_8010250c @ 0x8010250c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010250c(void)

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



/* ======= FUN_80102654 @ 0x80102654 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102654(void)

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



/* ======= FUN_801026fc @ 0x801026fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801026fc(void)

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



/* ======= FUN_80102850 @ 0x80102850 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102850(void)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if ((bVar1 & 3) != 0) {
    iVar3 = (int)*(short *)(_DAT_800ac784 + 0x6a);
    if ((((bVar1 & 0xf0) * 0x10 - iVar3) + 0x200 & 0xfff) < 0x400) {
      uVar2 = ((bVar1 & 1) + 9) * 0x100 | 1;
      *(uint *)(_DAT_800ac784 + 4) = uVar2;
      if (uVar2 == 0) {
        if ((*(ushort *)(iVar3 + 0x1d8) & 0x20) == 0) {
          FUN_801095a4(0,0);
        }
      }
      else if ((*(ushort *)(iVar3 + 0x1d8) & 0x40) == 0) {
        FUN_80103970(0,1);
        return;
      }
      return;
    }
  }
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar3 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar3 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar3 + 3) * 0x100 | 1;
  }
  return;
}



/* ======= FUN_80102948 @ 0x80102948 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102948(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  int extraout_v1;
  int iVar4;
  undefined4 uVar5;
  
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
      uVar5 = 5;
      if ((uVar3 & 1) != 0) {
        uVar5 = 4;
      }
      func_0x800453d0(uVar5);
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    iVar4 = *(int *)(extraout_v1 + -0x387c);
    if ((*(uint *)(iVar4 + 0x1d0) < 4000) && (*(char *)(iVar4 + 6) == '\0')) {
      *(undefined1 *)(iVar4 + 6) = 1;
      uVar2 = func_0x8001af20(0,1);
      *(ushort *)(_DAT_800ac784 + 0x9c) = uVar2 & 0xf;
      uVar3 = func_0x8001af20();
      if ((uVar3 & 3) == 0) {
        func_0x800453d0(5);
      }
    }
    return;
  }
  FUN_801094d0(0,0);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80102b18 @ 0x80102b18 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102b18(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 7) {
                    /* WARNING: Jumptable at 0x80102b50 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_8010004c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80102cf0 @ 0x80102cf0 ======= */

void FUN_80102cf0(void)

{
  return;
}



/* ======= FUN_80102cfc @ 0x80102cfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102cfc(void)

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
        FUN_8010371c();
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
      FUN_8010371c();
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
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  FUN_8010371c();
  return;
}



/* ======= FUN_80102f2c @ 0x80102f2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102f2c(void)

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



/* ======= FUN_80103010 @ 0x80103010 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103010(void)

{
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
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) == 0) {
        FUN_801095a4(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_80103970(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_80103190 @ 0x80103190 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103190(void)

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



/* ======= FUN_801032b4 @ 0x801032b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801032b4(void)

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



/* ======= FUN_80103330 @ 0x80103330 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103330(void)

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



/* ======= FUN_8010338c @ 0x8010338c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010338c(uint param_1)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int unaff_s1;
  char in_stack_00000030;
  
  uVar5 = _DAT_800ac784;
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  uVar3 = (uint)(uVar4 < 2);
  if (uVar4 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar4 = _DAT_800ac784;
    uVar3 = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  else if (uVar3 == 0) {
    if (uVar4 == 2) {
      uVar5 = *(uint *)(_DAT_800ac784 + 0x174);
      uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),uVar5,0,0x100);
      param_1 = _DAT_800ac784;
      uVar4 = *(byte *)(_DAT_800ac784 + 6) + uVar3;
      *(char *)(_DAT_800ac784 + 6) = (char)uVar4;
    }
    else {
      uVar3 = 0x201;
      if (uVar4 == 3) {
        *(undefined1 *)(_DAT_800ac784 + 9) = 0;
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
        return;
      }
    }
  }
  else if (uVar4 == 0) {
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = _DAT_800ac784;
    uVar3 = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
  }
  *(int *)(unaff_s1 + 0x54) = (int)*(short *)(unaff_s1 + 0x38) + *(int *)(unaff_s1 + 0x54);
  *(int *)(unaff_s1 + 0x58) = (int)*(short *)(unaff_s1 + 0x3a) + *(int *)(unaff_s1 + 0x58);
  *(int *)(unaff_s1 + 0x5c) = (int)*(short *)(unaff_s1 + 0x3c) + *(int *)(unaff_s1 + 0x5c);
  sVar2 = func_0x8001c2dc();
  if (in_stack_00000030 != '\0') {
    *(uint *)(unaff_s1 + 0x54) = uVar3;
    *(uint *)(unaff_s1 + 0x58) = uVar4;
    *(uint *)(unaff_s1 + 0x5c) = param_1;
    *(uint *)(unaff_s1 + 0x60) = uVar5;
    sVar2 = func_0x8001c2dc();
    *(short *)(unaff_s1 + 0x38) = -*(short *)(unaff_s1 + 0x38);
    *(short *)(unaff_s1 + 0x3c) = -*(short *)(unaff_s1 + 0x3c);
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a));
    func_0x800453d0(1);
  }
  *(short *)(unaff_s1 + 0x3a) = *(short *)(unaff_s1 + 0x3a) + 0x32;
  if ((int)sVar2 < *(int *)(unaff_s1 + 0x58)) {
    *(int *)(unaff_s1 + 0x58) = (int)sVar2;
    *(short *)(unaff_s1 + 0x38) = *(short *)(unaff_s1 + 0x38) >> 1;
    *(short *)(unaff_s1 + 0x3c) = *(short *)(unaff_s1 + 0x3c) >> 1;
    *(short *)(unaff_s1 + 0x3a) = -(*(short *)(unaff_s1 + 0x3a) >> 2);
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a));
    func_0x800453d0(1);
    cVar1 = *(char *)(unaff_s1 + 0x75);
    *(char *)(unaff_s1 + 0x75) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xfffb;
    }
  }
  return;
}



/* ======= FUN_801034c8 @ 0x801034c8 ======= */
// decompile failed: Exception while decompiling 801034c8: process: timeout


/* ======= FUN_8010371c @ 0x8010371c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010371c(void)

{
  byte bVar1;
  char cVar2;
  short in_v0;
  short sVar3;
  int iVar4;
  
  _DAT_800acaee = in_v0;
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x170),
                  *(undefined4 *)(_DAT_800ac784 + 0x174));
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar4 != 0) {
    _DAT_800acaee = _DAT_800acaee + -1;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x6b8;
    if (*(byte *)(_DAT_800ac784 + 8) - 0x13 < 2) {
      iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x764;
    }
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x40,&LAB_8011e864);
  }
  sVar3 = func_0x80037024();
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar3 * -5;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    DAT_800aca5a = 4;
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
  if ((cVar2 != '\0') && (-1 < _DAT_800acaee)) {
    return;
  }
  bVar1 = *(byte *)(_DAT_800ac784 + 5);
  *(byte *)(_DAT_800ac784 + 5) = bVar1 + 2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(*(int *)(bVar1 - 0x387a) + 0x8f) = 7;
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80104b6c();
  return;
}



/* ======= FUN_80103970 @ 0x80103970 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103970(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_8011e814)[*(byte *)(param_3 + 8)] * 0xac +
                   *(int *)(param_3 + 0x188));
  func_0x80019700(0x2000,(int)*(short *)(param_3 + 0x6a),puVar2 + 0x10,&stack0x00000010);
  puVar2[0x1a] = 0x6f;
  *(undefined2 *)(puVar2 + 0x25) = 0;
  *(undefined2 *)((int)puVar2 + 0x96) = 0;
  *(undefined2 *)(puVar2 + 0x26) = 0;
  *(undefined2 *)((int)puVar2 + 0x9a) = 0;
  *(undefined2 *)(puVar2 + 0x27) = 0;
  *(undefined2 *)((int)puVar2 + 0x9e) = 0;
  *puVar2 = *puVar2 | 0x4a;
  func_0x800453d0(9);
  *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104b6c();
  return;
}



/* ======= FUN_80103aac @ 0x80103aac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103aac(void)

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
    puVar3 = (uint *)((uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80103e7c @ 0x80103e7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103e7c(void)

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



/* ======= FUN_80103fa0 @ 0x80103fa0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103fa0(void)

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



/* ======= FUN_80104094 @ 0x80104094 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104094(void)

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



/* ======= FUN_8010413c @ 0x8010413c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010413c(void)

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



/* ======= FUN_8010421c @ 0x8010421c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010421c(void)

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
        FUN_80104b6c();
        return;
      }
      *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) & 0xffef;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      }
      return;
    }
    if (bVar2 != 0) {
      FUN_80104b6c();
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
  FUN_80104b6c();
  return;
}



/* ======= FUN_801042ac @ 0x801042ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801042ac(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104b6c();
  return;
}



/* ======= FUN_80104384 @ 0x80104384 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104384(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80104c5c();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      FUN_80104c5c();
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
  FUN_80104c5c();
  return;
}



/* ======= FUN_80104474 @ 0x80104474 ======= */
// decompile failed: Exception while decompiling 80104474: process: timeout


/* ======= FUN_801048b4 @ 0x801048b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801048b4(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  short unaff_s0;
  short unaff_s1;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
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
      goto LAB_80105248;
    }
    if (bVar2 != 0) goto LAB_80105248;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 1;
    }
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),
                          (int)*(char *)(_DAT_800ac784 + 0x9f),0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
LAB_80105248:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_8011ebe0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  FUN_80105b38(0,1);
  return;
}



/* ======= FUN_80104a60 @ 0x80104a60 ======= */
// decompile failed: Exception while decompiling 80104a60: process: timeout


/* ======= FUN_80104b6c @ 0x80104b6c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b6c(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_801094d0(0,0);
  iVar3 = (uint)(byte)(&LAB_8011e814)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(iVar3 + 100) = *(short *)(iVar3 + 100) + *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x80;
  func_0x80068098(iVar3 + 0x60,iVar3 + 0x18);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80104c5c @ 0x80104c5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104c5c(void)

{
  char cVar1;
  int iVar2;
  undefined1 in_v0;
  uint uVar3;
  int iVar4;
  int in_v1;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  *(undefined1 *)(in_v1 + 6) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 6) = 5;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 7) == 0) {
    uVar3 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar3 & 1) != 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
  }
  iVar6 = 0;
  iVar7 = 0x100;
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar4;
  iVar4 = (uint)(byte)(&LAB_8011e814)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + -0x10ec);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(iVar4 + 100) = *(short *)(iVar4 + 100) + *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x80;
  func_0x80068098(iVar4 + 0x60,iVar4 + 0x18);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80104e28 @ 0x80104e28 ======= */
// decompile failed: Exception while decompiling 80104e28: process: timeout


/* ======= FUN_80104f50 @ 0x80104f50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f50(void)

{
  short *psVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 0x90);
  if ((bVar2 & 3) != 0) {
    psVar1 = (short *)(_DAT_800ac784 + 0x6a);
    if ((((bVar2 & 0xf0) * 0x10 - (int)*psVar1) + 0x200 & 0xfff) < 0x400) {
      *(uint *)(_DAT_800ac784 + 4) = ((bVar2 & 1) + 9) * 0x100 | 1;
      *(undefined1 *)(*psVar1 + 0x93) = 1;
      uVar3 = func_0x8001af20();
      if ((uVar3 & 0xf) == 0) {
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
      }
      iVar4 = func_0x8001a780(&DAT_800aca54);
      if (iVar4 != 0) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
      }
      func_0x800245d8(0);
      return;
    }
  }
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) &&
     (iVar4 = func_0x8001a9cc(&DAT_800aca88,0x2c8), iVar4 != 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar4 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar4 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar4 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar4 + 3) * 0x100 | 1;
  }
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 0x5dc) && (_DAT_800acaee < 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x1000) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1001;
  }
  return;
}



/* ======= FUN_801050f0 @ 0x801050f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801050f0(void)

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
         *(undefined2 *)(&LAB_8011ebe0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_8011ebe0_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&LAB_8011ebe0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80105b38(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_801053c0 @ 0x801053c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801053c0(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee13e0))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_80106b6c();
  }
  else {
    FUN_801069d4();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105f3c:
    FUN_801094d0(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
    if (uVar3 == unaff_s1) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
    }
    if (uVar3 == 0) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    puVar4 = &DAT_800b0fe0;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_801094d0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105f3c;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106fd4();
  return;
}



/* ======= FUN_801054b0 @ 0x801054b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801054b0(void)

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
      else if (bVar2 != 3) goto LAB_80106160;
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
        if (4 < DAT_800aca4e) {
          uVar4 = func_0x8001af20();
          *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011eb80)[uVar4 & 0x1f];
          func_0x80019700();
          uVar4 = func_0x8001af20();
          if ((uVar4 & 1) == 0) {
            func_0x800453d0(6);
          }
          iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                                  *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x400);
          if (iVar3 != 0) {
            *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
            *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
            uVar4 = func_0x8001af20();
            if ((uVar4 & 1) == 0) {
              *(undefined1 *)(_DAT_800ac784 + 5) = 8;
            }
            iVar3 = func_0x8001a780(&DAT_800aca54);
            if (iVar3 != 0) {
              *(undefined1 *)(_DAT_800ac784 + 5) = 7;
            }
          }
          func_0x800245d8(*(char *)(_DAT_800ac784 + 0x9f) * -0x800 + 0x800);
          goto LAB_80106160;
        }
        uVar4 = func_0x8001af20();
        *(undefined1 *)(_DAT_800ac784 + 0x1de) =
             *(undefined1 *)((int)&PTR_LAB_8011eb90 + (uVar4 & 0x1f));
        if (*(char *)(_DAT_800ac784 + 0x1de) == '\x02') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0x13;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0x1f;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) =
             *(undefined2 *)
              (&DAT_8011ea80 +
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
      goto LAB_80105960;
    }
    if (bVar2 != 0) goto LAB_80106160;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
              *(int *)(_DAT_800ac784 + 0x188);
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0x40,&LAB_8011ef14);
    }
    if (*(char *)(_DAT_800ac784 + 6) == '\0') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(6);
    (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
LAB_80106160:
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
LAB_80105960:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010597c @ 0x8010597c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010597c(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int unaff_s2;
  uint *puVar9;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801063a0(0);
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      uVar4 = func_0x8001af20();
      if ((uVar4 & 7) == 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 8;
      }
      iVar5 = func_0x8001a780(&DAT_800aca54);
      if (iVar5 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      FUN_801094d0(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_801063a0(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_8011ef24)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011ef2c);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  }
  iVar8 = 0;
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  iVar6 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar5;
  *(char *)(_DAT_800ac784 + 7) = (char)iVar6;
  puVar9 = (uint *)((iVar5 * 0x10 + iVar6 * -5) * 4 + *(int *)(iVar8 + 0x188));
  func_0x80019700();
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar9 + 0x10);
  puVar9[0x1a] = 0x8f;
  *(undefined2 *)((int)puVar9 + 0x96) = 0xffce;
  *(undefined2 *)(puVar9 + 0x25) = 0;
  *(undefined2 *)(puVar9 + 0x26) = 0;
  *(undefined2 *)((int)puVar9 + 0x9a) = 3;
  *(undefined2 *)(puVar9 + 0x27) = 0;
  *(undefined2 *)((int)puVar9 + 0x9e) = 0;
  *puVar9 = *puVar9 | 0x4a;
  func_0x800453d0(9);
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x548) = 0x80;
  *(undefined4 *)(iVar5 + 0x54c) = 0x20;
  *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_801075e4(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011f20c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar5 = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar3 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    iVar5 = *(int *)(iVar5 + -0x387c);
    if (*(char *)(iVar5 + 0x95) == '#') {
      func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011f1fc);
    }
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    func_0x80022da0();
    if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
      func_0x80019d50(0x2000);
      func_0x80019d50(8,3,0x16,(uint)(byte)puVar9[-0x1ffb85e2] * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee1787) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee1786) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee1785) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      if (_DAT_800b0fe0 < 3) {
        FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
      FUN_80109298();
      return;
    }
    iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
    if (iVar5 == 0) {
      uVar4 = func_0x8001af20();
      uVar7 = 5;
      if ((uVar4 & 1) != 0) {
        uVar7 = 4;
      }
      func_0x800453d0(uVar7);
    }
    func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8)
    ;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
      FUN_80109410(0,1);
      return;
    }
    FUN_801094d0(0,0);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80105b38 @ 0x80105b38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105b38(int param_1)

{
  uint in_v0;
  int iVar1;
  
  *(uint *)(param_1 + 4) = in_v0;
  if ((in_v0 & 7) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  }
  iVar1 = func_0x8001a780(&DAT_800aca54);
  if (iVar1 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  FUN_801094d0(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_80105bc4 @ 0x80105bc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105bc4(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105d40:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105f3c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,1);
      uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
      if (uVar4 == unaff_s1) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,1);
        uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
      }
      if (uVar4 == 0) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,1);
      }
      func_0x8001af20();
      func_0x800453d0(5);
      puVar5 = &DAT_800b0fe0;
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      if (_DAT_800b0fe0 < 3) {
        FUN_801094d0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_8010673c;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010673c;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105d40;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010673c;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 != 0) {
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
      goto LAB_80105f3c;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105f3c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
    puVar5 = (undefined1 *)0x0;
LAB_8010673c:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106fd4();
    return;
  }
LAB_80105f3c:
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80105f58 @ 0x80105f58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105f58(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
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



/* ======= FUN_801061d4 @ 0x801061d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801061d4(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80106b58();
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
      FUN_80106b58();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011ef2c);
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
  FUN_80106b58();
  return;
}



/* ======= FUN_8010636c @ 0x8010636c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010636c(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  short sVar2;
  byte bVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  int unaff_s2;
  uint *puVar8;
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar6 != 1) {
    if (1 < uVar6) {
      iVar5 = 2;
      bVar1 = uVar6 == 2;
      uVar6 = 3;
      if (bVar1) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 1;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 3) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        }
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        return;
      }
      goto LAB_80106cc8;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106cc8;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef2c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar6 = func_0x8001af20();
    if ((uVar6 & 7) == 0) {
      func_0x800453d0(6);
    }
  }
  param_3 = 0;
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar5;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar6;
LAB_80106cc8:
  puVar8 = (uint *)((iVar5 * 0x10 + uVar6 * -5) * 4 + *(int *)(param_3 + 0x188));
  func_0x80019700();
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar8 + 0x10);
  puVar8[0x1a] = 0x8f;
  *(undefined2 *)((int)puVar8 + 0x96) = 0xffce;
  *(undefined2 *)(puVar8 + 0x25) = 0;
  *(undefined2 *)(puVar8 + 0x26) = 0;
  *(undefined2 *)((int)puVar8 + 0x9a) = 3;
  *(undefined2 *)(puVar8 + 0x27) = 0;
  *(undefined2 *)((int)puVar8 + 0x9e) = 0;
  *puVar8 = *puVar8 | 0x4a;
  func_0x800453d0(9);
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x548) = 0x80;
  *(undefined4 *)(iVar5 + 0x54c) = 0x20;
  *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_801075e4(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011f20c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar5 = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar4 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar4 + -1;
  if (cVar4 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011f1fc);
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)puVar8[-0x1ffb85e2] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee1787) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee1786) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee1785) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (2 < _DAT_800b0fe0) {
      func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
      FUN_80109298();
      return;
    }
    FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
  if (iVar5 == 0) {
    uVar6 = func_0x8001af20();
    uVar7 = 5;
    if ((uVar6 & 1) != 0) {
      uVar7 = 4;
    }
    func_0x800453d0(uVar7);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  FUN_80109410(0,1);
  return;
}



/* ======= FUN_801063a0 @ 0x801063a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801063a0(int param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  char cVar2;
  uint uVar3;
  int in_v1;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int unaff_s2;
  
  if (in_v1 == 2) {
    *(undefined4 *)(param_1 + 4) = param_2;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 3) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    return;
  }
  iVar4 = *(int *)(param_3 + 0x188);
  func_0x80019700(param_1,param_2,iVar4 + 0x84);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x84);
  *(undefined4 *)(iVar4 + 0xac) = 0x8f;
  *(undefined2 *)(iVar4 + 0xda) = 0xffce;
  *(undefined2 *)(iVar4 + 0xd8) = 0;
  *(undefined2 *)(iVar4 + 0xdc) = 0;
  *(undefined2 *)(iVar4 + 0xde) = 3;
  *(undefined2 *)(iVar4 + 0xe0) = 0;
  *(undefined2 *)(iVar4 + 0xe2) = 0;
  *(uint *)(iVar4 + 0x44) = *(uint *)(iVar4 + 0x44) | 0x4a;
  func_0x800453d0(9);
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x548) = 0x80;
  *(undefined4 *)(iVar5 + 0x54c) = 0x20;
  *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_801075e4(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011f20c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar5 = 0;
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011f1fc);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee1744) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee1743) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee1742) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee1741) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80109298();
    return;
  }
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar4;
  if (iVar4 == 0) {
    uVar3 = func_0x8001af20();
    uVar6 = 5;
    if ((uVar3 & 1) != 0) {
      uVar6 = 4;
    }
    func_0x800453d0(uVar6);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_801064d8 @ 0x801064d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801064d8(void)

{
  short sVar1;
  char cVar2;
  uint uVar3;
  uint extraout_v1;
  int iVar4;
  undefined4 uVar5;
  int unaff_s0;
  int unaff_s2;
  uint *unaff_s3;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee10c4))();
    return;
  }
  FUN_80107de4();
  *unaff_s3 = extraout_v1 | 0x4a;
  func_0x800453d0();
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_801075e4(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011f20c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar4 = 0;
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_8011f1fc);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e838)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e839)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83a)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83b)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80109298();
    return;
  }
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar4;
  if (iVar4 == 0) {
    uVar3 = func_0x8001af20();
    uVar5 = 5;
    if ((uVar3 & 1) != 0) {
      uVar5 = 4;
    }
    func_0x800453d0(uVar5);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_8010654c @ 0x8010654c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010654c(void)

{
  byte bVar1;
  char cVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80106fd4();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011f1fc);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_801094d0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80106fd4();
  return;
}



/* ======= FUN_80106810 @ 0x80106810 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106810(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80107334();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_801094d0(0,1);
      FUN_80107dd0();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
    puVar5 = (uint *)((uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011f1fc);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011f1fc);
    puVar5[0x1a] = 0x8f;
    *(undefined2 *)((int)puVar5 + 0x96) = 0xffce;
    *(undefined2 *)(puVar5 + 0x25) = 0;
    *(undefined2 *)(puVar5 + 0x26) = 0;
    *(undefined2 *)((int)puVar5 + 0x9a) = 3;
    *(undefined2 *)(puVar5 + 0x27) = 0;
    *(undefined2 *)((int)puVar5 + 0x9e) = 0;
    *puVar5 = *puVar5 | 0x4a;
    func_0x800453d0(9);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0x548) = 0x80;
    *(undefined4 *)(iVar4 + 0x54c) = 0x20;
    *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
    if (_DAT_800b0fe0 < 3) {
      FUN_801072a0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80107334();
  return;
}



/* ======= FUN_801069d4 @ 0x801069d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801069d4(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  int iVar2;
  undefined1 unaff_s2;
  uint *unaff_s3;
  
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a));
  unaff_s3[0x1a] = 0x8f;
  *(undefined2 *)((int)unaff_s3 + 0x96) = 0xffce;
  *(undefined2 *)(unaff_s3 + 0x25) = 0;
  *(undefined2 *)(unaff_s3 + 0x26) = 0;
  *(undefined2 *)((int)unaff_s3 + 0x9a) = 3;
  *(undefined2 *)(unaff_s3 + 0x27) = 0;
  *(undefined2 *)((int)unaff_s3 + 0x9e) = 0;
  *unaff_s3 = *unaff_s3 | 0x4a;
  func_0x800453d0(9);
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = unaff_s2;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x548) = 0x80;
  *(undefined4 *)(iVar2 + 0x54c) = 0x20;
  *(uint *)(iVar2 + 0x4b4) = *(uint *)(iVar2 + 0x4b4) | 0x80;
  if (_DAT_800b0fe0 < 3) {
    FUN_801072a0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80107334();
  return;
}



/* ======= FUN_80106b58 @ 0x80106b58 ======= */

void FUN_80106b58(void)

{
  return;
}



/* ======= FUN_80106b6c @ 0x80106b6c ======= */

void FUN_80106b6c(void)

{
  return;
}



/* ======= FUN_80106b78 @ 0x80106b78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106b78(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *unaff_s0;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x28;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar2 & 0x3f) + 0x3c;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
    puVar7 = (uint *)((uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    unaff_s0 = puVar7 + 0x10;
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s0,&LAB_8011f1fc);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s0,&LAB_8011f1fc);
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
      FUN_801075e4(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011f20c);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar5 = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar3 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_801094d0(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011e814)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011f1fc);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x80022da0();
  if ((_DAT_00000083 & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)unaff_s0[-0x1ffb85f2] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c7) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c6) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c5) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    FUN_80109298();
    return;
  }
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
  if (iVar5 == 0) {
    uVar4 = func_0x8001af20();
    uVar6 = 5;
    if ((uVar4 & 1) != 0) {
      uVar6 = 4;
    }
    func_0x800453d0(uVar6);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80106f68 @ 0x80106f68 ======= */
// decompile failed: Exception while decompiling 80106f68: process: timeout


/* ======= FUN_80106fd4 @ 0x80106fd4 ======= */
// decompile failed: Exception while decompiling 80106fd4: process: timeout


/* ======= FUN_801072a0 @ 0x801072a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801072a0(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_801094d0(0,1);
    FUN_80107dd0();
    return;
  }
  FUN_801094d0(0,0);
  FUN_80107dd0();
  return;
}



/* ======= FUN_80107334 @ 0x80107334 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107334(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_801094d0(0,1);
  FUN_80107dd0();
  return;
}



/* ======= FUN_801075e4 @ 0x801075e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801075e4(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 *unaff_s0;
  int unaff_s1;
  int unaff_s2;
  undefined4 in_stack_00000028;
  undefined4 in_stack_0000002c;
  undefined4 in_stack_00000030;
  undefined4 in_stack_00000034;
  undefined4 in_stack_00000038;
  undefined4 in_stack_0000003c;
  undefined4 in_stack_00000040;
  undefined4 in_stack_00000044;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\x01') {
LAB_80107700:
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
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
    }
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    iVar2 = _DAT_800ac784;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  }
  else {
    unaff_s0 = (undefined4 *)0x1;
    iVar2 = _DAT_800ac784;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
      func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011f1fc);
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      if (2 < _DAT_800b0fe0) {
        func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        goto LAB_80107700;
      }
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011f1fc)
        ;
        if (*(char *)(unaff_s1 + 0x529) != '\0') {
          *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
        }
      }
      if (*(short *)(unaff_s2 + 0x82) == 0) {
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
        goto LAB_801088b0;
      }
      *(short *)(unaff_s2 + 0x82) = *(short *)(unaff_s2 + 0x82) + -1;
      in_stack_00000028 = _DAT_80072d4c;
      in_stack_0000002c = _DAT_80072d50;
      in_stack_00000030 = _DAT_80072d54;
      in_stack_00000034 = _DAT_80072d58;
      in_stack_00000038 = _DAT_80072d5c;
      in_stack_0000003c = _DAT_80072d60;
      in_stack_00000040 = _DAT_80072d64;
      in_stack_00000044 = _DAT_80072d68;
      unaff_s0 = &stack0x00000028;
      func_0x80065b70(0x80,unaff_s0);
      iVar2 = unaff_s2 + 0x40;
    }
  }
  func_0x80022da0(iVar2);
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) == 0) {
    iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar2;
    if (iVar2 == 0) {
      uVar3 = func_0x8001af20();
      uVar4 = 5;
      if ((uVar3 & 1) != 0) {
        uVar4 = 4;
      }
      func_0x800453d0(uVar4);
    }
    func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8)
    ;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      FUN_801094d0(0,0);
      return;
    }
    FUN_80109410(0,1);
    return;
  }
  func_0x80019d50(0x2000);
LAB_801088b0:
  func_0x80019d50();
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c7) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c6) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c5) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  if (2 < _DAT_800b0fe0) {
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109298();
    return;
  }
  FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  return;
}



/* ======= FUN_80107814 @ 0x80107814 ======= */
// decompile failed: Exception while decompiling 80107814: process: timeout


/* ======= FUN_80107dd0 @ 0x80107dd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107dd0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  int in_v0;
  int iVar2;
  uint uVar3;
  int in_v1;
  short sVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int *unaff_s0;
  int unaff_s1;
  undefined4 uStack00000020;
  undefined4 uStack00000024;
  undefined4 uStack00000028;
  undefined4 uStack0000002c;
  int in_stack_00000040;
  undefined4 in_stack_00000044;
  undefined4 in_stack_00000048;
  undefined4 in_stack_0000004c;
  undefined4 in_stack_00000050;
  undefined4 in_stack_00000054;
  undefined4 in_stack_00000058;
  undefined4 in_stack_0000005c;
  
  uStack0000002c = *(undefined4 *)(in_v0 + 0x40);
  uVar5 = *(undefined4 *)(unaff_s1 + 0x50c);
  uVar6 = *(undefined4 *)(unaff_s1 + 0x510);
  uVar7 = *(undefined4 *)(unaff_s1 + 0x514);
  *(undefined4 *)(in_v1 + 0x34) = *(undefined4 *)(unaff_s1 + 0x508);
  *(undefined4 *)(in_v1 + 0x38) = uVar5;
  *(undefined4 *)(in_v1 + 0x3c) = uVar6;
  *(undefined4 *)(in_v1 + 0x40) = uVar7;
  uStack00000020 = param_1;
  uStack00000024 = param_2;
  uStack00000028 = param_3;
  func_0x8002b498(*unaff_s0);
  func_0x8002aec4(unaff_s0 + 0xb4,*unaff_s0);
  func_0x8002b544();
  func_0x8003b0a4(*unaff_s0 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x38);
  uVar6 = *(undefined4 *)(_DAT_800ac784 + 0x3c);
  uVar7 = *(undefined4 *)(_DAT_800ac784 + 0x40);
  *(undefined4 *)(unaff_s1 + 0x508) = *(undefined4 *)(_DAT_800ac784 + 0x34);
  *(undefined4 *)(unaff_s1 + 0x50c) = uVar5;
  *(undefined4 *)(unaff_s1 + 0x510) = uVar6;
  *(undefined4 *)(unaff_s1 + 0x514) = uVar7;
  iVar2 = _DAT_800ac784;
  *(undefined4 *)(_DAT_800ac784 + 0x34) = uStack00000020;
  *(undefined4 *)(iVar2 + 0x38) = uStack00000024;
  *(undefined4 *)(iVar2 + 0x3c) = uStack00000028;
  *(undefined4 *)(iVar2 + 0x40) = uStack0000002c;
  cVar1 = *(char *)(unaff_s1 + 0x529);
  *(short *)(unaff_s1 + 0x4f0) = *(short *)(unaff_s1 + 0x4f0) >> 1;
  *(short *)(unaff_s1 + 0x4ec) = *(short *)(unaff_s1 + 0x4ec) >> 1;
  if (cVar1 < '\x04') {
    if (cVar1 == '\0') {
      *(undefined1 *)(unaff_s1 + 0x52a) = 0;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0;
    }
    sVar4 = (short)*(char *)(unaff_s1 + 0x52a) + *(short *)(unaff_s1 + 0x4ee);
    *(int *)(unaff_s1 + 0x50c) = (int)*(short *)(unaff_s1 + 0x4ee) + *(int *)(unaff_s1 + 0x50c);
    *(short *)(unaff_s1 + 0x4ee) = sVar4;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
      *(short *)(unaff_s1 + 0x4ee) = -(sVar4 >> 2);
      *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011f1fc);
      if (*(char *)(unaff_s1 + 0x529) != '\0') {
        *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
      }
    }
    if (*(short *)(unaff_s1 + 0x536) == 0) {
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
    }
    else {
      *(short *)(unaff_s1 + 0x536) = *(short *)(unaff_s1 + 0x536) + -1;
      in_stack_00000040 = _DAT_80072d4c;
      in_stack_00000044 = _DAT_80072d50;
      in_stack_00000048 = _DAT_80072d54;
      in_stack_0000004c = _DAT_80072d58;
      in_stack_00000050 = _DAT_80072d5c;
      in_stack_00000054 = _DAT_80072d60;
      in_stack_00000058 = _DAT_80072d64;
      in_stack_0000005c = _DAT_80072d68;
      unaff_s0 = &stack0x00000040;
      func_0x80065b70(0x80,unaff_s0);
      func_0x80022da0(unaff_s1 + 0x4f4,unaff_s0,unaff_s1 + 0x4f4);
      if ((*(ushort *)(unaff_s1 + 0x536) & 1) == 0) goto LAB_801080b0;
      func_0x80019d50(0x2000);
    }
    func_0x80019d50();
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c7) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c6) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c5) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109298();
    return;
  }
  *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
  if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011f1fc);
  }
LAB_801080b0:
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar2;
  if (iVar2 == 0) {
    uVar3 = func_0x8001af20();
    uVar5 = 5;
    if ((uVar3 & 1) != 0) {
      uVar5 = 4;
    }
    func_0x800453d0(uVar5);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80107de4 @ 0x80107de4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107de4(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int in_v1;
  short sVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int *unaff_s0;
  int unaff_s1;
  undefined4 in_stack_00000020;
  undefined4 in_stack_00000024;
  undefined4 in_stack_00000028;
  undefined4 in_stack_0000002c;
  int in_stack_00000040;
  undefined4 in_stack_00000044;
  undefined4 in_stack_00000048;
  undefined4 in_stack_0000004c;
  undefined4 in_stack_00000050;
  undefined4 in_stack_00000054;
  undefined4 in_stack_00000058;
  undefined4 in_stack_0000005c;
  
  uVar5 = *(undefined4 *)(unaff_s1 + 0x50c);
  uVar6 = *(undefined4 *)(unaff_s1 + 0x510);
  uVar7 = *(undefined4 *)(unaff_s1 + 0x514);
  *(undefined4 *)(in_v1 + 0x34) = *(undefined4 *)(unaff_s1 + 0x508);
  *(undefined4 *)(in_v1 + 0x38) = uVar5;
  *(undefined4 *)(in_v1 + 0x3c) = uVar6;
  *(undefined4 *)(in_v1 + 0x40) = uVar7;
  func_0x8002b498(*unaff_s0);
  func_0x8002aec4(unaff_s0 + 0xb4,*unaff_s0);
  func_0x8002b544();
  func_0x8003b0a4(*unaff_s0 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x38);
  uVar6 = *(undefined4 *)(_DAT_800ac784 + 0x3c);
  uVar7 = *(undefined4 *)(_DAT_800ac784 + 0x40);
  *(undefined4 *)(unaff_s1 + 0x508) = *(undefined4 *)(_DAT_800ac784 + 0x34);
  *(undefined4 *)(unaff_s1 + 0x50c) = uVar5;
  *(undefined4 *)(unaff_s1 + 0x510) = uVar6;
  *(undefined4 *)(unaff_s1 + 0x514) = uVar7;
  iVar2 = _DAT_800ac784;
  *(undefined4 *)(_DAT_800ac784 + 0x34) = in_stack_00000020;
  *(undefined4 *)(iVar2 + 0x38) = in_stack_00000024;
  *(undefined4 *)(iVar2 + 0x3c) = in_stack_00000028;
  *(undefined4 *)(iVar2 + 0x40) = in_stack_0000002c;
  cVar1 = *(char *)(unaff_s1 + 0x529);
  *(short *)(unaff_s1 + 0x4f0) = *(short *)(unaff_s1 + 0x4f0) >> 1;
  *(short *)(unaff_s1 + 0x4ec) = *(short *)(unaff_s1 + 0x4ec) >> 1;
  if (cVar1 < '\x04') {
    if (cVar1 == '\0') {
      *(undefined1 *)(unaff_s1 + 0x52a) = 0;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0;
    }
    sVar4 = (short)*(char *)(unaff_s1 + 0x52a) + *(short *)(unaff_s1 + 0x4ee);
    *(int *)(unaff_s1 + 0x50c) = (int)*(short *)(unaff_s1 + 0x4ee) + *(int *)(unaff_s1 + 0x50c);
    *(short *)(unaff_s1 + 0x4ee) = sVar4;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
      *(short *)(unaff_s1 + 0x4ee) = -(sVar4 >> 2);
      *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011f1fc);
      if (*(char *)(unaff_s1 + 0x529) != '\0') {
        *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
      }
    }
    if (*(short *)(unaff_s1 + 0x536) == 0) {
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
    }
    else {
      *(short *)(unaff_s1 + 0x536) = *(short *)(unaff_s1 + 0x536) + -1;
      in_stack_00000040 = _DAT_80072d4c;
      in_stack_00000044 = _DAT_80072d50;
      in_stack_00000048 = _DAT_80072d54;
      in_stack_0000004c = _DAT_80072d58;
      in_stack_00000050 = _DAT_80072d5c;
      in_stack_00000054 = _DAT_80072d60;
      in_stack_00000058 = _DAT_80072d64;
      in_stack_0000005c = _DAT_80072d68;
      unaff_s0 = &stack0x00000040;
      func_0x80065b70(0x80,unaff_s0);
      func_0x80022da0(unaff_s1 + 0x4f4,unaff_s0,unaff_s1 + 0x4f4);
      if ((*(ushort *)(unaff_s1 + 0x536) & 1) == 0) goto LAB_801080b0;
      func_0x80019d50(0x2000);
    }
    func_0x80019d50();
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c7) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c6) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee17c5) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109298();
    return;
  }
  *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
  if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011f1fc);
  }
LAB_801080b0:
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar2;
  if (iVar2 == 0) {
    uVar3 = func_0x8001af20();
    uVar5 = 5;
    if ((uVar3 & 1) != 0) {
      uVar5 = 4;
    }
    func_0x800453d0(uVar5);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_801083f0 @ 0x801083f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801083f0(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  int unaff_s0;
  undefined4 uStackX_c;
  int in_stack_00000014;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    iVar4 = _DAT_800ac784;
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
        *(undefined4 *)(_DAT_800ac784 + 4) = 1;
        *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x1e;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        return;
      }
      goto LAB_80108e24;
    }
    unaff_s0 = 1;
    if (bVar1 != 0) goto LAB_80108e24;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    FUN_80108e38(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108e38(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108e38(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108e38(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0x140) = 0x80;
    *(undefined4 *)(iVar4 + 0x144) = 0x20;
    *(uint *)(iVar4 + 0xac) = *(uint *)(iVar4 + 0xac) | 0x80;
    unaff_s0 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(unaff_s0 + 0x344) = 0x80;
    *(undefined4 *)(unaff_s0 + 0x348) = 0x20;
    *(uint *)(unaff_s0 + 0x2b0) = *(uint *)(unaff_s0 + 0x2b0) | 0x80;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) != 0) {
      *(undefined4 *)(unaff_s0 + 0x2b0) = 0;
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
  iVar4 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_80108e24:
  func_0x80022da0(iVar4,unaff_s0 + 0x18,iVar4);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (uStackX_c - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_80108638 @ 0x80108638 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108638(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&LAB_8011f1fc);
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



/* ======= FUN_801086a8 @ 0x801086a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801086a8(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80109298();
        return;
      }
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *(undefined2 *)(_DAT_800ac784 + 1) = 7;
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        FUN_801094d0(0,0);
      }
      return;
    }
    if (bVar1 != 0) {
      FUN_80109298();
      return;
    }
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    uVar3 = func_0x8001af20();
    uVar4 = 8;
    if ((uVar3 & 1) != 0) {
      uVar4 = 5;
    }
    func_0x800453d0(uVar4);
    *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
    uVar3 = (ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_1)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_2)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_3)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e838)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e839)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83a)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83b)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  FUN_80109298();
  return;
}



/* ======= FUN_80108ad0 @ 0x80108ad0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108ad0(void)

{
  (**(code **)(&LAB_8011f21c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80108b18 @ 0x80108b18 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b18(void)

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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109410(0,1);
    return;
  }
  FUN_801094d0(0,0);
  return;
}



/* ======= FUN_80108c28 @ 0x80108c28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108c28(void)

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
    FUN_801094d0(0,1);
  }
  return;
}



/* ======= FUN_80108cd0 @ 0x80108cd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108cd0(undefined4 param_1,int param_2)

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



/* ======= FUN_80108da4 @ 0x80108da4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108da4(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int iStack_1c;
  int iStack_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x4cc,auStack_30);
  iVar1 = iVar1 + param_2 * 0x204 + 0x6b8;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (iStack_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (iStack_14 - *(int *)(iVar1 + 0x5c));
  return;
}



/* ======= FUN_80108e38 @ 0x80108e38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108e38(undefined4 param_1,int param_2)

{
  int unaff_s0;
  int in_stack_00000024;
  int in_stack_0000002c;
  
  *(int *)(param_2 + 0x34) =
       *(int *)(param_2 + 0x34) - (in_stack_00000024 - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_0000002c - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_80108e88 @ 0x80108e88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108e88(void)

{
  if (*(byte *)(_DAT_800ac784 + 5) < 8) {
                    /* WARNING: Jumptable at 0x80108ec8 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_80100144 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80109234 @ 0x80109234 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109234(void)

{
  ushort uVar1;
  
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x100) != 0) {
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar1 & 0xff) + 100;
    FUN_80109ea8();
    return;
  }
  return;
}



/* ======= FUN_80109298 @ 0x80109298 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109298(int param_1)

{
  int in_v0;
  undefined2 in_v1;
  
  *(undefined2 *)(in_v0 + 0x9c) = in_v1;
  if (param_1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  *(short *)(_DAT_800ac784 + 0x166) = *(short *)(_DAT_800ac784 + 0x166) + -2;
  if (*(short *)(_DAT_800ac784 + 0x166) < 100) {
    *(undefined2 *)(_DAT_800ac784 + 0x166) = 100;
    FUN_80109eac();
    return;
  }
  return;
}



/* ======= FUN_80109410 @ 0x80109410 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109410(void)

{
  uint *puVar1;
  int iVar2;
  int unaff_s0;
  
  func_0x80019d50();
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e838)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e839)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83a)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83b)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  puVar1 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar1[0x25] = 0;
  puVar1[0x26] = 0;
  *puVar1 = *puVar1 | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x548) = 0;
  *(undefined4 *)(iVar2 + 0x54c) = 0;
  *(uint *)(iVar2 + 0x4b4) = *(uint *)(iVar2 + 0x4b4) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x5f4) = 0;
  *(undefined4 *)(iVar2 + 0x5f8) = 0;
  *(uint *)(iVar2 + 0x560) = *(uint *)(iVar2 + 0x560) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x9fc) = 0;
  *(undefined4 *)(iVar2 + 0xa00) = 0;
  *(uint *)(iVar2 + 0x968) = *(uint *)(iVar2 + 0x968) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x7f8) = 0;
  *(undefined4 *)(iVar2 + 0x7fc) = 0;
  *(uint *)(iVar2 + 0x764) = *(uint *)(iVar2 + 0x764) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar2 + 0x60c) = *(uint *)(iVar2 + 0x60c) | 0x80;
  *(undefined4 *)(iVar2 + 0x6a0) = 0;
  *(undefined4 *)(iVar2 + 0x6a4) = 0;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x8a4) = 0;
  *(undefined4 *)(iVar2 + 0x8a8) = 0;
  *(uint *)(iVar2 + 0x810) = *(uint *)(iVar2 + 0x810) | 0x80;
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  return;
}



/* ======= FUN_801094d0 @ 0x801094d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801094d0(void)

{
  uint *puVar1;
  int iVar2;
  int unaff_s0;
  
  func_0x80019d50();
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83a)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83b)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  puVar1 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar1[0x25] = 0;
  puVar1[0x26] = 0;
  *puVar1 = *puVar1 | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x548) = 0;
  *(undefined4 *)(iVar2 + 0x54c) = 0;
  *(uint *)(iVar2 + 0x4b4) = *(uint *)(iVar2 + 0x4b4) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x5f4) = 0;
  *(undefined4 *)(iVar2 + 0x5f8) = 0;
  *(uint *)(iVar2 + 0x560) = *(uint *)(iVar2 + 0x560) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x9fc) = 0;
  *(undefined4 *)(iVar2 + 0xa00) = 0;
  *(uint *)(iVar2 + 0x968) = *(uint *)(iVar2 + 0x968) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x7f8) = 0;
  *(undefined4 *)(iVar2 + 0x7fc) = 0;
  *(uint *)(iVar2 + 0x764) = *(uint *)(iVar2 + 0x764) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar2 + 0x60c) = *(uint *)(iVar2 + 0x60c) | 0x80;
  *(undefined4 *)(iVar2 + 0x6a0) = 0;
  *(undefined4 *)(iVar2 + 0x6a4) = 0;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x8a4) = 0;
  *(undefined4 *)(iVar2 + 0x8a8) = 0;
  *(uint *)(iVar2 + 0x810) = *(uint *)(iVar2 + 0x810) | 0x80;
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  return;
}



/* ======= FUN_801095a4 @ 0x801095a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801095a4(void)

{
  undefined4 *in_v0;
  int iVar1;
  undefined4 in_v1;
  
  *in_v0 = in_v1;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar1 + 0x548) = 0;
  *(undefined4 *)(iVar1 + 0x54c) = 0;
  *(uint *)(iVar1 + 0x4b4) = *(uint *)(iVar1 + 0x4b4) | 0x80;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar1 + 0x5f4) = 0;
  *(undefined4 *)(iVar1 + 0x5f8) = 0;
  *(uint *)(iVar1 + 0x560) = *(uint *)(iVar1 + 0x560) | 0x80;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar1 + 0x9fc) = 0;
  *(undefined4 *)(iVar1 + 0xa00) = 0;
  *(uint *)(iVar1 + 0x968) = *(uint *)(iVar1 + 0x968) | 0x80;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar1 + 0x7f8) = 0;
  *(undefined4 *)(iVar1 + 0x7fc) = 0;
  *(uint *)(iVar1 + 0x764) = *(uint *)(iVar1 + 0x764) | 0x80;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar1 + 0x60c) = *(uint *)(iVar1 + 0x60c) | 0x80;
  *(undefined4 *)(iVar1 + 0x6a0) = 0;
  *(undefined4 *)(iVar1 + 0x6a4) = 0;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar1 + 0x8a4) = 0;
  *(undefined4 *)(iVar1 + 0x8a8) = 0;
  *(uint *)(iVar1 + 0x810) = *(uint *)(iVar1 + 0x810) | 0x80;
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  return;
}



/* ======= FUN_801096cc @ 0x801096cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801096cc(void)

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



/* ======= FUN_80109780 @ 0x80109780 ======= */
// decompile failed: Exception while decompiling 80109780: process: timeout


/* ======= FUN_80109b00 @ 0x80109b00 ======= */

void FUN_80109b00(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109b3c @ 0x80109b3c ======= */

void FUN_80109b3c(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109b78 @ 0x80109b78 ======= */

void FUN_80109b78(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ======= FUN_80109b9c @ 0x80109b9c ======= */

void FUN_80109b9c(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ======= FUN_80109ea8 @ 0x80109ea8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109ea8(void)

{
  int iVar1;
  
  func_0x80019700();
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&LAB_8011f248 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    if (DAT_800acaf3 == '\0') {
      _DAT_800acabe = _DAT_800acabe + 0x800;
    }
    if (DAT_800aca59 == '\x03') {
      _DAT_800acabe = _DAT_800acabe + -0x800;
    }
    _DAT_800aca58 = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    _DAT_800aca3c = _DAT_800aca3c & 0xffffff3f;
  }
  return;
}



/* ======= FUN_80109eac @ 0x80109eac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109eac(void)

{
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&LAB_8011f248 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    if (DAT_800acaf3 == '\0') {
      _DAT_800acabe = _DAT_800acabe + 0x800;
    }
    if (DAT_800aca59 == '\x03') {
      _DAT_800acabe = _DAT_800acabe + -0x800;
    }
    _DAT_800aca58 = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    _DAT_800aca3c = _DAT_800aca3c & 0xffffff3f;
  }
  return;
}



/* ======= FUN_8010a02c @ 0x8010a02c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a02c(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a9ec();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a9ec();
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011f258
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011f258
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8010a9ec();
  return;
}



/* ======= FUN_8010a1fc @ 0x8010a1fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a1fc(void)

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
    (**(code **)(&LAB_8011f298 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0) {
      *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xed;
    }
    func_0x8001b4e4();
    FUN_801042ac();
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



/* ======= FUN_8010a460 @ 0x8010a460 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a460(void)

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
  _DAT_800ac784[0x1e] = DAT_8011e820;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011e874)[uVar3 & 7];
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
                    (uint)(byte)(&LAB_8011e834)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &local_38);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011e834_3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e838)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e839)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e83a)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011e83b)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011e864);
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



/* ======= FUN_8010a9ec @ 0x8010a9ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a9ec(void)

{
  byte bVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
  bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0xf;
  if ((bVar1 == 1) || (bVar1 == 3)) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                  0x200);
  bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0xf;
  if (((bVar1 == 4) || (bVar1 == 7)) || (bVar1 == 9)) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
  }
  bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0xf;
  if (((bVar1 == 5) || (bVar1 == 8)) || (bVar1 == 10)) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
  }
  if (*(char *)(_DAT_800ac784 + 9) == '\x06') {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x20c00;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  return;
}



/* ======= FUN_8010aba8 @ 0x8010aba8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aba8(void)

{
  uint *puVar1;
  undefined2 uVar2;
  uint uVar3;
  uint unaff_s0;
  int iVar4;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar4 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_1)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_2)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_3)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e838)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e839)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83a)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83b)[iVar4] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      puVar1 = _DAT_800ac784;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      *(undefined1 *)((int)puVar1 + 0x1d7) = 0x1f;
      func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),
                      0x10);
      func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010c1bc(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                                  (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]));
          *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
          (**(code **)(&LAB_8011fa5c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
          (**(code **)(&LAB_8011fa9c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
            return;
          }
          *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
          FUN_8010ca2c();
          return;
        }
      }
      return;
    }
  }
  uVar3 = func_0x8001bc08();
  if ((uVar3 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar3 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
  }
  (**(code **)(&LAB_8011f2c0 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010b008 @ 0x8010b008 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b008(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (**(code **)(&LAB_8011f2f4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f338 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b0b8 @ 0x8010b0b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b0b8(void)

{
  (**(code **)(&LAB_8011f37c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f398 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b134 @ 0x8010b134 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b134(void)

{
  (**(code **)(&LAB_8011f3b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f338 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b1b0 @ 0x8010b1b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b1b0(void)

{
  (**(code **)(&LAB_8011f3ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f398 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b22c @ 0x8010b22c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b22c(void)

{
  (**(code **)(&DAT_8011f404 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f408 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b2a8 @ 0x8010b2a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b2a8(void)

{
  (**(code **)(&DAT_8011f40c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f410 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b324 @ 0x8010b324 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b324(void)

{
  (*(code *)(&PTR_LAB_8011f414)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&DAT_8011f410 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b3a0 @ 0x8010b3a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b3a0(void)

{
  (*(code *)(&PTR_LAB_8011f418)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&DAT_8011f410 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b41c @ 0x8010b41c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b41c(void)

{
  (**(code **)(&LAB_8011f41c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f420 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b498 @ 0x8010b498 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b498(void)

{
  (**(code **)(&LAB_8011f424 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f428 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b51c @ 0x8010b51c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b51c(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
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
      else if (bVar1 != 3) goto LAB_8010bf6c;
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
    if (bVar1 != 0) goto LAB_8010bf6c;
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
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_8010bf6c:
  *(undefined2 *)(_DAT_800ac784 + 0x166) = 0x14cc;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
  uVar4 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
  func_0x8001af5c(0,0,uVar4 + 100,uVar4 + 200,_DAT_800ac784 + 0xb0,0x808080);
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(int *)(iVar3 + 0xc84) = iVar3 + 0xec;
  *(int *)(iVar3 + 0xca8) = iVar3 + 0xac;
  *(undefined4 *)(iVar3 + 0xc44) = 0x66;
  *(undefined4 *)(iVar3 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(iVar3 + 0xc4c) = 0;
  *(undefined2 *)(iVar3 + 0xc78) = 0;
  *(undefined2 *)(iVar3 + 0xc7a) = 0;
  *(undefined2 *)(iVar3 + 0xc7c) = 0;
  func_0x80068098(iVar3 + 0xc78,iVar3 + 0xc30);
  return;
}



/* ======= FUN_8010b784 @ 0x8010b784 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b784(void)

{
  undefined2 uVar1;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x14;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 8;
  }
  func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),0x10);
  func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
      if ((_DAT_800ac784[0x76] & 0x20) == 0) {
        FUN_8010c1bc(0,0);
      }
    }
    else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
      uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                              (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                              (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                              (_DAT_800aca90 - _DAT_800ac784[0xf]));
      *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
      (**(code **)(&LAB_8011fa5c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
      (**(code **)(&LAB_8011fa9c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
      if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
        *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
        FUN_8010ca2c();
        return;
      }
      *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
      return;
    }
  }
  return;
}



/* ======= FUN_8010b8b4 @ 0x8010b8b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8b4(void)

{
  int iVar1;
  uint unaff_s0;
  
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)((int)_DAT_800ac784 + 5) * 0x20 + -0x7fee0bd4))();
    return;
  }
  if (*(char *)((int)_DAT_800ac784 + 5) == '\x02') {
    iVar1 = FUN_80106b6c();
  }
  else {
    iVar1 = FUN_801069d4();
  }
  (**(code **)(&LAB_8011fa9c + (uint)*(byte *)(*(int *)(iVar1 + -0x387c) + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & (unaff_s0 | 0xffff);
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(4000);
  if ((char)_DAT_800ac784[0x78] != '\0') {
    *_DAT_800ac784 = *_DAT_800ac784 & (unaff_s0 | 0xffff);
    func_0x80012a0c(5000);
  }
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    FUN_8010ca2c();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010b948 @ 0x8010b948 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b948(void)

{
  uint *extraout_v1;
  uint unaff_s0;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0934))();
    return;
  }
  FUN_80107de4();
  if ((char)extraout_v1[0x78] != '\0') {
    *extraout_v1 = *extraout_v1 & unaff_s0;
    func_0x80012a0c(5000);
  }
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
    FUN_8010ca2c();
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010b9bc @ 0x8010b9bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9bc(undefined4 param_1,int param_2)

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



/* ======= FUN_8010bb20 @ 0x8010bb20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb20(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_8011fa3c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010bcb8 @ 0x8010bcb8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bcb8(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee1ef8);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = *(uint *)(&LAB_8011f9dc + (uint)(byte)_DAT_800ac784[0x79] * 4);
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



/* ======= FUN_8010c03c @ 0x8010c03c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c03c(void)

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
  (**(code **)(&LAB_8011fa5c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011fa9c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    FUN_8010ca2c();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010c1bc @ 0x8010c1bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c1bc(void)

{
  uint *in_v1;
  uint unaff_s0;
  
  *in_v1 = *in_v1 & unaff_s0;
  func_0x80012a0c(5000);
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
    FUN_8010ca2c();
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010c330 @ 0x8010c330 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c330(void)

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



/* ======= FUN_8010c520 @ 0x8010c520 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c520(void)

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



/* ======= FUN_8010c61c @ 0x8010c61c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c61c(void)

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
LAB_8010c6ec:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(3);
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    bVar8 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    uVar10 = FUN_80111388(0,0);
    in_v1 = (uint)((ulonglong)uVar10 >> 0x20);
    uVar6 = (uint)uVar10;
  }
  else {
    if (uVar6 == 0) {
      uVar6 = 2;
      if (bVar8 != 2) goto LAB_8010cfe8;
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
          goto LAB_8010cfe8;
        }
        uVar6 = 3;
        uVar3 = 3;
        if (in_v1 != 0) goto LAB_8010cfe8;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_8010cfe8;
    }
    if (bVar8 != 0) goto LAB_8010cfe8;
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
        goto LAB_8010c6ec;
      }
    }
    uVar9 = unaff_s0 - 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    if (unaff_s0 != 0) {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011fae0)[uVar9]) {
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
LAB_8010cfe8:
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
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011faec)[unaff_s3]) {
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



/* ======= FUN_8010c7f8 @ 0x8010c7f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7f8(void)

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



/* ======= FUN_8010c930 @ 0x8010c930 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c930(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  ushort unaff_s3;
  short unaff_s4;
  
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
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x18) {
      FUN_80111388(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
        FUN_8010d2c0();
        return;
      }
LAB_8010cac8:
      func_0x800453d0(8);
      return;
    }
    do {
      if (*(char *)(iRamffffc784 + 0x95) == (&LAB_8011fae4)[unaff_s3]) {
        unaff_s4 = 1;
      }
      bVar1 = unaff_s3 != 0;
      unaff_s3 = unaff_s3 - 1;
    } while (bVar1);
    if (unaff_s4 == 0) {
      return;
    }
    iVar4 = -0x7ff50000;
  }
  else {
    iVar4 = _DAT_800ac784;
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x29) {
      FUN_80111388(0,1);
      if ((*(char *)(_DAT_800ac784 + 0x95) != '\x01') && (*(char *)(_DAT_800ac784 + 0x95) != ')')) {
        return;
      }
      goto LAB_8010cac8;
    }
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if ((*(short *)(iVar4 + 0x1d8) == 0) && (*(short *)(iVar4 + 0x1da) == 0)) {
    return;
  }
  _DAT_800acaee = _DAT_800acaee + -10;
  *(undefined2 *)(iVar4 + 0x1dc) = 0x2d;
  func_0x800453d0(4,0);
  DAT_800aca58 = 2;
  cVar3 = func_0x8001a780(&DAT_800aca54);
  DAT_800aca59 = cVar3 + '\x04';
  DAT_800aca5a = 0;
  if (_DAT_800acaee < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  *(byte *)(_DAT_800ac784 + 5) = DAT_800acae7;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}



/* ======= FUN_8010ca2c @ 0x8010ca2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ca2c(void)

{
  bool bVar1;
  char cVar2;
  int in_v1;
  ushort unaff_s3;
  short unaff_s4;
  
  if (*(byte *)(in_v1 + 0x95) < 0x18) {
    FUN_80111388(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
      FUN_8010d2c0();
      return;
    }
    func_0x800453d0(8);
    return;
  }
  do {
    if (*(char *)(iRamffffc784 + 0x95) == (&LAB_8011fae4)[unaff_s3]) {
      unaff_s4 = 1;
    }
    bVar1 = unaff_s3 != 0;
    unaff_s3 = unaff_s3 - 1;
  } while (bVar1);
  if ((unaff_s4 != 0) &&
     ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
    _DAT_800acaee = _DAT_800acaee + -10;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(4,0);
    DAT_800aca58 = 2;
    cVar2 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar2 + '\x04';
    DAT_800aca5a = 0;
    if (_DAT_800acaee < 0) {
      DAT_800aca58 = 3;
      DAT_800aca59 = '\0';
    }
    DAT_800acae7 = DAT_800acae7 | 1;
    *(byte *)(_DAT_800ac784 + 5) = DAT_800acae7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    return;
  }
  return;
}



/* ======= FUN_8010cae0 @ 0x8010cae0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cae0(void)

{
  int iVar1;
  uint uVar2;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(4000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    return;
  }
  if ((((((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) &&
        (*(char *)(_DAT_800ac784 + 0x1e2) == '\0')) &&
       ((*(short *)(_DAT_800ac784 + 0x1d6) == 0 &&
        ((6000 < *(ushort *)(_DAT_800ac784 + 0x1d4) &&
         (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)))))) &&
      ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
     (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_8010cc28 @ 0x8010cc28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc28(void)

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



/* ======= FUN_8010cd88 @ 0x8010cd88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cd88(void)

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
      goto LAB_8010d83c;
    }
    bVar4 = 1;
    if (uVar7 != 0) goto LAB_8010d83c;
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 0x12;
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  _DAT_800ac784[6] = _DAT_800ac784[6] + cVar3;
  FUN_80111388(0,0);
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
    if (_DAT_800ac784[0x95] == (&DAT_8011fae0)[uVar9]) {
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
LAB_8010d83c:
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
    local_28 = DAT_8011fa1c;
    local_24 = DAT_8011fa20;
    local_20 = DAT_8011fa24;
    local_1c = DAT_8011fa28;
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
          if (_DAT_800ac784[0x95] == (&DAT_8011faec)[uVar10]) {
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



/* ======= FUN_8010d064 @ 0x8010d064 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d064(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  byte bVar5;
  undefined2 uVar6;
  int extraout_v1;
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
      bVar5 = 2;
      if (bVar2 == 2) {
        if ((DAT_800acae7 == 0) && (iVar7 = func_0x8001a804(4000,0x180,&DAT_800aca88), iVar7 < 0)) {
          if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
            *(undefined1 *)(_DAT_800ac784 + 7) = 0;
            bVar5 = 2;
            if (-1 < _DAT_800acaee) {
              return;
            }
            goto LAB_8010dbf0;
          }
        }
        else if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 3;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_8010dbf0;
    }
    bVar5 = 1;
    if (bVar2 != 0) goto LAB_8010dbf0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_8011145c(0,0);
    *(undefined1 *)(extraout_v1 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0();
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    return;
  }
  FUN_80111388(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 != 0) {
    return;
  }
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
    if (*(char *)(_DAT_800ac784 + 0x95) == (&LAB_8011fae4)[uVar8]) {
      bVar3 = true;
    }
    bVar1 = uVar8 != 0;
    uVar8 = uVar8 - 1;
  } while (bVar1);
  if (!bVar3) {
    return;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d8) == 0) && (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
    return;
  }
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
  bVar5 = DAT_800acae7;
LAB_8010dbf0:
  *(byte *)(_DAT_800ac784 + 5) = bVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}



/* ======= FUN_8010d2c0 @ 0x8010d2c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d2c0(void)

{
  char cVar1;
  int in_v1;
  
  _DAT_800acaee = _DAT_800acaee + -10;
  *(undefined2 *)(in_v1 + 0x1dc) = 0x2d;
  func_0x800453d0();
  DAT_800aca58 = 2;
  cVar1 = func_0x8001a780(&DAT_800aca54);
  DAT_800aca59 = cVar1 + '\x04';
  DAT_800aca5a = 0;
  if (_DAT_800acaee < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  *(byte *)(_DAT_800ac784 + 5) = DAT_800acae7;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}



/* ======= FUN_8010d41c @ 0x8010d41c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d41c(void)

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
            FUN_8010df74();
            return;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
          local_28 = DAT_8011fa1c;
          local_24 = DAT_8011fa20;
          local_20 = DAT_8011fa24;
          local_1c = DAT_8011fa28;
          func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                          *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
        iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                                (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar3 == 0) {
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
        }
        else {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(1);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        }
        goto LAB_8010df8c;
      }
      if (bVar1 != 3) goto LAB_8010df8c;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        goto LAB_8010df8c;
      }
      goto LAB_8010d784;
    }
    if (bVar1 != 0) goto LAB_8010df8c;
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
    iVar3 = _DAT_800ac784;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(iVar3 + 0x94) = 0xc;
LAB_8010df8c:
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1c;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
LAB_8010d784:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010d7a4 @ 0x8010d7a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d7a4(void)

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
    local_48 = DAT_8011fa1c;
    local_44 = DAT_8011fa20;
    local_40 = DAT_8011fa24;
    local_3c = DAT_8011fa28;
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
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011faec)[uVar6]) {
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
        return;
      }
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800453d0(1);
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  return;
}



/* ======= FUN_8010dc80 @ 0x8010dc80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dc80(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  if (1 < *(byte *)(_DAT_800ac784 + 6)) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
    if ((bVar1 & 2) != 0) {
      if ((bVar1 & 0x40) != 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 2;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        uVar2 = func_0x8001a9cc();
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
        *(short *)(_DAT_800ac784 + 0x6a) =
             *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1da);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
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
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_8011fa1c;
      local_24 = DAT_8011fa20;
      local_20 = DAT_8011fa24;
      local_1c = DAT_8011fa28;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_8010dd68 @ 0x8010dd68 ======= */
// decompile failed: Exception while decompiling 8010dd68: process: timeout


/* ======= FUN_8010df74 @ 0x8010df74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010df74(void)

{
  int iVar1;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 6) = 4;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1c;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_8010e030 @ 0x8010e030 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e030(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  if (*(byte *)(_DAT_800ac784 + 6) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
    if ((bVar1 & 2) != 0) {
      if ((bVar1 & 0x40) != 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 2;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        FUN_8010e908();
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_8011fa1c;
      local_24 = DAT_8011fa20;
      local_20 = DAT_8011fa24;
      local_1c = DAT_8011fa28;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_8010e118 @ 0x8010e118 ======= */
// decompile failed: Exception while decompiling 8010e118: process: timeout


/* ======= FUN_8010e394 @ 0x8010e394 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e394(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010ec58();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011fa1c;
    local_24 = DAT_8011fa20;
    local_20 = DAT_8011fa24;
    local_1c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010e468 @ 0x8010e468 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e468(void)

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



/* ======= FUN_8010e634 @ 0x8010e634 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e634(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010eef8();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011fa1c;
    local_24 = DAT_8011fa20;
    local_20 = DAT_8011fa24;
    local_1c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010e708 @ 0x8010e708 ======= */
// decompile failed: Exception while decompiling 8010e708: process: timeout


/* ======= FUN_8010e908 @ 0x8010e908 ======= */

void FUN_8010e908(undefined4 param_1,undefined4 param_2,int param_3)

{
  char in_v0;
  
  *(char *)(*(int *)(param_3 + -0x387c) + 6) = *(char *)(*(int *)(param_3 + -0x387c) + 6) + in_v0;
  FUN_8011145c(0,0);
  FUN_8010f15c();
  return;
}



/* ======= FUN_8010e96c @ 0x8010e96c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e96c(void)

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
      FUN_8010f42c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = DAT_8011fa1c;
    local_34 = DAT_8011fa20;
    local_30 = DAT_8011fa24;
    local_2c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar4 = func_0x8001a780(&DAT_800aca54), iVar4 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_8010f42c();
    return;
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011faf1)[bVar5]) {
      bVar2 = true;
    }
    bVar1 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar1);
  if (((bVar2) && (DAT_800acae7 == '\0')) &&
     ((iVar4 = func_0x8001a804(5000,0x180,&DAT_800aca88), iVar4 < 0 &&
      (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
    DAT_00000014 = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    return;
  }
  bVar5 = 2;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x8011faf0)) {
      bVar3 = true;
    }
    bVar2 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar2);
  if (((bVar3) && (4000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 1 < 0x19) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_8010ec58 @ 0x8010ec58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec58(void)

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
  if (0x18 < *(byte *)(_DAT_800ac784 + 0x95)) {
    FUN_8010f5c4(0,0);
    return;
  }
  FUN_8011145c(0,1);
  return;
}



/* ======= FUN_8010ec5c @ 0x8010ec5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec5c(void)

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
  if (0x18 < *(byte *)(_DAT_800ac784 + 0x95)) {
    FUN_8010f5c4(0,0);
    return;
  }
  FUN_8011145c(0,1);
  return;
}



/* ======= FUN_8010eddc @ 0x8010eddc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eddc(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010f6a0();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011fa1c;
    local_24 = DAT_8011fa20;
    local_20 = DAT_8011fa24;
    local_1c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010eeb0 @ 0x8010eeb0 ======= */

/* WARNING: Removing unreachable block (ram,0x8010f1dc) */
/* WARNING: Removing unreachable block (ram,0x8010f1e0) */
/* WARNING: Removing unreachable block (ram,0x8010f1ec) */
/* WARNING: Removing unreachable block (ram,0x8010f1f8) */
/* WARNING: Removing unreachable block (ram,0x8010f204) */
/* WARNING: Removing unreachable block (ram,0x8010f264) */
/* WARNING: Removing unreachable block (ram,0x8010f274) */
/* WARNING: Removing unreachable block (ram,0x8010f168) */
/* WARNING: Removing unreachable block (ram,0x8010f194) */
/* WARNING: Removing unreachable block (ram,0x8010f1d8) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eeb0(void)

{
  bool bVar1;
  byte bVar2;
  undefined1 uVar3;
  char cVar4;
  ushort uVar5;
  undefined2 uVar6;
  int iVar7;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_8010ef68:
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x3f) + 100;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x10) + 0x40;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 0xd) ||
       (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x28 < 0xd)) {
      func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x28) {
      FUN_8011145c(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 != '\0') {
        return;
      }
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      uStack_28 = _DAT_80072d60;
      uStack_24 = _DAT_80072d64;
      uStack_20 = _DAT_80072d68;
      uStack_1c = _DAT_80072d6c;
      bVar2 = 5;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011faf4)) {
          uVar6 = func_0x8001bff8(iVar7 + 0x448,&uStack_28,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
        }
        bVar1 = bVar2 != 0;
        bVar2 = bVar2 - 1;
      } while (bVar1);
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      }
      return;
    }
    return;
  }
  if (bVar2 < 2) {
    uVar3 = 1;
    if (bVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x800453d0(9);
      goto LAB_8010ef68;
    }
  }
  else {
    uVar3 = 0xd;
    if (bVar2 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 0x93) = uVar3;
  *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  iVar7 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
  uStack_28 = *(undefined4 *)(&LAB_8011f9fc + iVar7);
  uStack_24 = *(undefined4 *)(&LAB_8011fa00 + iVar7);
  uStack_20 = *(undefined4 *)(&LAB_8011fa04 + iVar7);
  uStack_1c = *(undefined4 *)(&LAB_8011fa08 + iVar7);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
  func_0x800453d0(2);
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80111388(0,0);
  }
  else {
    FUN_80111388(0,1);
  }
  return;
}



/* ======= FUN_8010eef8 @ 0x8010eef8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eef8(undefined1 *param_1)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  int iVar6;
  int in_v1;
  char unaff_s0;
  byte unaff_s1;
  char unaff_s3;
  char unaff_s4;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  bVar2 = 1;
  if (in_v1 != 0) {
LAB_8010fab8:
    param_1[0x93] = bVar2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar6 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    in_stack_00000010 = *(undefined4 *)(&LAB_8011f9fc + iVar6);
    in_stack_00000014 = *(undefined4 *)(&LAB_8011fa00 + iVar6);
    in_stack_00000018 = *(undefined4 *)(&LAB_8011fa04 + iVar6);
    in_stack_0000001c = *(undefined4 *)(&LAB_8011fa08 + iVar6);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
    func_0x800453d0(2);
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
      FUN_80111388(0,0);
    }
    else {
      FUN_80111388(0,1);
    }
    return;
  }
  param_1[6] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  func_0x800453d0(9);
  uVar4 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 100;
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x10) + 0x40;
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 0xd) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x28 < 0xd)) {
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  if (0x27 < *(byte *)(_DAT_800ac784 + 0x95)) {
    return;
  }
  FUN_8011145c(0,1);
  if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    in_stack_00000010 = _DAT_80072d60;
    in_stack_00000014 = _DAT_80072d64;
    in_stack_00000018 = _DAT_80072d68;
    in_stack_0000001c = _DAT_80072d6c;
    bVar2 = unaff_s0 - 1;
    if (unaff_s0 != '\0') {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011faf4)) {
          uVar5 = func_0x8001bff8(iVar6 + 0x448,&stack0x00000010,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
        }
        bVar1 = bVar2 != 0;
        bVar2 = bVar2 - 1;
      } while (bVar1);
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      }
      return;
    }
    while (bVar1 = unaff_s1 != 0, unaff_s1 = unaff_s1 - 1, bVar1) {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x8011fafc)) {
        uVar5 = func_0x8001bff8(iVar6 + 0x6f8,&stack0x00000010,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
        if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
          unaff_s4 = '\x01';
        }
      }
    }
    if ((unaff_s3 != '\0') || (unaff_s4 != '\0')) {
      _DAT_800acaee = _DAT_800acaee + -10;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
      DAT_800aca58 = 2;
      param_1 = &DAT_800aca54;
      cVar3 = func_0x8001a780();
      DAT_800aca59 = cVar3 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      bVar2 = DAT_800acae7 | 1;
      DAT_800acae7 = bVar2;
      goto LAB_8010fab8;
    }
  }
  return;
}



/* ======= FUN_8010f15c @ 0x8010f15c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f15c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
  }
  if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
  }
  return;
}



/* ======= FUN_8010f2e4 @ 0x8010f2e4 ======= */
// decompile failed: Exception while decompiling 8010f2e4: process: timeout


/* ======= FUN_8010f42c @ 0x8010f42c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f42c(undefined4 param_1,int param_2)

{
  int in_at;
  undefined4 in_v0;
  undefined1 in_v1;
  
  *(undefined4 *)(in_at + -0x35a8) = in_v0;
  DAT_800acae7 = in_v1;
  _DAT_800acbd0 = param_1;
  *(undefined2 *)(param_2 + 0x6a) = _DAT_800acabe;
  FUN_8010fdc4(6);
  return;
}



/* ======= FUN_8010f5c4 @ 0x8010f5c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f5c4(void)

{
  char cVar1;
  
  func_0x800453d0();
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010ffa0();
  return;
}



/* ======= FUN_8010f6a0 @ 0x8010f6a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f6a0(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '1') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 8;
    FUN_8010ffa0();
    return;
  }
  return;
}



/* ======= FUN_8010f7b0 @ 0x8010f7b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f7b0(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
    func_0x800453d0(1);
  }
  (**(code **)(&LAB_8011fb1c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010f86c @ 0x8010f86c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f86c(void)

{
  int iVar1;
  uint uVar2;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        }
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
        return;
      }
      goto LAB_80110250;
    }
    if (uVar2 != 0) goto LAB_80110250;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar1 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011f9fc + iVar1);
    local_1c = *(undefined4 *)(&LAB_8011fa00 + iVar1);
    local_18 = *(undefined4 *)(&LAB_8011fa04 + iVar1);
    local_14 = *(undefined4 *)(&LAB_8011fa08 + iVar1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
LAB_80110250:
  if (uVar2 != 0x3d) {
    return;
  }
  func_0x800453d0(1);
  FUN_80110a90();
  return;
}



/* ======= FUN_8010fa60 @ 0x8010fa60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fa60(void)

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
      if (*(short *)(_DAT_800ac784 + 0x9a) < 0x32) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        }
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
      }
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
    local_20 = *(undefined4 *)(&LAB_8011f9fc + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011fa00 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011fa04 + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011fa08 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80111388(0,0);
  }
  else {
    FUN_80111388(0,1);
  }
  return;
}



/* ======= FUN_8010fcd8 @ 0x8010fcd8 ======= */
// decompile failed: Exception while decompiling 8010fcd8: process: timeout


/* ======= FUN_8010fdc4 @ 0x8010fdc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fdc4(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int extraout_v1;
  undefined4 uStack00000010;
  undefined4 uStack00000014;
  undefined4 uStack00000018;
  undefined4 uStack0000001c;
  
  iVar2 = (*(byte *)(*(int *)(in_v0 + -0x387c) + 6) & 1) * 0x20;
  uStack00000010 = *(undefined4 *)(&LAB_8011f9fc + iVar2);
  uStack00000014 = *(undefined4 *)(&LAB_8011fa00 + iVar2);
  uStack00000018 = *(undefined4 *)(&LAB_8011fa04 + iVar2);
  uStack0000001c = *(undefined4 *)(&LAB_8011fa08 + iVar2);
  func_0x80019700(param_1,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
  func_0x800453d0(2);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  iVar2 = 0;
  func_0x800245d8();
  if (extraout_v1 != 0) {
    FUN_801110f8();
    return;
  }
  *(undefined1 *)(iVar2 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_801110f8();
  return;
}



/* ======= FUN_8010ffa0 @ 0x8010ffa0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ffa0(void)

{
  undefined1 in_v0;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 5) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ======= FUN_8010ffd4 @ 0x8010ffd4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ffd4(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
    func_0x800453d0(1);
  }
  (**(code **)(&LAB_8011fb74 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80110090 @ 0x80110090 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110090(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  int extraout_v1;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_80110a90();
      return;
    }
    if (bVar1 != 0) {
      FUN_80110a90();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011f9fc + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011fa00 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011fa04 + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011fa08 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      *(char *)(extraout_v1 + 0x95) = (char)_DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80111388(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) != '=') {
    return;
  }
  func_0x800453d0(1);
  FUN_80110a90();
  return;
}



/* ======= FUN_801102a0 @ 0x801102a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801102a0(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int extraout_v1;
  int unaff_s0;
  int iVar4;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int iStack_c;
  int iStack_4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(bVar1 < 2);
  if (bVar1 != 1) {
    if (uVar3 == 0) {
      uVar3 = 2;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_80110cc4;
    }
    if (bVar1 != 0) goto LAB_80110cc4;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011f9fc + iVar4);
    local_1c = *(undefined4 *)(&LAB_8011fa00 + iVar4);
    local_18 = *(undefined4 *)(&LAB_8011fa04 + iVar4);
    local_14 = *(undefined4 *)(&LAB_8011fa08 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      *(int *)(*(byte *)(_DAT_800ac784 + 0x1c6) + 0x3c) =
           *(int *)(*(byte *)(_DAT_800ac784 + 0x1c6) + 0x3c) - (extraout_v1 + 0x7ff4efc8);
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
    return;
  }
  uVar3 = func_0x800453d0(1);
LAB_80110cc4:
  iVar4 = unaff_s0 + uVar3;
  func_0x80022da0(&local_20,iVar4 + -0x140,&local_20);
  func_0x80022da0(&local_20,iVar4 + -0x94,&local_20);
  func_0x80022da0(&local_20,iVar4 + 0x18,&local_20);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (iStack_c - *(int *)(iVar4 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (iStack_4 - *(int *)(iVar4 + 0x5c));
  return;
}



/* ======= FUN_801104d4 @ 0x801104d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801104d4(void)

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
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_80110f44();
      return;
    }
    if (bVar1 != 0) {
      FUN_80110f44();
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
    local_20 = *(undefined4 *)(&LAB_8011f9fc + iVar4);
    local_1c = *(undefined4 *)(&LAB_8011fa00 + iVar4);
    local_18 = *(undefined4 *)(&LAB_8011fa04 + iVar4);
    local_14 = *(undefined4 *)(&LAB_8011fa08 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      FUN_80110e8c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  func_0x800245d8(0);
  FUN_80110f44();
  return;
}



/* ======= FUN_80110754 @ 0x80110754 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110754(void)

{
  (**(code **)(&LAB_8011fbcc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8011079c @ 0x8011079c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011079c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801110f8();
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
      FUN_801110f8();
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
  FUN_801110f8();
  return;
}



/* ======= FUN_80110908 @ 0x80110908 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110908(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011f9e0 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
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



/* ======= FUN_80110a90 @ 0x80110a90 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a90(int param_1)

{
  uint *puVar1;
  byte bVar2;
  char cVar3;
  uint *puVar4;
  
  bVar2 = *(byte *)(*(int *)(param_1 + -0x387c) + 7);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(*(int *)(param_1 + -0x387c) + 0x9e) = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  }
  puVar4 = _DAT_800ac784;
  puVar1 = _DAT_800ac784 + 0x3b;
  _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xffff38;
  puVar4[0x3b] = *puVar1 & 0xff000000 | 0xffff38;
  *(short *)(puVar4 + 0x2f) = (short)puVar4[0x2f] + 8;
  *(short *)((int)puVar4 + 0xbe) = *(short *)((int)puVar4 + 0xbe) + 8;
  cVar3 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 2;
  }
  return;
}



/* ======= FUN_80110b88 @ 0x80110b88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b88(undefined4 param_1,int param_2)

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



/* ======= FUN_80110c5c @ 0x80110c5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110c5c(undefined4 param_1,int param_2)

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



/* ======= FUN_80110e8c @ 0x80110e8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110e8c(void)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  func_0x8001ad68(&DAT_800aca54);
  uVar3 = 0x80110eb8;
  iVar2 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (*(char *)(*(int *)(iVar2 + -0x387c) + 6) == '\0') {
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
                  0x200,uVar3);
  if (((int)*(short *)(_DAT_800ac784 + 0x9c) % 0xc) * 0x10000 >> 0x10 == 0xb) {
    func_0x800453d0(6);
  }
  return;
}



/* ======= FUN_80110f44 @ 0x80110f44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f44(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  int unaff_s0;
  undefined4 uVar4;
  
  func_0x8001ad68(unaff_s0 + -0x95,_DAT_800acbcc,_DAT_800acbd0);
  uVar4 = 0x80110f78;
  iVar2 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar2 != 0) {
    DAT_800aca5a = 6;
    uVar3 = *(uint *)(unaff_s0 + -0x95) & 0xffffefff;
    *(uint *)(unaff_s0 + -0x95) = uVar3;
    if (*(char *)(*(int *)(uVar3 - 0x387c) + 6) == '\0') {
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
                    0x200,uVar4);
    if (((int)*(short *)(_DAT_800ac784 + 0x9c) % 0xc) * 0x10000 >> 0x10 == 0xb) {
      func_0x800453d0(6);
    }
    return;
  }
  return;
}



/* ======= FUN_801110c4 @ 0x801110c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110c4(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = DAT_800aca5a < 2;
  if (DAT_800aca5a != 1) {
    if (!(bool)bVar1) {
      bVar1 = 0;
      iVar2 = 0;
      if (DAT_800aca5a == 2) {
        func_0x80037edc(0,10);
        func_0x80037edc(5,0x32);
        func_0x80037edc(7,0x32);
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x80111a3c;
    }
    iVar2 = 0x2000;
    if (DAT_800aca5a != 0) goto code_r0x80111a3c;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011fbe4
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011fbe4
                   );
  }
  iVar2 = _DAT_800acbcc;
  bVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + bVar1,_DAT_800aca58);
code_r0x80111a3c:
  if (((bVar1 & 0x19) == 0) &&
     ((((*(byte *)(iVar2 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(iVar2 + 0x6a)) + 0x200 & 0xfff) <
      0x400)) {
    *(byte *)(iVar2 + 0x9f) = *(byte *)(iVar2 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_801110f8 @ 0x801110f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110f8(void)

{
  int in_v1;
  
  if (in_v1 == 2) {
    func_0x80037edc(0,10);
    func_0x80037edc(5,0x32);
    func_0x80037edc(7,0x32);
    _DAT_800aca58 = 7;
    return;
  }
  if ((((bRam00000090 & 0xf0) * 0x10 - (int)sRam0000006a) + 0x200 & 0xfff) < 0x400) {
    bRam0000009f = bRam00000090;
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_80111250 @ 0x80111250 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111250(void)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  uVar2 = 4;
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_8011fc1c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_80111388 @ 0x80111388 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111388(int param_1)

{
  int in_v1;
  
  if (*(int *)(param_1 + 0x38) < in_v1) {
    *(undefined2 *)(param_1 + 0xb4) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0xb6) = 0;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_801113e4 @ 0x801113e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801113e4(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee1ef8);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = DAT_8011fc14;
  func_0x8001af5c(0,0,1000,0x44c,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_8011145c @ 0x8011145c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011145c(void)

{
  uint uVar1;
  char cVar2;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1be) = _DAT_800aca90;
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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee1ef8);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = DAT_8011fc14;
  func_0x8001af5c(0,0,1000,0x44c);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_80111618 @ 0x80111618 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111618(void)

{
  undefined2 uVar1;
  uint uVar2;
  char cVar3;
  char extraout_v1;
  uint uVar4;
  
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
  (**(code **)(&LAB_8011fc3c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011fc7c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar2 = _DAT_800ac784[0x78];
    *(ushort *)(_DAT_800ac784 + 0x78) = (ushort)uVar2 + 1;
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(*(int *)((ushort)uVar2 - 0x387b) + 0x9e));
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 0x95);
    uVar2 = uVar4 / 7 + (uVar4 - uVar4 / 7 >> 1) >> 2;
    cVar3 = (char)uVar2 * '\a';
    if ((uVar4 + uVar2 * -7 & 0xff) == 6) {
      func_0x800453d0(5);
      cVar3 = extraout_v1;
    }
    if (6 < *(byte *)((int)_DAT_800ac784 + 0x95) - 2) {
      SUB_00000006 = cVar3;
      return;
    }
    FUN_80115d6c(0,0);
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  return;
}



/* ======= FUN_80111864 @ 0x80111864 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111864(void)

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



/* ======= FUN_801119a4 @ 0x801119a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801119a4(void)

{
  int iVar1;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(8000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_80112294();
    return;
  }
  if ((((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
      ((*(byte *)(_DAT_800ac784 + 9) & 0x19) == 0)) &&
     ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
       0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_80111aa4 @ 0x80111aa4 ======= */
// decompile failed: Exception while decompiling 80111aa4: process: timeout


/* ======= FUN_80111c14 @ 0x80111c14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c14(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80112558();
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        FUN_80112558();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_80112558();
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
  FUN_80115d6c(0,0);
  FUN_80112558();
  return;
}



/* ======= FUN_80111d68 @ 0x80111d68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111d68(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(3000,0x100,&DAT_800aca88), iVar1 < 0)) &&
      (*(short *)(_DAT_800ac784 + 0x1de) == 0)) &&
     ((0x50 < _DAT_800acaee && (0x9c4 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
    DAT_800aca5a = 7;
    FUN_80112f84();
    return;
  }
  if (DAT_800acae7 == '\0') {
    iVar1 = func_0x8001a804(0x708,0x180,&DAT_800aca88);
    if ((iVar1 < 0) && (*(short *)(_DAT_800ac784 + 0x1de) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      FUN_8011274c();
      return;
    }
    if (((DAT_800acae7 == '\0') && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) &&
       (7000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      FUN_8011274c();
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
  return;
}



/* ======= FUN_80111f5c @ 0x80111f5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f5c(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  char cVar4;
  char extraout_v1;
  uint uVar5;
  
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
  uVar5 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  uVar3 = uVar5 / 7 + (uVar5 - uVar5 / 7 >> 1) >> 2;
  cVar4 = (char)uVar3 * '\a';
  if ((uVar5 + uVar3 * -7 & 0xff) == 6) {
    func_0x800453d0(5);
    cVar4 = extraout_v1;
  }
  if (6 < *(byte *)(_DAT_800ac784 + 0x95) - 2) {
    SUB_00000006 = cVar4;
    return;
  }
  FUN_80115d6c(0,0);
  return;
}



/* ======= FUN_801120dc @ 0x801120dc ======= */
// decompile failed: Exception while decompiling 801120dc: process: timeout


/* ======= FUN_80112294 @ 0x80112294 ======= */

void FUN_80112294(void)

{
  uint uVar1;
  byte in_v1;
  byte *unaff_s0;
  
  *unaff_s0 = in_v1 | 1;
  uVar1 = func_0x8001af20();
  func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee0348 + (uVar1 & 1)),0x32);
  FUN_80112bc8();
  return;
}



/* ======= FUN_801123e4 @ 0x801123e4 ======= */
// decompile failed: Exception while decompiling 801123e4: process: timeout


/* ======= FUN_80112558 @ 0x80112558 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112558(void)

{
  char cVar1;
  undefined1 in_v0;
  short sVar2;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 0x9e) = in_v0;
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if ((cVar1 != '\0') && (-1 < _DAT_800acaee)) {
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar2 * -100;
    if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      DAT_800aca5a = 4;
      FUN_80112f84();
      return;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  FUN_80112f84();
  return;
}



/* ======= FUN_8011274c @ 0x8011274c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011274c(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  DAT_800acae7 = DAT_800acae7 & 0xfe;
  return;
}



/* ======= FUN_801127a0 @ 0x801127a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801127a0(void)

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



/* ======= FUN_801128d8 @ 0x801128d8 ======= */
// decompile failed: Exception while decompiling 801128d8: process: timeout


/* ======= FUN_80112bc8 @ 0x80112bc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112bc8(void)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar1 = func_0x8001f314(*(undefined4 *)(in_v0 + 0x84),*(undefined4 *)(in_v0 + 0x16c),0,0x200);
  iVar3 = (uint)*(byte *)(_DAT_800ac784 + 6) + iVar1;
  *(char *)(_DAT_800ac784 + 6) = (char)iVar3;
  *(char *)(iVar3 + 5) = (char)iVar1;
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (*(char *)(_DAT_800ac784 + 9) == '\t') {
    uVar2 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3c + (uVar2 & 0xf);
    if (-0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}



/* ======= FUN_80112c80 @ 0x80112c80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112c80(void)

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



/* ======= FUN_80112db8 @ 0x80112db8 ======= */
// decompile failed: Exception while decompiling 80112db8: process: timeout


/* ======= FUN_80112f84 @ 0x80112f84 ======= */

void FUN_80112f84(void)

{
  int in_at;
  int in_v0;
  
                    /* WARNING: Could not recover jumptable at 0x8011380c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(in_at + 0x30c + in_v0))();
  return;
}



/* ======= FUN_801133f8 @ 0x801133f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801133f8(void)

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



/* ======= FUN_8011351c @ 0x8011351c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011351c(void)

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



/* ======= FUN_801136bc @ 0x801136bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801136bc(void)

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



/* ======= FUN_801137c8 @ 0x801137c8 ======= */
// decompile failed: Exception while decompiling 801137c8: process: timeout


/* ======= FUN_80113a9c @ 0x80113a9c ======= */
// decompile failed: Exception while decompiling 80113a9c: process: timeout


/* ======= FUN_80113e14 @ 0x80113e14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e14(void)

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
    FUN_80114730();
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



/* ======= FUN_80113f48 @ 0x80113f48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f48(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801148e0();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_801148e0();
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
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fcc0);
      *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80115d6c(0,1);
  FUN_801148e0();
  return;
}



/* ======= FUN_801140f0 @ 0x801140f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801140f0(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  byte bVar5;
  
  if (((((short)_DAT_800ac784[0x77] == 0) ||
       ((&LAB_8011fd10)[*(byte *)((int)_DAT_800ac784 + 5)] == '\0')) ||
      (*(char *)((int)_DAT_800ac784 + 6) != '\x01')) ||
     ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0)) {
    (**(code **)(&LAB_8011fd28 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
    return;
  }
  _DAT_800ac784[1] = 0x601;
  puVar3 = (uint *)&DAT_0000000b;
  func_0x800453d0();
  puVar4 = _DAT_800ac784;
  *(short *)((int)_DAT_800ac784 + 0x9a) = (short)_DAT_800ac784[0x77];
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  if (puVar4 != (uint *)0x1) {
    if (1 < (int)puVar4) {
      if (puVar4 != (uint *)0x2) {
        return;
      }
      goto LAB_80114b4c;
    }
    if (puVar4 != (uint *)0x0) {
      return;
    }
    *puVar3 = *puVar3 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    puVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 0;
    *(undefined2 *)((int)puVar4 + 0xbe) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    bVar5 = 0x19;
    do {
      puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar5 != 0;
      *puVar4 = *puVar4 & 0xfffffffe;
      bVar5 = bVar5 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 9) != 'B') {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_80114b4c:
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
    bVar5 = 0x19;
    do {
      puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar5 != 0;
      *puVar4 = *puVar4 | 1;
      bVar5 = bVar5 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '2') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar4 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar2 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}



/* ======= FUN_801141d0 @ 0x801141d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801141d0(undefined4 param_1,uint param_2)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint *puVar4;
  ulonglong uVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  uVar5 = (ulonglong)CONCAT14(bVar1,(uint)(bVar1 < 2));
  if (bVar1 != 1) {
    puVar4 = _DAT_800ac784;
    if (bVar1 < 2 == 0) {
      uVar5 = 0x60100000002;
      if (bVar1 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
        _DAT_800ac784[1] = 0x601;
        if ((short)_DAT_800ac784[0x77] != 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x77) = 100;
          *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        }
        return;
      }
      goto LAB_80114bb0;
    }
    if (bVar1 != 0) goto LAB_80114bb0;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 4;
    if ((short)_DAT_800ac784[0x77] != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 7;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)((int)_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011fcd0 + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011fcd4 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011fcd8 + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011fcdc + iVar3);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  puVar4 = (uint *)0x0;
  param_2 = 1;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  uVar5 = FUN_80115d6c();
LAB_80114bb0:
  while (*puVar4 = (uint)uVar5, (int)(uVar5 >> 0x20) != 0) {
    puVar4 = (uint *)((param_2 & 0xff) * 0xac + _DAT_800ac784[0x62]);
    uVar5 = CONCAT44(param_2,*puVar4) & 0xffffffffff | 1;
    param_2 = param_2 - 1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '2') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar4 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar3 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar3 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}



/* ======= FUN_801143c0 @ 0x801143c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801143c0(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  uint *puVar4;
  byte bVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar5 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar5 != 1) {
    puVar4 = _DAT_800ac784;
    if (1 < bVar5) {
      if (bVar5 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
        _DAT_800ac784[1] = 0x701;
        *(undefined2 *)(_DAT_800ac784 + 0x77) = 100;
        return;
      }
      goto LAB_80114dd8;
    }
    if (bVar5 != 0) goto LAB_80114dd8;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 8;
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 9;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)((int)_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011fcd0 + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011fcd4 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011fcd8 + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011fcdc + iVar3);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  if (((*(byte *)((int)_DAT_800ac784 + 0x95) < 0xf) || ((char)_DAT_800ac784[0x25] != '\b')) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x95) < 0x11 || ((char)_DAT_800ac784[0x25] != '\t')))) {
    puVar4 = (uint *)0x0;
    FUN_80115d6c(0,1);
  }
  else {
    puVar4 = (uint *)0x0;
    FUN_80115d6c(0,0);
  }
LAB_80114dd8:
  if (*(char *)((int)puVar4 + 9) == 'B') {
    *(undefined1 *)((int)puVar4 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
      bVar5 = 0x19;
      do {
        puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
        bVar1 = bVar5 != 0;
        *puVar4 = *puVar4 | 1;
        bVar5 = bVar5 - 1;
      } while (bVar1);
    }
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '!') {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      puVar4 = _DAT_800ac784;
      *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
      *(undefined2 *)((int)puVar4 + 0xbe) = 0x44c;
      *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
    }
    iVar3 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (iVar3 != 0) {
      _DAT_800ac784[1] = 0x101;
    }
  }
  return;
}



/* ======= FUN_801145e8 @ 0x801145e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801145e8(void)

{
  (**(code **)(&LAB_8011fd80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80114630 @ 0x80114630 ======= */
// decompile failed: Exception while decompiling 80114630: process: timeout


/* ======= FUN_80114730 @ 0x80114730 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114730(undefined4 param_1)

{
  byte bVar1;
  undefined4 *in_at;
  char cVar2;
  bool bVar3;
  int in_v0;
  uint *puVar4;
  int iVar5;
  int in_v1;
  int unaff_s0;
  undefined4 uStack00000010;
  undefined4 uStack00000014;
  undefined4 uStack00000018;
  undefined4 uStack0000001c;
  
  uStack00000010 = *in_at;
  uStack00000014 = *(undefined4 *)(&LAB_8011fcd4 + in_v0);
  uStack00000018 = *(undefined4 *)(&LAB_8011fcd8 + in_v0);
  uStack0000001c = *(undefined4 *)(&LAB_8011fcdc + in_v0);
  func_0x80019700(param_1,(int)*(short *)(in_v1 + 0x6a),*(int *)(in_v1 + 0x188) + 0x40);
  func_0x800453d0(2);
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  bVar1 = *(byte *)(_DAT_800ac784 + 0x95);
  if (((bVar1 < 0xf) || (bVar3 = bVar1 < 0x11, *(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
     ((bVar3 = true, bVar1 < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
    FUN_80115d6c(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x18') {
      return;
    }
    func_0x800453d0(7);
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  else {
    unaff_s0 = (uint)bVar3 * 8;
    func_0x80019700(0,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40);
    func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0_1)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40,&stack0x00000020);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0_2)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40);
  }
  func_0x80019700();
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_1)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_2)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_3)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  puVar4 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar4[0x25] = 0;
  puVar4[0x26] = 0;
  *puVar4 = *puVar4 | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x3f0) = 0;
  *(undefined4 *)(iVar5 + 0x3f4) = 0;
  *(uint *)(iVar5 + 0x35c) = *(uint *)(iVar5 + 0x35c) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xd58) = 0;
  *(undefined4 *)(iVar5 + 0xd5c) = 0;
  *(uint *)(iVar5 + 0xcc4) = *(uint *)(iVar5 + 0xcc4) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xc00) = 0;
  *(undefined4 *)(iVar5 + 0xc04) = 0;
  *(uint *)(iVar5 + 0xb6c) = *(uint *)(iVar5 + 0xb6c) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x8a4) = 0;
  *(undefined4 *)(iVar5 + 0x8a8) = 0;
  *(uint *)(iVar5 + 0x810) = *(uint *)(iVar5 + 0x810) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar5 + 0x60c) = *(uint *)(iVar5 + 0x60c) | 0x80;
  *(undefined4 *)(iVar5 + 0x6a0) = 0;
  *(undefined4 *)(iVar5 + 0x6a4) = 0;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xf5c) = 0;
  *(undefined4 *)(iVar5 + 0xf60) = 0;
  *(uint *)(iVar5 + 0xec8) = *(uint *)(iVar5 + 0xec8) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x1160) = 0;
  *(undefined4 *)(iVar5 + 0x1164) = 0;
  *(uint *)(iVar5 + 0x10cc) = *(uint *)(iVar5 + 0x10cc) | 0x80;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),1,
                  0x200);
  *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  return;
}



/* ======= FUN_801148e0 @ 0x801148e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801148e0(void)

{
  byte *in_at;
  undefined1 in_v0;
  char cVar1;
  uint *puVar2;
  int iVar3;
  int in_v1;
  int unaff_s0;
  
  *(undefined1 *)(in_v1 + 0x8f) = in_v0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  func_0x80019700(iVar3,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*in_at * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_1)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_2)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_3)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
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



/* ======= FUN_80114964 @ 0x80114964 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114964(void)

{
  (**(code **)(&LAB_8011fdd8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801149ac @ 0x801149ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801149ac(void)

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
      goto LAB_80114b4c;
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
LAB_80114b4c:
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



/* ======= FUN_80114c6c @ 0x80114c6c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114c6c(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  byte bVar4;
  
  bVar4 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        FUN_8011571c();
        return;
      }
      goto LAB_80114e0c;
    }
    if (bVar4 != 0) {
      FUN_8011571c();
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
LAB_80114e0c:
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



/* ======= FUN_80114f2c @ 0x80114f2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114f2c(void)

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
                  (uint)(byte)(&LAB_8011fdf0)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf0_1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_30);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf0_2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf0_3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
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



/* ======= FUN_8011556c @ 0x8011556c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011556c(undefined4 param_1,int param_2)

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



/* ======= FUN_8011571c @ 0x8011571c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011571c(void)

{
  char cVar1;
  int in_v0;
  
  if (in_v0 == 0) {
    _DAT_800acabe = _DAT_800acabe + 0x800;
  }
  func_0x80045024(0x4000001,_DAT_800ac784 + 0x34);
  _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8011615c();
  return;
}



/* ======= FUN_80115d6c @ 0x80115d6c ======= */
// decompile failed: Exception while decompiling 80115d6c: process: timeout


/* ======= FUN_8011615c @ 0x8011615c ======= */
// decompile failed: Exception while decompiling 8011615c: process: timeout


/* ======= FUN_80116244 @ 0x80116244 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116244(void)

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
    (**(code **)(&LAB_8011fe48 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_801166f4 @ 0x801166f4 ======= */
// decompile failed: Exception while decompiling 801166f4: process: timeout


/* ======= FUN_80116d4c @ 0x80116d4c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d4c(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012a0c(2000);
  func_0x80012974(4000);
  (**(code **)(&LAB_8011fe68 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80116de0 @ 0x80116de0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116de0(void)

{
  (**(code **)(&LAB_8011fe7c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011feb4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80116e5c @ 0x80116e5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116e5c(void)

{
  (**(code **)(&DAT_8011feec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011fef0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80116eec @ 0x80116eec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116eec(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80117770();
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  return;
}



/* ======= FUN_80116f80 @ 0x80116f80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f80(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  
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
      if (DAT_800aca58 == '\x05') goto LAB_80117140;
      if (((sVar4 == 0) && (100 < _DAT_800acae0)) && (*(char *)((int)_DAT_800ac784 + 0x1df) == '\0')
         ) {
        FUN_80117928();
        return;
      }
    }
  }
  else if (_DAT_800ac784[0x74] < 0x9c4) {
    if (DAT_800aca58 == '\x05') goto LAB_80117140;
    if (sVar3 == 0) {
      _DAT_800ac784[1] = 0x401;
    }
  }
  if (DAT_800aca58 != '\x05') {
    return;
  }
LAB_80117140:
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



/* ======= FUN_801171e8 @ 0x801171e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801171e8(void)

{
  bool bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int unaff_s0;
  undefined2 unaff_s1;
  int iVar7;
  short in_lo;
  undefined1 auStackX_0 [16];
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 6);
  uVar4 = (uint)(uVar6 < 2);
  if (uVar6 != 1) {
    if (uVar4 == 0) {
      uVar4 = 2;
      if (uVar6 != 2) goto LAB_80117be0;
      func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                      0x20);
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
      if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
        *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
      *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
      if (cVar2 == '\0') {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      }
      goto LAB_801173e0;
    }
    bVar1 = uVar6 != 0;
    uVar6 = 1;
    if (bVar1) goto LAB_80117be0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_8011bdd8(1,1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x14;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
  }
  iVar5 = func_0x8001a9cc(&DAT_800aca88,0x400);
  if ((iVar5 != 0) && (*(char *)(_DAT_800ac784 + 0x8f) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar6 = _DAT_800ac784;
    uVar4 = 0x5a;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
LAB_80117be0:
    if (((uVar4 - 0x23 < 8) &&
        (iVar5 = func_0x8001a5e0(*(int *)(uVar6 + 0x188) + 0xa14,&stack0xfffffff8,auStackX_0,
                                 &DAT_800aca54), iVar5 != 0)) && (DAT_800acae7 == '\0')) {
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
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar6 == 0x25) {
      func_0x800453d0(4);
      *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
      uVar6 = _DAT_800ac784;
    }
    else if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011bdd8(0,0);
      return;
    }
    if (*(byte *)(uVar6 + 0x95) - 0x47 < 0x1e) {
      iVar5 = *(int *)(uVar6 + 0x188);
      func_0x8003a95c(iVar5 + 0x4b4,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x5de) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar7 + 0x560,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x68a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar7 + 0x60c,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x93a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar7 + 0x8bc,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x9e6) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar7 + 0x968,1);
      *(short *)(iVar5 + 0x532) = *(short *)(iVar5 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801190dc();
    return;
  }
LAB_801173e0:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80117400 @ 0x80117400 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117400(void)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  int unaff_s0;
  int iVar4;
  undefined2 unaff_s1;
  int iVar5;
  short in_lo;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    _DAT_800acaee = 1;
    uVar3 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar3 == 0x25) {
      func_0x800453d0(4);
      *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
      uVar3 = _DAT_800ac784;
    }
    else if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011bdd8(0,0);
      return;
    }
    if (*(byte *)(uVar3 + 0x95) - 0x47 < 0x1e) {
      iVar4 = *(int *)(uVar3 + 0x188);
      func_0x8003a95c(iVar4 + 0x4b4,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(short *)(iVar4 + 0x532) = *(short *)(iVar4 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x532) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x4b4,1);
      *(undefined2 *)(iVar4 + 0x532) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x5de) =
           *(short *)(iVar4 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x560,1);
      *(undefined2 *)(iVar4 + 0x5de) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x68a) =
           *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x60c,1);
      *(undefined2 *)(iVar4 + 0x68a) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x93a) =
           *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x8bc,1);
      *(undefined2 *)(iVar4 + 0x93a) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x9e6) =
           *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x968,1);
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_801190dc();
    return;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x30
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  FUN_8011bdd8(1,1);
  iVar4 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar4 >> 0x10) - (iVar4 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80117510 @ 0x80117510 ======= */
// decompile failed: Exception while decompiling 80117510: process: timeout


/* ======= FUN_80117770 @ 0x80117770 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117770(void)

{
  undefined2 uVar1;
  char cVar2;
  short sVar3;
  int in_v0;
  uint uVar4;
  int iVar5;
  int iVar6;
  short in_lo;
  short sStack00000018;
  short sStack0000001a;
  short in_stack_0000001c;
  int in_stack_00000020;
  int in_stack_00000024;
  int in_stack_00000028;
  char in_stack_00000030;
  
  uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
  *(short *)(_DAT_800ac784 + 0x6a) = (short)in_v0 + -0x400;
  uVar4 = func_0x8001a780();
  *(undefined2 *)(_DAT_800ac784 + 0x6a) = uVar1;
  iVar6 = in_v0 - ((uVar4 & 0xff) * 0x800 + -0x400);
  _DAT_800acabe = (undefined2)iVar6;
  _DAT_800acbfc = _DAT_800ac784;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  _DAT_800aca58 = (uVar4 & 0xff) << 8 | 5;
  func_0x800453d0(7);
  _DAT_800acaee = _DAT_800acaee - 10;
  if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
    _DAT_800acaee = 1;
  }
  in_stack_00000020 = (int)sStack00000018;
  in_stack_00000024 = (int)sStack0000001a;
  iVar5 = (int)in_stack_0000001c;
  in_stack_00000028 = iVar5;
  func_0x8001c2dc(&stack0x00000020,400,&stack0x00000030);
  if (in_stack_00000030 != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
    *(short *)(iVar6 + 0x7e2) = *(short *)(iVar5 + 0x9c) + in_lo * -3;
    func_0x8003a95c();
    *(undefined2 *)(iVar6 + 0x7e2) = uVar1;
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x810,1);
    *(undefined2 *)(iVar6 + 0x88e) = uVar1;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0xb < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x3d < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x61 < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0xca;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == 'n') {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x736) = 0;
      func_0x8003a95c(iVar6 + 0x6b8,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x7e2) = 0;
      func_0x8003a95c(iVar6 + 0x764,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x88e) = 0;
      func_0x8003a95c(iVar6 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x532) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x4b4,1);
      *(undefined2 *)(iVar6 + 0x532) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar6 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x560,1);
      *(undefined2 *)(iVar6 + 0x5de) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar6 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x60c,1);
      *(undefined2 *)(iVar6 + 0x68a) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar6 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      *(undefined2 *)(iVar6 + 0x93a) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar6 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(undefined2 *)(iVar6 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar6 + 0x4b4,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(short *)(iVar6 + 0x532) = *(short *)(iVar6 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x532) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x4b4,1);
      *(undefined2 *)(iVar6 + 0x532) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar6 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x560,1);
      *(undefined2 *)(iVar6 + 0x5de) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar6 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x60c,1);
      *(undefined2 *)(iVar6 + 0x68a) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar6 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      *(undefined2 *)(iVar6 + 0x93a) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar6 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(undefined2 *)(iVar6 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801190dc();
    return;
  }
  FUN_8011bdd8(0,0);
  return;
}



/* ======= FUN_80117928 @ 0x80117928 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117928(undefined4 param_1)

{
  char cVar1;
  short sVar2;
  int in_v0;
  int iVar3;
  int unaff_s0;
  undefined2 unaff_s1;
  int iVar4;
  
  iVar3 = 0x400;
  cVar1 = func_0x8001f314(param_1,*(undefined4 *)(in_v0 + 0x16c));
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) && (*(char *)(_DAT_800ac784 + 0x95) == '\x1e')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(short *)(unaff_s0 + 0x7e2) = *(short *)(iVar3 + 0x9c) + -0x1c2;
    func_0x8003a95c();
    *(undefined2 *)(unaff_s0 + 0x7e2) = unaff_s1;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
    func_0x8003a95c(iVar3 + 0x810,1);
    *(undefined2 *)(iVar3 + 0x88e) = unaff_s1;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0xb < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x3d < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x61 < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0xca;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == 'n') {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x736) = 0;
      func_0x8003a95c(iVar3 + 0x6b8,1);
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x7e2) = 0;
      func_0x8003a95c(iVar3 + 0x764,1);
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x88e) = 0;
      func_0x8003a95c(iVar3 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) =
           *(short *)(iVar3 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x68a) =
           *(short *)(iVar3 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x60c,1);
      *(undefined2 *)(iVar3 + 0x68a) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x93a) =
           *(short *)(iVar3 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x8bc,1);
      *(undefined2 *)(iVar3 + 0x93a) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x9e6) =
           *(short *)(iVar3 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x968,1);
      *(undefined2 *)(iVar3 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar3 + 0x4b4,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x5de) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x560,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x68a) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x60c,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x93a) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x8bc,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x9e6) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x968,1);
      *(short *)(iVar3 + 0x532) = *(short *)(iVar3 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) =
           *(short *)(iVar3 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x68a) =
           *(short *)(iVar3 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x60c,1);
      *(undefined2 *)(iVar3 + 0x68a) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x93a) =
           *(short *)(iVar3 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x8bc,1);
      *(undefined2 *)(iVar3 + 0x93a) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x9e6) =
           *(short *)(iVar3 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x968,1);
      *(undefined2 *)(iVar3 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_801190dc();
    return;
  }
  FUN_8011bdd8(0,0);
  return;
}



/* ======= FUN_801179ec @ 0x801179ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801179ec(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  uint extraout_v1;
  undefined1 *unaff_s0;
  int iVar5;
  undefined2 unaff_s1;
  int iVar6;
  short in_lo;
  undefined8 uVar7;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100404;
  local_14 = DAT_80100408;
  local_10 = DAT_8010040c;
  local_c = DAT_80100410;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_80117b18:
    unaff_s0 = &DAT_800aca88;
    iVar5 = func_0x8001a9cc(&DAT_800aca88,800);
    if (((iVar5 != 0) || (0xdac < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
       (*(byte *)(_DAT_800ac784 + 0x95) < 0xf)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      uVar7 = func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto code_r0x8011852c;
    }
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    if (((*(byte *)(_DAT_800ac784 + 0x95) - 0x23 < 8) &&
        (iVar5 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                                 &DAT_800aca54), iVar5 != 0)) && (DAT_800acae7 == '\0')) {
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
    uVar4 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar4 == 0x25) {
      func_0x800453d0(4);
      goto LAB_801184f4;
    }
LAB_80117cf4:
    if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011bdd8(0,0);
      return;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
        }
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
        goto LAB_80117b18;
      }
    }
    else if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      uVar4 = extraout_v1;
      goto LAB_80117cf4;
    }
LAB_801184f4:
    *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
    func_0x8003a95c();
    *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
    uVar4 = _DAT_800ac784;
  }
  uVar7 = CONCAT44(uVar4,(uint)*(byte *)(uVar4 + 0x95));
code_r0x8011852c:
  if ((int)uVar7 - 0x47U < 0x1e) {
    iVar5 = *(int *)((int)((ulonglong)uVar7 >> 0x20) + 0x188);
    func_0x8003a95c(iVar5 + 0x4b4,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x5de) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar6 + 0x560,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x68a) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x60c,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x93a) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar6 + 0x8bc,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x9e6) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x968,1);
    *(short *)(iVar5 + 0x532) = *(short *)(iVar5 + 0x532) + -0x88;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x532) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar5 + 0x4b4,1);
    *(undefined2 *)(iVar5 + 0x532) = 0;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x5de) = *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar5 + 0x560,1);
    *(undefined2 *)(iVar5 + 0x5de) = 0;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x68a) = *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar5 + 0x60c,1);
    *(undefined2 *)(iVar5 + 0x68a) = 0;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x93a) =
         *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar5 + 0x8bc,1);
    *(undefined2 *)(iVar5 + 0x93a) = 0;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x9e6) = *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar5 + 0x968,1);
    *(undefined2 *)(iVar5 + 0x9e6) = 0;
  }
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_801190dc();
  return;
}



/* ======= FUN_80117d44 @ 0x80117d44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d44(void)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  uint extraout_v1;
  int unaff_s0;
  int iVar4;
  int unaff_s2;
  undefined4 local_18;
  undefined4 local_14;
  uint local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100414;
  local_14 = DAT_80100418;
  local_10 = DAT_8010041c;
  local_c = DAT_80100420;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar3 != 1) {
    if (1 < uVar3) {
      sVar2 = 0x101;
      if (uVar3 != 2) goto code_r0x80118778;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto LAB_80117f74;
    }
    sVar2 = 1;
    if (uVar3 != 0) goto code_r0x80118778;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    func_0x800453d0(3);
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(char *)(_DAT_800ac784 + 0x95) == ',') {
    iVar4 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                            &DAT_800aca54);
    if (iVar4 != 0) {
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      _DAT_800aca58 = 6;
      _DAT_800acc1e = (undefined2)local_c;
      _DAT_800acc1c = (undefined2)local_10;
      *(short *)(unaff_s0 + 0x5de) =
           *(short *)(unaff_s0 + 0x5de) +
           *(char *)(*(int *)((local_10 & 0xffff) - 0x387c) + 0x9e) * -6;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x5de) = 0;
      unaff_s0 = *(int *)(_DAT_800ac784 + 0x188);
      unaff_s2 = unaff_s0 + 0x60c;
      sVar2 = func_0x8001af20();
      uVar3 = _DAT_800ac784;
      goto code_r0x80118778;
    }
    func_0x800453d0(6);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) != '%') {
LAB_80117f74:
    FUN_8011bdd8(0,1);
    return;
  }
  sVar2 = func_0x800453d0(4);
  uVar3 = extraout_v1;
code_r0x80118778:
  *(short *)(unaff_s0 + 0x68a) = *(short *)(unaff_s0 + 0x68a) + *(char *)(uVar3 + 0x9e) * sVar2;
  func_0x8003a95c(unaff_s2,1);
  *(undefined2 *)(unaff_s0 + 0x68a) = 0;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar2 = func_0x8001af20();
  *(short *)(iVar4 + 0x93a) =
       *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
  func_0x8003a95c(iVar4 + 0x8bc,1);
  *(undefined2 *)(iVar4 + 0x93a) = 0;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar2 = func_0x8001af20();
  *(short *)(iVar4 + 0x9e6) = *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
  func_0x8003a95c(iVar4 + 0x968,1);
  *(undefined2 *)(iVar4 + 0x9e6) = 0;
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_801190dc();
  return;
}



/* ======= FUN_80117f94 @ 0x80117f94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117f94(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  int unaff_s0;
  int iVar6;
  undefined4 uVar7;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x1dc) = 9;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  uVar4 = (uint)(bVar1 < 2);
  if (bVar1 == 1) {
LAB_801180cc:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    if (((cVar2 == '`') || (cVar2 == '<')) || (cVar2 == 'x')) {
      func_0x800453d0(8);
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == -0x6a) {
      func_0x800453d0(9);
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x6e) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x736) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 3;
      func_0x8003a95c(iVar5 + 0x6b8,1);
      *(undefined2 *)(iVar5 + 0x736) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x7e2) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -3;
      func_0x8003a95c(iVar5 + 0x764,1);
      *(undefined2 *)(iVar5 + 0x7e2) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x88e) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x810,1);
      *(undefined2 *)(iVar5 + 0x88e) = 0xfff;
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
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x736) = 0;
      func_0x8003a95c(iVar5 + 0x6b8,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x7e2) = 0;
      func_0x8003a95c(iVar5 + 0x764,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x88e) = 0;
      func_0x8003a95c(iVar5 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar5 + 0x4b4,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x560,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x60c,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(short *)(iVar5 + 0x532) = *(short *)(iVar5 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801190dc();
    return;
  }
  if (uVar4 == 0) {
    uVar4 = 0x101;
    if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
      *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) & 0xf7;
      FUN_8011bdd8(0,1);
      return;
    }
  }
  else if (bVar1 == 0) {
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
    goto LAB_801180cc;
  }
  iVar5 = (uint)*(ushort *)(unaff_s0 + 0xa92) - (uint)*(ushort *)(uVar4 + 0x9c);
  *(short *)(unaff_s0 + 0xa92) = (short)iVar5;
  if (iVar5 * 0x10000 < 0) {
    *(undefined2 *)(unaff_s0 + 0xa92) = 0;
  }
  func_0x8003a95c(unaff_s0 + 0xa14,1);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
  if (*(short *)(unaff_s0 + 0xa92) < 1) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
  }
  iVar5 = *(int *)(_DAT_800ac784 + 0x84);
  uVar7 = 0x80119164;
  func_0x8001f314(iVar5,*(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  bVar1 = *(byte *)(iVar5 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(iVar5 + 6) = 1;
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
                  0x200,uVar7);
  if (*(uint *)(_DAT_800ac784 + 0x1d0) < 500) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  FUN_8011bdd8(1,1);
  iVar5 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar5 >> 0x10) - (iVar5 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8011890c @ 0x8011890c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011890c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100404;
  local_14 = DAT_80100408;
  local_10 = DAT_801003e0;
  local_c = DAT_801003e4;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80119284();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      *(undefined1 *)(_DAT_800ac784 + 0x1df) = 10;
      if ((*(char *)(_DAT_800ac784 + 0x95) == '\x01') || (*(char *)(_DAT_800ac784 + 0x95) == '\x12')
         ) {
        func_0x800453d0(4);
      }
      if ((((*(byte *)(_DAT_800ac784 + 0x95) - 4 < 10) ||
           (*(byte *)(_DAT_800ac784 + 0x95) - 0x12 < 10)) &&
          (iVar3 = func_0x8001a5e0(iVar3 + 0xa14,&local_18,&local_10,&DAT_800aca54), iVar3 != 0)) &&
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
    if (bVar1 != 0) {
      FUN_80119284();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80119284();
  return;
}



/* ======= FUN_80118d14 @ 0x80118d14 ======= */
// decompile failed: Exception while decompiling 80118d14: process: timeout


/* ======= FUN_801190dc @ 0x801190dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801190dc(void)

{
  byte bVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  iVar3 = *(int *)(in_v0 + 0x188);
  iVar2 = (uint)*(ushort *)(iVar3 + 0xa92) - (uint)*(ushort *)(in_v0 + 0x9c);
  *(short *)(iVar3 + 0xa92) = (short)iVar2;
  if (iVar2 * 0x10000 < 0) {
    *(undefined2 *)(iVar3 + 0xa92) = 0;
  }
  func_0x8003a95c(iVar3 + 0xa14,1);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
  if (*(short *)(iVar3 + 0xa92) < 1) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
  }
  iVar2 = *(int *)(_DAT_800ac784 + 0x84);
  func_0x8001f314(iVar2,*(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  bVar1 = *(byte *)(iVar2 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(iVar2 + 6) = 1;
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
  FUN_8011bdd8(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80119284 @ 0x80119284 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119284(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  char cVar2;
  int in_v0;
  int iVar3;
  
  cVar2 = func_0x8001f314(param_1,*(undefined4 *)(in_v0 + 0x16c),param_3,0x400);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  bVar1 = *(byte *)(iVar3 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(iVar3 + 6) = 1;
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
  FUN_8011bdd8(1,1);
  iVar3 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar3 >> 0x10) - (iVar3 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8011938c @ 0x8011938c ======= */
// decompile failed: Exception while decompiling 8011938c: process: timeout


/* ======= FUN_80119538 @ 0x80119538 ======= */
// decompile failed: Exception while decompiling 80119538: process: timeout


/* ======= FUN_80119b64 @ 0x80119b64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119b64(void)

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
  FUN_8011bdd8(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80119cc8 @ 0x80119cc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119cc8(void)

{
  short sVar1;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar1 == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  return;
}



/* ======= FUN_80119d1c @ 0x80119d1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119d1c(void)

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
    FUN_8011a674();
    return;
  }
  func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x10);
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_8011bdd8(1,1);
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



/* ======= FUN_80119e8c @ 0x80119e8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119e8c(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011a864();
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
      FUN_8011a864();
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
  FUN_8011a864();
  return;
}



/* ======= FUN_8011a074 @ 0x8011a074 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a074(void)

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
      FUN_8011aa7c();
      return;
    }
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee00f8))();
  return;
}



/* ======= FUN_8011a298 @ 0x8011a298 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a298(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011abf4();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      return;
    }
    if (bVar1 != 0) {
      FUN_8011abf4();
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
  FUN_8011abf4();
  return;
}



/* ======= FUN_8011a404 @ 0x8011a404 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a404(void)

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
    FUN_8011add4();
    return;
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedfe58))();
  return;
}



/* ======= FUN_8011a5ec @ 0x8011a5ec ======= */
// decompile failed: Exception while decompiling 8011a5ec: process: timeout


/* ======= FUN_8011a674 @ 0x8011a674 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a674(void)

{
  int in_v1;
  
  *(byte *)(in_v1 + 0x93) = *(byte *)(in_v1 + 0x93) | 1;
  func_0x800453d0(1);
  FUN_8011af50();
  return;
}



/* ======= FUN_8011a864 @ 0x8011a864 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a864(uint param_1)

{
  uint *puVar1;
  char cVar2;
  undefined1 *in_v0;
  code *pcVar3;
  int iVar4;
  byte bVar5;
  uint *unaff_s0;
  undefined4 in_stack_0000001c;
  
  *in_v0 = 1;
  DAT_800acae8 = 0xd;
  DAT_800acae9 = 6;
  DAT_800acae3 = 3;
  _DAT_800acae0 = 0x4b0;
  DAT_800acc0c = 1;
  _DAT_800aca3c = _DAT_800aca3c | 0x40;
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x80045024(param_1 | 1);
  cVar2 = func_0x8001f314(_DAT_800acad8,_DAT_800acbc0,0,0x400);
  bVar5 = DAT_800aca5a + cVar2;
  _DAT_800acae0 = _DAT_800acae0 + -0x96;
  _DAT_800aca58 = CONCAT12(bVar5,_DAT_800aca58);
  if (_DAT_800acae0 < 400) {
    _DAT_800acae0 = 400;
  }
  if ((_DAT_800aca54 & 0x10) == 0) {
    func_0x800245d8(0x800);
    return;
  }
  in_stack_0000001c = 5;
  if (bVar5 != 1) {
    pcVar3 = (code *)(uint)(bVar5 < 2);
    if (pcVar3 == (code *)0x0) {
      pcVar3 = (code *)&DAT_00000007;
      if (bVar5 == 2) {
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x8011bdb4;
    }
    if (bVar5 != 0) goto code_r0x8011bdb4;
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
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]));
    if (300 < iVar4) {
      *(undefined2 *)((int)puVar1 + 0x8a) = 3000;
      *(undefined2 *)(puVar1 + 0x22) = 5000;
      *(undefined2 *)(puVar1 + 0x23) = 5000;
      puVar1[0x25] = 0x70;
      *puVar1 = *puVar1 | 0x2000;
      puVar1[0x26] = 0x20;
      *puVar1 = *puVar1 | 0x80;
      func_0x80019700(0x3000,0,puVar1 + 0x10,&stack0x00000010);
    }
    puVar1 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]));
    if (300 < iVar4) {
      puVar1[0x56] = puVar1[0x56] | 0x80;
      puVar1[0x7b] = 0x70;
      puVar1[0x7c] = 0x20;
      *(undefined2 *)(puVar1 + 0x78) = 5000;
      *(undefined2 *)((int)puVar1 + 0x1e2) = 3000;
      *(undefined2 *)(puVar1 + 0x79) = 5000;
      puVar1[0x56] = puVar1[0x56] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0x66,&stack0x00000010);
      puVar1[0xd7] = puVar1[0xd7] | 0x80;
      puVar1[0xfc] = 0x70;
      puVar1[0xfd] = 0x20;
      *(undefined2 *)(puVar1 + 0xf9) = 5000;
      *(undefined2 *)((int)puVar1 + 0x3e6) = 3000;
      *(undefined2 *)(puVar1 + 0xfa) = 5000;
      puVar1[0xd7] = puVar1[0xd7] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0xe7,&stack0x00000010);
    }
    puVar1 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]));
    if (300 < iVar4) {
      puVar1[0x50] = 0x70;
      puVar1[0x51] = 0x20;
      *(undefined2 *)(puVar1 + 0x4d) = 6000;
      *(undefined2 *)(puVar1 + 0x4e) = 6000;
      puVar1[0x2b] = puVar1[0x2b] | 0x80;
      *(undefined2 *)((int)puVar1 + 0x136) = 3000;
      puVar1[0x2b] = puVar1[0x2b] | 0x2000;
      func_0x80019700(0x3000,0,puVar1 + 0x3b,&stack0x00000010);
    }
    unaff_s0 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]));
    if (300 < iVar4) {
      unaff_s0[0x17d] = 0x70;
      unaff_s0[0x17e] = 0x20;
      *(undefined2 *)(unaff_s0 + 0x17a) = 6000;
      *(undefined2 *)(unaff_s0 + 0x17b) = 6000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x80;
      *(undefined2 *)((int)unaff_s0 + 0x5ea) = 3000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x2000;
      func_0x80019700(0x3000,0,unaff_s0 + 0x168,&stack0x00000010);
    }
  }
  pcVar3 = (code *)func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + (char)pcVar3,_DAT_800aca58);
code_r0x8011bdb4:
  (*pcVar3)();
  func_0x8002b498(_DAT_800ac784);
  func_0x8002aec4(unaff_s0 + 5,_DAT_800ac784);
  func_0x8002b544();
  func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  func_0x80037358();
  func_0x8001b38c();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011aa7c @ 0x8011aa7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011aa7c(void)

{
  int iVar1;
  code *pcVar2;
  int in_v1;
  int iVar3;
  int iVar4;
  undefined2 unaff_s0;
  undefined2 unaff_s1;
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  int unaff_s4;
  
  DAT_800aca5a = 8;
  DAT_800acae8 = 0;
  _DAT_800acaf0 = 0x4b;
  if (in_v1 < 0x1e) {
    _DAT_800acaf0 = 0x113;
  }
  if (in_v1 < 10) {
    _DAT_800acaf0 = _DAT_800acaf0 + 0x17c;
  }
  func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  iVar1 = func_0x80037024();
  iVar1 = (_DAT_800acaf0 - 4) + iVar1 * -9;
  _DAT_800acaf0 = (ushort)iVar1;
  if (iVar1 * 0x10000 < 0) {
    DAT_800aca5a = '\t';
    *(uint *)(unaff_s4 + 0x35c) = *(uint *)(unaff_s4 + 0x35c) | 0x80;
    *(undefined4 *)(unaff_s4 + 0x3f0) = unaff_s3;
    *(undefined4 *)(unaff_s4 + 0x3f4) = unaff_s2;
    *(undefined2 *)(unaff_s4 + 0x3e4) = unaff_s0;
    *(undefined2 *)(unaff_s4 + 0x3e6) = unaff_s1;
    *(undefined2 *)(unaff_s4 + 1000) = unaff_s0;
    *(uint *)(unaff_s4 + 0x35c) = *(uint *)(unaff_s4 + 0x35c) | 0x2000;
    func_0x80019700(0x2800,0,unaff_s4 + 0x39c);
    iVar1 = _DAT_800acbdc;
    iVar3 = (int)_DAT_800acc1c - *(int *)(_DAT_800acbdc + 0x100);
    iVar4 = (int)_DAT_800acc1e - *(int *)(_DAT_800acbdc + 0x108);
    iVar3 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    if (300 < iVar3) {
      *(undefined4 *)(iVar1 + 0x140) = 0x70;
      *(undefined4 *)(iVar1 + 0x144) = 0x20;
      *(undefined2 *)(iVar1 + 0x134) = 6000;
      *(undefined2 *)(iVar1 + 0x138) = 6000;
      *(uint *)(iVar1 + 0xac) = *(uint *)(iVar1 + 0xac) | 0x80;
      *(undefined2 *)(iVar1 + 0x136) = 3000;
      *(uint *)(iVar1 + 0xac) = *(uint *)(iVar1 + 0xac) | 0x2000;
      func_0x80019700(0x3000,0,iVar1 + 0xec);
    }
    iVar1 = _DAT_800acbdc;
    iVar3 = (int)_DAT_800acc1c - *(int *)(_DAT_800acbdc + 0x5b4);
    iVar4 = (int)_DAT_800acc1e - *(int *)(_DAT_800acbdc + 0x5bc);
    iVar3 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    if (300 < iVar3) {
      *(undefined4 *)(iVar1 + 0x5f4) = 0x70;
      *(undefined4 *)(iVar1 + 0x5f8) = 0x20;
      *(undefined2 *)(iVar1 + 0x5e8) = 6000;
      *(undefined2 *)(iVar1 + 0x5ec) = 6000;
      *(uint *)(iVar1 + 0x560) = *(uint *)(iVar1 + 0x560) | 0x80;
      *(undefined2 *)(iVar1 + 0x5ea) = 3000;
      *(uint *)(iVar1 + 0x560) = *(uint *)(iVar1 + 0x560) | 0x2000;
      func_0x80019700(0x3000,0,iVar1 + 0x5a0);
    }
    pcVar2 = (code *)func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
    DAT_800aca5a = DAT_800aca5a + (char)pcVar2;
    (*pcVar2)();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(iVar1 + 0x14,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
    func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
    return;
  }
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_8011abf4 @ 0x8011abf4 ======= */

void FUN_8011abf4(void)

{
  return;
}



/* ======= FUN_8011add4 @ 0x8011add4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011add4(void)

{
  undefined2 uVar1;
  int iVar2;
  int in_v1;
  int iVar3;
  int unaff_s3;
  ushort in_stack_0000002c;
  
  iVar3 = (_DAT_800acaf0 - 4) + in_v1 * -2;
  _DAT_800acaf0 = (ushort)iVar3;
  if (iVar3 * 0x10000 < 0) {
    DAT_800aca5a = 4;
    iVar3 = (in_v1 * -2 + 4) * 0x10000 >> 0x10;
    iVar2 = (int)(((uint)in_stack_0000002c - (uint)*(ushort *)(unaff_s3 + 0x5c)) * 0x10000) >> 0x10;
    uVar1 = func_0x80065f60(iVar3 * iVar3 + iVar2 * iVar2);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
    return;
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8011af50 @ 0x8011af50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011af50(uint param_1,undefined4 param_2,undefined1 param_3)

{
  char cVar1;
  uint in_v0;
  char in_v1;
  
  DAT_800acae9 = 0;
  _DAT_800aca3c = in_v0 | 0x40;
  DAT_800acae7 = DAT_800acae7 | 1;
  DAT_800acaf2 = in_v1;
  DAT_800acc0c = param_3;
  func_0x80045024(param_1 | 1);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  DAT_800acaf2 = DAT_800acaf2 - cVar1;
  if (DAT_800acaf2 == '\0') {
    DAT_800aca5a = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    func_0x8001af5c(0,0,600,700);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 4;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    return;
  }
  return;
}



/* ======= FUN_8011b1bc @ 0x8011b1bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b1bc(void)

{
  uint *puVar1;
  code *pcVar2;
  int iVar3;
  uint *unaff_s0;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  local_30 = DAT_80100068;
  local_2c = DAT_8010006c;
  local_28 = DAT_80100070;
  local_24 = DAT_80100074;
  if (DAT_800aca5a != 1) {
    pcVar2 = (code *)(uint)(DAT_800aca5a < 2);
    if (pcVar2 == (code *)0x0) {
      pcVar2 = (code *)&DAT_00000007;
      if (DAT_800aca5a == 2) {
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x8011bdb4;
    }
    if (DAT_800aca5a != 0) goto code_r0x8011bdb4;
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
    unaff_s0 = _DAT_800acbdc;
    iVar3 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]));
    if (300 < iVar3) {
      unaff_s0[0x17d] = 0x70;
      unaff_s0[0x17e] = 0x20;
      *(undefined2 *)(unaff_s0 + 0x17a) = 6000;
      *(undefined2 *)(unaff_s0 + 0x17b) = 6000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x80;
      *(undefined2 *)((int)unaff_s0 + 0x5ea) = 3000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x2000;
      func_0x80019700(0x3000,0,unaff_s0 + 0x168,&local_30);
    }
  }
  pcVar2 = (code *)func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + (char)pcVar2,_DAT_800aca58);
code_r0x8011bdb4:
  (*pcVar2)();
  func_0x8002b498(_DAT_800ac784);
  func_0x8002aec4(unaff_s0 + 5,_DAT_800ac784);
  func_0x8002b544();
  func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  func_0x80037358();
  func_0x8001b38c();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011b5d8 @ 0x8011b5d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b5d8(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int aiStack_38 [5];
  uint local_24;
  uint local_1c;
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,aiStack_38);
  iVar3 = iVar3 + param_2 * 0x204 + 0x204;
  func_0x80022da0(aiStack_38,iVar3 + -0x140,aiStack_38);
  func_0x80022da0(aiStack_38,iVar3 + -0x94,aiStack_38);
  func_0x80022da0(aiStack_38,iVar3 + 0x18,aiStack_38);
  if (param_1 == 0) {
    *(uint *)(_DAT_800ac784 + 0x34) =
         *(int *)(_DAT_800ac784 + 0x34) - (local_24 - *(int *)(iVar3 + 0x54));
    iVar2 = local_1c - *(int *)(iVar3 + 0x5c);
    iVar3 = *(int *)(_DAT_800ac784 + 0x3c) - iVar2;
    *(int *)(_DAT_800ac784 + 0x3c) = iVar3;
    *(short *)(iVar2 + 0x5fe) = (short)iVar3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    aiStack_38[1] = 0x808080;
    aiStack_38[0] = _DAT_800ac784 + 0xb0;
    func_0x8001af5c(0,0,600,700);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 4;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    return;
  }
  iVar2 = (int)(((local_24 & 0xffff) - (uint)*(ushort *)(iVar3 + 0x54)) * 0x10000) >> 0x10;
  iVar3 = (int)(((local_1c & 0xffff) - (uint)*(ushort *)(iVar3 + 0x5c)) * 0x10000) >> 0x10;
  uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  return;
}



/* ======= FUN_8011b738 @ 0x8011b738 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b738(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedfb78))();
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



/* ======= FUN_8011b874 @ 0x8011b874 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b874(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80120484;
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



/* ======= FUN_8011ba1c @ 0x8011ba1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ba1c(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x5dc,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
      func_0x800369f8(0,1);
    }
    return;
  }
  uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedfb64))();
  return;
}



/* ======= FUN_8011bae8 @ 0x8011bae8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bae8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfb60))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfb40))();
  return;
}



/* ======= FUN_8011bb64 @ 0x8011bb64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bb64(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfb20))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfb00))();
  return;
}



/* ======= FUN_8011bbe0 @ 0x8011bbe0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bbe0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedfae0))();
  return;
}



/* ======= FUN_8011bc28 @ 0x8011bc28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc28(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedfae0))();
  return;
}



/* ======= FUN_8011bc70 @ 0x8011bc70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc70(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfae0))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011bd08 @ 0x8011bd08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bd08(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedfaa8))();
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



/* ======= FUN_8011bdd8 @ 0x8011bdd8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bdd8(void)

{
  int unaff_s0;
  
  func_0x8002b544(unaff_s0 + 0x14);
  func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  func_0x80037358();
  func_0x8001b38c();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011be44 @ 0x8011be44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011be44(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80120554;
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



/* ======= FUN_8011bfec @ 0x8011bfec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bfec(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x800,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    if ((_DAT_000001c5 & 2) != 0) {
      func_0x800369f8(0,1);
    }
    return;
  }
  uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedfa94))();
  return;
}



/* ======= FUN_8011c0b8 @ 0x8011c0b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c0b8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfa90))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfa70))();
  return;
}



/* ======= FUN_8011c134 @ 0x8011c134 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c134(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfa50))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfa30))();
  return;
}



/* ======= FUN_8011c1b0 @ 0x8011c1b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c1b0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedfa10))();
  return;
}



/* ======= FUN_8011c1f8 @ 0x8011c1f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c1f8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedfa10))();
  return;
}



/* ======= FUN_8011c240 @ 0x8011c240 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c240(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedfa10))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c2d8 @ 0x8011c2d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c2d8(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedf9d8))();
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



/* ======= FUN_8011c408 @ 0x8011c408 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c408(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80120624;
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



/* ======= FUN_8011c5b0 @ 0x8011c5b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c5b0(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x800,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    return;
  }
  uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedf9c4))();
  return;
}



/* ======= FUN_8011c67c @ 0x8011c67c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c67c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf9c0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf9a0))();
  return;
}



/* ======= FUN_8011c6f8 @ 0x8011c6f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c6f8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf980))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf960))();
  return;
}



/* ======= FUN_8011c774 @ 0x8011c774 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c774(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedf940))();
  return;
}



/* ======= FUN_8011c7bc @ 0x8011c7bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c7bc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedf940))();
  return;
}



/* ======= FUN_8011c804 @ 0x8011c804 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c804(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf940))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c89c @ 0x8011c89c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c89c(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedf908))();
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



/* ======= FUN_8011c9e0 @ 0x8011c9e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c9e0(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_801206f4;
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



/* ======= FUN_8011cb88 @ 0x8011cb88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cb88(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x578,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    (**(code **)((uint)DAT_00000007 * 4 + -0x7fedf788))();
    return;
  }
  uVar2 = func_0x8005070c(700,(int)local_10[0],-(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  if (((*(byte *)(_DAT_800ac784 + 0x1c2) & 1) != 0) && (100 < _DAT_800acae0)) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_8011d4a4();
    return;
  }
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedf8f4))();
  return;
}



/* ======= FUN_8011ccb4 @ 0x8011ccb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ccb4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf8ec))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf8cc))();
  return;
}



/* ======= FUN_8011cd30 @ 0x8011cd30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cd30(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf8ac))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf88c))();
  return;
}



/* ======= FUN_8011cdac @ 0x8011cdac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cdac(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf86c))();
  return;
}



/* ======= FUN_8011cdf4 @ 0x8011cdf4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cdf4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedf858))();
  return;
}



/* ======= FUN_8011ce3c @ 0x8011ce3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce3c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf858))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011ced4 @ 0x8011ced4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ced4(void)

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
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedf820))();
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



/* ======= FUN_8011d04c @ 0x8011d04c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d04c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_801207dc;
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



/* ======= FUN_8011d1f4 @ 0x8011d1f4 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d1f4(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  uVar2 = func_0x8005070c(1000,(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedf80c))();
  return;
}



/* ======= FUN_8011d2c0 @ 0x8011d2c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d2c0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf808))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf7e8))();
  return;
}



/* ======= FUN_8011d33c @ 0x8011d33c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d33c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf7c8))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf7a8))();
  return;
}



/* ======= FUN_8011d3b8 @ 0x8011d3b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d3b8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedf788))();
  return;
}



/* ======= FUN_8011d400 @ 0x8011d400 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d400(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fedf788))();
  return;
}



/* ======= FUN_8011d448 @ 0x8011d448 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d448(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf788))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011d4a4 @ 0x8011d4a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d4a4(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011d4e0 @ 0x8011d4e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d4e0(void)

{
  _DAT_80072bec = &LAB_80100558;
  _DAT_80072bf0 = &LAB_80100558;
  _DAT_80072bf4 = &LAB_80100558;
  _DAT_80072c04 = &LAB_80100558;
  _DAT_80072c0c = &LAB_80100558;
  _DAT_80072c1c = &LAB_80100558;
  _DAT_80072c20 = &LAB_80100558;
  _DAT_80072c24 = &LAB_80100558;
  _DAT_80072c28 = &LAB_80100558;
  _DAT_80072c50 = &LAB_8010c320;
  _DAT_80072c58 = 0x80111a50;
  _DAT_80072c6c = 0x80116a44;
  _DAT_80072cac = 0x8011bf38;
  _DAT_80072cb4 = 0x8011c508;
  _DAT_80072cd0 = 0x8011cad8;
  _DAT_80072cd8 = 0x8011d09c;
  _DAT_80072ce0 = 0x8011d6d4;
  _DAT_800b52c8 = &DAT_8011df18;
  _DAT_800ac998 = &LAB_8011e1b8;
  _DAT_80072bf8 = 0x8010a9fc;
  _DAT_800ac798 = 0x8010a3c0;
  _DAT_800ac79c = 0x8010a3c0;
  _DAT_800ac7a0 = 0x8010a3c0;
  _DAT_800ac7b0 = 0x8010a3c0;
  _DAT_800ac7b8 = 0x8010a3c0;
  _DAT_800ac7c8 = 0x8010a3c0;
  _DAT_800ac7cc = 0x8010a3c0;
  _DAT_800ac7d0 = 0x8010a3c0;
  _DAT_800ac7d4 = 0x8010a3c0;
  _DAT_800ac7fc = 0x80111550;
  _DAT_800ac804 = 0x80115e40;
  _DAT_800ac7a4 = &LAB_8010c2a0;
  _DAT_800ac818 = 0x8011afd8;
  _DAT_800ac898 = 0x8010a7ec;
  _DAT_800ac89c = 0x8010a7ec;
  _DAT_800ac8a0 = 0x8010a7ec;
  _DAT_800ac8b0 = 0x8010a7ec;
  _DAT_800ac8b8 = 0x8010a7ec;
  _DAT_800ac8c8 = 0x8010a7ec;
  _DAT_800ac8cc = 0x8010a7ec;
  _DAT_800ac8d0 = 0x8010a7ec;
  _DAT_800ac8d4 = 0x8010a7ec;
  _DAT_800ac8fc = 0x80111884;
  _DAT_800ac8a4 = &LAB_8010c2e0;
  _DAT_800ac904 = 0x8011635c;
  _DAT_800ac918 = 0x8011afd8;
  func_0x80029afc();
  return;
}



