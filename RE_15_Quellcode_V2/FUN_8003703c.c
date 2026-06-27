undefined4 FUN_8003703c(uint param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  short *psVar4;
  char cVar5;
  uint *puVar6;
  char cVar7;
  uint uVar8;
  byte bVar9;
  byte bVar10;
  byte local_38;
  
  cVar5 = '\0';
  bVar9 = 0;
  bVar10 = 0;
  local_38 = 0;
  puVar6 = &DAT_800acc2c;
  if (DAT_800aca4e != '\0') {
    psVar4 = &DAT_800accc6;
    uVar3 = param_1;
    uVar8 = param_1;
    cVar7 = DAT_800aca4e;
    do {
      if ((*puVar6 & 1) != 0) {
        uVar1 = SquareRoot0((*(int *)(psVar4 + -0x33) - DAT_800aca88) *
                            (*(int *)(psVar4 + -0x33) - DAT_800aca88) +
                            (*(int *)(psVar4 + -0x2f) - DAT_800aca90) *
                            (*(int *)(psVar4 + -0x2f) - DAT_800aca90));
        if ((*psVar4 & 0x6000000000000000) == 0) {
          if (*psVar4 < 0) {
            if (uVar1 < param_1) {
              local_38 = cVar5 + 1;
              param_1 = uVar1;
            }
          }
          else if ((*psVar4 & 0x8000000000000000) == 0) {
            if (uVar1 < uVar3) {
              bVar9 = cVar5 + 1;
              uVar3 = uVar1;
            }
          }
          else if (uVar1 < uVar8) {
            bVar10 = cVar5 + 1;
            uVar8 = uVar1;
          }
        }
        cVar7 = cVar7 + -1;
      }
      cVar5 = cVar5 + '\x01';
      puVar6 = puVar6 + 0x7d;
      psVar4 = psVar4 + 0xfa;
    } while (cVar7 != '\0');
  }
  uVar3 = (uint)bVar9;
  if ((uVar3 == 0) && (uVar3 = (uint)bVar10, uVar3 == 0)) {
    DAT_800acbfc = &DAT_800aca38 + (uint)local_38 * 0x7d;
    if (local_38 == 0) {
      DAT_800acbfc = &DAT_800acc2c;
      uVar2 = 0;
    }
    else {
      uVar2 = 2;
    }
  }
  else {
    DAT_800acbfc = &DAT_800aca38 + uVar3 * 0x7d;
    uVar2 = 1;
  }
  return uVar2;
}
