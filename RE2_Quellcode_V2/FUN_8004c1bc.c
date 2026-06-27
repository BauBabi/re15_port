/* FUN_8004c1bc @ 0x8004c1bc  (Ghidra headless, raw MIPS overlay) */

int FUN_8004c1bc(int *param_1,uint param_2,uint param_3,uint param_4)

{
  undefined4 *puVar1;
  uint *puVar2;
  ushort uVar3;
  short sVar4;
  int iVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  ushort uVar9;
  int iVar10;
  short *psVar11;
  short *psVar12;
  short *psVar13;
  int iVar14;
  int iVar15;
  
  puVar2 = DAT_800ce330;
  DAT_800cfaf0 = 0;
  DAT_800c3b6c = &DAT_800cfaf0;
  iVar15 = *param_1 + param_2;
  psVar11 = *(short **)(DAT_800ce324 + 0x20);
  iVar14 = param_1[2] + param_2;
  iVar5 = *(int *)(psVar11 + 2);
  uVar3 = FUN_8004c198(*param_1,param_1[2],(int)*psVar11,(int)psVar11[1]);
  DAT_800c3b68 = 0;
  psVar12 = psVar11;
switchD_8004c504_default:
  do {
    do {
      do {
        psVar13 = psVar12;
        psVar12 = psVar13 + 8;
        if (psVar12 == psVar11 + iVar5 * 8) {
          return (int)(short)DAT_800c3b68;
        }
      } while ((psVar13[0xd] & uVar3) == 0);
      if ((param_2 & 1) != 0) {
        lVar6 = ratan2(((int)psVar13[9] + (uint)((ushort)psVar13[0xb] >> 1)) - iVar14,
                       ((int)*psVar12 + (uint)((ushort)psVar13[10] >> 1)) - iVar15);
        uVar8 = lVar6 - (uint)*(ushort *)((int)puVar2 + 0x76);
        uVar9 = (ushort)uVar8 & 0xfff;
        if ((uVar8 & 0xc00) == 0xc00) {
          sVar4 = 0x1000;
LAB_8004c2fc:
          uVar9 = sVar4 - uVar9;
        }
        else if ((uVar8 & 0x800) == 0) {
          sVar4 = 0x800;
          if ((uVar8 & 0x400) != 0) goto LAB_8004c2fc;
        }
        else {
          uVar9 = uVar9 - 0x800;
        }
        param_2 = (uint)*(ushort *)((int)puVar2 + 0x92);
        uVar8 = (uint)(ushort)puVar2[0x24];
        if (param_2 < uVar8) {
          iVar14 = rcos((int)(short)uVar9);
          param_2 = param_2 + ((int)((uVar8 - param_2) * iVar14) >> 0xc);
        }
        else {
          iVar14 = rsin((int)(short)uVar9);
          param_2 = uVar8 + ((int)((param_2 - uVar8) * iVar14) >> 0xc);
        }
        iVar15 = *param_1 + param_2;
        iVar14 = param_1[2] + param_2;
      }
    } while (((((uint)(ushort)psVar13[10] + param_2 * 2 <= (uint)(iVar15 - *psVar12)) ||
              ((uint)(ushort)psVar13[0xb] + param_2 * 2 <= (uint)(iVar14 - psVar13[9]))) ||
             (((uint)(ushort)psVar13[0xc] & param_4 & 0xffff) == 0)) ||
            ((((uint)(ushort)psVar13[0xc] & param_4 >> 0x10) != 0 ||
             ((*(uint *)(psVar13 + 0xe) & param_3) == 0))));
    *DAT_800c3b6c = psVar12;
    puVar1 = DAT_800c3b6c + 1;
    DAT_800c3b6c = DAT_800c3b6c + 1;
    *puVar1 = 0;
    if (param_4 >> 0x10 == 0) {
      switch(psVar13[0xc] & 0xf) {
      case 0:
      case 7:
      case 8:
        DAT_800c3b68 = DAT_800c3b68 | 1;
        goto switchD_8004c504_default;
      case 1:
        uVar9 = FUN_8004cfc8(puVar2 + 0x21,param_2,psVar12);
        break;
      case 2:
        uVar9 = FUN_8004d484(puVar2 + 0x21,param_2,psVar12);
        break;
      case 3:
        uVar9 = FUN_8004d940(puVar2 + 0x21,param_2,psVar12);
        break;
      case 4:
        uVar9 = FUN_8004dde8(puVar2 + 0x21,param_2,psVar12);
        break;
      case 5:
        uVar9 = FUN_8004ea14(puVar2 + 0x21,param_2,psVar12);
        break;
      case 6:
        uVar9 = FUN_8004ed84(puVar2 + 0x21,param_2,psVar12);
        break;
      case 9:
        DAT_800c3b68 = DAT_800c3b68 | 3;
        goto switchD_8004c504_default;
      case 10:
        DAT_800c3b68 = DAT_800c3b68 | 4;
        goto switchD_8004c504_default;
      case 0xb:
        DAT_800c3b68 = DAT_800c3b68 | 8;
        goto switchD_8004c504_default;
      case 0xc:
        DAT_800c3b68 = DAT_800c3b68 | 0x10;
        DAT_800d7694 = psVar12;
      default:
        goto switchD_8004c504_default;
      }
      DAT_800c3b68 = DAT_800c3b68 | uVar9;
      goto switchD_8004c504_default;
    }
    DAT_800c3b70 = puVar2[0x21];
    DAT_800c3b74 = puVar2[0x23];
    iVar7 = (*(code *)(&PTR_FUN_800a737c)[(ushort)psVar13[0xc] & 0xf])(puVar2,param_2,psVar12);
    uVar8 = DAT_800c3b74;
    if (iVar7 != 0) {
      iVar7 = puVar2[0x21] - DAT_800c3b70;
      iVar15 = iVar15 + iVar7;
      puVar2[0xe] = puVar2[0xe] + iVar7;
      iVar10 = puVar2[0x23] - uVar8;
      iVar14 = iVar14 + iVar10;
      *(short *)(puVar2 + 0x59) = (short)puVar2[0x59] + (short)iVar7;
      DAT_800c3b68 = DAT_800c3b68 | 1;
      *(short *)(puVar2 + 0x5a) = (short)puVar2[0x5a] + (short)iVar10;
      puVar2[0x10] = puVar2[0x10] + iVar10;
      if (iVar10 != 0) {
        *puVar2 = *puVar2 | 0x4000;
      }
    }
  } while( true );
}


