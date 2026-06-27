/* ======= FUN_80100000 @ 0x80100000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(undefined4 param_1)

{
  short in_v0;
  byte bVar1;
  short unaff_s0;
  int unaff_s1;
  int iVar2;
  
  iVar2 = unaff_s1 * 8;
  *(short *)(_DAT_800ac784 + 0x1da) = in_v0 + unaff_s0;
  func_0x80019700(param_1,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_801178a8)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                 );
  func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_801178a8_1)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_801178a8_2)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&LAB_801178d8);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_801178a8_3)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&LAB_801178d8);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_801178ac)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&LAB_801178d8);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_801178ad)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                 );
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_801178ae)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&LAB_801178d8);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_801178af)[iVar2] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&LAB_801178d8);
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



/* ======= FUN_8010069c @ 0x8010069c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010069c(void)

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



/* ======= FUN_801006ec @ 0x801006ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801006ec(void)

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



/* ======= FUN_801007d0 @ 0x801007d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801007d0(void)

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



/* ======= FUN_801008b8 @ 0x801008b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801008b8(undefined4 param_1)

{
  uint uVar1;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_801178a8)[(unaff_s0 & 0xff) * 8] * 0xac +
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
  (**(code **)(&LAB_80117910 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80100d20 @ 0x80100d20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100d20(void)

{
  (**(code **)(&LAB_80117944 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117994 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100d9c @ 0x80100d9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100d9c(void)

{
  (**(code **)(&LAB_801179e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117a24 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100e18 @ 0x80100e18 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e18(void)

{
  (**(code **)(&LAB_80117a64 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117994 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100e94 @ 0x80100e94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e94(void)

{
  (**(code **)(&LAB_80117ab4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117a24 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100f10 @ 0x80100f10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f10(void)

{
  (**(code **)(&LAB_80117acc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ad0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100f8c @ 0x80100f8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f8c(void)

{
  (**(code **)(&LAB_80117ad4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ad8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101008 @ 0x80101008 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101008(void)

{
  (**(code **)(&LAB_80117adc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ad8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101084 @ 0x80101084 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101084(void)

{
  (**(code **)(&LAB_80117ae0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ad8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101100 @ 0x80101100 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101100(void)

{
  (**(code **)(&LAB_80117ae4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ae8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010117c @ 0x8010117c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010117c(void)

{
  (**(code **)(&LAB_80117aec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80117af0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801011f8 @ 0x801011f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011f8(void)

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



/* ======= FUN_80101310 @ 0x80101310 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101310(void)

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



/* ======= FUN_8010139c @ 0x8010139c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010139c(void)

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



/* ======= FUN_80101478 @ 0x80101478 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101478(void)

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



/* ======= FUN_80101584 @ 0x80101584 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101584(void)

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
    FUN_80101ed0(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_801016ec @ 0x801016ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016ec(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
    FUN_80102078();
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



/* ======= FUN_8010188c @ 0x8010188c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010188c(void)

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
      *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_80117bf4)[uVar5 & 0x1f];
      FUN_801021c4();
      return;
    }
    uVar5 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x1de) =
         *(undefined1 *)((int)&PTR_LAB_80117c04 + (uVar5 & 0x1f));
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_801023b4(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80101bdc @ 0x80101bdc ======= */
// decompile failed: Exception while decompiling 80101bdc: process: timeout


/* ======= FUN_80101ed0 @ 0x80101ed0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ed0(short param_1,int param_2)

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
  FUN_80102a50();
  return;
}



/* ======= FUN_80102078 @ 0x80102078 ======= */

void FUN_80102078(void)

{
  int in_v1;
  int iVar1;
  
  iVar1 = *(int *)(in_v1 + -0x387c);
  if (((((*(byte *)(iVar1 + 0x1c2) & 2) != 0) && (7 < *(byte *)(iVar1 + 0x95))) &&
      (iVar1 = *(int *)(iVar1 + 0x1ac), -1 < *(short *)(iVar1 + 0x9a))) &&
     (((*(byte *)(iVar1 + 9) & 0x80) == 0 && ((*(ushort *)(iVar1 + 0x1d8) & 1) == 0)))) {
    *(undefined4 *)(iVar1 + 4) = 0xb01;
    FUN_80102a50();
    return;
  }
  return;
}



/* ======= FUN_801021c4 @ 0x801021c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801021c4(void)

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
    FUN_80102a50();
    return;
  }
  return;
}



/* ======= FUN_8010226c @ 0x8010226c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010226c(void)

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



/* ======= FUN_801023b4 @ 0x801023b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801023b4(void)

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



/* ======= FUN_8010245c @ 0x8010245c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010245c(void)

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



/* ======= FUN_801025b0 @ 0x801025b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801025b0(void)

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
          FUN_80109304(0,0);
        }
      }
      else if ((*(ushort *)(iVar3 + 0x1d8) & 0x40) == 0) {
        FUN_801036d0(0,1);
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



/* ======= FUN_801026a8 @ 0x801026a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801026a8(void)

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
  FUN_80109230(0,0);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80102878 @ 0x80102878 ======= */
// decompile failed: Exception while decompiling 80102878: process: timeout


/* ======= FUN_80102a50 @ 0x80102a50 ======= */

void FUN_80102a50(void)

{
  return;
}



/* ======= FUN_80102a5c @ 0x80102a5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102a5c(void)

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
        FUN_8010347c();
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
      FUN_8010347c();
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
  FUN_8010347c();
  return;
}



/* ======= FUN_80102c8c @ 0x80102c8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102c8c(void)

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



/* ======= FUN_80102d70 @ 0x80102d70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102d70(void)

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
        FUN_80109304(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_801036d0(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_80102ef0 @ 0x80102ef0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102ef0(void)

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



/* ======= FUN_80103014 @ 0x80103014 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103014(void)

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



/* ======= FUN_80103090 @ 0x80103090 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103090(void)

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



/* ======= FUN_801030ec @ 0x801030ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801030ec(uint param_1)

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



/* ======= FUN_80103228 @ 0x80103228 ======= */
// decompile failed: Exception while decompiling 80103228: process: timeout


/* ======= FUN_8010347c @ 0x8010347c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010347c(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x40,&LAB_801178d8);
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
  FUN_801048cc();
  return;
}



/* ======= FUN_801036d0 @ 0x801036d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801036d0(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_80117888)[*(byte *)(param_3 + 8)] * 0xac +
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
  FUN_801048cc();
  return;
}



/* ======= FUN_8010380c @ 0x8010380c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010380c(void)

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
    puVar3 = (uint *)((uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80103bdc @ 0x80103bdc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103bdc(void)

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



/* ======= FUN_80103d00 @ 0x80103d00 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103d00(void)

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



/* ======= FUN_80103df4 @ 0x80103df4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103df4(void)

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



/* ======= FUN_80103e9c @ 0x80103e9c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103e9c(void)

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



/* ======= FUN_80103f7c @ 0x80103f7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103f7c(void)

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
        FUN_801048cc();
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
      FUN_801048cc();
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
  FUN_801048cc();
  return;
}



/* ======= FUN_8010400c @ 0x8010400c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010400c(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_801048cc();
  return;
}



/* ======= FUN_801040e4 @ 0x801040e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801040e4(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801049bc();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      FUN_801049bc();
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
  FUN_801049bc();
  return;
}



/* ======= FUN_801041d4 @ 0x801041d4 ======= */
// decompile failed: Exception while decompiling 801041d4: process: timeout


/* ======= FUN_80104614 @ 0x80104614 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104614(void)

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
      goto LAB_80104fa8;
    }
    if (bVar2 != 0) goto LAB_80104fa8;
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
LAB_80104fa8:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109230(0,0);
    return;
  }
  FUN_80105898(0,1);
  return;
}



/* ======= FUN_801047c0 @ 0x801047c0 ======= */
// decompile failed: Exception while decompiling 801047c0: process: timeout


/* ======= FUN_801048cc @ 0x801048cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801048cc(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_80109230(0,0);
  iVar3 = (uint)(byte)(&LAB_80117888)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_801049bc @ 0x801049bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801049bc(void)

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
  iVar4 = (uint)(byte)(&LAB_80117888)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + 0x7f88);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104b88 @ 0x80104b88 ======= */
// decompile failed: Exception while decompiling 80104b88: process: timeout


/* ======= FUN_80104cb0 @ 0x80104cb0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104cb0(void)

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



/* ======= FUN_80104e50 @ 0x80104e50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104e50(void)

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
         *(undefined2 *)(&LAB_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_80117c54_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&LAB_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80105898(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80105120 @ 0x80105120 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105120(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee836c))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801068cc();
  }
  else {
    FUN_80106734();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105c9c:
    FUN_80109230(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_80109230(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105c9c;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106d34();
  return;
}



/* ======= FUN_80105210 @ 0x80105210 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105210(void)

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
        if (4 < DAT_800aca4e) {
          uVar4 = func_0x8001af20();
          *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_80117bf4)[uVar4 & 0x1f];
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
          goto LAB_80105ec0;
        }
        uVar4 = func_0x8001af20();
        *(undefined1 *)(_DAT_800ac784 + 0x1de) =
             *(undefined1 *)((int)&PTR_LAB_80117c04 + (uVar4 & 0x1f));
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
      goto LAB_801056c0;
    }
    if (bVar2 != 0) goto LAB_80105ec0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
    (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
LAB_80105ec0:
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
LAB_801056c0:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_801056dc @ 0x801056dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801056dc(void)

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
        FUN_80106100(0);
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
      FUN_80109230(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_80106100(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_80117f98)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80117fa0);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
    FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
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
                      (uint)(byte)(&LAB_80117888)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&LAB_80118270);
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
      FUN_80109170(0,1);
      return;
    }
    FUN_80109230(0,0);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80105898 @ 0x80105898 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105898(int param_1)

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
  FUN_80109230(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_80105924 @ 0x80105924 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105924(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105aa0:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105c9c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
        FUN_80109230(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_8010649c;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010649c;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105aa0;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010649c;
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
      goto LAB_80105c9c;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105c9c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117f88);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    puVar5 = (undefined1 *)0x0;
LAB_8010649c:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106d34();
    return;
  }
LAB_80105c9c:
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80105cb8 @ 0x80105cb8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105cb8(void)

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
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80105f34 @ 0x80105f34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105f34(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801068b8();
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
      FUN_801068b8();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80117fa0);
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
  FUN_801068b8();
  return;
}



/* ======= FUN_801060cc @ 0x801060cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801060cc(undefined4 param_1,undefined4 param_2,int param_3)

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
      goto LAB_80106a28;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106a28;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80117fa0);
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
LAB_80106a28:
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
    FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
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
    FUN_80109230(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80118270);
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
    FUN_80109230(0,0);
    return;
  }
  FUN_80109170(0,1);
  return;
}



/* ======= FUN_80106100 @ 0x80106100 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106100(int param_1,undefined4 param_2,int param_3)

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
    FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
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
    FUN_80109230(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_80118270);
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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80106238 @ 0x80106238 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106238(void)

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
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee8050))();
    return;
  }
  FUN_80107b44();
  *unaff_s3 = extraout_v1 | 0x4a;
  func_0x800453d0();
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
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
    FUN_80109230(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_80118270);
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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_801062ac @ 0x801062ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801062ac(void)

{
  byte bVar1;
  char cVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80106d34();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80118270);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109230(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80106d34();
  return;
}



/* ======= FUN_80106570 @ 0x80106570 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106570(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80107094();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_80109230(0,1);
      FUN_80107b30();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    puVar5 = (uint *)((uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_80118270);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_80118270);
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
      FUN_80107000(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80107094();
  return;
}



/* ======= FUN_80106734 @ 0x80106734 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106734(undefined4 param_1)

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
    FUN_80107000(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80107094();
  return;
}



/* ======= FUN_801068b8 @ 0x801068b8 ======= */

void FUN_801068b8(void)

{
  return;
}



/* ======= FUN_801068cc @ 0x801068cc ======= */

void FUN_801068cc(void)

{
  return;
}



/* ======= FUN_801068d8 @ 0x801068d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801068d8(void)

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
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    puVar7 = (uint *)((uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
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
    FUN_80109230(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80118270);
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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80106cc8 @ 0x80106cc8 ======= */
// decompile failed: Exception while decompiling 80106cc8: process: timeout


/* ======= FUN_80106d34 @ 0x80106d34 ======= */
// decompile failed: Exception while decompiling 80106d34: process: timeout


/* ======= FUN_80107000 @ 0x80107000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107000(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109230(0,1);
    FUN_80107b30();
    return;
  }
  FUN_80109230(0,0);
  FUN_80107b30();
  return;
}



/* ======= FUN_80107094 @ 0x80107094 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107094(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80109230(0,1);
  FUN_80107b30();
  return;
}



/* ======= FUN_80107344 @ 0x80107344 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107344(void)

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
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80107d60;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80118270)
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
      goto LAB_80107d60;
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
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
LAB_80107d60:
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
    FUN_80109230(0,0);
    return;
  }
  FUN_80109170(0,1);
  return;
}



/* ======= FUN_80107574 @ 0x80107574 ======= */
// decompile failed: Exception while decompiling 80107574: process: timeout


/* ======= FUN_80107b30 @ 0x80107b30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107b30(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80118270);
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80118270);
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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80107b44 @ 0x80107b44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107b44(void)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80118270);
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80118270);
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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80108150 @ 0x80108150 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108150(void)

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
      goto LAB_80108b84;
    }
    unaff_s0 = 1;
    if (bVar1 != 0) goto LAB_80108b84;
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
LAB_80108b84:
  func_0x80022da0(iVar4,unaff_s0 + 0x18,iVar4);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (uStackX_c - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_80108398 @ 0x80108398 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108398(uint *param_1)

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



/* ======= FUN_80108408 @ 0x80108408 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108408(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 == 1) {
    cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
    FUN_80108ff8();
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      FUN_80108ff8();
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_801178a8)
                                         [(ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8] * 0xac +
                             _DAT_800ac784[0x62] + 0x40);
    return;
  }
  if (bVar1 != 2) {
    FUN_80108ff8();
    return;
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109230(0,0);
  }
  return;
}



/* ======= FUN_80108830 @ 0x80108830 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108830(void)

{
  (**(code **)(&LAB_80118290 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80108878 @ 0x80108878 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108878(void)

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
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}



/* ======= FUN_80108988 @ 0x80108988 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108988(void)

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



/* ======= FUN_80108a30 @ 0x80108a30 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108a30(undefined4 param_1,int param_2)

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



/* ======= FUN_80108b04 @ 0x80108b04 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b04(undefined4 param_1,int param_2)

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



/* ======= FUN_80108b98 @ 0x80108b98 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b98(undefined4 param_1,int param_2)

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



/* ======= FUN_80108be8 @ 0x80108be8 ======= */
// decompile failed: Exception while decompiling 80108be8: process: timeout


/* ======= FUN_80108ff8 @ 0x80108ff8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108ff8(int param_1)

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
    FUN_80109c0c();
    return;
  }
  return;
}



/* ======= FUN_80109170 @ 0x80109170 ======= */

void FUN_80109170(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_80109230 @ 0x80109230 ======= */

void FUN_80109230(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_80109304 @ 0x80109304 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109304(void)

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



/* ======= FUN_8010942c @ 0x8010942c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010942c(void)

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



/* ======= FUN_801094e0 @ 0x801094e0 ======= */
// decompile failed: Exception while decompiling 801094e0: process: timeout


/* ======= FUN_80109860 @ 0x80109860 ======= */

void FUN_80109860(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_8010989c @ 0x8010989c ======= */

void FUN_8010989c(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_801098d8 @ 0x801098d8 ======= */

void FUN_801098d8(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ======= FUN_801098fc @ 0x801098fc ======= */

void FUN_801098fc(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ======= FUN_80109c0c @ 0x80109c0c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109c0c(void)

{
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&LAB_801182bc + (_DAT_800aca58 >> 8 & 0xff) * 4))();
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



/* ======= FUN_80109d8c @ 0x80109d8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d8c(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a74c();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a74c();
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,
                    &PTR_LAB_801182cc);
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,
                    &PTR_LAB_801182cc);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8010a74c();
  return;
}



/* ======= FUN_80109f5c @ 0x80109f5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109f5c(void)

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
    (*(code *)(&PTR_LAB_8011830c)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ======= FUN_8010a1c0 @ 0x8010a1c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a1c0(void)

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
  _DAT_800ac784[0x1e] = DAT_80117894;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_801178e8)[uVar3 & 7];
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
                    (uint)(byte)(&LAB_801178a8)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_801178a8_1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &local_38);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_801178a8_2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_801178a8_3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ac)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_801178d8);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ad)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178ae)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_801178d8);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_801178af)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_801178d8);
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



/* ======= FUN_8010a74c @ 0x8010a74c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a74c(void)

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



/* ======= FUN_8010a908 @ 0x8010a908 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a908(undefined4 param_1)

{
  bool bVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_801178a8)[(unaff_s0 & 0xff) * 8] * 0xac +
                               _DAT_800ac784[0x62] + 0x40);
      return;
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      puVar2 = _DAT_800ac784;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      *(undefined1 *)((int)puVar2 + 0x1d7) = 0x1f;
      func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),
                      0x10);
      func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010bf1c(0,0);
          }
        }
        else if (((ushort)_DAT_800ac784[0x76] & 0x40) == 0) {
          *(undefined2 *)(((ushort)_DAT_800ac784[0x76] & 0x40) + 0x1dc) = 0;
          *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x78) = 1;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
          _DAT_800ac784[0xe] = 0xfffffb50;
          *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
          func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
          uVar3 = func_0x8001af20();
          *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
               *(undefined2 *)
                ((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
          *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
          _DAT_800ac784[0x1e] = _DAT_80118bb0;
          func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                          *(ushort *)(_DAT_800ac784[0x1e] + 10) + 200,_DAT_800ac784 + 0x2c,0x808080)
          ;
          if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
            _DAT_800ac784[1] = 0x1000001;
            *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
            *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
            _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
            *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
          }
          iVar5 = 0xe1c;
          iVar6 = 0x15;
          do {
            iVar4 = iVar5 + _DAT_800ac784[0x62];
            iVar5 = iVar5 + -0xac;
            *(int *)(iVar4 + 0x2c) = (*(int *)(iVar4 + 0x2c) * 5) / 2;
            *(int *)(iVar4 + 0x30) = (*(int *)(iVar4 + 0x30) * 5) / 2;
            bVar1 = iVar6 != 0;
            *(int *)(iVar4 + 0x34) = (*(int *)(iVar4 + 0x34) * 5) / 2;
            iVar6 = iVar6 + -1;
          } while (bVar1);
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
  (**(code **)(&LAB_80118334 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010ad68 @ 0x8010ad68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ad68(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_LAB_80118368)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&LAB_801183ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010ae18 @ 0x8010ae18 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ae18(void)

{
  (**(code **)(&LAB_801183f0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011840c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010ae94 @ 0x8010ae94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ae94(void)

{
  (**(code **)(&LAB_80118428 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801183ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010af10 @ 0x8010af10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010af10(void)

{
  (**(code **)(&LAB_80118460 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011840c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010af8c @ 0x8010af8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010af8c(void)

{
  (**(code **)(&LAB_80118478 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011847c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b008 @ 0x8010b008 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b008(void)

{
  (**(code **)(&LAB_80118480 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118484 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b084 @ 0x8010b084 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b084(void)

{
  (**(code **)(&LAB_80118488 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118484 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b100 @ 0x8010b100 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b100(void)

{
  (**(code **)(&LAB_8011848c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118484 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b17c @ 0x8010b17c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b17c(void)

{
  (**(code **)(&LAB_80118490 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118494 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b1f8 @ 0x8010b1f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b1f8(void)

{
  (**(code **)(&LAB_80118498 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011849c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b27c @ 0x8010b27c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b27c(void)

{
  byte bVar1;
  undefined4 *in_at;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  int unaff_s0;
  
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
      else if (bVar1 != 3) goto LAB_8010bccc;
      if (*(char *)(_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x474;
        *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar4 == 0) {
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
    if (bVar1 != 0) goto LAB_8010bccc;
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
LAB_8010bccc:
  (*(code *)*in_at)();
  func_0x8002b498(_DAT_800ac784);
  uVar3 = func_0x8002aec4(unaff_s0 + 0x14,_DAT_800ac784);
  *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar3;
  func_0x8002b544();
  uVar3 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4
                         );
  *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar3;
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_8010b4e4 @ 0x8010b4e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b4e4(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
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
        FUN_8010bf1c(0,0);
      }
    }
    else if (((ushort)_DAT_800ac784[0x76] & 0x40) == 0) {
      *(undefined2 *)(((ushort)_DAT_800ac784[0x76] & 0x40) + 0x1dc) = 0;
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
           *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      _DAT_800ac784[0x1e] = _DAT_80118bb0;
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
  }
  return;
}



/* ======= FUN_8010b614 @ 0x8010b614 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b614(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    if (*(char *)((int)_DAT_800ac784 + 5) == '\x02') {
      FUN_801068cc();
    }
    else {
      FUN_80106734();
    }
    uVar2 = func_0x8001af20();
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
         *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    _DAT_800ac784[0x1e] = _DAT_80118bb0;
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
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)((int)_DAT_800ac784 + 5) * 0x20 + -0x7fee7b60))();
  return;
}



/* ======= FUN_8010b6a8 @ 0x8010b6a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b6a8(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    FUN_80107b44();
    func_0x8001af5c(0);
    if ((*(byte *)(_DAT_800ac784 + 9) & 1) != 0) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x1000001;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
      *(uint *)(_DAT_800ac784 + 0x38) = (uint)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = *(undefined2 *)(_DAT_800ac784 + 0x38);
    }
    iVar3 = 0xe1c;
    iVar4 = 0x15;
    do {
      iVar2 = iVar3 + *(int *)(_DAT_800ac784 + 0x188);
      iVar3 = iVar3 + -0xac;
      *(int *)(iVar2 + 0x2c) = (*(int *)(iVar2 + 0x2c) * 5) / 2;
      *(int *)(iVar2 + 0x30) = (*(int *)(iVar2 + 0x30) * 5) / 2;
      bVar1 = iVar4 != 0;
      *(int *)(iVar2 + 0x34) = (*(int *)(iVar2 + 0x34) * 5) / 2;
      iVar4 = iVar4 + -1;
    } while (bVar1);
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee78c0))();
  return;
}



/* ======= FUN_8010b71c @ 0x8010b71c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b71c(undefined4 param_1,int param_2)

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



/* ======= FUN_8010b880 @ 0x8010b880 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b880(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_80118a20 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
  }
  return;
}



/* ======= FUN_8010b8fc @ 0x8010b8fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8fc(void)

{
  (*(code *)(&PTR_DAT_80118a40)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&LAB_80118a80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010ba78 @ 0x8010ba78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ba78(void)

{
  (**(code **)(&LAB_80118ac0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010bad8 @ 0x8010bad8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bad8(void)

{
  (**(code **)(&LAB_80118b18 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010bb38 @ 0x8010bb38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb38(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee7490))();
  return;
}



/* ======= FUN_8010bc48 @ 0x8010bc48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc48(void)

{
  undefined2 uVar1;
  
  func_0x800279c8(0x280010,0,&LAB_80100194,*(undefined4 *)(_DAT_800ac784 + 4));
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee7438))();
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



/* ======= FUN_8010bd6c @ 0x8010bd6c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bd6c(void)

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
       *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = _DAT_80118bb0;
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



/* ======= FUN_8010bf1c @ 0x8010bf1c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf1c(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  int in_v1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c(param_1,param_2,*(ushort *)(in_v1 + 6) + 100,*(ushort *)(in_v1 + 10) + 200);
  if ((*(byte *)(_DAT_800ac784 + 9) & 1) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1000001;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(uint *)(_DAT_800ac784 + 0x38) = (uint)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  }
  iVar3 = 0xe1c;
  iVar4 = 0x15;
  do {
    iVar2 = iVar3 + *(int *)(_DAT_800ac784 + 0x188);
    iVar3 = iVar3 + -0xac;
    *(int *)(iVar2 + 0x2c) = (*(int *)(iVar2 + 0x2c) * 5) / 2;
    *(int *)(iVar2 + 0x30) = (*(int *)(iVar2 + 0x30) * 5) / 2;
    bVar1 = iVar4 != 0;
    *(int *)(iVar2 + 0x34) = (*(int *)(iVar2 + 0x34) * 5) / 2;
    iVar4 = iVar4 + -1;
  } while (bVar1);
  return;
}



/* ======= FUN_8010c060 @ 0x8010c060 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c060(void)

{
  char cVar1;
  undefined2 uVar2;
  short sVar3;
  short *unaff_s0;
  
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
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee7418))();
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee73d8))();
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
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    sVar3 = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    *(short *)((int)_DAT_800ac784 + 0x1de) = sVar3;
    DAT_800aca58 = (undefined1)sVar3;
    cVar1 = func_0x8001a780(unaff_s0 + -0x4d);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    if (*unaff_s0 < 0) {
      DAT_800aca58 = 3;
      DAT_800aca59 = '\0';
    }
    DAT_800acae7 = DAT_800acae7 | 1;
    if (DAT_800acae7 != 0) {
      func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0xa54,
                      0x80118bb8);
      FUN_8010d390();
      return;
    }
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010c330 @ 0x8010c330 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c330(void)

{
  byte bVar1;
  byte bVar2;
  undefined1 uVar3;
  ushort uVar4;
  short sVar5;
  uint uVar6;
  uint uVar7;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  bVar2 = bVar1 < 2;
  if (bVar1 != 1) {
    if (!(bool)bVar2) {
      bVar2 = 0;
      if (bVar1 == 2) {
        sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
        if (sVar5 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      goto code_r0x8010cc5c;
    }
    if (bVar1 != 0) goto code_r0x8010cc5c;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(byte *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + bVar2;
code_r0x8010cc5c:
  if ((bVar2 & 1) != 0) {
    uVar6 = func_0x8001af20();
    uVar7 = func_0x8001af20();
    if ((uVar6 & 1) * (uVar7 & 1) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
      return;
    }
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)
     ) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x82);
    sVar5 = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(short *)(_DAT_800ac784 + 0x1ba) = sVar5;
  }
  else {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) == 0) ||
       (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff)))
    {
      sVar5 = func_0x8001a9cc(&DAT_800aca88,1000);
      *(short *)(_DAT_800ac784 + 0x1da) = sVar5;
      if ((sVar5 == 0) ||
         ((uVar3 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
          (uVar3 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
        if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
           ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
          *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
        }
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar6 = _DAT_800ac784;
      sVar5 = *(short *)(_DAT_800ac784 + 0x1da);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar5;
    }
    else {
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      sVar5 = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    }
  }
  *(short *)(uVar6 + 0x6a) = sVar5 + 0x800;
  *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
  if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}



/* ======= FUN_8010c470 @ 0x8010c470 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c470(undefined4 param_1,int param_2)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    iVar5 = _DAT_800ac784;
    if (1 < bVar1) {
      iVar4 = 2;
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
        return;
      }
      goto LAB_8010cd60;
    }
    iVar4 = 1;
    if (bVar1 != 0) goto LAB_8010cd60;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  param_2 = *(int *)(_DAT_800ac784 + 0x16c);
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),param_2,0,0x200);
  iVar5 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar4;
LAB_8010cd60:
  if (iVar4 < 0x400) {
    *(char *)(param_2 + 0x9f) = (char)iVar5;
    *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    iVar5 = _DAT_800ac784;
    sVar3 = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  else {
    sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
    *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
    if ((sVar3 == 0) ||
       ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
        (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    iVar5 = _DAT_800ac784;
    sVar3 = *(short *)(_DAT_800ac784 + 0x1da);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar3;
  }
  *(short *)(iVar5 + 0x6a) = sVar3 + 0x800;
  *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
  if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}



/* ======= FUN_8010c570 @ 0x8010c570 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c570(void)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
    iVar3 = _DAT_800ac784;
    *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    *(undefined1 *)(iVar3 + 0x1e1) = 0;
    return;
  }
  if (DAT_800acae7 != '\0') {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 5000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) &&
       (uVar2 = 1, *(char *)(_DAT_800ac784 + 0x1e2) == '\0')) goto LAB_8010c668;
    if (DAT_800acae7 != '\0') {
      return;
    }
  }
  iVar3 = func_0x8001a804(6000,0x180,&DAT_800aca88);
  if (-1 < iVar3) {
    return;
  }
  uVar2 = 3;
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    return;
  }
LAB_8010c668:
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ======= FUN_8010c69c @ 0x8010c69c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c69c(void)

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



/* ======= FUN_8010c7bc @ 0x8010c7bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7bc(void)

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
          FUN_8010d390();
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
                        *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0x80118bb8);
        FUN_8010d390();
        return;
      }
      if (bVar2 != 3) {
        FUN_8010d390();
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
        FUN_8010d390();
        return;
      }
      goto LAB_8010cb88;
    }
    if (bVar2 != 0) {
      FUN_8010d390();
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
    FUN_8010d388();
    return;
  }
LAB_8010cb88:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010cba4 @ 0x8010cba4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cba4(void)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\x01') {
    if (((DAT_800acae7 == '\0') &&
        (iVar4 = func_0x8001a804(6000,0x180,&DAT_800aca88), uVar5 = _DAT_800ac784, iVar4 < 0)) &&
       (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto code_r0x8010d674;
    }
    if ((5000 < *(ushort *)(_DAT_800ac784 + 0x1d4)) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      uVar5 = func_0x8001af20();
      uVar6 = func_0x8001af20();
      if ((uVar5 & 1) * (uVar6 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        return;
      }
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar5 = (uint)*(byte *)(_DAT_800ac784 + 0x82);
      uVar6 = uVar5 * -0x708;
      *(short *)(_DAT_800ac784 + 0x1ba) = (short)uVar6;
      goto code_r0x8010d674;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) != 0) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar5 = _DAT_800ac784;
      uVar6 = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      goto code_r0x8010d674;
    }
  }
  sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
  *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
  if ((sVar3 == 0) ||
     ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
      (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  uVar5 = _DAT_800ac784;
  uVar6 = (uint)*(ushort *)(_DAT_800ac784 + 0x1da);
  *(ushort *)(_DAT_800ac784 + 0x9c) = *(ushort *)(_DAT_800ac784 + 0x1da);
code_r0x8010d674:
  *(short *)(uVar5 + 0x6a) = (short)uVar6 + 0x800;
  *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
  if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}



/* ======= FUN_8010ce84 @ 0x8010ce84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ce84(void)

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



/* ======= FUN_8010d068 @ 0x8010d068 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d068(undefined4 param_1,int param_2)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    if (1 < uVar4) {
      uVar2 = 4;
      if (uVar4 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 2;
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 100;
        return;
      }
      goto LAB_8010d9fc;
    }
    uVar2 = 1;
    if (uVar4 != 0) goto LAB_8010d9fc;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  param_2 = 0;
  if (3 < *(byte *)(_DAT_800ac784 + 0x95)) {
    iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffba,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    uVar4 = _DAT_800ac784;
    uVar2 = 2;
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_8010d9fc:
      if (uVar4 == uVar2) {
        if (*(char *)(_DAT_800ac784 + 7) == '\0') {
          *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x32;
          if (-0x4b0 < *(int *)(_DAT_800ac784 + 0x38)) {
            *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
            *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
          }
        }
        else {
          cVar1 = func_0x8001c0dc(800,400,0xfffffff6);
          *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
          *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
          iVar3 = (*(byte *)(_DAT_800ac784 + 0x82) + 1) * -0x708;
          if (*(int *)(_DAT_800ac784 + 0x38) < iVar3) {
            *(int *)(_DAT_800ac784 + 0x38) = iVar3;
            FUN_8010e3dc();
            return;
          }
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        func_0x800245d8(0);
        FUN_8010e510();
        return;
      }
      if (uVar4 != 3) {
        FUN_8010e510();
        return;
      }
      if (*(char *)(param_2 + 7) != '\0') {
        *(char *)(param_2 + 0x82) = *(char *)(param_2 + 0x82) + '\x01';
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        FUN_8010e510();
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
  return;
}



/* ======= FUN_8010d350 @ 0x8010d350 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d350(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  uVar4 = (ushort)bVar1;
  uVar3 = (ushort)(bVar1 < 2);
  if (bVar1 != 1) {
    iVar5 = _DAT_800ac784;
    if (bVar1 >= 2) {
      uVar3 = 2;
      if (bVar1 == 2) {
        sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
        if (sVar2 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      goto LAB_8010dc78;
    }
    if (bVar1 != 0) goto LAB_8010dc78;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  iVar5 = _DAT_800ac784;
  uVar4 = *(byte *)(_DAT_800ac784 + 6) + uVar3;
  *(char *)(_DAT_800ac784 + 6) = (char)uVar4;
LAB_8010dc78:
  *(ushort *)(iVar5 + 0x1ba) = (uVar3 + uVar4) * -8;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  FUN_8010e510();
  return;
}



/* ======= FUN_8010d388 @ 0x8010d388 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d388(int param_1)

{
  short sVar1;
  int in_v1;
  
  if (in_v1 != 2) {
    *(short *)(param_1 + 0x1ba) = ((short)in_v1 + 2) * -8;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    FUN_8010e510();
    return;
  }
  sVar1 = *(short *)(param_1 + 0x9c);
  *(short *)(param_1 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_8010d390 @ 0x8010d390 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d390(int param_1)

{
  short in_v0;
  short in_v1;
  
  *(short *)(param_1 + 0x1ba) = (in_v0 + in_v1) * -8;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  FUN_8010e510();
  return;
}



/* ======= FUN_8010d490 @ 0x8010d490 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d490(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
        return;
      }
      goto LAB_8010dd80;
    }
    if (uVar2 != 0) goto LAB_8010dd80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6) + iVar1;
  *(char *)(_DAT_800ac784 + 6) = (char)uVar2;
LAB_8010dd80:
  if (*(char *)(uVar2 + 0x1e0) == '\0') {
    (**(code **)((uint)*(byte *)(uVar2 + 5) * 4 + -0x7fee7394))();
    return;
  }
  FUN_8010e91c();
  FUN_8010e5c8();
  return;
}



/* ======= FUN_8010d598 @ 0x8010d598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d598(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),1);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)(_DAT_ffffc785 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
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



/* ======= FUN_8010d734 @ 0x8010d734 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d734(void)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  iVar3 = _DAT_800ac784;
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),2);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)(_DAT_ffffc785 + 7) = 0;
    uVar2 = func_0x8001a9cc(iVar3,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
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



/* ======= FUN_8010d9c0 @ 0x8010d9c0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d9c0(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  short sVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        FUN_8010e510();
        return;
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      sVar4 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
      *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
      if (sVar4 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        FUN_8010e510();
        return;
      }
    }
    else {
      if (bVar1 == 2) goto LAB_8010dae4;
      if (bVar1 != 3) {
        FUN_8010e510();
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        FUN_8010e510();
        return;
      }
      func_0x8001c0dc(0x898,800,3000);
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0xfb50;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  if (*(char *)(_DAT_800ac784 + 7) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
LAB_8010dae4:
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
      FUN_8010e3dc();
      return;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  FUN_8010e510();
  return;
}



/* ======= FUN_8010dd70 @ 0x8010dd70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dd70(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1e0) != '\0') {
    FUN_8010e91c();
    FUN_8010e5c8();
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee7394))();
  return;
}



/* ======= FUN_8010ddd8 @ 0x8010ddd8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_8010ddd8(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  
  bVar5 = *(byte *)(_DAT_800ac784 + 7);
  uVar4 = (uint)(bVar5 < 2);
  if (bVar5 != 1) {
    if (uVar4 == 0) {
      uVar4 = 2;
      if (bVar5 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e0) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 4) = 1;
          iVar2 = _DAT_800ac784;
          *(undefined1 *)(_DAT_800ac784 + 5) = 4;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          iVar3 = _DAT_800ac784;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          return CONCAT44(iVar2,iVar3);
        }
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        return CONCAT44(_DAT_800ac784,2);
      }
      goto LAB_8010e738;
    }
    if (bVar5 != 0) goto LAB_8010e738;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0x80118bb8);
  }
  uVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  bVar5 = *(char *)(_DAT_800ac784 + 7) + (char)uVar4;
  *(byte *)(_DAT_800ac784 + 7) = bVar5;
LAB_8010e738:
  *(undefined1 *)(uVar4 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(byte *)(_DAT_800ac784 + 0x93) = bVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  FUN_8011089c(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  bVar5 = 0;
  uVar4 = 0;
  do {
    bVar5 = bVar5 + 1;
    *(undefined4 *)(uVar4 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar4 = (uint)bVar5;
  } while (bVar5 < 8);
  _DAT_80119318 = *(uint **)(_DAT_800ac784 + 0x188);
  *_DAT_80119318 = *_DAT_80119318 & 0xfffffffe;
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  iVar2 = _DAT_800ac784;
  pbVar1 = (byte *)(_DAT_800ac784 + 9);
  *(byte *)(_DAT_800ac784 + 5) = *pbVar1;
  return CONCAT44(iVar2,(uint)*pbVar1);
}



/* ======= FUN_8010df48 @ 0x8010df48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010df48(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      FUN_8010e90c();
      return;
    }
    if (bVar1 != 0) {
      FUN_8010e90c();
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
  if (0x11 < *(byte *)(_DAT_800ac784 + 0x95)) {
    return;
  }
  func_0x800245d8((int)*(short *)(_DAT_800ac784 + 0x9c));
  FUN_8010e90c();
  return;
}



/* ======= FUN_8010e11c @ 0x8010e11c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e11c(void)

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



/* ======= FUN_8010e1e8 @ 0x8010e1e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e1e8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee733c))();
  return;
}



/* ======= FUN_8010e230 @ 0x8010e230 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e230(void)

{
  byte bVar1;
  int in_at;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(bVar1 < 2);
  if (bVar1 != 1) {
    if (uVar3 == 0) {
      uVar3 = 2;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8010ec1c;
    }
    if (bVar1 != 0) goto LAB_8010ec1c;
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
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x15) {
    return;
  }
  uVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffb0,
                          (int)*(short *)(_DAT_800ac784 + 0x1ba));
LAB_8010ec1c:
  (**(code **)(in_at + -0x7270 + uVar3))();
  return;
}



/* ======= FUN_8010e3dc @ 0x8010e3dc ======= */

void FUN_8010e3dc(void)

{
  int in_at;
  int iVar1;
  int in_v1;
  
  iVar1 = func_0x8001c1a4((int)*(short *)(in_v1 + 0x8c),0,0xffffffb0,(int)*(short *)(in_v1 + 0x1ba))
  ;
  (**(code **)(in_at + -0x7270 + iVar1))();
  return;
}



/* ======= FUN_8010e43c @ 0x8010e43c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e43c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee72e4))();
  return;
}



/* ======= FUN_8010e510 @ 0x8010e510 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e510(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint *puVar1;
  char cVar2;
  int iVar3;
  uint *in_v1;
  
  *in_v1 = *in_v1 | 0x40;
  iVar3 = _DAT_800ac784;
  puVar1 = (uint *)(_DAT_800ac784 + 0xec);
  *(uint *)(_DAT_800ac784 + 0xc4) = *(uint *)(_DAT_800ac784 + 0xc4) & 0xff000000 | param_3 | 0xff38;
  *(uint *)(iVar3 + 0xec) = *puVar1 & 0xff000000 | param_3 | 0xff38;
  *(short *)(iVar3 + 0xbc) = *(short *)(iVar3 + 0xbc) + 8;
  *(short *)(iVar3 + 0xbe) = *(short *)(iVar3 + 0xbe) + 8;
  cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}



/* ======= FUN_8010e5c8 @ 0x8010e5c8 ======= */

void FUN_8010e5c8(void)

{
  code *in_v0;
  
  (*in_v0)();
  return;
}



/* ======= FUN_8010e69c @ 0x8010e69c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e69c(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee72b0))();
  }
  return;
}



/* ======= FUN_8010e71c @ 0x8010e71c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e71c(void)

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
  _DAT_80119318 = *(uint **)(_DAT_800ac784 + 0x188);
  *_DAT_80119318 = *_DAT_80119318 & 0xfffffffe;
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = *(undefined1 *)(_DAT_800ac784 + 9);
  return;
}



/* ======= FUN_8010e90c @ 0x8010e90c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e90c(void)

{
  undefined4 in_v0;
  int in_v1;
  
  *(undefined4 *)(in_v1 + 0x34) = in_v0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xe1;
  return;
}



/* ======= FUN_8010e91c @ 0x8010e91c ======= */

void FUN_8010e91c(void)

{
  undefined2 in_v0;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x8c) = in_v0;
  return;
}



/* ======= FUN_8010e930 @ 0x8010e930 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e930(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7278))();
  return;
}



/* ======= FUN_8010e978 @ 0x8010e978 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e978(void)

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



/* ======= FUN_8010eac0 @ 0x8010eac0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eac0(void)

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



/* ======= FUN_8010ebfc @ 0x8010ebfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ebfc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7270))();
  return;
}



/* ======= FUN_8010ec44 @ 0x8010ec44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec44(void)

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



/* ======= FUN_8010ee40 @ 0x8010ee40 ======= */

void FUN_8010ee40(void)

{
  return;
}



/* ======= FUN_8010eeb0 @ 0x8010eeb0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eeb0(void)

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



/* ======= FUN_8010f110 @ 0x8010f110 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f110(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7260))();
  return;
}



/* ======= FUN_8010f1bc @ 0x8010f1bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f1bc(void)

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



/* ======= FUN_8010f428 @ 0x8010f428 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f428(void)

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



/* ======= FUN_8010f624 @ 0x8010f624 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f624(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7250))();
  return;
}



/* ======= FUN_8010f66c @ 0x8010f66c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f66c(void)

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



/* ======= FUN_8010f8d8 @ 0x8010f8d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f8d8(void)

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



/* ======= FUN_8010fb38 @ 0x8010fb38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fb38(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7240))();
  return;
}



/* ======= FUN_8010fbe4 @ 0x8010fbe4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fbe4(void)

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



/* ======= FUN_8010fe50 @ 0x8010fe50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe50(void)

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



/* ======= FUN_80110104 @ 0x80110104 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110104(void)

{
  int iVar1;
  
  iVar1 = func_0x8001af20();
  _DAT_8011931c = _DAT_8011931c + -0x180 + iVar1 * 3;
  iVar1 = func_0x8001af20();
  _DAT_80119320 = _DAT_80119320 + -0x200 + iVar1 * 4;
  iVar1 = func_0x8001af20();
  _DAT_80119324 = _DAT_80119324 + -0x100 + iVar1 * 2;
  return;
}



/* ======= FUN_80110188 @ 0x80110188 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110188(uint param_1,short param_2,uint param_3)

{
  _DAT_80119318 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80019700((param_3 & 0xff) << 0x10 | param_1 & 0xffff | 0xf000000,(int)param_2,
                  _DAT_80119318 + 0x40,&DAT_8011931c);
  return;
}



/* ======= FUN_801101e4 @ 0x801101e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801101e4(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)(byte)_DAT_800ac784[1] * 4 + -0x7fee71dc))();
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



/* ======= FUN_8011036c @ 0x8011036c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011036c(void)

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
  _DAT_800ac784[0x1e] = _DAT_80118e04;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,0x514,0x578,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0xccc;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1d6) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_80110650 @ 0x80110650 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110650(void)

{
  uint *puVar1;
  undefined2 uVar2;
  short sVar3;
  undefined4 uVar4;
  
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
  if (*(char *)((int)_DAT_800ac784 + 5) == '\0') {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 9) * 4 + -0x7fee71bc))();
  }
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 + -0x7fee719c))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((*(byte *)((int)_DAT_800ac784 + 9) < 2) || (*(byte *)((int)_DAT_800ac784 + 9) == 5)) {
    func_0x80012aa4(4000);
  }
  puVar1 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar3 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar3;
    *(ushort *)((int)puVar1 + 0x6a) =
         *(short *)((int)puVar1 + 0x6a) + ((ushort)puVar1[0x74] & 1) * sVar3;
    FUN_801118a8();
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x3c;
    uVar4 = 0;
    if (*(char *)((int)_DAT_800ac784 + 9) == '\x02') {
      uVar4 = 0x800;
    }
    func_0x800245d8(uVar4);
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_80110840 @ 0x80110840 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110840(void)

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



/* ======= FUN_8011089c @ 0x8011089c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011089c(void)

{
  char cVar1;
  ushort uVar2;
  int in_v0;
  int iVar3;
  uint uVar4;
  
  if (in_v0 == 0) {
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



/* ======= FUN_801108cc @ 0x801108cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801108cc(void)

{
  ushort uVar1;
  uint in_v0;
  int iVar2;
  uint uVar3;
  int in_v1;
  
  if ((in_v0 & 1) != 0) {
    *(undefined1 *)(in_v1 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 6;
  }
  if (((*(short *)(_DAT_800ac784 + 0x1d4) < 7000) &&
      (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar2 == 0)) &&
     (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 6) = (uVar1 & 1) + 6;
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar3 = func_0x8001af20(), (uVar3 & 1) == 0)) {
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



/* ======= FUN_80110988 @ 0x80110988 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110988(void)

{
  int in_v0;
  uint uVar1;
  int in_v1;
  
  if ((in_v1 == in_v0) && (uVar1 = func_0x8001af20(), (uVar1 & 1) == 0)) {
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



/* ======= FUN_80110a40 @ 0x80110a40 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a40(void)

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



/* ======= FUN_80110c88 @ 0x80110c88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110c88(int param_1)

{
  int iVar1;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 3000) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
  }
  else {
    if ((_DAT_800aca58 != 0x701) || (DAT_800aca5a != '\x02')) {
      if (5000 < *(short *)(_DAT_800ac784 + 0x1d4)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined2 *)(_DAT_800ac784 + 6) = 2;
        if ((5000 < *(short *)(_DAT_800ac784 + 0x1d4)) &&
           (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar1 == 0)) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 1;
          *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
        }
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
  }
  _DAT_000000a4 = (short)param_1 + -1;
  if (param_1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_80110de4 @ 0x80110de4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110de4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 9) * 4 + -0x7fee714c))();
  return;
}



/* ======= FUN_80110e2c @ 0x80110e2c ======= */
// decompile failed: Exception while decompiling 80110e2c: process: timeout


/* ======= FUN_80110f68 @ 0x80110f68 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f68(void)

{
  uint uVar1;
  
  uVar1 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (uVar1 != 1) {
    *(undefined2 *)(uVar1 + 0x1be) = 1;
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}



/* ======= FUN_80110ffc @ 0x80110ffc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110ffc(void)

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



/* ======= FUN_801110a8 @ 0x801110a8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110a8(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  int extraout_v1;
  undefined8 uVar7;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if ((((bVar1 & 3) != 0) && (*(byte *)(_DAT_800ac784 + 9) < 2)) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      FUN_80111c48();
      return;
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
      uVar3 = (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      uVar6 = (uint)uVar3;
      *(ushort *)(_DAT_800ac784 + 0x1ea) = **(short **)(_DAT_800ac784 + 0x1b4) + uVar3;
      goto LAB_80111c4c;
    }
  }
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar6 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) * (ushort)*(byte *)(_DAT_800ac784 + 6) + 0x20
    ;
    if (*(byte *)(_DAT_800ac784 + 6) >> 1 == 0) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x50;
    }
  }
  else if (uVar6 != 1) {
LAB_80111c4c:
    *(undefined1 *)(*(int *)(uVar6 - 0x387c) + 0x94) = 0xb;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) + 10;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar4 = func_0x8001a9cc(&DAT_800aca88,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar4;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0);
    sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + -1;
    if ((sVar4 == 0) || (iVar5 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar5 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
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
      uVar6 = func_0x8001af20();
      func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar6 & 1)),0x32);
      *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
      iVar5 = _DAT_800ac784;
      func_0x8001af20();
      if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
        FUN_80112f50();
        return;
      }
      *(undefined2 *)(iVar5 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 200;
      cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
      iVar5 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
      uVar7 = func_0x800245d8();
      *(int *)(iVar5 + 0x38) = (int)((ulonglong)uVar7 >> 0x20) + (int)uVar7;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
        *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      }
      return;
    }
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(byte *)(_DAT_800ac784 + 0x95) != 0) && (*(byte *)(_DAT_800ac784 + 0x95) < 9)) {
    FUN_80111bc4(1,(int)*(short *)(_DAT_800ac784 + 0x166));
    return;
  }
  FUN_8011388c(0,(int)*(short *)(_DAT_800ac784 + 0x166));
  *(ushort *)(_DAT_800ac784 + 0x8c) = (ushort)*(byte *)(_DAT_800ac784 + 0x8c);
  sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + -1;
  if (sVar4 == 0) {
    *(undefined2 *)(_DAT_800ac784 + 4) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    if (*(byte *)(_DAT_800ac784 + 9) - 3 < 2) {
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xd;
    }
  }
  return;
}



/* ======= FUN_8011145c @ 0x8011145c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011145c(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  int extraout_v1;
  int iVar4;
  undefined8 uVar5;
  
  cVar2 = *(char *)(_DAT_800ac784 + 7);
  if (cVar2 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = uVar3 & 0x3f;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x18;
    if ((*(ushort *)(_DAT_800ac784 + 0x9c) & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffe8;
    }
  }
  else if (cVar2 != '\x01') {
    DAT_00000096 = 0;
    *(char *)(_DAT_800ac784 + 0x8f) = cVar2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800245d8(0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
      *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 200;
      cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
      iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
      uVar5 = func_0x800245d8();
      *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar5 >> 0x20) + (int)uVar5;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
        *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      }
      return;
    }
    FUN_80112f50();
    return;
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e6);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) &&
     (iVar4 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6)), iVar4 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    FUN_80111de4();
    return;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}



/* ======= FUN_80111604 @ 0x80111604 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111604(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x80;
  FUN_80111e40();
  return;
}



/* ======= FUN_80111640 @ 0x80111640 ======= */
// decompile failed: Exception while decompiling 80111640: process: timeout


/* ======= FUN_80111804 @ 0x80111804 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111804(void)

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



/* ======= FUN_8011187c @ 0x8011187c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011187c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x20;
  FUN_80111e40();
  return;
}



/* ======= FUN_801118a8 @ 0x801118a8 ======= */

void FUN_801118a8(void)

{
  return;
}



/* ======= FUN_801118b8 @ 0x801118b8 ======= */
// decompile failed: Exception while decompiling 801118b8: process: timeout


/* ======= FUN_80111bc4 @ 0x80111bc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111bc4(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
  undefined8 uVar5;
  
  if (*(short *)(_DAT_800ac784 + 0x1e0) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  iVar4 = _DAT_800ac784;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    *(undefined1 *)(*(int *)(iVar4 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      FUN_80112f50();
      return;
    }
    *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 200;
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
    uVar5 = func_0x800245d8();
    *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar5 >> 0x20) + (int)uVar5;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    return;
  }
  return;
}



/* ======= FUN_80111c48 @ 0x80111c48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c48(void)

{
  char cVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  int extraout_v1;
  undefined8 uVar6;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar2 & 0x1f) + 10;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar3;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
  func_0x800245d8(0);
  sVar3 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar3 + -1;
  if ((sVar3 == 0) || (iVar4 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar4 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    func_0x800453d0(2);
    DAT_800aca58 = 2;
    cVar1 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar1 + 2;
    DAT_800aca5a = 0;
    DAT_800acae7 = DAT_800acae7 | 1;
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 1;
    }
    uVar5 = func_0x8001af20();
    func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar5 & 1)),0x32);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      FUN_80112f50();
      return;
    }
    *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
    uVar6 = func_0x800245d8();
    *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar6 >> 0x20) + (int)uVar6;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    return;
  }
  return;
}



/* ======= FUN_80111de4 @ 0x80111de4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111de4(void)

{
  char cVar1;
  ushort uVar2;
  int in_v0;
  int extraout_v1;
  int iVar3;
  undefined8 uVar4;
  
  *(undefined1 *)(in_v0 + 7) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800245d8(0);
  *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
  iVar3 = _DAT_800ac784;
  func_0x8001af20();
  if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
    FUN_80112f50();
    return;
  }
  *(undefined2 *)(iVar3 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  iVar3 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
  uVar4 = func_0x800245d8();
  *(int *)(iVar3 + 0x38) = (int)((ulonglong)uVar4 >> 0x20) + (int)uVar4;
  if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}



/* ======= FUN_80111e40 @ 0x80111e40 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e40(void)

{
  char cVar1;
  ushort uVar2;
  int extraout_v1;
  int iVar3;
  undefined8 uVar4;
  
  cVar1 = func_0x8001f314();
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800245d8(0);
  *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
  iVar3 = _DAT_800ac784;
  func_0x8001af20();
  if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
    FUN_80112f50();
    return;
  }
  *(undefined2 *)(iVar3 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  iVar3 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
  uVar4 = func_0x800245d8();
  *(int *)(iVar3 + 0x38) = (int)((ulonglong)uVar4 >> 0x20) + (int)uVar4;
  if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}



/* ======= FUN_80111ed4 @ 0x80111ed4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111ed4(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined2 *)(_DAT_800ac784 + 4) = 1;
        *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
        *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xf;
        return;
      }
      goto code_r0x801127e4;
    }
    if (uVar2 != 0) goto code_r0x801127e4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    func_0x80019700(0xd002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118ee8);
    func_0x800453d0(4);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
code_r0x801127e4:
  *(char *)(uVar2 + 6) = (char)((*(ushort *)(uVar2 + 0x1d0) & 1) << 2);
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
  return;
}



/* ======= FUN_80111ff0 @ 0x80111ff0 ======= */
// decompile failed: Exception while decompiling 80111ff0: process: timeout


/* ======= FUN_801122f8 @ 0x801122f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801122f8(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80112380:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar2 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    FUN_80112e48();
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      FUN_80112e48();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    goto LAB_80112380;
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
    FUN_80112e48();
    return;
  }
  if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) {
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x15') {
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0xa8c;
      *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + 0x708;
    }
    if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) goto LAB_80112500;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x1a') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x904;
    *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
  }
LAB_80112500:
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



/* ======= FUN_80112658 @ 0x80112658 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112658(void)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar3 != 1) {
    iVar4 = _DAT_800ac784;
    if (1 < uVar3) {
      uVar5 = CONCAT44(uVar3,2);
      if (uVar3 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
        return;
      }
      goto LAB_80113018;
    }
    uVar5 = CONCAT44(uVar3,7);
    if (uVar3 != 0) goto LAB_80113018;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      FUN_80112f50();
      return;
    }
    *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
  uVar5 = func_0x800245d8();
LAB_80113018:
  *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar5 >> 0x20) + (int)uVar5;
  if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}



/* ======= FUN_8011282c @ 0x8011282c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011282c(void)

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



/* ======= FUN_801128a4 @ 0x801128a4 ======= */
// decompile failed: Exception while decompiling 801128a4: process: timeout


/* ======= FUN_80112e48 @ 0x80112e48 ======= */

void FUN_80112e48(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int in_v0;
  int extraout_v1;
  
  func_0x8001f314(param_1,*(undefined4 *)(in_v0 + 0x16c),param_3,0x200);
  *(undefined1 *)(extraout_v1 + 6) = 10;
  return;
}



/* ======= FUN_80112f34 @ 0x80112f34 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f34(void)

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



/* ======= FUN_80112f50 @ 0x80112f50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f50(void)

{
  int in_v0;
  uint uVar1;
  undefined1 unaff_s0;
  
  *(undefined1 *)(in_v0 + 0x94) = unaff_s0;
  if (*(char *)(_DAT_800ac784 + 9) == '\x03') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = (*(int *)(_DAT_800ac784 + 0x38) + -100) - (uVar1 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) < -9000) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = unaff_s0;
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



/* ======= FUN_8011308c @ 0x8011308c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011308c(uint param_1,int param_2,undefined4 param_3,undefined1 param_4)

{
  short sVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  undefined1 *puVar6;
  int in_t0;
  undefined2 in_t1;
  undefined1 in_t2;
  byte bVar7;
  int iVar8;
  int local_48;
  int local_44;
  int local_40;
  undefined1 auStack_38 [20];
  int local_24 [3];
  
  iVar8 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  puVar6 = auStack_38;
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar8 + 0x18);
  uVar3 = SUB42(puVar6,0);
  if (param_2 != 0) {
    local_48 = param_2;
    local_44 = param_2;
    local_40 = param_2;
    func_0x80065ff0(auStack_38,&local_48);
  }
  iVar4 = _DAT_800ac784;
  iVar8 = iVar8 + (param_1 & 0xff) * 0x560 + 0x2b0;
  bVar7 = 2;
  if ((*(uint *)(iVar8 + -0xac) & 1) != 0) {
    do {
      func_0x80022da0(auStack_38,iVar8 + (uint)bVar7 * -0xac + 0xc4,auStack_38);
      bVar7 = bVar7 - 1;
    } while (bVar7 != 0);
    local_24[1] = 0;
    local_24[0] = local_24[0] - *(int *)(iVar8 + 0x54);
    local_24[2] = local_24[2] - *(int *)(iVar8 + 0x5c);
    func_0x800662e8(local_24,&local_48);
    uVar3 = func_0x80065f60(local_48 + local_40);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar3;
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  do {
    iVar8 = *(short *)(iVar4 + 0x1e0) * 0x158 + in_t0;
    uVar5 = *(uint *)(iVar8 + 0xac);
    if ((uVar5 & 0x41) == 1) {
      *(uint *)(iVar8 + 0xac) = uVar5 | 0x1062;
      iVar8 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0;
      *(uint *)(iVar8 + 0x158) = *(uint *)(iVar8 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x121) = in_t2;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x12e) = in_t1;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x122) = param_4;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe6) = uVar3;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1cd) = in_t2;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1da) = in_t1;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1ce) = param_4;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x192) = uVar3;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
    iVar4 = _DAT_800ac784;
  } while (sVar1 != 1);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ======= FUN_8011320c @ 0x8011320c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011320c(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7108))();
  return;
}



/* ======= FUN_801132a4 @ 0x801132a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801132a4(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined4 unaff_s0;
  int unaff_s1;
  int iStackX_c;
  int in_stack_00000014;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(uVar4 < 2);
  if (uVar4 != 1) {
    uVar6 = _DAT_800ac784;
    if (uVar3 == 0) {
      uVar3 = 1;
      if (uVar4 == 2) {
        *(undefined2 *)(_DAT_800ac784 + 4) = 1;
        *(byte *)(_DAT_800ac784 + 6) = (*(byte *)(_DAT_800ac784 + 9) & 1) + 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
        *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 1;
          *(undefined1 *)(_DAT_800ac784 + 6) = 10;
        }
        return;
      }
      goto LAB_80113f2c;
    }
    if (uVar4 != 0) goto LAB_80113f2c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
    func_0x800453d0(2);
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) {
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e0) = uVar2 & 7;
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 8;
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
      sVar1 = *(short *)(_DAT_800ac784 + 0x1e4);
      *(short *)(_DAT_800ac784 + 0x1e4) = sVar1 + -1;
      while (sVar1 != 0) {
        if (*(char *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) ==
            *(char *)(*(short *)(_DAT_800ac784 + 0x1e4) + -0x7fee70d0)) {
          *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 1;
        }
        sVar1 = *(short *)(_DAT_800ac784 + 0x1e4);
        *(short *)(_DAT_800ac784 + 0x1e4) = sVar1 + -1;
      }
      if (*(short *)(_DAT_800ac784 + 0x1e2) == 0) {
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1de) + -0x7fee70d0) =
             *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8);
        puVar5 = (uint *)((uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                         *(int *)(_DAT_800ac784 + 0x188));
        puVar5[0x1a] = 0x207f60;
        *(undefined2 *)(puVar5 + 0x25) = 0;
        *(undefined2 *)((int)puVar5 + 0x96) = 0xffce;
        *(undefined2 *)(puVar5 + 0x26) = 0;
        *(undefined2 *)((int)puVar5 + 0x9a) = 3;
        *(undefined2 *)(puVar5 + 0x27) = 0;
        *(undefined2 *)((int)puVar5 + 0x9e) = 0;
        *puVar5 = *puVar5 | 0x4a;
        unaff_s0 = 0x80118f18;
        iVar8 = (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                *(int *)(_DAT_800ac784 + 0x188);
        *(undefined4 *)(iVar8 + 0x114) = 0x207f60;
        *(undefined2 *)(iVar8 + 0x140) = 0;
        *(undefined2 *)(iVar8 + 0x142) = 0xffce;
        *(undefined2 *)(iVar8 + 0x144) = 0;
        *(undefined2 *)(iVar8 + 0x146) = 3;
        *(undefined2 *)(iVar8 + 0x148) = 0;
        *(undefined2 *)(iVar8 + 0x14a) = 0;
        *(uint *)(iVar8 + 0xac) = *(uint *)(iVar8 + 0xac) | 0x4a;
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                        0x198,0x80118f18);
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                        0x198,0x80118f18);
        func_0x800453d0(5);
        *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
      }
    }
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar6 = _DAT_800ac784;
  uVar4 = *(byte *)(_DAT_800ac784 + 7) + uVar3;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar4;
LAB_80113f2c:
  *(undefined2 *)(uVar3 + 0xd62) = 0;
  *(uint *)(uVar3 + 0xcc4) = uVar4 | 0x4a;
  func_0x80019700(uVar6,(int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,unaff_s0);
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xd04,unaff_s0);
  uVar3 = _DAT_800ac784;
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
  sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
  iVar8 = *(int *)(uVar3 + 0x188);
  *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  while (sVar1 != 1) {
    iVar7 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8;
    uVar3 = *(uint *)(iVar7 + 0xac);
    if ((uVar3 & 0x41) == 1) {
      *(uint *)(iVar7 + 0xac) = uVar3 | 0x1062;
      iVar7 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8;
      *(uint *)(iVar7 + 0x158) = *(uint *)(iVar7 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x121) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x12e) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x122) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe6) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1cd) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1da) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1ce) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x192) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  iVar8 = _DAT_0000018a;
  func_0x80022da0(*(int *)(uVar3 - 0x387c) + 0x20,_DAT_0000018a + 0x18,&stack0xfffffff8);
  iVar8 = iVar8 + unaff_s1 * 0x204 + 0xe1c;
  func_0x80022da0(&stack0xfffffff8,iVar8 + -0x140,&stack0xfffffff8);
  func_0x80022da0(&stack0xfffffff8,iVar8 + -0x94,&stack0xfffffff8);
  func_0x80022da0(&stack0xfffffff8,iVar8 + 0x18,&stack0xfffffff8);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (iStackX_c - *(int *)(iVar8 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(iVar8 + 0x5c));
  return;
}



/* ======= FUN_80113740 @ 0x80113740 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113740(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee70c8))();
  return;
}



/* ======= FUN_801137f8 @ 0x801137f8 ======= */
// decompile failed: Exception while decompiling 801137f8: process: timeout


/* ======= FUN_8011388c @ 0x8011388c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011388c(undefined4 param_1)

{
  short sVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
  func_0x80019700(param_1,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
  puVar6 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar6[0x1a] = 0x207f60;
  *(undefined2 *)(puVar6 + 0x25) = 0;
  *(undefined2 *)((int)puVar6 + 0x96) = 0xffce;
  *(undefined2 *)(puVar6 + 0x26) = 0;
  *(undefined2 *)((int)puVar6 + 0x9a) = 3;
  *(undefined2 *)(puVar6 + 0x27) = 0;
  *(undefined2 *)((int)puVar6 + 0x9e) = 0;
  *puVar6 = *puVar6 | 0x4a;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xd2c) = 0x207f60;
  *(undefined2 *)(iVar3 + 0xd58) = 0;
  *(undefined2 *)(iVar3 + 0xd5a) = 0xffce;
  *(undefined2 *)(iVar3 + 0xd5c) = 0;
  *(undefined2 *)(iVar3 + 0xd5e) = 3;
  *(undefined2 *)(iVar3 + 0xd60) = 0;
  *(undefined2 *)(iVar3 + 0xd62) = 0;
  *(uint *)(iVar3 + 0xcc4) = *(uint *)(iVar3 + 0xcc4) | 0x4a;
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xd04,0x80118f18);
  func_0x800453d0(5);
  iVar3 = _DAT_800ac784;
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
  sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
  iVar3 = *(int *)(iVar3 + 0x188);
  *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  while (sVar1 != 1) {
    iVar5 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3;
    uVar4 = *(uint *)(iVar5 + 0xac);
    if ((uVar4 & 0x41) == 1) {
      *(uint *)(iVar5 + 0xac) = uVar4 | 0x1062;
      iVar5 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3;
      *(uint *)(iVar5 + 0x158) = *(uint *)(iVar5 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x121) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x12e) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x122) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe6) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1cd) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1da) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1ce) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x192) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}



/* ======= FUN_80113e1c @ 0x80113e1c ======= */
// decompile failed: Exception while decompiling 80113e1c: process: timeout


/* ======= FUN_80114618 @ 0x80114618 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114618(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee7070))();
  return;
}



/* ======= FUN_801146c8 @ 0x801146c8 ======= */
// decompile failed: Exception while decompiling 801146c8: process: timeout


/* ======= FUN_80114bd4 @ 0x80114bd4 ======= */
// decompile failed: Exception while decompiling 80114bd4: process: timeout


/* ======= FUN_80114de4 @ 0x80114de4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114de4(undefined4 param_1,int param_2)

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



/* ======= FUN_80114f50 @ 0x80114f50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114f50(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee7030))();
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



/* ======= FUN_8011508c @ 0x8011508c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011508c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80118fcc;
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



/* ======= FUN_80115234 @ 0x80115234 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115234(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee701c))();
  return;
}



/* ======= FUN_80115300 @ 0x80115300 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115300(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee7018))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6ff8))();
  return;
}



/* ======= FUN_8011537c @ 0x8011537c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011537c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6fd8))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6fb8))();
  return;
}



/* ======= FUN_801153f8 @ 0x801153f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801153f8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6f98))();
  return;
}



/* ======= FUN_80115440 @ 0x80115440 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115440(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6f98))();
  return;
}



/* ======= FUN_80115488 @ 0x80115488 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115488(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6f98))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80115520 @ 0x80115520 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115520(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee6f60))();
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



/* ======= FUN_8011565c @ 0x8011565c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011565c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011909c;
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



/* ======= FUN_80115804 @ 0x80115804 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115804(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee6f4c))();
  return;
}



/* ======= FUN_801158d0 @ 0x801158d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801158d0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6f48))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6f28))();
  return;
}



/* ======= FUN_8011594c @ 0x8011594c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011594c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6f08))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6ee8))();
  return;
}



/* ======= FUN_801159c8 @ 0x801159c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801159c8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6ec8))();
  return;
}



/* ======= FUN_80115a10 @ 0x80115a10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a10(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6ec8))();
  return;
}



/* ======= FUN_80115a58 @ 0x80115a58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a58(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6ec8))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80115af0 @ 0x80115af0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115af0(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee6e90))();
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



/* ======= FUN_80115c20 @ 0x80115c20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115c20(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011916c;
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



/* ======= FUN_80115dc8 @ 0x80115dc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115dc8(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee6e7c))();
  return;
}



/* ======= FUN_80115e94 @ 0x80115e94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e94(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6e78))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6e58))();
  return;
}



/* ======= FUN_80115f10 @ 0x80115f10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f10(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6e38))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6e18))();
  return;
}



/* ======= FUN_80115f8c @ 0x80115f8c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f8c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6df8))();
  return;
}



/* ======= FUN_80115fd4 @ 0x80115fd4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115fd4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6df8))();
  return;
}



/* ======= FUN_8011601c @ 0x8011601c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011601c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6df8))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_801160b4 @ 0x801160b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801160b4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee6dc0))();
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



/* ======= FUN_801161f8 @ 0x801161f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161f8(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011923c;
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



/* ======= FUN_801163a0 @ 0x801163a0 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801163a0(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee6dac))();
  return;
}



/* ======= FUN_801164cc @ 0x801164cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801164cc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6da4))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6d84))();
  return;
}



/* ======= FUN_80116548 @ 0x80116548 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116548(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6d64))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6d44))();
  return;
}



/* ======= FUN_801165c4 @ 0x801165c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801165c4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6d24))();
  return;
}



/* ======= FUN_8011660c @ 0x8011660c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011660c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee6d10))();
  return;
}



/* ======= FUN_80116654 @ 0x80116654 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116654(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee6d10))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_801166ec @ 0x801166ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166ec(void)

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
  _DAT_80072c34 = 0x8010c080;
  _DAT_80072c38 = 0x8010c448;
  _DAT_80072c3c = &LAB_8010ee9c;
  _DAT_80072c40 = 0x801109e4;
  _DAT_80072cac = 0x80115750;
  _DAT_80072cb4 = 0x80115d20;
  _DAT_80072cd0 = 0x801162f0;
  _DAT_80072cd8 = 0x801168b4;
  _DAT_800b52c8 = &LAB_8011713c;
  _DAT_80072bf8 = 0x8010a75c;
  _DAT_800ac998 = &LAB_801172bc;
  _DAT_800ac7a0 = 0x8010a120;
  _DAT_800ac7b0 = 0x8010a120;
  _DAT_800ac7b8 = 0x8010a120;
  _DAT_800ac7c8 = 0x8010a120;
  _DAT_800ac7cc = 0x8010a120;
  _DAT_800ac7d0 = 0x8010a120;
  _DAT_800ac7d4 = 0x8010a120;
  _DAT_800ac7e0 = 0x8010c3a8;
  _DAT_800ac7e4 = 0x8010eda0;
  _DAT_800ac798 = 0x8010c000;
  _DAT_800ac79c = 0x8010c000;
  _DAT_800ac7a4 = 0x8010c000;
  _DAT_800ac7ec = 0x801156b8;
  _DAT_800ac898 = 0x8010c040;
  _DAT_800ac8a0 = 0x8010a54c;
  _DAT_800ac8b0 = 0x8010a54c;
  _DAT_800ac8b8 = 0x8010a54c;
  _DAT_800ac8c8 = 0x8010a54c;
  _DAT_800ac8cc = 0x8010a54c;
  _DAT_800ac8d0 = 0x8010a54c;
  _DAT_800ac8d4 = 0x8010a54c;
  _DAT_800ac8e0 = 0x8010c3f8;
  _DAT_800ac8e4 = 0x8010edf0;
  _DAT_800ac8e8 = 0x80110854;
  _DAT_800ac89c = 0x8010c040;
  _DAT_800ac8a4 = 0x8010c040;
  _DAT_800ac8ec = 0x80115700;
  func_0x80029afc();
  return;
}



