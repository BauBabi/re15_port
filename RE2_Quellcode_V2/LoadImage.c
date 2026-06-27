/* LoadImage @ 0x80090664  (Ghidra headless, raw MIPS overlay) */

int LoadImage(RECT *rect,u_long *p)

{
  int iVar1;
  
  checkRECT("LoadImage",rect);
  iVar1 = (**(code **)(PTR_PTR_800b26f8 + 8))(*(undefined4 *)(PTR_PTR_800b26f8 + 0x20),rect,8,p);
  return iVar1;
}


