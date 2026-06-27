void FUN_800453d0(uint param_1)

{
  byte bVar1;
  byte bVar2;
  undefined1 uVar3;
  byte bVar4;
  undefined2 *puVar5;
  char cVar6;
  int iVar7;
  ushort uVar8;
  byte *pbVar9;
  uint uVar10;
  byte bVar11;
  uint uVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  undefined2 uVar16;
  uint uVar17;
  
  if ((*DAT_800ac784 & 0x2000) != 0) {
    param_1 = param_1 + 0xc;
  }
  if ((param_1 & 0xff) < 0x19) {
    uVar17 = (uint)DAT_800b21ef;
    if (uVar17 != 0xffffffff) {
      pbVar9 = (byte *)(*(int *)(DAT_800ac778 + 0x14) + (param_1 & 0xff) * 4);
      if ((*pbVar9 & 0x80) != 0) {
        uVar17 = *pbVar9 & 0x7f;
      }
      bVar1 = pbVar9[2];
      bVar2 = pbVar9[3];
      uVar12 = (uint)(bVar1 >> 4);
      uVar10 = (bVar2 & 0x1f) - 0x10;
      uVar3 = *(undefined1 *)(DAT_800ac778 + 6);
      uVar13 = (uint)(pbVar9[3] >> 5);
      bVar4 = pbVar9[1];
      bVar11 = pbVar9[2];
      iVar15 = DAT_800b25a4 + (bVar4 & 0x7f) * 0x200 + 0x820;
      iVar14 = iVar15 + uVar12 * 0x20;
      *(undefined1 *)(iVar15 + 3) = uVar3;
      *(undefined1 *)(iVar14 + 1) = uVar3;
      cVar6 = FUN_80045a18(uVar10 & 0xff,bVar11 & 0xf);
      bVar11 = bVar11 & 7;
      if (cVar6 == '\0') {
        (&DAT_800b22bc)[bVar2 & 0x1f] = bVar11;
        FUN_80045a64(DAT_800ac784 + 0xd);
        iVar7 = uVar10 * 0x12;
        uVar8 = (ushort)param_1 & 0xff;
        *(ushort *)(&DAT_800b2422 + iVar7) = uVar8;
        *(undefined2 *)(&DAT_800b2420 + iVar7) = 1;
        *(short *)(&DAT_800b2424 + iVar7) = (short)uVar17;
        uVar16 = (undefined2)(bVar4 & 0x7f);
        *(undefined2 *)(&DAT_800b2426 + iVar7) = uVar16;
        *(ushort *)(&DAT_800b2428 + iVar7) = (ushort)(bVar1 >> 4);
        *(ushort *)(&DAT_800b242a + iVar7) = (ushort)*(byte *)(iVar14 + 6);
        *(ushort *)(&DAT_800b242c + iVar7) = (ushort)*(byte *)(iVar14 + 5);
        *(undefined2 *)(&DAT_800b242e + iVar7) = DAT_800b2824;
        *(undefined2 *)(&DAT_800b2430 + iVar7) = DAT_800b2826;
        puVar5 = (undefined2 *)(&DAT_800b2420 + iVar7);
        while (uVar13 != 0) {
          uVar13 = uVar13 - 1;
          uVar12 = uVar12 + 1;
          if (7 < uVar10 + 1) {
            return;
          }
          uVar3 = *(undefined1 *)(DAT_800ac778 + 6);
          *(undefined1 *)(iVar15 + 3) = uVar3;
          *(undefined1 *)(iVar14 + 0x21) = uVar3;
          (&DAT_800b22cd)[uVar10] = bVar11;
          puVar5[10] = uVar8;
          puVar5[9] = 1;
          puVar5[0xb] = (short)uVar17;
          puVar5[0xc] = uVar16;
          puVar5[0xd] = (short)uVar12;
          puVar5[0xe] = (ushort)*(byte *)(iVar14 + 0x26);
          puVar5[0xf] = (ushort)*(byte *)(iVar14 + 0x25);
          puVar5[0x10] = DAT_800b2824;
          puVar5[0x11] = DAT_800b2826;
          uVar10 = uVar10 + 1;
          iVar14 = iVar14 + 0x20;
          puVar5 = puVar5 + 9;
        }
      }
    }
  }
  return;
}
