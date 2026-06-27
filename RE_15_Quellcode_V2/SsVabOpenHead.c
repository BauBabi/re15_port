short SsVabOpenHead(uchar *param_1,short param_2)

{
  short sVar1;
  
  sVar1 = SsVabOpenHeadWithMode(param_1,(int)param_2,0,0);
  return sVar1;
}
