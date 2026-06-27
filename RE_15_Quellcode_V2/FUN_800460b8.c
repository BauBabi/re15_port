void FUN_800460b8(void)

{
  int iVar1;
  int iVar2;
  
  FUN_80029bf8(0);
  FUN_80030640(0xf000,0xf,4);
  DAT_800b5456 = 0;
  DrawSync(0);
  InitGeom();
  FUN_80066d60(0xa0,0x78);
  FUN_80066c30(*(ushort *)(DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24) + 2) >> 7);
  FUN_80053ca4(DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24));
  DAT_800b2610 = GetClut(0x100,0x1e8);
  DAT_800b2612 = GetClut(0x100,0x1e9);
  DAT_800b2614 = GetClut(0x100,0x1ea);
  DAT_800b2616 = GetClut(0x100,0x1eb);
  DAT_800b2618 = GetClut(0x100,0x1ec);
  DAT_800b261a = GetClut(0x100,0x1ed);
  DAT_800b261c = GetClut(0x100,0x1ee);
  DAT_800b261e = GetClut(0x100,0x1ef);
  iVar1 = GetGraphType();
  iVar2 = 0x22a;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x9a;
    if (iVar1 == 2) {
      iVar2 = 0x22a;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b2620,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x22a;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x9a;
    if (iVar1 == 2) {
      iVar2 = 0x22a;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b262c,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x2b;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x1b;
    if (iVar1 == 2) {
      iVar2 = 0x2b;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b2638,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x2b;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x1b;
    if (iVar1 == 2) {
      iVar2 = 0x2b;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b2644,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x27;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x17;
    if (iVar1 == 2) {
      iVar2 = 0x27;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b2650,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x27;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0x17;
    if (iVar1 == 2) {
      iVar2 = 0x27;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_800b265c,0,0,iVar2,(RECT *)0x0);
  DAT_800b25e4 = 0xd;
  DAT_800b25e6 = 0x52;
  DAT_800b25d8 = 0x7e;
  DAT_800b25f0 = 0xe;
  DAT_800b25f4 = 0xd7;
  DAT_800b25da = 0x1a;
  DAT_800b25f2 = 0x1a;
  DAT_800b25bc = 0;
  DAT_800b25bd = 0;
  DAT_800b25be = 0;
  DAT_800b25d6 = 0;
  DAT_800b25d7 = 0;
  DAT_800b25f6 = 0xa4;
  DAT_800b25ce = (&DAT_800b10ac)[(uint)DAT_800b25c8 * 4];
  DAT_800b2600 = 0x20;
  DAT_800b25fe = 0x20;
  DAT_800b25ca = 0;
  DAT_800b25fc = 0x6a;
  DAT_800b2602 = 0;
  DAT_800b2603 = 0;
  DAT_800b25d4 = 0;
  DAT_800b25d5 = 0;
  DAT_800b25d0 = 0;
  DAT_800b25d1 = 0;
  DAT_800b25d2 = 0;
  DAT_800b25d3 = 0;
  DAT_800b2668._0_1_ = 0;
  DAT_800b2668._1_1_ = 0;
  FUN_8004dadc();
  (*(code *)(&PTR_LAB_80074be8)[(byte)DAT_800b25c0])();
  DAT_800b25bf = DAT_800b25bf + '\x01';
  return;
}
