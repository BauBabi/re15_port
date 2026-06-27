/* FUN_80072c6c @ 0x80072c6c  (Ghidra headless, raw MIPS overlay) */

void FUN_80072c6c(void)

{
  int iVar1;
  int iVar2;
  undefined *puVar3;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int unaff_s7;
  
  puVar3 = &UNK_80199600;
  iVar7 = 0;
  if (DAT_800ce5e0 != 0) {
    puVar3 = &UNK_80199614;
  }
  pbVar4 = &DAT_800d4a6c;
  iVar2 = 0x34;
  uVar6 = 0xd;
  do {
    uVar5 = uVar6;
    uVar6 = uVar5 - 1;
    if ((pbVar4[2] == 2) || (DAT_800d46ac <= uVar6)) {
      puVar3 = puVar3 + 0xa0;
    }
    else {
      puVar3 = (undefined *)
               FUN_8007300c(puVar3,pbVar4[1],
                            ((int)*(short *)(&UNK_800aadf4 + iVar2) + (int)DAT_800d5c24) * 0x10000 +
                            (int)*(short *)(&UNK_800aadf6 + iVar2) + (int)DAT_800d5c26,
                            (uint)(byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] |
                            (-(uint)((byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] >> 7) & 10) << 0x10);
    }
    pbVar4 = pbVar4 + -4;
    iVar2 = iVar2 + -4;
  } while (uVar6 != 0);
  iVar2 = (uint)DAT_800d5bf8 * 4;
  pbVar4 = &DAT_800d4a3c + iVar2;
  if (DAT_800d5bf8 == 0x80) {
    puVar3 = puVar3 + 0xa0;
  }
  else {
    if ((&DAT_800d4a3e)[iVar2] == '\0') {
      iVar7 = 0x14;
    }
    if (DAT_800d5c00 == '\x01') {
      iVar7 = iVar7 + 0x36;
    }
    puVar3 = (undefined *)
             FUN_8007300c(puVar3,(&DAT_800d4a3d)[iVar2],
                          ((int)DAT_800aae24 + (int)DAT_800d5c24 + iVar7) * 0x10000 +
                          (int)DAT_800aae26 + (int)DAT_800d5c26,
                          (uint)(byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] |
                          (-(uint)((byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] >> 7) & 10) << 0x10);
  }
  if (DAT_800d5c00 == '\x01') {
    uVar6 = (uint)DAT_800d5bf2;
    if (uVar6 == 3) {
      unaff_s7 = 0x52;
      pbVar4 = &DAT_800d4a68 + (DAT_800d5c14 + 5 & 0x3f) * 4;
    }
    if (uVar6 == 2) {
      unaff_s7 = -0x26;
      pbVar4 = &DAT_800d4a68 + (DAT_800d5c14 - 1 & 0x3f) * 4;
    }
    if (uVar6 - 2 < 2) {
      puVar3 = (undefined *)
               FUN_8007300c(puVar3,pbVar4[1],
                            ((int)*(short *)(&UNK_800aadf8 + (uVar5 + 0xb) * 4) + (int)DAT_800d5c24)
                            * 0x10000 + (int)DAT_800d5c26 + (int)DAT_800d5c15 + unaff_s7,
                            (uint)(byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] |
                            (-(uint)((byte)(&DAT_800a9e1e)[(uint)*pbVar4 * 8] >> 7) & 10) << 0x10);
    }
    iVar2 = 5;
    iVar7 = 0x48;
    do {
      iVar2 = iVar2 + -1;
      iVar1 = ((uint)DAT_800d5c14 + iVar2 & 0x3f) * 4;
      puVar3 = (undefined *)
               FUN_8007300c(puVar3,(&DAT_800d4a69)[iVar1],
                            ((int)*(short *)(&UNK_800aadf4 + iVar7) + (int)DAT_800d5c24) * 0x10000 +
                            (int)*(short *)(&UNK_800aadf6 + iVar7) + (int)DAT_800d5c26 +
                            (int)DAT_800d5c15,
                            (uint)(byte)(&DAT_800a9e1e)[(uint)(byte)(&DAT_800d4a68)[iVar1] * 8] |
                            (-(uint)((byte)(&DAT_800a9e1e)[(uint)(byte)(&DAT_800d4a68)[iVar1] * 8]
                                    >> 7) & 10) << 0x10);
      iVar7 = iVar7 + -4;
    } while (iVar2 != 0);
  }
  AddPrim(&UNK_800cc1f4 + (uint)DAT_800ce5e0 * 0x20,&UNK_800d6b30 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


