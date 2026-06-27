/* MargePrim @ 0x8008fce8  (Ghidra headless, raw MIPS overlay) */

int MargePrim(void *p0,void *p1)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)((int)p0 + 3) + (uint)*(byte *)((int)p1 + 3) + 1;
  if (uVar2 < 0x11) {
    *(char *)((int)p0 + 3) = (char)uVar2;
    *(undefined4 *)p1 = 0;
    iVar1 = PRIM_OBJ_52C();
    return iVar1;
  }
  return -1;
}


