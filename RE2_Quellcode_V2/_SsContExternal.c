/* _SsContExternal @ 0x8007afe4  (Ghidra headless, raw MIPS overlay) */

void _SsContExternal(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (&DAT_800ea250)[param_1];
  SsUtSetReverbDepth((ushort)param_3,(ushort)param_3);
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar2 + param_2 * 0xb0 + 0x90) = uVar1;
  return;
}


