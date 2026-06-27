void FUN_80045024(uint param_1,undefined4 param_2)

{
  byte bVar1;
  byte bVar2;
  undefined1 uVar3;
  byte bVar4;
  char cVar5;
  int iVar6;
  uint uVar7;
  undefined2 *puVar8;
  undefined *puVar9;
  byte *pbVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  short prog;
  short vabId;
  
  uVar7 = param_1 >> 0x18;
  uVar11 = (uint)(char)(&DAT_800b21ec)[uVar7];
  if (uVar11 == 0xffffffff) {
    return;
  }
  uVar16 = param_1 >> 0x10 & 0xff;
  switch(uVar7) {
  case 0:
    if (0x20 < uVar16) {
      return;
    }
    puVar9 = &DAT_801fdd00;
    break;
  case 1:
    if (0x20 < uVar16) {
      return;
    }
    puVar9 = &DAT_801fcd00;
    break;
  case 2:
    if (0x20 < uVar16) {
      return;
    }
  case 5:
    puVar9 = *(undefined **)(DAT_800ac778 + 8);
    break;
  case 3:
    if (0x18 < uVar16) {
      return;
    }
    puVar9 = *(undefined **)(DAT_800ac778 + 0x14);
    break;
  case 4:
    if (0x20 < uVar16) {
      return;
    }
    puVar9 = &DAT_801fbd00;
    break;
  default:
    goto switchD_800450b4_default;
  }
  pbVar10 = puVar9 + uVar16 * 4;
  if ((*pbVar10 & 0x80) != 0) {
    uVar11 = *pbVar10 & 0x7f;
  }
  bVar4 = pbVar10[2] >> 4;
  uVar14 = (uint)bVar4;
  uVar15 = (uint)(pbVar10[3] >> 5);
  bVar1 = pbVar10[1];
  bVar2 = pbVar10[2];
  uVar12 = pbVar10[3] & 0x1f;
  iVar17 = (&DAT_800b2598)[uVar7] + (bVar1 & 0x7f) * 0x200 + 0x820;
  iVar13 = iVar17 + uVar14 * 0x20;
  uVar3 = *(undefined1 *)(DAT_800ac778 + 6);
  *(undefined1 *)(iVar17 + 3) = uVar3;
  *(undefined1 *)(iVar13 + 1) = uVar3;
  vabId = (short)uVar11;
  prog = (short)(bVar1 & 0x7f);
  if (uVar12 < 0x10) {
    if ((param_1 & 0xff) == 0) {
      DAT_800b2824 = (ushort)*(byte *)(iVar13 + 2);
      DAT_800b2826 = DAT_800b2824;
    }
    else {
      FUN_80045a64(param_2);
    }
    SsUtKeyOnV((short)uVar12,vabId,prog,(ushort)bVar4,(ushort)*(byte *)(iVar13 + 6),
               (ushort)*(byte *)(iVar13 + 5),DAT_800b2824,DAT_800b2826);
    uVar7 = uVar12;
  }
  else {
    uVar7 = uVar12 - 0x10;
    cVar5 = FUN_80045a18(uVar7 & 0xff,bVar2 & 0xf);
    if (cVar5 != '\0') {
      return;
    }
    (&DAT_800b22bc)[uVar12] = bVar2 & 7;
  }
  if ((param_1 & 0xff) == 0) {
    DAT_800b2824 = (ushort)*(byte *)(iVar13 + 2);
    DAT_800b2826 = DAT_800b2824;
  }
  else {
    FUN_80045a64(param_2);
  }
  iVar6 = uVar7 * 0x12;
  *(short *)(&DAT_800b2422 + iVar6) = (short)uVar16;
  *(undefined2 *)(&DAT_800b2420 + iVar6) = 1;
  *(short *)(&DAT_800b2424 + iVar6) = vabId;
  *(short *)(&DAT_800b2426 + iVar6) = prog;
  *(ushort *)(&DAT_800b2428 + iVar6) = (ushort)bVar4;
  *(ushort *)(&DAT_800b242a + iVar6) = (ushort)*(byte *)(iVar13 + 6);
  *(ushort *)(&DAT_800b242c + iVar6) = (ushort)*(byte *)(iVar13 + 5);
  *(ushort *)(&DAT_800b242e + iVar6) = DAT_800b2824;
  *(ushort *)(&DAT_800b2430 + iVar6) = DAT_800b2826;
  if (uVar15 != 0) {
    puVar8 = (undefined2 *)(&DAT_800b2420 + iVar6);
    iVar13 = uVar14 * 0x20 + iVar17;
    do {
      uVar3 = *(undefined1 *)(DAT_800ac778 + 6);
      uVar14 = uVar14 + 1;
      *(undefined1 *)(iVar17 + 3) = uVar3;
      *(undefined1 *)(iVar13 + 0x21) = uVar3;
      (&DAT_800b22cd)[uVar7] = bVar2 & 7;
      puVar8[10] = (short)uVar16;
      puVar8[9] = 1;
      puVar8[0xb] = vabId;
      puVar8[0xc] = prog;
      puVar8[0xd] = (short)uVar14;
      puVar8[0xe] = (ushort)*(byte *)(iVar13 + 0x26);
      puVar8[0xf] = (ushort)*(byte *)(iVar13 + 0x25);
      puVar8[0x10] = DAT_800b2824;
      uVar15 = uVar15 - 1;
      puVar8[0x11] = DAT_800b2826;
      puVar8 = puVar8 + 9;
      iVar13 = iVar13 + 0x20;
      uVar7 = uVar7 + 1;
    } while (uVar15 != 0);
  }
switchD_800450b4_default:
  return;
}
