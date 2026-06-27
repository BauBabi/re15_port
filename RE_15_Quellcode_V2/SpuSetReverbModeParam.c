long SpuSetReverbModeParam(SpuReverbAttr *attr)

{
  undefined1 uVar1;
  bool bVar2;
  int iVar3;
  long lVar4;
  undefined *puVar5;
  undefined4 *puVar6;
  uint uVar7;
  undefined4 local_80;
  short local_7c;
  short local_7a;
  short local_68;
  short local_66;
  short local_64;
  short local_62;
  short local_5c;
  short local_5a;
  short local_48;
  short local_46;
  short local_44;
  short local_42;
  uint local_38;
  
  local_38 = 0;
  uVar7 = attr->mask;
  bVar2 = uVar7 == 0;
  local_80 = 0;
  if ((!bVar2) && ((uVar7 & 1) == 0)) {
    if ((bVar2) || ((uVar7 & 8) != 0)) {
      if (((int)DAT_80076dd8 < 9) && (6 < (int)DAT_80076dd8)) {
        puVar6 = &local_80;
        iVar3 = 0x43;
        puVar5 = &DAT_80077274 + DAT_80076dd8 * 0x44;
        do {
          uVar1 = *puVar5;
          puVar5 = puVar5 + 1;
          iVar3 = iVar3 + -1;
          *(undefined1 *)puVar6 = uVar1;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        } while (iVar3 != -1);
        local_80 = 0xc011c00;
        DAT_80076de0 = attr->delay;
        local_68 = (short)((DAT_80076de0 * 0x2000) / 0x7f) - local_7c;
        local_46 = (short)((DAT_80076de0 * 0x1000) / 0x7f);
        local_66 = local_46 - local_7a;
        local_5c = local_46 + local_5a;
        local_64 = local_46 + local_62;
        local_48 = local_46 + local_44;
        local_46 = local_46 + local_42;
        lVar4 = S_SRMP_OBJ_2AC();
        return lVar4;
      }
      DAT_80076de0 = 0;
    }
    if ((bVar2) || ((uVar7 & 0x10) != 0)) {
      if (((int)DAT_80076dd8 < 9) && (6 < (int)DAT_80076dd8)) {
        puVar6 = &local_80;
        iVar3 = 0x43;
        puVar5 = &DAT_80077274 + DAT_80076dd8 * 0x44;
        do {
          uVar1 = *puVar5;
          puVar5 = puVar5 + 1;
          iVar3 = iVar3 + -1;
          *(undefined1 *)puVar6 = uVar1;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        } while (iVar3 != -1);
        lVar4 = S_SRMP_OBJ_340();
        return lVar4;
      }
      DAT_80076de4 = 0;
    }
    if ((bVar2) || ((uVar7 & 2) != 0)) {
      SPU_REVERB_OUT_L = (attr->depth).left;
      DAT_80076ddc = (attr->depth).left;
    }
    if ((!bVar2) && ((uVar7 & 4) == 0)) {
      return 0;
    }
    SPU_REVERB_OUT_R = (attr->depth).right;
    DAT_80076dde = (attr->depth).right;
    lVar4 = S_SRMP_OBJ_45C();
    return lVar4;
  }
  uVar7 = attr->mode;
  bVar2 = (uVar7 & 0x100) != 0;
  if (bVar2) {
    uVar7 = uVar7 & 0xfffffeff;
  }
  local_38 = (uint)bVar2;
  if ((9 < uVar7) || (iVar3 = _SpuIsInAllocateArea_((&DAT_80077224)[uVar7]), iVar3 != 0)) {
    lVar4 = S_SRMP_OBJ_4DC();
    return lVar4;
  }
  puVar6 = &local_80;
  iVar3 = 0x43;
  DAT_80076dd0 = (&DAT_80077224)[uVar7];
  puVar5 = &DAT_80077274 + uVar7 * 0x44;
  do {
    uVar1 = *puVar5;
    puVar5 = puVar5 + 1;
    iVar3 = iVar3 + -1;
    *(undefined1 *)puVar6 = uVar1;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  } while (iVar3 != -1);
  DAT_80076dd8 = uVar7;
  if (uVar7 != 7) {
    if (uVar7 != 8) {
      lVar4 = S_SRMP_OBJ_170();
      return lVar4;
    }
    DAT_80076de4 = 0;
    DAT_80076de0 = 0x7f;
    lVar4 = S_SRMP_OBJ_180();
    return lVar4;
  }
  DAT_80076de4 = 0x7f;
  DAT_80076de0 = 0x7f;
  lVar4 = S_SRMP_OBJ_180();
  return lVar4;
}
