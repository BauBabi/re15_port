/* _SsTrapIntrVSync @ 0x8007d690  (Ghidra headless, raw MIPS overlay) */

void _SsTrapIntrVSync(void)

{
  if (DAT_800aba34 != (code *)0x0) {
    (*DAT_800aba34)();
  }
  (*(code *)PTR_SsSeqCalledTbyT_800aba30)();
  return;
}


