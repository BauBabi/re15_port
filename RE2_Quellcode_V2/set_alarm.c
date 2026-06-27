/* set_alarm @ 0x80092ccc  (Ghidra headless, raw MIPS overlay) */

void set_alarm(void)

{
  int iVar1;
  
  iVar1 = VSync(-1);
  DAT_800b2818 = iVar1 + 0xf0;
  DAT_800b281c = 0;
  return;
}


