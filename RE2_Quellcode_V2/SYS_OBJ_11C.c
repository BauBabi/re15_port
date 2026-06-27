/* SYS_OBJ_11C @ 0x8008fffc  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_11C(void)

{
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("ResetGraph(%d)...\n");
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x34))(1);
  return;
}


