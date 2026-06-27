void SSSTART_OBJ_E0(void)

{
  code *pcVar1;
  
  if (DAT_800776d8 == 0) {
    if (DAT_800776d4 < 0x46) {
      if (DAT_800776d4 == 0) {
        trap(0x1c00);
      }
      DAT_800776e5 = DAT_800776e5 + '\x01';
      SSSTART_OBJ_184();
      return;
    }
    if (DAT_800776d4 == 0) {
      trap(0x1c00);
    }
    FUN_8006e170();
    ResetRCnt();
    SetRCnt();
    pcVar1 = _SsSeqCalledTbyT_1per2;
    if (DAT_800776e5 == '\0') {
      pcVar1 = SsSeqCalledTbyT;
    }
    InterruptCallback(0xff,pcVar1);
    FUN_8006e468();
  }
  return;
}
