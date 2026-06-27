short SsVabTransCompleted(short param_1)

{
  long lVar1;
  
  lVar1 = SpuIsTransferCompleted((int)param_1);
  return (short)lVar1;
}
