/* SYS_OBJ_604 @ 0x800904e4  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_604(void)

{
  short *unaff_s0;
  
  (*(code *)PTR_printf_800b26fc)();
  (*(code *)PTR_printf_800b26fc)
            ("(%d,%d)-(%d,%d)\n",(int)*unaff_s0,(int)unaff_s0[1],(int)unaff_s0[2]);
  return;
}


