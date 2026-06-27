void FUN_800279c8(undefined4 param_1,undefined2 param_2,byte *param_3,byte *param_4)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  char *pcVar10;
  int iVar11;
  int iVar12;
  
  if ((*param_3 != 0) && (DAT_800b851c < &UNK_800b8515)) {
    *DAT_800b851c = (short)param_1;
    DAT_800b851c[1] = (short)((uint)param_1 >> 0x10);
    DAT_800b851c[2] = param_2;
    pbVar6 = (byte *)(DAT_800b851c + 3);
    pcVar10 = "0123456789ABCDEF";
    do {
      uVar4 = 0;
      if (*param_3 != 0x25) {
        *pbVar6 = *param_3;
        pbVar6 = pbVar6 + 1;
        param_3 = param_3 + 1;
        if (pbVar6 < &UNK_800b851b) goto switchD_80027aa4_caseD_59;
        break;
      }
      pbVar5 = param_3 + 1;
      bVar1 = *pbVar5;
      while (bVar1 - 0x30 < 10) {
        bVar1 = *pbVar5;
        pbVar5 = pbVar5 + 1;
        uVar4 = uVar4 * 10 + -0x30 + (uint)bVar1;
        bVar1 = *pbVar5;
      }
      param_3 = pbVar5 + 1;
      switch(*pbVar5) {
      case 0x58:
        goto switchD_80027aa4_caseD_58;
      case 0x62:
        pbVar5 = pbVar6;
        if (uVar4 == 0) {
          uVar4 = 1;
          pbVar6 = param_4;
          while (((uint)pbVar6 & 0xfffffffe) != 0) {
            uVar2 = uVar4 & 0x1f;
            uVar4 = uVar4 + 1;
            pbVar6 = (byte *)((uint)param_4 >> uVar2);
          }
        }
        do {
          uVar4 = uVar4 - 1;
          pbVar6 = pbVar5 + 1;
          *pbVar5 = pcVar10[((uint)param_4 & 1 << (uVar4 & 0x1f)) != 0];
          if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
          if ((uVar4 & 7) == 0) {
            *pbVar6 = 0x20;
            pbVar6 = pbVar5 + 2;
            if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
          }
          pbVar5 = pbVar6;
        } while (uVar4 != 0);
        break;
      case 100:
        iVar9 = 1;
        if ((int)param_4 < 0) {
          *pbVar6 = 0x2d;
          pbVar6 = pbVar6 + 1;
          iVar9 = -1;
          if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
        }
        iVar11 = (int)param_4 * iVar9;
        iVar8 = 1;
        iVar7 = 1;
        iVar12 = iVar11;
        while (9 < iVar12) {
          iVar7 = iVar7 * 10;
          iVar12 = iVar11 / iVar7;
          if (iVar7 == 0) {
            trap(0x1c00);
          }
          if ((iVar7 == -1) && (iVar11 == -0x80000000)) {
            trap(0x1800);
          }
          iVar8 = iVar8 + 1;
        }
        pbVar5 = param_4;
        if (uVar4 == 0) {
          do {
            param_4 = (byte *)((int)pbVar5 % iVar7);
            if (iVar7 == 0) {
              trap(0x1c00);
            }
            if ((iVar7 == -1) && (pbVar5 == &DAT_80000000)) {
              trap(0x1800);
            }
            *pbVar6 = pcVar10[(((int)pbVar5 / iVar7) % 10) * iVar9];
            pbVar6 = pbVar6 + 1;
            if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
            iVar8 = iVar8 + -1;
            iVar7 = iVar7 / 10;
            pbVar5 = param_4;
          } while (iVar8 != 0);
        }
        else {
          iVar8 = uVar4 - iVar8;
          if (iVar9 == -1) {
            while( true ) {
              iVar8 = iVar8 + -1;
              uVar4 = uVar4 - 1;
              if (iVar8 < 1) break;
                    /* Possible PsyQ macro: setPolyFT4() + setShadeTex(polyFT4, 1) */
              *pbVar6 = 0x2d;
              pbVar6[-1] = 0x20;
              pbVar6 = pbVar6 + 1;
              if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
            }
          }
          else {
            while (0 < iVar8) {
              *pbVar6 = 0x20;
              pbVar6 = pbVar6 + 1;
              iVar8 = iVar8 + -1;
              if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
              uVar4 = uVar4 - 1;
            }
          }
          for (; uVar4 != 0; uVar4 = uVar4 - 1) {
            pbVar5 = (byte *)((int)param_4 % iVar7);
            if (iVar7 == 0) {
              trap(0x1c00);
            }
            if ((iVar7 == -1) && (param_4 == &DAT_80000000)) {
              trap(0x1800);
            }
            *pbVar6 = pcVar10[(((int)param_4 / iVar7) % 10) * iVar9];
            pbVar6 = pbVar6 + 1;
            if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
            iVar7 = iVar7 / 10;
            param_4 = pbVar5;
          }
        }
        break;
      case 0x73:
        bVar1 = *param_4;
        while (bVar1 != 0) {
          *pbVar6 = *param_4;
          pbVar5 = pbVar6 + 1;
          pbVar3 = pbVar6 + 3;
          if (&UNK_800b851a < pbVar5) goto LAB_80027e4c;
          param_4 = param_4 + 1;
          pbVar6 = pbVar5;
          bVar1 = *param_4;
        }
        break;
      case 0x78:
        pcVar10 = "0123456789abcdef";
switchD_80027aa4_caseD_58:
        if (uVar4 == 0) {
          uVar4 = 1;
          pbVar5 = param_4;
          while (((uint)pbVar5 & 0xfffffff0) != 0) {
            uVar2 = uVar4 & 7;
            uVar4 = uVar4 + 1;
            pbVar5 = (byte *)((uint)param_4 >> (uVar2 << 2));
          }
        }
        do {
          uVar4 = uVar4 - 1;
          *pbVar6 = pcVar10[((uint)param_4 & 0xf << (uVar4 * 4 & 0x1f)) >> (uVar4 * 4 & 0x1f)];
          pbVar6 = pbVar6 + 1;
          if (&UNK_800b851a < pbVar6) goto LAB_80027e48;
        } while (uVar4 != 0);
      }
switchD_80027aa4_caseD_59:
    } while (*param_3 != 0);
LAB_80027e48:
    pbVar3 = pbVar6 + 2;
    pbVar5 = pbVar6;
LAB_80027e4c:
    DAT_800b851c = (undefined2 *)((uint)pbVar3 & 0xfffffffe);
    *pbVar5 = 0;
  }
  return;
}
