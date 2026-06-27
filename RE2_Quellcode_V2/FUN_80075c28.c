/* FUN_80075c28 @ 0x80075c28  (Ghidra headless, raw MIPS overlay) */

void FUN_80075c28(int param_1)

{
  short sVar1;
  short sVar2;
  undefined *p;
  undefined *p_00;
  short sVar3;
  short sVar4;
  undefined *ot;
  
  p = &DAT_8019c200;
  p_00 = &DAT_8019c000;
  ot = &UNK_800cc1f4 + (uint)DAT_800ce5e0 * 0x20;
  sVar3 = DAT_800d5c3c + DAT_800dcba4;
  sVar4 = DAT_800d5c3e + DAT_800dcba6;
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019c214;
  }
  sVar2 = DAT_800d5c3c + 0x10;
  if (param_1 == 0) {
    sVar2 = DAT_800d5c3c + 0x32;
  }
  *(short *)(p + 8) = sVar2;
  *(short *)(p + 10) = DAT_800d5c3e + 10;
  AddPrim(ot,p);
  AddPrim(ot,&UNK_800d6ba8 + (uint)DAT_800ce5e0 * 0xc);
  sVar1 = DAT_800d783e;
  sVar2 = DAT_800d7834;
  if (param_1 != 0) {
    if (DAT_800ce5e0 != 0) {
      p_00 = &DAT_8019c028;
    }
    *(short *)(p_00 + 8) = sVar3;
    *(short *)(p_00 + 10) = sVar4;
    *(short *)(p_00 + 0x12) = sVar4;
    *(short *)(p_00 + 0x18) = sVar3;
    *(short *)(p_00 + 0x10) = sVar2 + sVar3;
    *(short *)(p_00 + 0x1a) = sVar1 + sVar4;
    *(short *)(p_00 + 0x20) = sVar2 + sVar3;
    *(short *)(p_00 + 0x22) = sVar1 + sVar4;
    if (DAT_800d5bf2 == '\x02') {
      AddPrim(ot,p_00);
    }
    AddPrim(ot,&UNK_800d6aa0 + (uint)DAT_800ce5e0 * 0xc);
  }
  return;
}


