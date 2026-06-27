/* ======= FUN_80100000 @ 0x80100000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(int param_1)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  
  iVar3 = (uint)(byte)(&LAB_80118cac)[*(byte *)(param_1 + 8)] * 0xac + *(int *)(param_1 + 0x188);
  *(undefined2 *)(iVar3 + 0x98) = 0x60;
  *(undefined2 *)(iVar3 + 0x9a) = 0x30;
  *(undefined2 *)(iVar3 + 0x9c) = 0x390;
  *(undefined2 *)(iVar3 + 0x94) = 0;
  *(undefined2 *)(iVar3 + 0x96) = 0;
  *(undefined2 *)(iVar3 + 0x9e) = 0x138;
  uVar4 = func_0x8001af20();
  if ((uVar4 & 3) == 0) {
    uVar4 = func_0x8001af20();
    (**(code **)((uVar4 & 7) * 4 + -0x7fee72ec))();
  }
  if ((_DAT_800aca3c & 1) != 0) {
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x100;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1d8) =
         *(ushort *)(_DAT_800ac784 + 0x1d8) | (ushort)((uVar4 & 1) << 9);
    uVar1 = func_0x8001af20();
    uVar2 = func_0x8001af20();
    iVar3 = (uVar4 & 1) * 8;
    *(ushort *)(_DAT_800ac784 + 0x1da) = (uVar2 & 0xff) + (uVar1 & 0xff) + 600;
    func_0x80019700(0x8031800,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000028);
    func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd0)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
  }
  func_0x8001af5c(0,0,600,700);
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if ((bVar5 == 1) || (bVar5 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
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



/* ======= FUN_801007bc @ 0x801007bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801007bc(void)

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



/* ======= FUN_8010080c @ 0x8010080c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010080c(void)

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



/* ======= FUN_801008f0 @ 0x801008f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801008f0(void)

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



/* ======= FUN_801009d8 @ 0x801009d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801009d8(undefined4 param_1)

{
  uint uVar1;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_80118ccc)[(unaff_s0 & 0xff) * 8] * 0xac +
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
  (**(code **)(&LAB_80118d34 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_80100e40 @ 0x80100e40 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e40(void)

{
  (**(code **)(&LAB_80118d68 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118db8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100ebc @ 0x80100ebc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100ebc(void)

{
  (**(code **)(&LAB_80118e08 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118e48 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100f38 @ 0x80100f38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f38(void)

{
  (**(code **)(&LAB_80118e88 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118db8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80100fb4 @ 0x80100fb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fb4(void)

{
  (**(code **)(&LAB_80118ed8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118e48 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101030 @ 0x80101030 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101030(void)

{
  (**(code **)(&LAB_80118ef0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118ef4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801010ac @ 0x801010ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010ac(void)

{
  (**(code **)(&LAB_80118ef8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118efc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101128 @ 0x80101128 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101128(void)

{
  (**(code **)(&LAB_80118f00 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118efc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_801011a4 @ 0x801011a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801011a4(void)

{
  (**(code **)(&LAB_80118f04 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118efc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101220 @ 0x80101220 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101220(void)

{
  (**(code **)(&LAB_80118f08 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118f0c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010129c @ 0x8010129c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010129c(void)

{
  (**(code **)(&LAB_80118f10 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80118f14 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80101318 @ 0x80101318 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101318(void)

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



/* ======= FUN_80101430 @ 0x80101430 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101430(void)

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



/* ======= FUN_801014bc @ 0x801014bc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801014bc(void)

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



/* ======= FUN_80101598 @ 0x80101598 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101598(void)

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



/* ======= FUN_801016a4 @ 0x801016a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016a4(void)

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
    FUN_80101ff0(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_8010180c @ 0x8010180c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010180c(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
    FUN_80102198();
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



/* ======= FUN_801019ac @ 0x801019ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801019ac(void)

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
      *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_80119018)[uVar5 & 0x1f];
      FUN_801022e4();
      return;
    }
    uVar5 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x1de) =
         *(undefined1 *)((int)&PTR_LAB_80119028 + (uVar5 & 0x1f));
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_801024d4(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80101cfc @ 0x80101cfc ======= */
// decompile failed: Exception while decompiling 80101cfc: process: timeout


/* ======= FUN_80101ff0 @ 0x80101ff0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ff0(short param_1,int param_2)

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
  FUN_80102b70();
  return;
}



/* ======= FUN_80102198 @ 0x80102198 ======= */

void FUN_80102198(void)

{
  int in_v1;
  int iVar1;
  
  iVar1 = *(int *)(in_v1 + -0x387c);
  if (((((*(byte *)(iVar1 + 0x1c2) & 2) != 0) && (7 < *(byte *)(iVar1 + 0x95))) &&
      (iVar1 = *(int *)(iVar1 + 0x1ac), -1 < *(short *)(iVar1 + 0x9a))) &&
     (((*(byte *)(iVar1 + 9) & 0x80) == 0 && ((*(ushort *)(iVar1 + 0x1d8) & 1) == 0)))) {
    *(undefined4 *)(iVar1 + 4) = 0xb01;
    FUN_80102b70();
    return;
  }
  return;
}



/* ======= FUN_801022e4 @ 0x801022e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801022e4(void)

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
    FUN_80102b70();
    return;
  }
  return;
}



/* ======= FUN_8010238c @ 0x8010238c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010238c(void)

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



/* ======= FUN_801024d4 @ 0x801024d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801024d4(void)

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



/* ======= FUN_8010257c @ 0x8010257c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010257c(void)

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



/* ======= FUN_801026d0 @ 0x801026d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801026d0(void)

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
          FUN_80109424(0,0);
        }
      }
      else if ((*(ushort *)(iVar3 + 0x1d8) & 0x40) == 0) {
        FUN_801037f0(0,1);
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



/* ======= FUN_801027c8 @ 0x801027c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801027c8(void)

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
  FUN_80109350(0,0);
  func_0x800245d8(0);
  return;
}



/* ======= FUN_80102998 @ 0x80102998 ======= */
// decompile failed: Exception while decompiling 80102998: process: timeout


/* ======= FUN_80102b70 @ 0x80102b70 ======= */

void FUN_80102b70(void)

{
  return;
}



/* ======= FUN_80102b7c @ 0x80102b7c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102b7c(void)

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
        FUN_8010359c();
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
      FUN_8010359c();
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
  FUN_8010359c();
  return;
}



/* ======= FUN_80102dac @ 0x80102dac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102dac(void)

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



/* ======= FUN_80102e90 @ 0x80102e90 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102e90(void)

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
        FUN_80109424(0,0);
      }
    }
    else if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x40) == 0) {
      FUN_801037f0(0,1);
      return;
    }
  }
  return;
}



/* ======= FUN_80103010 @ 0x80103010 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103010(void)

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



/* ======= FUN_80103134 @ 0x80103134 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103134(void)

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



/* ======= FUN_801031b0 @ 0x801031b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801031b0(void)

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



/* ======= FUN_8010320c @ 0x8010320c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010320c(uint param_1)

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



/* ======= FUN_80103348 @ 0x80103348 ======= */
// decompile failed: Exception while decompiling 80103348: process: timeout


/* ======= FUN_8010359c @ 0x8010359c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010359c(void)

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
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar4 + 0x40,&LAB_80118cfc);
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
  FUN_801049ec();
  return;
}



/* ======= FUN_801037f0 @ 0x801037f0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801037f0(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_80118cac)[*(byte *)(param_3 + 8)] * 0xac +
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
  FUN_801049ec();
  return;
}



/* ======= FUN_8010392c @ 0x8010392c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010392c(void)

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
    puVar3 = (uint *)((uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80103cfc @ 0x80103cfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103cfc(void)

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



/* ======= FUN_80103e20 @ 0x80103e20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103e20(void)

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



/* ======= FUN_80103f14 @ 0x80103f14 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103f14(void)

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



/* ======= FUN_80103fbc @ 0x80103fbc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103fbc(void)

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



/* ======= FUN_8010409c @ 0x8010409c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010409c(void)

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
        FUN_801049ec();
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
      FUN_801049ec();
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
  FUN_801049ec();
  return;
}



/* ======= FUN_8010412c @ 0x8010412c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010412c(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_801049ec();
  return;
}



/* ======= FUN_80104204 @ 0x80104204 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104204(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80104adc();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      return;
    }
    if (bVar1 != 0) {
      FUN_80104adc();
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
  FUN_80104adc();
  return;
}



/* ======= FUN_801042f4 @ 0x801042f4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801042f4(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 8) {
                    /* WARNING: Jumptable at 0x80104364 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801000a4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}



/* ======= FUN_80104734 @ 0x80104734 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104734(void)

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
      goto LAB_801050c8;
    }
    if (bVar2 != 0) goto LAB_801050c8;
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
LAB_801050c8:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_80119078 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109350(0,0);
    return;
  }
  FUN_801059b8(0,1);
  return;
}



/* ======= FUN_801048e0 @ 0x801048e0 ======= */
// decompile failed: Exception while decompiling 801048e0: process: timeout


/* ======= FUN_801049ec @ 0x801049ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801049ec(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_80109350(0,0);
  iVar3 = (uint)(byte)(&LAB_80118cac)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104adc @ 0x80104adc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104adc(void)

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
  iVar4 = (uint)(byte)(&LAB_80118cac)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + -0x6c54);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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



/* ======= FUN_80104ca8 @ 0x80104ca8 ======= */
// decompile failed: Exception while decompiling 80104ca8: process: timeout


/* ======= FUN_80104dd0 @ 0x80104dd0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104dd0(void)

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



/* ======= FUN_80104f70 @ 0x80104f70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f70(void)

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
         *(undefined2 *)(&LAB_80119078 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_80119078_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
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
         *(undefined2 *)(&LAB_80119078 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_801059b8(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80105240 @ 0x80105240 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105240(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee6f48))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801069ec();
  }
  else {
    FUN_80106854();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105dbc:
    FUN_80109350(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_80109350(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105dbc;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106e54();
  return;
}



/* ======= FUN_80105330 @ 0x80105330 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105330(void)

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
        if (4 < DAT_800aca4e) {
          uVar4 = func_0x8001af20();
          *(undefined1 *)(_DAT_800ac784 + 0x1de) = (&LAB_80119018)[uVar4 & 0x1f];
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
          goto LAB_80105fe0;
        }
        uVar4 = func_0x8001af20();
        *(undefined1 *)(_DAT_800ac784 + 0x1de) =
             *(undefined1 *)((int)&PTR_LAB_80119028 + (uVar4 & 0x1f));
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
      goto LAB_801057e0;
    }
    if (bVar2 != 0) goto LAB_80105fe0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar5 = (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
    (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
LAB_80105fe0:
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
LAB_801057e0:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_801057fc @ 0x801057fc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801057fc(void)

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
        FUN_80106220(0);
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
      FUN_80109350(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_80106220(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_801193bc)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_801193c4);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
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
    FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
                      (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&LAB_80119694);
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
      FUN_80109290(0,1);
      return;
    }
    FUN_80109350(0,0);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_801059b8 @ 0x801059b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801059b8(int param_1)

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
  FUN_80109350(0,0);
  func_0x800245d8(0x800);
  return;
}



/* ======= FUN_80105a44 @ 0x80105a44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105a44(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105bc0:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105dbc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
        FUN_80109350(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_801065bc;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_801065bc;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_80119358 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105bc0;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_801065bc;
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
      goto LAB_80105dbc;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105dbc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193ac);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_801193ac);
    puVar5 = (undefined1 *)0x0;
LAB_801065bc:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106e54();
    return;
  }
LAB_80105dbc:
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80105dd8 @ 0x80105dd8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105dd8(void)

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
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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



/* ======= FUN_80106054 @ 0x80106054 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106054(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801069d8();
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
      FUN_801069d8();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_801193c4);
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
  FUN_801069d8();
  return;
}



/* ======= FUN_801061ec @ 0x801061ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801061ec(undefined4 param_1,undefined4 param_2,int param_3)

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
      goto LAB_80106b48;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106b48;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193c4);
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
LAB_80106b48:
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
    FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
    FUN_80109350(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80119694);
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
    FUN_80109350(0,0);
    return;
  }
  FUN_80109290(0,1);
  return;
}



/* ======= FUN_80106220 @ 0x80106220 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106220(int param_1,undefined4 param_2,int param_3)

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
    FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
    FUN_80109350(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_80119694);
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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80106358 @ 0x80106358 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106358(void)

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
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee6c2c))();
    return;
  }
  FUN_80107c64();
  *unaff_s3 = extraout_v1 | 0x4a;
  func_0x800453d0();
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar4 + 0x548) = 0x80;
  *(undefined4 *)(iVar4 + 0x54c) = 0x20;
  *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
    FUN_80109350(0,0);
    return;
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if (*(char *)(iVar4 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar4 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar4 + 8)] * 0xac +
                    *(int *)(iVar4 + 0x188) + 0x40,&LAB_80119694);
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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_801063cc @ 0x801063cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801063cc(void)

{
  byte bVar1;
  char cVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_80106e54();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80119694);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109350(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80106e54();
  return;
}



/* ======= FUN_80106690 @ 0x80106690 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106690(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_801071b4();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_80109350(0,1);
      FUN_80107c50();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    puVar5 = (uint *)((uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_80119694);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_80119694);
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
      FUN_80107120(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_801071b4();
  return;
}



/* ======= FUN_80106854 @ 0x80106854 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106854(undefined4 param_1)

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
    FUN_80107120(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_801071b4();
  return;
}



/* ======= FUN_801069d8 @ 0x801069d8 ======= */

void FUN_801069d8(void)

{
  return;
}



/* ======= FUN_801069ec @ 0x801069ec ======= */

void FUN_801069ec(void)

{
  return;
}



/* ======= FUN_801069f8 @ 0x801069f8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801069f8(void)

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
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    puVar7 = (uint *)((uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
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
      FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
    FUN_80109350(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80119694);
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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80106de8 @ 0x80106de8 ======= */
// decompile failed: Exception while decompiling 80106de8: process: timeout


/* ======= FUN_80106e54 @ 0x80106e54 ======= */
// decompile failed: Exception while decompiling 80106e54: process: timeout


/* ======= FUN_80107120 @ 0x80107120 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107120(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109350(0,1);
    FUN_80107c50();
    return;
  }
  FUN_80109350(0,0);
  FUN_80107c50();
  return;
}



/* ======= FUN_801071b4 @ 0x801071b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801071b4(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80109350(0,1);
  FUN_80107c50();
  return;
}



/* ======= FUN_80107464 @ 0x80107464 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107464(void)

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
    if (*(char *)(_DAT_800ac784 + 7) != '\0') goto LAB_80107e80;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80119694)
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
      goto LAB_80107e80;
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
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
LAB_80107e80:
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
    FUN_80109350(0,0);
    return;
  }
  FUN_80109290(0,1);
  return;
}



/* ======= FUN_80107694 @ 0x80107694 ======= */
// decompile failed: Exception while decompiling 80107694: process: timeout


/* ======= FUN_80107c50 @ 0x80107c50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107c50(undefined4 param_1,undefined4 param_2,undefined4 param_3)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80119694);
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80119694);
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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80107c64 @ 0x80107c64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107c64(void)

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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80119694);
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
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&LAB_80119694);
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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80108270 @ 0x80108270 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108270(void)

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
      goto LAB_80108ca4;
    }
    unaff_s0 = 1;
    if (bVar1 != 0) goto LAB_80108ca4;
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
LAB_80108ca4:
  func_0x80022da0(iVar4,unaff_s0 + 0x18,iVar4);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (uStackX_c - *(int *)(unaff_s0 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(unaff_s0 + 0x5c));
  return;
}



/* ======= FUN_801084b8 @ 0x801084b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801084b8(uint *param_1)

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



/* ======= FUN_80108528 @ 0x80108528 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108528(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 == 1) {
    cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
    FUN_80109118();
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      FUN_80109118();
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
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_80118ccc)
                                         [(ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8] * 0xac +
                             _DAT_800ac784[0x62] + 0x40);
    return;
  }
  if (bVar1 != 2) {
    FUN_80109118();
    return;
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109350(0,0);
  }
  return;
}



/* ======= FUN_80108950 @ 0x80108950 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108950(void)

{
  (**(code **)(&LAB_801196b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_80108998 @ 0x80108998 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108998(void)

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
    FUN_80109290(0,1);
    return;
  }
  FUN_80109350(0,0);
  return;
}



/* ======= FUN_80108aa8 @ 0x80108aa8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108aa8(void)

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



/* ======= FUN_80108b50 @ 0x80108b50 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108b50(undefined4 param_1,int param_2)

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



/* ======= FUN_80108c24 @ 0x80108c24 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108c24(undefined4 param_1,int param_2)

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



/* ======= FUN_80108cb8 @ 0x80108cb8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108cb8(undefined4 param_1,int param_2)

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



/* ======= FUN_80108d08 @ 0x80108d08 ======= */
// decompile failed: Exception while decompiling 80108d08: process: timeout


/* ======= FUN_80109118 @ 0x80109118 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109118(int param_1)

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
    FUN_80109d2c();
    return;
  }
  return;
}



/* ======= FUN_80109290 @ 0x80109290 ======= */

void FUN_80109290(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_80109350 @ 0x80109350 ======= */

void FUN_80109350(void)

{
  func_0x80019d50();
  return;
}



/* ======= FUN_80109424 @ 0x80109424 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109424(void)

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



/* ======= FUN_8010954c @ 0x8010954c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010954c(void)

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



/* ======= FUN_80109600 @ 0x80109600 ======= */
// decompile failed: Exception while decompiling 80109600: process: timeout


/* ======= FUN_80109980 @ 0x80109980 ======= */

void FUN_80109980(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(5,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_801099bc @ 0x801099bc ======= */

void FUN_801099bc(void)

{
  func_0x80037edc(0,10);
  func_0x80037edc(4,0x32);
  func_0x80037edc(7,0x32);
  return;
}



/* ======= FUN_801099f8 @ 0x801099f8 ======= */

void FUN_801099f8(void)

{
  func_0x80037edc(1,0x32);
  return;
}



/* ======= FUN_80109a1c @ 0x80109a1c ======= */

void FUN_80109a1c(void)

{
  func_0x80037edc(2,0x32);
  return;
}



/* ======= FUN_80109d2c @ 0x80109d2c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109d2c(void)

{
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acae3 = 7;
  (**(code **)(&LAB_801196e0 + (_DAT_800aca58 >> 8 & 0xff) * 4))();
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
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a86c();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a86c();
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_801196f0
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&LAB_801196f0
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8010a86c();
  return;
}



/* ======= FUN_8010a07c @ 0x8010a07c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a07c(void)

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
    (**(code **)(&LAB_80119730 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
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



/* ======= FUN_8010a2e0 @ 0x8010a2e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a2e0(void)

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
  _DAT_800ac784[0x1e] = DAT_80118cb8;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_80118d0c)[uVar3 & 7];
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
                    (uint)(byte)(&LAB_80118ccc)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_28
                   );
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &local_38);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd0)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd1)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,&local_38
                   );
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd2)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd3)[iVar5] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_80118cfc);
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



/* ======= FUN_8010a86c @ 0x8010a86c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a86c(void)

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



/* ======= FUN_8010aa28 @ 0x8010aa28 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aa28(undefined4 param_1)

{
  uint *puVar1;
  undefined2 uVar2;
  uint uVar3;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_80118ccc)[(unaff_s0 & 0xff) * 8] * 0xac +
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
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010c03c(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                                  (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]));
          *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
          (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee610c))();
          (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee60cc))();
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
          FUN_8010c8ac();
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
  (**(code **)(&LAB_80119758 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}



/* ======= FUN_8010ae88 @ 0x8010ae88 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ae88(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (**(code **)(&LAB_8011978c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801197d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010af38 @ 0x8010af38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010af38(void)

{
  (**(code **)(&LAB_80119814 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80119830 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010afb4 @ 0x8010afb4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010afb4(void)

{
  (**(code **)(&LAB_8011984c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801197d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b030 @ 0x8010b030 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b030(void)

{
  (**(code **)(&LAB_80119884 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80119830 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b0ac @ 0x8010b0ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b0ac(void)

{
  (**(code **)(&DAT_8011989c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801198a0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b128 @ 0x8010b128 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b128(void)

{
  (**(code **)(&DAT_801198a4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801198a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b1a4 @ 0x8010b1a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b1a4(void)

{
  (**(code **)(&LAB_801198ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801198a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b220 @ 0x8010b220 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b220(void)

{
  (**(code **)(&LAB_801198b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801198a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b29c @ 0x8010b29c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b29c(void)

{
  (**(code **)(&LAB_801198b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801198b8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b318 @ 0x8010b318 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b318(void)

{
  (**(code **)(&LAB_801198bc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801198c0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ======= FUN_8010b39c @ 0x8010b39c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b39c(void)

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
      else if (bVar1 != 3) goto LAB_8010bdec;
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
    if (bVar1 != 0) goto LAB_8010bdec;
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
LAB_8010bdec:
  *(undefined2 *)(_DAT_800ac784 + 0x166) = 0x14cc;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee618c);
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



/* ======= FUN_8010b604 @ 0x8010b604 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b604(void)

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
        FUN_8010c03c(0,0);
      }
    }
    else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
      uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                              (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                              (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                              (_DAT_800aca90 - _DAT_800ac784[0xf]));
      *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
      (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee610c))();
      (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee60cc))();
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
        FUN_8010c8ac();
        return;
      }
      *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
      return;
    }
  }
  return;
}



/* ======= FUN_8010b734 @ 0x8010b734 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b734(void)

{
  int iVar1;
  uint unaff_s0;
  
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)((int)_DAT_800ac784 + 5) * 0x20 + -0x7fee673c))();
    return;
  }
  if (*(char *)((int)_DAT_800ac784 + 5) == '\x02') {
    iVar1 = FUN_801069ec();
  }
  else {
    iVar1 = FUN_80106854();
  }
  (**(code **)((uint)*(byte *)(*(int *)(iVar1 + -0x387c) + 5) * 4 + -0x7fee60cc))();
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
    FUN_8010c8ac();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010b7c8 @ 0x8010b7c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b7c8(void)

{
  uint *extraout_v1;
  uint unaff_s0;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee649c))();
    return;
  }
  FUN_80107c64();
  if ((char)extraout_v1[0x78] != '\0') {
    *extraout_v1 = *extraout_v1 & unaff_s0;
    func_0x80012a0c(5000);
  }
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
    FUN_8010c8ac();
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010b83c @ 0x8010b83c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b83c(undefined4 param_1,int param_2)

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



/* ======= FUN_8010b9a0 @ 0x8010b9a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b9a0(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee612c))();
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



/* ======= FUN_8010bb38 @ 0x8010bb38 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bb38(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee7a60);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = *(uint *)((uint)(byte)_DAT_800ac784[0x79] * 4 + -0x7fee618c);
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



/* ======= FUN_8010bebc @ 0x8010bebc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bebc(void)

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
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee610c))();
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee60cc))();
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
    FUN_8010c8ac();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010c03c @ 0x8010c03c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c03c(void)

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
    FUN_8010c8ac();
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}



/* ======= FUN_8010c1b0 @ 0x8010c1b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c1b0(void)

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



/* ======= FUN_8010c3a0 @ 0x8010c3a0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c3a0(void)

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



/* ======= FUN_8010c49c @ 0x8010c49c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c49c(void)

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
LAB_8010c56c:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(3);
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    bVar8 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    uVar10 = FUN_80111208(0,0);
    in_v1 = (uint)((ulonglong)uVar10 >> 0x20);
    uVar6 = (uint)uVar10;
  }
  else {
    if (uVar6 == 0) {
      uVar6 = 2;
      if (bVar8 != 2) goto LAB_8010ce68;
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
          goto LAB_8010ce68;
        }
        uVar6 = 3;
        uVar3 = 3;
        if (in_v1 != 0) goto LAB_8010ce68;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_8010ce68;
    }
    if (bVar8 != 0) goto LAB_8010ce68;
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
        goto LAB_8010c56c;
      }
    }
    uVar9 = unaff_s0 - 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    if (unaff_s0 != 0) {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar9 + 0x80119f78)) {
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
LAB_8010ce68:
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s3 + 0x80119f84)) {
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



/* ======= FUN_8010c678 @ 0x8010c678 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c678(void)

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



/* ======= FUN_8010c7b0 @ 0x8010c7b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7b0(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  ushort unaff_s3;
  short unaff_s4;
  undefined1 *puVar5;
  
  if (_DAT_800ac784[6] == '\0') {
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 4;
    if (_DAT_800ac784[7] == '\0') {
      _DAT_800ac784[0x94] = 5;
      bVar2 = func_0x8001af20();
      _DAT_800ac784[0x9e] = (bVar2 & 0x1f) + 2;
    }
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  bVar2 = func_0x8001af20();
  _DAT_800ac784[0x9e] = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)(char)_DAT_800ac784[0x9e]);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (_DAT_800ac784[7] == '\0') {
    if ((byte)_DAT_800ac784[0x95] < 0x18) {
      FUN_80111208(0,1);
      if (_DAT_800ac784[0x95] != '\x01') {
        FUN_8010d140();
        return;
      }
LAB_8010c948:
      func_0x800453d0(8);
      return;
    }
    do {
      if (*(char *)(iRamffffc784 + 0x95) == *(char *)(unaff_s3 + 0x80119f7c)) {
        unaff_s4 = 1;
      }
      bVar1 = unaff_s3 != 0;
      unaff_s3 = unaff_s3 - 1;
    } while (bVar1);
    if (unaff_s4 == 0) {
      return;
    }
    puVar5 = &DAT_800b0000;
  }
  else {
    puVar5 = _DAT_800ac784;
    if ((byte)_DAT_800ac784[0x95] < 0x29) {
      FUN_80111208(0,1);
      if ((_DAT_800ac784[0x95] != '\x01') && (_DAT_800ac784[0x95] != ')')) {
        return;
      }
      goto LAB_8010c948;
    }
  }
  iVar4 = *(int *)(puVar5 + -0x387c);
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
  _DAT_800ac784[5] = DAT_800acae7;
  _DAT_800ac784[6] = 1;
  _DAT_800ac784[7] = 0;
  _DAT_800ac784[0x95] = 0x1d;
  _DAT_800ac784[0x8f] = 7;
  _DAT_800ac784[0x94] = 3;
  return;
}



/* ======= FUN_8010c8ac @ 0x8010c8ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c8ac(void)

{
  bool bVar1;
  char cVar2;
  int in_v1;
  ushort unaff_s3;
  short unaff_s4;
  
  if (*(byte *)(in_v1 + 0x95) < 0x18) {
    FUN_80111208(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
      FUN_8010d140();
      return;
    }
    func_0x800453d0(8);
    return;
  }
  do {
    if (*(char *)(iRamffffc784 + 0x95) == *(char *)(unaff_s3 + 0x80119f7c)) {
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



/* ======= FUN_8010c960 @ 0x8010c960 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c960(void)

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



/* ======= FUN_8010caa8 @ 0x8010caa8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010caa8(void)

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



/* ======= FUN_8010cc08 @ 0x8010cc08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cc08(void)

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
      goto LAB_8010d6bc;
    }
    bVar4 = 1;
    if (uVar7 != 0) goto LAB_8010d6bc;
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 0x12;
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  _DAT_800ac784[6] = _DAT_800ac784[6] + cVar3;
  FUN_80111208(0,0);
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
    if (_DAT_800ac784[0x95] == *(char *)(uVar9 + 0x80119f78)) {
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
LAB_8010d6bc:
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
    local_28 = _DAT_80119eb4;
    local_24 = _DAT_80119eb8;
    local_20 = _DAT_80119ebc;
    local_1c = _DAT_80119ec0;
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
          if (_DAT_800ac784[0x95] == *(char *)(uVar10 + 0x80119f84)) {
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



/* ======= FUN_8010cee4 @ 0x8010cee4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cee4(void)

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
            goto LAB_8010da70;
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
      goto LAB_8010da70;
    }
    bVar5 = 1;
    if (bVar2 != 0) goto LAB_8010da70;
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
    *(undefined1 *)(extraout_v1 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800453d0();
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    return;
  }
  FUN_80111208(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x15);
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
    if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar8 + 0x80119f7c)) {
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
LAB_8010da70:
  *(byte *)(_DAT_800ac784 + 5) = bVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}



/* ======= FUN_8010d140 @ 0x8010d140 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d140(void)

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



/* ======= FUN_8010d29c @ 0x8010d29c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d29c(void)

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
            FUN_8010ddf4();
            return;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
          local_28 = _DAT_80119eb4;
          local_24 = _DAT_80119eb8;
          local_20 = _DAT_80119ebc;
          local_1c = _DAT_80119ec0;
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
        goto LAB_8010de0c;
      }
      if (bVar1 != 3) goto LAB_8010de0c;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        goto LAB_8010de0c;
      }
      goto LAB_8010d604;
    }
    if (bVar1 != 0) goto LAB_8010de0c;
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
LAB_8010de0c:
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
LAB_8010d604:
  func_0x800245d8(0);
  return;
}



/* ======= FUN_8010d624 @ 0x8010d624 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d624(void)

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
    local_48 = _DAT_80119eb4;
    local_44 = _DAT_80119eb8;
    local_40 = _DAT_80119ebc;
    local_3c = _DAT_80119ec0;
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



/* ======= FUN_8010db00 @ 0x8010db00 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010db00(void)

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
      local_28 = _DAT_80119eb4;
      local_24 = _DAT_80119eb8;
      local_20 = _DAT_80119ebc;
      local_1c = _DAT_80119ec0;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_8010dbe8 @ 0x8010dbe8 ======= */
// decompile failed: Exception while decompiling 8010dbe8: process: timeout


/* ======= FUN_8010ddf4 @ 0x8010ddf4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ddf4(void)

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



/* ======= FUN_8010deb0 @ 0x8010deb0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010deb0(void)

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
        FUN_8010e788();
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = _DAT_80119eb4;
      local_24 = _DAT_80119eb8;
      local_20 = _DAT_80119ebc;
      local_1c = _DAT_80119ec0;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}



/* ======= FUN_8010df98 @ 0x8010df98 ======= */
// decompile failed: Exception while decompiling 8010df98: process: timeout


/* ======= FUN_8010e214 @ 0x8010e214 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e214(void)

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
      FUN_8010ead8();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = _DAT_80119eb4;
    local_24 = _DAT_80119eb8;
    local_20 = _DAT_80119ebc;
    local_1c = _DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010e2e8 @ 0x8010e2e8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e2e8(void)

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



/* ======= FUN_8010e4b4 @ 0x8010e4b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e4b4(void)

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
      FUN_8010ed78();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = _DAT_80119eb4;
    local_24 = _DAT_80119eb8;
    local_20 = _DAT_80119ebc;
    local_1c = _DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010e588 @ 0x8010e588 ======= */
// decompile failed: Exception while decompiling 8010e588: process: timeout


/* ======= FUN_8010e788 @ 0x8010e788 ======= */

void FUN_8010e788(undefined4 param_1,undefined4 param_2,int param_3)

{
  char in_v0;
  
  *(char *)(*(int *)(param_3 + -0x387c) + 6) = *(char *)(*(int *)(param_3 + -0x387c) + 6) + in_v0;
  FUN_801112dc(0,0);
  FUN_8010efdc();
  return;
}



/* ======= FUN_8010e7ec @ 0x8010e7ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e7ec(void)

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
      FUN_8010f2ac();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = _DAT_80119eb4;
    local_34 = _DAT_80119eb8;
    local_30 = _DAT_80119ebc;
    local_2c = _DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar4 = func_0x8001a780(&DAT_800aca54), iVar4 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_8010f2ac();
    return;
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_80119f89)[bVar5]) {
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
    if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x80119f88)) {
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



/* ======= FUN_8010ead8 @ 0x8010ead8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ead8(void)

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
    FUN_8010f444(0,0);
    return;
  }
  FUN_801112dc(0,1);
  return;
}



/* ======= FUN_8010eadc @ 0x8010eadc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eadc(void)

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
    FUN_8010f444(0,0);
    return;
  }
  FUN_801112dc(0,1);
  return;
}



/* ======= FUN_8010ec5c @ 0x8010ec5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec5c(void)

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
      FUN_8010f520();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = _DAT_80119eb4;
    local_24 = _DAT_80119eb8;
    local_20 = _DAT_80119ebc;
    local_1c = _DAT_80119ec0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}



/* ======= FUN_8010ed30 @ 0x8010ed30 ======= */

/* WARNING: Removing unreachable block (ram,0x8010f05c) */
/* WARNING: Removing unreachable block (ram,0x8010f060) */
/* WARNING: Removing unreachable block (ram,0x8010f06c) */
/* WARNING: Removing unreachable block (ram,0x8010f078) */
/* WARNING: Removing unreachable block (ram,0x8010f084) */
/* WARNING: Removing unreachable block (ram,0x8010f0e4) */
/* WARNING: Removing unreachable block (ram,0x8010f0f4) */
/* WARNING: Removing unreachable block (ram,0x8010efe8) */
/* WARNING: Removing unreachable block (ram,0x8010f014) */
/* WARNING: Removing unreachable block (ram,0x8010f058) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ed30(void)

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
LAB_8010ede8:
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
      FUN_801112dc(0,1);
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80119f8c)) {
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
      goto LAB_8010ede8;
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
  uStack_28 = *(undefined4 *)(iVar7 + -0x7fee616c);
  uStack_24 = *(undefined4 *)(iVar7 + -0x7fee6168);
  uStack_20 = *(undefined4 *)(iVar7 + -0x7fee6164);
  uStack_1c = *(undefined4 *)(iVar7 + -0x7fee6160);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
  func_0x800453d0(2);
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80111208(0,0);
  }
  else {
    FUN_80111208(0,1);
  }
  return;
}



/* ======= FUN_8010ed78 @ 0x8010ed78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ed78(undefined1 *param_1)

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
LAB_8010f938:
    param_1[0x93] = bVar2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar6 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    in_stack_00000010 = *(undefined4 *)(iVar6 + -0x7fee616c);
    in_stack_00000014 = *(undefined4 *)(iVar6 + -0x7fee6168);
    in_stack_00000018 = *(undefined4 *)(iVar6 + -0x7fee6164);
    in_stack_0000001c = *(undefined4 *)(iVar6 + -0x7fee6160);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
    func_0x800453d0(2);
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
      FUN_80111208(0,0);
    }
    else {
      FUN_80111208(0,1);
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
  FUN_801112dc(0,1);
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
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80119f8c)) {
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
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x80119f94)) {
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
      goto LAB_8010f938;
    }
  }
  return;
}



/* ======= FUN_8010efdc @ 0x8010efdc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010efdc(void)

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



/* ======= FUN_8010f164 @ 0x8010f164 ======= */
// decompile failed: Exception while decompiling 8010f164: process: timeout


/* ======= FUN_8010f2ac @ 0x8010f2ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f2ac(undefined4 param_1,int param_2)

{
  int in_at;
  undefined4 in_v0;
  undefined1 in_v1;
  
  *(undefined4 *)(in_at + -0x35a8) = in_v0;
  DAT_800acae7 = in_v1;
  _DAT_800acbd0 = param_1;
  *(undefined2 *)(param_2 + 0x6a) = _DAT_800acabe;
  FUN_8010fc44(6);
  return;
}



/* ======= FUN_8010f444 @ 0x8010f444 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f444(void)

{
  char cVar1;
  
  func_0x800453d0();
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010fe20();
  return;
}



/* ======= FUN_8010f520 @ 0x8010f520 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f520(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee618c);
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '1') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 8;
    FUN_8010fe20();
    return;
  }
  return;
}



/* ======= FUN_8010f630 @ 0x8010f630 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f630(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee618c);
    func_0x800453d0(1);
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee604c))();
  return;
}



/* ======= FUN_8010f6ec @ 0x8010f6ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f6ec(void)

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
      goto LAB_801100d0;
    }
    if (uVar2 != 0) goto LAB_801100d0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar1 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar1 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar1 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar1 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar1 + -0x7fee6160);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
LAB_801100d0:
  if (uVar2 != 0x3d) {
    return;
  }
  func_0x800453d0(1);
  FUN_80110910();
  return;
}



/* ======= FUN_8010f8e0 @ 0x8010f8e0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f8e0(void)

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
    local_20 = *(undefined4 *)(iVar3 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar3 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar3 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar3 + -0x7fee6160);
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



/* ======= FUN_8010fb58 @ 0x8010fb58 ======= */
// decompile failed: Exception while decompiling 8010fb58: process: timeout


/* ======= FUN_8010fc44 @ 0x8010fc44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc44(undefined4 param_1)

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
  uStack00000010 = *(undefined4 *)(iVar2 + -0x7fee616c);
  uStack00000014 = *(undefined4 *)(iVar2 + -0x7fee6168);
  uStack00000018 = *(undefined4 *)(iVar2 + -0x7fee6164);
  uStack0000001c = *(undefined4 *)(iVar2 + -0x7fee6160);
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
    FUN_80110f78();
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
  FUN_80110f78();
  return;
}



/* ======= FUN_8010fe20 @ 0x8010fe20 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe20(void)

{
  undefined1 in_v0;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 5) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}



/* ======= FUN_8010fe54 @ 0x8010fe54 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe54(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee618c);
    func_0x800453d0(1);
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5ff4))();
  return;
}



/* ======= FUN_8010ff10 @ 0x8010ff10 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ff10(void)

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
      FUN_80110910();
      return;
    }
    if (bVar1 != 0) {
      FUN_80110910();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar3 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar3 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar3 + -0x7fee6160);
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
  FUN_80111208(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) != '=') {
    return;
  }
  func_0x800453d0(1);
  FUN_80110910();
  return;
}



/* ======= FUN_80110120 @ 0x80110120 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110120(void)

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
      goto LAB_80110b44;
    }
    if (bVar1 != 0) goto LAB_80110b44;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar4 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar4 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar4 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar4 + -0x7fee6160);
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
LAB_80110b44:
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



/* ======= FUN_80110354 @ 0x80110354 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110354(void)

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
      FUN_80110dc4();
      return;
    }
    if (bVar1 != 0) {
      FUN_80110dc4();
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
    local_20 = *(undefined4 *)(iVar4 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar4 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar4 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar4 + -0x7fee6160);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      FUN_80110d0c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
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
  FUN_80110dc4();
  return;
}



/* ======= FUN_801105d4 @ 0x801105d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801105d4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5f9c))();
  return;
}



/* ======= FUN_8011061c @ 0x8011061c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011061c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80110f78();
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
      FUN_80110f78();
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
  FUN_80110f78();
  return;
}



/* ======= FUN_80110788 @ 0x80110788 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110788(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee6188);
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



/* ======= FUN_80110910 @ 0x80110910 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110910(int param_1)

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



/* ======= FUN_80110a08 @ 0x80110a08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a08(undefined4 param_1,int param_2)

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



/* ======= FUN_80110adc @ 0x80110adc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110adc(undefined4 param_1,int param_2)

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



/* ======= FUN_80110d0c @ 0x80110d0c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110d0c(void)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  func_0x8001ad68(&DAT_800aca54);
  uVar3 = 0x80110d38;
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



/* ======= FUN_80110dc4 @ 0x80110dc4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110dc4(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  int unaff_s0;
  undefined4 uVar4;
  
  func_0x8001ad68(unaff_s0 + -0x95,_DAT_800acbcc,_DAT_800acbd0);
  uVar4 = 0x80110df8;
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



/* ======= FUN_80110f44 @ 0x80110f44 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f44(void)

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
      goto code_r0x801118bc;
    }
    iVar2 = 0x2000;
    if (DAT_800aca5a != 0) goto code_r0x801118bc;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a07c);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a07c);
  }
  iVar2 = _DAT_800acbcc;
  bVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + bVar1,_DAT_800aca58);
code_r0x801118bc:
  if (((bVar1 & 0x19) == 0) &&
     ((((*(byte *)(iVar2 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(iVar2 + 0x6a)) + 0x200 & 0xfff) <
      0x400)) {
    *(byte *)(iVar2 + 0x9f) = *(byte *)(iVar2 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ======= FUN_80110f78 @ 0x80110f78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f78(void)

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



/* ======= FUN_801110d0 @ 0x801110d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110d0(void)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  uVar2 = 4;
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5f4c))();
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



/* ======= FUN_80111208 @ 0x80111208 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111208(int param_1)

{
  int in_v1;
  
  if (*(int *)(param_1 + 0x38) < in_v1) {
    *(undefined2 *)(param_1 + 0xb4) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0xb6) = 0;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_80111264 @ 0x80111264 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111264(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee7a60);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = _DAT_8011a0ac;
  func_0x8001af5c(0,0,1000,0x44c,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_801112dc @ 0x801112dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801112dc(void)

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
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee7a60);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = _DAT_8011a0ac;
  func_0x8001af5c(0,0,1000,0x44c);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}



/* ======= FUN_80111498 @ 0x80111498 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111498(void)

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
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee5f2c))();
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee5eec))();
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
      cRam00000006 = cVar3;
      return;
    }
    FUN_80115bec(0,0);
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  return;
}



/* ======= FUN_801116e4 @ 0x801116e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801116e4(void)

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



/* ======= FUN_80111824 @ 0x80111824 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111824(void)

{
  int iVar1;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(8000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_80112114();
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



/* ======= FUN_80111924 @ 0x80111924 ======= */
// decompile failed: Exception while decompiling 80111924: process: timeout


/* ======= FUN_80111a94 @ 0x80111a94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111a94(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801123d8();
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        FUN_801123d8();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_801123d8();
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
  FUN_801123d8();
  return;
}



/* ======= FUN_80111be8 @ 0x80111be8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111be8(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(3000,0x100,&DAT_800aca88), iVar1 < 0)) &&
      (*(short *)(_DAT_800ac784 + 0x1de) == 0)) &&
     ((0x50 < _DAT_800acaee && (0x9c4 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
    DAT_800aca5a = 7;
    FUN_80112e04();
    return;
  }
  if (DAT_800acae7 == '\0') {
    iVar1 = func_0x8001a804(0x708,0x180,&DAT_800aca88);
    if ((iVar1 < 0) && (*(short *)(_DAT_800ac784 + 0x1de) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      FUN_801125cc();
      return;
    }
    if (((DAT_800acae7 == '\0') && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) &&
       (7000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      FUN_801125cc();
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



/* ======= FUN_80111ddc @ 0x80111ddc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111ddc(void)

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
    cRam00000006 = cVar4;
    return;
  }
  FUN_80115bec(0,0);
  return;
}



/* ======= FUN_80111f5c @ 0x80111f5c ======= */
// decompile failed: Exception while decompiling 80111f5c: process: timeout


/* ======= FUN_80112114 @ 0x80112114 ======= */

void FUN_80112114(void)

{
  uint uVar1;
  byte in_v1;
  byte *unaff_s0;
  
  *unaff_s0 = in_v1 | 1;
  uVar1 = func_0x8001af20();
  func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee5eb0 + (uVar1 & 1)),0x32);
  FUN_80112a48();
  return;
}



/* ======= FUN_80112264 @ 0x80112264 ======= */
// decompile failed: Exception while decompiling 80112264: process: timeout


/* ======= FUN_801123d8 @ 0x801123d8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123d8(void)

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
      FUN_80112e04();
      return;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  FUN_80112e04();
  return;
}



/* ======= FUN_801125cc @ 0x801125cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801125cc(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  DAT_800acae7 = DAT_800acae7 & 0xfe;
  return;
}



/* ======= FUN_80112620 @ 0x80112620 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112620(void)

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



/* ======= FUN_80112758 @ 0x80112758 ======= */
// decompile failed: Exception while decompiling 80112758: process: timeout


/* ======= FUN_80112a48 @ 0x80112a48 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a48(void)

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



/* ======= FUN_80112b00 @ 0x80112b00 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112b00(void)

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



/* ======= FUN_80112c38 @ 0x80112c38 ======= */
// decompile failed: Exception while decompiling 80112c38: process: timeout


/* ======= FUN_80112e04 @ 0x80112e04 ======= */

void FUN_80112e04(void)

{
  int in_at;
  int in_v0;
  
                    /* WARNING: Could not recover jumptable at 0x8011368c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(in_at + 0x30c + in_v0))();
  return;
}



/* ======= FUN_80113278 @ 0x80113278 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113278(void)

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



/* ======= FUN_8011339c @ 0x8011339c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011339c(void)

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



/* ======= FUN_8011353c @ 0x8011353c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011353c(void)

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



/* ======= FUN_80113648 @ 0x80113648 ======= */
// decompile failed: Exception while decompiling 80113648: process: timeout


/* ======= FUN_8011391c @ 0x8011391c ======= */
// decompile failed: Exception while decompiling 8011391c: process: timeout


/* ======= FUN_80113c94 @ 0x80113c94 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c94(void)

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
    FUN_801145b0();
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



/* ======= FUN_80113dc8 @ 0x80113dc8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113dc8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80114760();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_80114760();
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
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x8011a158);
      *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80115bec(0,1);
  FUN_80114760();
  return;
}



/* ======= FUN_80113f70 @ 0x80113f70 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f70(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  byte bVar5;
  
  if (((((short)_DAT_800ac784[0x77] == 0) ||
       (*(char *)(*(byte *)((int)_DAT_800ac784 + 5) + 0x8011a1a8) == '\0')) ||
      (*(char *)((int)_DAT_800ac784 + 6) != '\x01')) ||
     ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0)) {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee5e40))();
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
      goto LAB_801149cc;
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
LAB_801149cc:
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



/* ======= FUN_80114050 @ 0x80114050 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114050(undefined4 param_1,uint param_2)

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
      goto LAB_80114a30;
    }
    if (bVar1 != 0) goto LAB_80114a30;
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
    local_20 = *(undefined4 *)(iVar3 + -0x7fee5e98);
    local_1c = *(undefined4 *)(iVar3 + -0x7fee5e94);
    local_18 = *(undefined4 *)(iVar3 + -0x7fee5e90);
    local_14 = *(undefined4 *)(iVar3 + -0x7fee5e8c);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  puVar4 = (uint *)0x0;
  param_2 = 1;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  uVar5 = FUN_80115bec();
LAB_80114a30:
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



/* ======= FUN_80114240 @ 0x80114240 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114240(void)

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
      goto LAB_80114c58;
    }
    if (bVar5 != 0) goto LAB_80114c58;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 8;
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 9;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)((int)_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fee5e98);
    local_1c = *(undefined4 *)(iVar3 + -0x7fee5e94);
    local_18 = *(undefined4 *)(iVar3 + -0x7fee5e90);
    local_14 = *(undefined4 *)(iVar3 + -0x7fee5e8c);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  if (((*(byte *)((int)_DAT_800ac784 + 0x95) < 0xf) || ((char)_DAT_800ac784[0x25] != '\b')) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x95) < 0x11 || ((char)_DAT_800ac784[0x25] != '\t')))) {
    puVar4 = (uint *)0x0;
    FUN_80115bec(0,1);
  }
  else {
    puVar4 = (uint *)0x0;
    FUN_80115bec(0,0);
  }
LAB_80114c58:
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



/* ======= FUN_80114468 @ 0x80114468 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114468(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5de8))();
  return;
}



/* ======= FUN_801144b0 @ 0x801144b0 ======= */
// decompile failed: Exception while decompiling 801144b0: process: timeout


/* ======= FUN_801145b0 @ 0x801145b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801145b0(undefined4 param_1)

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
  uStack00000014 = *(undefined4 *)(in_v0 + -0x7fee5e94);
  uStack00000018 = *(undefined4 *)(in_v0 + -0x7fee5e90);
  uStack0000001c = *(undefined4 *)(in_v0 + -0x7fee5e8c);
  func_0x80019700(param_1,(int)*(short *)(in_v1 + 0x6a),*(int *)(in_v1 + 0x188) + 0x40);
  func_0x800453d0(2);
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  bVar1 = *(byte *)(_DAT_800ac784 + 0x95);
  if (((bVar1 < 0xf) || (bVar3 = bVar1 < 0x11, *(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
     ((bVar3 = true, bVar1 < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
    FUN_80115bec(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x18') {
      return;
    }
    func_0x800453d0(7);
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  else {
    unaff_s0 = (uint)bVar3 * 8;
    func_0x80019700(0,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)*(byte *)(unaff_s0 + -0x7fee5d78) * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40);
    func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)*(byte *)(unaff_s0 + -0x7fee5d77) * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40,&stack0x00000020);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)*(byte *)(unaff_s0 + -0x7fee5d76) * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40);
  }
  func_0x80019700();
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d74) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d73) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d72) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d71) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
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



/* ======= FUN_80114760 @ 0x80114760 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114760(void)

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
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d74) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d73) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d72) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d71) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
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



/* ======= FUN_801147e4 @ 0x801147e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801147e4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5d90))();
  return;
}



/* ======= FUN_8011482c @ 0x8011482c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011482c(void)

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
      goto LAB_801149cc;
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
LAB_801149cc:
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



/* ======= FUN_80114aec @ 0x80114aec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114aec(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  byte bVar4;
  
  bVar4 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        FUN_8011559c();
        return;
      }
      goto LAB_80114c8c;
    }
    if (bVar4 != 0) {
      FUN_8011559c();
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
LAB_80114c8c:
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



/* ======= FUN_80114dac @ 0x80114dac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114dac(void)

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
                  (uint)*(byte *)(iVar3 + -0x7fee5d78) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d77) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_30);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d76) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d75) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d74) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d73) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d72) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(iVar3 + -0x7fee5d71) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
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



/* ======= FUN_801153ec @ 0x801153ec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801153ec(undefined4 param_1,int param_2)

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



/* ======= FUN_8011559c @ 0x8011559c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011559c(void)

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
  FUN_80115fdc();
  return;
}



/* ======= FUN_80115bec @ 0x80115bec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115bec(void)

{
  int iVar1;
  char cVar2;
  int in_v1;
  int iVar3;
  uint *puVar4;
  
  cVar2 = *(char *)(in_v1 + -0x3517);
  if (*(char *)(in_v1 + -0x3517) == '<') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a2a8);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    *(uint *)(_DAT_800acbdc + 0x560) = *(uint *)(_DAT_800acbdc + 0x560) ^ 0x1062;
    *(undefined1 *)(_DAT_800acbdc + 0x5d5) = 0x14;
    *(undefined2 *)(_DAT_800acbdc + 0x5e2) = 0xff80;
    *(undefined1 *)(_DAT_800acbdc + 0x5d6) = 8;
    *(undefined2 *)(_DAT_800acbdc + 0x598) = 0;
    *(undefined2 *)(_DAT_800acbdc + 0x59a) = 0xff9c;
    *(undefined2 *)(_DAT_800acbdc + 0x59c) = 0;
    puVar4 = *(uint **)(_DAT_800ac784 + 0x188);
    puVar4[0x25] = 0x80;
    puVar4[0x26] = 0x20;
    *puVar4 = *puVar4 | 0x80;
    cVar2 = DAT_800acae9;
  }
  if (cVar2 == '=') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a2a8);
  }
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  iVar3 = (uint)DAT_800aca5a + iVar1;
  DAT_800aca5a = (byte)iVar3;
  *(short *)(iVar3 + 0x5fc) = (short)iVar1;
  *(undefined2 *)(iVar3 + 0x5f4) = 0;
  *(undefined2 *)(iVar3 + 0x5f6) = 0;
  *(undefined2 *)(iVar3 + 0x5fe) = 0x138;
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



/* ======= FUN_80115fdc @ 0x80115fdc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115fdc(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),_DAT_800acbdc + 0x5a0);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  (*_DAT_800b0000)();
  return;
}



/* ======= FUN_801160c4 @ 0x801160c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801160c4(void)

{
  if ((_DAT_800aca40 & 0x20000000) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5d40))();
  }
  return;
}



/* ======= FUN_80116120 @ 0x80116120 ======= */

void FUN_80116120(uint *param_1)

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



/* ======= FUN_801161b8 @ 0x801161b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161b8(int param_1)

{
  int iVar1;
  uint uVar2;
  uint extraout_v1;
  
  uVar2 = (uint)*(byte *)(param_1 + 5);
  if (uVar2 != 1) {
    if (uVar2 < 2) {
      if (uVar2 == 0) {
        iVar1 = func_0x8004efe4(0x800b1028,0x1f);
        if (iVar1 == 0) {
          return;
        }
        *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
        func_0x8004efb8(0x800b1028,0x1f);
        uVar2 = extraout_v1;
      }
    }
    else if (uVar2 == 2) goto LAB_80116258;
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
  *(undefined1 *)(param_1 + 0x94) = 0;
  *(undefined1 *)(param_1 + 0x95) = 0;
  *(undefined1 *)(param_1 + 0x8f) = 0;
  *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
LAB_80116258:
  iVar1 = *(int *)(param_1 + 0x188);
  if (*(byte *)(param_1 + 0x95) < 10) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xb;
  }
  if (*(byte *)(param_1 + 0x95) - 10 < 0xd) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x17 < 7) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xb;
  }
  if (*(char *)(param_1 + 0x95) == '(') {
    *(int *)(iVar1 + 0x188) = *(int *)(iVar1 + 0x188) + -0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x29 < 7) {
    *(int *)(iVar1 + 0x188) = *(int *)(iVar1 + 0x188) + -0xd;
  }
  if (*(byte *)(param_1 + 0x95) - 0x2f < 7) {
    *(int *)(iVar1 + 0x2e0) = *(int *)(iVar1 + 0x2e0) + 0xc;
    *(int *)(iVar1 + 0x438) = *(int *)(iVar1 + 0x438) + 0xc;
    *(int *)(iVar1 + 0x590) = *(int *)(iVar1 + 0x590) + 0xc;
    *(int *)(iVar1 + 0x6e8) = *(int *)(iVar1 + 0x6e8) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x36 < 6) {
    *(int *)(iVar1 + 0x2e0) = *(int *)(iVar1 + 0x2e0) + 0xb;
    *(int *)(iVar1 + 0x438) = *(int *)(iVar1 + 0x438) + 0xb;
    *(int *)(iVar1 + 0x590) = *(int *)(iVar1 + 0x590) + 0xb;
    *(int *)(iVar1 + 0x6e8) = *(int *)(iVar1 + 0x6e8) + 0xb;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  return;
}



/* ======= FUN_801163e4 @ 0x801163e4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801163e4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5d28))();
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



/* ======= FUN_80116520 @ 0x80116520 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116520(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011a2d4;
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



/* ======= FUN_801166c8 @ 0x801166c8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166c8(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee5d14))();
  return;
}



/* ======= FUN_80116794 @ 0x80116794 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116794(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5d10))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5cf0))();
  return;
}



/* ======= FUN_80116810 @ 0x80116810 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116810(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5cd0))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5cb0))();
  return;
}



/* ======= FUN_8011688c @ 0x8011688c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011688c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5c90))();
  return;
}



/* ======= FUN_801168d4 @ 0x801168d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801168d4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5c90))();
  return;
}



/* ======= FUN_8011691c @ 0x8011691c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011691c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5c90))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_801169b4 @ 0x801169b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801169b4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5c58))();
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



/* ======= FUN_80116af0 @ 0x80116af0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116af0(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011a3a4;
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



/* ======= FUN_80116c98 @ 0x80116c98 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116c98(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee5c44))();
  return;
}



/* ======= FUN_80116d64 @ 0x80116d64 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d64(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5c40))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5c20))();
  return;
}



/* ======= FUN_80116de0 @ 0x80116de0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116de0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5c00))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5be0))();
  return;
}



/* ======= FUN_80116e5c @ 0x80116e5c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116e5c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5bc0))();
  return;
}



/* ======= FUN_80116ea4 @ 0x80116ea4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116ea4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5bc0))();
  return;
}



/* ======= FUN_80116eec @ 0x80116eec ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116eec(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5bc0))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80116f84 @ 0x80116f84 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116f84(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5b88))();
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



/* ======= FUN_801170b4 @ 0x801170b4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801170b4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011a474;
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



/* ======= FUN_8011725c @ 0x8011725c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011725c(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee5b74))();
  return;
}



/* ======= FUN_80117328 @ 0x80117328 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117328(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5b70))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5b50))();
  return;
}



/* ======= FUN_801173a4 @ 0x801173a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801173a4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5b30))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5b10))();
  return;
}



/* ======= FUN_80117420 @ 0x80117420 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117420(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5af0))();
  return;
}



/* ======= FUN_80117468 @ 0x80117468 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117468(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5af0))();
  return;
}



/* ======= FUN_801174b0 @ 0x801174b0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801174b0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5af0))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80117548 @ 0x80117548 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117548(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5ab8))();
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



/* ======= FUN_8011768c @ 0x8011768c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011768c(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_8011a544;
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



/* ======= FUN_80117834 @ 0x80117834 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117834(void)

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
    FUN_80118150();
    return;
  }
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee5aa4))();
  return;
}



/* ======= FUN_80117960 @ 0x80117960 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117960(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a9c))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a7c))();
  return;
}



/* ======= FUN_801179dc @ 0x801179dc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801179dc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a5c))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a3c))();
  return;
}



/* ======= FUN_80117a58 @ 0x80117a58 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117a58(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a1c))();
  return;
}



/* ======= FUN_80117aa0 @ 0x80117aa0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117aa0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee5a08))();
  return;
}



/* ======= FUN_80117ae8 @ 0x80117ae8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117ae8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee5a08))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80117b80 @ 0x80117b80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117b80(void)

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
  _DAT_80072c50 = &LAB_8010c1a0;
  _DAT_80072c58 = 0x801118d0;
  _DAT_80072c60 = 0x801168c4;
  _DAT_80072cac = 0x80116be4;
  _DAT_80072cb4 = 0x801171b4;
  _DAT_80072cd0 = 0x80117784;
  _DAT_80072cd8 = 0x80117d48;
  _DAT_800b52c8 = &LAB_80118590;
  _DAT_800ac998 = &LAB_801186f0;
  _DAT_80072bf8 = 0x8010a87c;
  _DAT_800ac798 = 0x8010a240;
  _DAT_800ac79c = 0x8010a240;
  _DAT_800ac7a0 = 0x8010a240;
  _DAT_800ac7b0 = 0x8010a240;
  _DAT_800ac7b8 = 0x8010a240;
  _DAT_800ac7c8 = 0x8010a240;
  _DAT_800ac7cc = 0x8010a240;
  _DAT_800ac7d0 = 0x8010a240;
  _DAT_800ac7d4 = 0x8010a240;
  _DAT_800ac7fc = 0x801113d0;
  _DAT_800ac804 = 0x80115cc0;
  _DAT_800ac7a4 = &LAB_8010c120;
  _DAT_800ac898 = 0x8010a66c;
  _DAT_800ac89c = 0x8010a66c;
  _DAT_800ac8a0 = 0x8010a66c;
  _DAT_800ac8b0 = 0x8010a66c;
  _DAT_800ac8b8 = 0x8010a66c;
  _DAT_800ac8c8 = 0x8010a66c;
  _DAT_800ac8cc = 0x8010a66c;
  _DAT_800ac8d0 = 0x8010a66c;
  _DAT_800ac8d4 = 0x8010a66c;
  _DAT_800ac8fc = 0x80111704;
  _DAT_800ac8a4 = &LAB_8010c160;
  _DAT_800ac904 = 0x801161dc;
  func_0x80029afc();
  return;
}



/* ======= FUN_80118150 @ 0x80118150 ======= */

/* WARNING: Control flow encountered bad instruction data */

void FUN_80118150(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



