void _SsTrapIntrVSync(void)

{
  if (DAT_800776e0 != (code *)0x0) {
    (*DAT_800776e0)();
  }
  SsSeqCalledTbyT();
  return;
}
