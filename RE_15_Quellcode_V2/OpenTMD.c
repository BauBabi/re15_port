int OpenTMD(u_long *tmd,int obj_no)

{
  DAT_8008ff78 = get_tmd_addr(tmd,obj_no,&DAT_8008ff74,&DAT_8008ff6c,&DAT_8008ff70);
  return DAT_8008ff78;
}
