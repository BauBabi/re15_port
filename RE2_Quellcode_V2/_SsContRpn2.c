/* _SsContRpn2 @ 0x8007b340  (Ghidra headless, raw MIPS overlay) */

void _SsContRpn2(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  *(uchar *)(iVar2 + 0x19) = param_3;
  *(char *)(iVar2 + 0x1e) = *(char *)(iVar2 + 0x1e) + '\x01';
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(iVar2 + 0x90) = uVar1;
  return;
}


