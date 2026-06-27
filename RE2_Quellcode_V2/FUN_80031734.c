/* FUN_80031734 @ 0x80031734  (Ghidra headless, raw MIPS overlay) */

uint FUN_80031734(ushort *param_1,uint param_2,int param_3)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  
  uVar3 = (uint)DAT_800ce5e0;
  if (DAT_800e8434 == (uint *)(&UNK_800e6c30 + uVar3 * 0x1800)) {
    uVar3 = *(uint *)(param_3 + 0x5c88);
  }
  else {
    uVar1 = *param_1;
    param_1 = param_1 + 1;
    uVar4 = uVar1 >> 8 & 1;
    puVar5 = (uint *)(param_3 + uVar3 * 0xc0 + (uVar1 & 7) * 0x18);
    uVar8 = *puVar5;
    puVar7 = DAT_800e8434;
    do {
      puVar6 = puVar7;
      if ((byte)*param_1 != 0x20) {
        puVar7[1] = uVar4 << 0x19 | 0x74808080;
        puVar7[2] = param_2;
        uVar2 = (byte)*param_1 - 0x20;
        puVar7[3] = (uVar2 & 0x1f) * 8 + (uVar2 & 0xe0) * 0x40 +
                    ((((uVar1 & 0xf0) >> 3) + uVar4 + 0x1e0) * 0x40 | 0x10) * 0x10000;
        uVar2 = (uint)DAT_800e8434 & 0xffffff;
        DAT_800e8434 = DAT_800e8434 + 4;
        *puVar5 = *puVar5 & 0xff000000 | uVar2;
        puVar6 = puVar7 + 4;
        puVar5 = puVar7;
        if (DAT_800e8434 == (uint *)(&UNK_800e6c30 + uVar3 * 0x1800)) break;
      }
      param_1 = (ushort *)((int)param_1 + 1);
      param_2 = param_2 + 8;
      puVar7 = puVar6;
    } while (*(byte *)param_1 != 0);
    uVar3 = (uint)(param_1 + 1) & 0xfffffffe;
    *puVar5 = *puVar5 & 0xff000000 | uVar8 & 0xffffff;
  }
  return uVar3;
}


