/* ======= FUN_80100000 @ 0x80100000 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
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



/* ======= FUN_801000b8 @ 0x801000b8 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801000b8(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fefdae0))();
  return;
}



/* ======= FUN_80100184 @ 0x80100184 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100184(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefdadc))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefdabc))();
  return;
}



/* ======= FUN_80100200 @ 0x80100200 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100200(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefda9c))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefda7c))();
  return;
}



/* ======= FUN_8010027c @ 0x8010027c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010027c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefda5c))();
  return;
}



/* ======= FUN_801002c4 @ 0x801002c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801002c4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefda5c))();
  return;
}



/* ======= FUN_8010030c @ 0x8010030c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010030c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefda5c))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_801003a4 @ 0x801003a4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801003a4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fefda24))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(0x800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_801004d4 @ 0x801004d4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801004d4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_801025d8;
  _DAT_800ac784[0x6a] = 0x800aca54;
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



/* ======= FUN_8010067c @ 0x8010067c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010067c(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fefda10))();
  return;
}



/* ======= FUN_80100748 @ 0x80100748 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100748(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefda0c))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd9ec))();
  return;
}



/* ======= FUN_801007c4 @ 0x801007c4 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801007c4(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd9cc))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd9ac))();
  return;
}



/* ======= FUN_80100840 @ 0x80100840 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100840(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefd98c))();
  return;
}



/* ======= FUN_80100888 @ 0x80100888 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100888(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefd98c))();
  return;
}



/* ======= FUN_801008d0 @ 0x801008d0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801008d0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd98c))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80100968 @ 0x80100968 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100968(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fefd954))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(0x800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  *(undefined2 *)(_DAT_800ac784 + 0x1c8) = 0;
  return;
}



/* ======= FUN_80100aac @ 0x80100aac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100aac(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_801026a8;
  _DAT_800ac784[0x6a] = 0x800aca54;
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



/* ======= FUN_80100c54 @ 0x80100c54 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100c54(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x578,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    (**(code **)((uint)DAT_00000007 * 4 + -0x7fefd7d4))();
    return;
  }
  uVar2 = func_0x8005070c(700,(int)local_10[0],-(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  if (((*(byte *)(_DAT_800ac784 + 0x1c2) & 1) != 0) && (100 < _DAT_800acae0)) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_80101570();
    return;
  }
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fefd940))();
  return;
}



/* ======= FUN_80100d80 @ 0x80100d80 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100d80(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd938))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd918))();
  return;
}



/* ======= FUN_80100dfc @ 0x80100dfc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100dfc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd8f8))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd8d8))();
  return;
}



/* ======= FUN_80100e78 @ 0x80100e78 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e78(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd8b8))();
  return;
}



/* ======= FUN_80100ec0 @ 0x80100ec0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100ec0(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefd8a4))();
  return;
}



/* ======= FUN_80100f08 @ 0x80100f08 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f08(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd8a4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80100fa0 @ 0x80100fa0 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fa0(void)

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
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fefd86c))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(0x800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}



/* ======= FUN_80101118 @ 0x80101118 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101118(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = _DAT_80102790;
  _DAT_800ac784[0x6a] = 0x800aca54;
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



/* ======= FUN_801012c0 @ 0x801012c0 ======= */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801012c0(void)

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
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fefd858))();
  return;
}



/* ======= FUN_8010138c @ 0x8010138c ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010138c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd854))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd834))();
  return;
}



/* ======= FUN_80101408 @ 0x80101408 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101408(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd814))();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd7f4))();
  return;
}



/* ======= FUN_80101484 @ 0x80101484 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101484(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefd7d4))();
  return;
}



/* ======= FUN_801014cc @ 0x801014cc ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801014cc(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fefd7d4))();
  return;
}



/* ======= FUN_80101514 @ 0x80101514 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101514(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fefd7d4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_80101570 @ 0x80101570 ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101570(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ======= FUN_801015ac @ 0x801015ac ======= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801015ac(void)

{
  _DAT_80072cac = 0x80100004;
  _DAT_80072cb4 = 0x801005d4;
  _DAT_80072cd0 = 0x80100ba4;
  _DAT_80072cd8 = 0x80101168;
  _DAT_80072ce0 = &DAT_801017a0;
  _DAT_800b52c8 = &LAB_80101e3c;
  _DAT_800ac998 = &LAB_80101edc;
  func_0x80029afc();
  return;
}



