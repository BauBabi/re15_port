/* FUN_80050858 @ 0x80050858  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80050858(int *param_1,int *param_2,uint param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  short *psVar5;
  short *psVar6;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  short *psVar7;
  
  psVar5 = *(short **)(DAT_800ce324 + 0x20);
  psVar6 = psVar5;
LAB_800508b0:
  do {
    psVar7 = psVar6;
    psVar6 = psVar7 + 8;
    if (psVar6 == psVar5 + *(int *)(psVar5 + 2) * 8) {
      return 0;
    }
  } while ((((ushort)psVar7[0xc] & param_3) == 0) ||
          ((&DAT_800a73b4)[(ushort)psVar7[0xc] & 0xf] != '\0'));
  DAT_800c3b80 = 3;
  if (((uint)(*param_1 - (int)*psVar6) < (uint)(ushort)psVar7[10]) &&
     ((uint)(param_1[2] - (int)psVar7[9]) < (uint)(ushort)psVar7[0xb])) {
    DAT_800c3b80 = 2;
  }
  else if (((uint)(*param_2 - (int)*psVar6) < (uint)(ushort)psVar7[10]) &&
          ((uint)(param_2[2] - (int)psVar7[9]) < (uint)(ushort)psVar7[0xb])) {
    DAT_800c3b80 = 2;
  }
  else {
    iVar10 = (int)psVar7[9];
    iVar4 = param_1[2];
    uVar14 = (uint)(ushort)psVar7[10];
    iVar2 = (int)*psVar6;
    iVar3 = *param_1;
    uVar1 = (uint)(ushort)psVar7[0xb];
    iVar11 = param_2[2];
    iVar13 = *param_2;
    iVar12 = iVar3 - iVar13;
    iVar9 = iVar4 - iVar11;
    iVar8 = iVar10 + uVar1;
    if ((-1 < (int)((uVar14 * (iVar4 - iVar10) - uVar1 * (iVar3 - iVar2) ^
                    uVar14 * (iVar11 - iVar10) - uVar1 * (iVar13 - iVar2)) &
                   (iVar12 * (iVar10 - iVar4) - iVar9 * (iVar2 - iVar3) ^
                   iVar12 * (iVar8 - iVar4) - iVar9 * ((iVar2 + uVar14) - iVar3)))) &&
       (-1 < (int)((uVar14 * (iVar8 - iVar4) - uVar1 * (iVar3 - iVar2) ^
                   uVar14 * (iVar8 - iVar11) - uVar1 * (iVar13 - iVar2)) &
                  (iVar12 * (iVar8 - iVar4) - iVar9 * (iVar2 - iVar3) ^
                  iVar12 * (iVar10 - iVar4) - iVar9 * ((iVar2 + uVar14) - iVar3)))))
    goto LAB_800508b0;
  }
  if (param_4 == 0) {
    return 1;
  }
  iVar11 = 0;
  for (uVar1 = *(uint *)(psVar7 + 0xe); (uVar1 & 1) == 0; uVar1 = (int)uVar1 >> 1) {
    iVar11 = iVar11 + -0x708;
  }
  iVar10 = (uint)((ushort)psVar7[0xd] >> 0xb) * -100 + ((ushort)psVar7[0xd] >> 6 & 0x1f) * -0x708;
  if ((((uint)(param_1[1] - iVar10) < (uint)(iVar11 - iVar10)) &&
      ((uint)(param_1[2] - (int)psVar7[9]) < (uint)(ushort)psVar7[0xb])) ||
     (((uint)(param_2[1] - iVar10) < (uint)(iVar11 - iVar10) &&
      ((uint)(param_2[2] - (int)psVar7[9]) < (uint)(ushort)psVar7[0xb])))) {
    DAT_800c3b80 = DAT_800c3b80 ^ 2;
  }
  else {
    iVar9 = param_1[1];
    uVar1 = (uint)(ushort)psVar7[0xb];
    iVar4 = (int)psVar7[9];
    iVar8 = param_1[2];
    iVar15 = iVar11 - iVar10;
    iVar3 = param_2[1];
    iVar2 = param_2[2];
    iVar13 = iVar8 - iVar2;
    iVar12 = iVar9 - iVar3;
    if ((-1 < (int)((uVar1 * (iVar9 - iVar10) - iVar15 * (iVar8 - iVar4) ^
                    uVar1 * (iVar3 - iVar10) - iVar15 * (iVar2 - iVar4)) &
                   (iVar13 * (iVar10 - iVar9) - iVar12 * (iVar4 - iVar8) ^
                   iVar13 * (iVar11 - iVar9) - iVar12 * ((iVar4 + uVar1) - iVar8)))) &&
       (-1 < (int)((uVar1 * (iVar11 - iVar9) - iVar15 * (iVar8 - iVar4) ^
                   uVar1 * (iVar11 - iVar3) - iVar15 * (iVar2 - iVar4)) &
                  (iVar13 * (iVar11 - iVar9) - iVar12 * (iVar4 - iVar8) ^
                  iVar13 * (iVar10 - iVar9) - iVar12 * ((iVar4 + uVar1) - iVar8)))))
    goto LAB_800508b0;
  }
  if ((((uint)(param_1[1] - iVar10) < (uint)(iVar11 - iVar10)) &&
      ((uint)(*param_1 - (int)*psVar6) < (uint)(ushort)psVar7[10])) ||
     (((uint)(param_2[1] - iVar10) < (uint)(iVar11 - iVar10) &&
      ((uint)(*param_2 - (int)*psVar6) < (uint)(ushort)psVar7[10])))) {
    if (DAT_800c3b80 != 0) {
      return 1;
    }
  }
  else {
    iVar9 = param_1[1];
    uVar1 = (uint)(ushort)psVar7[10];
    iVar4 = (int)*psVar6;
    iVar8 = *param_1;
    iVar15 = iVar11 - iVar10;
    iVar3 = param_2[1];
    iVar2 = *param_2;
    iVar13 = iVar8 - iVar2;
    iVar12 = iVar9 - iVar3;
    if (((int)((uVar1 * (iVar9 - iVar10) - iVar15 * (iVar8 - iVar4) ^
               uVar1 * (iVar3 - iVar10) - iVar15 * (iVar2 - iVar4)) &
              (iVar13 * (iVar10 - iVar9) - iVar12 * (iVar4 - iVar8) ^
              iVar13 * (iVar11 - iVar9) - iVar12 * ((iVar4 + uVar1) - iVar8))) < 0) ||
       ((int)((uVar1 * (iVar11 - iVar9) - iVar15 * (iVar8 - iVar4) ^
              uVar1 * (iVar11 - iVar3) - iVar15 * (iVar2 - iVar4)) &
             (iVar13 * (iVar11 - iVar9) - iVar12 * (iVar4 - iVar8) ^
             iVar13 * (iVar10 - iVar9) - iVar12 * ((iVar4 + uVar1) - iVar8))) < 0)) {
      return 1;
    }
  }
  goto LAB_800508b0;
}


