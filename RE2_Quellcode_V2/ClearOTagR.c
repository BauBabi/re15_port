/* ClearOTagR @ 0x800908a8  (Ghidra headless, raw MIPS overlay) */

u_long * ClearOTagR(u_long *ot,int n)

{
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("ClearOTagR(%08x,%d)...\n",ot,n);
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x2c))(ot,n);
  *ot = 0xb27bc;
  return ot;
}


