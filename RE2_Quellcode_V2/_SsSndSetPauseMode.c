/* _SsSndSetPauseMode @ 0x8007ca34  (Ghidra headless, raw MIPS overlay) */

void _SsSndSetPauseMode(ushort param_1,short param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar1 = param_2 * 0xb0;
  FUN_800845e8((int)(short)(param_1 | param_2 << 8),*piVar2 + iVar1 + 0x5c,*piVar2 + iVar1 + 0x5e);
  *(uint *)(iVar1 + *piVar2 + 0x98) = *(uint *)(iVar1 + *piVar2 + 0x98) & 0xfffffffe;
  *(uint *)(iVar1 + *piVar2 + 0x98) = *(uint *)(iVar1 + *piVar2 + 0x98) & 0xfffffff7;
  *(uint *)(iVar1 + *piVar2 + 0x98) = *(uint *)(iVar1 + *piVar2 + 0x98) | 2;
  return;
}


