/* FUN_8008de04 @ 0x8008de04  (Ghidra headless, raw MIPS overlay) */

/* Possible REG12.OBJ/SetGeomOffset */

void FUN_8008de04(int param_1,int param_2)

{
  gte_ldOFX(param_1 << 0x10);
  gte_ldOFY(param_2 << 0x10);
  return;
}


