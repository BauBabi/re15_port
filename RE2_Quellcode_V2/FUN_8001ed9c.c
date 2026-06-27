/* FUN_8001ed9c @ 0x8001ed9c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001ed9c(void)

{
  undefined1 *puVar1;
  short *psVar2;
  char *pcVar3;
  char cVar4;
  char cVar5;
  char cVar6;
  char cVar7;
  short sVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  int local_30;
  int local_2c;
  int local_28;
  undefined1 auStack_20 [4];
  undefined1 auStack_1c [12];
  
  puVar1 = auStack_20 + 3;
  uVar10 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar10) =
       *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | DAT_80010900 >> (3 - uVar10) * 8;
  auStack_20 = (undefined1  [4])DAT_80010900;
  puVar1 = auStack_1c + 3;
  uVar10 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar10) =
       *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | DAT_80010904 >> (3 - uVar10) * 8;
  auStack_1c._0_4_ = DAT_80010904;
  iVar9 = FUN_800527b4((int)*(short *)(DAT_800dcbd0 + 0x34),(int)*(short *)(DAT_800dcbd0 + 0x38));
  if ((iVar9 == 0) || (*(short *)(DAT_800dcbd0 + 0x36) <= iVar9)) {
    *(char *)(DAT_800dcbd0 + 0xb) = *(char *)(DAT_800dcbd0 + 0xb) + -1;
    local_30 = (int)*(short *)(DAT_800dcbd0 + 0x34);
    local_2c = (int)*(short *)(DAT_800dcbd0 + 0x36);
    local_28 = (int)*(short *)(DAT_800dcbd0 + 0x38);
    iVar11 = FUN_8004fba0(&local_30,2,0x2000,0);
    iVar9 = DAT_800dcbd0;
    *(int *)(DAT_800dcbd0 + 0x14) = iVar11 + -10;
    local_2c = local_2c + 1000;
    iVar11 = FUN_800470c0(&local_30,(int)*(short *)(iVar9 + 0x22),auStack_20,
                          *(char *)(iVar9 + 0x1b) + 0x30009);
    local_2c = local_2c + -2000;
    iVar12 = FUN_800470c0(&local_30,(int)*(short *)(DAT_800dcbd0 + 0x22),auStack_20,
                          *(char *)(DAT_800dcbd0 + 0x1b) + 0x30009);
    iVar9 = DAT_800dcbd0;
    if ((iVar11 + iVar12 == 0) && (*(char *)(DAT_800dcbd0 + 0xb) != '\0')) {
      if (DAT_800dcbc8 == 0) {
        return;
      }
      pcVar3 = (char *)(DAT_800dcbd0 + 9);
      cVar4 = *(char *)(DAT_800dcbd0 + 10);
      cVar5 = *(char *)(DAT_800dcbd0 + 8);
      cVar6 = *(char *)(DAT_800dcbd0 + 9);
      cVar7 = *(char *)(DAT_800dcbd0 + 10);
      *(short *)(DAT_800dcbd0 + 0xc) =
           *(short *)(DAT_800dcbd0 + 0xc) - (short)*(char *)(DAT_800dcbd0 + 8);
      *(short *)(iVar9 + 0xe) = *(short *)(iVar9 + 0xe) - (short)*pcVar3;
      sVar8 = *(short *)(iVar9 + 0xc);
      *(short *)(iVar9 + 0x10) = *(short *)(iVar9 + 0x10) - (short)cVar4;
      *(short *)(iVar9 + 0xc) = *(short *)(iVar9 + 0xc) - (short)cVar5;
      *(short *)(iVar9 + 0x24) = *(short *)(iVar9 + 0x24) - sVar8;
      sVar8 = *(short *)(iVar9 + 0xe);
      *(short *)(iVar9 + 0xe) = *(short *)(iVar9 + 0xe) - (short)cVar6;
      *(short *)(iVar9 + 0x26) = *(short *)(iVar9 + 0x26) - sVar8;
      sVar8 = *(short *)(iVar9 + 0x10);
      *(short *)(iVar9 + 0x10) = *(short *)(iVar9 + 0x10) - (short)cVar7;
      *(short *)(iVar9 + 0x28) = *(short *)(iVar9 + 0x28) - sVar8;
      *(short *)(iVar9 + 0x24) = *(short *)(iVar9 + 0x24) - *(short *)(iVar9 + 0xc) / 3;
      *(short *)(iVar9 + 0x26) = *(short *)(iVar9 + 0x26) - *(short *)(iVar9 + 0xe) / 3;
      *(short *)(iVar9 + 0x28) =
           *(short *)(iVar9 + 0x28) -
           ((short)((ulonglong)((longlong)(int)*(short *)(iVar9 + 0x10) * 0x55555556) >> 0x20) -
           (*(short *)(iVar9 + 0x10) >> 0xf));
      FUN_8001d894();
      uVar10 = (uint)*(byte *)(DAT_800dcbd0 + 3);
      iVar9 = DAT_800dcbd0;
    }
    else {
      DAT_800cfb88 = (int)*(short *)(DAT_800dcbd0 + 0x34);
      *(ushort *)(DAT_800dcbd0 + 0x18) = *(ushort *)(DAT_800dcbd0 + 0x18) | 0x80;
      DAT_800cfb8c = (int)*(short *)(iVar9 + 0x36);
      DAT_800cfb90 = (int)*(short *)(iVar9 + 0x38);
      uVar10 = (uint)*(byte *)(iVar9 + 2);
    }
  }
  else {
    iVar9 = DAT_800dcbd0 + 0x34;
    psVar2 = (short *)(DAT_800dcbd0 + 0x22);
    *(short *)(DAT_800dcbd0 + 0x36) = *(short *)(DAT_800dcbd0 + 0x36) + 500;
    FUN_8001cbe8(0x1a051c00,(int)*psVar2,&DAT_8009db44,iVar9);
    uVar10 = (uint)*(byte *)(DAT_800dcbd0 + 2);
    iVar9 = DAT_800dcbd0;
  }
  (*(code *)(&PTR_FUN_8009d868)[uVar10 + (int)*(char *)(iVar9 + 0x1b)])();
  return;
}


