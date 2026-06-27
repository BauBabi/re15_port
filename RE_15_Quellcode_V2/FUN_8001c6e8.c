int FUN_8001c6e8(int *param_1,int param_2,int param_3,uint param_4)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  ushort *puVar4;
  ushort *puVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  undefined1 auStack_40 [4];
  undefined1 auStack_3c [4];
  undefined4 *local_38;
  int local_30;
  
  puVar1 = auStack_40 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  auStack_40 = (undefined1  [4])0x0;
  puVar1 = auStack_3c + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  auStack_3c = (undefined1  [4])0x0;
  uVar8 = param_3 << 0xc;
  local_30 = param_2 << 0x10;
  iVar9 = local_30 >> 0x10;
  iVar2 = FUN_8003b068(param_1,auStack_40,(int)**(short **)(DAT_800ac778 + 0x20),
                       (int)(*(short **)(DAT_800ac778 + 0x20))[1]);
  local_38 = (undefined4 *)(iVar2 * 4 + 4 + *(int *)(DAT_800ac778 + 0x20));
  puVar10 = local_38 + 1;
  do {
    uVar8 = uVar8 - 0x1000;
    if (((param_4 & 0x10000) == 0) && (uVar6 = (uint)*(byte *)(DAT_800ac778 + 2), uVar6 != 0)) {
      iVar2 = local_30 >> 0x1f;
      iVar7 = uVar6 * 0x94;
      do {
        if ((param_1 != (int *)(&DAT_800b3f38 + iVar7)) &&
           (iVar3 = FUN_8002da4c(param_1,&DAT_800b3f04 + iVar7,(int)(short)(iVar9 - iVar2 >> 1),
                                 uVar8 >> 0xc), iVar3 != 0)) {
          return (int)(((uint)*(ushort *)(&DAT_800b3f3c + iVar7) +
                       (uint)*(ushort *)(*(int *)(&DAT_800b3f7c + iVar7) + 8) * -2) * 0x10000) >>
                 0x10;
        }
        uVar6 = uVar6 - 1;
        iVar7 = iVar7 + -0x94;
      } while (uVar6 != 0);
    }
    puVar5 = (ushort *)*local_38;
    if (puVar5 < (ushort *)*puVar10) {
      puVar4 = puVar5 + 1;
      do {
        if ((((uVar8 == ((int)(short)puVar4[4] & 0xf002U)) &&
             (((int)(short)puVar4[3] & param_4) != 0)) &&
            ((uint)(*param_1 - ((short)puVar4[1] + iVar9)) < (uint)*puVar5 + iVar9 * -2)) &&
           ((uint)(param_1[2] - ((short)puVar4[2] + iVar9)) < (uint)*puVar4 + iVar9 * -2)) {
          return (int)(((uVar8 >> 0xc) + 1) * -0x7080000) >> 0x10;
        }
        puVar5 = puVar5 + 6;
        puVar4 = puVar4 + 6;
      } while (puVar5 < (ushort *)*puVar10);
    }
    if (uVar8 == 0) {
      return 0;
    }
  } while( true );
}
