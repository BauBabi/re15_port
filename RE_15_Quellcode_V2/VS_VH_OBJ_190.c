void VS_VH_OBJ_190(void)

{
  byte bVar1;
  short in_v0;
  int iVar2;
  uint uVar3;
  int *piVar4;
  uint *puVar5;
  uint *puVar6;
  int iVar7;
  int *piVar8;
  uint uVar9;
  ushort unaff_s2;
  uint *unaff_s3;
  short unaff_s5;
  long unaff_s6;
  
  DAT_800bbe00 = in_v0 + 1;
  iVar7 = (int)(short)unaff_s2;
  if (0xf < iVar7) {
    FUN_8005a970(0);
    VS_VH_OBJ_478();
    return;
  }
  *(uint **)(&DAT_800b2500 + iVar7 * 4) = unaff_s3;
  puVar5 = unaff_s3 + 8;
  if (*unaff_s3 >> 8 != 0x564142) {
    (&DAT_800b5334)[iVar7] = 0;
    VS_VH_OBJ_398(0);
    return;
  }
  DAT_800b284c = 0x40;
  if (((*unaff_s3 & 0xff) == 0x70) && (DAT_800b284c = 0x40, 4 < (int)unaff_s3[1])) {
    DAT_800b284c = 0x80;
  }
  uVar3 = (uint)DAT_800b284c;
  if (*(ushort *)((int)unaff_s3 + 0x12) <= uVar3) {
    *(uint **)(&DAT_800b24b8 + ((int)((uint)unaff_s2 << 0x10) >> 0xe)) = puVar5;
    iVar7 = 0;
    uVar9 = 0;
    puVar6 = puVar5;
    if (uVar3 != 0) {
      do {
        puVar6[2] = uVar9;
        if ((char)*puVar6 != '\0') {
          uVar9 = uVar9 + 1;
        }
        iVar7 = iVar7 + 1;
        puVar6 = puVar6 + 4;
      } while (iVar7 < (int)uVar3);
    }
    iVar7 = 0;
    piVar4 = (int *)&stack0x00000010;
    *(uint **)(&DAT_800b2544 + ((int)((uint)unaff_s2 << 0x10) >> 0xe)) = puVar5 + uVar3 * 4;
    bVar1 = *(byte *)((int)unaff_s3 + 0x16);
    puVar6 = puVar5 + uVar3 * 4 + (uint)*(ushort *)((int)unaff_s3 + 0x12) * 0x80;
    piVar8 = piVar4 + bVar1;
    do {
      if ((int)piVar4 <= (int)piVar8) {
        iVar2 = (uint)(ushort)*puVar6 << 2;
        if (4 < (int)unaff_s3[1]) {
          iVar2 = (uint)(ushort)*puVar6 << 3;
        }
        *piVar4 = iVar2;
        iVar7 = iVar7 + *piVar4;
      }
      piVar4 = piVar4 + 1;
      puVar6 = (uint *)((int)puVar6 + 2);
    } while ((int)piVar4 < (int)&stack0x00000410);
    *(uint **)(&DAT_800bbefc + (short)unaff_s2 * 4) = puVar6;
    if (unaff_s5 == 0) {
      unaff_s6 = SpuMalloc(iVar7);
      uVar3 = unaff_s6 + iVar7;
      if (unaff_s6 == -1) {
        (&DAT_800b5334)[(short)unaff_s2] = 0;
        VS_VH_OBJ_398(0);
        return;
      }
    }
    else {
      uVar3 = unaff_s6 + iVar7;
    }
    iVar7 = 0;
    if (uVar3 < 0x80001) {
      *(long *)(&DAT_800bbe08 + ((int)((uint)unaff_s2 << 0x10) >> 0xe)) = unaff_s6;
      uVar3 = 0;
      piVar4 = (int *)&stack0x00000010;
      do {
        iVar7 = iVar7 + *piVar4;
        if ((uVar3 & 1) == 0) {
          *(short *)(puVar5 + ((int)uVar3 / 2) * 4 + 3) = (short)((uint)(unaff_s6 + iVar7) >> 3);
        }
        else {
          *(short *)((int)puVar5 + ((int)uVar3 / 2) * 0x10 + 0xe) =
               (short)((uint)(unaff_s6 + iVar7) >> 3);
        }
        uVar3 = uVar3 + 1;
        piVar4 = piVar4 + 1;
      } while ((int)uVar3 <= (int)(uint)bVar1);
      *(int *)(&DAT_800bbda8 + (short)unaff_s2 * 4) = iVar7;
      (&DAT_800b5334)[(short)unaff_s2] = 2;
      return;
    }
  }
  (&DAT_800b5334)[(short)unaff_s2] = 0;
  FUN_8005a970(0);
  DAT_800bbe00 = DAT_800bbe00 + -1;
  VS_VH_OBJ_478();
  return;
}
