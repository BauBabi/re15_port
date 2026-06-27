/* FUN_80073350 @ 0x80073350  (Ghidra headless, raw MIPS overlay) */

void FUN_80073350(int param_1)

{
  short sVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  undefined *p;
  int iVar6;
  short sVar7;
  short sVar8;
  
  p = &UNK_8019bc00;
  iVar6 = 3;
  sVar8 = DAT_800d5c24 + DAT_800d5c0e;
  uVar3 = (uint)DAT_800ce5e0;
  if (DAT_800d5bf2 == '\0') {
    if (DAT_800d5c19 == '\0') {
      cVar2 = DAT_800d5c18 + 3;
      if (0x78 < DAT_800d5c18) {
        DAT_800d5c19 = '\x01';
        cVar2 = DAT_800d5c18 + 3;
      }
    }
    else {
      cVar2 = DAT_800d5c18 - 3;
      if (DAT_800d5c18 < 0x32) {
        DAT_800d5c19 = '\0';
      }
    }
  }
  else {
    cVar2 = '<';
  }
  if (DAT_800d5bfc == 10) {
    sVar1 = 0x28;
    sVar7 = DAT_800d5c26 + DAT_800d5c0f + -0x26;
  }
  else {
    sVar1 = (DAT_800d5bfc & 1) * 0x28;
    sVar7 = DAT_800d5c26 + DAT_800d5c0f + (ushort)(DAT_800d5bfc >> 1) * 0x1e;
  }
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019bc14;
  }
  pcVar5 = p + 6;
  DAT_800d5c18 = cVar2;
  do {
    iVar6 = iVar6 + -1;
    iVar4 = (iVar6 + param_1 * 3) * 4;
    *(short *)(pcVar5 + 2) = *(short *)(&DAT_800aae4c + iVar4) + sVar8 + sVar1;
    *(short *)(pcVar5 + 4) = *(short *)(&DAT_800aae4e + iVar4) + sVar7;
    pcVar5[-2] = DAT_800d5c18;
    pcVar5[-1] = DAT_800d5c18;
    *pcVar5 = DAT_800d5c18;
    AddPrim(&UNK_800cc1f0 + uVar3 * 0x20,p);
    pcVar5 = pcVar5 + 0x28;
    p = p + 0x28;
  } while (iVar6 != 0);
  AddPrim(&UNK_800cc1f0 + uVar3 * 0x20,&UNK_800d6ab8 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


