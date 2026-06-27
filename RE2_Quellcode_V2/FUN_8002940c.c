/* FUN_8002940c @ 0x8002940c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002940c(int param_1,uint param_2,uint param_3)

{
  byte *pbVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  
  iVar2 = *(int *)(*(int *)(param_1 + 4) + 0x14);
  bVar4 = (byte)(param_2 >> 8);
  bVar3 = (byte)(param_2 >> 0x10);
  if (iVar2 != 0) {
    pbVar1 = (byte *)(*(int *)(param_1 + 8) + (uint)DAT_800ce5e0 * 0x28 + 0x1e);
    do {
      iVar2 = iVar2 + -1;
      pbVar1[-0xe] = (byte)param_2;
      pbVar1[-0xd] = bVar4;
      pbVar1[-0xc] = bVar3;
      pbVar1[-2] = (byte)((uint)pbVar1[-2] + (param_3 & 0xff) >> 1);
      pbVar1[-1] = (byte)((uint)pbVar1[-1] + (param_3 >> 8 & 0xff) >> 1);
      *pbVar1 = (byte)((uint)*pbVar1 + (param_3 >> 0x10 & 0xff) >> 1);
      pbVar1 = pbVar1 + 0x50;
    } while (iVar2 != 0);
  }
  iVar2 = *(int *)(*(int *)(param_1 + 0xc) + 0x14);
  if (iVar2 != 0) {
    pbVar1 = (byte *)(*(int *)(param_1 + 0x10) + (uint)DAT_800ce5e0 * 0x34 + 0x2a);
    do {
      iVar2 = iVar2 + -1;
      pbVar1[-2] = (byte)param_2;
      pbVar1[-1] = bVar4;
      *pbVar1 = bVar3;
      pbVar1[-0x1a] = (byte)((uint)pbVar1[-0x1a] + (param_2 & 0xff) >> 1);
      pbVar1[-0x19] = (byte)((uint)pbVar1[-0x19] + (param_2 >> 8 & 0xff) >> 1);
      pbVar1[-0x18] = (byte)((uint)pbVar1[-0x18] + (param_2 >> 0x10 & 0xff) >> 1);
      pbVar1[-0xe] = (byte)((uint)pbVar1[-0xe] + (param_3 & 0xff) >> 1);
      pbVar1[-0xd] = (byte)((uint)pbVar1[-0xd] + (param_3 >> 8 & 0xff) >> 1);
      pbVar1[-0xc] = (byte)((uint)pbVar1[-0xc] + (param_3 >> 0x10 & 0xff) >> 1);
      pbVar1 = pbVar1 + 0x68;
    } while (iVar2 != 0);
  }
  return;
}


