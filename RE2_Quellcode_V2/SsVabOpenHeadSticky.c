/* SsVabOpenHeadSticky @ 0x80085368  (Ghidra headless, raw MIPS overlay) */

short SsVabOpenHeadSticky(uchar *param_1,short param_2,ulong param_3)

{
  short sVar1;
  
  sVar1 = SsVabOpenHeadWithMode(param_1,(int)param_2,1,param_3);
  return sVar1;
}


