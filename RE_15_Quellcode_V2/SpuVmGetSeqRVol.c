int SpuVmGetSeqRVol(uint param_1)

{
  DAT_800b532a = (short)param_1;
  return (int)*(short *)(((int)(param_1 & 0xff00) >> 8) * 0xac + (&DAT_800bb500)[param_1 & 0xff] +
                        0x76);
}
