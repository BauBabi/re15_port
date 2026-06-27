/* SsVabOpenHead @ 0x80085338  (Ghidra headless, raw MIPS overlay) */

short SsVabOpenHead(uchar *param_1,short param_2)

{
  short sVar1;
  
  sVar1 = SsVabOpenHeadWithMode(param_1,(int)param_2,0,0);
  return sVar1;
}


