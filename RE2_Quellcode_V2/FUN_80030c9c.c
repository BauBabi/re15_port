/* FUN_80030c9c @ 0x80030c9c  (Ghidra headless, raw MIPS overlay) */

void FUN_80030c9c(int param_1)

{
  ushort uVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  byte bVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  void *ot;
  byte *pbVar9;
  byte *pbVar10;
  undefined4 *puVar11;
  int iVar12;
  undefined *p;
  undefined *puVar13;
  void *pvVar14;
  byte *local_40;
  
  puVar13 = DAT_800e8430;
  uVar3 = 0x7810;
  iVar12 = *(int *)(param_1 + 0x5ca4);
  uVar6 = (uint)DAT_800ce5e0;
  *(undefined2 *)(param_1 + 0x5ca2) = *(undefined2 *)(param_1 + 0x5ca6);
  if ((*(ushort *)(param_1 + 0x5ca8) & 0xf) == 0) {
    iVar7 = 6;
  }
  else {
    iVar7 = (*(ushort *)(param_1 + 0x5ca8) & 7) << 1;
  }
  pvVar14 = (void *)(param_1 + (uint)DAT_800ce5e0 * 0xc0 + iVar7 * 0xc);
  uVar1 = *(ushort *)(param_1 + 0x5ca8);
  if (*(byte **)(param_1 + 0x5c94) != *(byte **)(param_1 + 0x5c98)) {
    puVar11 = (undefined4 *)(puVar13 + 0x10);
    pbVar9 = *(byte **)(param_1 + 0x5c94);
    p = puVar13;
    do {
      iVar7 = 0xe;
      puVar13 = p;
      if (*pbVar9 == 0) {
        if ((uVar1 & 0x4000) != 0) {
          iVar12 = iVar12 - DAT_8009de34;
          iVar7 = (int)DAT_8009de35 + (int)DAT_8009de34;
        }
switchD_80030dac_caseD_f5:
        goto LAB_8003101c;
      }
      switch(*pbVar9) {
      case 0xee:
        pbVar9 = pbVar9 + 1;
        cVar2 = (*pbVar9 / 0x12) * '\x0e' + -0x2e;
        goto LAB_80030f0c;
      case 0xef:
        pbVar9 = pbVar9 + 1;
        ot = (void *)((int)pvVar14 + 0xc);
        *(byte *)((int)puVar11 + -3) = (*pbVar9 / 0x12) * '\x0e';
        goto LAB_80030f14;
      case 0xf0:
        pbVar9 = pbVar9 + 1;
        ot = (void *)((int)pvVar14 + 0xc);
        *(byte *)((int)puVar11 + -3) = (*pbVar9 / 0x12) * '\x0e' + -0x3c;
        goto LAB_80030f14;
      default:
        cVar2 = (*pbVar9 / 0x12) * '\x0e' + '\x1c';
LAB_80030f0c:
        *(char *)((int)puVar11 + -3) = cVar2;
        ot = pvVar14;
LAB_80030f14:
        if (((((uVar1 & 0x4000) != 0) && (*pbVar9 < 0x57)) && (1 < pbVar9[-1] - 0xee)) &&
           (iVar8 = (uint)*pbVar9 * 2, pbVar9[-1] != 0xf0)) {
          iVar12 = iVar12 - (&DAT_8009de34)[iVar8];
          iVar7 = (int)(&DAT_8009de35)[iVar8] + (int)(&DAT_8009de34)[iVar8];
        }
        puVar11[-2] = iVar12;
        puVar11[-3] = 0x64808080;
        bVar5 = *pbVar9;
        puVar13 = p + 0x14;
        *puVar11 = 0xe000e;
        *(ushort *)((int)puVar11 + -2) = uVar3;
        *(byte *)(puVar11 + -1) = (bVar5 % 0x12) * '\x0e';
        AddPrim(ot,p);
        puVar11 = puVar11 + 5;
        if (puVar13 == &UNK_800e4030 + uVar6 * 0x1400) goto LAB_80031030;
LAB_8003101c:
        pbVar10 = pbVar9 + 1;
        iVar12 = iVar12 + iVar7;
        break;
      case 0xf5:
        goto switchD_80030dac_caseD_f5;
      case 0xf7:
        pbVar10 = local_40 + 2;
        break;
      case 0xf8:
        bVar5 = pbVar9[1];
        if (bVar5 == 0) {
          bVar5 = *(byte *)(param_1 + 0x5c8f);
        }
        pbVar10 = (byte *)FUN_80030b9c(bVar5);
        local_40 = pbVar9;
        break;
      case 0xf9:
        pbVar10 = pbVar9 + 2;
        uVar3 = (ushort)pbVar9[1] << 7 | 0x7810;
        break;
      case 0xfc:
        pbVar10 = pbVar9 + 1;
        sVar4 = *(short *)(param_1 + 0x5ca2) + 0x10;
        iVar12 = CONCAT22(sVar4,*(undefined2 *)(param_1 + 0x5ca4));
        *(short *)(param_1 + 0x5ca2) = sVar4;
        break;
      case 0xfd:
        *(undefined2 *)(param_1 + 0x5ca2) = *(undefined2 *)(param_1 + 0x5ca6);
      case 0xfa:
        pbVar9 = pbVar9 + 1;
      case 0xfe:
        pbVar10 = pbVar9 + 1;
        *(undefined2 *)(param_1 + 0x5ca2) = *(undefined2 *)(param_1 + 0x5ca6);
      }
      pbVar9 = pbVar10;
      p = puVar13;
    } while (pbVar10 != *(byte **)(param_1 + 0x5c98));
  }
LAB_80031030:
  DAT_800e8430 = puVar13;
  *(short *)(param_1 + 0x5ca0) = (short)iVar12;
  return;
}


