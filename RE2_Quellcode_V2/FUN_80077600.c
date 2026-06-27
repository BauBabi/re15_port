/* FUN_80077600 @ 0x80077600  (Ghidra headless, raw MIPS overlay) */

void FUN_80077600(int param_1)

{
  DAT_800c3c24 = 0;
  DAT_800c3c20 = (short)((int)((uint)*(ushort *)(param_1 + 0x90) * 0xb3) >> 7);
  FUN_80077384((int)*(short *)(param_1 + 0x76),&DAT_800c3c20,&DAT_800c3c20);
  *(short *)(param_1 + 0x15c) = DAT_800c3c20 + (short)*(undefined4 *)(param_1 + 0x38);
  *(short *)(param_1 + 0x160) = DAT_800c3c24 + (short)*(undefined4 *)(param_1 + 0x40);
  return;
}


