/* FUN_8002cbc4 @ 0x8002cbc4  (Ghidra headless, raw MIPS overlay) */

undefined4 * FUN_8002cbc4(uint *param_1,undefined4 *param_2,int param_3)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  int iVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  
  uVar5 = param_1[1];
  param_1[2] = (uint)param_2;
  iVar11 = *(int *)(uVar5 + 0x14);
  puVar12 = *(undefined4 **)(uVar5 + 0x18);
  puVar13 = param_2 + iVar11 * 0x14;
  if (param_3 == 0) {
    if (((uint)puVar12 & 0xff000000) == 0xff000000) {
      FUN_80010778(param_2,(uint)puVar12 & 0xffffff | 0x80000000,iVar11 * 0x50);
      return puVar13;
    }
    *(uint *)(uVar5 + 0x18) = (uint)param_2 & 0xffffff | 0xff000000;
  }
  uVar5 = *param_1;
  if (iVar11 != 0) {
    puVar10 = puVar12 + 2;
    puVar8 = param_2 + 8;
    do {
      iVar11 = iVar11 + -1;
      *(undefined1 *)((int)puVar8 + -0x1d) = 9;
                    /* Possible PsyQ macro: setPolyGT3() */
      *(undefined1 *)((int)puVar8 + -0x19) = 0x34;
      puVar8[-7] = 0x34808080;
      uVar2 = *puVar12;
      puVar8[-4] = 0x808080;
      puVar8[-5] = uVar2;
      uVar3 = puVar10[-1];
      puVar8[-1] = 0x808080;
      puVar8[-2] = uVar3 | (uVar5 & 0xc) << 0x13;
      puVar8[1] = *puVar10;
      puVar7 = param_2;
      puVar1 = param_2 + 10;
      do {
        puVar9 = puVar1;
        uVar2 = puVar7[1];
        uVar4 = puVar7[2];
        uVar6 = puVar7[3];
        *puVar9 = *puVar7;
        puVar9[1] = uVar2;
        puVar9[2] = uVar4;
        puVar9[3] = uVar6;
        puVar7 = puVar7 + 4;
        puVar1 = puVar9 + 4;
      } while (puVar7 != puVar8);
      uVar2 = puVar8[1];
      puVar9[4] = *puVar8;
      puVar9[5] = uVar2;
      puVar8 = puVar8 + 0x14;
      param_2 = param_2 + 0x14;
      puVar10 = puVar10 + 3;
      puVar12 = puVar12 + 3;
    } while (iVar11 != 0);
  }
  return puVar13;
}


