/* ClearOTag @ 0x800907f0  (Ghidra headless, raw MIPS overlay) */

u_long * ClearOTag(u_long *ot,int n)

{
  int iVar1;
  
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("ClearOTag(%08x,%d)...\n",ot,n);
  }
  for (iVar1 = n + -1; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)((int)ot + 3) = 0;
    *ot = *ot & 0xff000000 | (uint)(ot + 1) & 0xffffff;
    ot = ot + 1;
  }
  *ot = 0xb27bc;
  return ot;
}


