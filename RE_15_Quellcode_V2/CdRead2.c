int CdRead2(long mode)

{
  int iVar1;
  u_char local_10 [8];
  
  local_10[0] = (u_char)mode;
  CdControl('\x0e',local_10,(u_char *)0x0);
  if ((mode & 0x100U) != 0) {
    if ((mode & 0x20U) != 0) {
      DAT_800b2240 = 0;
      iVar1 = CDREAD2_OBJ_50();
      return iVar1;
    }
    DAT_800b2240 = 1;
    CdDataCallback(data_ready_callback);
    CdReadyCallback(StCdInterrupt2);
  }
  iVar1 = CdControl('\x1b',(u_char *)0x0,(u_char *)0x0);
  return iVar1;
}
