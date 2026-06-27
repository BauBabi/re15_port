void _SsSndCrescendo(ushort param_1,int param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  ushort local_30;
  ushort local_2e [11];
  
  piVar7 = (int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar6 = (short)param_2 * 0xac;
  iVar5 = iVar6 + *piVar7;
  uVar4 = (uint)*(short *)(iVar5 + 0x42);
  uVar2 = *(int *)(iVar5 + 0x98) - 1;
  *(uint *)(iVar5 + 0x98) = uVar2;
  if ((int)uVar4 < 1) {
    if (-1 < (int)uVar4) goto CRES_OBJ_2A0;
    iVar3 = *(ushort *)(iVar5 + 0x40) + uVar4;
    *(short *)(iVar5 + 0x40) = (short)iVar3;
    if (iVar3 * 0x10000 < 0) goto CRES_OBJ_1F8;
    iVar6 = (int)(short)(param_1 | (ushort)(param_2 << 8));
    SpuVmGetSeqVol(iVar6,&local_30,local_2e);
    if ((0x7e < (int)((uint)local_30 - (int)*(short *)(iVar5 + 0x42))) &&
       (0x7e < (int)((uint)local_2e[0] - (int)*(short *)(iVar5 + 0x42)))) {
      SpuVmSetSeqVol(iVar6,0x7f,0x7f,0);
    }
    iVar6 = (int)*(short *)(iVar5 + 0x42);
    if ((uint)((*(int *)(iVar5 + 0x94) - *(int *)(iVar5 + 0x98)) * -iVar6) <
        (uint)(int)*(short *)(iVar5 + 0x3e)) {
      SpuVmSetSeqVol((int)(short)(param_1 | (ushort)(param_2 << 8)),(uint)local_30 - iVar6 & 0xffff,
                     (uint)local_2e[0] - iVar6 & 0xffff,0);
      CRES_OBJ_234();
      return;
    }
  }
  else {
    if (uVar4 == 0) {
      trap(0x1c00);
    }
    if (uVar2 % uVar4 != 0) goto CRES_OBJ_2A0;
    uVar1 = *(short *)(iVar5 + 0x40) - 1;
    *(ushort *)(iVar5 + 0x40) = uVar1;
    if ((int)((uint)uVar1 << 0x10) < 0) {
CRES_OBJ_1F8:
      SpuVmSetSeqVol((int)(short)(param_1 | (ushort)(param_2 << 8)),0x7f,0x7f,0);
      iVar6 = iVar6 + *piVar7;
      *(uint *)(iVar6 + 0x90) = *(uint *)(iVar6 + 0x90) & 0xffffffef;
    }
    else {
      iVar6 = (int)(short)(param_1 | (ushort)(param_2 << 8));
      SpuVmGetSeqVol(iVar6,&local_30,local_2e);
      if ((int)(local_30 + 1) <= (int)((uint)local_30 + (int)*(short *)(iVar5 + 0x40))) {
        CRES_OBJ_1E8(iVar6,local_30 + 1,local_2e[0] + 1,0);
        return;
      }
    }
  }
  if ((*(int *)(iVar5 + 0x98) == 0) || (*(short *)(iVar5 + 0x40) == 0)) {
    iVar6 = (short)param_2 * 0xac +
            *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
    *(uint *)(iVar6 + 0x90) = *(uint *)(iVar6 + 0x90) & 0xffffffef;
  }
CRES_OBJ_2A0:
  SpuVmGetSeqVol((int)(short)(param_1 | (ushort)(param_2 << 8)),iVar5 + 0x78,iVar5 + 0x7a);
  return;
}
