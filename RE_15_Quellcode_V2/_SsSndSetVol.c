void _SsSndSetVol(ushort param_1,short param_2,undefined2 param_3,undefined2 param_4)

{
  SpuVmSetSeqVol((int)(short)(param_1 | param_2 << 8),param_3,param_4,0);
  return;
}
