/* FUN_800318a4 @ 0x800318a4  (Ghidra headless, raw MIPS overlay) */

uint FUN_800318a4(ushort *param_1,uint param_2,int param_3)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  uint *puVar9;
  int iVar10;
  uint uVar11;
  
  uVar6 = (uint)DAT_800ce5e0;
  if (DAT_800e8430 == (uint *)(&UNK_800e4030 + uVar6 * 0x1400)) {
    uVar6 = *(uint *)(param_3 + 0x5c88);
  }
  else {
    uVar1 = *param_1;
    param_1 = param_1 + 1;
    uVar5 = uVar1 >> 8 & 1;
    puVar7 = (uint *)(param_3 + uVar6 * 0xc0 + (uVar1 & 7) * 0x18);
    uVar11 = *puVar7;
    puVar9 = DAT_800e8430;
    do {
      puVar8 = puVar9;
      iVar10 = 0xe;
      if ((byte)*param_1 == 0x20) {
        puVar9 = puVar8;
        if ((uVar1 & 0x4000) != 0) {
          param_2 = param_2 - (int)DAT_8009de34;
          iVar10 = (int)DAT_8009de35 + (int)DAT_8009de34;
        }
      }
      else {
        uVar3 = (byte)*param_1 - 0x24;
        if (((uVar1 & 0x4000) != 0) && (iVar4 = uVar3 * 2, uVar3 < 0x57)) {
          param_2 = param_2 - (int)(&DAT_8009de34)[iVar4];
          iVar10 = (int)(&DAT_8009de35)[iVar4] + (int)(&DAT_8009de34)[iVar4];
        }
        puVar8[1] = uVar5 << 0x19 | 0x64808080;
        puVar8[2] = param_2;
        uVar2 = *param_1;
        uVar3 = (uint)DAT_800e8430 & 0xffffff;
        DAT_800e8430 = DAT_800e8430 + 5;
        puVar8[4] = 0xe000e;
        puVar8[3] = ((uint)(byte)uVar2 % 0x12) * 0xe + ((byte)uVar2 / 0x12) * 0xe00 +
                    ((((uVar1 & 0xf0) >> 3) + uVar5 + 0x1e0) * 0x40 | 0x10) * 0x10000;
        *puVar7 = *puVar7 & 0xff000000 | uVar3;
        puVar9 = puVar8 + 5;
        puVar7 = puVar8;
        if (DAT_800e8430 == (uint *)(&UNK_800e4030 + uVar6 * 0x1400)) break;
      }
      puVar8 = puVar7;
      param_1 = (ushort *)((int)param_1 + 1);
      param_2 = param_2 + iVar10;
      puVar7 = puVar8;
    } while (*(byte *)param_1 != 0);
    uVar6 = (uint)(param_1 + 1) & 0xfffffffe;
    *puVar8 = *puVar8 & 0xff000000 | uVar11 & 0xffffff;
  }
  return uVar6;
}


