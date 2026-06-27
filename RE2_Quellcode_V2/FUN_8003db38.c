/* FUN_8003db38 @ 0x8003db38  (Ghidra headless, raw MIPS overlay) */

void FUN_8003db38(int param_1,uint param_2,short param_3)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint *puVar6;
  uint *puVar7;
  
  puVar7 = (uint *)0x0;
  uVar5 = 0x7fffffff;
  puVar6 = (uint *)0x0;
  if ((*(byte *)(param_1 + 0x1c0) & 1) != 0) {
    return;
  }
  if (DAT_800a4004 == '\0') {
    DAT_800a4004 = 0x2d;
    if ((DAT_800cfbf3 != '\0') || ((DAT_800cfbd8 & 0x10000000) != 0)) {
      puVar4 = &DAT_800cfe18;
      do {
        puVar2 = (uint *)*puVar4;
        if (((*puVar2 & 1) != 0) && ((*(ushort *)((int)puVar2 + 0x10e) & 0xc000) == 0)) {
          uVar3 = puVar2[0x7c];
          iVar1 = FUN_80050858(*(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac +
                               0x5c,puVar2[0x66] + (uint)*(byte *)((int)puVar2 + 0x1c1) * 0xac +
                                    0x5c,0x2080,1);
          if ((iVar1 == 0) &&
             (iVar1 = FUN_80015614(param_1,puVar2[0xe],puVar2[0x10],(int)param_3), iVar1 == 0)) {
            if ((*(ushort *)((int)puVar2 + 0x10e) & 0x2000) == 0) {
              if (uVar3 < param_2) {
                param_2 = uVar3;
                puVar7 = puVar2;
              }
            }
            else if (uVar3 < uVar5) {
              uVar5 = uVar3;
              puVar6 = puVar2;
            }
          }
        }
        puVar4 = puVar4 + 1;
      } while (puVar4 != DAT_800ce334);
      if (puVar7 != (uint *)0x0) {
        *(uint **)(param_1 + 0x1b8) = puVar7;
        return;
      }
      if (puVar6 != (uint *)0x0) {
        *(uint **)(param_1 + 0x1b8) = puVar6;
        return;
      }
    }
    *(int *)(param_1 + 0x1b8) = param_1;
    return;
  }
  DAT_800a4004 = DAT_800a4004 + -1;
  return;
}


