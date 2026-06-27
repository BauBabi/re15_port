void _SsSndSetReplayMode(int param_1,short param_2)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  
  piVar2 = (int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  iVar3 = param_2 * 0xac;
  uVar1 = *(uint *)(iVar3 + *piVar2 + 0x90);
  if (((uVar1 & 0x204) == 0) && ((uVar1 & 0x100) == 0)) {
    *(uint *)(iVar3 + *piVar2 + 0x90) = uVar1 & 0xfffffffd;
    *(uint *)(iVar3 + *piVar2 + 0x90) = *(uint *)(iVar3 + *piVar2 + 0x90) | 8;
    *(uint *)(iVar3 + *piVar2 + 0x90) = *(uint *)(iVar3 + *piVar2 + 0x90) | 1;
  }
  return;
}
