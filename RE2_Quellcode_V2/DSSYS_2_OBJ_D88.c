/* DSSYS_2_OBJ_D88 @ 0x8008a83c  (Ghidra headless, raw MIPS overlay) */

void DSSYS_2_OBJ_D88(void)

{
  int iVar1;
  int unaff_s6;
  
  iVar1 = DS_system_status(0);
  if ((iVar1 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == unaff_s6)) {
    CQ_execute();
    DSSYS_2_OBJ_DE0();
    return;
  }
  return;
}


