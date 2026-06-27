short SsVabOpen(uchar *param_1,VabHdr *param_2)

{
  short sVar1;
  
  sVar1 = SsVabOpenHead(param_1,-1);
  if (sVar1 != -1) {
    sVar1 = SsVabTransBody(*(uchar **)(&DAT_800bbefc + sVar1 * 4),sVar1);
  }
  return sVar1;
}
