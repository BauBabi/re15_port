/* OpenTMD @ 0x80092fec  (Ghidra headless, raw MIPS overlay) */

int OpenTMD(u_long *tmd,int obj_no)

{
  DAT_800c4070 = get_tmd_addr(tmd,obj_no,&DAT_800c406c,&DAT_800c4064,&DAT_800c4068);
  return DAT_800c4070;
}


