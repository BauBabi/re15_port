bool FUN_8004e054(byte param_1)

{
  byte bVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  undefined1 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  byte bVar9;
  byte bVar10;
  byte bVar11;
  
  bVar11 = (&DAT_800b10ac)[(uint)DAT_800b25bd * 4];
  bVar1 = (&DAT_800b10ac)[(uint)DAT_800b25be * 4];
  bVar10 = DAT_800b25bd;
  if ((&DAT_800b10ae)[(uint)DAT_800b25bd * 4] == '\x02') {
    bVar10 = DAT_800b25bd - 1;
  }
  bVar9 = DAT_800b25be;
  if ((&DAT_800b10ae)[(uint)DAT_800b25be * 4] == '\x02') {
    bVar9 = DAT_800b25be - 1;
  }
  uVar5 = (uint)(byte)(&DAT_800b10ad)[(uint)bVar9 * 4] +
          (uint)(byte)(&DAT_800b10ad)[(uint)bVar10 * 4];
  if ((DAT_800b260c != 0) && (param_1 != 0)) {
    FUN_80013b60(0x19,&DAT_801a0000,0);
  }
  uVar4 = DAT_800b25d5;
  switch(param_1) {
  case 0:
    goto switchD_8004e1a4_caseD_0;
  case 1:
    uVar5 = (uint)bVar10;
    uVar6 = (uint)bVar9;
    iVar7 = uVar5 * 4;
    if (uVar5 < uVar6) {
      iVar8 = uVar6 * 4;
      (&DAT_800b10ad)[iVar7] = (&DAT_800b10ad)[iVar7] + (&DAT_800b10ad)[iVar8];
      (&DAT_800b10ac)[iVar7] = DAT_800b25d5;
      FUN_800492b8(bVar10,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
      bVar10 = bVar9;
    }
    else {
      iVar7 = uVar6 * 4;
      iVar8 = uVar5 * 4;
      (&DAT_800b10ad)[iVar7] = (&DAT_800b10ad)[iVar7] + (&DAT_800b10ad)[iVar8];
      (&DAT_800b10ac)[iVar7] = DAT_800b25d5;
      FUN_800492b8(bVar9,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
    }
    (&DAT_800b10ac)[iVar8] = 0;
    (&DAT_800b10ad)[iVar8] = 0;
    (&DAT_800b10ae)[iVar8] = 0;
    break;
  case 2:
    goto LAB_8004e324;
  case 3:
LAB_8004e430:
    if ((byte)(&DAT_80074da8)[(uint)bVar1 * 0xc] < uVar5) {
      (&DAT_800b10ad)[(uint)bVar9 * 4] = (&DAT_80074da8)[(uint)bVar1 * 0xc];
      (&DAT_800b10ad)[(uint)bVar10 * 4] = (char)uVar5 - (&DAT_80074da8)[(uint)bVar1 * 0xc];
      return (bool)param_1;
    }
    iVar8 = (uint)bVar9 * 4;
    iVar7 = (uint)bVar10 * 4;
    (&DAT_800b10ac)[iVar8] = DAT_800b25d5;
    (&DAT_800b10ad)[iVar8] = (&DAT_800b10ad)[iVar8] + (&DAT_800b10ad)[iVar7];
    (&DAT_800b10ac)[iVar7] = 0;
    (&DAT_800b10ad)[iVar7] = 0;
    (&DAT_800b10ae)[iVar7] = 0;
    break;
  case 4:
    iVar7 = (uint)bVar9 * 4;
    uVar4 = (&DAT_800b10ac)[iVar7];
    uVar2 = (&DAT_800b10ad)[iVar7];
    uVar3 = (&DAT_800b10ae)[iVar7];
    iVar8 = (uint)bVar10 * 4;
    (&DAT_800b10ac)[iVar7] = DAT_800b25d5;
    (&DAT_800b10ad)[iVar7] = (&DAT_800b10ad)[iVar8];
    (&DAT_800b10ae)[iVar7] = (&DAT_800b10ae)[iVar8];
    FUN_800492b8(bVar9,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
    (&DAT_800b10ac)[iVar8] = uVar4;
    (&DAT_800b10ad)[iVar8] = uVar2;
    (&DAT_800b10ae)[iVar8] = uVar3;
    return (bool)param_1;
  case 5:
    if (bVar9 <= bVar10) goto LAB_8004e430;
LAB_8004e324:
    if ((byte)(&DAT_80074da8)[(uint)bVar11 * 0xc] < uVar5) {
      (&DAT_800b10ad)[(uint)bVar10 * 4] = (&DAT_80074da8)[(uint)bVar11 * 0xc];
      (&DAT_800b10ad)[(uint)bVar9 * 4] = (char)uVar5 - (&DAT_80074da8)[(uint)bVar11 * 0xc];
      return (bool)param_1;
    }
    iVar8 = (uint)bVar10 * 4;
    iVar7 = (uint)bVar9 * 4;
    (&DAT_800b10ac)[iVar8] = DAT_800b25d5;
    (&DAT_800b10ad)[iVar8] = (&DAT_800b10ad)[iVar8] + (&DAT_800b10ad)[iVar7];
    (&DAT_800b10ac)[iVar7] = 0;
    (&DAT_800b10ad)[iVar7] = 0;
    (&DAT_800b10ae)[iVar7] = 0;
    bVar10 = bVar9;
    break;
  case 6:
    bVar11 = bVar10;
    if (bVar9 < bVar10) {
      bVar11 = bVar9;
      bVar9 = bVar10;
    }
    iVar7 = (uint)bVar11 * 4;
    iVar8 = (uint)bVar9 * 4;
    bVar1 = (&DAT_800b10ad)[iVar7];
    bVar10 = (&DAT_800b10ad)[iVar8];
    if (bVar1 == bVar10) {
      (&DAT_800b10ac)[iVar7] = DAT_800b25d5;
      FUN_800492b8(bVar11,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
      (&DAT_800b10ac)[iVar8] = 0;
      (&DAT_800b10ad)[iVar8] = 0;
      (&DAT_800b10ae)[iVar8] = 0;
      FUN_8004947c(bVar9);
      return (bool)param_1;
    }
    if (bVar10 < bVar1) {
      (&DAT_800b10ac)[iVar8] = (&DAT_800b10ac)[iVar7];
      uVar4 = (&DAT_800b10ad)[iVar8];
      (&DAT_800b10ad)[iVar8] = (&DAT_800b10ad)[iVar7] - (&DAT_800b10ad)[iVar8];
      FUN_80049390((uint)bVar11,bVar9);
      uVar2 = DAT_800b25d5;
      (&DAT_800b10ad)[iVar7] = uVar4;
      (&DAT_800b10ac)[iVar7] = uVar2;
      FUN_800492b8(bVar11,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
      return (bool)param_1;
    }
    if (bVar1 < bVar10) {
      (&DAT_800b10ae)[iVar7] = 0;
      (&DAT_800b10ac)[iVar7] = uVar4;
      FUN_800492b8(bVar11,0,&DAT_801a0000 + (DAT_800b260c - 1) * 0x4b0);
      (&DAT_800b10ad)[iVar8] = (&DAT_800b10ad)[iVar8] - (&DAT_800b10ad)[iVar7];
      return (bool)param_1;
    }
    return (bool)param_1;
  default:
    return param_1 < 7;
  }
  FUN_8004947c(bVar10);
switchD_8004e1a4_caseD_0:
  return (bool)param_1;
}
