/* ======= FUN_80100424 @ 0x80100424 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100424(void)

{
  byte bVar1;
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c(0,0,600,700);
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar1 == 4) || (bVar1 == 7)) || (bVar1 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 5) || (bVar1 == 8)) || (bVar1 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if ((bVar1 == 1) || (bVar1 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 4) || (bVar1 == 7)) || (bVar1 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 5) || (bVar1 == 8)) || (bVar1 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    }
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 6) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x20c01;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xb) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xd) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xe) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x2a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  func_0x8001ee68(_DAT_800ac784);
  return;
}



/* ======= FUN_80100808 @ 0x80100808 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100808(void)

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



/* ======= FUN_80100858 @ 0x80100858 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100858(void)

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



/* ======= FUN_8010093c @ 0x8010093c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010093c(void)

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



/* ======= FUN_80100a24 @ 0x80100a24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a24(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_1)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_2)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_3)[iVar2] * 0xac + _DAT_800ac784[0x62] +
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
  (**(code **)(&LAB_8011f80c + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80100e8c @ 0x80100e8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e8c(void)

{
  (**(code **)(&LAB_8011f840 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f890 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100f08 @ 0x80100f08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f08(void)

{
  (**(code **)(&LAB_8011f8e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f920 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100f84 @ 0x80100f84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f84(void)

{
  (**(code **)(&LAB_8011f960 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f890 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101000 @ 0x80101000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101000(void)

{
  (**(code **)(&LAB_8011f9b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f920 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010107c @ 0x8010107c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010107c(void)

{
  (**(code **)(&LAB_8011f9c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f9cc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801010f8 @ 0x801010f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010f8(void)

{
  (**(code **)(&LAB_8011f9d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f9d4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101174 @ 0x80101174 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101174(void)

{
  (**(code **)(&LAB_8011f9d8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f9d4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801011f0 @ 0x801011f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011f0(void)

{
  (**(code **)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f9d4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010126c @ 0x8010126c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010126c(void)

{
  (**(code **)(&LAB_8011f9e0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f9e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801012e8 @ 0x801012e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801012e8(void)

{
  (**(code **)(&LAB_8011f9e8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f9ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101364 @ 0x80101364 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101364(void)

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



/* ======= FUN_8010147c @ 0x8010147c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010147c(void)

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



/* ======= FUN_80101508 @ 0x80101508 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101508(void)

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



/* ======= FUN_801015e4 @ 0x801015e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801015e4(void)

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



/* ======= FUN_801016f0 @ 0x801016f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016f0(void)

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
    FUN_8010203c(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80101858 @ 0x80101858 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101858(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
    FUN_801021e4();
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



/* ======= FUN_801019f8 @ 0x801019f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019f8(void)

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
      *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011faf0)[uVar5 & 0x1f];
      FUN_80102330();
      return;
    }
    uVar5 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x1de) =
         *(undefined1 *)((int)&PTR_LAB_8011fb00 + (uVar5 & 0x1f));
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80102520(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80101d48 @ 0x80101d48 ======= */
// decompile failed: Exception while decompiling 80101d48: process: timeout


/* ======= FUN_8010203c @ 0x8010203c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010203c(short param_1,int param_2)

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
  FUN_80102bbc();
  return;
}



/* ======= FUN_801021e4 @ 0x801021e4 ======= */

void FUN_801021e4(void)

{
  int in_v1;
  int iVar1;
  
  iVar1 = *(int *)(in_v1 + -0x387c);
  if (((((*(byte *)(iVar1 + 0x1c2) & 2) != 0) && (7 < *(byte *)(iVar1 + 0x95))) &&
      (iVar1 = *(int *)(iVar1 + 0x1ac), -1 < *(short *)(iVar1 + 0x9a))) &&
     (((*(byte *)(iVar1 + 9) & 0x80) == 0 && ((*(ushort *)(iVar1 + 0x1d8) & 1) == 0)))) {
    *(undefined4 *)(iVar1 + 4) = 0xb01;
    FUN_80102bbc();
    return;
  }
  return;
}



/* ======= FUN_80102330 @ 0x80102330 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102330(void)

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
    FUN_80102bbc();
    return;
  }
  return;
}



/* ======= FUN_801023d8 @ 0x801023d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801023d8(void)

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



/* ======= FUN_80102520 @ 0x80102520 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102520(void)

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



/* ======= FUN_801025c8 @ 0x801025c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801025c8(void)

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



/* ======= FUN_8010271c @ 0x8010271c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010271c(void)

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
          FUN_80109470(0,0);
        }
      }
      else if ((*(ushort *)(iVar3 + 0x1d8) & 0x40) == 0) {
        FUN_8010383c(0,1);
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



/* ======= FUN_80102814 @ 0x80102814 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102814(void)

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
  FUN_8010939c(0,0);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_801029e4 @ 0x801029e4 ======= */
// decompile failed: Exception while decompiling 801029e4: process: timeout


/* ======= FUN_80102bbc @ 0x80102bbc ======= */

void FUN_80102bbc(void)

{
  return;
}



/* ======= FUN_80102bc8 @ 0x80102bc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102bc8(void)

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
        FUN_801035e8();
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
      FUN_801035e8();
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
  FUN_801035e8();
  return;
}



/* ======= FUN_80102df8 @ 0x80102df8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102df8(void)

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



/* ======= FUN_80102edc @ 0x80102edc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102edc(void)

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
        FUN_80109470(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_8010383c(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_8010305c @ 0x8010305c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010305c(void)

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



/* ======= FUN_80103180 @ 0x80103180 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103180(void)

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



/* ======= FUN_801031fc @ 0x801031fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801031fc(void)

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



/* ======= FUN_80103258 @ 0x80103258 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103258(uint param_1)

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



/* ======= FUN_80103394 @ 0x80103394 ======= */
// decompile failed: Exception while decompiling 80103394: process: timeout


/* ======= FUN_801035e8 @ 0x801035e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801035e8(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x40,&LAB_8011f7d4);
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
  *(undefined1 *)(*(int *)(&DAT_ffffc786 + bVar1) + 0x8f) = 7;
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80104a38();
  return;
}



/* ======= FUN_8010383c @ 0x8010383c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010383c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_8011f784)[*(byte *)(param_3 + 8)] * 0xac +
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
  FUN_80104a38();
  return;
}



/* ======= FUN_80103978 @ 0x80103978 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103978(void)

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
    puVar3 = (uint *)((uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80103d48 @ 0x80103d48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103d48(void)

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



/* ======= FUN_80103e6c @ 0x80103e6c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103e6c(void)

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



/* ======= FUN_80103f60 @ 0x80103f60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103f60(void)

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



/* ======= FUN_80104008 @ 0x80104008 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104008(void)

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



/* ======= FUN_801040e8 @ 0x801040e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801040e8(void)

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
        FUN_80104a38();
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
      FUN_80104a38();
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
  FUN_80104a38();
  return;
}



/* ======= FUN_80104178 @ 0x80104178 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104178(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104a38();
  return;
}



/* ======= FUN_80104250 @ 0x80104250 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104250(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80104b28();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      FUN_80104b28();
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
  FUN_80104b28();
  return;
}



/* ======= FUN_80104340 @ 0x80104340 ======= */
// decompile failed: Exception while decompiling 80104340: process: timeout


/* ======= FUN_80104780 @ 0x80104780 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104780(void)

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
      goto LAB_80105114;
    }
    if (bVar2 != 0) goto LAB_80105114;
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
LAB_80105114:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_8010939c(0,0);
    return;
  }
  FUN_80105a04(0,1);
  return;
}



/* ======= FUN_8010492c @ 0x8010492c ======= */
// decompile failed: Exception while decompiling 8010492c: process: timeout


/* ======= FUN_80104a38 @ 0x80104a38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104a38(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_8010939c(0,0);
  iVar3 = (uint)(byte)(&LAB_8011f784)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104b28 @ 0x80104b28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b28(void)

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
  iVar4 = (uint)(byte)(&LAB_8011f784)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + -0x17c);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104cf4 @ 0x80104cf4 ======= */
// decompile failed: Exception while decompiling 80104cf4: process: timeout


/* ======= FUN_80104e1c @ 0x80104e1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104e1c(void)

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



/* ======= FUN_80104fbc @ 0x80104fbc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104fbc(void)

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
         *(undefined2 *)(&LAB_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_8011fb50_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&LAB_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80105a04(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_8010528c @ 0x8010528c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010528c(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0470))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_80106a38();
  }
  else {
    FUN_801068a0();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105e08:
    FUN_8010939c(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_8010939c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105e08;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106ea0();
  return;
}



/* ======= FUN_8010537c @ 0x8010537c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010537c(void)

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
        if (4 < DAT_800aca4e) {
          uVar4 = func_0x8001af20();
          *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_8011faf0)[uVar4 & 0x1f];
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
          goto LAB_8010602c;
        }
        uVar4 = func_0x8001af20();
        *(undefined1 *)(_DAT_800ac784 + 0x1de) =
             *(undefined1 *)((int)&PTR_LAB_8011fb00 + (uVar4 & 0x1f));
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
      goto LAB_8010582c;
    }
    if (bVar2 != 0) goto LAB_8010602c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
    (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
LAB_8010602c:
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
LAB_8010582c:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80105848 @ 0x80105848 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105848(void)

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
        FUN_8010626c(0);
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
      FUN_8010939c(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_8010626c(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_8011fe94)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011fe9c);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
    FUN_801074b0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
                      (uint)(byte)(&LAB_8011f784)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&DAT_8012016c);
    }
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    func_0x80022da0();
    if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
      func_0x80019d50(0x2000);
      func_0x80019d50(8,3,0x16,(uint)(byte)puVar9[-0x1ffb8206] * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee0817) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee0816) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar9 + -0x7fee0815) * 0xac +
                               *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      if (_DAT_800b0fe0 < 3) {
        FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
      FUN_80109164();
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
      FUN_801092dc(0,1);
      return;
    }
    FUN_8010939c(0,0);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80105a04 @ 0x80105a04 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a04(int param_1)

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
  FUN_8010939c(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_80105a90 @ 0x80105a90 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a90(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105c0c:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105e08;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
        FUN_8010939c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_80106608;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_80106608;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_8011fe30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105c0c;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_80106608;
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
      goto LAB_80105e08;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105e08;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011fe84);
    puVar5 = (undefined1 *)0x0;
LAB_80106608:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106ea0();
    return;
  }
LAB_80105e08:
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80105e24 @ 0x80105e24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105e24(void)

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
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_801060a0 @ 0x801060a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801060a0(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80106a24();
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
      FUN_80106a24();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011fe9c);
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
  FUN_80106a24();
  return;
}



/* ======= FUN_80106238 @ 0x80106238 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106238(undefined4 param_1,undefined4 param_2,int param_3)

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
      goto LAB_80106b94;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106b94;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe9c);
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
LAB_80106b94:
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
    FUN_801074b0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
    FUN_8010939c(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&DAT_8012016c);
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)puVar8[-0x1ffb8206] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee0817) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee0816) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)puVar8 + -0x7fee0815) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (2 < _DAT_800b0fe0) {
      func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
      FUN_80109164();
      return;
    }
    FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
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
    FUN_8010939c(0,0);
    return;
  }
  FUN_801092dc(0,1);
  return;
}



/* ======= FUN_8010626c @ 0x8010626c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010626c(int param_1,undefined4 param_2,int param_3)

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
    FUN_801074b0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
    FUN_8010939c(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&DAT_8012016c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee07d4) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee07d3) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee07d2) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)(iVar4 + -0x7fee07d1) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80109164();
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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_801063a4 @ 0x801063a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801063a4(void)

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
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0154))();
    return;
  }
  FUN_80107cb0();
  *unaff_s3 = extraout_v1 | 0x4a;
  func_0x800453d0();
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_801074b0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
    FUN_8010939c(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&DAT_8012016c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[unaff_s0] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80109164();
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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80106418 @ 0x80106418 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106418(void)

{
  byte bVar1;
  char cVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80106ea0();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012016c);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_8010939c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80106ea0();
  return;
}



/* ======= FUN_801066dc @ 0x801066dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801066dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80107200();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_8010939c(0,1);
      FUN_80107c9c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
    puVar5 = (uint *)((uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&DAT_8012016c);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&DAT_8012016c);
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
      FUN_8010716c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80107200();
  return;
}



/* ======= FUN_801068a0 @ 0x801068a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801068a0(undefined4 param_1)

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
    FUN_8010716c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80107200();
  return;
}



/* ======= FUN_80106a24 @ 0x80106a24 ======= */

void FUN_80106a24(void)

{
  return;
}



/* ======= FUN_80106a38 @ 0x80106a38 ======= */

void FUN_80106a38(void)

{
  return;
}



/* ======= FUN_80106a44 @ 0x80106a44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106a44(void)

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
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
    puVar7 = (uint *)((uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    unaff_s0 = puVar7 + 0x10;
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s0,&DAT_8012016c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s0,&DAT_8012016c);
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
      FUN_801074b0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8012017c);
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
    FUN_8010939c(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_8011f784)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&DAT_8012016c);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x80022da0();
  if ((_DAT_00000083 & 1) != 0) {
    func_0x80019d50(0x2000);
    func_0x80019d50(8,3,0x16,(uint)(byte)unaff_s0[-0x1ffb8216] * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0857) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0856) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0855) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    FUN_80109164();
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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80106e34 @ 0x80106e34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106e34(void)

{
  if (*(byte *)(_DAT_800ac784 + 7) < 0xc) {
                    /* WARNING: Jumptable at 0x80106e70 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801000fc + (uint)*(byte *)(_DAT_800ac784 + 7) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80106ea0 @ 0x80106ea0 ======= */
// decompile failed: Exception while decompiling 80106ea0: process: timeout


/* ======= FUN_8010716c @ 0x8010716c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010716c(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_8010939c(0,1);
    FUN_80107c9c();
    return;
  }
  FUN_8010939c(0,0);
  FUN_80107c9c();
  return;
}



/* ======= FUN_80107200 @ 0x80107200 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107200(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_8010939c(0,1);
  FUN_80107c9c();
  return;
}



/* ======= FUN_801074b0 @ 0x801074b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801074b0(void)

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
LAB_801075cc:
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
                      (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
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
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      if (2 < _DAT_800b0fe0) {
        func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        goto LAB_801075cc;
      }
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c)
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
        goto LAB_8010877c;
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
      FUN_8010939c(0,0);
      return;
    }
    FUN_801092dc(0,1);
    return;
  }
  func_0x80019d50(0x2000);
LAB_8010877c:
  func_0x80019d50();
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0857) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0856) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0855) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  if (2 < _DAT_800b0fe0) {
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109164();
    return;
  }
  FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  return;
}



/* ======= FUN_801076e0 @ 0x801076e0 ======= */
// decompile failed: Exception while decompiling 801076e0: process: timeout


/* ======= FUN_80107c9c @ 0x80107c9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107c9c(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c);
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
      if ((*(ushort *)(unaff_s1 + 0x536) & 1) == 0) goto LAB_80107f7c;
      func_0x80019d50(0x2000);
    }
    func_0x80019d50();
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0857) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0856) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0855) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109164();
    return;
  }
  *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
  if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c);
  }
LAB_80107f7c:
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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80107cb0 @ 0x80107cb0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107cb0(void)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c);
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
      if ((*(ushort *)(unaff_s1 + 0x536) & 1) == 0) goto LAB_80107f7c;
      func_0x80019d50(0x2000);
    }
    func_0x80019d50();
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0857) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0856) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0855) * 0xac +
                             *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109164();
    return;
  }
  *(char *)(unaff_s1 + 0x529) = cVar1 + -1;
  if ((*(ushort *)(unaff_s1 + 0x536) & 1) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c);
  }
LAB_80107f7c:
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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_801082bc @ 0x801082bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801082bc(void)

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
      goto LAB_80108cf0;
    }
    unaff_s0 = 1;
    if (bVar1 != 0) goto LAB_80108cf0;
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
LAB_80108cf0:
  func_0x80022da0(iVar4,unaff_s0 + 0x18,iVar4);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (uStackX_c - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_80108504 @ 0x80108504 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108504(uint *param_1)

{
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),param_1 + 0x10,&DAT_8012016c);
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



/* ======= FUN_80108574 @ 0x80108574 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108574(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80109164();
        return;
      }
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *(undefined2 *)(_DAT_800ac784 + 1) = 7;
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        FUN_8010939c(0,0);
      }
      return;
    }
    if (bVar1 != 0) {
      FUN_80109164();
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_1)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_2)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_3)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109100(0x800b1038,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  FUN_80109164();
  return;
}



/* ======= FUN_8010899c @ 0x8010899c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010899c(void)

{
  (**(code **)(&DAT_8012018c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801089e4 @ 0x801089e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801089e4(void)

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
    FUN_801092dc(0,1);
    return;
  }
  FUN_8010939c(0,0);
  return;
}



/* ======= FUN_80108af4 @ 0x80108af4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108af4(void)

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



/* ======= FUN_80108b9c @ 0x80108b9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b9c(undefined4 param_1,int param_2)

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



/* ======= FUN_80108c70 @ 0x80108c70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108c70(undefined4 param_1,int param_2)

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



/* ======= FUN_80108d04 @ 0x80108d04 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108d04(undefined4 param_1,int param_2)

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



/* ======= FUN_80108d54 @ 0x80108d54 ======= */
// decompile failed: Exception while decompiling 80108d54: process: timeout


/* ======= FUN_80109100 @ 0x80109100 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109100(void)

{
  ushort uVar1;
  
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x100) != 0) {
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar1 & 0xff) + 100;
    FUN_80109d74();
    return;
  }
  return;
}



/* ======= FUN_80109164 @ 0x80109164 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109164(int param_1)

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
    FUN_80109d78();
    return;
  }
  return;
}



/* ======= FUN_801092dc @ 0x801092dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801092dc(void)

{
  uint *puVar1;
  int iVar2;
  int unaff_s0;
  
  func_0x80019d50();
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a8)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7a9)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7aa)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7ab)[unaff_s0] * 0xac +
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



/* ======= FUN_8010939c @ 0x8010939c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010939c(void)

{
  uint *puVar1;
  int iVar2;
  int unaff_s0;
  
  func_0x80019d50();
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7aa)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011f7ab)[unaff_s0] * 0xac +
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



/* ======= FUN_80109470 @ 0x80109470 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109470(void)

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



/* ======= FUN_80109598 @ 0x80109598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109598(void)

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



/* ======= FUN_8010964c @ 0x8010964c ======= */
// decompile failed: Exception while decompiling 8010964c: process: timeout


/* ======= FUN_801099cc @ 0x801099cc ======= */

void FUN_801099cc(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109a08 @ 0x80109a08 ======= */

void FUN_80109a08(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_80109a44 @ 0x80109a44 ======= */

void FUN_80109a44(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ======= FUN_80109a68 @ 0x80109a68 ======= */

void FUN_80109a68(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ======= FUN_80109d74 @ 0x80109d74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d74(void)

{
  int iVar1;
  
  func_0x80019700();
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&DAT_801201b8 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
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



/* ======= FUN_80109d78 @ 0x80109d78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d78(void)

{
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&DAT_801201b8 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
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



/* ======= FUN_80109ef8 @ 0x80109ef8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109ef8(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a8b8();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a8b8();
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
  FUN_8010a8b8();
  return;
}



/* ======= FUN_8010a0c8 @ 0x8010a0c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a0c8(void)

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
    (**(code **)(&DAT_80120208 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010a32c @ 0x8010a32c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a32c(void)

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
  _DAT_800ac784[0x1e] = DAT_8011f790;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011f7e4)[uVar3 & 7];
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
                    (uint)(byte)(&LAB_8011f7a4)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f7a4_1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &local_38);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f7a4_2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011f7a4_3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011f7d4);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7a9)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7aa)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011f7d4);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f7ab)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011f7d4);
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



/* ======= FUN_8010a8b8 @ 0x8010a8b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a8b8(void)

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



/* ======= FUN_8010aa74 @ 0x8010aa74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aa74(void)

{
  uint *puVar1;
  uint uVar2;
  uint unaff_s0;
  int iVar3;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar3 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_1)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_2)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_3)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[iVar3] * 0xac + _DAT_800ac784[0x62] +
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
            FUN_8010c088(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          FUN_8010bf68(0,1);
          return;
        }
      }
      return;
    }
  }
  uVar2 = func_0x8001bc08();
  if ((uVar2 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar2 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
  }
  (**(code **)(&DAT_80120230 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010aed4 @ 0x8010aed4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aed4(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (**(code **)(&DAT_80120264 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801202a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010af84 @ 0x8010af84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010af84(void)

{
  (**(code **)(&DAT_801202ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120308 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b000 @ 0x8010b000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b000(void)

{
  (**(code **)(&DAT_80120324 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801202a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b07c @ 0x8010b07c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b07c(void)

{
  (**(code **)(&DAT_8012035c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120308 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b0f8 @ 0x8010b0f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b0f8(void)

{
  (**(code **)(&DAT_80120374 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120378 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b174 @ 0x8010b174 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b174(void)

{
  (**(code **)(&DAT_8012037c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120380 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b1f0 @ 0x8010b1f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b1f0(void)

{
  (**(code **)(&DAT_80120384 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120380 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b26c @ 0x8010b26c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b26c(void)

{
  (**(code **)(&DAT_80120388 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120380 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b2e8 @ 0x8010b2e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b2e8(void)

{
  (**(code **)(&DAT_8012038c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120390 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b364 @ 0x8010b364 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b364(void)

{
  (**(code **)(&DAT_80120394 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120398 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b3e8 @ 0x8010b3e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b3e8(void)

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
        FUN_8010be38();
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
      FUN_8010be38();
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
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  FUN_8010be38();
  return;
}



/* ======= FUN_8010b650 @ 0x8010b650 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b650(void)

{
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
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x20) == 0) {
        FUN_8010c088(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_8010bf68(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_8010b780 @ 0x8010b780 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b780(undefined1 param_1)

{
  int extraout_v1;
  int extraout_v1_00;
  int iVar1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
      FUN_80106a38();
      iVar1 = extraout_v1_00;
    }
    else {
      FUN_801068a0();
      iVar1 = extraout_v1;
    }
    *(undefined2 *)(iVar1 + 0x9c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = param_1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    FUN_8010c8f8();
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedfc64))();
  return;
}



/* ======= FUN_8010b814 @ 0x8010b814 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b814(void)

{
  int iVar1;
  undefined8 uVar2;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    iVar1 = FUN_80107cb0();
    *(undefined1 *)(*(int *)(iVar1 + -0x387c) + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    (**(code **)((int)uVar2 * 4 + (int)((ulonglong)uVar2 >> 0x20) * 0x20 + 0x800))();
    if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf9c4))();
  return;
}



/* ======= FUN_8010b888 @ 0x8010b888 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b888(undefined4 param_1,int param_2)

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



/* ======= FUN_8010b9ec @ 0x8010b9ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9ec(void)

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
    (**(code **)(&DAT_8012093c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  }
  return;
}



/* ======= FUN_8010bb3c @ 0x8010bb3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb3c(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = DAT_80120934;
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



/* ======= FUN_8010bc88 @ 0x8010bc88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc88(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  (**(code **)(&DAT_8012095c + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010bd10 @ 0x8010bd10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bd10(void)

{
  (**(code **)(&DAT_80120968 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120984 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010bd8c @ 0x8010bd8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bd8c(void)

{
  (**(code **)(&DAT_80120968 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120984 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010be38 @ 0x8010be38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be38(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 2) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_8010be78 @ 0x8010be78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be78(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_8010c6fc();
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ======= FUN_8010bf14 @ 0x8010bf14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf14(void)

{
  short sVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    if (1 < uVar4) {
      iVar3 = 3;
      if (uVar4 == 2) {
        sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
        if (sVar1 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
          *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
          *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        }
        uVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                                *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
        iVar3 = (int)uVar5;
        uVar4 = (int)((ulonglong)uVar5 >> 0x20) * 0x20 + 0x800;
LAB_8010c920:
        (**(code **)(iVar3 * 4 + uVar4))();
        if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
          *(undefined1 *)(_DAT_800ac784 + 4) = 1;
          *(undefined1 *)(_DAT_800ac784 + 5) = 4;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      if (uVar4 != 3) goto LAB_8010c920;
      sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
      if (sVar1 == 0) {
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 5) = (bVar2 & 1) + 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      goto LAB_8010c0f8;
    }
    iVar3 = 1;
    if (uVar4 != 0) goto LAB_8010c920;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 800;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    FUN_8010c8f8();
    return;
  }
LAB_8010c0f8:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010bf68 @ 0x8010bf68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf68(int param_1)

{
  short sVar1;
  undefined1 in_v0;
  
  *(undefined1 *)(param_1 + 6) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 800;
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    FUN_8010c8f8();
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010c088 @ 0x8010c088 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c088(void)

{
  int in_v1;
  undefined8 uVar1;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 0x8f) = 7;
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  (**(code **)((int)uVar1 * 4 + (int)((ulonglong)uVar1 >> 0x20) * 0x20 + 0x800))();
  if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_8010c138 @ 0x8010c138 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c138(void)

{
  uint *puVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  short sVar6;
  int iVar7;
  uint uVar8;
  int in_lo;
  
  puVar1 = _DAT_800ac784;
  uVar8 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  uVar4 = (ushort)(uVar8 < 2);
  if (uVar8 != 1) {
    if (uVar8 >= 2) {
      uVar4 = 3;
      if (uVar8 != 2) {
        if (uVar8 == 3) {
          cVar3 = *(char *)((int)_DAT_800ac784 + 0x9f);
          *(char *)((int)_DAT_800ac784 + 0x9f) = cVar3 + -1;
          if (cVar3 == '\0') {
            *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
            uVar8 = _DAT_800ac784[0x74];
            *(short *)(_DAT_800ac784 + 0x74) = (short)uVar8 + -1;
            if ((short)uVar8 == 0) {
              *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
              *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
              uVar4 = func_0x8001af20();
              *(ushort *)(_DAT_800ac784 + 0x74) = (uVar4 & 0x1f) + 0x1e;
            }
          }
          func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
          func_0x800245d8(0);
          return;
        }
        goto LAB_8010cb1c;
      }
      goto LAB_8010c228;
    }
    if (uVar8 != 0) {
LAB_8010cb1c:
      *(ushort *)(uVar8 + 0x9c) = uVar4;
      *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + -0x14;
      uVar8 = _DAT_800ac784[0x23];
      if (-1 < (short)uVar8) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
      iVar7 = (int)(short)uVar8 - puVar1[0xf];
      uVar5 = func_0x80065f60(in_lo + iVar7 * iVar7);
      *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar5;
      (**(code **)(&DAT_80120f94 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
      (**(code **)(&DAT_80120fd4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
      }
      func_0x80012974(4000);
      if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
        *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
      }
      puVar1 = _DAT_800ac784;
      if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
        return;
      }
      sVar6 = (short)_DAT_800ac784[0x77] + 1;
      *(short *)(_DAT_800ac784 + 0x77) = sVar6;
      *(char *)((int)puVar1 + 5) = (char)sVar6;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      if ((DAT_800acae7 != '\0') &&
         (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
        uVar8 = func_0x8001af20();
        if ((uVar8 & 1) != 0) {
          *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
          FUN_8010e6c4();
          return;
        }
      }
      return;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  }
  bVar2 = func_0x8001af20();
  puVar1 = _DAT_800ac784;
  cVar3 = (bVar2 & 3) + 1;
  *(char *)((int)_DAT_800ac784 + 0x9f) = cVar3;
  *(char *)((int)puVar1 + 0x9e) = cVar3;
  uVar4 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x23) = (uVar4 & 0x7f) + 0x32;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  uVar8 = func_0x8001af20();
  if ((uVar8 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  }
LAB_8010c228:
  cVar3 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_8010cb14(0x800);
  return;
}



/* ======= FUN_8010c334 @ 0x8010c334 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c334(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_8010c47c:
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x9c) + (int)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x28e) = (short)*(char *)(iVar5 + 0x27a) + *(short *)(iVar5 + 0x28e);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    uVar3 = *(ushort *)(iVar5 + 0x1e2);
    sVar4 = *(short *)(iVar5 + 0x1da);
    *(ushort *)(iVar5 + 0x1e2) = (short)*(char *)(iVar5 + 0x1ce) + uVar3;
    *(short *)(iVar5 + 0x1da) = sVar4 + -1;
    if (sVar4 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      iVar5 = *(int *)(uVar3 - 0x387c);
      *(char *)(iVar5 + 7) = *(char *)(iVar5 + 7) + '\x02';
      FUN_8010d5c0();
      return;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_8010ce34;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      bVar1 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 5;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x1e0) = 0x1000;
      *(undefined2 *)(iVar5 + 0x1e2) = 0x1000;
      *(undefined2 *)(iVar5 + 0x1e4) = 0x1000;
      *(uint *)(iVar5 + 0x158) = *(uint *)(iVar5 + 0x158) | 0x400;
      uVar3 = func_0x8001af20();
      sVar4 = (uVar3 & 0xff) + 100;
      *(short *)(iVar5 + 400) = sVar4;
      *(short *)(iVar5 + 0x1da) = sVar4;
      bVar1 = func_0x8001af20();
      *(byte *)(iVar5 + 0x1ce) = (bVar1 & 7) + 5;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x28c) = 0x1000;
      *(undefined2 *)(iVar5 + 0x28e) = 0x1000;
      *(undefined2 *)(iVar5 + 0x290) = 0x1000;
      *(uint *)(iVar5 + 0x204) = *(uint *)(iVar5 + 0x204) | 0x400;
      bVar1 = func_0x8001af20();
      *(byte *)(iVar5 + 0x27a) = bVar1 & 7;
      goto LAB_8010c47c;
    }
    if (bVar1 != 2) {
      if (bVar1 == 3) {
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) & 0xfffffbff;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) & 0xfffffbff;
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        uVar3 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar3 & 0x1f) + 0x1e;
        return;
      }
      goto LAB_8010ce34;
    }
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x28e) = *(short *)(iVar5 + 0x28e) + *(char *)(iVar5 + 0x27a) * -2;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x1e2) = *(short *)(iVar5 + 0x1e2) + *(char *)(iVar5 + 0x1ce) * -2;
    if (*(short *)(iVar5 + 400) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    }
    *(short *)(iVar5 + 400) = *(short *)(iVar5 + 400) + -2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
LAB_8010ce34:
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0xec,&LAB_8011fe84);
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
  *(undefined4 *)(iVar5 + 0x114) = 0x8f;
  *(undefined2 *)(iVar5 + 0x142) = 0xffce;
  *(undefined2 *)(iVar5 + 0x140) = 0;
  *(undefined2 *)(iVar5 + 0x144) = 0;
  *(undefined2 *)(iVar5 + 0x146) = 3;
  *(undefined2 *)(iVar5 + 0x148) = 0;
  *(undefined2 *)(iVar5 + 0x14a) = 0;
  *(uint *)(iVar5 + 0xac) = *(uint *)(iVar5 + 0xac) | 0x4a;
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  iVar5 = _DAT_800ac784;
  sVar4 = *(short *)(_DAT_800ac784 + 0x8c);
  if (-1 < sVar4) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(char *)(sVar4 + 5) = (char)iVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ======= FUN_8010c654 @ 0x8010c654 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c654(void)

{
  char cVar1;
  byte bVar2;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        FUN_8010cfc8();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      if (*(char *)(_DAT_800ac784 + 0x9e) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x1d2) = (bVar2 & 3) + 1;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
      return;
    }
    if (bVar2 != 0) {
      FUN_8010cfc8();
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
  FUN_8010cfc8();
  return;
}



/* ======= FUN_8010c6fc @ 0x8010c6fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c6fc(int param_1)

{
  char cVar1;
  
  *(undefined1 *)(param_1 + 0x9e) = 0;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010cfc8();
  return;
}



/* ======= FUN_8010c7e0 @ 0x8010c7e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7e0(void)

{
  byte bVar1;
  short sVar2;
  undefined1 uVar3;
  ushort uVar4;
  undefined2 uVar5;
  int iVar6;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    iVar6 = _DAT_800ac784;
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        return;
      }
      goto code_r0x8010d0a4;
    }
    if (bVar1 != 0) goto code_r0x8010d0a4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0xff) + 0x1e;
  }
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  iVar6 = 0;
code_r0x8010d0a4:
  uVar5 = func_0x8003b0a4(iVar6 + 0x34,*(undefined2 *)(*(int *)(iVar6 + 0x78) + 6),4);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
  uVar3 = 1;
  if ((*(byte *)(_DAT_800ac784 + 4) < 2) && (uVar3 = 0x10, 1 < *(byte *)(_DAT_800ac784 + 5) - 1)) {
    uVar3 = 0x12;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = uVar3;
  func_0x8001b4e4();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8010c8f8 @ 0x8010c8f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c8f8(void)

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



/* ======= FUN_8010c988 @ 0x8010c988 ======= */
// decompile failed: Exception while decompiling 8010c988: process: timeout


/* ======= FUN_8010cb14 @ 0x8010cb14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cb14(int param_1)

{
  uint *puVar1;
  short in_v0;
  undefined2 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  int in_v1;
  int in_lo;
  
  *(short *)(in_v1 + 0x9c) = in_v0 + (short)param_1;
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + -0x14;
  uVar5 = _DAT_800ac784[0x23];
  if (-1 < (short)uVar5) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
  iVar4 = (int)(short)uVar5 - *(int *)(param_1 + 0x3c);
  uVar2 = func_0x80065f60(in_lo + iVar4 * iVar4);
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
  (**(code **)(&DAT_80120f94 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120fd4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
      uVar5 = func_0x8001af20();
      if ((uVar5 & 1) != 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        FUN_8010e6c4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_8010cc74 @ 0x8010cc74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc74(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf36c))();
  return;
}



/* ======= FUN_8010ccc4 @ 0x8010ccc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ccc4(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8010d5c0();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      FUN_8010d5c0();
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
  FUN_8010d5c0();
  return;
}



/* ======= FUN_8010cdd0 @ 0x8010cdd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cdd0(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar3 != 1) {
    if (1 < uVar3) {
      iVar2 = 7;
      if (uVar3 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8010d754;
    }
    iVar2 = 1;
    if (uVar3 != 0) goto LAB_8010d754;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    iVar2 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar2 + 0xec,&LAB_8011fe84);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined4 *)(iVar2 + 0x114) = 0x8f;
    *(undefined2 *)(iVar2 + 0x142) = 0xffce;
    *(undefined2 *)(iVar2 + 0x140) = 0;
    *(undefined2 *)(iVar2 + 0x144) = 0;
    *(undefined2 *)(iVar2 + 0x146) = 3;
    *(undefined2 *)(iVar2 + 0x148) = 0;
    *(undefined2 *)(iVar2 + 0x14a) = 0;
    *(uint *)(iVar2 + 0xac) = *(uint *)(iVar2 + 0xac) | 0x4a;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  uVar3 = _DAT_800ac784;
  iVar2 = (int)*(short *)(_DAT_800ac784 + 0x8c);
  if (-1 < iVar2) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
LAB_8010d754:
  *(char *)(iVar2 + 5) = (char)uVar3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ======= FUN_8010cfc8 @ 0x8010cfc8 ======= */

void FUN_8010cfc8(void)

{
  int in_v0;
  
  (**(code **)(&DAT_80120f54 + in_v0 * 4))();
  return;
}



/* ======= FUN_8010cff8 @ 0x8010cff8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cff8(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&DAT_80120f74 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010d13c @ 0x8010d13c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d13c(void)

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
  _DAT_800ac784[0x1e] = DAT_80120f70;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,1000,500,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
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



/* ======= FUN_8010d3cc @ 0x8010d3cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d3cc(void)

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
  (**(code **)(&DAT_80120f94 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120fd4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
        FUN_8010e6c4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_8010d5c0 @ 0x8010d5c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d5c0(void)

{
  int in_v1;
  
  if ((*(short *)(in_v1 + 0x1d4) < 4000) && ((*(ushort *)(in_v1 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(in_v1 + 5) = 1;
  }
  else if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) && (_DAT_800aca58 == 0x701)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    in_v1 = 0x701;
  }
  else {
    if ((5999 < *(short *)(_DAT_800ac784 + 0x1d4)) || (_DAT_800aca58 != 0x201)) {
      if ((_DAT_800aca50 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    in_v1 = 0x201;
  }
  *(ushort *)(in_v1 + 0x1e8) = *(byte *)(in_v1 + 0x90) & 1;
  *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ======= FUN_8010d6a0 @ 0x8010d6a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d6a0(void)

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



/* ======= FUN_8010d79c @ 0x8010d79c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d79c(void)

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
      goto LAB_8010e0b4;
    }
    iVar3 = 1;
    if (bVar1 != 0) goto LAB_8010e0b4;
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
LAB_8010e0b4:
  if (0x3c < iVar3) {
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
      FUN_8010ea24();
      return;
    }
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
    *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  }
  FUN_80111fcc((int)unaff_s1);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
  if ((uVar4 + (uVar4 / 7 + (uVar4 - uVar4 / 7 >> 1) >> 2) * -7 & 0xff) == 6) {
    func_0x800453d0(6);
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010d8c4 @ 0x8010d8c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d8c4(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800acae7 != '\0') || (iVar1 = func_0x8001a804(0x9c4,0xc0,&DAT_800aca88), -1 < iVar1)) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
       ((_DAT_800aca58 == 0x701 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010e244();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) &&
       ((_DAT_800aca58 == 0x201 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010e244();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1e6) == 0) &&
       (((0x50 < _DAT_800acaee && (7000 < *(short *)(_DAT_800ac784 + 0x1d4))) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)))) {
      uVar2 = func_0x8001af20();
      uVar3 = func_0x8001af20();
      if ((uVar2 & 1) * (uVar3 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        FUN_8010e244();
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
  FUN_8010e2f0();
  return;
}



/* ======= FUN_8010db04 @ 0x8010db04 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010db04(void)

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
    FUN_8010e540(0,1);
    return;
  }
  FUN_80111870(0,0);
  func_0x800245d8(8);
  return;
}



/* ======= FUN_8010dd68 @ 0x8010dd68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dd68(void)

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
        FUN_8010e6c4();
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



/* ======= FUN_8010ded4 @ 0x8010ded4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ded4(void)

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
    func_0x8001a9cc(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    FUN_8010e87c();
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
      FUN_80111f6c(0x1e,200);
    }
    if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010ea24();
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



/* ======= FUN_8010e244 @ 0x8010e244 ======= */
// decompile failed: Exception while decompiling 8010e244: process: timeout


/* ======= FUN_8010e2f0 @ 0x8010e2f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e2f0(void)

{
  int in_v1;
  
  *(undefined1 *)(in_v1 + 6) = 3;
  *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  func_0x800453d0(2);
  _DAT_800aca50 = _DAT_800aca50 | 1;
  FUN_8010ec40();
  return;
}



/* ======= FUN_8010e4b4 @ 0x8010e4b4 ======= */
// decompile failed: Exception while decompiling 8010e4b4: process: timeout


/* ======= FUN_8010e540 @ 0x8010e540 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e540(void)

{
  undefined1 in_v0;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 0x8f) = in_v0;
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 1;
  FUN_8010ee1c();
  return;
}



/* ======= FUN_8010e690 @ 0x8010e690 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e690(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8010efa4();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_8010efa4();
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
  FUN_8010efa4();
  return;
}



/* ======= FUN_8010e6c4 @ 0x8010e6c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e6c4(int param_1)

{
  int in_v1;
  
  if (in_v1 != 2) {
    FUN_8010efa4();
    return;
  }
  *(undefined1 *)(param_1 + 5) = 3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  return;
}



/* ======= FUN_8010e7bc @ 0x8010e7bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e7bc(void)

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
      goto code_r0x8010f148;
    }
    uVar5 = CONCAT44(uVar3,1);
    if (uVar3 != 0) goto code_r0x8010f148;
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
    FUN_8010f118(0,1);
    return;
  }
  FUN_80111870(0,0);
  iVar4 = 0x800;
  uVar5 = func_0x800245d8(0x800);
code_r0x8010f148:
  _DAT_800acbcc = (undefined4)uVar5;
  _DAT_800acbd0 = *(undefined4 *)((int)((ulonglong)uVar5 >> 0x20) + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(iVar4 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010fc48();
  return;
}



/* ======= FUN_8010e87c @ 0x8010e87c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e87c(void)

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
    FUN_8010f118(0,1);
    return;
  }
  FUN_80111870(0,0);
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
  FUN_8010fc48();
  return;
}



/* ======= FUN_8010e95c @ 0x8010e95c ======= */
// decompile failed: Exception while decompiling 8010e95c: process: timeout


/* ======= FUN_8010ea24 @ 0x8010ea24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea24(void)

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
        func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fedeff0 + (uVar3 & 1)),0x32);
        FUN_8010f7f0();
        return;
      }
      cVar1 = func_0x8001a780(&DAT_800aca54);
      *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
      FUN_8010f7f0();
      return;
    }
  }
  return;
}



/* ======= FUN_8010ec40 @ 0x8010ec40 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec40(void)

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
    FUN_8010f598();
    return;
  }
  cVar1 = func_0x8001a780(unaff_s0 + -0x93);
  *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
  FUN_8010f7f0();
  return;
}



/* ======= FUN_8010ee1c @ 0x8010ee1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee1c(void)

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
      FUN_8010f7f0();
      return;
    }
  }
  return;
}



/* ======= FUN_8010efa4 @ 0x8010efa4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010efa4(void)

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



/* ======= FUN_8010f00c @ 0x8010f00c ======= */
// decompile failed: Exception while decompiling 8010f00c: process: timeout


/* ======= FUN_8010f118 @ 0x8010f118 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f118(int param_1)

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
  FUN_8010fc48();
  return;
}



/* ======= FUN_8010f460 @ 0x8010f460 ======= */
// decompile failed: Exception while decompiling 8010f460: process: timeout


/* ======= FUN_8010f598 @ 0x8010f598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f598(int param_1)

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



/* ======= FUN_8010f7f0 @ 0x8010f7f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f7f0(void)

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



/* ======= FUN_8010f8b4 @ 0x8010f8b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f8b4(void)

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



/* ======= FUN_8010f9e4 @ 0x8010f9e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f9e4(void)

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
      goto LAB_801102c4;
    }
    cVar2 = '\x01';
    if (bVar1 != 0) goto LAB_801102c4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
LAB_801102c4:
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ======= FUN_8010fadc @ 0x8010fadc ======= */
// decompile failed: Exception while decompiling 8010fadc: process: timeout


/* ======= FUN_8010fc48 @ 0x8010fc48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc48(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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



/* ======= FUN_8010fdbc @ 0x8010fdbc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fdbc(void)

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
        FUN_801108d0();
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
      FUN_801108d0();
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
  FUN_801108d0();
  return;
}



/* ======= FUN_801100f0 @ 0x801100f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801100f0(void)

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
    FUN_8011099c();
    return;
  }
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  (**(code **)(&DAT_80121018 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801101e0 @ 0x801101e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801101e0(void)

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



/* ======= FUN_8011039c @ 0x8011039c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011039c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80110db0();
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
      FUN_80110db0();
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
  if (*(char *)(_DAT_800ac784 + 7) != '\x02') {
    return;
  }
  func_0x800453d0(7);
  FUN_80110db0();
  return;
}



/* ======= FUN_801105c0 @ 0x801105c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801105c0(void)

{
  short *psVar1;
  int iVar2;
  int iVar3;
  
  if ((int)*(short *)(_DAT_800ac784 + 0x1ba) <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    (**(code **)(&DAT_80121070 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
    FUN_80111754();
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



/* ======= FUN_801106b0 @ 0x801106b0 ======= */
// decompile failed: Exception while decompiling 801106b0: process: timeout


/* ======= FUN_801108d0 @ 0x801108d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801108d0(int param_1,uint param_2,uint param_3)

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



/* ======= FUN_80110920 @ 0x80110920 ======= */
// decompile failed: Exception while decompiling 80110920: process: timeout


/* ======= FUN_8011099c @ 0x8011099c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011099c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  FUN_80111340();
  return;
}



/* ======= FUN_80110b50 @ 0x80110b50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110b50(void)

{
  (**(code **)(&DAT_801210c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80110b98 @ 0x80110b98 ======= */
// decompile failed: Exception while decompiling 80110b98: process: timeout


/* ======= FUN_80110db0 @ 0x80110db0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110db0(undefined4 param_1,undefined4 param_2)

{
  int in_v0;
  
  func_0x8001f314(param_1,param_2,*(undefined1 *)(in_v0 + 7),0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    FUN_80111648();
    return;
  }
  return;
}



/* ======= FUN_80110e58 @ 0x80110e58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110e58(void)

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
    FUN_80111754();
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



/* ======= FUN_80111070 @ 0x80111070 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111070(undefined4 param_1,int param_2)

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



/* ======= FUN_80111340 @ 0x80111340 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111340(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v0;
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acaea = 0;
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),param_3 + 0x5a0,param_4 + 0x10e0);
  func_0x80037edc(0,10);
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    DAT_800aca5a = 6;
    _DAT_800acabe = _DAT_800acabe + 0x800;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    FUN_80111c9c();
    return;
  }
  return;
}



/* ======= FUN_80111648 @ 0x80111648 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111648(undefined4 param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  
  cVar1 = func_0x8001f314(param_1,*(undefined4 *)(param_2 + -0x3430),param_3,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  if (((_DAT_800aca52 & 1) != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}



/* ======= FUN_80111754 @ 0x80111754 ======= */

void FUN_80111754(void)

{
  int in_v0;
  int in_v1;
  undefined2 in_a3;
  
  if (in_v0 != 0) {
    *(undefined2 *)(in_v1 + 0x8c) = in_a3;
  }
  return;
}



/* ======= FUN_80111820 @ 0x80111820 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111820(void)

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
    (**(code **)(&DAT_8012111c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_80111870 @ 0x80111870 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111870(int param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  if ((*(byte *)(param_1 + 9) & 0x40) != 0) {
    *(undefined1 *)(param_1 + 4) = 3;
    FUN_80115d74(0);
    func_0x8004efb8();
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
    (**(code **)(&DAT_8012111c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_80111a4c @ 0x80111a4c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111a4c(void)

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
  *(undefined4 *)(_DAT_800ac784 + 0x78) = DAT_80121108;
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
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x1db) = 0;
    FUN_801123ec();
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x1db) = 1;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 4;
  }
  return;
}



/* ======= FUN_80111c20 @ 0x80111c20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c20(void)

{
  undefined1 uVar1;
  char cVar2;
  undefined2 uVar3;
  
  uVar1 = func_0x8001bc08();
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d2) = uVar1;
  *(byte *)((int)_DAT_800ac784 + 0x1d2) = *(byte *)((int)_DAT_800ac784 + 0x1d2) & 1;
  uVar3 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x77) = uVar3;
  *(short *)(_DAT_800ac784 + 0x7b) = _DAT_800aca8c - (short)_DAT_800ac784[0xe];
  if ((_DAT_800aca50 & 0xff00) != 0) {
    FUN_80116068();
  }
  (**(code **)(&DAT_8012113c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121184 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((short)_DAT_800ac784[0x7b] < 0xfa1) {
    if (799 < (short)_DAT_800ac784[0x7b]) {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
      FUN_80115f70();
      if ((_DAT_800aca50 & 0x800) != 0) {
        if (*(char *)((int)_DAT_800ac784 + 0x1db) == '\0') {
          _DAT_800aca50 = _DAT_800aca50 & 0xf0ff;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1db) = 1;
        }
      }
      return;
    }
    func_0x80012974(6000);
  }
  else {
    func_0x80012a0c(6000);
  }
  func_0x8001a8f8();
  cVar2 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x79) = ((byte)_DAT_800ac784[0x75] & 0x3f) * (short)cVar2;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}



/* ======= FUN_80111c9c @ 0x80111c9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c9c(void)

{
  char cVar1;
  undefined2 in_v0;
  
  *(undefined2 *)(_DAT_800ac784 + 0x77) = in_v0;
  *(short *)(_DAT_800ac784 + 0x7b) = _DAT_800aca8c - (short)_DAT_800ac784[0xe];
  if ((_DAT_800aca50 & 0xff00) != 0) {
    FUN_80116068();
  }
  (**(code **)(&DAT_8012113c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121184 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((short)_DAT_800ac784[0x7b] < 0xfa1) {
    if (799 < (short)_DAT_800ac784[0x7b]) {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
      FUN_80115f70();
      if ((_DAT_800aca50 & 0x800) != 0) {
        if (*(char *)((int)_DAT_800ac784 + 0x1db) == '\0') {
          _DAT_800aca50 = _DAT_800aca50 & 0xf0ff;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1db) = 1;
        }
      }
      return;
    }
    func_0x80012974(6000);
  }
  else {
    func_0x80012a0c(6000);
  }
  func_0x8001a8f8();
  cVar1 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x79) = ((byte)_DAT_800ac784[0x75] & 0x3f) * (short)cVar1;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}



/* ======= FUN_80111e28 @ 0x80111e28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e28(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 0) {
      FUN_80112684();
      return;
    }
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + -0x28;
    FUN_80115d74(*(byte *)(_DAT_800ac784 + 0x1d4) & 3);
  }
  if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
    FUN_801126d8();
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518))
  {
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



/* ======= FUN_80111f0c @ 0x80111f0c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f0c(void)

{
  int in_v0;
  
  if ((in_v0 != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}



/* ======= FUN_80111f6c @ 0x80111f6c ======= */

void FUN_80111f6c(int param_1)

{
  uint in_hi;
  
  FUN_80115d74(param_1 + (in_hi >> 1) * -3 + 1 & 0xff);
  return;
}



/* ======= FUN_80111fcc @ 0x80111fcc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111fcc(undefined4 param_1,int param_2)

{
  char cVar1;
  uint in_v0;
  
  *(byte *)(param_2 + 0x1d5) =
       *(byte *)(param_2 + 0x1d4) +
       (char)(uint)((ulonglong)*(byte *)(param_2 + 0x1d4) * (ulonglong)(in_v0 | 0x8889) >> 0x25) *
       -0x3c;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)(_DAT_800ac784 + 0x1d3) = *(char *)(_DAT_800ac784 + 0x1d3) + '\x01';
  }
  return;
}



/* ======= FUN_80112054 @ 0x80112054 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112054(int param_1)

{
  undefined1 uVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar2 == 0) {
    FUN_80115d94(1);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar2 != 1) {
    *(undefined1 *)(uVar2 + 0x1d5) = 1;
    if (param_1 == 0) {
      FUN_80115d74(8);
    }
    func_0x800245d8(0);
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  return;
}



/* ======= FUN_801120e4 @ 0x801120e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801120e4(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(2);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80112964();
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  return;
}



/* ======= FUN_80112174 @ 0x80112174 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112174(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80112a18();
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



/* ======= FUN_80112230 @ 0x80112230 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112230(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 == 1) {
LAB_801122b8:
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xffb0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x3c;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_801122f4:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
      func_0x800453d0(1);
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    }
    *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    FUN_80115e24();
    func_0x800245d8(0);
    iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar5 != 0) {
      FUN_80115d74(5);
    }
    return;
  }
  if (uVar4 < 2) {
    if (uVar4 == 0) {
      func_0x800453d0(1);
      *(undefined1 *)(_DAT_800ac784 + 0x1da) = 1;
      FUN_80115d94(6);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      goto LAB_801122b8;
    }
  }
  else if (uVar4 == 2) goto LAB_801122f4;
  if (param_1 == 0) {
    if ((*(byte *)(uVar4 + 0x1d4) & 1) != 0) {
      *(undefined2 *)(uVar4 + 0x8c) = 0xa0;
      _DAT_000000d8 = _DAT_00000284 + _DAT_000000d8;
      func_0x800245d8(0);
      return;
    }
    *(undefined2 *)(uVar4 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    cVar1 = FUN_80115dc8();
    *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (param_1 != 1) goto LAB_801134e8;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 4) != 0) {
      func_0x8001a8f8(&DAT_800aca88,0x1e);
    }
    func_0x800245d8(0);
    uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar2;
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,0x1e);
  uVar4 = FUN_80115e24();
  if ((uVar4 & 1) != 0) {
    FUN_80115d94(3);
    uVar3 = *(ushort *)(_DAT_800ac784 - 0x3512) - 4;
    *(ushort *)(_DAT_800ac784 - 0x3512) = uVar3;
    if ((int)((uint)uVar3 << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    return;
  }
  FUN_80115d94(4);
  uVar4 = _DAT_800ac784;
LAB_801134e8:
  *(undefined2 *)(uVar4 + 0x8c) = 0xb4;
  *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
  if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
    *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
  }
  uVar4 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  iVar5 = *(int *)(uVar4 - 0x387c);
  if ((*(ushort *)(iVar5 + 0x1dc) < 1000) &&
     (*(short *)(iVar5 + 0x1ba) + -0x2ee < *(int *)(iVar5 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}



/* ======= FUN_801123ac @ 0x801123ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123ac(void)

{
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0xe10) {
    FUN_80115d74(9);
  }
  return;
}



/* ======= FUN_801123ec @ 0x801123ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123ec(void)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  uVar2 = (uint)(bVar1 < 2);
  if (bVar1 != 1) {
    if (uVar2 == 0) {
      uVar2 = 2;
      if (bVar1 != 2) goto LAB_80112d24;
      goto LAB_801124a0;
    }
    if (bVar1 != 0) {
LAB_80112d24:
      if (*(ushort *)(*(int *)(uVar2 - 0x387c) + 0x1dc) < 0x9c4) {
        FUN_80115d74(0xe);
      }
      if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
        FUN_80115d74(5);
        *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
      }
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_801124a0:
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



/* ======= FUN_80112534 @ 0x80112534 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112534(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar5 == 0) {
    *(byte *)(_DAT_800ac784 + 0x1d5) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x32;
    FUN_80115d94(6);
    func_0x800453d0(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar5 != 1) {
    if (*(int *)(_DAT_800ac784 + 0x38) <= (int)(uVar5 - 0xe10)) {
      if (*(int *)(_DAT_800ac784 + 0x38) < (int)(uVar5 - 0x1518)) {
        *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
        iVar4 = _DAT_800ac784;
        iVar3 = (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        *(int *)(_DAT_800ac784 + 0x38) = iVar3;
        *(char *)(iVar4 + 0x1d6) = (char)iVar3 + '\x01';
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
        FUN_801161e8();
        if (_DAT_800acaee < 0) {
          DAT_800aca58 = 3;
          _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
          DAT_800aca59 = 0;
          _DAT_800aca5a = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
        }
      }
      else {
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
        *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
      }
      func_0x80019700();
      func_0x800453d0(2);
      *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar4 != 0) {
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
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    FUN_80113694();
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



/* ======= FUN_8011264c @ 0x8011264c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011264c(void)

{
  if (*(short *)(_DAT_800ac784 + 0x1ba) != *(short *)(_DAT_800ac784 + 0x1ea)) {
    FUN_80115d74(5);
  }
  return;
}



/* ======= FUN_80112684 @ 0x80112684 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112684(void)

{
  char cVar1;
  byte bVar2;
  uint *puVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 *extraout_v1;
  int extraout_v1_00;
  ushort *puVar6;
  int iVar7;
  undefined8 uVar8;
  
  uVar5 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (uVar5 != 1) {
    if (1 < uVar5) {
      uVar8 = CONCAT44(uVar5,3);
      if (uVar5 == 2) {
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 3;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        if (*(short *)((int)_DAT_800ac784 + 0x1ba) + -0x2ee < (int)_DAT_800ac784[0xe]) {
          *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
          *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xc;
        }
        FUN_80115e24();
        uVar8 = CONCAT44(extraout_v1,*extraout_v1);
      }
      else if (uVar5 == 3) {
        *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 9;
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -9;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 == '\0') {
          FUN_80115d74(8);
        }
        func_0x800245d8(0);
        return;
      }
      goto code_r0x801130f0;
    }
    uVar8 = CONCAT44(uVar5,0x8c);
    if (uVar5 != 0) goto code_r0x801130f0;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x8c;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x15;
    FUN_80115d94(5);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 3;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 8;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar8 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
code_r0x801130f0:
  *(uint *)((ulonglong)uVar8 >> 0x20) = (uint)uVar8 | 0x40;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  iVar7 = 0;
  puVar6 = (ushort *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar6,_DAT_800ac784[0x5b],0,0x200);
  bVar2 = *(byte *)(extraout_v1_00 + 6);
  *(byte *)(extraout_v1_00 + 6) = bVar2 + 1;
  uVar4 = bVar2 + 2;
  *puVar6 = uVar4;
  if (*(char *)(iVar7 + 0x1db) != '\0') {
    *puVar6 = uVar4 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(iVar7 + 0x6a),iVar7 + 0x20,&DAT_8012110c);
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar5 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar5) {
    return;
  }
  _DAT_800ac784[0xe] = uVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar3 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar3 = *puVar3 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}



/* ======= FUN_80112688 @ 0x80112688 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112688(void)

{
  char cVar1;
  byte bVar2;
  uint *puVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 *extraout_v1;
  int extraout_v1_00;
  ushort *puVar6;
  int iVar7;
  undefined8 uVar8;
  
  uVar5 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (uVar5 != 1) {
    if (1 < uVar5) {
      uVar8 = CONCAT44(uVar5,3);
      if (uVar5 == 2) {
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 3;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        if (*(short *)((int)_DAT_800ac784 + 0x1ba) + -0x2ee < (int)_DAT_800ac784[0xe]) {
          *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
          *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xc;
        }
        FUN_80115e24();
        uVar8 = CONCAT44(extraout_v1,*extraout_v1);
      }
      else if (uVar5 == 3) {
        *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 9;
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -9;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 == '\0') {
          FUN_80115d74(8);
        }
        func_0x800245d8(0);
        return;
      }
      goto code_r0x801130f0;
    }
    uVar8 = CONCAT44(uVar5,0x8c);
    if (uVar5 != 0) goto code_r0x801130f0;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x8c;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x15;
    FUN_80115d94(5);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 3;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 8;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar8 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
code_r0x801130f0:
  *(uint *)((ulonglong)uVar8 >> 0x20) = (uint)uVar8 | 0x40;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  iVar7 = 0;
  puVar6 = (ushort *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar6,_DAT_800ac784[0x5b],0,0x200);
  bVar2 = *(byte *)(extraout_v1_00 + 6);
  *(byte *)(extraout_v1_00 + 6) = bVar2 + 1;
  uVar4 = bVar2 + 2;
  *puVar6 = uVar4;
  if (*(char *)(iVar7 + 0x1db) != '\0') {
    *puVar6 = uVar4 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(iVar7 + 0x6a),iVar7 + 0x20,&DAT_8012110c);
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar5 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar5) {
    return;
  }
  _DAT_800ac784[0xe] = uVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar3 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar3 = *puVar3 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}



/* ======= FUN_801126d8 @ 0x801126d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801126d8(int param_1)

{
  char cVar1;
  byte bVar2;
  uint *puVar3;
  undefined2 in_v0;
  ushort uVar4;
  int extraout_v1;
  uint uVar5;
  ushort *puVar6;
  int iVar7;
  undefined8 uVar8;
  
  *(undefined2 *)(param_1 + 0x8c) = in_v0;
  *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x15;
  FUN_80115d94(5);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 3;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 8;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar8 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(uint *)((ulonglong)uVar8 >> 0x20) = (uint)uVar8 | 0x40;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  iVar7 = 0;
  puVar6 = (ushort *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar6,_DAT_800ac784[0x5b],0,0x200);
  bVar2 = *(byte *)(extraout_v1 + 6);
  *(byte *)(extraout_v1 + 6) = bVar2 + 1;
  uVar4 = bVar2 + 2;
  *puVar6 = uVar4;
  if (*(char *)(iVar7 + 0x1db) != '\0') {
    *puVar6 = uVar4 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(iVar7 + 0x6a),iVar7 + 0x20,&DAT_8012110c);
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar5 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)uVar5 <= (int)_DAT_800ac784[0xe]) {
    _DAT_800ac784[0xe] = uVar5;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    FUN_80115d94(10);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    puVar3 = _DAT_800ac784;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *puVar3 = *puVar3 | 0x40;
    *_DAT_800ac784 = *_DAT_800ac784 | 8;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
    *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
      *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    }
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
    *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    FUN_801157b4();
    return;
  }
  return;
}



/* ======= FUN_801128fc @ 0x801128fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801128fc(void)

{
  if (*(short *)(_DAT_800ac784 + 0x1ba) != *(short *)(_DAT_800ac784 + 0x1ea)) {
    FUN_80115d74(5);
  }
  return;
}



/* ======= FUN_80112938 @ 0x80112938 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112938(void)

{
  char cVar1;
  int iVar2;
  int unaff_s0;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(7);
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) + *(short *)(_DAT_800ac784 + 0x1e6);
    *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + -7;
    if (((*(ushort *)(_DAT_800ac784 + 0x1dc) < 600) &&
        (*(ushort *)(_DAT_800ac784 + 0x1ec) - 1 < 0xe0f)) && (DAT_800acae7 == '\0')) {
      *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 6;
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffec;
      func_0x800453d0(4);
      DAT_800aca58 = 2;
      _DAT_800acaee = _DAT_800acaee + -4;
      cVar1 = func_0x8001a780(unaff_s0 + -0x34);
      DAT_800aca59 = cVar1 + '\x02';
      _DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = '\0';
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      FUN_801161e8();
    }
    if (*(byte *)(_DAT_800ac784 + 0x1d7) != 0) {
      func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d7) << 0xb,
                      (int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x198,
                      &DAT_8012110c);
      *(char *)(_DAT_800ac784 + 0x1d7) = *(char *)(_DAT_800ac784 + 0x1d7) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x800245d8(0);
    return;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 10;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  iVar2 = *(short *)(_DAT_800ac784 + 0x1ba) + -400;
  if (iVar2 <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = iVar2;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    FUN_80115d74(0);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 0;
  }
  return;
}



/* ======= FUN_80112964 @ 0x80112964 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112964(void)

{
  int iVar1;
  
  FUN_80115d94(7);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
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



/* ======= FUN_80112a18 @ 0x80112a18 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a18(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int in_v0;
  
  if (in_v0 != 0) {
    FUN_80115d74(0,param_2,param_3,0x200);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 0;
  }
  return;
}



/* ======= FUN_80112a5c @ 0x80112a5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a5c(void)

{
  undefined1 uVar1;
  char cVar2;
  int extraout_v1;
  undefined4 uVar3;
  int unaff_s0;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0xf) != 0) {
      FUN_801132d0(9);
      return;
    }
    if (((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) != 0) || (*(char *)(_DAT_800ac784 + 5) == '\x06')
       ) {
      uVar1 = FUN_80115f00();
      *(undefined1 *)(extraout_v1 + 0x1d7) = uVar1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffec;
      func_0x800453d0();
      DAT_800aca58 = 2;
      _DAT_800acaee = _DAT_800acaee + -4;
      cVar2 = func_0x8001a780(unaff_s0 + -0x34);
      DAT_800aca59 = cVar2 + '\x02';
      _DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = '\0';
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      FUN_801161e8();
      if (*(byte *)(_DAT_800ac784 + 0x1d7) != 0) {
        func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d7) << 0xb,
                        (int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
        *(char *)(_DAT_800ac784 + 0x1d7) = *(char *)(_DAT_800ac784 + 0x1d7) + -1;
      }
      *(int *)(_DAT_800ac784 + 0x38) =
           (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
      func_0x800245d8(0);
      return;
    }
    FUN_80115d74(6);
    if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
      FUN_80115f00();
    }
  }
  if ((((*(char *)(_DAT_800ac784 + 0x1d2) != '\0') && (*(char *)(_DAT_800ac784 + 0x1db) != '\0')) &&
      (*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000)) && (*(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    uVar3 = 10;
    if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
      uVar3 = 0xc;
    }
    FUN_80115d74(uVar3);
  }
  return;
}



/* ======= FUN_80112b84 @ 0x80112b84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112b84(void)

{
  char cVar1;
  undefined1 uVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
      _DAT_000000d8 = _DAT_00000284 + _DAT_000000d8;
      func_0x800245d8(0);
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    cVar1 = FUN_80115dc8();
    *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') goto LAB_801134e8;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 4) != 0) {
      func_0x8001a8f8(&DAT_800aca88,0x1e);
    }
    func_0x800245d8(0);
    uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar2;
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,0x1e);
  uVar4 = FUN_80115e24();
  if ((uVar4 & 1) != 0) {
    FUN_80115d94(3);
    uVar3 = *(ushort *)(_DAT_800ac784 + -0x3512) - 4;
    *(ushort *)(_DAT_800ac784 + -0x3512) = uVar3;
    if ((int)((uint)uVar3 << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    return;
  }
  FUN_80115d94(4);
LAB_801134e8:
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
  *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
  if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
    *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
  }
  iVar5 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  iVar5 = *(int *)(iVar5 + -0x387c);
  if ((*(ushort *)(iVar5 + 0x1dc) < 1000) &&
     (*(short *)(iVar5 + 0x1ba) + -0x2ee < *(int *)(iVar5 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}



/* ======= FUN_80112cf8 @ 0x80112cf8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112cf8(void)

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



/* ======= FUN_80112d90 @ 0x80112d90 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112d90(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x8001a8f8(&DAT_800aca88,0x32);
        func_0x800245d8(0);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        iVar5 = func_0x8001a804(9000,100,&DAT_800aca88);
        if (iVar5 < 0) {
          FUN_80115d74(0xb);
        }
        return;
      }
      goto LAB_80113758;
    }
    if (bVar1 != 0) goto LAB_80113758;
    FUN_80115d94(6);
    func_0x800453d0(1);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0xe10 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    FUN_80113694();
    return;
  }
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0x1518) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    iVar5 = _DAT_800ac784;
    iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    *(int *)(_DAT_800ac784 + 0x38) = iVar4;
    *(char *)(iVar5 + 0x1d6) = (char)iVar4 + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
    FUN_801161e8();
    if (_DAT_800acaee < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
  }
  else {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
  }
LAB_80113758:
  func_0x80019700();
  func_0x800453d0(2);
  *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar5 != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  }
  if ((uint)*(byte *)(_DAT_800ac784 + 0x1d5) % 10 == 0) {
    func_0x800453d0(0);
  }
  sVar3 = func_0x80037024();
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar3 * -3;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    _DAT_800aca5a = CONCAT11(DAT_800aca5b,DAT_800aca5a + '\x01');
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x4000;
    FUN_80115d74(0xe);
  }
  return;
}



/* ======= FUN_80112f6c @ 0x80112f6c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f6c(void)

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



/* ======= FUN_80112ffc @ 0x80112ffc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112ffc(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  uint uVar4;
  uint extraout_v1;
  uint *puVar5;
  
  uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    puVar5 = _DAT_800ac784;
    if (1 < uVar4) {
      if (uVar4 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(5);
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
        }
        *(ushort *)((int)_DAT_800ac784 + 0x1de) = *(ushort *)((int)_DAT_800ac784 + 0x1de) + 2;
        func_0x8001a8f8(&DAT_800aca88,*(ushort *)((int)_DAT_800ac784 + 0x1de));
        *(ushort *)(_DAT_800ac784 + 0x23) =
             (ushort)_DAT_800ac784[0x23] + *(ushort *)((int)_DAT_800ac784 + 0x1e6);
        *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
        if ((((ushort)_DAT_800ac784[0x77] < 600) && ((ushort)_DAT_800ac784[0x7b] - 1 < 0xe0f)) &&
           (DAT_800acae7 == '\0')) {
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 6;
          *(ushort *)((int)_DAT_800ac784 + 0x1e6) = 0xffec;
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
                          (int)(short)*(ushort *)((int)_DAT_800ac784 + 0x6a),
                          _DAT_800ac784[0x62] + 0x198,&DAT_8012110c);
          *(char *)((int)_DAT_800ac784 + 0x1d7) = *(char *)((int)_DAT_800ac784 + 0x1d7) + -1;
        }
        _DAT_800ac784[0xe] = (int)(short)(ushort)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        func_0x800245d8(0);
        return;
      }
      goto code_r0x80113bd8;
    }
    if (uVar4 != 0) goto code_r0x80113bd8;
    FUN_80115d94(4);
    *(ushort *)(_DAT_800ac784 + 0x23) = 0x50;
    *(ushort *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(ushort *)(_DAT_800ac784 + 0x79) = 200;
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = 5;
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
  func_0x8001a8f8(&DAT_800aca88,*(ushort *)((int)_DAT_800ac784 + 0x1de));
  *(ushort *)(_DAT_800ac784 + 0x23) = (ushort)_DAT_800ac784[0x23] + 0x14;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
  param_3 = 0;
  puVar5 = (uint *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar5,_DAT_800ac784[0x5b],0,0x200);
  uVar4 = extraout_v1;
code_r0x80113bd8:
  bVar1 = *(byte *)(uVar4 + 6);
  *(byte *)(uVar4 + 6) = bVar1 + 1;
  uVar3 = bVar1 + 2;
  *(ushort *)puVar5 = uVar3;
  if (*(char *)(param_3 + 0x1db) != '\0') {
    *(ushort *)puVar5 = uVar3 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(param_3 + 0x6a),param_3 + 0x20,&DAT_8012110c);
  *(ushort *)(_DAT_800ac784 + 0x1b) = (ushort)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)(ushort)_DAT_800ac784[0x1b]) {
    *(ushort *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(ushort *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] + (ushort)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)(ushort)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar4 = (int)(short)*(ushort *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar4) {
    return;
  }
  _DAT_800ac784[0xe] = uVar4;
  *(ushort *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar5 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar5 = *puVar5 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(ushort *)(_DAT_800ac784 + 0x23) = (ushort)_DAT_800ac784[0x23] + 0x14;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}



/* ======= FUN_801132d0 @ 0x801132d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801132d0(void)

{
  char cVar1;
  int in_v1;
  int unaff_s0;
  
  *(undefined1 *)(in_v1 + 0x1d7) = 6;
  *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffec;
  func_0x800453d0(4);
  DAT_800aca58 = 2;
  _DAT_800acaee = _DAT_800acaee + -4;
  cVar1 = func_0x8001a780(unaff_s0 + -0x34);
  DAT_800aca59 = cVar1 + '\x02';
  _DAT_800aca5a = 0;
  if (_DAT_800acaee < 0) {
    DAT_800aca58 = 3;
    _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
    DAT_800aca59 = '\0';
    _DAT_800aca5a = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
  }
  FUN_801161e8();
  if (*(byte *)(_DAT_800ac784 + 0x1d7) != 0) {
    func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d7) << 0xb,
                    (int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x198,
                    &DAT_8012110c);
    *(char *)(_DAT_800ac784 + 0x1d7) = *(char *)(_DAT_800ac784 + 0x1d7) + -1;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8011340c @ 0x8011340c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011340c(void)

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



/* ======= FUN_8011347c @ 0x8011347c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011347c(void)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 0) {
      FUN_80115d94(3);
      uVar2 = *(ushort *)(_DAT_800ac784 + -0x3512) - 4;
      *(ushort *)(_DAT_800ac784 + -0x3512) = uVar2;
      if ((int)((uint)uVar2 << 0x10) < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = 0;
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      return;
    }
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    iVar3 = _DAT_800ac784;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    iVar3 = _DAT_800ac784;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
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
      return;
    }
  }
  iVar3 = *(int *)(iVar3 + -0x387c);
  if ((*(ushort *)(iVar3 + 0x1dc) < 1000) &&
     (*(short *)(iVar3 + 0x1ba) + -0x2ee < *(int *)(iVar3 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}



/* ======= FUN_80113694 @ 0x80113694 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113694(void)

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
    _DAT_0000006d = 0;
    FUN_80115d94(10);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    iVar3 = _DAT_800ac784;
    sVar2 = *(byte *)(_DAT_800ac784 + 7) + 1;
    *(char *)(_DAT_800ac784 + 7) = (char)sVar2;
    *(short *)(iVar3 + 0x1e4) = -sVar2;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    FUN_801152cc();
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



/* ======= FUN_80113900 @ 0x80113900 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113900(void)

{
  if (12000 < *(ushort *)(_DAT_800ac784 + 0x1dc)) {
    FUN_80115d74(9);
  }
  return;
}



/* ======= FUN_8011393c @ 0x8011393c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011393c(undefined4 param_1,uint param_2,int param_3,undefined2 param_4)

{
  uint *puVar1;
  char cVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  undefined2 in_t0;
  uint in_t1;
  
  puVar4 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (puVar4 != (uint *)0x1) {
    puVar5 = _DAT_800ac784;
    if ((uint *)0x1 < puVar4) {
      if (puVar4 != (uint *)0x2) goto LAB_8011430c;
      cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(9);
      }
      goto LAB_80113aa0;
    }
    if (puVar4 != (uint *)0x0) goto LAB_8011430c;
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0xb4;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x20;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x3c;
    if ((_DAT_800ac784[0x75] & 0x80) != 0) {
      *(short *)((int)_DAT_800ac784 + 0x1de) = -*(short *)((int)_DAT_800ac784 + 0x1de);
    }
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)((int)_DAT_800ac784 + 0x6a) =
       *(short *)((int)_DAT_800ac784 + 0x6a) + *(short *)((int)_DAT_800ac784 + 0x1de);
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    puVar4 = _DAT_800ac784;
    uVar3 = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    puVar5 = (uint *)0x0;
    while( true ) {
      *puVar4 = uVar3 | 0x4a;
      *(char *)((int)_DAT_800ac784 + 0x1d5) = (char)param_3;
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      param_2 = param_2 + 1;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      puVar1 = _DAT_800ac784;
      puVar4 = _DAT_800ac784;
      if (0xc < (param_2 & 0xffff)) break;
      puVar4 = (uint *)((param_2 & 0xffff) * 0xac + _DAT_800ac784[0x62]);
      uVar3 = *puVar4;
      puVar4[0x1a] = in_t1;
      *(undefined2 *)(puVar4 + 0x25) = 0;
      *(undefined2 *)((int)puVar4 + 0x96) = in_t0;
      *(undefined2 *)(puVar4 + 0x26) = 0;
      *(undefined2 *)((int)puVar4 + 0x9a) = param_4;
      *(undefined2 *)(puVar4 + 0x27) = 0;
      *(undefined2 *)((int)puVar4 + 0x9e) = 0;
      puVar5 = puVar1;
    }
LAB_8011430c:
    *(char *)((int)puVar4 + 7) = *(char *)((int)puVar4 + 7) + '\x01';
    func_0x80019700(puVar5,param_2,param_3 + 0x198);
    return;
  }
LAB_80113aa0:
  cVar2 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x79) = ((byte)_DAT_800ac784[0x75] & 0x3f) * (short)cVar2;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}



/* ======= FUN_80113b1c @ 0x80113b1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113b1c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  uint *puVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  ushort *puVar6;
  
  puVar6 = (ushort *)(uint)*(byte *)((int)_DAT_800ac784 + 6);
  sVar3 = 1;
  if (puVar6 == (ushort *)0x0) {
    puVar6 = (ushort *)&DAT_00000004;
    FUN_80115d94();
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0xb4;
    *(char *)((int)_DAT_800ac784 + 0x1d6) = *(char *)((int)_DAT_800ac784 + 0x1d6) + '\x01';
    *(ushort *)(_DAT_800ac784 + 0x79) = (byte)_DAT_800ac784[0x75] & 0x3f;
    if ((short)_DAT_800ac784[0x7b] < 2000) {
      *(short *)(_DAT_800ac784 + 0x79) = -(short)_DAT_800ac784[0x79];
    }
    sVar3 = *(byte *)((int)_DAT_800ac784 + 6) + 1;
    *(char *)((int)_DAT_800ac784 + 6) = (char)sVar3;
  }
  else {
    param_3 = 0;
    if (puVar6 == (ushort *)0x1) {
      iVar4 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      if (iVar4 != 0) {
        *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + -1;
      }
      _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
      func_0x8001a8f8(&DAT_800aca88,0x32);
      if ((char)_DAT_800ac784[0x74] != '\0') {
        FUN_80115d74(0x10);
      }
      func_0x800245d8(0);
      return;
    }
  }
  *puVar6 = sVar3 + 1U;
  if (*(char *)(param_3 + 0x1db) != '\0') {
    *puVar6 = sVar3 + 1U & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(param_3 + 0x6a),param_3 + 0x20,&DAT_8012110c);
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar5 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar5) {
    return;
  }
  _DAT_800ac784[0xe] = uVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar2 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar2 = *puVar2 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}



/* ======= FUN_80113c84 @ 0x80113c84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c84(void)

{
  char cVar1;
  uint *puVar2;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    FUN_801161e8();
    func_0x80019700(0x1000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x198,
                    &DAT_8012110c);
    _DAT_800acaee = _DAT_800acaee - 4;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x76) = 1;
    }
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)((int)_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    puVar2 = _DAT_800ac784;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *puVar2 = *puVar2 | 0x40;
    *_DAT_800ac784 = *_DAT_800ac784 | 8;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
    *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
      *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    }
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
    *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    FUN_801157b4();
    return;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}



/* ======= FUN_80113d94 @ 0x80113d94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113d94(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int unaff_s0;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 == 0) {
    FUN_80115d94(3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar4 != 1) {
    *(undefined2 *)(*(int *)(uVar4 - 0x387c) + 0x1ba) = 1;
    iVar2 = _DAT_800aca88 - *(int *)(_DAT_800ac784 + 0x34);
    iVar3 = _DAT_800aca90 - *(int *)(_DAT_800ac784 + 0x3c);
    uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = uVar1;
    *(short *)(_DAT_800ac784 + 0x1ec) = _DAT_800aca8c - *(short *)(_DAT_800ac784 + 0x38);
    iVar2 = func_0x8004efe4(unaff_s0 + 0x48a4,0x1d);
    if ((iVar2 != 0) && (*(char *)(_DAT_800ac784 + 5) != '\x03')) {
      FUN_80115d74(3);
    }
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedede0))();
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



/* ======= FUN_80113ed0 @ 0x80113ed0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113ed0(void)

{
  func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedee34))();
  return;
}



/* ======= FUN_80113f38 @ 0x80113f38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f38(void)

{
  char cVar1;
  ushort uVar2;
  uint *puVar3;
  uint uVar4;
  
  puVar3 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 7);
  uVar2 = (ushort)(puVar3 < (uint *)0x2);
  if (puVar3 != (uint *)0x1) {
    if (puVar3 >= (uint *)0x2) {
      uVar2 = 2;
      if (puVar3 == (uint *)0x2) {
        func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 != '\0') {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 1) = 7;
        FUN_80115d74(0);
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
        return;
      }
      goto code_r0x801149b8;
    }
    if (puVar3 != (uint *)0x0) goto code_r0x801149b8;
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
  uVar4 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar4) {
    return;
  }
  _DAT_800ac784[0xe] = uVar4;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar3 = _DAT_800ac784;
  uVar2 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
  *(char *)((int)_DAT_800ac784 + 7) = (char)uVar2;
code_r0x801149b8:
  *(ushort *)(puVar3 + 0x79) = -uVar2;
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  FUN_801152cc();
  return;
}



/* ======= FUN_801141c4 @ 0x801141c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801141c4(undefined4 param_1,uint param_2,int param_3)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  ushort uVar5;
  
  if (*(char *)((int)_DAT_800ac784 + 7) == '\0') {
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
    func_0x800453d0(3);
    param_2 = 0;
    param_3 = 0x32;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    uVar3 = 0;
    do {
      puVar4 = _DAT_800ac784;
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
      param_2 = param_2 + 1;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      uVar3 = param_2 & 0xffff;
    } while ((param_2 & 0xffff) < 0xd);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  else {
    puVar4 = _DAT_800ac784;
    if (*(char *)((int)_DAT_800ac784 + 7) == '\x01') {
      cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
      if (cVar1 == '\0') {
        uVar5 = 0;
        do {
          uVar3 = (uint)uVar5;
          uVar5 = uVar5 + 1;
          *(undefined4 *)(uVar3 * 0xac + _DAT_800ac784[0x62]) = 0;
        } while (uVar5 < 0xd);
        *(undefined1 *)(_DAT_800ac784 + 1) = 7;
        FUN_80115d74(1);
      }
      return;
    }
  }
  func_0x80019700(puVar4,param_2,param_3 + 0x198);
  return;
}



/* ======= FUN_801143a4 @ 0x801143a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801143a4(void)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar2 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 7);
  if (puVar2 != (uint *)0x1) {
    if ((uint *)0x1 < puVar2) {
      if (puVar2 == (uint *)0x2) {
        func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 != '\0') {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 1) = 7;
        FUN_80115d74(0);
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
        return;
      }
      goto LAB_80114e3c;
    }
    if (puVar2 != (uint *)0x0) goto LAB_80114e3c;
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
  if ((int)_DAT_800ac784[0xe] < (int)uVar3) {
    return;
  }
  _DAT_800ac784[0xe] = uVar3;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar2 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
LAB_80114e3c:
  *puVar2 = *puVar2 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}



/* ======= FUN_80114654 @ 0x80114654 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114654(void)

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
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedede0))();
  return;
}



/* ======= FUN_801147b8 @ 0x801147b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801147b8(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801150f8();
        return;
      }
      goto LAB_8011486c;
    }
    if (bVar1 != 0) {
      FUN_801150f8();
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_8011486c:
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
  if (2000 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(1);
  }
  return;
}



/* ======= FUN_80114930 @ 0x80114930 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114930(void)

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
    FUN_801152cc();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_801152cc();
    return;
  }
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
  return;
}



/* ======= FUN_80114ae0 @ 0x80114ae0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114ae0(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_8011539c();
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



/* ======= FUN_80114bac @ 0x80114bac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114bac(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011552c();
        return;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(1);
      }
      goto LAB_80114cec;
    }
    if (bVar1 != 0) {
      FUN_8011552c();
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
    FUN_801154ec();
    return;
  }
LAB_80114cec:
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ======= FUN_80114d3c @ 0x80114d3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114d3c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801157b4();
        return;
      }
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
      _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) {
      FUN_801157b4();
      return;
    }
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
  FUN_801157b4();
  return;
}



/* ======= FUN_80114fe8 @ 0x80114fe8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114fe8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fededcc))();
  return;
}



/* ======= FUN_801150f8 @ 0x801150f8 ======= */

void FUN_801150f8(void)

{
  int in_v1;
  
  *(char *)(in_v1 + 6) = *(char *)(in_v1 + 6) + '\x01';
  return;
}



/* ======= FUN_801152cc @ 0x801152cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801152cc(void)

{
  int iVar1;
  char cVar2;
  
  DAT_800acae8 = 2;
  DAT_800acae9 = 0;
  cVar2 = DAT_800aca5a + '\x01';
  _DAT_800aca58 = CONCAT12(cVar2,_DAT_800aca58);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    _DAT_800aca58 = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    _DAT_800aca3c = _DAT_800aca3c & 0xffffff3f;
  }
  return;
}



/* ======= FUN_8011539c @ 0x8011539c ======= */

void FUN_8011539c(void)

{
  int in_v0;
  
                    /* WARNING: Could not recover jumptable at 0x801153b0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(&LAB_80100304 + in_v0))();
  return;
}



/* ======= FUN_801154ec @ 0x801154ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801154ec(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = func_0x8001f314(param_1,*(undefined4 *)(param_2 + -0x3430),param_3,0x200);
  if (iVar1 != 0) {
    DAT_800aca58 = 4;
    DAT_800aca59 = 6;
    _DAT_800aca5a = 0;
  }
  return;
}



/* ======= FUN_8011552c @ 0x8011552c ======= */

void FUN_8011552c(void)

{
  (**(code **)((uint)DAT_800aca59 * 4 + -0x7fededbc))();
  return;
}



/* ======= FUN_80115700 @ 0x80115700 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115700(void)

{
  undefined4 uVar1;
  
  if (((*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0x708) &&
      (uVar1 = 7, (int)*(short *)(_DAT_800ac784 + 0x1ba) == (int)*(short *)(_DAT_800ac784 + 0x1ea)))
     || (uVar1 = 5, *(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    FUN_80115d74(uVar1);
  }
  return;
}



/* ======= FUN_801157b4 @ 0x801157b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801157b4(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int in_v0;
  int iVar2;
  uint uVar3;
  undefined2 uVar4;
  int in_v1;
  
  iVar2 = (in_v0 - in_v1 >> 4) + 400;
  uVar4 = (undefined2)iVar2;
  if (iVar2 * 0x10000 >> 0x10 < 0x32) {
    uVar4 = 100;
  }
  *(undefined2 *)(param_3 + 0xbe) = uVar4;
  *(undefined2 *)(param_3 + 0xbc) = uVar4;
  iVar2 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 5) + 0x80;
  sVar1 = (short)iVar2;
  if (iVar2 * 0x10000 >> 0x10 < 0x20) {
    sVar1 = 0x20;
  }
  uVar3 = (uint)sVar1;
  uVar3 = uVar3 << 0x10 | uVar3 << 8 | uVar3;
  *(uint *)(param_3 + 0xc4) = uVar3 | *(uint *)(param_3 + 0xc4) & 0xff000000;
  *(uint *)(param_3 + 0xec) = uVar3 | *(uint *)(param_3 + 0xec) & 0xff000000;
  return;
}



/* ======= FUN_80115868 @ 0x80115868 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115868(void)

{
  bool bVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)_DAT_800aca50;
  uVar3 = uVar5 & 0x4000;
  if (((_DAT_800aca50 & 0x8000) != 0) &&
     (uVar3 = uVar5 & 0x4000, *(byte *)(_DAT_800ac784 + 9) < 0x80)) {
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
      bVar1 = _DAT_800ac784 != 0;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff;
      *(short *)(uVar5 + 0x1d4) = (short)_DAT_800ac784 + -1;
      if ((bVar1) || (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0))) {
        return;
      }
      if (7 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
        FUN_80116bec();
      }
      *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
      uVar2 = _DAT_800aca50;
      goto code_r0x80116148;
    }
    if (4 < *(byte *)(_DAT_800ac784 + 5)) {
      FUN_80115d74(0xf);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    uVar5 = (uint)_DAT_800aca50;
    uVar3 = uVar5 & 0x4000;
  }
  if (uVar3 != 0) {
    uVar2 = (ushort)uVar5 & 0xfff;
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
code_r0x80116148:
      _DAT_800aca50 = uVar2;
      if (*(byte *)(_DAT_800ac784 + 6) != 0xffffffff) {
        *(short *)(uVar5 + 0x166) = (short)((((_DAT_800ac784 & 0xf) + 10) * 0x1000) / 10);
      }
      *(undefined4 *)(_DAT_800ac784 + 0x78) =
           *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
      uVar3 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
      func_0x8001af5c(0,0,uVar3 + 100,uVar3 + 200,_DAT_800ac784 + 0xb0,0x808080);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(int *)(iVar4 + 0xc84) = iVar4 + 0xec;
      *(int *)(iVar4 + 0xca8) = iVar4 + 0xac;
      *(undefined4 *)(iVar4 + 0xc44) = 0x66;
      *(undefined4 *)(iVar4 + 0xc48) = 0xfffffcd6;
      *(undefined4 *)(iVar4 + 0xc4c) = 0;
      *(undefined2 *)(iVar4 + 0xc78) = 0;
      *(undefined2 *)(iVar4 + 0xc7a) = 0;
      *(undefined2 *)(iVar4 + 0xc7c) = 0;
      func_0x80068098(iVar4 + 0xc78,iVar4 + 0xc30);
      return;
    }
    if (3 < *(byte *)(_DAT_800ac784 + 5)) {
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



/* ======= FUN_801159e8 @ 0x801159e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801159e8(void)

{
  ushort uVar1;
  uint *puVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  byte bVar6;
  uint unaff_s0;
  
  sVar3 = (short)_DAT_800ac784[0x7b];
  if (sVar3 < 0x5dc) {
    func_0x80037edc(1,10);
    FUN_80116270(2,10);
    return;
  }
  if (sVar3 < 3000) {
    func_0x80037edc(3,10);
    func_0x80037edc(4,0x28);
    iVar4 = func_0x80037edc(6,0x28);
    if (((iVar4 != 0) || (FUN_80116b70(5), (byte)_DAT_800ac784[0x74] < 8)) &&
       (bVar6 = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f, bVar6 < 3)) {
      func_0x8004ef90(0x800b1028,bVar6 + 0x1d);
    }
    puVar2 = _DAT_800ac784;
    if ((byte)_DAT_800ac784[0x74] < 2) {
      uVar5 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
      *(char *)((int)_DAT_800ac784 + 7) = (char)uVar5;
    }
    else {
      sVar3 = (byte)_DAT_800ac784[0x74] - 1;
      *(char *)(_DAT_800ac784 + 0x74) = (char)sVar3;
      *(short *)(puVar2[-0xe1f] + 0x1d4) = sVar3;
      unaff_s0 = 0x1fff0000;
      (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedec18))();
      uVar5 = (uint)*(byte *)((int)_DAT_800ac784 + 5);
    }
    (**(code **)(uVar5 * 4 + -0x7fedebd8))();
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
    if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
      *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
      uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
      *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
      *(undefined1 *)(uVar1 + 8) = 0;
      return;
    }
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
    return;
  }
  if (sVar3 < 0xe10) {
    func_0x80037edc(0,8);
    func_0x80037edc(7,0x28);
  }
  return;
}



/* ======= FUN_80115a88 @ 0x80115a88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a88(void)

{
  undefined4 *puVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) != 0)) {
    puVar1 = (undefined4 *)((uint)(byte)_DAT_800ac784[1] * 4 + -0x7feded98);
    (*(code *)*puVar1)();
    func_0x8002b498(_DAT_800ac784);
    if ((byte)_DAT_800ac784[0x74] < 0xd) {
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      iVar4 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
      if (iVar4 != 0) {
        if (DAT_800acae7 == '\0') {
          DAT_800aca58 = 2;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          puVar1 = (undefined4 *)0x800b0000;
          _DAT_800aca5a = 0;
        }
        uVar5 = (uint)_DAT_800acaee;
        sVar3 = _DAT_800acaee + -2;
        if (3 < (int)uVar5) {
          _DAT_800acaee = sVar3;
          if (uVar5 < 5) {
            (**(code **)(&LAB_80100334 + uVar5 * 4))();
            return;
          }
          return;
        }
      }
    }
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x76];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0x1da);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
                    /* WARNING: Could not recover jumptable at 0x801164a4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)((int)puVar1 + *(short *)((int)_DAT_800ac784 + 0x1d6) + 0x34c))();
      return;
    }
    _DAT_800ac784[0xe] =
         (int)*(short *)((int)_DAT_800ac784 + 0x1d6) + ((byte)_DAT_800ac784[0x74] - 1) * -0x14;
  }
  return;
}



/* ======= FUN_80115cb0 @ 0x80115cb0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115cb0(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 100;
  _DAT_800ac784[0x1e] = _DAT_80121264;
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
                    0x80121248);
  }
  return;
}



/* ======= FUN_80115d74 @ 0x80115d74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d74(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  byte bVar2;
  uint *puStack00000010;
  
  puStack00000010 = _DAT_800ac784 + 0x2c;
  func_0x8001af5c(param_1,param_2,600,600);
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
                    0x80121248);
  }
  return;
}



/* ======= FUN_80115d94 @ 0x80115d94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d94(void)

{
  uint uVar1;
  byte bVar2;
  
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
                    0x80121248);
  }
  return;
}



/* ======= FUN_80115dc8 @ 0x80115dc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115dc8(void)

{
  int in_v0;
  uint *puVar1;
  
  puVar1 = *(uint **)(*(int *)(in_v0 + -0x387c) + 0x188);
  *puVar1 = *puVar1 & 0xfffffffe;
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
                    0x80121248);
  }
  return;
}



/* ======= FUN_80115e24 @ 0x80115e24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e24(void)

{
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1d6) = *(undefined2 *)(in_v1 + 0x38);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  _DAT_800ac784[1] = 1;
  *(byte *)((int)_DAT_800ac784 + 5) = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    func_0x80019700(0x9031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    0x80121248);
  }
  return;
}



/* ======= FUN_80115efc @ 0x80115efc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115efc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7feded84))();
  func_0x8001b064(_DAT_800ac784 + 0xb0,0xfffff8f8);
  return;
}



/* ======= FUN_80115f00 @ 0x80115f00 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f00(void)

{
  int in_v0;
  
  (**(code **)((uint)*(byte *)(*(int *)(in_v0 + -0x387c) + 5) * 4 + -0x7feded84))();
  func_0x8001b064(_DAT_800ac784 + 0xb0,0xfffff8f8);
  return;
}



/* ======= FUN_80115f58 @ 0x80115f58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f58(void)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    uVar2 = _DAT_800ac784[0x75];
    *(short *)(_DAT_800ac784 + 0x75) = (short)uVar2 + -1;
    if (((short)uVar2 != 0) || (0x2f < (byte)_DAT_800ac784[0x74])) {
      return;
    }
    if ((byte)_DAT_800ac784[0x74] < 8) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
      goto LAB_80116860;
    }
    FUN_80116bec();
    *(char *)(_DAT_800ac784 + 0x74) = (char)_DAT_800ac784[0x74] + '\x01';
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  else {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        FUN_80116c68();
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
        return;
      }
      goto LAB_80116860;
    }
    if (bVar1 != 0) goto LAB_80116860;
    *(undefined1 *)(_DAT_800ac784 + 0x74) = 0x28;
    FUN_80116d00();
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  *(undefined2 *)(bVar1 + 0x1dd) = 0;
LAB_80116860:
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 4;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  uVar2 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = *(uint *)((uint)*(byte *)((int)_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200,_DAT_800ac784 + 0x2c,0x808080);
  uVar2 = _DAT_800ac784[0x62];
  *(uint *)(uVar2 + 0xc84) = uVar2 + 0xec;
  *(uint *)(uVar2 + 0xca8) = uVar2 + 0xac;
  *(undefined4 *)(uVar2 + 0xc44) = 0x66;
  *(undefined4 *)(uVar2 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar2 + 0xc4c) = 0;
  *(undefined2 *)(uVar2 + 0xc78) = 0;
  *(undefined2 *)(uVar2 + 0xc7a) = 0;
  *(undefined2 *)(uVar2 + 0xc7c) = 0;
  func_0x80068098(uVar2 + 0xc78,uVar2 + 0xc30);
  return;
}



/* ======= FUN_80115f70 @ 0x80115f70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f70(int param_1)

{
  byte bVar1;
  short sVar2;
  int in_v0;
  uint uVar3;
  int in_v1;
  
  if (in_v1 == in_v0) {
    sVar2 = *(short *)(param_1 + 0x1d4);
    *(short *)(param_1 + 0x1d4) = sVar2 + -1;
    if ((sVar2 != 0) || (0x2f < (byte)_DAT_800ac784[0x74])) {
      return;
    }
    if ((byte)_DAT_800ac784[0x74] < 8) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
      goto LAB_80116860;
    }
    FUN_80116bec();
    *(char *)(_DAT_800ac784 + 0x74) = (char)_DAT_800ac784[0x74] + '\x01';
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  else {
    if (1 < in_v1) {
      if (in_v1 == 2) {
        FUN_80116c68();
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
        return;
      }
      goto LAB_80116860;
    }
    if (in_v1 != 0) goto LAB_80116860;
    *(undefined1 *)(param_1 + 0x1d0) = 0x28;
    FUN_80116d00();
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  *(undefined2 *)(bVar1 + 0x1dd) = 0;
LAB_80116860:
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 4;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = *(uint *)((uint)*(byte *)((int)_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200);
  uVar3 = _DAT_800ac784[0x62];
  *(uint *)(uVar3 + 0xc84) = uVar3 + 0xec;
  *(uint *)(uVar3 + 0xca8) = uVar3 + 0xac;
  *(undefined4 *)(uVar3 + 0xc44) = 0x66;
  *(undefined4 *)(uVar3 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar3 + 0xc4c) = 0;
  *(undefined2 *)(uVar3 + 0xc78) = 0;
  *(undefined2 *)(uVar3 + 0xc7a) = 0;
  *(undefined2 *)(uVar3 + 0xc7c) = 0;
  func_0x80068098(uVar3 + 0xc78,uVar3 + 0xc30);
  return;
}



/* ======= FUN_80116068 @ 0x80116068 ======= */

void FUN_80116068(void)

{
  return;
}



/* ======= FUN_80116070 @ 0x80116070 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116070(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = _DAT_800ac784;
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
    bVar1 = *(byte *)(_DAT_800ac784 + 6);
  }
  else {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        FUN_80116c68();
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        return;
      }
      FUN_8011696c();
      return;
    }
    if (bVar1 != 0) {
      FUN_8011696c();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0x2c;
    FUN_80116d00();
    bVar1 = *(byte *)(_DAT_800ac784 + 6);
  }
  if (bVar1 != 0xffffffff) {
    *(short *)(uVar4 + 0x166) = (short)((((_DAT_800ac784 & 0xf) + 10) * 0x1000) / 10);
  }
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
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



/* ======= FUN_8011617c @ 0x8011617c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011617c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7feded70))();
  return;
}



/* ======= FUN_801161e8 @ 0x801161e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161e8(void)

{
  byte in_v0;
  int in_v1;
  
  *(byte *)(in_v1 + 5) = in_v0 & 0x7f;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  return;
}



/* ======= FUN_80116204 @ 0x80116204 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116204(void)

{
  ushort uVar1;
  uint *puVar2;
  short sVar3;
  uint uVar4;
  byte bVar5;
  uint unaff_s0;
  
  bVar5 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar5 == 1) {
    FUN_80116c68();
  }
  else {
    if (1 < bVar5) {
      uVar4 = 0x10000;
      if (bVar5 == 2) {
        _DAT_800ac784[1] = 0x10001;
        *(byte *)((int)_DAT_800ac784 + 5) = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f;
        *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
        return;
      }
      goto LAB_80116b60;
    }
    uVar4 = 3;
    if (bVar5 != 0) goto LAB_80116b60;
    *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 3;
    if ((((byte)_DAT_800ac784[0x74] < 8) || (FUN_80116b70(), (byte)_DAT_800ac784[0x74] < 8)) &&
       (bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f, bVar5 < 3)) {
      func_0x8004ef90(0x800b1028,bVar5 + 0x1d);
    }
    puVar2 = _DAT_800ac784;
    if (1 < (byte)_DAT_800ac784[0x74]) {
      sVar3 = (byte)_DAT_800ac784[0x74] - 1;
      *(char *)(_DAT_800ac784 + 0x74) = (char)sVar3;
      *(short *)(puVar2[-0xe1f] + 0x1d4) = sVar3;
      unaff_s0 = 0x1fff0000;
      (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedec18))();
      uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 5);
      goto LAB_80116b60;
    }
  }
  uVar4 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
  *(char *)((int)_DAT_800ac784 + 7) = (char)uVar4;
LAB_80116b60:
  (**(code **)(uVar4 * 4 + -0x7fedebd8))();
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
  if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
    return;
  }
  uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
  *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
  *(undefined1 *)(uVar1 + 8) = 0;
  return;
}



/* ======= FUN_80116270 @ 0x80116270 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116270(void)

{
  ushort uVar1;
  uint *puVar2;
  short sVar3;
  uint uVar4;
  byte bVar5;
  uint unaff_s0;
  
  FUN_80116b70();
  if (((byte)_DAT_800ac784[0x74] < 8) &&
     (bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f, bVar5 < 3)) {
    func_0x8004ef90(0x800b1028,bVar5 + 0x1d);
  }
  puVar2 = _DAT_800ac784;
  if ((byte)_DAT_800ac784[0x74] < 2) {
    uVar4 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
    *(char *)((int)_DAT_800ac784 + 7) = (char)uVar4;
  }
  else {
    sVar3 = (byte)_DAT_800ac784[0x74] - 1;
    *(char *)(_DAT_800ac784 + 0x74) = (char)sVar3;
    *(short *)(puVar2[-0xe1f] + 0x1d4) = sVar3;
    unaff_s0 = 0x1fff0000;
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedec18))();
    uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 5);
  }
  (**(code **)(uVar4 * 4 + -0x7fedebd8))();
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
  if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_80116370 @ 0x80116370 ======= */
// decompile failed: Exception while decompiling 80116370: process: timeout


/* ======= FUN_801163ec @ 0x801163ec ======= */
// decompile failed: Exception while decompiling 801163ec: process: timeout


/* ======= FUN_80116468 @ 0x80116468 ======= */
// decompile failed: Exception while decompiling 80116468: process: timeout


/* ======= FUN_80116500 @ 0x80116500 ======= */
// decompile failed: Exception while decompiling 80116500: process: timeout


/* ======= FUN_801165b8 @ 0x801165b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801165b8(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedec38))();
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



/* ======= FUN_80116750 @ 0x80116750 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116750(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = *(uint *)((uint)*(byte *)((int)_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
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



/* ======= FUN_8011696c @ 0x8011696c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011696c(int param_1)

{
  int iVar1;
  uint in_v1;
  uint uVar2;
  int iStack00000010;
  undefined4 uStack00000014;
  
  *(short *)(param_1 + 0x166) = (short)((((in_v1 & 0xf) + 10) * 0x1000) / 10);
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  uVar2 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
  iStack00000010 = _DAT_800ac784 + 0xb0;
  uStack00000014 = 0x808080;
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



/* ======= FUN_80116a54 @ 0x80116a54 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116a54(void)

{
  ushort uVar1;
  undefined2 uVar2;
  
  uVar2 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar2;
  if ((ushort)((ushort)_DAT_800ac784[0x76] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x76] & 1;
  }
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedec18))();
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedebd8))();
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
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_80116b70 @ 0x80116b70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116b70(void)

{
  ushort uVar1;
  undefined4 *in_at;
  uint unaff_s0;
  
  (*(code *)*in_at)();
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
  if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_80116bec @ 0x80116bec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116bec(void)

{
  ushort uVar1;
  
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)(_DAT_800ac784 + 0x1e1) = *(char *)(_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)(_DAT_800ac784 + 0x1de);
    *(ushort *)(_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_80116c68 @ 0x80116c68 ======= */

void FUN_80116c68(void)

{
  undefined2 in_v0;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1de) = in_v0;
  *(undefined2 *)(in_v1 + 0x1de) = 0;
  return;
}



/* ======= FUN_80116d00 @ 0x80116d00 ======= */

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



/* ======= FUN_80116d74 @ 0x80116d74 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d74(void)

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



/* ======= FUN_80116f64 @ 0x80116f64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f64(void)

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



/* ======= FUN_80117060 @ 0x80117060 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117060(void)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  ushort uVar4;
  uint uVar5;
  uint in_v1;
  uint uVar6;
  undefined8 uVar7;
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 6);
  uVar5 = (uint)(uVar6 < 2);
  if (uVar6 == 1) {
LAB_80117130:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(4);
    }
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    uVar6 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    uVar7 = FUN_8011bf50(0,0);
    in_v1 = (uint)((ulonglong)uVar7 >> 0x20);
    uVar5 = (uint)uVar7;
  }
  else {
    if (uVar5 == 0) {
      uVar5 = 2;
      if (uVar6 != 2) goto LAB_80117a2c;
      in_v1 = (uint)*(byte *)(_DAT_800ac784 + 7);
      if (in_v1 == 1) {
        uVar2 = 4;
      }
      else {
        if (1 < in_v1) {
          uVar5 = 3;
          if (in_v1 == 2) {
            *(undefined1 *)(_DAT_800ac784 + 5) = 3;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 1;
            return;
          }
          goto LAB_80117a2c;
        }
        uVar5 = 3;
        uVar2 = 3;
        if (in_v1 != 0) goto LAB_80117a2c;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar5 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_80117a2c;
    }
    if (uVar6 != 0) goto LAB_80117a2c;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar5 = (uint)*(byte *)(_DAT_800ac784 + 7);
    if (uVar5 == 1) {
      uVar2 = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    }
    else if (uVar5 < 2) {
      uVar2 = 3;
      if (uVar5 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      }
    }
    else {
      uVar2 = 2;
      if (uVar5 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
        goto LAB_80117130;
      }
    }
    *(undefined1 *)(uVar5 + 6) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x1f) + 0xb4;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x1f) + 0x40;
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
      *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
    }
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    in_v1 = uVar6 / 0xd;
    uVar5 = in_v1 * 3;
  }
LAB_80117a2c:
  if ((uVar6 - (uVar5 * 4 + in_v1) & 0xff) == 1) {
    func_0x800453d0(8);
  }
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8011723c @ 0x8011723c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011723c(void)

{
  byte *pbVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  short *unaff_s0;
  
  if (DAT_800acae7 == 0) {
    unaff_s0 = (short *)&DAT_800aca88;
    iVar4 = func_0x8001a804(3000,0x180,&DAT_800aca88);
    if ((iVar4 < 0) && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar3 = func_0x8001a9cc(&DAT_800aca88,0x20);
      *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
      if ((*(short *)(_DAT_800ac784 + 0x1da) != 0) && (*(short *)(_DAT_800ac784 + 0x1da) < 1)) {
        return;
      }
      uVar5 = func_0x8001a804(0x9c4,0x100,&DAT_800aca88);
      if ((uVar5 & 0x80000000) == 0) {
        return;
      }
      if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
      FUN_80117c78();
      return;
    }
  }
  iVar4 = _DAT_800ac784;
  if (DAT_800acae7 != 0) {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) &&
       (pbVar1 = (byte *)(_DAT_800ac784 + 7), *pbVar1 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *unaff_s0 = *pbVar1 - 6;
      *(undefined2 *)(iVar4 + 0x1dc) = 0x2d;
      func_0x800453d0();
      DAT_800aca58 = 2;
      cVar2 = func_0x8001a780(unaff_s0 + -0x4d);
      DAT_800aca59 = cVar2 + '\x02';
      DAT_800aca5a = 0;
      if (*unaff_s0 < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
      FUN_80118524();
      return;
    }
    if (DAT_800acae7 != 0) {
      return;
    }
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
      ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  if (((DAT_800acae7 == 0) && (6000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_80117490 @ 0x80117490 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117490(void)

{
  bool bVar1;
  undefined4 uVar2;
  byte bVar3;
  char cVar4;
  undefined2 uVar5;
  int iVar6;
  int extraout_v1;
  undefined1 uVar7;
  undefined4 *puVar8;
  undefined1 *puVar9;
  undefined1 *puVar10;
  ushort unaff_s3;
  ushort uVar11;
  short unaff_s4;
  undefined4 uStackX_8;
  undefined4 uStackX_c;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      bVar3 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar3 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
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
    cVar4 = func_0x8001f314(0,0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  }
  uVar7 = 0;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_8011c024(0,0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = uVar7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0'))
      {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        iVar6 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        func_0x80019700(3,(int)*(short *)(iVar6 + 0x6a),*(int *)(iVar6 + 0x188) + 0x40,
                        &stack0xfffffff8);
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0xfffffff8);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
    }
    iVar6 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                            (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar6 != 0) {
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
    puVar8 = &uStackX_8;
    if (DAT_800acae7 != 0) {
      return;
    }
    puVar9 = &DAT_800aca88;
    puVar10 = *(undefined1 **)(_DAT_800ac784 + 0x188);
    uStackX_8 = _DAT_80072d60;
    uStackX_c = _DAT_80072d64;
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(9);
    }
    puVar9 = &stack0xfffffff8;
    if (DAT_800acae7 != 0) {
      return;
    }
    puVar10 = &DAT_800aca88;
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    uVar5 = func_0x8001bff8(iVar6 + 0x448,puVar9,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
    uVar5 = func_0x8001bff8(iVar6 + 0x6f8,puVar9,800,&DAT_800aca88);
    uVar11 = unaff_s3 - 1;
    bVar1 = unaff_s3 != 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    unaff_s3 = uVar11;
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
    puVar8 = (undefined4 *)&DAT_800acaee;
    _DAT_800acaee = _DAT_800acaee + -0xc;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(5);
    _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
    cVar4 = func_0x8001a780(&DAT_800aca54);
    uVar2 = _DAT_800aca58;
    DAT_800aca5b = SUB41(uVar2,3);
    _DAT_800aca58 = (uint3)CONCAT11(cVar4 + '\x04',DAT_800aca58);
    if (_DAT_800acaee < 0) {
      _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
    }
    DAT_800acae7 = DAT_800acae7 | 1;
  }
  uVar5 = func_0x8001bff8(puVar10 + 0x448);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
  uVar5 = func_0x8001bff8(puVar10 + 0x6f8,puVar8,800,puVar9);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
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



/* ======= FUN_80117910 @ 0x80117910 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117910(void)

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



/* ======= FUN_80117a70 @ 0x80117a70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117a70(void)

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
      if (bVar2 == 2) {
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 3;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      FUN_80118524();
      return;
    }
    if (bVar2 != 0) {
      FUN_80118524();
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
      FUN_80118524();
      return;
    }
  }
  return;
}



/* ======= FUN_80117c78 @ 0x80117c78 ======= */

void FUN_80117c78(void)

{
  char cVar1;
  undefined2 in_v0;
  int in_v1;
  short *unaff_s0;
  
  *(undefined2 *)(in_v1 + 0x1dc) = in_v0;
  DAT_800aca58 = 2;
  cVar1 = func_0x8001a780(unaff_s0 + -0x4d);
  DAT_800aca59 = cVar1 + '\x02';
  DAT_800aca5a = 0;
  if (*unaff_s0 < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  FUN_80118524();
  return;
}



/* ======= FUN_80117d4c @ 0x80117d4c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d4c(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  undefined4 uVar4;
  char cVar5;
  undefined2 uVar6;
  int iVar7;
  int extraout_v1;
  undefined1 uVar8;
  undefined1 *unaff_s0;
  undefined4 *unaff_s1;
  undefined1 *unaff_s2;
  ushort uVar9;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  uVar9 = 6;
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        if ((DAT_800acae7 == 0) && (iVar7 = func_0x8001a804(4000,0x180,&DAT_800aca88), iVar7 < 0)) {
          if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
LAB_801180b8:
            *(undefined1 *)(_DAT_800ac784 + 5) = 3;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 0;
            return;
          }
        }
        else {
          if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) goto LAB_801180b8;
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
        }
      }
      goto LAB_801188dc;
    }
    if (bVar2 != 0) goto LAB_801188dc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar5;
  uVar8 = 0;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 6) {
    FUN_8011c024(0,0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = uVar8;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0'))
      {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 3;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        iVar7 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        func_0x80019700(3,(int)*(short *)(iVar7 + 0x6a),*(int *)(iVar7 + 0x188) + 0x40,&uStack_28);
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
      uStack_28 = _DAT_801213a8;
      uStack_24 = _DAT_801213ac;
      uStack_20 = _DAT_801213b0;
      uStack_1c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
    }
    iVar7 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                            (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar7 != 0) {
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
    unaff_s0 = &stack0xffffffe8;
    if (DAT_800acae7 != 0) {
      return;
    }
    unaff_s1 = (undefined4 *)&DAT_800aca88;
    unaff_s2 = *(undefined1 **)(_DAT_800ac784 + 0x188);
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(9);
    }
    unaff_s1 = &uStack_28;
    if (DAT_800acae7 != 0) {
      return;
    }
    unaff_s2 = &DAT_800aca88;
    iVar7 = *(int *)(_DAT_800ac784 + 0x188);
    uStack_28 = _DAT_80072d60;
    uStack_24 = _DAT_80072d64;
    uStack_20 = _DAT_80072d68;
    uStack_1c = _DAT_80072d6c;
    uVar6 = func_0x8001bff8(iVar7 + 0x448,unaff_s1,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
    uVar6 = func_0x8001bff8(iVar7 + 0x6f8,unaff_s1,800,&DAT_800aca88);
    uVar9 = 5;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80121470)) {
        bVar3 = true;
      }
      bVar1 = uVar9 != 0;
      uVar9 = uVar9 - 1;
    } while (bVar1);
    if (!bVar3) {
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d8) == 0) && (*(short *)(_DAT_800ac784 + 0x1da) == 0)) {
      return;
    }
    unaff_s0 = &DAT_800acaee;
    _DAT_800acaee = _DAT_800acaee + -0xc;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(5);
    _DAT_800aca58 = CONCAT31(_DAT_800aca59,2);
    cVar5 = func_0x8001a780(&DAT_800aca54);
    uVar4 = _DAT_800aca58;
    DAT_800aca5b = SUB41(uVar4,3);
    _DAT_800aca58 = (uint3)CONCAT11(cVar5 + '\x04',DAT_800aca58);
    if (_DAT_800acaee < 0) {
      _DAT_800aca58 = CONCAT22(_DAT_800aca5a,3);
    }
    DAT_800acae7 = DAT_800acae7 | 1;
  }
LAB_801188dc:
  uVar6 = func_0x8001bff8(unaff_s2 + 0x448);
  *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
  uVar6 = func_0x8001bff8(unaff_s2 + 0x6f8,unaff_s0,800,unaff_s1);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
  if (uVar9 != 0) {
    do {
      uVar9 = uVar9 - 1;
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80121478)) {
        bVar3 = true;
      }
    } while (uVar9 != 0);
  }
  if ((bVar3) &&
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



/* ======= FUN_80117db0 @ 0x80117db0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117db0(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined1 in_v0;
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
  
  *(undefined1 *)(param_1 + 6) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
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



/* ======= FUN_80118108 @ 0x80118108 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118108(void)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      iVar4 = 3;
      if (bVar1 == 2) {
        if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
          if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) &&
             (*(char *)(_DAT_800ac784 + 7) == '\0')) {
            *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 2;
            *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            *(undefined1 *)(_DAT_800ac784 + 6) = 3;
            FUN_80118dac();
            return;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
          local_28 = _DAT_801213a8;
          local_24 = _DAT_801213ac;
          local_20 = _DAT_801213b0;
          local_1c = _DAT_801213b4;
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
        if (iVar4 == 0) {
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
          iVar4 = func_0x800453d0(7);
        }
        else {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(2);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          iVar4 = _DAT_800ac784;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        }
        goto LAB_80118dc4;
      }
      if (bVar1 != 3) goto LAB_80118dc4;
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar4 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        iVar4 = _DAT_800ac784;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        goto LAB_80118dc4;
      }
      goto LAB_801185bc;
    }
    iVar4 = 1;
    if (bVar1 != 0) goto LAB_80118dc4;
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
    uVar5 = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(char *)(_DAT_800ac784 + 0x93) = (char)uVar5;
    *(undefined2 *)(uVar5 + 0x8c) = 0;
    iVar4 = -0x7ff50000;
LAB_80118dc4:
    *(undefined2 *)(*(int *)(iVar4 + -0x387c) + 0x9c) = 0;
    iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar4 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
LAB_801185bc:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80118524 @ 0x80118524 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118524(void)

{
  int iVar1;
  
  if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    iVar1 = func_0x800453d0(7);
    *(undefined2 *)(*(int *)(iVar1 + -0x387c) + 0x9c) = 0;
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
  return;
}



/* ======= FUN_801185dc @ 0x801185dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801185dc(void)

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
      if (iVar5 == 0) {
        func_0x800245d8(0);
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
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
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) && (*(char *)(_DAT_800ac784 + 7) == '\0')) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 3;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      iVar5 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      func_0x80019700(3,(int)*(short *)(iVar5 + 0x6a),*(int *)(iVar5 + 0x188) + 0x40,&local_48);
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    local_48 = _DAT_801213a8;
    local_44 = _DAT_801213ac;
    local_40 = _DAT_801213b0;
    local_3c = _DAT_801213b4;
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
        DAT_800acae7 = DAT_800acae7 | 1;
        _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
        _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
        _DAT_800acbfc = _DAT_800ac784;
        return;
      }
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800453d0(2);
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  return;
}



/* ======= FUN_80118a84 @ 0x80118a84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118a84(void)

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
      local_28 = _DAT_801213a8;
      local_24 = _DAT_801213ac;
      local_20 = _DAT_801213b0;
      local_1c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_80118b6c @ 0x80118b6c ======= */
// decompile failed: Exception while decompiling 80118b6c: process: timeout


/* ======= FUN_80118dac @ 0x80118dac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118dac(void)

{
  undefined1 in_v0;
  int iVar1;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 0x8f) = in_v0;
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



/* ======= FUN_80118e34 @ 0x80118e34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118e34(void)

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
        FUN_8011970c();
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = _DAT_801213a8;
      local_24 = _DAT_801213ac;
      local_20 = _DAT_801213b0;
      local_1c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_80118f1c @ 0x80118f1c ======= */
// decompile failed: Exception while decompiling 80118f1c: process: timeout


/* ======= FUN_80119198 @ 0x80119198 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119198(void)

{
  int iVar1;
  int iVar2;
  byte bVar3;
  ushort uVar4;
  undefined2 uVar5;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_1c;
  
  bVar3 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar3 & 2) != 0) {
    if ((bVar3 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      iVar1 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      iVar2 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(short *)(iVar1 + 0x1d8) = (short)iVar2;
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 8) ||
         (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x21 < 5)) {
        uVar4 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 0x50;
        bVar3 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x30) + 0x30;
        if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
          *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
        }
        func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      local_1c = (uint)(*(byte *)(_DAT_800ac784 + 0x95) < 0x19);
      if (local_1c == 0) {
        uVar5 = func_0x8001bff8(0x39c,0);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
        if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 2;
        }
        else {
          if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
            return;
          }
          *(undefined1 *)(_DAT_800ac784 + 6) = 7;
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
        }
        *(undefined4 *)(_DAT_800ac784 + 0x78) =
             *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
        func_0x800453d0();
        (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
        return;
      }
      FUN_8011c024(0,1);
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar3 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8011926c @ 0x8011926c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011926c(void)

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



/* ======= FUN_80119438 @ 0x80119438 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119438(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  undefined1 *puVar6;
  char unaff_s0;
  byte unaff_s1;
  int iVar7;
  char unaff_s3;
  char unaff_s4;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar2 & 2) != 0) {
    if ((bVar2 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      iVar7 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(iVar7 + 0x9c) = 0;
      func_0x800453d0();
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
        FUN_8011a5f8(0,0);
        return;
      }
      FUN_8011c024(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 == 0) {
        iVar7 = *(int *)(_DAT_800ac784 + 0x188);
        local_28 = _DAT_80072d60;
        local_24 = _DAT_80072d64;
        local_20 = _DAT_80072d68;
        local_1c = _DAT_80072d6c;
        bVar2 = unaff_s0 - 1;
        if (unaff_s0 != '\0') {
          do {
            if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80121480)) {
              uVar5 = func_0x8001bff8(iVar7 + 0x448,&local_28,800,&DAT_800aca88);
              *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
            }
            bVar1 = bVar2 != 0;
            bVar2 = bVar2 - 1;
          } while (bVar1);
          iVar7 = func_0x8001c1a4();
          if (iVar7 != 0) {
            *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
            *(undefined4 *)(_DAT_800ac784 + 0x78) =
                 *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
            func_0x800453d0(2);
          }
          (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb58))();
          return;
        }
        while (bVar1 = unaff_s1 != 0, unaff_s1 = unaff_s1 - 1, bVar1) {
          if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x80121488)) {
            uVar5 = func_0x8001bff8(iVar7 + 0x6f8,&local_28,800,&DAT_800aca88);
            *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
            if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
              unaff_s4 = '\x01';
            }
          }
        }
        if ((unaff_s3 != '\0') || (unaff_s4 != '\0')) {
          _DAT_800acaee = _DAT_800acaee - 0xc;
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
          func_0x800453d0(6);
          DAT_800aca58 = 2;
          puVar6 = &DAT_800aca54;
          cVar3 = func_0x8001a780();
          DAT_800aca59 = cVar3 + '\x02';
          DAT_800aca5a = 0;
          if ((short)_DAT_800acaee < 0) {
            DAT_800aca58 = 3;
            DAT_800aca59 = '\0';
          }
          uVar4 = (ushort)DAT_800acae7;
          DAT_800acae7 = (byte)(uVar4 | 1);
          if (_DAT_800acaee == (uVar4 | 1)) {
            puVar6[0x93] = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 1;
            *(undefined1 *)(_DAT_800ac784 + 5) = 7;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 0;
            if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
              *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            }
            return;
          }
          FUN_8011bb8c();
          return;
        }
      }
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar2 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8011950c @ 0x8011950c ======= */
// decompile failed: Exception while decompiling 8011950c: process: timeout


/* ======= FUN_8011970c @ 0x8011970c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011970c(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  char in_v0;
  char cVar2;
  undefined2 uVar3;
  ushort uVar4;
  int iVar5;
  undefined1 *puVar6;
  byte unaff_s1;
  int unaff_s2;
  char unaff_s3;
  char unaff_s4;
  
  *(char *)(*(int *)(param_3 + -0x387c) + 6) = *(char *)(*(int *)(param_3 + -0x387c) + 6) + in_v0;
  iVar5 = FUN_8011c024(0,0);
  do {
    if (*(short *)(iVar5 + 0x1da) != 0) {
      unaff_s4 = '\x01';
    }
    do {
      bVar1 = unaff_s1 == 0;
      unaff_s1 = unaff_s1 - 1;
      if (bVar1) {
        if ((unaff_s3 == '\0') && (unaff_s4 == '\0')) {
          return;
        }
        _DAT_800acaee = _DAT_800acaee - 0xc;
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
        func_0x800453d0(6);
        DAT_800aca58 = 2;
        puVar6 = &DAT_800aca54;
        cVar2 = func_0x8001a780();
        DAT_800aca59 = cVar2 + '\x02';
        DAT_800aca5a = 0;
        if ((short)_DAT_800acaee < 0) {
          DAT_800aca58 = 3;
          DAT_800aca59 = '\0';
        }
        uVar4 = (ushort)DAT_800acae7;
        DAT_800acae7 = (byte)(uVar4 | 1);
        if (_DAT_800acaee != (uVar4 | 1)) {
          FUN_8011bb8c();
          return;
        }
        puVar6[0x93] = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
        return;
      }
    } while (*(char *)(_DAT_800ac784 + 0x95) != *(char *)(unaff_s1 + 0x80121488));
    uVar3 = func_0x8001bff8(unaff_s2 + 0x6f8,&stack0x00000010,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
    iVar5 = _DAT_800ac784;
  } while( true );
}



/* ======= FUN_80119770 @ 0x80119770 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119770(void)

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
      FUN_8011a1c8();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = _DAT_801213a8;
    local_34 = _DAT_801213ac;
    local_30 = _DAT_801213b0;
    local_2c = _DAT_801213b4;
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
    FUN_8011a1c8();
    return;
  }
  bVar5 = 2;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8012147c)[bVar5]) {
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



/* ======= FUN_801199f8 @ 0x801199f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801199f8(void)

{
  byte bVar1;
  ushort uVar2;
  undefined2 uVar3;
  
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
    uVar3 = func_0x8001bff8(0x39c,0);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar3;
    if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    else {
      if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
    }
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
    func_0x800453d0();
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
    return;
  }
  FUN_8011c024(0,1);
  return;
}



/* ======= FUN_80119b78 @ 0x80119b78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119b78(void)

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
      FUN_8011a43c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_80119c4c @ 0x80119c4c ======= */

/* WARNING: Removing unreachable block (ram,0x80119f78) */
/* WARNING: Removing unreachable block (ram,0x80119f7c) */
/* WARNING: Removing unreachable block (ram,0x80119f88) */
/* WARNING: Removing unreachable block (ram,0x80119f94) */
/* WARNING: Removing unreachable block (ram,0x80119fa0) */
/* WARNING: Removing unreachable block (ram,0x8011a000) */
/* WARNING: Removing unreachable block (ram,0x8011a010) */
/* WARNING: Removing unreachable block (ram,0x80119f04) */
/* WARNING: Removing unreachable block (ram,0x80119f30) */
/* WARNING: Removing unreachable block (ram,0x80119f74) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119c4c(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  byte bVar4;
  ushort uVar5;
  undefined2 uVar6;
  int iVar7;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_80119d04:
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x3f) + 100;
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
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x28) {
      FUN_8011c024(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 != '\0') {
        return;
      }
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      local_28 = _DAT_80072d60;
      local_24 = _DAT_80072d64;
      local_20 = _DAT_80072d68;
      local_1c = _DAT_80072d6c;
      bVar2 = 5;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80121480)) {
          uVar6 = func_0x8001bff8(iVar7 + 0x448,&local_28,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
        }
        bVar1 = bVar2 != 0;
        bVar2 = bVar2 - 1;
      } while (bVar1);
      iVar7 = func_0x8001c1a4();
      if (iVar7 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        *(undefined4 *)(_DAT_800ac784 + 0x78) =
             *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
        func_0x800453d0(2);
      }
      (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb58))();
      return;
    }
    FUN_8011a5f8(0,0);
    return;
  }
  if (bVar2 < 2) {
    bVar4 = 1;
    if (bVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x800453d0(9);
      goto LAB_80119d04;
    }
  }
  else {
    bVar4 = 0xd;
    if (bVar2 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
  }
  if (bVar2 == bVar4) {
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    }
    return;
  }
  FUN_8011bb8c();
  return;
}



/* ======= FUN_8011a078 @ 0x8011a078 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a078(void)

{
  byte bVar1;
  char cVar2;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) != 0) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      cVar2 = func_0x8001f314();
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
      FUN_8011bb8c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8011a160 @ 0x8011a160 ======= */
// decompile failed: Exception while decompiling 8011a160: process: timeout


/* ======= FUN_8011a1c8 @ 0x8011a1c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a1c8(void)

{
  undefined2 uVar1;
  int iVar2;
  ushort extraout_var;
  int in_v1;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1c;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x800453d0(7);
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\x04') &&
     (iVar2 = func_0x8001a780(&DAT_800aca54), iVar2 == 0)) {
    in_stack_00000010 = _DAT_80072d60;
    in_stack_00000014 = _DAT_80072d64;
    in_stack_00000018 = _DAT_80072d68;
    in_stack_0000001c = _DAT_80072d6c;
    func_0x8001a804(3000,0x100,&DAT_800aca88);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = extraout_var >> 0xf;
    if ((DAT_800acae7 != '\0') || (*(short *)(_DAT_800ac784 + 0x1d8) == 0)) goto LAB_8011a2f0;
LAB_8011a3ac:
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  else {
LAB_8011a2f0:
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
      return;
    }
    iVar2 = func_0x8001a780(&DAT_800aca54);
    if (iVar2 != 0) {
      in_stack_00000010 = _DAT_80072d60;
      in_stack_00000014 = _DAT_80072d64;
      in_stack_00000018 = _DAT_80072d68;
      in_stack_0000001c = _DAT_80072d6c;
      uVar1 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0x39c,&stack0x00000010,800,
                              &DAT_800aca88);
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar1;
      if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) goto LAB_8011a3ac;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
  }
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}



/* ======= FUN_8011a43c @ 0x8011a43c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a43c(void)

{
  char cVar1;
  int unaff_s0;
  
  DAT_800aca58 = 5;
  *(uint *)(unaff_s0 + -0x1a8) = *(uint *)(unaff_s0 + -0x1a8) | 0x1000;
  DAT_800aca59 = func_0x8001a780();
  DAT_800aca5a = 0;
  DAT_800aca5b = 0;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(unaff_s0 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}



/* ======= FUN_8011a5f8 @ 0x8011a5f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5f8(void)

{
  *(undefined1 *)(_DAT_800ac784 + 5) = 2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}



/* ======= FUN_8011a75c @ 0x8011a75c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a75c(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
    func_0x800453d0(2);
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb58))();
  return;
}



/* ======= FUN_8011a818 @ 0x8011a818 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a818(void)

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
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
        return;
      }
      goto LAB_8011b1dc;
    }
    if (bVar1 != 0) goto LAB_8011b1dc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar3 + -0x7fedec74);
    local_18 = *(undefined4 *)(iVar3 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar3 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_8011b1dc:
  FUN_8011bb8c();
  return;
}



/* ======= FUN_8011a9ec @ 0x8011a9ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a9ec(undefined4 param_1,int param_2)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  uint uVar6;
  int unaff_s0;
  int local_20;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  bVar2 = bVar1 < 2;
  if (bVar1 != 1) {
    if (!(bool)bVar2) {
      bVar2 = 1;
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_8011b3f0;
    }
    if (bVar1 != 0) goto LAB_8011b3f0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar5 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(int *)(iVar5 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar5 + -0x7fedec74);
    local_18 = *(undefined4 *)(iVar5 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar5 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(3);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    param_2 = 0;
    bVar2 = FUN_8011bf50(0);
  }
  else {
    param_2 = 1;
    bVar2 = FUN_8011bf50(0);
  }
LAB_8011b3f0:
  *(byte *)(param_2 + 0x93) = bVar2 | 2;
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
  local_20 = *(int *)(iVar5 + -0x7fedec78);
  local_1c = *(undefined4 *)(iVar5 + -0x7fedec74);
  local_18 = *(int *)(iVar5 + -0x7fedec70);
  local_14 = *(undefined4 *)(iVar5 + -0x7fedec6c);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
  func_0x800453d0(0);
  func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  uVar6 = func_0x800245d8(0);
  if ((uVar6 & 0x20) == 0) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_20 = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18 = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_1c = 0;
    func_0x80039e7c(&local_20,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedea68))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(unaff_s0 + 0x14,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011ac00 @ 0x8011ac00 ======= */
// decompile failed: Exception while decompiling 8011ac00: process: timeout


/* ======= FUN_8011aefc @ 0x8011aefc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011aefc(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
    func_0x800453d0(2);
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}



/* ======= FUN_8011afb8 @ 0x8011afb8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011afb8(void)

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
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_8011b988();
      return;
    }
    if (bVar1 != 0) {
      FUN_8011b988();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar3 + -0x7fedec74);
    local_18 = *(undefined4 *)(iVar3 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar3 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_8011bf50(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) != '=') {
    return;
  }
  func_0x800453d0(2);
  FUN_8011b988();
  return;
}



/* ======= FUN_8011b198 @ 0x8011b198 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b198(void)

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
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_8011bb8c();
      return;
    }
    if (bVar1 != 0) {
      FUN_8011bb8c();
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
    local_20 = *(undefined4 *)(iVar3 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar3 + -0x7fedec74);
    local_18 = *(undefined4 *)(iVar3 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar3 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
    return;
  }
  func_0x800453d0(2);
  FUN_8011bb8c();
  return;
}



/* ======= FUN_8011b39c @ 0x8011b39c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b39c(void)

{
  byte bVar1;
  char cVar2;
  byte bVar3;
  ushort uVar4;
  int iVar5;
  int unaff_s0;
  int local_20;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  bVar3 = bVar1 < 2;
  if (bVar1 != 1) {
    if (!(bool)bVar3) {
      bVar3 = 0;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8011bddc;
    }
    if (bVar1 != 0) goto LAB_8011bddc;
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
    local_20 = *(int *)(iVar5 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar5 + -0x7fedec74);
    local_18 = *(int *)(iVar5 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar5 + -0x7fedec6c);
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
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  bVar3 = func_0x800245d8(0);
LAB_8011bddc:
  if ((bVar3 & 0x20) == 0) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_20 = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18 = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_1c = 0;
    func_0x80039e7c(&local_20,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedea68))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(unaff_s0 + 0x14,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8011b5ec @ 0x8011b5ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b5ec(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeaa8))();
  return;
}



/* ======= FUN_8011b750 @ 0x8011b750 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b750(undefined4 param_1,int param_2)

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



/* ======= FUN_8011b824 @ 0x8011b824 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b824(undefined4 param_1,int param_2)

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



/* ======= FUN_8011b988 @ 0x8011b988 ======= */

void FUN_8011b988(void)

{
  code *UNRECOVERED_JUMPTABLE;
  
                    /* WARNING: Could not recover jumptable at 0x8011b98c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}



/* ======= FUN_8011bb8c @ 0x8011bb8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bb8c(void)

{
  int in_at;
  undefined4 in_v0;
  
  *(undefined4 *)(in_at + -0x35a8) = in_v0;
  DAT_800acae7 = 0;
  _DAT_800aca3c = _DAT_800aca3c & 0xffffff3f;
  return;
}



/* ======= FUN_8011bc14 @ 0x8011bc14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc14(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = _DAT_800aca58 >> 0x10 & 0xff;
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        func_0x80037edc(0,10);
        func_0x80037edc(5,0x32);
        func_0x80037edc(7,0x32);
        _DAT_800aca58 = 7;
        return;
      }
      goto LAB_8011c588;
    }
    if (uVar2 != 0) goto LAB_8011c588;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x80121570);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x80121570);
  }
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  uVar2 = (_DAT_800aca58 >> 0x10 & 0xff) + iVar1;
  _DAT_800aca58 = CONCAT12((char)uVar2,_DAT_800aca58);
LAB_8011c588:
  *(undefined1 *)(*(int *)(uVar2 - 0x387c) + 0x94) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0xb0,0x808080);
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ======= FUN_8011bda0 @ 0x8011bda0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bda0(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedea68))();
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



/* ======= FUN_8011bedc @ 0x8011bedc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bedc(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80121594;
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



/* ======= FUN_8011bf50 @ 0x8011bf50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bf50(void)

{
  int in_v1;
  int iVar1;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 8;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(iVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(iVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(iVar1 + 0x5f4) = 0;
  *(undefined2 *)(iVar1 + 0x5f6) = 0;
  *(undefined2 *)(iVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
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



/* ======= FUN_8011c024 @ 0x8011c024 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c024(void)

{
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c();
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}



/* ======= FUN_8011c084 @ 0x8011c084 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c084(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fedea54))();
  return;
}



/* ======= FUN_8011c150 @ 0x8011c150 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c150(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedea50))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedea30))();
  return;
}



/* ======= FUN_8011c1cc @ 0x8011c1cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c1cc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedea10))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede9f0))();
  return;
}



/* ======= FUN_8011c248 @ 0x8011c248 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c248(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede9d0))();
  return;
}



/* ======= FUN_8011c290 @ 0x8011c290 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c290(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede9d0))();
  return;
}



/* ======= FUN_8011c2d8 @ 0x8011c2d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c2d8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede9d0))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c370 @ 0x8011c370 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c370(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede998))();
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



/* ======= FUN_8011c4ac @ 0x8011c4ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c4ac(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80121664;
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



/* ======= FUN_8011c654 @ 0x8011c654 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c654(void)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  short in_stack_fffffff0;
  
  cVar1 = func_0x800509e4(15000,&stack0xfffffff0,0x800,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)in_stack_fffffff0,-(int)in_stack_fffffff0);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
      iVar3 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >>
              0x10;
      iVar4 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >>
              0x10;
      uVar2 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
      *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
      func_0x8001bd60(0xfffffff6,0x14);
      func_0x80039e7c(&DAT_800aca88,0,0);
      (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede860))();
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
  uVar2 = func_0x8005070c(0x5dc,(int)in_stack_fffffff0,-(int)in_stack_fffffff0);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede984))();
  return;
}



/* ======= FUN_8011c720 @ 0x8011c720 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c720(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede980))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede960))();
  return;
}



/* ======= FUN_8011c79c @ 0x8011c79c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c79c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede940))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede920))();
  return;
}



/* ======= FUN_8011c818 @ 0x8011c818 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c818(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede900))();
  return;
}



/* ======= FUN_8011c860 @ 0x8011c860 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c860(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede900))();
  return;
}



/* ======= FUN_8011c8a8 @ 0x8011c8a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c8a8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede900))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011c940 @ 0x8011c940 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c940(void)

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
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede8c8))();
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



/* ======= FUN_8011cab8 @ 0x8011cab8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cab8(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80121734;
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



/* ======= FUN_8011cc60 @ 0x8011cc60 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cc60(void)

{
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede8b4))();
  func_0x80012aa4(3000);
  return;
}



/* ======= FUN_8011ccb4 @ 0x8011ccb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ccb4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede8b0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede8a4))();
  return;
}



/* ======= FUN_8011cd30 @ 0x8011cd30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cd30(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede898))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede898))();
  return;
}



/* ======= FUN_8011cdac @ 0x8011cdac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cdac(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede898))();
  return;
}



/* ======= FUN_8011cdf4 @ 0x8011cdf4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011cdf4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede898))();
  return;
}



/* ======= FUN_8011ce3c @ 0x8011ce3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce3c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede898))();
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
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede860))();
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
  _DAT_800ac784[0x1e] = _DAT_8012179c;
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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d1f4(void)

{
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede84c))();
  func_0x80012aa4(3000);
  return;
}



/* ======= FUN_8011d248 @ 0x8011d248 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d248(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede848))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede83c))();
  return;
}



/* ======= FUN_8011d2c4 @ 0x8011d2c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d2c4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede830))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede830))();
  return;
}



/* ======= FUN_8011d340 @ 0x8011d340 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d340(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede830))();
  return;
}



/* ======= FUN_8011d388 @ 0x8011d388 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d388(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede830))();
  return;
}



/* ======= FUN_8011d3d0 @ 0x8011d3d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d3d0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede830))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011d468 @ 0x8011d468 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d468(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede7f8))();
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



/* ======= FUN_8011d598 @ 0x8011d598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d598(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80121804;
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



/* ======= FUN_8011d740 @ 0x8011d740 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d740(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede7e4))();
  return;
}



/* ======= FUN_8011d80c @ 0x8011d80c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d80c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede7e0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede7c0))();
  return;
}



/* ======= FUN_8011d888 @ 0x8011d888 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d888(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede7a0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede780))();
  return;
}



/* ======= FUN_8011d904 @ 0x8011d904 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d904(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede760))();
  return;
}



/* ======= FUN_8011d94c @ 0x8011d94c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d94c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede760))();
  return;
}



/* ======= FUN_8011d994 @ 0x8011d994 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d994(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede760))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011da2c @ 0x8011da2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011da2c(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede728))();
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



/* ======= FUN_8011db70 @ 0x8011db70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011db70(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_801218d4;
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



/* ======= FUN_8011dd18 @ 0x8011dd18 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dd18(void)

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
    FUN_8011e634();
    return;
  }
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede714))();
  return;
}



/* ======= FUN_8011de44 @ 0x8011de44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011de44(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede70c))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede6ec))();
  return;
}



/* ======= FUN_8011dec0 @ 0x8011dec0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dec0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede6cc))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede6ac))();
  return;
}



/* ======= FUN_8011df3c @ 0x8011df3c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011df3c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede68c))();
  return;
}



/* ======= FUN_8011df84 @ 0x8011df84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011df84(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fede678))();
  return;
}



/* ======= FUN_8011dfcc @ 0x8011dfcc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011dfcc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fede678))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_8011e064 @ 0x8011e064 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e064(void)

{
  _DAT_80072bf8 = 0x8010a8c8;
  _DAT_80072bec = FUN_80100424;
  _DAT_80072bf0 = FUN_80100424;
  _DAT_80072bf4 = FUN_80100424;
  _DAT_80072c04 = FUN_80100424;
  _DAT_80072c0c = FUN_80100424;
  _DAT_80072c1c = FUN_80100424;
  _DAT_80072c20 = FUN_80100424;
  _DAT_80072c24 = FUN_80100424;
  _DAT_80072c28 = FUN_80100424;
  _DAT_80072c2c = 0x8010d7f8;
  _DAT_80072c30 = 0x80112020;
  _DAT_80072c44 = 0x80116288;
  _DAT_80072c48 = 0x80116db8;
  _DAT_80072cac = 0x8011c5a0;
  _DAT_80072cb4 = 0x8011cb70;
  _DAT_80072cc0 = 0x8011d140;
  _DAT_80072cc8 = 0x8011d6d4;
  _DAT_80072cd0 = 0x8011dc68;
  _DAT_80072cd8 = 0x8011e22c;
  _DAT_800b52c8 = &DAT_8011eae4;
  _DAT_800ac998 = &LAB_8011efc4;
  _DAT_800ac99c = &LAB_8011f734;
  _DAT_80072c14 = 0x8010c1ec;
  _DAT_800ac798 = 0x8010a28c;
  _DAT_800ac79c = 0x8010a28c;
  _DAT_800ac7a0 = 0x8010a28c;
  _DAT_800ac7a4 = 0x8010c16c;
  _DAT_800ac7b0 = 0x8010a28c;
  _DAT_800ac7b8 = 0x8010a28c;
  _DAT_800ac7c8 = 0x8010a28c;
  _DAT_800ac7cc = 0x8010a28c;
  _DAT_800ac7d0 = 0x8010a28c;
  _DAT_800ac7d4 = 0x8010a28c;
  _DAT_800ac7d8 = 0x80111944;
  _DAT_800ac7dc = 0x8011597c;
  _DAT_800ac7f4 = 0x8011c118;
  _DAT_800ac898 = 0x8010a6b8;
  _DAT_800ac89c = 0x8010a6b8;
  _DAT_800ac8a0 = 0x8010a6b8;
  _DAT_800ac8b0 = 0x8010a6b8;
  _DAT_800ac8b8 = 0x8010a6b8;
  _DAT_800ac8c8 = 0x8010a6b8;
  _DAT_800ac8cc = 0x8010a6b8;
  _DAT_800ac8d0 = 0x8010a6b8;
  _DAT_800ac8d4 = 0x8010a6b8;
  _DAT_800ac8d8 = 0x80111cb0;
  _DAT_800ac8dc = 0x80115d2c;
  _DAT_800ac8a4 = 0x8010c1ac;
  _DAT_800ac8f4 = 0x8011c3d4;
  func_0x80029afc();
  return;
}



/* ======= FUN_8011e634 @ 0x8011e634 ======= */

/* WARNING: Control flow encountered unimplemented instructions */

void FUN_8011e634(void)

{
                    /* WARNING: Unimplemented instruction - Truncating control flow here */
  halt_unimplemented();
}



