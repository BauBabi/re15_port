void * FUN_80048a44(uint param_1,void *param_2)

{
  bool bVar1;
  short sVar2;
  short sVar3;
  char cVar4;
  uint uVar5;
  short *psVar6;
  ushort uVar7;
  void *p;
  int iVar8;
  undefined *puVar9;
  
  puVar9 = (&PTR_DAT_80076214)[param_1 & 0xff];
  if (DAT_800aca34 == 0) {
    param_2 = (void *)((int)param_2 + 0x10);
  }
  bVar1 = 0x7f < DAT_800b2600;
  DAT_800b2600 = DAT_800b2600 + 1;
  if (bVar1) {
    DAT_800b2600 = 0x20;
  }
  cVar4 = DAT_800b2603 + '\x01';
  if (DAT_800b2603 < ' ') {
    DAT_800b2602 = DAT_800b2602 + '\x04';
  }
  else {
    DAT_800b2602 = DAT_800b2602 + -4;
  }
  uVar7 = 0;
  DAT_800b2603 = cVar4;
  if ('?' < cVar4) {
    DAT_800b2603 = '\0';
  }
  iVar8 = (param_1 & 0xff) * 4;
  psVar6 = (short *)((int)param_2 + 0xe);
  uVar5 = 0;
  do {
    p = param_2;
    cVar4 = (char)(uVar5 << 3);
    *(byte *)(psVar6 + -5) = (&DAT_8007622c)[iVar8] - ((&DAT_8007622f)[iVar8] & 1) * cVar4;
    *(byte *)((int)psVar6 + -9) =
         (&DAT_8007622d)[iVar8] - ((byte)(&DAT_8007622f)[iVar8] >> 1 & 1) * cVar4;
    *(byte *)(psVar6 + -4) =
         (&DAT_8007622e)[iVar8] - ((byte)(&DAT_8007622f)[iVar8] >> 2 & 1) * cVar4;
    if (((int)DAT_800b2600 - uVar5) - 0x20 < 0x4d) {
      psVar6[-3] = (DAT_800b2600 - (uVar7 + 1)) + DAT_800b25e4 + -0x12;
      sVar2 = DAT_800b25e6;
      sVar3 = *(short *)(puVar9 + ((int)DAT_800b2600 - uVar5) * 4);
      psVar6[-1] = psVar6[-3];
      psVar6[-2] = sVar3 + sVar2;
      *psVar6 = psVar6[-2] + *(short *)(puVar9 + ((int)DAT_800b2600 - uVar5) * 4 + 2);
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
    }
    param_2 = (void *)((int)p + 0x20);
    psVar6 = psVar6 + 0x10;
    uVar7 = uVar7 + 1;
    uVar5 = (uint)uVar7;
  } while (uVar7 < 0x20);
  if (DAT_800b25d4 == 1) {
    uVar7 = 0;
    psVar6 = (short *)((int)p + 0x2e);
    uVar5 = 0;
    do {
      *(undefined1 *)(psVar6 + -5) = 0x10;
      *(byte *)((int)psVar6 + -9) = ~(byte)(uVar5 << 3);
      *(undefined1 *)(psVar6 + -4) = 0x10;
      if (((int)DAT_800b25fe - uVar5) - 0x1f < 0x4e) {
        psVar6[-3] = (DAT_800b25fe - (uVar7 + 1)) + DAT_800b25e4 + -0x12;
        psVar6[-2] = DAT_800b25e6 + 0x22;
        psVar6[-1] = psVar6[-3];
        *psVar6 = DAT_800b25e6 + 0x46;
        AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,param_2);
      }
      param_2 = (void *)((int)param_2 + 0x20);
      psVar6 = psVar6 + 0x10;
      uVar7 = uVar7 + 1;
      uVar5 = (uint)uVar7;
    } while (uVar7 < 0x20);
    psVar6 = &DAT_800b25fe;
    sVar3 = DAT_800b25fe + 3;
    if (DAT_800b25fe < 0x80) goto LAB_80048ef0;
    sVar3 = 0x20;
  }
  else {
    if (DAT_800b25d4 < 2) {
      if (DAT_800b25d4 != 0) {
        return param_2;
      }
      return (void *)((int)p + 0x220);
    }
    uVar7 = 0;
    if (DAT_800b25d4 != 2) {
      return param_2;
    }
    psVar6 = (short *)((int)p + 0x2e);
    uVar5 = 0;
    do {
      *(undefined1 *)(psVar6 + -5) = 0x10;
      *(undefined1 *)((int)psVar6 + -9) = 0x10;
      *(char *)(psVar6 + -4) = (char)(uVar5 << 3);
      if (((int)DAT_800b25fc - uVar5) - 0x22 < 0x25) {
        psVar6[-3] = DAT_800b25e4 + 0xc;
        psVar6[-2] = (DAT_800b25e6 + DAT_800b25fc) - uVar7;
        psVar6[-1] = psVar6[-3] + 0x4d;
        *psVar6 = (DAT_800b25e6 + DAT_800b25fc) - uVar7;
        AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,param_2);
      }
      param_2 = (void *)((int)param_2 + 0x20);
      psVar6 = psVar6 + 0x10;
      uVar7 = uVar7 + 1;
      uVar5 = (uint)uVar7;
    } while (uVar7 < 0x20);
    psVar6 = &DAT_800b25fc;
    sVar3 = 0x6a;
    if (0x22 < DAT_800b25fc) {
      sVar3 = DAT_800b25fc + -2;
      goto LAB_80048ef0;
    }
  }
  DAT_800b25d4 = 0;
LAB_80048ef0:
  *psVar6 = sVar3;
  return param_2;
}
