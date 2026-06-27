/* FUN_8002fa54 @ 0x8002fa54  (Ghidra headless, raw MIPS overlay) */

byte * FUN_8002fa54(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined4 *param_4)

{
  byte bVar1;
  undefined2 *puVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  int iVar7;
  byte *pbVar8;
  byte *pbVar9;
  int iVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  
  puVar2 = DAT_800e8738;
  pbVar8 = (byte *)*param_4;
  pbVar3 = (byte *)(uint)*pbVar8;
  if ((pbVar3 != (byte *)0x0) &&
     (pbVar3 = (byte *)(uint)(DAT_800e8738 < &UNK_800e86f8), pbVar3 != (byte *)0x0)) {
    *DAT_800e8738 = param_1;
    puVar2[1] = param_2;
    puVar2[2] = param_3;
    pbVar6 = (byte *)(puVar2 + 3);
    puVar11 = param_4 + 1;
    iVar10 = 0;
    do {
      if (*pbVar8 == 0x25) {
        iVar13 = iVar10 + 1;
        puVar12 = puVar11 + 1;
        pbVar9 = pbVar8 + 1;
        pbVar3 = (byte *)*puVar11;
        iVar5 = 0;
        bVar1 = pbVar8[1];
        while (bVar1 - 0x30 < 10) {
          bVar1 = *pbVar9;
          pbVar9 = pbVar9 + 1;
          iVar5 = iVar5 * 10 + -0x30 + (uint)bVar1;
          bVar1 = *pbVar9;
        }
        bVar1 = *pbVar9;
        pbVar8 = pbVar9 + 1;
        if (bVar1 == 100) {
          iVar10 = 1;
          if ((int)pbVar3 < 0) {
            iVar10 = -1;
            *pbVar6 = 0x2d;
            pbVar6 = pbVar6 + 1;
          }
          iVar14 = (int)pbVar3 * iVar10;
          iVar7 = 1;
          iVar4 = 1;
          iVar15 = iVar14;
          while (9 < iVar15) {
            iVar4 = iVar4 * 10;
            iVar15 = iVar14 / iVar4;
            if (iVar4 == 0) {
              trap(0x1c00);
            }
            if ((iVar4 == -1) && (iVar14 == -0x80000000)) {
              trap(0x1800);
            }
            iVar7 = iVar7 + 1;
          }
          if (iVar5 == 0) {
            do {
              iVar5 = (int)pbVar3 / iVar4;
              pbVar9 = (byte *)((int)pbVar3 % iVar4);
              if (iVar4 == 0) {
                trap(0x1c00);
              }
              if ((iVar4 == -1) && (pbVar3 == &DAT_80000000)) {
                trap(0x1800);
              }
              iVar4 = iVar4 / 10;
              iVar7 = iVar7 + -1;
              *pbVar6 = "0123456789ABCDEF"[(iVar5 % 10) * iVar10];
              pbVar6 = pbVar6 + 1;
              pbVar3 = pbVar9;
            } while (iVar7 != 0);
          }
          else {
            iVar7 = iVar5 - iVar7;
            if (iVar10 == -1) {
              while( true ) {
                iVar7 = iVar7 + -1;
                iVar5 = iVar5 + -1;
                if (iVar7 < 1) break;
                    /* Possible PsyQ macro: setPolyFT4() + setShadeTex(polyFT4, 1) */
                *pbVar6 = 0x2d;
                pbVar6[-1] = 0x20;
                pbVar6 = pbVar6 + 1;
              }
            }
            else {
              for (; 0 < iVar7; iVar7 = iVar7 + -1) {
                *pbVar6 = 0x20;
                pbVar6 = pbVar6 + 1;
                iVar5 = iVar5 + -1;
              }
            }
            for (; iVar5 != 0; iVar5 = iVar5 + -1) {
              iVar7 = (int)pbVar3 / iVar4;
              pbVar9 = (byte *)((int)pbVar3 % iVar4);
              if (iVar4 == 0) {
                trap(0x1c00);
              }
              if ((iVar4 == -1) && (pbVar3 == &DAT_80000000)) {
                trap(0x1800);
              }
              iVar4 = iVar4 / 10;
              *pbVar6 = "0123456789ABCDEF"[(iVar7 % 10) * iVar10];
              pbVar6 = pbVar6 + 1;
              pbVar3 = pbVar9;
            }
          }
        }
        else {
          if (bVar1 < 0x65) {
            if (bVar1 == 0x25) {
              *pbVar6 = 0x25;
            }
            else {
              if (bVar1 != 99) goto LAB_8002fdb4;
              *pbVar6 = (byte)pbVar3;
            }
            goto LAB_8002fde0;
          }
          if (bVar1 != 0x73) {
LAB_8002fdb4:
            puVar11 = param_4 + 1;
            pbVar8 = (byte *)0x0;
            if (0 < iVar10) {
              do {
                pbVar8 = pbVar8 + 1;
                puVar11 = puVar11 + 1;
              } while ((int)pbVar8 < iVar10);
            }
            goto LAB_8002fdd4;
          }
          bVar1 = *pbVar3;
          while (bVar1 != 0) {
            pbVar3 = pbVar3 + 1;
            *pbVar6 = bVar1;
            pbVar6 = pbVar6 + 1;
            bVar1 = *pbVar3;
          }
        }
      }
      else {
LAB_8002fdd4:
        bVar1 = *pbVar8;
        pbVar8 = pbVar8 + 1;
        *pbVar6 = bVar1;
        puVar12 = puVar11;
        iVar13 = iVar10;
LAB_8002fde0:
        pbVar6 = pbVar6 + 1;
      }
      puVar11 = puVar12;
      iVar10 = iVar13;
    } while (*pbVar8 != 0);
    *pbVar6 = 0;
    pbVar3 = pbVar6 + (-6 - (int)DAT_800e8738);
    DAT_800e8738 = (undefined2 *)((uint)(pbVar6 + 2) & 0xfffffffe);
  }
  return pbVar3;
}


