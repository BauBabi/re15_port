/* SsVabTransCompleted @ 0x80085dd0  (Ghidra headless, raw MIPS overlay) */

short SsVabTransCompleted(short param_1)

{
  long lVar1;
  
  lVar1 = SpuIsTransferCompleted((int)param_1);
  return (short)lVar1;
}


