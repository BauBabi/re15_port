int SpuVmGetSeqVol(uint param_1,undefined2 *param_2,undefined2 *param_3)

{
  int iVar1;
  
  DAT_800b532a = (short)param_1;
  iVar1 = ((int)(param_1 & 0xff00) >> 8) * 0xac + (&DAT_800bb500)[param_1 & 0xff];
  *param_2 = *(undefined2 *)(iVar1 + 0x74);
  *param_3 = *(undefined2 *)(iVar1 + 0x76);
  return (int)DAT_800b532a;
}
