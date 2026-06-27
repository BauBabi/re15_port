void SsSetRVol(short param_1,short param_2)

{
  SpuReverbAttr local_20;
  
  local_20.depth.left = (short)((param_1 * 0x7fff) / 0x7f);
  local_20.mask = 6;
  local_20.depth.right = (short)((param_2 * 0x7fff) / 0x7f);
  SpuSetReverbDepth(&local_20);
  return;
}
