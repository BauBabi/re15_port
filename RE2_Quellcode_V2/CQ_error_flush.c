/* CQ_error_flush @ 0x80089c2c  (Ghidra headless, raw MIPS overlay) */

void CQ_error_flush(undefined1 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int *piVar7;
  int local_48 [7];
  undefined4 local_2c [3];
  
  iVar3 = 7;
  puVar2 = local_2c;
  do {
    *puVar2 = 0;
    iVar3 = iVar3 + -1;
    puVar2 = puVar2 + -1;
  } while (-1 < iVar3);
  iVar3 = 0;
  iVar4 = 0;
  if (0 < DAT_800c3e80) {
    piVar7 = local_48;
    iVar1 = DAT_800c3e78 * 0x18;
    piVar5 = &DAT_800c3db8 + DAT_800c3e78 * 6;
    iVar6 = DAT_800c3e78;
    do {
      if (*piVar5 != iVar3) {
        CQ_add_result(*piVar5,param_1,param_2);
        if (*(int *)((int)&DAT_800c3dc8 + iVar1) != 0) {
          *piVar7 = *(int *)((int)&DAT_800c3dc8 + iVar1);
        }
        iVar3 = *piVar5;
      }
      piVar5 = piVar5 + 6;
      iVar6 = iVar6 + 1;
      iVar1 = iVar1 + 0x18;
      if (7 < iVar6) {
        piVar5 = &DAT_800c3db8;
        iVar1 = 0;
        iVar6 = 0;
      }
      iVar4 = iVar4 + 1;
      piVar7 = piVar7 + 1;
    } while (iVar4 < DAT_800c3e80);
  }
  iVar3 = 0;
  puVar2 = &DAT_800c3db8;
  DAT_800c3e80 = 0;
  DAT_800c3e7c = 0;
  DAT_800c3e78 = 0;
  do {
    iVar6 = 3;
    iVar4 = (int)puVar2 + 3;
    *puVar2 = 0;
    *(undefined1 *)(puVar2 + 1) = 0;
    do {
      *(undefined1 *)(iVar4 + 5) = 0;
      iVar6 = iVar6 + -1;
      iVar4 = iVar4 + -1;
    } while (-1 < iVar6);
    puVar2[3] = 0;
    puVar2[4] = 0;
    puVar2[5] = 0;
    iVar3 = iVar3 + 1;
    puVar2 = puVar2 + 6;
  } while (iVar3 < 8);
  iVar3 = 0;
  piVar5 = local_48;
  do {
    if ((code *)*piVar5 != (code *)0x0) {
      (*(code *)*piVar5)(param_1,param_2);
    }
    iVar3 = iVar3 + 1;
    piVar5 = piVar5 + 1;
  } while (iVar3 < 8);
  return;
}


