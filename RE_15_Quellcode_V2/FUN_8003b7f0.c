ushort FUN_8003b7f0(int *param_1,short param_2,uint param_3)

{
  undefined1 *puVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  undefined4 *puVar5;
  ushort *puVar6;
  ushort *puVar7;
  ushort *puVar8;
  undefined1 auStack_18 [4];
  undefined4 uStack_14;
  
  puVar1 = auStack_18 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | 0U >> (3 - uVar2) * 8;
  auStack_18 = (undefined1  [4])0x0;
  uVar2 = (int)&uStack_14 + 3U & 3;
  puVar3 = (uint *)(((int)&uStack_14 + 3U) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | 0U >> (3 - uVar2) * 8;
  uStack_14 = 0;
  iVar4 = FUN_8003b068(param_1,auStack_18,(int)**(short **)(DAT_800ac778 + 0x20),
                       (int)(*(short **)(DAT_800ac778 + 0x20))[1]);
  puVar5 = (undefined4 *)(((iVar4 << 0x10) >> 0xe) + 4 + *(int *)(DAT_800ac778 + 0x20));
  puVar8 = (ushort *)*puVar5;
  puVar6 = (ushort *)puVar5[1];
  if (puVar8 < puVar6) {
    iVar4 = (int)param_2;
    puVar7 = puVar8 + 5;
    do {
      if ((((param_3 & 0xff) == (int)((uint)*puVar7 << 0x10) >> 0x1c) &&
          ((uint)(*param_1 - ((short)puVar7[-3] - iVar4)) < (uint)*puVar8 + iVar4 * 2)) &&
         ((uint)(param_1[2] - ((short)puVar7[-2] - iVar4)) < (uint)puVar7[-4] + iVar4 * 2)) {
        return *puVar7;
      }
      puVar8 = puVar8 + 6;
      puVar7 = puVar7 + 6;
    } while (puVar8 < puVar6);
  }
  return 0;
}
