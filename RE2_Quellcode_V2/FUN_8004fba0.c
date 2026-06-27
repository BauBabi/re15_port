/* FUN_8004fba0 @ 0x8004fba0  (Ghidra headless, raw MIPS overlay) */

int FUN_8004fba0(uint *param_1,int param_2,ushort param_3,int param_4)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  int iVar6;
  int *piVar7;
  short *psVar8;
  uint uVar9;
  uint uVar10;
  short *psVar11;
  
  uVar10 = *param_1;
  uVar9 = param_1[2];
  psVar8 = *(short **)(DAT_800ce324 + 0x20);
  psVar11 = psVar8 + *(int *)(psVar8 + 2) * 8;
  uVar1 = FUN_8004c198(uVar10,uVar9,(int)*psVar8,(int)psVar8[1]);
  DAT_800d5be4 = *(int *)(psVar8 + 4);
  DAT_800c3b7c = 0;
  DAT_800dcc2c = 0;
  DAT_800dcbc8 = (uint)(0 < (int)param_1[1]);
  DAT_800ea198 = DAT_800d5be4;
  if (param_4 != 0) goto switchD_8004fe54_caseD_a;
  puVar5 = &DAT_800d0324;
  piVar7 = &DAT_800d03ac;
  if (DAT_800d4224 == &DAT_800d0324) goto switchD_8004fe54_caseD_a;
  do {
    if (((*puVar5 & 1) != 0) && (param_1 != puVar5 + 0xe)) {
      FUN_80036e30(puVar5);
      iVar2 = FUN_80038950(param_1,puVar5,param_2,0);
      if (iVar2 != 0) {
        iVar2 = *piVar7 - (uint)*(ushort *)((int)piVar7 + 0x16);
        if (iVar2 < (int)param_1[1]) {
          if ((int)(*piVar7 + (uint)*(ushort *)((int)piVar7 + 0x16)) < (int)param_1[1])
          goto LAB_8004fd58;
          iVar2 = *piVar7 - (uint)*(ushort *)((int)piVar7 + 0x16);
          DAT_800dcbc8 = DAT_800dcbc8 | 1;
          if (iVar2 < DAT_800dcc2c) {
            DAT_800dcc2c = iVar2;
          }
          iVar2 = param_1[1] - 1;
        }
        if (iVar2 < DAT_800c3b7c) {
          DAT_800c3b7c = (short)iVar2;
        }
      }
    }
LAB_8004fd58:
    puVar5 = puVar5 + 0x7e;
    piVar7 = piVar7 + 0x7e;
  } while (puVar5 != DAT_800d4224);
  psVar8 = psVar8 + 8;
LAB_800500cc:
  if (psVar8 == psVar11) {
    return (int)DAT_800c3b7c;
  }
  if (((((psVar8[5] & uVar1) == 0) ||
       ((uint)(ushort)psVar8[2] + param_2 * 2 <= (uVar10 + param_2) - (int)*psVar8)) ||
      ((uint)(ushort)psVar8[3] + param_2 * 2 <= (uVar9 + param_2) - (int)psVar8[1])) ||
     (((psVar8[4] & param_3) == 0 || ((psVar8[4] & 0xfU) == 10)))) goto switchD_8004fe54_caseD_a;
  iVar2 = 0;
  for (uVar4 = *(uint *)(psVar8 + 6); (uVar4 & 1) == 0; uVar4 = (int)uVar4 >> 1) {
    iVar2 = iVar2 + -0x708;
  }
  iVar6 = ((ushort)psVar8[5] >> 6 & 0x1f) * -0x708;
  switch(psVar8[4] & 0xf) {
  case 1:
    iVar3 = FUN_8004cfc8(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 2:
    iVar3 = FUN_8004d484(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 3:
    iVar3 = FUN_8004d940(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 4:
    iVar3 = FUN_8004dde8(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 5:
    iVar3 = FUN_8004ea14(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 6:
    iVar3 = FUN_8004ed84(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 7:
    iVar3 = FUN_8004ef0c(param_1,param_2,psVar8);
    if (iVar3 == 0) {
      psVar8 = psVar8 + 8;
      goto LAB_800500cc;
    }
    break;
  case 8:
    iVar3 = FUN_8004f17c(param_1,param_2,psVar8);
    if (iVar3 != 0) break;
    psVar8 = psVar8 + 8;
    goto LAB_800500cc;
  case 10:
    goto switchD_8004fe54_caseD_a;
  case 0xb:
    iVar3 = FUN_8004f8b8(*param_1,param_1[2],param_2,psVar8);
    iVar6 = iVar6 + iVar3;
    goto LAB_8004ffd4;
  case 0xc:
    iVar3 = FUN_8004fa28(param_1,iVar2,iVar6,psVar8);
    iVar6 = iVar6 + iVar3;
    goto LAB_8004ffd4;
  case 0xd:
    iVar6 = FUN_8004fb38(param_1,iVar2,iVar6,psVar8);
    goto LAB_8004ffd4;
  }
  iVar6 = iVar6 + (uint)((ushort)psVar8[5] >> 0xb) * -100;
LAB_8004ffd4:
  if (iVar2 < (int)param_1[1]) {
LAB_8005005c:
    if ((int)param_1[1] < iVar2) {
      DAT_800dcbc8 = DAT_800dcbc8 | 2;
      if (DAT_800ea198 < iVar2) {
        psVar8 = psVar8 + 8;
        DAT_800ea198 = iVar2;
        goto LAB_800500cc;
      }
    }
    else if (DAT_800d5be4 < iVar2) {
      DAT_800d5be4 = iVar2;
    }
  }
  else {
    if (iVar6 < (int)param_1[1]) {
      DAT_800dcbc8 = DAT_800dcbc8 | 1;
      if (iVar6 < DAT_800dcc2c) {
        DAT_800dcc2c = iVar6;
      }
      goto LAB_8005005c;
    }
    if (iVar6 < DAT_800c3b7c) {
      DAT_800c3b7c = (short)iVar6;
    }
    if (iVar6 <= (int)param_1[1]) goto LAB_8005005c;
  }
switchD_8004fe54_caseD_a:
  psVar8 = psVar8 + 8;
  goto LAB_800500cc;
}


