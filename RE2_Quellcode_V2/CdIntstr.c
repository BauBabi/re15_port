/* CdIntstr @ 0x80086db8  (Ghidra headless, raw MIPS overlay) */

char * CdIntstr(u_char intr)

{
  char *pcVar1;
  
  if (intr < 7) {
    pcVar1 = (char *)SYS_OBJ_144();
    return pcVar1;
  }
  return "none";
}


