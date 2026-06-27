void FUN_8002441c(int param_1,uint param_2,uint param_3)

{
  byte *pbVar1;
  int iVar2;
  
  iVar2 = *(int *)(*(int *)(param_1 + 4) + 0x14);
  if (iVar2 != 0) {
    pbVar1 = (byte *)((uint)DAT_800aca34 * 0x28 + *(int *)(param_1 + 8) + 0x1e);
    do {
      iVar2 = iVar2 + -1;
      pbVar1[-0xe] = (byte)((uint)pbVar1[-0xe] + (param_2 & 0xff) >> 1);
      pbVar1[-0xd] = (byte)((uint)pbVar1[-0xd] + ((param_2 & 0xff00) >> 8) >> 1);
      pbVar1[-0xc] = (byte)((uint)pbVar1[-0xc] + ((param_2 & 0xff0000) >> 0x10) >> 1);
      pbVar1[-2] = (byte)((uint)pbVar1[-2] + (param_3 & 0xff) >> 1);
      pbVar1[-1] = (byte)((uint)pbVar1[-1] + ((param_3 & 0xff00) >> 8) >> 1);
      *pbVar1 = (byte)((uint)*pbVar1 + ((param_3 & 0xff0000) >> 0x10) >> 1);
      pbVar1 = pbVar1 + 0x50;
    } while (iVar2 != 0);
  }
  iVar2 = *(int *)(*(int *)(param_1 + 0xc) + 0x14);
  if (iVar2 != 0) {
    pbVar1 = (byte *)((uint)DAT_800aca34 * 0x34 + *(int *)(param_1 + 0x10) + 0x1e);
    do {
      iVar2 = iVar2 + -1;
      pbVar1[-0xe] = (byte)((uint)pbVar1[-0xe] + (param_2 & 0xff) >> 1);
      pbVar1[-0xd] = (byte)((uint)pbVar1[-0xd] + ((param_2 & 0xff00) >> 8) >> 1);
      pbVar1[-0xc] = (byte)((uint)pbVar1[-0xc] + ((param_2 & 0xff0000) >> 0x10) >> 1);
      pbVar1[-2] = (byte)((uint)pbVar1[-2] + (param_3 & 0xff) >> 1);
      pbVar1[-1] = (byte)((uint)pbVar1[-1] + ((param_3 & 0xff00) >> 8) >> 1);
      *pbVar1 = (byte)((uint)*pbVar1 + ((param_3 & 0xff0000) >> 0x10) >> 1);
      pbVar1 = pbVar1 + 0x68;
    } while (iVar2 != 0);
  }
  return;
}
