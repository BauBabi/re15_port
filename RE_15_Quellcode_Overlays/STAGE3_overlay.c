/* ======= FUN_80100000 @ 0x80100000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(void)

{
  ushort uVar1;
  ushort uVar2;
  int in_v0;
  uint uVar3;
  int iVar4;
  byte bVar5;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1bc) = *(undefined2 *)(in_v0 + -0x3578);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = DAT_8011d8e0;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011d934)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee2e38);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&LAB_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (**(code **)((uVar3 & 7) * 4 + -0x7fee26c4))();
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
                    (uint)(byte)(&LAB_8011d8f4)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000028);
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_1)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_2)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_3)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f8)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f9)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fa)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fb)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
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



/* ======= FUN_801008f4 @ 0x801008f4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801008f4(void)

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



/* ======= FUN_80100944 @ 0x80100944 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100944(void)

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



/* ======= FUN_80100a28 @ 0x80100a28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a28(void)

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



/* ======= FUN_80100b10 @ 0x80100b10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100b10(undefined4 param_1)

{
  uint uVar1;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011d8f4)[(unaff_s0 & 0xff) * 8] * 0xac +
                               _DAT_800ac784[0x62] + 0x40);
      return;
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
  uVar1 = func_0x8001bc08(param_1);
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
  (**(code **)(&LAB_8011d95c + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80100f78 @ 0x80100f78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f78(void)

{
  (**(code **)(&LAB_8011d990 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011d9e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100ff4 @ 0x80100ff4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100ff4(void)

{
  (**(code **)(&LAB_8011da30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011da70 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101070 @ 0x80101070 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101070(void)

{
  (**(code **)(&LAB_8011dab0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011d9e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801010ec @ 0x801010ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010ec(void)

{
  (**(code **)(&LAB_8011db00 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011da70 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101168 @ 0x80101168 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101168(void)

{
  (**(code **)(&LAB_8011db18 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011db1c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801011e4 @ 0x801011e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011e4(void)

{
  (**(code **)(&LAB_8011db20 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011db24 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101260 @ 0x80101260 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101260(void)

{
  (**(code **)(&LAB_8011db28 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011db24 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801012dc @ 0x801012dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801012dc(void)

{
  (**(code **)(&LAB_8011db2c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011db24 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101358 @ 0x80101358 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101358(void)

{
  (**(code **)(&LAB_8011db30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011db34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801013d4 @ 0x801013d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801013d4(void)

{
  (**(code **)(&LAB_8011db38 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011db3c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101450 @ 0x80101450 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101450(void)

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



/* ======= FUN_80101568 @ 0x80101568 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101568(void)

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



/* ======= FUN_801015f4 @ 0x801015f4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801015f4(void)

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



/* ======= FUN_801016d0 @ 0x801016d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016d0(void)

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



/* ======= FUN_801017dc @ 0x801017dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801017dc(void)

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
    FUN_80102128(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80101944 @ 0x80101944 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101944(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
    FUN_801022d0();
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



/* ======= FUN_80101ae4 @ 0x80101ae4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ae4(void)

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
      *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011dc40)[uVar5 & 0x1f];
      FUN_8010241c();
      return;
    }
    uVar5 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x1de) =
         *(undefined1 *)((int)&PTR_LAB_8011dc50 + (uVar5 & 0x1f));
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_8010260c(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80101e34 @ 0x80101e34 ======= */
// decompile failed: Exception while decompiling 80101e34: process: timeout


/* ======= FUN_80102128 @ 0x80102128 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102128(short param_1,int param_2)

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
  FUN_80102ca8();
  return;
}



/* ======= FUN_801022d0 @ 0x801022d0 ======= */

void FUN_801022d0(void)

{
  int in_v1;
  int iVar1;
  
  iVar1 = *(int *)(in_v1 + -0x387c);
  if (((((*(byte *)(iVar1 + 0x1c2) & 2) != 0) && (7 < *(byte *)(iVar1 + 0x95))) &&
      (iVar1 = *(int *)(iVar1 + 0x1ac), -1 < *(short *)(iVar1 + 0x9a))) &&
     (((*(byte *)(iVar1 + 9) & 0x80) == 0 && ((*(ushort *)(iVar1 + 0x1d8) & 1) == 0)))) {
    *(undefined4 *)(iVar1 + 4) = 0xb01;
    FUN_80102ca8();
    return;
  }
  return;
}



/* ======= FUN_8010241c @ 0x8010241c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010241c(void)

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
    FUN_80102ca8();
    return;
  }
  return;
}



/* ======= FUN_801024c4 @ 0x801024c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801024c4(void)

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



/* ======= FUN_8010260c @ 0x8010260c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010260c(void)

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



/* ======= FUN_801026b4 @ 0x801026b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801026b4(void)

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



/* ======= FUN_80102808 @ 0x80102808 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102808(void)

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
          FUN_8010955c(0,0);
        }
      }
      else if ((*(ushort *)(iVar3 + 0x1d8) & 0x40) == 0) {
        FUN_80103928(0,1);
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



/* ======= FUN_80102900 @ 0x80102900 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102900(void)

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
  FUN_80109488(0,0);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80102ad0 @ 0x80102ad0 ======= */
// decompile failed: Exception while decompiling 80102ad0: process: timeout


/* ======= FUN_80102ca8 @ 0x80102ca8 ======= */

void FUN_80102ca8(void)

{
  return;
}



/* ======= FUN_80102cb4 @ 0x80102cb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102cb4(void)

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
        FUN_801036d4();
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
      FUN_801036d4();
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
  FUN_801036d4();
  return;
}



/* ======= FUN_80102ee4 @ 0x80102ee4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102ee4(void)

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



/* ======= FUN_80102fc8 @ 0x80102fc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102fc8(void)

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
        FUN_8010955c(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_80103928(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_80103148 @ 0x80103148 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103148(void)

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



/* ======= FUN_8010326c @ 0x8010326c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010326c(void)

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



/* ======= FUN_801032e8 @ 0x801032e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801032e8(void)

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



/* ======= FUN_80103344 @ 0x80103344 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103344(uint param_1)

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



/* ======= FUN_80103480 @ 0x80103480 ======= */
// decompile failed: Exception while decompiling 80103480: process: timeout


/* ======= FUN_801036d4 @ 0x801036d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801036d4(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x40,&LAB_8011d924);
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
  FUN_80104b24();
  return;
}



/* ======= FUN_80103928 @ 0x80103928 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103928(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_8011d8d4)[*(byte *)(param_3 + 8)] * 0xac +
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
  FUN_80104b24();
  return;
}



/* ======= FUN_80103a64 @ 0x80103a64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a64(void)

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
    puVar3 = (uint *)((uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80103e34 @ 0x80103e34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103e34(void)

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



/* ======= FUN_80103f58 @ 0x80103f58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103f58(void)

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



/* ======= FUN_8010404c @ 0x8010404c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010404c(void)

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



/* ======= FUN_801040f4 @ 0x801040f4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801040f4(void)

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



/* ======= FUN_801041d4 @ 0x801041d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801041d4(void)

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
        FUN_80104b24();
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
      FUN_80104b24();
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
  FUN_80104b24();
  return;
}



/* ======= FUN_80104264 @ 0x80104264 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104264(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104b24();
  return;
}



/* ======= FUN_8010433c @ 0x8010433c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010433c(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80104c14();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      FUN_80104c14();
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
  FUN_80104c14();
  return;
}



/* ======= FUN_8010442c @ 0x8010442c ======= */
// decompile failed: Exception while decompiling 8010442c: process: timeout


/* ======= FUN_8010486c @ 0x8010486c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010486c(void)

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
      goto LAB_80105200;
    }
    if (bVar2 != 0) goto LAB_80105200;
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
LAB_80105200:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_8011dca0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109488(0,0);
    return;
  }
  FUN_80105af0(0,1);
  return;
}



/* ======= FUN_80104a18 @ 0x80104a18 ======= */
// decompile failed: Exception while decompiling 80104a18: process: timeout


/* ======= FUN_80104b24 @ 0x80104b24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b24(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_80109488(0,0);
  iVar3 = (uint)(byte)(&LAB_8011d8d4)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104c14 @ 0x80104c14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104c14(void)

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
  iVar4 = (uint)(byte)(&LAB_8011d8d4)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + -0x202c);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104de0 @ 0x80104de0 ======= */
// decompile failed: Exception while decompiling 80104de0: process: timeout


/* ======= FUN_80104f08 @ 0x80104f08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f08(void)

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



/* ======= FUN_801050a8 @ 0x801050a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801050a8(void)

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
         *(undefined2 *)(&LAB_8011dca0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_8011dca0_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&LAB_8011dca0 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80105af0(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80105378 @ 0x80105378 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105378(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee2320))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_80106b24();
  }
  else {
    FUN_8010698c();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105ef4:
    FUN_80109488(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_80109488(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105ef4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106f8c();
  return;
}



/* ======= FUN_80105468 @ 0x80105468 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105468(void)

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
        if (4 < DAT_800aca4e) {
          uVar4 = func_0x8001af20();
          *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011dc40)[uVar4 & 0x1f];
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
          goto LAB_80106118;
        }
        uVar4 = func_0x8001af20();
        *(undefined1 *)(_DAT_800ac784 + 0x1de) =
             *(undefined1 *)((int)&PTR_LAB_8011dc50 + (uVar4 & 0x1f));
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
      goto LAB_80105918;
    }
    if (bVar2 != 0) goto LAB_80106118;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
    (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
LAB_80106118:
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
LAB_80105918:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80105934 @ 0x80105934 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105934(void)

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
        FUN_80106358(0);
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
      FUN_80109488(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_80106358(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_8011dfe4)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011dfec);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
    FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
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
                      (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011e2bc);
    }
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    func_0x80022da0();
    if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
      func_0x80019d50(0x2000);
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
      FUN_801093c8(0,1);
      return;
    }
    FUN_80109488(0,0);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80105af0 @ 0x80105af0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105af0(int param_1)

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
  FUN_80109488(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_80105b7c @ 0x80105b7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105b7c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105cf8:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105ef4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
        FUN_80109488(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_801066f4;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_801066f4;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105cf8;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_801066f4;
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
      goto LAB_80105ef4;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105ef4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfd4);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    puVar5 = (undefined1 *)0x0;
LAB_801066f4:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106f8c();
    return;
  }
LAB_80105ef4:
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80105f10 @ 0x80105f10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105f10(void)

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
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_8010618c @ 0x8010618c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010618c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80106b10();
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
      FUN_80106b10();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011dfec);
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
  FUN_80106b10();
  return;
}



/* ======= FUN_80106324 @ 0x80106324 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106324(undefined4 param_1,undefined4 param_2,int param_3)

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
      goto LAB_80106c80;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106c80;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011dfec);
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
LAB_80106c80:
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
    FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
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
    FUN_80109488(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
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
    FUN_80109488(0,0);
    return;
  }
  FUN_801093c8(0,1);
  return;
}



/* ======= FUN_80106358 @ 0x80106358 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106358(int param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  char cVar2;
  uint uVar3;
  int in_v1;
  int iVar4;
  undefined4 uVar5;
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
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
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
    FUN_80109488(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80106490 @ 0x80106490 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106490(void)

{
  short sVar1;
  char cVar2;
  uint uVar3;
  uint extraout_v1;
  int iVar4;
  undefined4 uVar5;
  int unaff_s2;
  uint *unaff_s3;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee2004))();
    return;
  }
  FUN_80107d9c();
  *unaff_s3 = extraout_v1 | 0x4a;
  func_0x800453d0();
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
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
    FUN_80109488(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80106504 @ 0x80106504 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106504(void)

{
  byte bVar1;
  char cVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80106f8c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011e2bc);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109488(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80106f8c();
  return;
}



/* ======= FUN_801067c8 @ 0x801067c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801067c8(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_801072ec();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_80109488(0,1);
      FUN_80107d88();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    puVar5 = (uint *)((uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011e2bc);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011e2bc);
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
      FUN_80107258(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_801072ec();
  return;
}



/* ======= FUN_8010698c @ 0x8010698c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010698c(undefined4 param_1)

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
    FUN_80107258(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_801072ec();
  return;
}



/* ======= FUN_80106b10 @ 0x80106b10 ======= */

void FUN_80106b10(void)

{
  return;
}



/* ======= FUN_80106b24 @ 0x80106b24 ======= */

void FUN_80106b24(void)

{
  return;
}



/* ======= FUN_80106b30 @ 0x80106b30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106b30(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
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
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    puVar7 = (uint *)((uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
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
    FUN_80109488(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x80022da0();
  if ((_DAT_00000083 & 1) != 0) {
    func_0x80019d50(0x2000);
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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80106f20 @ 0x80106f20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106f20(void)

{
  if (*(byte *)(_DAT_800ac784 + 7) < 0xc) {
                    /* WARNING: Jumptable at 0x80106f5c did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801000fc + (uint)*(byte *)(_DAT_800ac784 + 7) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80106f8c @ 0x80106f8c ======= */
// decompile failed: Exception while decompiling 80106f8c: process: timeout


/* ======= FUN_80107258 @ 0x80107258 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107258(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109488(0,1);
    FUN_80107d88();
    return;
  }
  FUN_80109488(0,0);
  FUN_80107d88();
  return;
}



/* ======= FUN_801072ec @ 0x801072ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801072ec(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80109488(0,1);
  FUN_80107d88();
  return;
}



/* ======= FUN_8010759c @ 0x8010759c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010759c(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
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
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    iVar2 = _DAT_800ac784;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80107fb8;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011e2bc)
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
        func_0x80019d50();
        return;
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
      func_0x80065b70(0x80,&stack0x00000028);
      iVar2 = unaff_s2 + 0x40;
      goto LAB_80107fb8;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
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
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
LAB_80107fb8:
  func_0x80022da0(iVar2);
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    return;
  }
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
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109488(0,0);
    return;
  }
  FUN_801093c8(0,1);
  return;
}



/* ======= FUN_801077cc @ 0x801077cc ======= */
// decompile failed: Exception while decompiling 801077cc: process: timeout


/* ======= FUN_80107d88 @ 0x80107d88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107d88(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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
  undefined4 in_stack_00000040;
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011e2bc);
      if (*(char *)(unaff_s1 + 0x529) != '\0') {
        *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
      }
    }
    if (*(short *)(unaff_s1 + 0x536) == 0) {
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
      func_0x80019d50();
      return;
    }
    *(short *)(unaff_s1 + 0x536) = *(short *)(unaff_s1 + 0x536) + -1;
    in_stack_00000040 = _DAT_80072d4c;
    in_stack_00000044 = _DAT_80072d50;
    in_stack_00000048 = _DAT_80072d54;
    in_stack_0000004c = _DAT_80072d58;
    in_stack_00000050 = _DAT_80072d5c;
    in_stack_00000054 = _DAT_80072d60;
    in_stack_00000058 = _DAT_80072d64;
    in_stack_0000005c = _DAT_80072d68;
    func_0x80065b70(0x80,&stack0x00000040);
    func_0x80022da0(unaff_s1 + 0x4f4,&stack0x00000040,unaff_s1 + 0x4f4);
    if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
      func_0x80019d50(0x2000);
      return;
    }
  }
  else {
    *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
    if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011e2bc);
    }
  }
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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80107d9c @ 0x80107d9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107d9c(void)

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
  undefined4 in_stack_00000040;
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011e2bc);
      if (*(char *)(unaff_s1 + 0x529) != '\0') {
        *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
      }
    }
    if (*(short *)(unaff_s1 + 0x536) == 0) {
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
      func_0x80019d50();
      return;
    }
    *(short *)(unaff_s1 + 0x536) = *(short *)(unaff_s1 + 0x536) + -1;
    in_stack_00000040 = _DAT_80072d4c;
    in_stack_00000044 = _DAT_80072d50;
    in_stack_00000048 = _DAT_80072d54;
    in_stack_0000004c = _DAT_80072d58;
    in_stack_00000050 = _DAT_80072d5c;
    in_stack_00000054 = _DAT_80072d60;
    in_stack_00000058 = _DAT_80072d64;
    in_stack_0000005c = _DAT_80072d68;
    func_0x80065b70(0x80,&stack0x00000040);
    func_0x80022da0(unaff_s1 + 0x4f4,&stack0x00000040,unaff_s1 + 0x4f4);
    if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
      func_0x80019d50(0x2000);
      return;
    }
  }
  else {
    *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
    if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_8011e2bc);
    }
  }
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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_801083a8 @ 0x801083a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801083a8(void)

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
      goto LAB_80108ddc;
    }
    unaff_s0 = 1;
    if (bVar1 != 0) goto LAB_80108ddc;
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
LAB_80108ddc:
  func_0x80022da0(iVar4,unaff_s0 + 0x18,iVar4);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (uStackX_c - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_801085f0 @ 0x801085f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801085f0(uint *param_1)

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



/* ======= FUN_80108660 @ 0x80108660 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108660(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 == 1) {
    cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
    FUN_80109250();
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      FUN_80109250();
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011d8f4)
                                         [(ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8] * 0xac +
                             _DAT_800ac784[0x62] + 0x40);
    return;
  }
  if (bVar1 != 2) {
    FUN_80109250();
    return;
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109488(0,0);
  }
  return;
}



/* ======= FUN_80108a88 @ 0x80108a88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108a88(void)

{
  (**(code **)(&LAB_8011e2dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80108ad0 @ 0x80108ad0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108ad0(void)

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
    FUN_801093c8(0,1);
    return;
  }
  FUN_80109488(0,0);
  return;
}



/* ======= FUN_80108be0 @ 0x80108be0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108be0(void)

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



/* ======= FUN_80108c88 @ 0x80108c88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108c88(undefined4 param_1,int param_2)

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



/* ======= FUN_80108d5c @ 0x80108d5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108d5c(undefined4 param_1,int param_2)

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



/* ======= FUN_80108df0 @ 0x80108df0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108df0(undefined4 param_1,int param_2)

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



/* ======= FUN_80108e40 @ 0x80108e40 ======= */
// decompile failed: Exception while decompiling 80108e40: process: timeout


/* ======= FUN_80109250 @ 0x80109250 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109250(int param_1)

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
    FUN_80109e64();
    return;
  }
  return;
}



/* ======= FUN_801093c8 @ 0x801093c8 ======= */

void FUN_801093c8(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_80109488 @ 0x80109488 ======= */

void FUN_80109488(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_8010955c @ 0x8010955c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010955c(void)

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



/* ======= FUN_80109684 @ 0x80109684 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109684(void)

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



/* ======= FUN_80109738 @ 0x80109738 ======= */
// decompile failed: Exception while decompiling 80109738: process: timeout


/* ======= FUN_80109ab8 @ 0x80109ab8 ======= */

void FUN_80109ab8(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109af4 @ 0x80109af4 ======= */

void FUN_80109af4(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109b30 @ 0x80109b30 ======= */

void FUN_80109b30(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ======= FUN_80109b54 @ 0x80109b54 ======= */

void FUN_80109b54(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ======= FUN_80109e64 @ 0x80109e64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109e64(void)

{
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&LAB_8011e308 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
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



/* ======= FUN_80109fe4 @ 0x80109fe4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109fe4(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a9a4();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a9a4();
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011e318
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_8011e318
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8010a9a4();
  return;
}



/* ======= FUN_8010a1b4 @ 0x8010a1b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a1b4(void)

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
    (**(code **)(&LAB_8011e358 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010a418 @ 0x8010a418 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a418(void)

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
  _DAT_800ac784[0x1e] = DAT_8011d8e0;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011d934)[uVar3 & 7];
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
                    (uint)(byte)(&LAB_8011d8f4)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &local_38);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f9)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fa)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fb)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
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



/* ======= FUN_8010a9a4 @ 0x8010a9a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a9a4(void)

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



/* ======= FUN_8010ab60 @ 0x8010ab60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ab60(undefined4 param_1)

{
  uint *puVar1;
  short sVar2;
  uint uVar3;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011d8f4)[(unaff_s0 & 0xff) * 8] * 0xac +
                               _DAT_800ac784[0x62] + 0x40);
      return;
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
      puVar1 = _DAT_800ac784;
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010c174(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
            *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
            return;
          }
          sVar2 = (short)_DAT_800ac784[0x77] + 1;
          *(short *)(_DAT_800ac784 + 0x77) = sVar2;
          *(char *)((int)puVar1 + 5) = (char)sVar2;
          *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
          if ((DAT_800acae7 != '\0') &&
             (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
            *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
            uVar3 = func_0x8001af20(0,1);
            if ((uVar3 & 1) != 0) {
              *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
              FUN_8010d1a4();
              return;
            }
          }
          return;
        }
      }
      return;
    }
  }
  uVar3 = func_0x8001bc08(param_1);
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
  (**(code **)(&LAB_8011e380 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010afc0 @ 0x8010afc0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010afc0(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (**(code **)(&LAB_8011e3b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e3f8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b070 @ 0x8010b070 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b070(void)

{
  (**(code **)(&DAT_8011e43c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (*(code *)(&PTR_LAB_8011e458)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ======= FUN_8010b0ec @ 0x8010b0ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b0ec(void)

{
  (**(code **)(&DAT_8011e474 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e3f8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b168 @ 0x8010b168 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b168(void)

{
  (**(code **)(&LAB_8011e4ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (*(code *)(&PTR_LAB_8011e458)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}



/* ======= FUN_8010b1e4 @ 0x8010b1e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b1e4(void)

{
  (**(code **)(&LAB_8011e4c4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b260 @ 0x8010b260 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b260(void)

{
  (**(code **)(&LAB_8011e4cc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b2dc @ 0x8010b2dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b2dc(void)

{
  (**(code **)(&LAB_8011e4d4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b358 @ 0x8010b358 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b358(void)

{
  (**(code **)(&LAB_8011e4d8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b3d4 @ 0x8010b3d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b3d4(void)

{
  (**(code **)(&LAB_8011e4dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b450 @ 0x8010b450 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b450(void)

{
  (**(code **)(&LAB_8011e4e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011e4e8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b4d4 @ 0x8010b4d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b4d4(void)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  uint *puVar6;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    puVar6 = _DAT_800ac784;
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x16;
        *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
        *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
        *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x474;
        *(ushort *)((int)_DAT_800ac784 + 0x6a) = ((byte)_DAT_800ac784[0x24] & 0xf0) << 4;
      }
      else if (bVar1 != 3) goto LAB_8010bf24;
      if (*(char *)((int)_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0x2c) = 0x474;
        *(short *)((int)_DAT_800ac784 + 0x1ba) = *(short *)((int)_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar4 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
      if (iVar4 == 0) {
        return;
      }
      *(char *)((int)_DAT_800ac784 + 0x82) = *(char *)((int)_DAT_800ac784 + 0x82) + '\x01';
      _DAT_800ac784[0xe] = _DAT_800ac784[0xe] - 0x708;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x2c) = 0;
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) goto LAB_8010bf24;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  sVar2 = func_0x8001aa68(((byte)_DAT_800ac784[0x24] & 0xf0) << 4,0x10);
  puVar6 = _DAT_800ac784;
  *(short *)((int)_DAT_800ac784 + 0x6a) = sVar2 + *(short *)((int)_DAT_800ac784 + 0x6a);
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_8010bf24:
  uVar3 = func_0x80065f60((_DAT_800aca88 - puVar6[0xd]) * (_DAT_800aca88 - puVar6[0xd]) +
                          (_DAT_800aca90 - puVar6[0xf]) * (_DAT_800aca90 - puVar6[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar3;
  (**(code **)(&LAB_8011eaac + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011eaec + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  puVar6 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar2 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar2;
    *(char *)((int)puVar6 + 5) = (char)sVar2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') &&
       (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
      uVar5 = func_0x8001af20();
      if ((uVar5 & 1) != 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        FUN_8010d1a4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_8010b73c @ 0x8010b73c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b73c(void)

{
  int iVar1;
  short sVar2;
  uint uVar3;
  
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
  iVar1 = _DAT_800ac784;
  if (*(char *)(_DAT_800ac784 + 0x8f) == '\0') {
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) == 0) {
        FUN_8010c174(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      if (*(short *)(_DAT_800ac784 + 0x1da) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
        return;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x1dc) + 1;
      *(short *)(_DAT_800ac784 + 0x1dc) = sVar2;
      *(char *)(iVar1 + 5) = (char)sVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      if ((DAT_800acae7 != '\0') && (*(undefined1 *)(_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32))
      {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        uVar3 = func_0x8001af20(0,1);
        if ((uVar3 & 1) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
          FUN_8010d1a4();
          return;
        }
      }
      return;
    }
  }
  return;
}



/* ======= FUN_8010b86c @ 0x8010b86c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b86c(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1b14))();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 5) != '\x02') {
    FUN_8010698c();
    FUN_8010c0f0();
    return;
  }
  FUN_80106b24();
  FUN_8010c0f0();
  return;
}



/* ======= FUN_8010b900 @ 0x8010b900 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b900(void)

{
  int iVar1;
  undefined1 extraout_v1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    iVar1 = FUN_80107d9c();
    *(undefined1 *)(iVar1 + 5) = extraout_v1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1874))();
  return;
}



/* ======= FUN_8010b974 @ 0x8010b974 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b974(undefined4 param_1,int param_2)

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



/* ======= FUN_8010bad8 @ 0x8010bad8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bad8(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_8011ea8c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010bc1c @ 0x8010bc1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc1c(void)

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
  _DAT_800ac784[0x1e] = DAT_8011ea88;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,1000,500,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee2e38);
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



/* ======= FUN_8010beac @ 0x8010beac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010beac(void)

{
  uint *puVar1;
  undefined2 uVar2;
  short sVar3;
  uint uVar4;
  
  uVar2 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x78) = uVar2;
  if ((ushort)((ushort)_DAT_800ac784[0x78] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x78] & 1;
  }
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
  (**(code **)(&LAB_8011eaac + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011eaec + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  puVar1 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar3 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar3;
    *(char *)((int)puVar1 + 5) = (char)sVar3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') &&
       (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
      uVar4 = func_0x8001af20();
      if ((uVar4 & 1) != 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        FUN_8010d1a4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_8010c0f0 @ 0x8010c0f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c0f0(int param_1)

{
  ushort uVar1;
  
  uVar1 = _DAT_800aca58;
  if (_DAT_800aca58 == 0x701) {
    *(undefined1 *)(param_1 + 5) = 1;
  }
  else {
    if ((5999 < *(short *)(param_1 + 0x1d4)) || (_DAT_800aca58 != 0x201)) {
      if ((_DAT_800aca50 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
    *(undefined1 *)(param_1 + 5) = 1;
  }
  *(ushort *)(uVar1 + 0x1e8) = *(byte *)(uVar1 + 0x90) & 1;
  *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ======= FUN_8010c174 @ 0x8010c174 ======= */

void FUN_8010c174(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  return;
}



/* ======= FUN_8010c180 @ 0x8010c180 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c180(void)

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



/* ======= FUN_8010c27c @ 0x8010c27c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c27c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  short unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      iVar3 = 2;
      if (bVar1 == 2) {
        if (*(char *)(_DAT_800ac784 + 7) == '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 2;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        iVar3 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      }
      goto LAB_8010cb94;
    }
    iVar3 = 1;
    if (bVar1 != 0) goto LAB_8010cb94;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar3;
LAB_8010cb94:
  if (0x3c < iVar3) {
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
      FUN_8010d504();
      return;
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
    *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  }
  FUN_80110aac((int)unaff_s1);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar4 + (uVar4 / 7 + (uVar4 - uVar4 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010c3a4 @ 0x8010c3a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c3a4(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800acae7 != '\0') || (iVar1 = func_0x8001a804(0x9c4,0xc0,&DAT_800aca88), -1 < iVar1)) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
       ((_DAT_800aca58 == 0x701 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010cd24();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) &&
       ((_DAT_800aca58 == 0x201 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010cd24();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1e6) == 0) &&
       (((0x50 < _DAT_800acaee && (7000 < *(short *)(_DAT_800ac784 + 0x1d4))) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)))) {
      uVar2 = func_0x8001af20();
      uVar3 = func_0x8001af20();
      if ((uVar2 & 1) * (uVar3 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        FUN_8010cd24();
        return;
      }
    }
    if ((_DAT_800aca50 & 1) == 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x90) & 3) != 0) &&
         ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a))
           + 0x200 & 0xfff) < 0x400)) {
        *(undefined2 *)(_DAT_800ac784 + 0x1ea) =
             *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10);
        *(ushort *)(_DAT_800ac784 + 0x1e8) = *(byte *)(_DAT_800ac784 + 0x90) & 1;
        *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
        *(undefined2 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  FUN_8010cdd0();
  return;
}



/* ======= FUN_8010c5e4 @ 0x8010c5e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c5e4(void)

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
      return;
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
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x1e) {
    FUN_8010d020(0,1);
    return;
  }
  FUN_80110350(0,0);
  func_0x800245d8(8);
  return;
}



/* ======= FUN_8010c848 @ 0x8010c848 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c848(void)

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
        FUN_8010d1a4();
        return;
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



/* ======= FUN_8010c9b4 @ 0x8010c9b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c9b4(void)

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
    func_0x8001a9cc(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    FUN_8010d35c();
    return;
  }
  sVar4 = func_0x8001a9cc(&DAT_800aca88,0x10);
  if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
    sVar4 = func_0x8001a9cc(&DAT_800aca88,0x40);
  }
  *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
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
      FUN_8010d504();
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



/* ======= FUN_8010cd24 @ 0x8010cd24 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cd24(void)

{
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 1:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  default:
    return;
  }
}



/* ======= FUN_8010cdd0 @ 0x8010cdd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cdd0(void)

{
  int in_v1;
  
  *(undefined1 *)(in_v1 + 6) = 3;
  *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  func_0x800453d0(2);
  _DAT_800aca50 = _DAT_800aca50 | 1;
  FUN_8010d720();
  return;
}



/* ======= FUN_8010cf94 @ 0x8010cf94 ======= */
// decompile failed: Exception while decompiling 8010cf94: process: timeout


/* ======= FUN_8010d020 @ 0x8010d020 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d020(void)

{
  undefined1 in_v0;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 0x8f) = in_v0;
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 1;
  FUN_8010d8fc();
  return;
}



/* ======= FUN_8010d170 @ 0x8010d170 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d170(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8010da84();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_8010da84();
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
  FUN_8010da84();
  return;
}



/* ======= FUN_8010d1a4 @ 0x8010d1a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d1a4(int param_1)

{
  int in_v1;
  
  if (in_v1 != 2) {
    FUN_8010da84();
    return;
  }
  *(undefined1 *)(param_1 + 5) = 3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ======= FUN_8010d29c @ 0x8010d29c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d29c(void)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1b')) {
    func_0x800453d0(6);
  }
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar3 != 1) {
    iVar4 = _DAT_800ac784;
    if (1 < uVar3) {
      uVar5 = CONCAT44(uVar3,3);
      if (uVar3 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 3;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        return;
      }
      goto code_r0x8010dc28;
    }
    uVar5 = CONCAT44(uVar3,1);
    if (uVar3 != 0) goto code_r0x8010dc28;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar2 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0xd) {
    FUN_8010dbf8(0,1);
    return;
  }
  FUN_80110350(0,0);
  iVar4 = 0x800;
  uVar5 = func_0x800245d8(0x800);
code_r0x8010dc28:
  _DAT_800acbcc = (undefined4)uVar5;
  _DAT_800acbd0 = *(undefined4 *)((int)((ulonglong)uVar5 >> 0x20) + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(iVar4 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}



/* ======= FUN_8010d35c @ 0x8010d35c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d35c(void)

{
  char cVar1;
  ushort in_v0;
  int iVar2;
  undefined8 uVar3;
  
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (in_v0 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0xd) {
    FUN_8010dbf8(0,1);
    return;
  }
  FUN_80110350(0,0);
  iVar2 = 0x800;
  uVar3 = func_0x800245d8(0x800);
  _DAT_800acbcc = (undefined4)uVar3;
  _DAT_800acbd0 = *(undefined4 *)((int)((ulonglong)uVar3 >> 0x20) + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(iVar2 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}



/* ======= FUN_8010d43c @ 0x8010d43c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d43c(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 8) {
                    /* WARNING: Jumptable at 0x8010d478 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801001d4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}



/* ======= FUN_8010d504 @ 0x8010d504 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d504(void)

{
  char in_v0;
  char cVar1;
  int iVar2;
  uint uVar3;
  
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + in_v0;
  if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
    func_0x800245d8(0);
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xdc;
    if ((DAT_800acae7 == 0) && (iVar2 = func_0x8001a804(2000,0x180,&DAT_800aca88), iVar2 < 0)) {
      _DAT_800acaee = _DAT_800acaee - 10;
      if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
        func_0x800453d0(3);
        *(undefined1 *)(_DAT_800ac784 + 6) = 5;
        DAT_800aca58 = 2;
        cVar1 = func_0x8001a780(&DAT_800aca54);
        DAT_800aca59 = cVar1 + 2;
        DAT_800aca5a = 0;
        DAT_800acae7 = DAT_800acae7 | 1;
        uVar3 = func_0x8001af20();
        func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee14d8 + (uVar3 & 1)),0x32);
        FUN_8010e2d0();
        return;
      }
      cVar1 = func_0x8001a780(&DAT_800aca54);
      *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
      FUN_8010e2d0();
      return;
    }
  }
  return;
}



/* ======= FUN_8010d720 @ 0x8010d720 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d720(void)

{
  char cVar1;
  short in_v0;
  int in_v1;
  byte *unaff_s0;
  
  _DAT_800acaee = in_v0 - 10;
  if ((*(short *)(*(int *)(in_v1 + -0x387c) + 0x1e4) == 0) &&
     (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
    func_0x800453d0(3);
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    DAT_800aca58 = 2;
    cVar1 = func_0x8001a780(unaff_s0 + -0x93);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    *unaff_s0 = *unaff_s0 | 1;
    FUN_8010e078();
    return;
  }
  cVar1 = func_0x8001a780(unaff_s0 + -0x93);
  *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
  FUN_8010e2d0();
  return;
}



/* ======= FUN_8010d8fc @ 0x8010d8fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d8fc(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(*(int *)(in_v0 + -0x387c) + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  *(short *)(_DAT_800ac784 + 0x9c) =
       *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x11) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
    func_0x800245d8(0x800);
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xe) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
    if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      FUN_8010e2d0();
      return;
    }
  }
  return;
}



/* ======= FUN_8010da84 @ 0x8010da84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010da84(void)

{
  uint uVar1;
  
  uVar1 = func_0x8001af20();
  if ((uVar1 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  return;
}



/* ======= FUN_8010daec @ 0x8010daec ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010daec(void)

{
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 1:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  default:
    return;
  }
}



/* ======= FUN_8010dbf8 @ 0x8010dbf8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dbf8(int param_1)

{
  char cVar1;
  int in_v0;
  
  DAT_800aca59 = 0;
  DAT_800aca5a = 0;
  _DAT_800aca54 = *(uint *)(in_v0 + -0x35ac) | 0x1000;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(param_1 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010e728();
  return;
}



/* ======= FUN_8010df40 @ 0x8010df40 ======= */
// decompile failed: Exception while decompiling 8010df40: process: timeout


/* ======= FUN_8010e078 @ 0x8010e078 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e078(int param_1)

{
  undefined4 in_v0;
  int iVar1;
  int iVar2;
  int in_v1;
  
  _DAT_800acbd0 = *(undefined4 *)(in_v1 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  _DAT_800acbcc = in_v0;
  func_0x8001a8f8(param_1 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar1;
  iVar2 = func_0x8001f314(iVar2,*(undefined4 *)(iVar1 + 0x16c));
  if (iVar2 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_8010e2d0 @ 0x8010e2d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e2d0(void)

{
  char cVar1;
  int iVar2;
  
  cVar1 = func_0x8001f314();
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(char *)(_DAT_800ac784 + 6) == '\x06') {
    func_0x800453d0(7);
    iVar2 = func_0x8001f314();
    if (iVar2 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  return;
}



/* ======= FUN_8010e394 @ 0x8010e394 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e394(void)

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



/* ======= FUN_8010e4c4 @ 0x8010e4c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e4c4(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      cVar2 = '\x02';
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        return;
      }
      goto LAB_8010eda4;
    }
    cVar2 = '\x01';
    if (bVar1 != 0) goto LAB_8010eda4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
LAB_8010eda4:
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ======= FUN_8010e5bc @ 0x8010e5bc ======= */
// decompile failed: Exception while decompiling 8010e5bc: process: timeout


/* ======= FUN_8010e728 @ 0x8010e728 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e728(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int in_v1;
  
  *(short *)(*(int *)(in_v1 + -0x387c) + 0x1ba) =
       *(short *)(*(int *)(in_v1 + -0x387c) + 0x1ba) + -0x708;
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),param_3,0x200);
  if (iVar1 != 0) {
    *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xc1c;
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    func_0x800245d8(0);
    *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
    *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
    *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_ffffc788 + 0x93) = 0;
    return;
  }
  return;
}



/* ======= FUN_8010e89c @ 0x8010e89c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e89c(void)

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
        FUN_8010f3b0();
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
      FUN_8010f3b0();
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
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  FUN_8010f3b0();
  return;
}



/* ======= FUN_8010ebd0 @ 0x8010ebd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ebd0(void)

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
    FUN_8010f47c();
    return;
  }
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  (**(code **)(&LAB_8011eb30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010ecc0 @ 0x8010ecc0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ecc0(void)

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
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&LAB_8011ea6c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ======= FUN_8010ee7c @ 0x8010ee7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee7c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8010f890();
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
      FUN_8010f890();
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
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ea6c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -2;
  func_0x800245d8((1 - (*(byte *)(_DAT_800ac784 + 0x93) & 0x80)) * 0x800);
  if (*(char *)(_DAT_800ac784 + 7) != '\x02') {
    return;
  }
  func_0x800453d0(7);
  FUN_8010f890();
  return;
}



/* ======= FUN_8010f0a0 @ 0x8010f0a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f0a0(void)

{
  short *psVar1;
  int iVar2;
  int iVar3;
  
  if ((int)*(short *)(_DAT_800ac784 + 0x1ba) <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    (**(code **)(&LAB_8011eb88 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  }
  iVar2 = _DAT_800ac784;
  iVar3 = *(int *)(_DAT_800ac784 + 0x38);
  *(int *)(_DAT_800ac784 + 0x38) =
       (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xc) * 0x10000 >> 0x10) + iVar3;
  if (iVar3 == 0) {
    *(undefined1 *)(iVar2 + 6) = 1;
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
  else if (iVar3 != 1) {
    FUN_80110234();
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



/* ======= FUN_8010f190 @ 0x8010f190 ======= */
// decompile failed: Exception while decompiling 8010f190: process: timeout


/* ======= FUN_8010f3b0 @ 0x8010f3b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f3b0(int param_1,uint param_2,uint param_3)

{
  char cVar1;
  undefined4 in_v0;
  uint in_v1;
  
  *(undefined4 *)(param_1 + 0xc4) = in_v0;
  *(uint *)(param_1 + 0xec) = in_v1 & param_2 | param_3;
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
  }
  return;
}



/* ======= FUN_8010f400 @ 0x8010f400 ======= */
// decompile failed: Exception while decompiling 8010f400: process: timeout


/* ======= FUN_8010f47c @ 0x8010f47c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f47c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  FUN_8010fe20();
  return;
}



/* ======= FUN_8010f630 @ 0x8010f630 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f630(void)

{
  (**(code **)(&LAB_8011ebe0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010f678 @ 0x8010f678 ======= */
// decompile failed: Exception while decompiling 8010f678: process: timeout


/* ======= FUN_8010f890 @ 0x8010f890 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f890(undefined4 param_1,undefined4 param_2)

{
  int in_v0;
  
  func_0x8001f314(param_1,param_2,*(undefined1 *)(in_v0 + 7),0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    FUN_80110128();
    return;
  }
  return;
}



/* ======= FUN_8010f938 @ 0x8010f938 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f938(void)

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
    FUN_80110234();
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



/* ======= FUN_8010fb50 @ 0x8010fb50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fb50(undefined4 param_1,int param_2)

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



/* ======= FUN_8010fe20 @ 0x8010fe20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe20(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  
  DAT_800acae9 = 0;
  DAT_800acaea = 0;
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),param_3 + 0x5a0,param_4 + -0x1408);
  func_0x80037edc(0,10);
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    DAT_800aca5a = 6;
    _DAT_800acabe = _DAT_800acabe + 0x800;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    _DAT_fffff077 = *(undefined4 *)(_DAT_800aca54 + 0x800b0000);
    uVar2 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
    func_0x8001af5c(0,0,uVar2 + 100,uVar2 + 200);
    iVar1 = *(int *)(_DAT_800ac784 + 0x188);
    *(int *)(iVar1 + 0xc84) = iVar1 + 0xec;
    *(int *)(iVar1 + 0xca8) = iVar1 + 0xac;
    *(undefined4 *)(iVar1 + 0xc44) = 0x66;
    *(undefined4 *)(iVar1 + 0xc48) = 0xfffffcd6;
    *(undefined4 *)(iVar1 + 0xc4c) = 0;
    *(undefined2 *)(iVar1 + 0xc78) = 0;
    *(undefined2 *)(iVar1 + 0xc7a) = 0;
    *(undefined2 *)(iVar1 + 0xc7c) = 0;
    func_0x80068098(iVar1 + 0xc78,iVar1 + 0xc30);
    return;
  }
  return;
}



/* ======= FUN_80110128 @ 0x80110128 ======= */

void FUN_80110128(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = func_0x8001f314(param_1,*(undefined4 *)(param_2 + -0x3430),param_3,0x200);
  uVar2 = (uint)DAT_800aca5a;
  DAT_800aca5a = (byte)(uVar2 + iVar1);
  iVar1 = *(int *)(uVar2 + iVar1 + -0x387c);
  if (*(short *)(iVar1 + 0x1d6) != 0) {
    *(short *)(iVar1 + 0x1de) = *(short *)(iVar1 + 0x1de) + 1;
    FUN_8011120c();
    return;
  }
  *(undefined2 *)(iVar1 + 0x1de) = 0;
  return;
}



/* ======= FUN_80110234 @ 0x80110234 ======= */

void FUN_80110234(void)

{
  int in_v0;
  int in_v1;
  undefined2 in_a3;
  
  if (in_v0 != 0) {
    *(undefined2 *)(in_v1 + 0x8c) = in_a3;
  }
  return;
}



/* ======= FUN_80110300 @ 0x80110300 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110300(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_LAB_8011eca4)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ======= FUN_80110350 @ 0x80110350 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110350(void)

{
  undefined2 uVar1;
  int in_v0;
  int unaff_s0;
  
  (*(code *)(&PTR_LAB_8011eca4)[in_v0])();
  func_0x8002b498(_DAT_800ac784);
  uVar1 = func_0x8002aec4(unaff_s0 + 0x14,_DAT_800ac784);
  *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
  func_0x8002b544();
  uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4
                         );
  *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
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



/* ======= FUN_80110498 @ 0x80110498 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110498(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee2e38);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = *(uint *)(&LAB_8011ec44 + (uint)(byte)_DAT_800ac784[0x79] * 4);
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



/* ======= FUN_8011081c @ 0x8011081c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011081c(void)

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
  (**(code **)(&LAB_8011ecc4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ed04 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
    FUN_8011120c();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_801109ec @ 0x801109ec ======= */

void FUN_801109ec(void)

{
  int in_v0;
  int in_v1;
  
  if (in_v0 != 0) {
    *(short *)(in_v1 + 0x1de) = *(short *)(in_v1 + 0x1de) + 1;
    FUN_8011120c();
    return;
  }
  *(undefined2 *)(in_v1 + 0x1de) = 0;
  return;
}



/* ======= FUN_80110a4c @ 0x80110a4c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a4c(void)

{
  int iVar1;
  uint uVar2;
  int in_v1;
  
  if ((*(ushort *)(in_v1 + 0x1d0) & 1) != 0) {
    *(undefined1 *)(in_v1 + 5) = 3;
    FUN_801112a4();
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 6000) && (_DAT_800aca58 == 0x701)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    iVar1 = _DAT_800ac784;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    if (iVar1 == 0) {
      uRam00000706 = 6;
      return;
    }
    if (((((uRam000008d1 & 1) != 0) && (cRam000008e3 == '\0')) && (sRam000008d7 == 0)) &&
       ((((6000 < uRam000008d5 && (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)) &&
         ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
        (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  return;
}



/* ======= FUN_80110aac @ 0x80110aac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110aac(void)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  int in_v1;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  iVar1 = _DAT_800ac784;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (iVar1 == 0) {
    *(undefined1 *)(in_v1 + 5) = 6;
    return;
  }
  if (((((*(ushort *)(in_v1 + 0x1d0) & 1) != 0) && (*(char *)(in_v1 + 0x1e2) == '\0')) &&
      (*(short *)(in_v1 + 0x1d6) == 0)) &&
     ((((6000 < *(ushort *)(in_v1 + 0x1d4) &&
        (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)) &&
       ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
      (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)))) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_80110b10 @ 0x80110b10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b10(void)

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



/* ======= FUN_80110d00 @ 0x80110d00 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110d00(void)

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



/* ======= FUN_80110dfc @ 0x80110dfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110dfc(void)

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
LAB_80110ecc:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(3);
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    bVar8 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    uVar10 = FUN_80115b68(0,0);
    in_v1 = (uint)((ulonglong)uVar10 >> 0x20);
    uVar6 = (uint)uVar10;
  }
  else {
    if (uVar6 == 0) {
      uVar6 = 2;
      if (bVar8 != 2) goto LAB_801117c8;
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
          goto LAB_801117c8;
        }
        uVar6 = 3;
        uVar3 = 3;
        if (in_v1 != 0) goto LAB_801117c8;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_801117c8;
    }
    if (bVar8 != 0) goto LAB_801117c8;
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
        goto LAB_80110ecc;
      }
    }
    uVar9 = unaff_s0 - 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    if (unaff_s0 != 0) {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed48)[uVar9]) {
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
LAB_801117c8:
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
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed54)[unaff_s3]) {
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



/* ======= FUN_80110fd8 @ 0x80110fd8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110fd8(void)

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



/* ======= FUN_80111110 @ 0x80111110 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111110(void)

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
      FUN_80115b68(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
        FUN_80111aa0();
        return;
      }
LAB_801112a8:
      func_0x800453d0(8);
      return;
    }
    do {
      if (*(char *)(iRamffffc784 + 0x95) == (&LAB_8011ed4c)[unaff_s3]) {
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
      FUN_80115b68(0,1);
      if ((*(char *)(_DAT_800ac784 + 0x95) != '\x01') && (*(char *)(_DAT_800ac784 + 0x95) != ')')) {
        return;
      }
      goto LAB_801112a8;
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



/* ======= FUN_8011120c @ 0x8011120c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011120c(void)

{
  bool bVar1;
  char cVar2;
  int in_v1;
  ushort unaff_s3;
  short unaff_s4;
  
  if (*(byte *)(in_v1 + 0x95) < 0x18) {
    FUN_80115b68(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
      FUN_80111aa0();
      return;
    }
    func_0x800453d0(8);
    return;
  }
  do {
    if (*(char *)(iRamffffc784 + 0x95) == (&LAB_8011ed4c)[unaff_s3]) {
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



/* ======= FUN_801112a4 @ 0x801112a4 ======= */

void FUN_801112a4(void)

{
  func_0x800453d0(8);
  return;
}



/* ======= FUN_801112c0 @ 0x801112c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801112c0(void)

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



/* ======= FUN_80111408 @ 0x80111408 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111408(void)

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



/* ======= FUN_80111568 @ 0x80111568 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111568(void)

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
      goto LAB_8011201c;
    }
    bVar4 = 1;
    if (uVar7 != 0) goto LAB_8011201c;
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 0x12;
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  _DAT_800ac784[6] = _DAT_800ac784[6] + cVar3;
  FUN_80115b68(0,0);
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
    if (_DAT_800ac784[0x95] == (&DAT_8011ed48)[uVar9]) {
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
LAB_8011201c:
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
    local_28 = DAT_8011ec84;
    local_24 = DAT_8011ec88;
    local_20 = DAT_8011ec8c;
    local_1c = DAT_8011ec90;
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
          if (_DAT_800ac784[0x95] == (&DAT_8011ed54)[uVar10]) {
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



/* ======= FUN_80111844 @ 0x80111844 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111844(void)

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
            goto LAB_801123d0;
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
      goto LAB_801123d0;
    }
    bVar5 = 1;
    if (bVar2 != 0) goto LAB_801123d0;
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
    *(undefined1 *)(extraout_v1 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0();
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    return;
  }
  FUN_80115b68(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
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
    if (*(char *)(_DAT_800ac784 + 0x95) == (&LAB_8011ed4c)[uVar8]) {
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
LAB_801123d0:
  *(byte *)(_DAT_800ac784 + 5) = bVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}



/* ======= FUN_80111aa0 @ 0x80111aa0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111aa0(void)

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



/* ======= FUN_80111bfc @ 0x80111bfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111bfc(void)

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
            FUN_80112754();
            return;
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
        goto LAB_8011276c;
      }
      if (bVar1 != 3) goto LAB_8011276c;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        goto LAB_8011276c;
      }
      goto LAB_80111f64;
    }
    if (bVar1 != 0) goto LAB_8011276c;
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
LAB_8011276c:
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
LAB_80111f64:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80111f84 @ 0x80111f84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f84(void)

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
        if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed54)[uVar6]) {
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



/* ======= FUN_80112460 @ 0x80112460 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112460(void)

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
      local_28 = DAT_8011ec84;
      local_24 = DAT_8011ec88;
      local_20 = DAT_8011ec8c;
      local_1c = DAT_8011ec90;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_80112548 @ 0x80112548 ======= */
// decompile failed: Exception while decompiling 80112548: process: timeout


/* ======= FUN_80112754 @ 0x80112754 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112754(void)

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



/* ======= FUN_80112810 @ 0x80112810 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112810(void)

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
        FUN_801130e8();
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_8011ec84;
      local_24 = DAT_8011ec88;
      local_20 = DAT_8011ec8c;
      local_1c = DAT_8011ec90;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_801128f8 @ 0x801128f8 ======= */
// decompile failed: Exception while decompiling 801128f8: process: timeout


/* ======= FUN_80112b74 @ 0x80112b74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112b74(void)

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
      FUN_80113438();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011ec84;
    local_24 = DAT_8011ec88;
    local_20 = DAT_8011ec8c;
    local_1c = DAT_8011ec90;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_80112c48 @ 0x80112c48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112c48(void)

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



/* ======= FUN_80112e14 @ 0x80112e14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e14(void)

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
      FUN_801136d8();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011ec84;
    local_24 = DAT_8011ec88;
    local_20 = DAT_8011ec8c;
    local_1c = DAT_8011ec90;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_80112ee8 @ 0x80112ee8 ======= */
// decompile failed: Exception while decompiling 80112ee8: process: timeout


/* ======= FUN_801130e8 @ 0x801130e8 ======= */

void FUN_801130e8(undefined4 param_1,undefined4 param_2,int param_3)

{
  char in_v0;
  
  *(char *)(*(int *)(param_3 + -0x387c) + 6) = *(char *)(*(int *)(param_3 + -0x387c) + 6) + in_v0;
  FUN_80115c3c(0,0);
  FUN_8011393c();
  return;
}



/* ======= FUN_8011314c @ 0x8011314c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011314c(void)

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
      FUN_80113c0c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = DAT_8011ec84;
    local_34 = DAT_8011ec88;
    local_30 = DAT_8011ec8c;
    local_2c = DAT_8011ec90;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar4 = func_0x8001a780(&DAT_800aca54), iVar4 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_80113c0c();
    return;
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed59)[bVar5]) {
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
    if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x8011ed58)) {
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



/* ======= FUN_80113438 @ 0x80113438 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113438(void)

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
    FUN_80113da4(0,0);
    return;
  }
  FUN_80115c3c(0,1);
  return;
}



/* ======= FUN_8011343c @ 0x8011343c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011343c(void)

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
    FUN_80113da4(0,0);
    return;
  }
  FUN_80115c3c(0,1);
  return;
}



/* ======= FUN_801135bc @ 0x801135bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801135bc(void)

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
      FUN_80113e80();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011ec84;
    local_24 = DAT_8011ec88;
    local_20 = DAT_8011ec8c;
    local_1c = DAT_8011ec90;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_80113690 @ 0x80113690 ======= */

/* WARNING: Removing unreachable block (ram,0x801139bc) */
/* WARNING: Removing unreachable block (ram,0x801139c0) */
/* WARNING: Removing unreachable block (ram,0x801139cc) */
/* WARNING: Removing unreachable block (ram,0x801139d8) */
/* WARNING: Removing unreachable block (ram,0x801139e4) */
/* WARNING: Removing unreachable block (ram,0x80113a44) */
/* WARNING: Removing unreachable block (ram,0x80113a54) */
/* WARNING: Removing unreachable block (ram,0x80113948) */
/* WARNING: Removing unreachable block (ram,0x80113974) */
/* WARNING: Removing unreachable block (ram,0x801139b8) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113690(void)

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
LAB_80113748:
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
      FUN_80115c3c(0,1);
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011ed5c)) {
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
      goto LAB_80113748;
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
  uStack_28 = *(undefined4 *)(&LAB_8011ec64 + iVar7);
  uStack_24 = *(undefined4 *)(&LAB_8011ec68 + iVar7);
  uStack_20 = *(undefined4 *)(&LAB_8011ec6c + iVar7);
  uStack_1c = *(undefined4 *)(&LAB_8011ec70 + iVar7);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
  func_0x800453d0(2);
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80115b68(0,0);
  }
  else {
    FUN_80115b68(0,1);
  }
  return;
}



/* ======= FUN_801136d8 @ 0x801136d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801136d8(undefined1 *param_1)

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
LAB_80114298:
    param_1[0x93] = bVar2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar6 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    in_stack_00000010 = *(undefined4 *)(&LAB_8011ec64 + iVar6);
    in_stack_00000014 = *(undefined4 *)(&LAB_8011ec68 + iVar6);
    in_stack_00000018 = *(undefined4 *)(&LAB_8011ec6c + iVar6);
    in_stack_0000001c = *(undefined4 *)(&LAB_8011ec70 + iVar6);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
    func_0x800453d0(2);
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
      FUN_80115b68(0,0);
    }
    else {
      FUN_80115b68(0,1);
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
  FUN_80115c3c(0,1);
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011ed5c)) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x8011ed64)) {
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
      goto LAB_80114298;
    }
  }
  return;
}



/* ======= FUN_8011393c @ 0x8011393c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011393c(void)

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



/* ======= FUN_80113ac4 @ 0x80113ac4 ======= */
// decompile failed: Exception while decompiling 80113ac4: process: timeout


/* ======= FUN_80113c0c @ 0x80113c0c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c0c(undefined4 param_1,int param_2)

{
  int in_at;
  undefined4 in_v0;
  undefined1 in_v1;
  
  *(undefined4 *)(in_at + -0x35a8) = in_v0;
  DAT_800acae7 = in_v1;
  _DAT_800acbd0 = param_1;
  *(undefined2 *)(param_2 + 0x6a) = _DAT_800acabe;
  FUN_801145a4(6);
  return;
}



/* ======= FUN_80113da4 @ 0x80113da4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113da4(void)

{
  char cVar1;
  
  func_0x800453d0();
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80114780();
  return;
}



/* ======= FUN_80113e80 @ 0x80113e80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e80(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011ec44 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '1') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 8;
    FUN_80114780();
    return;
  }
  return;
}



/* ======= FUN_80113f90 @ 0x80113f90 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f90(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)(&LAB_8011ec44 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
    func_0x800453d0(1);
  }
  (**(code **)(&LAB_8011ed84 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8011404c @ 0x8011404c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011404c(void)

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
      goto LAB_80114a30;
    }
    if (uVar2 != 0) goto LAB_80114a30;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar1 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar1);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar1);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar1);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
LAB_80114a30:
  if (uVar2 != 0x3d) {
    return;
  }
  func_0x800453d0(1);
  FUN_80115270();
  return;
}



/* ======= FUN_80114240 @ 0x80114240 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114240(void)

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
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar3);
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



/* ======= FUN_801144b8 @ 0x801144b8 ======= */
// decompile failed: Exception while decompiling 801144b8: process: timeout


/* ======= FUN_801145a4 @ 0x801145a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801145a4(undefined4 param_1)

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
  uStack00000010 = *(undefined4 *)(&LAB_8011ec64 + iVar2);
  uStack00000014 = *(undefined4 *)(&LAB_8011ec68 + iVar2);
  uStack00000018 = *(undefined4 *)(&LAB_8011ec6c + iVar2);
  uStack0000001c = *(undefined4 *)(&LAB_8011ec70 + iVar2);
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
    FUN_801158d8();
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
  FUN_801158d8();
  return;
}



/* ======= FUN_80114780 @ 0x80114780 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114780(void)

{
  undefined1 in_v0;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 5) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ======= FUN_801147b4 @ 0x801147b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801147b4(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)(&LAB_8011ec44 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
    func_0x800453d0(1);
  }
  (**(code **)(&LAB_8011eddc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80114870 @ 0x80114870 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114870(void)

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
      FUN_80115270();
      return;
    }
    if (bVar1 != 0) {
      FUN_80115270();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar3);
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
  FUN_80115b68(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) != '=') {
    return;
  }
  func_0x800453d0(1);
  FUN_80115270();
  return;
}



/* ======= FUN_80114a80 @ 0x80114a80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114a80(void)

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
      goto LAB_801154a4;
    }
    if (bVar1 != 0) goto LAB_801154a4;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar4);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar4);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar4);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar4);
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
LAB_801154a4:
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



/* ======= FUN_80114cb4 @ 0x80114cb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114cb4(void)

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
      FUN_80115724();
      return;
    }
    if (bVar1 != 0) {
      FUN_80115724();
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
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar4);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar4);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar4);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      FUN_8011566c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
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
  FUN_80115724();
  return;
}



/* ======= FUN_80114f34 @ 0x80114f34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114f34(void)

{
  (**(code **)(&LAB_8011ee34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80114f7c @ 0x80114f7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114f7c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801158d8();
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
      FUN_801158d8();
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
  FUN_801158d8();
  return;
}



/* ======= FUN_801150e8 @ 0x801150e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801150e8(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011ec48 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
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



/* ======= FUN_80115270 @ 0x80115270 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115270(int param_1)

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



/* ======= FUN_80115368 @ 0x80115368 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115368(undefined4 param_1,int param_2)

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



/* ======= FUN_8011543c @ 0x8011543c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011543c(undefined4 param_1,int param_2)

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



/* ======= FUN_8011566c @ 0x8011566c ======= */
// decompile failed: Exception while decompiling 8011566c: process: timeout


/* ======= FUN_80115724 @ 0x80115724 ======= */
// decompile failed: Exception while decompiling 80115724: process: timeout


/* ======= FUN_801158a4 @ 0x801158a4 ======= */
// decompile failed: Exception while decompiling 801158a4: process: timeout


/* ======= FUN_801158d8 @ 0x801158d8 ======= */
// decompile failed: Exception while decompiling 801158d8: process: timeout


/* ======= FUN_80115a30 @ 0x80115a30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a30(void)

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
    (**(code **)(&LAB_8011ee84 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_80115b68 @ 0x80115b68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115b68(void)

{
  byte bVar1;
  short sVar2;
  int in_v0;
  int iVar3;
  undefined4 uVar4;
  int in_v1;
  short sVar5;
  int in_lo;
  
  iVar3 = (in_v0 - in_v1) * 0x10000 >> 0x10;
  uVar4 = func_0x80065f60(in_lo + iVar3 * iVar3);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar4;
  func_0x80039e7c();
  if (*(char *)(_DAT_800ac784 + 0x1de) != '\0') {
    *(char *)(_DAT_800ac784 + 0x1de) = *(char *)(_DAT_800ac784 + 0x1de) + -1;
    if (*(char *)(_DAT_800ac784 + 0x1de) == '\0') {
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    }
  }
  if (*(char *)(_DAT_800ac784 + 0x1df) != '\0') {
    *(char *)(_DAT_800ac784 + 0x1df) = *(char *)(_DAT_800ac784 + 0x1df) + -1;
  }
  (**(code **)(&LAB_8011ee84 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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
    sVar5 = -(ushort)((bVar1 & 4) != 0);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x736) = sVar5 * sVar2 * -3 + 0x1000;
    func_0x8003a95c(iVar3 + 0x6b8,1);
    *(undefined2 *)(iVar3 + 0x736) = 0xfff;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x7e2) = sVar5 * sVar2 * 3 + 0x1000;
    func_0x8003a95c(iVar3 + 0x764,1);
    *(undefined2 *)(iVar3 + 0x7e2) = 0xfff;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x88e) = sVar5 * sVar2 * -2 + 0x1000;
    func_0x8003a95c(iVar3 + 0x810,1);
    *(undefined2 *)(iVar3 + 0x88e) = 0xfff;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x532) = sVar5 * sVar2 * 3 + 0x1000;
    func_0x8003a95c(iVar3 + 0x4b4,1);
    *(undefined2 *)(iVar3 + 0x532) = 0xfff;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x5de) = sVar5 * sVar2 * -2 + 0x1000;
    func_0x8003a95c(iVar3 + 0x560,1);
    *(undefined2 *)(iVar3 + 0x5de) = 0xfff;
    if (*(short *)(_DAT_800ac784 + 0x9a) < 100) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
    }
  }
  *(undefined4 *)(_DAT_800ac784 + 0x1d8) = *(undefined4 *)(_DAT_800ac784 + 4);
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_80115c3c @ 0x80115c3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115c3c(void)

{
  byte bVar1;
  short sVar2;
  int in_v0;
  short sVar3;
  int iVar4;
  
  (**(code **)(&LAB_8011ee84 + (uint)*(byte *)(in_v0 + 4) * 4))();
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
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = -(ushort)((bVar1 & 4) != 0);
    sVar2 = func_0x8001af20();
    *(short *)(iVar4 + 0x736) = sVar3 * sVar2 * -3 + 0x1000;
    func_0x8003a95c(iVar4 + 0x6b8,1);
    *(undefined2 *)(iVar4 + 0x736) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar4 + 0x7e2) = sVar3 * sVar2 * 3 + 0x1000;
    func_0x8003a95c(iVar4 + 0x764,1);
    *(undefined2 *)(iVar4 + 0x7e2) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar4 + 0x88e) = sVar3 * sVar2 * -2 + 0x1000;
    func_0x8003a95c(iVar4 + 0x810,1);
    *(undefined2 *)(iVar4 + 0x88e) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar4 + 0x532) = sVar3 * sVar2 * 3 + 0x1000;
    func_0x8003a95c(iVar4 + 0x4b4,1);
    *(undefined2 *)(iVar4 + 0x532) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar4 + 0x5de) = sVar3 * sVar2 * -2 + 0x1000;
    func_0x8003a95c(iVar4 + 0x560,1);
    *(undefined2 *)(iVar4 + 0x5de) = 0xfff;
    if (*(short *)(_DAT_800ac784 + 0x9a) < 100) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
    }
  }
  *(undefined4 *)(_DAT_800ac784 + 0x1d8) = *(undefined4 *)(_DAT_800ac784 + 4);
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_80115ee0 @ 0x80115ee0 ======= */
// decompile failed: Exception while decompiling 80115ee0: process: timeout


/* ======= FUN_80116538 @ 0x80116538 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116538(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012a0c(2000);
  func_0x80012974(4000);
  (**(code **)(&LAB_8011eea4 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_801165cc @ 0x801165cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801165cc(void)

{
  (**(code **)(&LAB_8011eeb8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011eef0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80116648 @ 0x80116648 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116648(void)

{
  (**(code **)(&DAT_8011ef28 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011ef2c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801166d8 @ 0x801166d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166d8(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80116f5c();
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  return;
}



/* ======= FUN_8011676c @ 0x8011676c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011676c(void)

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
      if (DAT_800aca58 == '\x05') goto LAB_8011692c;
      if (((sVar4 == 0) && (100 < _DAT_800acae0)) && (*(char *)((int)_DAT_800ac784 + 0x1df) == '\0')
         ) {
        FUN_80117114();
        return;
      }
    }
  }
  else if (_DAT_800ac784[0x74] < 0x9c4) {
    if (DAT_800aca58 == '\x05') goto LAB_8011692c;
    if (sVar3 == 0) {
      _DAT_800ac784[1] = 0x401;
    }
  }
  if (DAT_800aca58 != '\x05') {
    return;
  }
LAB_8011692c:
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



/* ======= FUN_801169d4 @ 0x801169d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801169d4(void)

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
      if (uVar6 != 2) goto LAB_801173cc;
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
      goto LAB_80116bcc;
    }
    bVar1 = uVar6 != 0;
    uVar6 = 1;
    if (bVar1) goto LAB_801173cc;
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
  iVar5 = func_0x8001a9cc(&DAT_800aca88,0x400);
  if ((iVar5 != 0) && (*(char *)(_DAT_800ac784 + 0x8f) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar6 = _DAT_800ac784;
    uVar4 = 0x5a;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
LAB_801173cc:
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
      FUN_8011b5c4(0,0);
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
    FUN_801188c8();
    return;
  }
LAB_80116bcc:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80116bec @ 0x80116bec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116bec(void)

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
      FUN_8011b5c4(0,0);
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
    FUN_801188c8();
    return;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x30
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  FUN_8011b5c4(1,1);
  iVar4 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar4 >> 0x10) - (iVar4 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80116cfc @ 0x80116cfc ======= */
// decompile failed: Exception while decompiling 80116cfc: process: timeout


/* ======= FUN_80116f5c @ 0x80116f5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f5c(void)

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
    FUN_801188c8();
    return;
  }
  FUN_8011b5c4(0,0);
  return;
}



/* ======= FUN_80117114 @ 0x80117114 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117114(undefined4 param_1)

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
    FUN_801188c8();
    return;
  }
  FUN_8011b5c4(0,0);
  return;
}



/* ======= FUN_801171d8 @ 0x801171d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801171d8(void)

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
  
  local_18 = DAT_801003bc;
  local_14 = DAT_801003c0;
  local_10 = DAT_801003c4;
  local_c = DAT_801003c8;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_80117304:
    unaff_s0 = &DAT_800aca88;
    iVar5 = func_0x8001a9cc(&DAT_800aca88,800);
    if (((iVar5 != 0) || (0xdac < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
       (*(byte *)(_DAT_800ac784 + 0x95) < 0xf)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      uVar7 = func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto code_r0x80117d18;
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
      goto LAB_80117ce0;
    }
LAB_801174e0:
    if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011b5c4(0,0);
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
        goto LAB_80117304;
      }
    }
    else if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      uVar4 = extraout_v1;
      goto LAB_801174e0;
    }
LAB_80117ce0:
    *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
    func_0x8003a95c();
    *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
    uVar4 = _DAT_800ac784;
  }
  uVar7 = CONCAT44(uVar4,(uint)*(byte *)(uVar4 + 0x95));
code_r0x80117d18:
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
  FUN_801188c8();
  return;
}



/* ======= FUN_80117530 @ 0x80117530 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117530(void)

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
  
  local_18 = DAT_801003cc;
  local_14 = DAT_801003d0;
  local_10 = DAT_801003d4;
  local_c = DAT_801003d8;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar3 != 1) {
    if (1 < uVar3) {
      sVar2 = 0x101;
      if (uVar3 != 2) goto code_r0x80117f64;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto LAB_80117760;
    }
    sVar2 = 1;
    if (uVar3 != 0) goto code_r0x80117f64;
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
      goto code_r0x80117f64;
    }
    func_0x800453d0(6);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) != '%') {
LAB_80117760:
    FUN_8011b5c4(0,1);
    return;
  }
  sVar2 = func_0x800453d0(4);
  uVar3 = extraout_v1;
code_r0x80117f64:
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
  FUN_801188c8();
  return;
}



/* ======= FUN_80117780 @ 0x80117780 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117780(void)

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
LAB_801178b8:
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
    FUN_801188c8();
    return;
  }
  if (uVar4 == 0) {
    uVar4 = 0x101;
    if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
      *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) & 0xf7;
      FUN_8011b5c4(0,1);
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
    goto LAB_801178b8;
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
  uVar7 = 0x80118950;
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
  FUN_8011b5c4(1,1);
  iVar5 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar5 >> 0x10) - (iVar5 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_801180f8 @ 0x801180f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801180f8(void)

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
      if (bVar1 != 2) {
        FUN_80118a70();
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
      FUN_80118a70();
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
  FUN_80118a70();
  return;
}



/* ======= FUN_80118500 @ 0x80118500 ======= */
// decompile failed: Exception while decompiling 80118500: process: timeout


/* ======= FUN_801188c8 @ 0x801188c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801188c8(void)

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
  FUN_8011b5c4(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80118a70 @ 0x80118a70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118a70(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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
  FUN_8011b5c4(1,1);
  iVar3 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar3 >> 0x10) - (iVar3 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80118b78 @ 0x80118b78 ======= */
// decompile failed: Exception while decompiling 80118b78: process: timeout


/* ======= FUN_80118d24 @ 0x80118d24 ======= */
// decompile failed: Exception while decompiling 80118d24: process: timeout


/* ======= FUN_80119350 @ 0x80119350 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119350(void)

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



/* ======= FUN_801194b4 @ 0x801194b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801194b4(void)

{
  short sVar1;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar1 == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  return;
}



/* ======= FUN_80119508 @ 0x80119508 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119508(void)

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
    FUN_80119e60();
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



/* ======= FUN_80119678 @ 0x80119678 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119678(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011a050();
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
      FUN_8011a050();
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
  FUN_8011a050();
  return;
}



/* ======= FUN_80119860 @ 0x80119860 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119860(void)

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
      FUN_8011a268();
      return;
    }
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee10bc))();
  return;
}



/* ======= FUN_80119a84 @ 0x80119a84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119a84(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011a3e0();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      return;
    }
    if (bVar1 != 0) {
      FUN_8011a3e0();
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
  FUN_8011a3e0();
  return;
}



/* ======= FUN_80119bf0 @ 0x80119bf0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119bf0(void)

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
    FUN_8011a5c0();
    return;
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0e1c))();
  return;
}



/* ======= FUN_80119dd8 @ 0x80119dd8 ======= */
// decompile failed: Exception while decompiling 80119dd8: process: timeout


/* ======= FUN_80119e60 @ 0x80119e60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119e60(void)

{
  int in_v1;
  
  *(byte *)(in_v1 + 0x93) = *(byte *)(in_v1 + 0x93) | 1;
  func_0x800453d0(1);
  FUN_8011a73c();
  return;
}



/* ======= FUN_8011a050 @ 0x8011a050 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a050(uint param_1)

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
      pcVar3 = (code *)0x7;
      if (bVar5 == 2) {
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x8011b5a0;
    }
    if (bVar5 != 0) goto code_r0x8011b5a0;
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
code_r0x8011b5a0:
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



/* ======= FUN_8011a268 @ 0x8011a268 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a268(void)

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



/* ======= FUN_8011a3e0 @ 0x8011a3e0 ======= */

void FUN_8011a3e0(void)

{
  return;
}



/* ======= FUN_8011a5c0 @ 0x8011a5c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5c0(void)

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



/* ======= FUN_8011a73c @ 0x8011a73c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a73c(uint param_1,undefined4 param_2,undefined1 param_3)

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



/* ======= FUN_8011a9a8 @ 0x8011a9a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a9a8(void)

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
      pcVar2 = (code *)0x7;
      if (DAT_800aca5a == 2) {
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x8011b5a0;
    }
    if (DAT_800aca5a != 0) goto code_r0x8011b5a0;
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
code_r0x8011b5a0:
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



/* ======= FUN_8011adc4 @ 0x8011adc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011adc4(int param_1,int param_2)

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



/* ======= FUN_8011af24 @ 0x8011af24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011af24(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee0b3c))();
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



/* ======= FUN_8011b060 @ 0x8011b060 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b060(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011f4c0;
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



/* ======= FUN_8011b208 @ 0x8011b208 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b208(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee0b28))();
  return;
}



/* ======= FUN_8011b2d4 @ 0x8011b2d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b2d4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0b24))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0b04))();
  return;
}



/* ======= FUN_8011b350 @ 0x8011b350 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b350(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0ae4))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0ac4))();
  return;
}



/* ======= FUN_8011b3cc @ 0x8011b3cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b3cc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee0aa4))();
  return;
}



/* ======= FUN_8011b414 @ 0x8011b414 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b414(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee0aa4))();
  return;
}



/* ======= FUN_8011b45c @ 0x8011b45c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b45c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0aa4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011b4f4 @ 0x8011b4f4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b4f4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee0a6c))();
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



/* ======= FUN_8011b5c4 @ 0x8011b5c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b5c4(void)

{
  int unaff_s0;
  
  func_0x8002b544(unaff_s0 + 0x14);
  func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  func_0x80037358();
  func_0x8001b38c();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011b630 @ 0x8011b630 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b630(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011f590;
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



/* ======= FUN_8011b7d8 @ 0x8011b7d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b7d8(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee0a58))();
  return;
}



/* ======= FUN_8011b8a4 @ 0x8011b8a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b8a4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0a54))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0a34))();
  return;
}



/* ======= FUN_8011b920 @ 0x8011b920 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b920(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0a14))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee09f4))();
  return;
}



/* ======= FUN_8011b99c @ 0x8011b99c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b99c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee09d4))();
  return;
}



/* ======= FUN_8011b9e4 @ 0x8011b9e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b9e4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee09d4))();
  return;
}



/* ======= FUN_8011ba2c @ 0x8011ba2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ba2c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee09d4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011bac4 @ 0x8011bac4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bac4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee099c))();
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



/* ======= FUN_8011bbf4 @ 0x8011bbf4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bbf4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011f660;
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



/* ======= FUN_8011bd9c @ 0x8011bd9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bd9c(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee0988))();
  return;
}



/* ======= FUN_8011be68 @ 0x8011be68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011be68(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0984))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0964))();
  return;
}



/* ======= FUN_8011bee4 @ 0x8011bee4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bee4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0944))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0924))();
  return;
}



/* ======= FUN_8011bf60 @ 0x8011bf60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bf60(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee0904))();
  return;
}



/* ======= FUN_8011bfa8 @ 0x8011bfa8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bfa8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee0904))();
  return;
}



/* ======= FUN_8011bff0 @ 0x8011bff0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bff0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0904))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c088 @ 0x8011c088 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c088(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee08cc))();
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



/* ======= FUN_8011c1cc @ 0x8011c1cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c1cc(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011f730;
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



/* ======= FUN_8011c374 @ 0x8011c374 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c374(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x578,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  uVar2 = func_0x8005070c(700,(int)local_10[0],-(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  if (((*(byte *)(_DAT_800ac784 + 0x1c2) & 1) != 0) && (100 < _DAT_800acae0)) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee08b8))();
  return;
}



/* ======= FUN_8011c4a0 @ 0x8011c4a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c4a0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee08b0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0890))();
  return;
}



/* ======= FUN_8011c51c @ 0x8011c51c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c51c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0870))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0850))();
  return;
}



/* ======= FUN_8011c598 @ 0x8011c598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c598(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee0830))();
  return;
}



/* ======= FUN_8011c5e0 @ 0x8011c5e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c5e0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee081c))();
  return;
}



/* ======= FUN_8011c628 @ 0x8011c628 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c628(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee081c))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c6c0 @ 0x8011c6c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c6c0(void)

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
  _DAT_80072c2c = 0x8010c2d8;
  _DAT_80072c50 = &LAB_80110b00;
  _DAT_80072c6c = 0x80116230;
  _DAT_80072c84 = 0x80116230;
  _DAT_80072cac = 0x8011b724;
  _DAT_80072cb4 = 0x8011bcf4;
  _DAT_80072cd0 = 0x8011c2c4;
  _DAT_80072cd8 = 0x8011c888;
  _DAT_800b52c8 = &DAT_8011d0f8;
  _DAT_800ac998 = &LAB_8011d2d8;
  _DAT_80072bf8 = 0x8010a9b4;
  _DAT_800ac798 = 0x8010a378;
  _DAT_800ac79c = 0x8010a378;
  _DAT_800ac7a0 = 0x8010a378;
  _DAT_800ac7b0 = 0x8010a378;
  _DAT_800ac7b8 = 0x8010a378;
  _DAT_800ac7c8 = 0x8010a378;
  _DAT_800ac7cc = 0x8010a378;
  _DAT_800ac7d0 = 0x8010a378;
  _DAT_800ac7d4 = 0x8010a378;
  _DAT_800ac7d8 = 0x80110424;
  _DAT_800ac7fc = 0x80115d30;
  _DAT_800ac7a4 = 0x8010c258;
  _DAT_800ac818 = 0x8011a7c4;
  _DAT_800ac898 = 0x8010a7a4;
  _DAT_800ac89c = 0x8010a7a4;
  _DAT_800ac8a0 = 0x8010a7a4;
  _DAT_800ac8b0 = 0x8010a7a4;
  _DAT_800ac8b8 = 0x8010a7a4;
  _DAT_800ac8c8 = 0x8010a7a4;
  _DAT_800ac8cc = 0x8010a7a4;
  _DAT_800ac8d0 = 0x8010a7a4;
  _DAT_800ac8d4 = 0x8010a7a4;
  _DAT_800ac8d8 = 0x80110790;
  _DAT_800ac8fc = 0x80116064;
  _DAT_800ac8a4 = 0x8010c298;
  _DAT_800ac918 = 0x8011b168;
  func_0x80029afc();
  return;
}



