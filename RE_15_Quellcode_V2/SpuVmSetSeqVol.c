int SpuVmSetSeqVol(uint param_1,short param_2,short param_3,short param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  DAT_800b532a = (short)param_1;
  iVar3 = ((param_1 << 0x10) >> 0x18) * 0xac + (&DAT_800bb500)[param_1 & 0xff];
  *(short *)(iVar3 + 0x74) = param_2;
  *(short *)(iVar3 + 0x76) = param_3;
  if (0x7f < *(ushort *)(iVar3 + 0x74)) {
    *(undefined2 *)(iVar3 + 0x74) = 0x7f;
  }
  if (0x7f < *(ushort *)(iVar3 + 0x76)) {
    *(undefined2 *)(iVar3 + 0x76) = 0x7f;
  }
  if ((param_4 == 1) && (iVar3 = 0, DAT_800b52a8 != 0)) {
    iVar1 = 0;
    do {
      iVar1 = iVar1 >> 0x10;
      if ((uint)(ushort)(&DAT_8008f852)[iVar1 * 0x1a] == ((int)(param_1 << 0x10) >> 0x10 & 0xffffU))
      {
        iVar2 = (iVar1 << 0x13) >> 0xf;
        *(short *)((int)&DAT_8008f6ac + iVar2) = param_2 * 0x81;
        *(short *)((int)&DAT_8008f6ae + iVar2) = param_3 * 0x81;
        (&DAT_8008f82c)[iVar1] = (&DAT_8008f82c)[iVar1] | 3;
      }
      iVar3 = iVar3 + 1;
      iVar1 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < (int)(uint)DAT_800b52a8);
  }
  return (int)DAT_800b532a;
}
