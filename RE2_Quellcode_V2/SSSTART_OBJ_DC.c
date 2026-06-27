/* SSSTART_OBJ_DC @ 0x8007d4c4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007d564) */
/* WARNING: Removing unreachable block (ram,0x8007d518) */

void SSSTART_OBJ_DC(void)

{
  code *pcVar1;
  
  if (DAT_800aba2c == 0) {
    if (DAT_800aba28 < 0x46) {
      if (DAT_800aba28 == 0) {
        trap(0x1c00);
      }
      DAT_800aba39 = DAT_800aba39 + '\x01';
      SSSTART_OBJ_18C();
      return;
    }
    if (DAT_800aba28 == 0) {
      trap(0x1c00);
    }
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
  }
  return;
}


