/* DrawPrim @ 0x80090940  (Ghidra headless, raw MIPS overlay) */

void DrawPrim(void *p)

{
  undefined1 uVar1;
  
  uVar1 = *(undefined1 *)((int)p + 3);
  (**(code **)(PTR_PTR_800b26f8 + 0x3c))(0);
  (**(code **)(PTR_PTR_800b26f8 + 0x14))((int)p + 4,uVar1);
  return;
}


