void SEQREAD_OBJ_6F8(void)

{
  int unaff_s4;
  uint unaff_s6;
  
  if (unaff_s6 < 0x40) {
    FUN_8005fb00();
    SEQREAD_OBJ_7F8(unaff_s4 << 0x10);
    return;
  }
  FUN_8005f89c();
  SEQREAD_OBJ_7F8(unaff_s4 << 0x10);
  return;
}
