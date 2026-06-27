/* note2pitch @ 0x80082f4c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x80082f70) */

uint note2pitch(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = (int)(((DAT_800dcc32 + 0x3c) - (uint)DAT_800dcc40) * 0x10000) >> 0x10;
  uVar2 = (uint)(DAT_800dcc41 >> 3);
  if (0xf < uVar2) {
    uVar2 = 0xf;
  }
  uVar2 = (uint)*(ushort *)(&DAT_800aba40 + (((iVar1 % 0xc) * 0x10000 >> 0xc) + uVar2) * 2);
  iVar1 = (iVar1 / 0xc + -5) * 0x10000 >> 0x10;
  if (iVar1 < 1) {
    if (iVar1 < 0) {
      uVar2 = (int)uVar2 >> (-iVar1 & 0x1fU);
    }
    return uVar2 & 0xffff;
  }
  uVar2 = VM_N2P_OBJ_BC();
  return uVar2;
}


