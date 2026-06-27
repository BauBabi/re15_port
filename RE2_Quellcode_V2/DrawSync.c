/* DrawSync @ 0x800903a0  (Ghidra headless, raw MIPS overlay) */

int DrawSync(int mode)

{
  int iVar1;
  
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("DrawSync(%d)...\n",mode);
  }
  iVar1 = (**(code **)(PTR_PTR_800b26f8 + 0x3c))(mode);
  return iVar1;
}


