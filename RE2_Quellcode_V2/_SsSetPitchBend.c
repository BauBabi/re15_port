/* _SsSetPitchBend @ 0x8007c03c  (Ghidra headless, raw MIPS overlay) */

void _SsSetPitchBend(short param_1,short param_2)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)((&DAT_800ea250)[param_1] + param_2 * 0xb0);
  puVar2 = (undefined1 *)*puVar3;
  *puVar3 = puVar2 + 1;
  FUN_80083e2c((int)(short)(param_1 | param_2 << 8),*(undefined1 *)((int)puVar3 + 0x26),
               *(undefined1 *)((int)puVar3 + *(byte *)((int)puVar3 + 0x17) + 0x37),*puVar2);
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  puVar3[0x24] = uVar1;
  return;
}


