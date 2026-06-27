/* FUN_80074160 @ 0x80074160  (Ghidra headless, raw MIPS overlay) */

void FUN_80074160(int param_1)

{
  short sVar1;
  short sVar2;
  int iVar3;
  short *psVar4;
  undefined *p;
  int iVar5;
  undefined *p_00;
  undefined *ot;
  short sVar6;
  
  sVar2 = DAT_800d5c3e;
  sVar1 = DAT_800d5c3c;
  p_00 = &UNK_80198c00;
  p = &DAT_80198c80;
  iVar5 = 6;
  ot = &DAT_800cc200 + (uint)DAT_800ce5e0 * 0x20;
  if (param_1 == 0) {
    sVar6 = 0x5a;
    DAT_800ab02e = 0x55;
  }
  else {
    iVar5 = 0x11;
    sVar6 = 0x93;
    DAT_800ab02e = 0x91;
  }
  if (DAT_800ce5e0 != 0) {
    p_00 = &DAT_80198c18;
    p = &DAT_80198c94;
  }
  psVar4 = (short *)(p + 10);
  iVar3 = iVar5 << 2;
  DAT_800ab032 = DAT_800ab02e;
  do {
    iVar5 = iVar5 + -1;
    psVar4[-1] = *(short *)(&DAT_800ab020 + iVar3) + sVar1;
    *psVar4 = *(short *)(&DAT_800ab022 + iVar3) + sVar2;
    AddPrim(ot,p);
    psVar4 = psVar4 + 0x14;
    iVar3 = iVar3 + -4;
    p = p + 0x28;
  } while (iVar5 != 0);
  *(short *)(p_00 + 8) = sVar1;
  *(short *)(p_00 + 10) = sVar2;
  *(short *)(p_00 + 0xc) = sVar1 + 0xd2;
  *(short *)(p_00 + 0xe) = sVar2;
  *(short *)(p_00 + 0x10) = sVar1;
  *(short *)(p_00 + 0x12) = sVar2 + sVar6;
  *(short *)(p_00 + 0x14) = sVar1 + 0xd2;
  *(short *)(p_00 + 0x16) = sVar2 + sVar6;
  if (param_1 == 0) {
    AddPrim(ot,p_00);
  }
  AddPrim(ot,&UNK_800d6a58 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


