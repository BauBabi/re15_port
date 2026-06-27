/* FUN_8003513c @ 0x8003513c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8003513c(short *param_1,int param_2,int param_3)

{
  long lVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  undefined4 *puVar7;
  
  puVar7 = &DAT_800cfe14;
  if (DAT_800ce334 != &DAT_800cfe14) {
    do {
      puVar6 = (uint *)*puVar7;
      if (((*puVar6 & 1) != 0) && (puVar5 = puVar6 + 0x21, (*puVar6 & 2) == 0)) {
        DAT_800c3b08 = puVar6[0x7a];
        while (DAT_800c3b08 = DAT_800c3b08 - 1, -1 < (int)DAT_800c3b08) {
          iVar4 = param_2 + (uint)*(ushort *)((int)puVar5 + 0x16);
          iVar3 = *puVar5 - (int)*param_1;
          if ((uint)(iVar3 + iVar4) <= (uint)(iVar4 * 2)) {
            iVar2 = puVar5[2] - (int)param_1[2];
            if (((((uint)(iVar2 + iVar4) <= (uint)(iVar4 * 2)) &&
                 (lVar1 = SquareRoot0(iVar3 * iVar3 + iVar2 * iVar2), 0 < iVar4 - lVar1)) &&
                (iVar3 = (uint)*(ushort *)((int)puVar5 + 0x1a) + param_3,
                -iVar3 < (int)(puVar5[1] - (int)param_1[1]))) &&
               ((int)(puVar5[1] - (int)param_1[1]) < iVar3)) {
              if (puVar6 != &DAT_800cfbf8) {
                return 1;
              }
              return 2;
            }
          }
          puVar5 = puVar5 + 8;
        }
      }
      puVar7 = puVar7 + 1;
    } while (puVar7 != DAT_800ce334);
  }
  return 0;
}


