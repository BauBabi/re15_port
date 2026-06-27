/* CQ_clear_queue @ 0x80089ab4  (Ghidra headless, raw MIPS overlay) */

void CQ_clear_queue(undefined4 *param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 3;
  iVar1 = (int)param_1 + 3;
  *param_1 = 0;
  *(undefined1 *)(param_1 + 1) = 0;
  do {
    *(undefined1 *)(iVar1 + 5) = 0;
    iVar2 = iVar2 + -1;
    iVar1 = iVar1 + -1;
  } while (-1 < iVar2);
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  return;
}


