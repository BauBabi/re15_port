void _SsSndReplay(int param_1,short param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  *(undefined1 *)(param_2 * 0xac + *piVar2 + 0x2b) = 1;
  iVar1 = param_2 * 0xac + *piVar2;
  *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xfffffff7;
  return;
}
