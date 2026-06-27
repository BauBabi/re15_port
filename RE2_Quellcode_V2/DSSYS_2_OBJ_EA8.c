/* DSSYS_2_OBJ_EA8 @ 0x8008a95c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8008aaa8) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 DSSYS_2_OBJ_EA8(void)

{
  undefined4 uVar1;
  int in_v1;
  int iVar2;
  int iVar3;
  int iVar4;
  int unaff_s0;
  
  if (in_v1 != 7) {
    return 0;
  }
  iVar2 = DAT_800c3f04 + -1;
  if (iVar2 < 0) {
    iVar2 = 7;
  }
  iVar3 = 0;
  if (unaff_s0 == 0) {
    iVar4 = iVar2 * 0x10;
    if ((&DAT_800c3e84)[iVar2 * 4] == 0) {
      uVar1 = DSSYS_2_OBJ_FD8();
      return uVar1;
    }
  }
  else {
    iVar4 = iVar2 << 4;
    while (iVar2 = iVar2 + -1, *(int *)((int)&DAT_800c3e84 + iVar4) != unaff_s0) {
      iVar4 = iVar4 + -0x10;
      if (iVar2 < 0) {
        iVar4 = 0x70;
        iVar2 = 7;
      }
      iVar3 = iVar3 + 1;
      if (7 < iVar3) {
        uVar1 = DSSYS_2_OBJ_FD8();
        return uVar1;
      }
    }
  }
  DAT_800c3d78 = *(undefined4 *)((int)&DAT_800c3e84 + iVar4);
  _DAT_800c3d7c = *(undefined4 *)(&DAT_800c3e88 + iVar4);
  DAT_800c3d80 = *(undefined4 *)(&DAT_800c3e8c + iVar4);
  DAT_800c3d84 = *(undefined4 *)(&UNK_800c3e90 + iVar4);
  rescpy();
  uVar1 = DSSYS_2_OBJ_FF8();
  return uVar1;
}


