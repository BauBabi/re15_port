/* DSSYS_2_OBJ_9F0 @ 0x8008a4a4  (Ghidra headless, raw MIPS overlay) */

int DSSYS_2_OBJ_9F0(void)

{
  int in_v0;
  int iVar1;
  int *piVar2;
  int iVar3;
  int unaff_s2;
  undefined1 unaff_s4;
  int unaff_s5;
  int unaff_s6;
  
  iVar1 = 0;
  if ((in_v0 != 0) && (iVar1 = 0, DAT_800c3e80 < 8)) {
    iVar1 = DAT_800ad22c + 1;
    DAT_800ad22c = iVar1;
    piVar2 = (int *)CQ_last_queue();
    *piVar2 = iVar1;
    *(undefined1 *)(piVar2 + 1) = unaff_s4;
    if (unaff_s2 != 0) {
      parcpy((int)piVar2 + 5);
      piVar2[3] = unaff_s2;
      iVar1 = DSSYS_2_OBJ_A58();
      return iVar1;
    }
    piVar2[3] = 0;
    piVar2[4] = unaff_s5;
    piVar2[5] = unaff_s6;
    DAT_800c3e80 = DAT_800c3e80 + 1;
    iVar3 = DS_system_status(0);
    if ((iVar3 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == iVar1)) {
      CQ_execute();
    }
  }
  return iVar1;
}


