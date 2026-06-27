/* _SsSetProgramChange @ 0x8007c5c8  (Ghidra headless, raw MIPS overlay) */

void _SsSetProgramChange(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  *(uchar *)(iVar2 + (uint)*(byte *)(iVar2 + 0x17) + 0x37) = param_3;
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(iVar2 + 0x90) = uVar1;
  return;
}


