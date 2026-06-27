/* FUN_80074f4c @ 0x80074f4c  (Ghidra headless, raw MIPS overlay) */

void FUN_80074f4c(void)

{
  char cVar1;
  short sVar2;
  u_short uVar3;
  int iVar4;
  short *psVar5;
  undefined *puVar6;
  undefined *p;
  uint uVar7;
  short sVar8;
  uint uVar9;
  undefined *ot;
  short sVar10;
  
  puVar6 = &UNK_8019a600;
  uVar7 = 7;
  uVar9 = (uint)DAT_800d5bf8;
  ot = &UNK_800cc1f8 + (uint)DAT_800ce5e0 * 0x20;
  sVar10 = DAT_800d5c26 + -0x29;
  cVar1 = (&DAT_800d4a3e)[uVar9 * 4];
  if (DAT_800ce5e0 != 0) {
    puVar6 = &DAT_8019a614;
  }
  sVar8 = DAT_800d5c24 + -0x34;
  if (DAT_800d5c00 == '\x01') {
    uVar7 = 6;
    puVar6 = puVar6 + 0x28;
    sVar8 = DAT_800d5c24;
  }
  psVar5 = (short *)(puVar6 + 10);
  iVar4 = uVar7 << 2;
  do {
    p = puVar6;
    uVar7 = uVar7 - 1;
    psVar5[-1] = *(short *)(&DAT_800ab140 + iVar4) + sVar8;
    *psVar5 = *(short *)(&DAT_800ab142 + iVar4) + sVar10;
    AddPrim(ot,p);
    psVar5 = psVar5 + 0x14;
    iVar4 = iVar4 + -4;
    puVar6 = p + 0x28;
  } while (2 < uVar7);
  AddPrim(ot,&UNK_800d6a40 + (uint)DAT_800ce5e0 * 0xc);
  if (cVar1 == '\0') {
    *(short *)(p + 0x30) = DAT_800ab144 + sVar8 + 0x14;
    sVar2 = DAT_800ab146;
    *(undefined2 *)(p + 0x38) = 0x28;
    *(undefined2 *)(p + 0x3a) = 0x1e;
    *(short *)(p + 0x32) = sVar2 + sVar10;
  }
  else {
    *(short *)(p + 0x30) = DAT_800ab144 + sVar8;
    sVar2 = DAT_800ab146;
    *(undefined2 *)(p + 0x38) = 0x50;
    *(undefined2 *)(p + 0x3a) = 0x1e;
    *(short *)(p + 0x32) = sVar2 + sVar10;
    if (cVar1 == '\x02') {
      uVar9 = uVar9 - 1;
    }
  }
  p[0x34] = ((byte)uVar9 & 1) * '(';
  p[0x35] = (char)(uVar9 >> 1) * '\x1e';
  uVar3 = GetClut(0,0x1e5);
  *(u_short *)(p + 0x36) = uVar3;
  if (DAT_800d5bf8 != 0x80) {
    AddPrim(ot,p + 0x28);
  }
  *(short *)(p + 0x58) = DAT_800ab148 + sVar8;
  *(short *)(p + 0x5a) = DAT_800ab14a + sVar10;
  uVar3 = GetClut(0,0x1e5);
  *(u_short *)(p + 0x5e) = uVar3;
  AddPrim(ot,p + 0x50);
  AddPrim(ot,&UNK_800d6b78 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


