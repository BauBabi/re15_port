/* DSSYS_2_OBJ_A58 @ 0x8008a50c  (Ghidra headless, raw MIPS overlay) */

void DSSYS_2_OBJ_A58(void)

{
  int iVar1;
  int unaff_s0;
  int unaff_s1;
  undefined4 unaff_s5;
  undefined4 unaff_s6;
  
  *(undefined4 *)(unaff_s0 + 0x10) = unaff_s5;
  *(undefined4 *)(unaff_s0 + 0x14) = unaff_s6;
  DAT_800c3e80 = DAT_800c3e80 + 1;
  iVar1 = DS_system_status(0);
  if ((iVar1 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == unaff_s1)) {
    CQ_execute();
  }
  return;
}


