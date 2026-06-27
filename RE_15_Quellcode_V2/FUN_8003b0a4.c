undefined4 FUN_8003b0a4(int *param_1,uint param_2,byte param_3)

{
  undefined1 *puVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  undefined4 *puVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  ushort uVar9;
  uint uVar10;
  ushort *puVar11;
  ushort *puVar12;
  short *psVar13;
  undefined4 uVar14;
  undefined1 local_38 [4];
  undefined1 local_34 [4];
  byte local_30;
  
  puVar1 = local_38 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  puVar1 = local_34 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  *DAT_800ac784 = *DAT_800ac784 & 0xffffffcf;
  uVar14 = 0;
  if ((*DAT_800ac784 & 8) == 0) {
    psVar13 = (short *)DAT_800ac784[0x1f];
    local_34._2_2_ = 0;
    local_34._0_2_ = psVar13[2];
    local_38 = *(undefined1 (*) [4])psVar13;
    local_30 = param_3;
    iVar4 = FUN_8003b068(param_1,local_38,(int)**(short **)(DAT_800ac778 + 0x20),
                         (int)(*(short **)(DAT_800ac778 + 0x20))[1]);
    iVar7 = *(int *)(DAT_800ac778 + 0x20);
    DAT_800bbe48 = *param_1;
    DAT_800bbe4c = param_1[1];
    DAT_800bbe50 = param_1[2];
    DAT_800bbe54 = param_1[3];
    *(byte *)(DAT_800ac784 + 0x24) = (byte)DAT_800ac784[0x24] & 0xf0;
    DAT_800ac784[0x6d] = 0;
    puVar5 = (undefined4 *)(((iVar4 << 0x10) >> 0xe) + 4 + iVar7);
    *(undefined2 *)(DAT_800ac784[0x6d] + 10) = 0;
    puVar12 = (ushort *)*puVar5;
    if (puVar12 < (ushort *)puVar5[1]) {
      puVar11 = puVar12 + 5;
      do {
        if ((uint)*(byte *)((int)DAT_800ac784 + 0x82) == ((uint)*puVar11 << 0x10) >> 0x1c) {
          if (((uint)local_30 & (int)((uint)puVar11[-1] << 0x10) >> 0x18) == 0) {
            uVar8 = param_2 & 0xffff;
            if ((((*puVar11 & 0x80) != 0) &&
                (((int)*psVar13 + *param_1) - ((int)(short)puVar11[-3] - uVar8) <
                 (uint)*puVar12 + uVar8 * 2)) &&
               (((int)psVar13[2] + param_1[2]) - ((int)(short)puVar11[-2] - uVar8) <
                (uint)puVar11[-4] + uVar8 * 2)) {
              *DAT_800ac784 = *DAT_800ac784 | 0x20;
            }
          }
          else {
            if (*(short *)(DAT_800ac784[0x1e] + 6) != *(short *)(DAT_800ac784[0x1e] + 10)) {
              lVar6 = ratan2(((int)(short)puVar11[-2] + (uint)(puVar11[-4] >> 1)) -
                             ((int)psVar13[2] + param_1[2]),
                             ((int)(short)puVar11[-3] + (uint)(*puVar12 >> 1)) -
                             ((int)*psVar13 + *param_1));
              uVar8 = lVar6 - (uint)*(ushort *)((int)DAT_800ac784 + 0x6a);
              uVar9 = (ushort)uVar8 & 0xfff;
              if ((uVar8 & 0xc00) == 0xc00) {
                sVar3 = 0x1000;
LAB_8003b390:
                uVar9 = sVar3 - uVar9;
              }
              else if ((uVar8 & 0x800) == 0) {
                sVar3 = 0x800;
                if ((uVar8 & 0x400) != 0) goto LAB_8003b390;
              }
              else {
                uVar9 = uVar9 - 0x800;
              }
              uVar8 = (uint)*(ushort *)(DAT_800ac784[0x1e] + 10);
              uVar10 = (uint)*(ushort *)(DAT_800ac784[0x1e] + 6);
              iVar4 = rsin((int)(short)uVar9);
              param_2 = uVar10 + ((int)((uVar8 - uVar10) * iVar4) >> 0xc);
              if (uVar8 < uVar10) {
                uVar2 = *(ushort *)(DAT_800ac784[0x1e] + 6);
                param_2 = (uint)*(ushort *)(DAT_800ac784[0x1e] + 10);
                iVar4 = rcos((int)(short)uVar9);
                param_2 = param_2 + ((int)((uVar2 - param_2) * iVar4) >> 0xc);
              }
            }
            uVar8 = param_2 & 0xffff;
            if ((((int)*psVar13 + *param_1) - ((int)(short)puVar11[-3] - uVar8) <
                 (uint)*puVar12 + uVar8 * 2) &&
               (((int)psVar13[2] + param_1[2]) - ((int)(short)puVar11[-2] - uVar8) <
                (uint)puVar11[-4] + uVar8 * 2)) {
              (**(code **)(&DAT_800b2858 + (uint)(byte)puVar11[-1] * 4))
                        (puVar12,DAT_800ac784 + 0xd,DAT_800ac784 + 0x10);
              iVar4 = FUN_8001bf04(param_1,local_38,puVar12);
              *(byte *)(DAT_800ac784 + 0x24) = (char)(iVar4 >> 4) + '\b' + ((byte)*puVar11 & 3);
              DAT_800ac784[0x6d] = (uint)puVar12;
              uVar14 = 1;
              *DAT_800ac784 = *DAT_800ac784 | 0x10;
            }
          }
        }
        puVar12 = puVar12 + 6;
        puVar11 = puVar11 + 6;
      } while (puVar12 < (ushort *)puVar5[1]);
    }
  }
  else {
    uVar14 = 0;
  }
  return uVar14;
}
