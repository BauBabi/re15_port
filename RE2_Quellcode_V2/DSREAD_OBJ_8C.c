/* DSREAD_OBJ_8C @ 0x8008c2bc  (Ghidra headless, raw MIPS overlay) */

int DSREAD_OBJ_8C(void)

{
  int iVar1;
  u_char unaff_s0;
  
  iVar1 = DsPacket(unaff_s0,(DslLOC *)&stack0x00000018,'\x06',DS_read_cbsync,-1);
  DAT_800ad31c = VSync(-1);
  if ((DAT_800ad310 & 1) != 0) {
    DAT_800ad318 = DsDataCallback(DS_read_cbdata);
  }
  DAT_800ad324 = 1;
  return iVar1;
}


