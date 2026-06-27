/* SsVabOpenHeadWithMode @ 0x800853c8  (Ghidra headless, raw MIPS overlay) */

void SsVabOpenHeadWithMode(uint *param_1,ushort param_2,short param_3,long param_4)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  uint *puVar6;
  uint *puVar7;
  int *piVar8;
  uint uVar9;
  ushort uVar10;
  int local_428 [256];
  undefined1 auStack_28 [8];
  
  iVar2 = FUN_8008273c();
  if (iVar2 == 1) {
    VS_VH_OBJ_444();
    return;
  }
  FUN_80082710(1);
  iVar2 = (int)(short)param_2;
  if (iVar2 < 0x10) {
    iVar5 = 0;
    if (iVar2 == -1) {
      do {
        if ((&DAT_800dcc68)[iVar5] == '\0') {
          (&DAT_800dcc68)[iVar5] = 1;
          VS_VH_OBJ_180();
          return;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < 0x10);
      VS_VH_OBJ_190();
      return;
    }
    iVar5 = 0x100000;
    uVar10 = 0x10;
    if ((&DAT_800dcc68)[iVar2] == '\0') {
      (&DAT_800dcc68)[iVar2] = 1;
      DAT_800eade8 = DAT_800eade8 + 1;
      iVar5 = (uint)param_2 << 0x10;
      uVar10 = param_2;
    }
    iVar5 = iVar5 >> 0x10;
    if (iVar5 < 0x10) {
      *(uint **)(&DAT_800d5270 + iVar5 * 4) = param_1;
      puVar6 = param_1 + 8;
      if (*param_1 >> 8 != 0x564142) {
        (&DAT_800dcc68)[iVar5] = 0;
        VS_VH_OBJ_364(0);
        return;
      }
      DAT_800d7664 = 0x40;
      if (((*param_1 & 0xff) == 0x70) && (DAT_800d7664 = 0x40, 4 < (int)param_1[1])) {
        DAT_800d7664 = 0x80;
      }
      uVar3 = (uint)DAT_800d7664;
      if (*(ushort *)((int)param_1 + 0x12) <= uVar3) {
        *(uint **)(&DAT_800d5228 + ((int)((uint)uVar10 << 0x10) >> 0xe)) = puVar6;
        iVar2 = 0;
        uVar9 = 0;
        puVar7 = puVar6;
        if (uVar3 != 0) {
          do {
            puVar7[2] = uVar9;
            if ((char)*puVar7 != '\0') {
              uVar9 = uVar9 + 1;
            }
            iVar2 = iVar2 + 1;
            puVar7 = puVar7 + 4;
          } while (iVar2 < (int)uVar3);
        }
        iVar2 = 0;
        piVar4 = local_428;
        *(uint **)(&DAT_800d52b8 + ((int)((uint)uVar10 << 0x10) >> 0xe)) = puVar6 + uVar3 * 4;
        bVar1 = *(byte *)((int)param_1 + 0x16);
        puVar7 = puVar6 + uVar3 * 4 + (uint)*(ushort *)((int)param_1 + 0x12) * 0x80;
        piVar8 = piVar4 + bVar1;
        do {
          if ((int)piVar4 <= (int)piVar8) {
            iVar5 = (uint)(ushort)*puVar7 << 2;
            if (4 < (int)param_1[1]) {
              iVar5 = (uint)(ushort)*puVar7 << 3;
            }
            *piVar4 = iVar5;
            iVar2 = iVar2 + *piVar4;
          }
          piVar4 = piVar4 + 1;
          puVar7 = (uint *)((int)puVar7 + 2);
        } while ((int)piVar4 < (int)auStack_28);
        if (param_3 == 0) {
          param_4 = SpuMalloc(iVar2);
          uVar3 = param_4 + iVar2;
          if (param_4 == -1) {
            VS_VH_OBJ_350(0);
            return;
          }
        }
        else {
          uVar3 = param_4 + iVar2;
        }
        iVar2 = 0;
        if (uVar3 < 0x80001) {
          *(long *)(&DAT_800eadf0 + ((int)((uint)uVar10 << 0x10) >> 0xe)) = param_4;
          uVar3 = 0;
          piVar4 = local_428;
          do {
            iVar2 = iVar2 + *piVar4;
            if ((uVar3 & 1) == 0) {
              *(short *)(puVar6 + ((int)uVar3 / 2) * 4 + 3) = (short)((uint)(param_4 + iVar2) >> 3);
            }
            else {
              *(short *)((int)puVar6 + ((int)uVar3 / 2) * 0x10 + 0xe) =
                   (short)((uint)(param_4 + iVar2) >> 3);
            }
            uVar3 = uVar3 + 1;
            piVar4 = piVar4 + 1;
          } while ((int)uVar3 <= (int)(uint)bVar1);
          *(int *)(&DAT_800eada0 + (short)uVar10 * 4) = iVar2;
          (&DAT_800dcc68)[(short)uVar10] = 2;
          return;
        }
      }
      (&DAT_800dcc68)[(short)uVar10] = 0;
      FUN_80082710(0);
      DAT_800eade8 = DAT_800eade8 + -1;
      VS_VH_OBJ_444();
      return;
    }
  }
  FUN_80082710(0);
  VS_VH_OBJ_444();
  return;
}


