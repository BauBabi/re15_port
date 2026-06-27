/* _SsSndSetReplayMode @ 0x8007cc64  (Ghidra headless, raw MIPS overlay) */

void _SsSndSetReplayMode(int param_1,short param_2)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  
  piVar1 = (int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe));
  iVar3 = param_2 * 0xb0;
  uVar2 = *(uint *)(*piVar1 + iVar3 + 0x98);
  if (((uVar2 & 0x204) == 0) && ((uVar2 & 0x100) == 0)) {
    *(uint *)(*piVar1 + iVar3 + 0x98) = uVar2 & 0xfffffffd;
    *(uint *)(iVar3 + *piVar1 + 0x98) = *(uint *)(iVar3 + *piVar1 + 0x98) | 8;
    *(uint *)(iVar3 + *piVar1 + 0x98) = *(uint *)(iVar3 + *piVar1 + 0x98) | 1;
  }
  return;
}


