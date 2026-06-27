void SsSetMVol(short param_1,short param_2)

{
  SpuCommonAttr local_30;
  
  local_30.mask = 3;
  local_30.mvol.left = param_1 * 0x81;
  local_30.mvol.right = param_2 * 0x81;
  SpuSetCommonAttr(&local_30);
  return;
}
