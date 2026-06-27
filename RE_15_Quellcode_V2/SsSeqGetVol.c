void SsSeqGetVol(short param_1,short param_2,short *param_3,short *param_4)

{
  SpuVmGetSeqVol((int)(short)(param_1 | param_2 << 8),param_3,param_4);
  return;
}
