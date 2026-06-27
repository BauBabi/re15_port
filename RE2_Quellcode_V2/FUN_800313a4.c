/* FUN_800313a4 @ 0x800313a4  (Ghidra headless, raw MIPS overlay) */

void FUN_800313a4(uint param_1,int param_2,byte *param_3,uint param_4)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  ushort *puVar5;
  uint *puVar6;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  
  uVar4 = (uint)DAT_800ce5e0;
  bVar1 = *param_3;
  puVar9 = (uint *)(&DAT_800e2ab0 + uVar4 * 0xc0 + (param_4 & 7) * 0x18);
  puVar7 = DAT_800e8430;
  if (bVar1 != 0xfe) {
    puVar5 = (ushort *)((int)DAT_800e8430 + 0xe);
    do {
      iVar8 = 0xe;
      if (bVar1 == 0) {
        if ((param_4 & 0x4000) != 0) {
          param_1 = param_1 - (int)DAT_8009de34;
          iVar8 = (int)DAT_8009de35 + (int)DAT_8009de34;
        }
        param_1 = param_1 + iVar8;
        goto switchD_80031490_caseD_f5;
      }
      puVar6 = puVar9;
      switch(bVar1) {
      case 0xee:
        param_3 = param_3 + 1;
        *(byte *)((int)puVar5 + -1) = (*param_3 / 0x12) * '\x0e' + -0x2e;
        break;
      case 0xef:
        param_3 = param_3 + 1;
        *(byte *)((int)puVar5 + -1) = (*param_3 / 0x12) * '\x0e';
        puVar6 = puVar9 + 3;
        break;
      case 0xf0:
        param_3 = param_3 + 1;
        *(byte *)((int)puVar5 + -1) = (*param_3 / 0x12) * '\x0e' + -0x3c;
        puVar6 = puVar9 + 3;
        break;
      default:
        *(byte *)((int)puVar5 + -1) = (*param_3 / 0x12) * '\x0e' + '\x1c';
        if (((param_4 & 0x4000) != 0) && (iVar2 = (uint)*param_3 * 2, *param_3 < 0x57)) {
          param_1 = param_1 - (int)(&DAT_8009de34)[iVar2];
          iVar8 = (int)(&DAT_8009de35)[iVar2] + (int)(&DAT_8009de34)[iVar2];
        }
        break;
      case 0xf5:
        goto switchD_80031490_caseD_f5;
      case 0xf6:
        param_1 = param_1 + 7;
        goto switchD_80031490_caseD_f5;
      case 0xf7:
      case 0xfe:
        return;
      }
      puVar5[1] = 0xe;
      puVar5[2] = 0xe;
      puVar5[-0xffffffff00000005] = 0x8080;
      puVar5[-0xffffffff00000004] = 0x6480;
      *(uint *)(puVar5 + -3) = param_1 | param_2 << 0x10;
      bVar1 = *param_3;
      *puVar5 = (ushort)((param_4 & 0xf0) << 3) | 0x7810;
      *(byte *)(puVar5 + -1) = (bVar1 % 0x12) * '\x0e';
      puVar5 = puVar5 + 10;
                    /* Probable PsyQ macro: addPrim(). */
      *puVar7 = *puVar7 & 0xff000000 | *puVar6 & 0xffffff;
      uVar3 = (uint)puVar7 & 0xffffff;
      puVar7 = puVar7 + 5;
      *puVar6 = *puVar6 & 0xff000000 | uVar3;
      if (puVar7 == (uint *)(&UNK_800e4030 + uVar4 * 0x1400)) {
        DAT_800e8430 = puVar7;
        return;
      }
      param_1 = param_1 + iVar8;
switchD_80031490_caseD_f5:
      param_3 = param_3 + 1;
      bVar1 = *param_3;
    } while (bVar1 != 0xfe);
  }
  DAT_800e8430 = puVar7;
  return;
}


