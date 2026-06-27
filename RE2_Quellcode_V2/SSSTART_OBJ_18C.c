/* SSSTART_OBJ_18C @ 0x8007d574  (Ghidra headless, raw MIPS overlay) */

void SSSTART_OBJ_18C(void)

{
  code *pcVar1;
  
  if (DAT_800aba38 != '\0') {
    FUN_800957a4();
    VSyncCallback((f *)PTR_SsSeqCalledTbyT_800aba30);
    SSSTART_OBJ_248();
    return;
  }
  FUN_800957a4();
  FUN_8009599c();
  FUN_80095864();
  if (DAT_800aba3a == '\0') {
    DAT_800aba34 = InterruptCallback(0,0);
    SSSTART_OBJ_240(DAT_800aba3a,_SsTrapIntrVSync);
    return;
  }
  pcVar1 = _SsSeqCalledTbyT_1per2;
  if (DAT_800aba39 == '\0') {
    pcVar1 = (code *)PTR_SsSeqCalledTbyT_800aba30;
  }
  InterruptCallback(DAT_800aba3a,pcVar1);
  FUN_800957b4();
  return;
}


