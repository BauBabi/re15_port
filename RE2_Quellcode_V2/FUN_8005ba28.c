/* FUN_8005ba28 @ 0x8005ba28  (Ghidra headless, raw MIPS overlay) */

void FUN_8005ba28(uint param_1,undefined4 *param_2)

{
  byte bVar1;
  undefined1 uVar2;
  byte bVar3;
  char cVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  byte *pbVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  undefined1 uVar17;
  uint uVar18;
  ushort local_38;
  
  uVar10 = param_1 >> 0x18;
  uVar5 = (uint)(char)(&DAT_800d4c48)[uVar10];
  if (uVar5 == 0xffffffff) {
    return;
  }
  uVar18 = param_1 >> 0x10 & 0xff;
  pbVar9 = (byte *)((&DAT_800dbb78)[uVar10] + uVar18 * 4);
  local_38 = (ushort)param_1 & 0xff;
  if (*(int *)pbVar9 == -1) {
    return;
  }
  if ((*pbVar9 & 0x80) != 0) {
    uVar5 = *pbVar9 & 0x7f;
  }
  bVar1 = pbVar9[2];
  uVar13 = pbVar9[3] & 0x1f;
  uVar14 = (uint)(bVar1 >> 4);
  uVar15 = (uint)(pbVar9[3] >> 5);
  uVar16 = pbVar9[1] & 0x7f;
  iVar11 = (&DAT_800d75a0)[uVar10] + uVar16 * 0x200 + uVar14 * 0x20 + 0x820;
  if ((uVar5 == 4) && (3 < uVar18)) {
LAB_8005bb78:
    bVar3 = *(byte *)(iVar11 + 1) & 0xfb;
  }
  else {
    if (uVar5 == 2) {
      if (((*(byte *)(iVar11 + 1) & 4) != 0) && (DAT_800c4410 != 0)) {
        *(byte *)(iVar11 + 1) = *(byte *)(iVar11 + 1) | DAT_800c4410;
        goto LAB_8005bb88;
      }
      goto LAB_8005bb78;
    }
    if ((DAT_800c4410 == 0) || (uVar5 == 0)) goto LAB_8005bb78;
    bVar3 = *(byte *)(iVar11 + 1) | DAT_800c4410;
  }
  *(byte *)(iVar11 + 1) = bVar3;
LAB_8005bb88:
  uVar2 = *(undefined1 *)(iVar11 + 1);
  cVar4 = FUN_8005c92c(uVar13,bVar1 & 0xf);
  iVar7 = uVar13 * 0x20;
  if (cVar4 == '\0') {
    iVar8 = uVar13 * 2;
    (&DAT_800d4ca0)[iVar8] = bVar1 & 7;
    (&DAT_800d4ca1)[iVar8] = (char)uVar13;
    uVar17 = (undefined1)(param_1 >> 0x10);
    (&DAT_800d4f19)[iVar7] = uVar17;
    (&DAT_800d4f18)[iVar7] = 1;
    *(short *)(&DAT_800d4f1c + iVar7) = (short)uVar5;
    *(short *)(&DAT_800d4f1e + iVar7) = (short)uVar16;
    *(ushort *)(&DAT_800d4f20 + iVar7) = (ushort)(bVar1 >> 4);
    *(ushort *)(&DAT_800d4f22 + iVar7) = (ushort)*(byte *)(iVar11 + 6);
    *(ushort *)(&DAT_800d4f24 + iVar7) = (ushort)*(byte *)(iVar11 + 5);
    *(ushort *)(&DAT_800d4f26 + iVar7) = (ushort)*(byte *)(iVar11 + 2);
    *(ushort *)(&DAT_800d4f28 + iVar7) = (ushort)*(byte *)(iVar11 + 2);
    *(undefined4 *)(&DAT_800d4f2c + iVar7) = *param_2;
    *(undefined4 *)(&DAT_800d4f30 + iVar7) = param_2[1];
    uVar6 = param_2[2];
    *(ushort *)(&DAT_800d4f2a + iVar7) = local_38;
    *(undefined4 *)(&DAT_800d4f34 + iVar7) = uVar6;
    if (uVar15 != 0) {
      iVar11 = uVar14 * 0x20 + 0x820;
      do {
        uVar13 = uVar13 + 1;
        iVar11 = iVar11 + 0x20;
        uVar14 = uVar14 + 1;
        iVar7 = uVar13 * 0x20;
        iVar12 = (&DAT_800d75a0)[uVar10] + uVar16 * 0x200 + iVar11;
        *(undefined1 *)(iVar12 + 1) = uVar2;
        (&DAT_800d4ca2)[iVar8] = bVar1 & 7;
        (&DAT_800d4ca3)[iVar8] = (char)uVar13;
        (&DAT_800d4f19)[iVar7] = uVar17;
        (&DAT_800d4f18)[iVar7] = 1;
        *(short *)(&DAT_800d4f1c + iVar7) = (short)uVar5;
        *(short *)(&DAT_800d4f1e + iVar7) = (short)uVar16;
        *(short *)(&DAT_800d4f20 + iVar7) = (short)uVar14;
        *(ushort *)(&DAT_800d4f22 + iVar7) = (ushort)*(byte *)(iVar12 + 6);
        *(ushort *)(&DAT_800d4f24 + iVar7) = (ushort)*(byte *)(iVar12 + 5);
        *(ushort *)(&DAT_800d4f26 + iVar7) = (ushort)*(byte *)(iVar12 + 2);
        *(ushort *)(&DAT_800d4f28 + iVar7) = (ushort)*(byte *)(iVar12 + 2);
        *(undefined4 *)(&DAT_800d4f2c + iVar7) = *param_2;
        uVar15 = uVar15 - 1;
        *(undefined4 *)(&DAT_800d4f30 + iVar7) = param_2[1];
        uVar6 = param_2[2];
        *(ushort *)(&DAT_800d4f2a + iVar7) = local_38;
        *(undefined4 *)(&DAT_800d4f34 + iVar7) = uVar6;
        iVar8 = iVar8 + 2;
      } while (uVar15 != 0);
    }
  }
  return;
}


