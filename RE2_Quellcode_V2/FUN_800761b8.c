/* FUN_800761b8 @ 0x800761b8  (Ghidra headless, raw MIPS overlay) */

void FUN_800761b8(void)

{
  ushort uVar1;
  byte bVar2;
  undefined1 uVar3;
  u_short uVar4;
  undefined *p;
  undefined *ot;
  
  ot = &UNK_800cc1fc + (uint)DAT_800ce5e0 * 0x20;
  uVar1 = *(ushort *)
           (&DAT_800aa144 + (uint)(byte)(&DAT_800d4b68)[DAT_800d5c01 * 8 + (int)DAT_800d5c02] * 4);
  p = &DAT_8019c100;
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019c114;
  }
  *(undefined2 *)(p + 8) = DAT_800d5c4c;
  *(undefined2 *)(p + 10) = DAT_800d5c4e;
  AddPrim(ot,p);
  AddPrim(ot,&UNK_800d6c08 + (uint)DAT_800ce5e0 * 0xc);
  *(undefined2 *)(p + 0x30) = 100;
  *(undefined2 *)(p + 0x32) = 0x3c;
  AddPrim(ot,p + 0x28);
  AddPrim(ot,&UNK_800d6ba8 + (uint)DAT_800ce5e0 * 0xc);
  if (DAT_800d5c03 == uVar1) {
    *(undefined2 *)(p + 0x60) = 0x2a;
    *(undefined2 *)(p + 0x62) = 0xe;
    p[0x5c] = 0x38;
    p[0x5d] = 0xc;
    *(undefined2 *)(p + 0x58) = 0x118;
    *(undefined2 *)(p + 0x5a) = 0x6e;
    uVar4 = GetClut(0x100,0x1ea);
    *(u_short *)(p + 0x5e) = uVar4;
    uVar3 = 0x30;
    if (DAT_800d5bf2 == 0xe) {
      uVar3 = 0x80;
    }
  }
  else {
    *(undefined2 *)(p + 0x60) = 0xc;
    *(undefined2 *)(p + 0x62) = 0xd;
    p[0x5c] = 0x2a;
    p[0x5d] = 0xc;
    bVar2 = DAT_800d5c19;
    *(undefined2 *)(p + 0x5a) = 0x6e;
    *(ushort *)(p + 0x58) = (ushort)bVar2 * 3 + 0x11a;
    uVar4 = GetClut(0x100,0x1ec);
    *(u_short *)(p + 0x5e) = uVar4;
    uVar3 = 0x80;
    if ((DAT_800d5bf2 != 0xd) && (uVar3 = 0x80, DAT_800d5c19 == 0)) {
      uVar3 = 0x40;
    }
  }
  p[0x54] = uVar3;
  p[0x55] = uVar3;
  p[0x56] = uVar3;
  if (DAT_800d5bf2 - 0xd < 2) {
    AddPrim(ot,p + 0x50);
  }
  if ((DAT_800d5bf2 == 0xd) || (uVar3 = 0x40, DAT_800d5c19 != 0)) {
    uVar3 = 0x80;
  }
  p[0x7c] = uVar3;
  p[0x7d] = uVar3;
  p[0x7e] = uVar3;
  bVar2 = DAT_800d5c19;
  *(undefined2 *)(p + 0x82) = 0x6e;
  *(ushort *)(p + 0x80) = (ushort)bVar2 * -3 + 0xc;
  if ((DAT_800d5bf2 == 0xd) && (DAT_800d5c03 != 0)) {
    AddPrim(ot,p + 0x78);
  }
  AddPrim(ot,&UNK_800d6c20 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


