void CDREAD2_OBJ_50(void)

{
  CdDataCallback(data_ready_callback);
  CdReadyCallback(StCdInterrupt2);
  CdControl('\x1b',(u_char *)0x0,(u_char *)0x0);
  return;
}
