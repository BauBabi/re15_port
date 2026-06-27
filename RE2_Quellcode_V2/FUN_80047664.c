/* FUN_80047664 @ 0x80047664  (Ghidra headless, raw MIPS overlay) */

uint * FUN_80047664(int *param_1,uint param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  undefined4 *puVar4;
  int iVar5;
  uint *puVar6;
  int iVar7;
  
  puVar6 = (uint *)0x0;
  iVar5 = 0;
  iVar7 = 1;
  if ((DAT_800cfbd8 & 0x10000000) != 0) {
    iVar7 = 2;
  }
  puVar4 = &DAT_800cfe14;
  do {
    puVar3 = (uint *)*puVar4;
    if (((((*puVar3 & 1) != 0) && (*(char *)((int)puVar3 + 0x1d3) == '\0')) &&
        (-1 < *(short *)((int)puVar3 + 0x156))) &&
       ((((*(ushort *)((int)puVar3 + 0x10e) & 0xc000) == 0 &&
         (*(ushort *)(puVar3 + 0x74) = (ushort)puVar3[0x74] & 0xff00,
         (puVar3[0xf] + (int)(short)puVar3[0x26] + (uint)*(ushort *)((int)puVar3 + 0x9e)) -
         param_1[1] < (uint)*(ushort *)((int)puVar3 + 0x9e) << 1)) &&
        (uVar1 = SquareRoot0((*param_1 - puVar3[0xe]) * (*param_1 - puVar3[0xe]) +
                             (param_1[2] - puVar3[0x10]) * (param_1[2] - puVar3[0x10])),
        uVar1 < param_2)))) {
      puVar3[1] = 2;
      puVar6 = puVar3;
      if (iVar5 == 0) {
        iVar2 = FUN_800401d4(param_3,0);
        if (iVar2 == 2) {
          puVar3[1] = 3;
        }
      }
      else {
        *(short *)((int)puVar3 + 0x156) = *(short *)((int)puVar3 + 0x156) - (short)param_3;
        DAT_800d4b84 = DAT_800d4b84 - (short)param_3;
        if (*(short *)((int)puVar3 + 0x156) < 0) {
          puVar3[1] = 3;
        }
      }
    }
    iVar5 = iVar5 + 1;
    puVar4 = puVar4 + 1;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  return puVar6;
}


