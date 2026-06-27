/* ClearImage @ 0x80090534  (Ghidra headless, raw MIPS overlay) */

int ClearImage(RECT *rect,u_char r,u_char g,u_char b)

{
  int iVar1;
  
  checkRECT("ClearImage",rect);
  iVar1 = (**(code **)(PTR_PTR_800b26f8 + 8))
                    (*(undefined4 *)(PTR_PTR_800b26f8 + 0xc),rect,8,
                     (uint)b << 0x10 | (uint)g << 8 | (uint)r);
  return iVar1;
}


