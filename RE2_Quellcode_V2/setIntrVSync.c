/* setIntrVSync @ 0x80086864  (Ghidra headless, raw MIPS overlay) */

void setIntrVSync(int param_1,int param_2)

{
  if (param_2 != (&DAT_800acce0)[param_1]) {
    (&DAT_800acce0)[param_1] = param_2;
  }
  return;
}


