/* FUN_8002eaf8 @ 0x8002eaf8  (Ghidra headless, raw MIPS overlay) */

void FUN_8002eaf8(undefined4 *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  char cVar9;
  undefined4 uVar10;
  
  if ((DAT_800cfb7d == '\x02') ||
     (cVar9 = FUN_8002ea40(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,DAT_800cfb7d,
                           param_1 + 0xe), cVar9 != '\0')) {
    FUN_8002e788(param_1);
    uVar10 = param_1[0x66];
    param_1[0x66] = param_1[0x69];
    uVar8 = DAT_800dcbc4;
    uVar7 = DAT_800dcbc0;
    uVar6 = DAT_800dcbbc;
    uVar5 = DAT_800dcbb8;
    uVar4 = DAT_800dcbb4;
    uVar3 = DAT_800dcbb0;
    uVar2 = DAT_800dcbac;
    uVar1 = DAT_800dcba8;
    DAT_800dcba8 = DAT_800e2a90;
    DAT_800dcbac = DAT_800e2a94;
    DAT_800dcbb0 = DAT_800e2a98;
    DAT_800dcbb4 = DAT_800e2a9c;
    DAT_800dcbb8 = DAT_800e2aa0;
    DAT_800dcbbc = DAT_800e2aa4;
    DAT_800dcbc0 = DAT_800e2aa8;
    DAT_800dcbc4 = DAT_800e2aac;
    FUN_8002ec70(param_1);
    FUN_80027160(param_1,param_1[0x66],*param_1,param_1 + 0xe);
    DAT_800dcba8 = uVar1;
    DAT_800dcbac = uVar2;
    DAT_800dcbb0 = uVar3;
    DAT_800dcbb4 = uVar4;
    DAT_800dcbb8 = uVar5;
    DAT_800dcbbc = uVar6;
    DAT_800dcbc0 = uVar7;
    DAT_800dcbc4 = uVar8;
    param_1[0x66] = uVar10;
  }
  return;
}


