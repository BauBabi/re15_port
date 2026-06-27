/* _SsSndPause @ 0x8007e060  (Ghidra headless, raw MIPS overlay) */

void _SsSndPause(ushort param_1,short param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar1 = *piVar2;
  FUN_800846dc((int)(short)(param_1 | param_2 << 8));
  *(undefined1 *)(iVar1 + param_2 * 0xb0 + 0x14) = 0;
  iVar1 = param_2 * 0xb0 + *piVar2;
  *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xfffffffd;
  return;
}


