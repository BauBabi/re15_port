undefined4 * FUN_80025a98(uint *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  
  uVar4 = param_1[3];
  param_1[4] = (uint)param_2;
  iVar10 = *(int *)(uVar4 + 0x14);
  puVar11 = *(undefined4 **)(uVar4 + 0x18);
  puVar12 = param_2 + iVar10 * 0x1a;
  if (((uint)puVar11 & 0xff000000) == 0xff000000) {
    FUN_800104b0(param_2,(uint)puVar11 & 0xffffff | 0x80000000);
  }
  else {
    *(uint *)(uVar4 + 0x18) = (uint)param_2 & 0xffffff | 0xff000000;
    uVar4 = *param_1;
    if (iVar10 != 0) {
      puVar9 = puVar11 + 3;
      puVar7 = param_2 + 0xc;
      do {
        iVar10 = iVar10 + -1;
        *(undefined1 *)((int)puVar7 + -0x2d) = 0xc;
                    /* Possible PsyQ macro: setPolyGT4() */
        *(undefined1 *)((int)puVar7 + -0x29) = 0x3c;
        puVar7[-0xb] = 0x34808080;
        uVar1 = *puVar11;
        puVar7[-8] = 0x808080;
        puVar7[-9] = uVar1;
        uVar2 = puVar9[-2];
        puVar7[-5] = 0x808080;
        puVar7[-6] = (uVar4 & 0xc) << 0x13 | uVar2;
        uVar1 = puVar9[-1];
        puVar8 = param_2 + 0xd;
        puVar7[-2] = 0x808080;
        puVar7[-3] = uVar1;
        *puVar7 = *puVar9;
        puVar6 = param_2;
        do {
          uVar1 = puVar6[1];
          uVar3 = puVar6[2];
          uVar5 = puVar6[3];
          *puVar8 = *puVar6;
          puVar8[1] = uVar1;
          puVar8[2] = uVar3;
          puVar8[3] = uVar5;
          puVar6 = puVar6 + 4;
          puVar8 = puVar8 + 4;
        } while (puVar6 != puVar7);
        *puVar8 = *puVar7;
        puVar7 = puVar7 + 0x1a;
        param_2 = param_2 + 0x1a;
        puVar9 = puVar9 + 4;
        puVar11 = puVar11 + 4;
      } while (iVar10 != 0);
    }
  }
  return puVar12;
}
