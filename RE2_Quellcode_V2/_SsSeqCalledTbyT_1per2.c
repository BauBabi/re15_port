/* _SsSeqCalledTbyT_1per2 @ 0x8007d6d0  (Ghidra headless, raw MIPS overlay) */

void _SsSeqCalledTbyT_1per2(void)

{
  if (DAT_800aba3c == 0) {
    DAT_800aba3c = 1;
    SSSTART_OBJ_328();
    return;
  }
  DAT_800aba3c = 0;
  (*(code *)PTR_SsSeqCalledTbyT_800aba30)();
  return;
}


