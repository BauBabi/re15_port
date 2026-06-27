void SSSTART_OBJ_184(void)

{
  code *pcVar1;
  
  FUN_8006e170();
  ResetRCnt();
  SetRCnt();
  pcVar1 = _SsSeqCalledTbyT_1per2;
  if (DAT_800776e5 == '\0') {
    pcVar1 = SsSeqCalledTbyT;
  }
  InterruptCallback(0xff,pcVar1);
  FUN_8006e468();
  return;
}
