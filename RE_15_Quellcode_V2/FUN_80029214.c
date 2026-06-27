uint FUN_80029214(ushort *param_1,uint param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  uint uVar9;
  
  uVar7 = (uint)DAT_800aca34;
  uVar6 = DAT_800b851c;
  if (DAT_800a7398 != (uint *)(&DAT_80091b94 + uVar7 * 0x800)) {
    uVar1 = *param_1;
    iVar3 = 6;
    if ((uVar1 & 7) != 0) {
      iVar3 = (uVar1 & 7) << 1;
    }
    uVar6 = (uint)((uVar1 & 0x40) != 0);
    uVar1 = *param_1;
    param_1 = param_1 + 1;
    puVar8 = (uint *)(&DAT_800b829c + uVar7 * 0xc0 + iVar3 * 0xc);
    uVar9 = *puVar8;
    puVar5 = DAT_800a7398;
    do {
      puVar4 = puVar5;
      if ((byte)*param_1 != 0x20) {
        puVar5[1] = uVar6 << 0x19 | 0x74808080;
        puVar5[2] = param_2;
        uVar2 = (byte)*param_1 - 0x20;
        puVar5[3] = (uVar2 & 0x1f) * 8 + (uVar2 & 0xe0) * 0x40 +
                    (((((byte)uVar1 & 0x30) >> 3) + uVar6 + 0x1e0) * 0x40 | 0x10) * 0x10000;
        uVar2 = (uint)DAT_800a7398 & 0xffffff;
        DAT_800a7398 = DAT_800a7398 + 4;
        *puVar8 = *puVar8 & 0xff000000 | uVar2;
        puVar4 = puVar5 + 4;
        puVar8 = puVar5;
        if (DAT_800a7398 == (uint *)(&DAT_80091b94 + uVar7 * 0x800)) break;
      }
      param_1 = (ushort *)((int)param_1 + 1);
      param_2 = param_2 + 8;
      puVar5 = puVar4;
    } while (*(byte *)param_1 != 0);
    *puVar8 = *puVar8 & 0xff000000 | uVar9 & 0xffffff;
    uVar6 = (uint)(param_1 + 1) & 0xfffffffe;
  }
  return uVar6;
}
