void _SsSndNextSep(int param_1,short param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  piVar1 = (int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  iVar2 = param_2 * 0xac;
  iVar3 = iVar2 + *piVar1;
  *(undefined2 *)(iVar3 + 0x46) = 1;
  *(undefined2 *)(iVar3 + 0x48) = 0;
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) & 0xfffffeff;
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) & 0xfffffff7;
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) & 0xfffffffd;
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) & 0xfffffffb;
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) & 0xfffffdff;
  *(undefined1 *)(iVar3 + 0x2b) = 1;
  *(undefined4 *)(iVar3 + 4) = *(undefined4 *)(iVar3 + 8);
  *(uint *)(iVar2 + *piVar1 + 0x90) = *(uint *)(iVar2 + *piVar1 + 0x90) | 1;
  return;
}
