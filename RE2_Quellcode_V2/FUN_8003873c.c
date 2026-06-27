/* FUN_8003873c @ 0x8003873c  (Ghidra headless, raw MIPS overlay) */

uint * FUN_8003873c(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  byte bVar3;
  int *piVar4;
  uint *puVar5;
  
  puVar5 = &DAT_800d0324;
  if (&DAT_800d0324 < DAT_800d4224) {
    piVar4 = &DAT_800d03b0;
    do {
      if ((((*puVar5 & 0x101) == 0x101) && ((*puVar5 & 2) == 0)) &&
         (*(int *)(param_1 + 0x3c) == piVar4[-1] - (uint)*(ushort *)((int)piVar4 + 0x12))) {
        iVar2 = -(uint)(*(ushort *)(param_2 + 3) >> 1) + (uint)*(ushort *)(piVar4 + 1);
        iVar1 = -(uint)(*(ushort *)(param_2 + 3) >> 1) + (uint)*(ushort *)((int)piVar4 + 6);
        bVar3 = (uint)(iVar2 * 2) < (uint)((piVar4[-2] - *param_2) + iVar2);
        if ((uint)(iVar1 * 2) < (uint)((*piVar4 - param_2[2]) + iVar1)) {
          bVar3 = bVar3 | 2;
        }
        if (bVar3 == 0) {
          return puVar5;
        }
      }
      puVar5 = puVar5 + 0x7e;
      piVar4 = piVar4 + 0x7e;
    } while (puVar5 < DAT_800d4224);
  }
  return *(uint **)(param_1 + 0x1e4);
}


