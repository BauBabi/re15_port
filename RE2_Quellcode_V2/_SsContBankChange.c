/* _SsContBankChange @ 0x8007a81c  (Ghidra headless, raw MIPS overlay) */

void _SsContBankChange(short param_1,short param_2)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)((&DAT_800ea250)[param_1] + param_2 * 0xb0);
  puVar2 = (undefined1 *)*puVar3;
  *puVar3 = puVar2 + 1;
  *(undefined1 *)((int)puVar3 + 0x26) = *puVar2;
  uVar1 = _SsReadDeltaValue();
  puVar3[0x24] = uVar1;
  return;
}


