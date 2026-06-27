uint FUN_8002939c(ushort *param_1,uint param_2)

{
  byte bVar1;
  ushort uVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint *puVar9;
  uint *puVar10;
  uint *puVar11;
  uint *puVar12;
  uint uVar13;
  
  uVar8 = (uint)DAT_800aca34;
  uVar6 = DAT_800b851c;
  if (DAT_800a7394 != (uint *)(&UNK_80090994 + uVar8 * 0xa00)) {
    uVar2 = *param_1;
    iVar5 = 6;
    if ((uVar2 & 7) != 0) {
      iVar5 = (uVar2 & 7) << 1;
    }
    uVar6 = (uint)((uVar2 & 0x40) != 0);
    uVar2 = *param_1;
    param_1 = param_1 + 1;
    puVar11 = (uint *)(&DAT_800b829c + uVar8 * 0xc0 + iVar5 * 0xc);
    uVar13 = *puVar11;
    puVar10 = DAT_800a7394;
    puVar12 = DAT_800a7394;
    while( true ) {
      uVar4 = (uint)(byte)*param_1;
      if (uVar4 == 1) {
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      }
      bVar1 = (&DAT_800c43f2)[uVar4];
      if (uVar4 == 0) break;
      puVar9 = puVar10;
      if (uVar4 != 0x20) {
        puVar10[1] = uVar6 << 0x19 | 0x64808080;
        puVar10[2] = param_2;
        uVar3 = *param_1;
        uVar7 = (uint)puVar12 & 0xffffff;
        puVar12 = puVar12 + 5;
        puVar10[4] = 0x100010;
        uVar4 = (byte)uVar3 - 0x24;
        puVar10[3] = ((uVar4 & 0xf) << 4 | ((uVar4 & 0xfffffff0) + 0x20) * 0x100) +
                     (((((byte)uVar2 & 0x30) >> 3) + uVar6 + 0x1e0) * 0x40 | 0x10) * 0x10000;
        *puVar11 = *puVar11 & 0xff000000 | uVar7;
        param_2 = param_2 + bVar1;
        puVar9 = puVar10 + 5;
        puVar11 = puVar10;
        if (puVar12 == (uint *)(&UNK_80090994 + uVar8 * 0xa00)) break;
      }
      param_1 = (ushort *)((int)param_1 + 1);
      puVar10 = puVar9;
    }
    *puVar11 = *puVar11 & 0xff000000 | uVar13 & 0xffffff;
    uVar6 = (uint)(param_1 + 1) & 0xfffffffe;
    DAT_800a7394 = puVar12;
  }
  return uVar6;
}
