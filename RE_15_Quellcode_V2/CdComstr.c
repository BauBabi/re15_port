char * CdComstr(u_char com)

{
  char *pcVar1;
  
  if (com < 0x1c) {
    pcVar1 = (char *)SYS_OBJ_110();
    return pcVar1;
  }
  return "none";
}
