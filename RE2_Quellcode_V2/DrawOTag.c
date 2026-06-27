/* DrawOTag @ 0x800909a0  (Ghidra headless, raw MIPS overlay) */

void DrawOTag(u_long *p)

{
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("DrawOTag(%08x)...\n",p);
  }
  (**(code **)(PTR_PTR_800b26f8 + 8))(*(undefined4 *)(PTR_PTR_800b26f8 + 0x18),p,0,0);
  return;
}


