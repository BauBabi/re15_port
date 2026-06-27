/* ============ FUN_80100140 @ 0x80100140 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100140(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = DAT_80102438;
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



/* ============ FUN_801002e8 @ 0x801002e8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801002e8(void)

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
  (*(code *)(&PTR_FUN_80102450)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_801003b4 @ 0x801003b4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801003b4(void)

{
  (**(code **)(&DAT_80102454 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102474 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80100430 @ 0x80100430 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100430(void)

{
  (**(code **)(&DAT_80102494 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801024b4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801004ac @ 0x801004ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801004ac(void)

{
  (**(code **)(&DAT_801024d4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801004f4 @ 0x801004f4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801004f4(void)

{
  (**(code **)(&DAT_801024d4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_8010053c @ 0x8010053c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010053c(void)

{
  (**(code **)(&DAT_801024d4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_801005d4 @ 0x801005d4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801005d4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8010250c)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80100710 @ 0x80100710 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100710(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80102508;
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



/* ============ FUN_801008b8 @ 0x801008b8 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801008b8(void)

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
  (*(code *)(&PTR_FUN_80102520)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80100984 @ 0x80100984 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100984(void)

{
  (**(code **)(&DAT_80102524 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102544 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80100a00 @ 0x80100a00 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a00(void)

{
  (**(code **)(&DAT_80102564 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102584 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80100a7c @ 0x80100a7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a7c(void)

{
  (**(code **)(&DAT_801025a4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80100ac4 @ 0x80100ac4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100ac4(void)

{
  (**(code **)(&DAT_801025a4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80100b0c @ 0x80100b0c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100b0c(void)

{
  (**(code **)(&DAT_801025a4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80100ba4 @ 0x80100ba4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100ba4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x8001bd60(0xfffffff6,0x14);
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_801025dc)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80100cd4 @ 0x80100cd4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100cd4(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_801025d8;
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



/* ============ FUN_80100e7c @ 0x80100e7c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e7c(void)

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
  (*(code *)(&PTR_FUN_801025f0)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80100f48 @ 0x80100f48 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f48(void)

{
  (**(code **)(&DAT_801025f4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102614 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80100fc4 @ 0x80100fc4 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100fc4(void)

{
  (**(code **)(&DAT_80102634 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102654 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80101040 @ 0x80101040 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101040(void)

{
  (**(code **)(&DAT_80102674 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80101088 @ 0x80101088 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101088(void)

{
  (**(code **)(&DAT_80102674 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_801010d0 @ 0x801010d0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801010d0(void)

{
  (**(code **)(&DAT_80102674 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80101168 @ 0x80101168 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101168(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_801026ac)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_801012ac @ 0x801012ac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801012ac(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_801026a8;
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



/* ============ FUN_80101454 @ 0x80101454 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101454(void)

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
    (*(code *)(&PTR_FUN_801026c0)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}



/* ============ FUN_80101580 @ 0x80101580 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101580(void)

{
  (**(code **)(&DAT_801026c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801026e8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801015fc @ 0x801015fc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801015fc(void)

{
  (**(code **)(&DAT_80102708 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102728 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80101678 @ 0x80101678 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101678(void)

{
  (**(code **)(&DAT_80102748 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_801016c0 @ 0x801016c0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801016c0(void)

{
  (**(code **)(&DAT_8010275c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80101708 @ 0x80101708 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101708(void)

{
  (**(code **)(&DAT_8010275c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_801017a0 @ 0x801017a0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801017a0(void)

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
    (*(code *)(&PTR_FUN_80102794)[*(byte *)(_DAT_800ac784 + 4)])();
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



/* ============ FUN_80101918 @ 0x80101918 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101918(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80102790;
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



/* ============ FUN_80101ac0 @ 0x80101ac0 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ac0(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(1000,(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  (*(code *)(&PTR_FUN_801027a8)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  return;
}



/* ============ FUN_80101b8c @ 0x80101b8c ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b8c(void)

{
  (**(code **)(&DAT_801027ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801027cc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80101c08 @ 0x80101c08 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c08(void)

{
  (**(code **)(&DAT_801027ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8010280c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}



/* ============ FUN_80101c84 @ 0x80101c84 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c84(void)

{
  (**(code **)(&DAT_8010282c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80101ccc @ 0x80101ccc ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101ccc(void)

{
  (**(code **)(&DAT_8010282c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
  return;
}



/* ============ FUN_80101d14 @ 0x80101d14 ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101d14(void)

{
  (**(code **)(&DAT_8010282c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}



/* ============ FUN_80101dac @ 0x80101dac ============ */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101dac(void)

{
  _DAT_80072cac = &LAB_80100004;
  _DAT_80072cb4 = FUN_801005d4;
  _DAT_80072cd0 = FUN_80100ba4;
  _DAT_80072cd8 = FUN_80101168;
  _DAT_80072ce0 = FUN_801017a0;
  _DAT_800b52c8 = &DAT_80101e3c;
  _DAT_800ac998 = &DAT_80101edc;
  func_0x80029afc();
  return;
}



