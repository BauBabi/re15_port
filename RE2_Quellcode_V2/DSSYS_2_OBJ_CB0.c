/* DSSYS_2_OBJ_CB0 @ 0x8008a764  (Ghidra headless, raw MIPS overlay) */

int DSSYS_2_OBJ_CB0(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined1 unaff_s0;
  undefined4 unaff_s2;
  int *piVar4;
  int iVar5;
  undefined1 *puVar6;
  int unaff_s7;
  
  puVar6 = &stack0x00000010;
  puVar6[param_1 * 0x10] = unaff_s0;
  *(undefined4 *)(&stack0x0000001c + param_1 * 0x10) = unaff_s2;
  param_1 = param_1 + 1;
  iVar5 = 0;
  if (DAT_800c3e80 + param_1 < 9) {
    iVar1 = DAT_800ad22c + 1;
    DAT_800ad22c = iVar1;
    if (0 < param_1) {
      piVar4 = (int *)&stack0x0000001c;
      do {
        piVar2 = (int *)CQ_last_queue();
        if (piVar2 == (int *)0x0) {
          return 0;
        }
        *piVar2 = iVar1;
        *(undefined1 *)(piVar2 + 1) = *puVar6;
        if (piVar4[-1] != 0) {
          parcpy((int)piVar2 + 5,puVar6 + 1);
          piVar2[3] = (int)piVar2 + 5;
          iVar5 = DSSYS_2_OBJ_D54();
          return iVar5;
        }
        piVar2[3] = 0;
        iVar3 = *piVar4;
        piVar4 = piVar4 + 4;
        piVar2[5] = unaff_s7;
        piVar2[4] = iVar3;
        iVar5 = iVar5 + 1;
        DAT_800c3e80 = DAT_800c3e80 + 1;
        puVar6 = puVar6 + 0x10;
      } while (iVar5 < param_1);
    }
    iVar5 = DS_system_status(0);
    if ((iVar5 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == iVar1)) {
      CQ_execute();
      iVar5 = DSSYS_2_OBJ_DE0();
      return iVar5;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}


