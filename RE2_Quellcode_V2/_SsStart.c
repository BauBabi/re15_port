/* _SsStart @ 0x8007d3e8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007d588) */

void _SsStart(int param_1)

{
  bool bVar1;
  int iVar2;
  code *pcVar3;
  
  iVar2 = 0x3e6;
  do {
    bVar1 = -1 < iVar2;
    iVar2 = iVar2 + -1;
  } while (bVar1);
  DAT_800aba38 = 0;
  DAT_800aba3a = '\x06';
  DAT_800aba39 = '\0';
  DAT_800aba34 = 0;
  if (DAT_800aba28 == 2) {
    FUN_800957a4();
    FUN_8009599c(0xf2000002);
    FUN_80095864(0xf2000002,0x44e8,0x1000);
    if (DAT_800aba3a != '\0') {
      pcVar3 = _SsSeqCalledTbyT_1per2;
      if (DAT_800aba39 == '\0') {
        pcVar3 = (code *)PTR_SsSeqCalledTbyT_800aba30;
      }
      InterruptCallback(DAT_800aba3a,pcVar3);
      FUN_800957b4();
      return;
    }
    DAT_800aba34 = InterruptCallback(0,0);
    SSSTART_OBJ_240(DAT_800aba3a,_SsTrapIntrVSync);
    return;
  }
  if (DAT_800aba28 < 3) {
    if (DAT_800aba28 == 0) {
      DAT_800aba3a = 0x7f;
      SSSTART_OBJ_250();
      return;
    }
    SSSTART_OBJ_DC();
    return;
  }
  if (DAT_800aba28 != 3) {
    if (DAT_800aba28 != 5) {
      SSSTART_OBJ_DC();
      return;
    }
    DAT_800aba3a = 0;
    if (param_1 != 0) {
      SSSTART_OBJ_18C();
      return;
    }
    DAT_800aba38 = 1;
    SSSTART_OBJ_18C();
    return;
  }
  SSSTART_OBJ_18C();
  return;
}


