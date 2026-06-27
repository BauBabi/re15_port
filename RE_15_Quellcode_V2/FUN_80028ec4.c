void FUN_80028ec4(uint param_1,int param_2,ushort *param_3,uint param_4)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  uint local_30 [4];
  
  iVar4 = (param_4 & 7) << 1;
  if ((param_4 & 7) == 0) {
    iVar4 = 6;
  }
  puVar5 = (uint *)(&DAT_800b829c + (uint)DAT_800aca34 * 0xc0 + iVar4 * 0xc);
  uVar7 = (param_4 & 0x30) << 3 | 0x7810;
  uVar2 = param_1;
  puVar6 = DAT_800a7394;
LAB_80028f68:
  while( true ) {
    while( true ) {
      while( true ) {
        while( true ) {
          param_3 = (ushort *)FUN_80013160(param_3,local_30);
          if (10 < (int)local_30[0]) goto LAB_80028fc0;
          if ((local_30[0] == 7) || (local_30[0] == 1)) {
            DAT_800a7394 = puVar6;
            return;
          }
          if (local_30[0] != 5) break;
          uVar1 = *param_3;
          param_3 = (ushort *)((int)param_3 + 1);
          uVar7 = ((byte)uVar1 & 3) << 7 | 0x7810 | ((byte)uVar1 & 4) << 4;
        }
        if (local_30[0] != 8) break;
        param_2 = param_2 + 0x10;
        uVar2 = param_1;
      }
      if (local_30[0] != 9) break;
      uVar2 = FUN_800c0214(param_3 + 1);
      uVar1 = *param_3;
      param_3 = param_3 + 1;
      uVar2 = (uVar1 + param_1) - (uVar2 >> 1);
    }
    if (local_30[0] != 10) break;
    iVar4 = FUN_800c0214(param_3 + 1);
    uVar1 = *param_3;
    param_3 = param_3 + 1;
    uVar2 = (uVar1 + param_1) - iVar4;
  }
  goto LAB_80029074;
LAB_80028fc0:
  if (local_30[0] != 0xfb) {
    if (((int)local_30[0] < 0xfc) || (local_30[0] != 0xfc)) {
LAB_80029074:
      *(byte *)((int)puVar6 + 0xd) = ((byte)local_30[0] & 0xf0) + 0x20;
      puVar6[1] = 0x7c808080;
      puVar6[2] = uVar2 & 0xffff | param_2 << 0x10;
      puVar6[4] = 0;
      *(short *)((int)puVar6 + 0xe) = (short)uVar7;
      *(char *)(puVar6 + 3) = (char)((local_30[0] & 0xf) << 4);
                    /* Probable PsyQ macro: addPrim(). */
      *puVar6 = *puVar6 & 0xff000000 | *puVar5 & 0xffffff;
      uVar3 = (uint)puVar6 & 0xffffff;
      puVar6 = puVar6 + 5;
      *puVar5 = *puVar5 & 0xff000000 | uVar3;
      uVar2 = (uVar2 & 0xffff) + (uint)(byte)(&DAT_800c4416)[local_30[0]];
    }
    else {
      uVar2 = uVar2 + 7;
    }
  }
  goto LAB_80028f68;
}
