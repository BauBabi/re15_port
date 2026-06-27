void _SsSndDecrescendo(ushort param_1,int param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  ushort local_30;
  ushort local_2e [11];
  
  piVar6 = (int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar5 = (short)param_2 * 0xac;
  iVar7 = iVar5 + *piVar6;
  uVar4 = (uint)*(short *)(iVar7 + 0x42);
  uVar2 = *(int *)(iVar7 + 0x98) - 1;
  *(uint *)(iVar7 + 0x98) = uVar2;
  if ((int)uVar4 < 1) {
    iVar3 = *(ushort *)(iVar7 + 0x40) + uVar4;
    *(short *)(iVar7 + 0x40) = (short)iVar3;
    if (0 < iVar3 * 0x10000) {
      iVar3 = (int)(short)(param_1 | (ushort)(param_2 << 8));
      SpuVmGetSeqVol(iVar3,&local_30,local_2e);
      iVar5 = (int)*(short *)(iVar7 + 0x42);
      if (((uint)((*(int *)(iVar7 + 0x94) - *(int *)(iVar7 + 0x98)) * -iVar5) <=
           (uint)(int)*(short *)(iVar7 + 0x3e)) && (-iVar5 < (int)(uint)local_30)) {
        SpuVmSetSeqVol(iVar3,(uint)local_30 + iVar5 & 0xffff,(uint)local_2e[0] + iVar5 & 0xffff,0);
        DECRE_OBJ_208();
        return;
      }
DECRE_OBJ_1C4:
      SpuVmSetSeqVol((int)(short)(param_1 | (ushort)(param_2 << 8)),1,1,0);
      DECRE_OBJ_208();
      return;
    }
  }
  else {
    if (uVar4 == 0) {
      trap(0x1c00);
    }
    if (uVar2 % uVar4 != 0) goto DECRE_OBJ_270;
    uVar1 = *(short *)(iVar7 + 0x40) - 1;
    *(ushort *)(iVar7 + 0x40) = uVar1;
    if (0 < (int)((uint)uVar1 << 0x10)) {
      iVar5 = (int)(short)(param_1 | (ushort)(param_2 << 8));
      SpuVmGetSeqVol(iVar5,&local_30,local_2e);
      if (((0 < (int)((uint)local_30 - (int)*(short *)(iVar7 + 0x40))) &&
          (0 < (int)((uint)local_2e[0] - (int)*(short *)(iVar7 + 0x40)))) && (local_30 != 1)) {
        DECRE_OBJ_1B4(iVar5,local_30 - 1,local_2e[0] + 0xffff,0);
        return;
      }
      goto DECRE_OBJ_1C4;
    }
  }
  iVar5 = iVar5 + *piVar6;
  *(uint *)(iVar5 + 0x90) = *(uint *)(iVar5 + 0x90) & 0xffffffdf;
  if ((*(int *)(iVar7 + 0x98) == 0) || (*(short *)(iVar7 + 0x40) == 0)) {
    iVar5 = (short)param_2 * 0xac +
            *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
    *(uint *)(iVar5 + 0x90) = *(uint *)(iVar5 + 0x90) & 0xffffffdf;
  }
DECRE_OBJ_270:
  SpuVmGetSeqVol((int)(short)(param_1 | (ushort)(param_2 << 8)),iVar7 + 0x78,iVar7 + 0x7a);
  return;
}
