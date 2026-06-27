/* _SsContPanpot @ 0x8007ad8c  (Ghidra headless, raw MIPS overlay) */

void _SsContPanpot(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  iVar3 = iVar2 + (uint)*(byte *)(iVar2 + 0x17);
  FUN_8008482c((int)(short)(param_1 | param_2 << 8),*(undefined1 *)(iVar2 + 0x26),
               *(undefined1 *)(iVar3 + 0x37),
               *(undefined2 *)((uint)*(byte *)(iVar2 + 0x17) * 2 + iVar2 + 0x60),param_3);
  *(uchar *)(iVar3 + 0x27) = param_3;
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar2 + 0x90) = uVar1;
  return;
}


