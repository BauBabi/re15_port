/* DSSYS_2_OBJ_D54 @ 0x8008a808  (Ghidra headless, raw MIPS overlay) */

void DSSYS_2_OBJ_D54(void)

{
  int iVar1;
  int *unaff_s1;
  int *unaff_s2;
  int unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  int unaff_s6;
  int unaff_s7;
  
  while( true ) {
    iVar1 = *unaff_s2;
    unaff_s1[5] = unaff_s7;
    unaff_s1[4] = iVar1;
    unaff_s3 = unaff_s3 + 1;
    DAT_800c3e80 = DAT_800c3e80 + 1;
    if (unaff_s5 <= unaff_s3) {
      iVar1 = DS_system_status(0);
      if ((iVar1 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == unaff_s6)) {
        CQ_execute();
        DSSYS_2_OBJ_DE0();
        return;
      }
      return;
    }
    unaff_s1 = (int *)CQ_last_queue();
    if (unaff_s1 == (int *)0x0) {
      return;
    }
    *unaff_s1 = unaff_s6;
    *(undefined1 *)(unaff_s1 + 1) = unaff_s4[0x10];
    if (unaff_s2[3] != 0) break;
    unaff_s1[3] = 0;
    unaff_s2 = unaff_s2 + 4;
    unaff_s4 = unaff_s4 + 0x10;
  }
  parcpy((int)unaff_s1 + 5,unaff_s4 + 0x11);
  unaff_s1[3] = (int)unaff_s1 + 5;
  DSSYS_2_OBJ_D54();
  return;
}


