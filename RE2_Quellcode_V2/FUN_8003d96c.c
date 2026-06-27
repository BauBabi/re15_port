/* FUN_8003d96c @ 0x8003d96c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8003d96c(int param_1,uint param_2,short param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined4 *puVar5;
  uint *puVar6;
  
  puVar6 = (uint *)0x0;
  if (DAT_800cfbf3 != '\0') {
    puVar5 = &DAT_800cfe1c;
    do {
      puVar4 = (uint *)*puVar5;
      if (((((*puVar4 & 1) != 0) && ((*(ushort *)((int)puVar4 + 0x10e) & 0xe000) == 0)) &&
          (iVar1 = *(int *)(param_1 + 0x38) - puVar4[0xe],
          iVar2 = *(int *)(param_1 + 0x40) - puVar4[0x10],
          uVar3 = SquareRoot0(iVar1 * iVar1 + iVar2 * iVar2), -1 < *(short *)((int)puVar4 + 0x156)))
         && ((iVar1 = FUN_80050858(*(int *)(param_1 + 0x198) +
                                   (uint)*(byte *)(param_1 + 0x1c1) * 0xac + 0x5c,
                                   puVar4[0x66] + (uint)*(byte *)((int)puVar4 + 0x1c1) * 0xac + 0x5c
                                   ,0x2080,1), iVar1 == 0 &&
             (iVar1 = FUN_80015614(param_1,puVar4[0xe],puVar4[0x10],(int)param_3), iVar1 == 0)))) {
        iVar1 = FUN_80015910(param_1,puVar4);
        if ((iVar1 == 0) && (uVar3 < param_2)) {
          param_2 = uVar3;
          puVar6 = puVar4;
        }
      }
      puVar5 = puVar5 + 1;
    } while (puVar5 != DAT_800ce334);
    if (puVar6 != (uint *)0x0) {
      *(uint **)(param_1 + 0x1b4) = puVar6;
      return 1;
    }
  }
  *(int *)(param_1 + 0x1b4) = param_1;
  return 0;
}


