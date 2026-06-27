undefined4 FUN_8003b558(uint *param_1,uint param_2)

{
  undefined1 *puVar1;
  uint *puVar2;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  ushort *puVar8;
  ushort *puVar9;
  short *psVar10;
  undefined4 uVar11;
  undefined1 local_30 [4];
  undefined4 local_2c;
  
  puVar1 = local_30 + 3;
  uVar6 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar6) =
       *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | 0U >> (3 - uVar6) * 8;
  uVar6 = (int)&local_2c + 3U & 3;
  puVar2 = (uint *)(((int)&local_2c + 3U) - uVar6);
  *puVar2 = *puVar2 & -1 << (uVar6 + 1) * 8 | 0U >> (3 - uVar6) * 8;
  psVar10 = (short *)param_1[0x1e];
  uVar11 = 0;
  local_2c = (uint)(ushort)psVar10[2];
  local_30 = *(undefined1 (*) [4])psVar10;
  iVar3 = FUN_8003b068(param_1 + 0xd,local_30,(int)**(short **)(DAT_800ac778 + 0x20),
                       (int)(*(short **)(DAT_800ac778 + 0x20))[1]);
  puVar4 = (undefined4 *)(((iVar3 << 0x10) >> 0xe) + 4 + *(int *)(DAT_800ac778 + 0x20));
  puVar9 = (ushort *)*puVar4;
  if (puVar9 < (ushort *)puVar4[1]) {
    puVar8 = puVar9 + 1;
    do {
      if ((((uint)*(byte *)((int)param_1 + 0x82) == (int)((uint)puVar8[4] << 0x10) >> 0x1c) &&
          ((param_2 & 0xff & (int)((uint)puVar8[3] << 0x10) >> 0x18) != 0)) && ((*param_1 & 8) == 0)
         ) {
        uVar6 = param_1[0x1e];
        uVar7 = (uint)*(ushort *)(uVar6 + 6);
        if ((*(short *)(uVar6 + 6) != *(short *)(uVar6 + 10)) &&
           ((((int)*psVar10 + param_1[0xd]) - ((int)(short)puVar8[1] + (uint)*puVar9) & 0x80000000 |
            (((int)*psVar10 + param_1[0xd]) - (int)(short)puVar8[1] & 0x80000000) >> 1) ==
            0x80000000)) {
          uVar5 = (((int)psVar10[2] + param_1[0xf]) - ((int)(short)puVar8[2] + (uint)*puVar8) &
                   0x80000000 |
                  (((int)psVar10[2] + param_1[0xf]) - (int)(short)puVar8[2] & 0x80000000) >> 1) >>
                  0x1e;
          if (uVar5 != 2) {
            if (uVar5 < 3) {
              if (uVar5 == 0) {
LAB_8003b734:
                uVar7 = (uint)*(ushort *)(uVar6 + 10);
              }
            }
            else if (uVar5 == 3) goto LAB_8003b734;
          }
        }
        if ((((int)*psVar10 + param_1[0xd]) - ((int)(short)puVar8[1] - uVar7) <
             (uint)*puVar9 + uVar7 * 2) &&
           (((int)psVar10[2] + param_1[0xf]) - ((int)(short)puVar8[2] - uVar7) <
            (uint)*puVar8 + uVar7 * 2)) {
          FUN_8003da78(puVar9,param_1);
          uVar11 = 1;
        }
      }
      puVar9 = puVar9 + 6;
      puVar8 = puVar8 + 6;
    } while (puVar9 < (ushort *)puVar4[1]);
  }
  return uVar11;
}
