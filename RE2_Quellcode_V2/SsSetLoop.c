/* SsSetLoop @ 0x8007a47c  (Ghidra headless, raw MIPS overlay) */

void SsSetLoop(short param_1,short param_2,short param_3)

{
  int iVar1;
  
  iVar1 = *(int *)((int)&DAT_800ea250 + ((int)((uint)(ushort)param_1 << 0x10) >> 0xe)) +
          param_2 * 0xb0;
  *(char *)(iVar1 + 0x20) = (char)param_3;
  *(undefined1 *)(iVar1 + 0x21) = 0;
  return;
}


