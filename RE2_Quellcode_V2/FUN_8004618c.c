/* FUN_8004618c @ 0x8004618c  (Ghidra headless, raw MIPS overlay) */

uint * FUN_8004618c(int *param_1,short param_2,uint param_3,short param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  undefined4 *puVar5;
  uint *puVar6;
  int local_30 [2];
  
  puVar6 = (uint *)0x0;
  if (DAT_800cfbf3 != '\0') {
    puVar5 = &DAT_800cfe1c;
    do {
      puVar4 = (uint *)*puVar5;
      if (((*puVar4 & 1) != 0) && ((*(ushort *)((int)puVar4 + 0x10e) & 0xe000) == 0)) {
        local_30[0] = *param_1 - puVar4[0xe];
        uVar2 = SquareRoot0(local_30[0] * local_30[0] +
                            (param_1[2] - puVar4[0x10]) * (param_1[2] - puVar4[0x10]));
        sVar1 = FUN_800154ac(*param_1,param_1[2],puVar4[0xe],puVar4[0x10]);
        if (-1 < *(short *)((int)puVar4 + 0x156)) {
          iVar3 = FUN_80046f44((int)sVar1,(int)param_2,(int)param_4,local_30);
          if ((iVar3 == 0) && (uVar2 < param_3)) {
            param_3 = uVar2;
            puVar6 = puVar4;
          }
        }
      }
      puVar5 = puVar5 + 1;
    } while (puVar5 != DAT_800ce334);
    if (puVar6 != (uint *)0x0) {
      return puVar6;
    }
  }
  return DAT_800ce330;
}


