/* _SsContResetAll @ 0x8007b3b0  (Ghidra headless, raw MIPS overlay) */

void _SsContResetAll(short param_1,short param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  FUN_80081328();
  FUN_80081efc();
  *(byte *)((uint)*(byte *)(iVar2 + 0x17) + iVar2 + 0x37) = *(byte *)(iVar2 + 0x17);
  *(undefined1 *)(iVar2 + 0x18) = 0;
  *(undefined1 *)(iVar2 + 0x19) = 0;
  *(undefined2 *)((uint)*(byte *)(iVar2 + 0x17) * 2 + iVar2 + 0x60) = 0x7f;
                    /* Possible PsyQ macro: setLineF2() */
  *(undefined1 *)(iVar2 + (uint)*(byte *)(iVar2 + 0x17) + 0x27) = 0x40;
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar2 + 0x90) = uVar1;
  return;
}


