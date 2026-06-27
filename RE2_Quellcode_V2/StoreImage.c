/* StoreImage @ 0x800906c8  (Ghidra headless, raw MIPS overlay) */

int StoreImage(RECT *rect,u_long *p)

{
  int iVar1;
  
  checkRECT("StoreImage",rect);
  iVar1 = (**(code **)(PTR_PTR_800b26f8 + 8))(*(undefined4 *)(PTR_PTR_800b26f8 + 0x1c),rect,8,p);
  return iVar1;
}


