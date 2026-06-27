/* FUN_80073674 @ 0x80073674  (Ghidra headless, raw MIPS overlay) */

void FUN_80073674(int param_1)

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
  
  p = &UNK_8019bd00;
  iVar6 = 2;
  sVar8 = DAT_800d5c24 + DAT_800d5c12;
  uVar3 = (uint)DAT_800ce5e0;
  if (DAT_800d5c1b == '\0') {
    cVar2 = DAT_800d5c1a + 3;
    if (0x7c < DAT_800d5c1a) {
      DAT_800d5c1b = '\x01';
      cVar2 = DAT_800d5c1a + 3;
    }
  }
  else {
    cVar2 = DAT_800d5c1a - 3;
    if (DAT_800d5c1a < 0x46) {
      DAT_800d5c1b = '\0';
    }
  }
  if (DAT_800d5bfd == 10) {
    sVar1 = 0x28;
    sVar7 = DAT_800d5c26 + DAT_800d5c13 + -0x26;
  }
  else {
    sVar1 = (DAT_800d5bfd & 1) * 0x28;
    sVar7 = DAT_800d5c26 + DAT_800d5c13 + (ushort)(DAT_800d5bfd >> 1) * 0x1e;
  }
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019bd14;
  }
  pcVar5 = p + 6;
  DAT_800d5c1a = cVar2;
  do {
    iVar6 = iVar6 + -1;
    iVar4 = (iVar6 + param_1 * 3) * 4;
    *(short *)(pcVar5 + 2) = *(short *)(&DAT_800aae7c + iVar4) + sVar8 + sVar1;
    *(short *)(pcVar5 + 4) = *(short *)(&DAT_800aae7e + iVar4) + sVar7;
    pcVar5[-2] = DAT_800d5c1a;
    pcVar5[-1] = DAT_800d5c1a;
    *pcVar5 = DAT_800d5c1a;
    AddPrim(&UNK_800cc1f0 + uVar3 * 0x20,p);
    pcVar5 = pcVar5 + 0x28;
    p = p + 0x28;
  } while (iVar6 != 0);
  AddPrim(&UNK_800cc1f0 + uVar3 * 0x20,&UNK_800d6ad0 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


