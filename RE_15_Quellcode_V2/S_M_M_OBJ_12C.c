undefined4 S_M_M_OBJ_12C(void)

{
  undefined4 uVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint uVar6;
  uint unaff_s1;
  int unaff_s2;
  int unaff_s3;
  
  uVar1 = 0xffffffff;
  if (unaff_s2 != -1) {
    puVar5 = (uint *)(unaff_s2 * 8 + DAT_80076dbc);
    if ((*puVar5 & 0x40000000) == 0) {
      uVar6 = puVar5[1];
      if (unaff_s1 < uVar6) {
        if (DAT_80076db8 < DAT_80076db4) {
          puVar2 = (uint *)(DAT_80076db8 * 8 + DAT_80076dbc);
          uVar4 = *puVar2;
          uVar3 = puVar2[1];
          *puVar2 = unaff_s1 + *puVar5 | 0x80000000;
          puVar2[1] = uVar6 - unaff_s1;
          DAT_80076db8 = DAT_80076db8 + 1;
          puVar2[2] = uVar4;
          puVar2[3] = uVar3;
        }
      }
      puVar5 = (uint *)(unaff_s2 * 8 + DAT_80076dbc);
      puVar5[1] = unaff_s1;
      *puVar5 = *puVar5 & 0xfffffff;
      _spu_gcSPU();
      uVar1 = *(undefined4 *)(unaff_s2 * 8 + DAT_80076dbc);
    }
    else {
      uVar1 = 0xffffffff;
      if ((unaff_s2 < DAT_80076db4) && (uVar1 = 0xffffffff, unaff_s1 <= puVar5[1] - unaff_s3)) {
        DAT_80076db8 = unaff_s2 + 1;
        puVar2 = (uint *)(DAT_80076db8 * 8 + DAT_80076dbc);
        *puVar2 = (*puVar5 & 0xfffffff) + unaff_s1 | 0x40000000;
        puVar2[1] = puVar5[1] - unaff_s1;
        puVar5[1] = unaff_s1;
        *puVar5 = *puVar5 & 0xfffffff;
        _spu_gcSPU();
        uVar1 = S_M_M_OBJ_2A8();
        return uVar1;
      }
    }
  }
  return uVar1;
}
