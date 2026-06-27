/* SpuSetReverbModeParam @ 0x80080704  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x80080a80) */
/* WARNING: Removing unreachable block (ram,0x80080aa4) */
/* WARNING: Removing unreachable block (ram,0x80080b24) */
/* WARNING: Removing unreachable block (ram,0x80080b90) */
/* WARNING: Removing unreachable block (ram,0x80080ba8) */

long SpuSetReverbModeParam(SpuReverbAttr *attr)

{
  undefined1 uVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  int iVar5;
  long lVar6;
  undefined *puVar7;
  uint *puVar8;
  uint uVar9;
  uint local_80;
  short local_7c;
  short local_7a;
  undefined2 local_6e;
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
  
  bVar3 = false;
  local_38 = 0;
  uVar9 = attr->mask;
  bVar4 = false;
  bVar2 = uVar9 == 0;
  local_80 = 0;
  if ((!bVar2) && ((uVar9 & 1) == 0)) {
    if ((((bVar2) || ((uVar9 & 8) != 0)) && ((int)DAT_800ab29c < 9)) && (6 < (int)DAT_800ab29c)) {
      bVar3 = true;
      puVar8 = &local_80;
      iVar5 = 0x43;
      puVar7 = &DAT_800ab748 + DAT_800ab29c * 0x44;
      do {
        uVar1 = *puVar7;
        puVar7 = puVar7 + 1;
        iVar5 = iVar5 + -1;
        *(undefined1 *)puVar8 = uVar1;
        puVar8 = (uint *)((int)puVar8 + 1);
      } while (iVar5 != -1);
      local_80 = 0xc011c00;
      DAT_800ab2a4 = attr->delay;
      local_68 = (short)((DAT_800ab2a4 * 0x2000) / 0x7f) - local_7c;
      local_48 = (short)((DAT_800ab2a4 * 0x1000) / 0x7f);
      local_66 = local_48 - local_7a;
      local_5c = local_5a + local_48;
      local_64 = local_62 + local_48;
      local_46 = local_42 + local_48;
      local_48 = local_44 + local_48;
    }
    if (((bVar2) || ((uVar9 & 0x10) != 0)) && (((int)DAT_800ab29c < 9 && (6 < (int)DAT_800ab29c))))
    {
      bVar4 = true;
      puVar8 = &local_80;
      if (!bVar3) {
        iVar5 = 0x43;
        puVar7 = &DAT_800ab748 + DAT_800ab29c * 0x44;
        do {
          uVar1 = *puVar7;
          puVar7 = puVar7 + 1;
          iVar5 = iVar5 + -1;
          *(undefined1 *)puVar8 = uVar1;
          puVar8 = (uint *)((int)puVar8 + 1);
        } while (iVar5 != -1);
        lVar6 = S_SRMP_OBJ_334();
        return lVar6;
      }
      local_80 = local_80 | 0x80;
      DAT_800ab2a8 = attr->feedback;
      local_6e = (undefined2)((DAT_800ab2a8 * 0x8100) / 0x7f);
    }
    if ((bVar2) || ((uVar9 & 2) != 0)) {
      *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = (attr->depth).left;
      DAT_800ab2a0 = (attr->depth).left;
    }
    if ((!bVar2) && ((uVar9 & 4) == 0)) {
      if ((bVar3) || (bVar4)) {
        _spu_setReverbAttr(&local_80);
      }
      if (local_38 != 0) {
        SpuClearReverbWorkArea(DAT_800ab29c);
      }
      return 0;
    }
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x186) = (attr->depth).right;
    DAT_800ab2a2 = (attr->depth).right;
    lVar6 = S_SRMP_OBJ_444();
    return lVar6;
  }
  uVar9 = attr->mode;
  bVar2 = (uVar9 & 0x100) != 0;
  if (bVar2) {
    uVar9 = uVar9 & 0xfffffeff;
  }
  local_38 = (uint)bVar2;
  if ((9 < uVar9) || (iVar5 = _SpuIsInAllocateArea_((&DAT_800ab6f8)[uVar9]), iVar5 != 0)) {
    lVar6 = S_SRMP_OBJ_4C4();
    return lVar6;
  }
  puVar8 = &local_80;
  iVar5 = 0x43;
  DAT_800ab294 = (&DAT_800ab6f8)[uVar9];
  puVar7 = &DAT_800ab748 + uVar9 * 0x44;
  do {
    uVar1 = *puVar7;
    puVar7 = puVar7 + 1;
    iVar5 = iVar5 + -1;
    *(undefined1 *)puVar8 = uVar1;
    puVar8 = (uint *)((int)puVar8 + 1);
  } while (iVar5 != -1);
  DAT_800ab29c = uVar9;
  if (uVar9 == 7) {
    DAT_800ab2a8 = 0x7f;
    DAT_800ab2a4 = 0x7f;
    lVar6 = S_SRMP_OBJ_180();
    return lVar6;
  }
  if (uVar9 == 8) {
    DAT_800ab2a8 = 0;
    DAT_800ab2a4 = 0x7f;
    lVar6 = S_SRMP_OBJ_180();
    return lVar6;
  }
  lVar6 = S_SRMP_OBJ_170();
  return lVar6;
}


