/* FUN_80081a24 @ 0x80081a24  (Ghidra headless, raw MIPS overlay) */

uint FUN_80081a24(int param_1,uint param_2,int param_3,int param_4)

{
  ushort uVar1;
  undefined *puVar2;
  ushort *puVar3;
  ushort uVar4;
  uint uVar5;
  
  puVar2 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  if ((DAT_800ab6e4 & 1) != 0) {
    puVar2 = &DAT_800d5a00;
  }
  uVar5 = (uint)*(ushort *)(puVar2 + param_3 * 2) |
          (*(ushort *)(puVar2 + param_4 * 2) & 0xff) << 0x10;
  uVar4 = (ushort)param_2;
  uVar1 = (ushort)(param_2 >> 0x10);
  if (param_1 == 1) {
    if ((DAT_800ab6e4 & 1) == 0) {
      puVar3 = (ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_4 * 2);
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_3 * 2) =
           *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_3 * 2) | uVar4;
      *puVar3 = *puVar3 | uVar1 & 0xff;
    }
    else {
      *(ushort *)(&DAT_800d5a00 + param_3 * 2) = *(ushort *)(&DAT_800d5a00 + param_3 * 2) | uVar4;
      *(ushort *)(&DAT_800d5a00 + param_4 * 2) =
           *(ushort *)(&DAT_800d5a00 + param_4 * 2) | uVar1 & 0xff;
      DAT_800ab2b0 = DAT_800ab2b0 | 1 << (param_3 + -0xc6 >> 1 & 0x1fU);
    }
    uVar5 = uVar5 | param_2 & 0xffffff;
  }
  else if (param_1 < 2) {
    if (param_1 == 0) {
      if ((DAT_800ab6e4 & 1) == 0) {
        puVar3 = (ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_4 * 2);
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_3 * 2) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_3 * 2) & ~uVar4;
        *puVar3 = *puVar3 & ~(uVar1 & 0xff);
      }
      else {
        *(ushort *)(&DAT_800d5a00 + param_3 * 2) = *(ushort *)(&DAT_800d5a00 + param_3 * 2) & ~uVar4
        ;
        *(ushort *)(&DAT_800d5a00 + param_4 * 2) =
             *(ushort *)(&DAT_800d5a00 + param_4 * 2) & ~(uVar1 & 0xff);
        DAT_800ab2b0 = DAT_800ab2b0 | 1 << (param_3 + -0xc6 >> 1 & 0x1fU);
      }
      uVar5 = uVar5 & ~(param_2 & 0xffffff);
    }
  }
  else if (param_1 == 8) {
    if ((DAT_800ab6e4 & 1) == 0) {
      puVar3 = (ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_4 * 2);
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_3 * 2) = uVar4;
      *puVar3 = uVar1 & 0xff;
    }
    else {
      *(ushort *)(&DAT_800d5a00 + param_3 * 2) = uVar4;
      *(ushort *)(&DAT_800d5a00 + param_4 * 2) = uVar1 & 0xff;
      DAT_800ab2b0 = DAT_800ab2b0 | 1 << (param_3 + -0xc6 >> 1 & 0x1fU);
    }
    uVar5 = param_2 & 0xffffff;
  }
  return uVar5;
}


