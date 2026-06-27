/* FUN_8005bd6c @ 0x8005bd6c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005bd6c(byte param_1,uint *param_2)

{
  byte bVar1;
  char cVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  byte *pbVar13;
  uint uVar14;
  uint uVar15;
  
  if ((*param_2 & 0x2000) != 0) {
    param_1 = param_1 + 0x10;
  }
  pbVar13 = (byte *)(DAT_800dbb84 + (uint)param_1 * 4);
  uVar15 = (uint)DAT_800d4c4b;
  if ((*(int *)pbVar13 != -1) && (uVar15 != 0xffffffff)) {
    if ((*pbVar13 & 0x80) != 0) {
      uVar15 = *pbVar13 & 0x7f;
    }
    bVar1 = pbVar13[2];
    uVar14 = pbVar13[3] & 0x1f;
    cVar2 = FUN_8005c92c(uVar14,bVar1 & 0xf);
    if (cVar2 == '\0') {
      bVar4 = pbVar13[2];
      uVar10 = (uint)(bVar4 >> 4);
      uVar12 = pbVar13[1] & 0x7f;
      uVar11 = (uint)(pbVar13[3] >> 5);
      iVar8 = DAT_800d75ac + uVar12 * 0x200 + uVar10 * 0x20 + 0x820;
      if (DAT_800c4410 == 0) {
        bVar3 = *(byte *)(iVar8 + 1) & 0xfb;
      }
      else {
        bVar3 = *(byte *)(iVar8 + 1) | DAT_800c4410;
      }
      *(byte *)(iVar8 + 1) = bVar3;
      iVar5 = uVar14 * 0x20;
      iVar9 = uVar14 * 2;
      (&DAT_800d4ca0)[iVar9] = bVar1 & 7;
      (&DAT_800d4ca1)[iVar9] = (char)uVar14;
      (&DAT_800d4f19)[iVar5] = param_1;
      (&DAT_800d4f18)[iVar5] = 1;
      *(short *)(&DAT_800d4f1c + iVar5) = (short)uVar15;
      *(short *)(&DAT_800d4f1e + iVar5) = (short)uVar12;
      *(ushort *)(&DAT_800d4f20 + iVar5) = (ushort)(bVar4 >> 4);
      *(ushort *)(&DAT_800d4f22 + iVar5) = (ushort)*(byte *)(iVar8 + 6);
      *(ushort *)(&DAT_800d4f24 + iVar5) = (ushort)*(byte *)(iVar8 + 5);
      *(ushort *)(&DAT_800d4f26 + iVar5) = (ushort)*(byte *)(iVar8 + 2);
      *(ushort *)(&DAT_800d4f28 + iVar5) = (ushort)*(byte *)(iVar8 + 2);
      *(uint *)(&DAT_800d4f2c + iVar5) = param_2[0xe];
      *(uint *)(&DAT_800d4f30 + iVar5) = param_2[0xf];
      uVar6 = param_2[0x10];
      *(undefined2 *)(&DAT_800d4f2a + iVar5) = 1;
      *(uint *)(&DAT_800d4f34 + iVar5) = uVar6;
      if (uVar11 != 0) {
        iVar8 = uVar10 * 0x20 + 0x820;
        do {
          uVar11 = uVar11 - 1;
          uVar14 = uVar14 + 1;
          iVar8 = iVar8 + 0x20;
          uVar10 = uVar10 + 1;
          iVar5 = DAT_800d75ac + uVar12 * 0x200 + iVar8;
          if (DAT_800c4410 == 0) {
            bVar4 = *(byte *)(iVar5 + 1) & 0xfb;
          }
          else {
            bVar4 = *(byte *)(iVar5 + 1) | DAT_800c4410;
          }
          *(byte *)(iVar5 + 1) = bVar4;
          iVar7 = uVar14 * 0x20;
          (&DAT_800d4ca2)[iVar9] = bVar1 & 7;
          (&DAT_800d4ca3)[iVar9] = (char)uVar14;
          (&DAT_800d4f19)[iVar7] = param_1;
          (&DAT_800d4f18)[iVar7] = 1;
          *(short *)(&DAT_800d4f1c + iVar7) = (short)uVar15;
          *(short *)(&DAT_800d4f1e + iVar7) = (short)uVar12;
          *(short *)(&DAT_800d4f20 + iVar7) = (short)uVar10;
          *(ushort *)(&DAT_800d4f22 + iVar7) = (ushort)*(byte *)(iVar5 + 6);
          *(ushort *)(&DAT_800d4f24 + iVar7) = (ushort)*(byte *)(iVar5 + 5);
          *(ushort *)(&DAT_800d4f26 + iVar7) = (ushort)*(byte *)(iVar5 + 2);
          *(ushort *)(&DAT_800d4f28 + iVar7) = (ushort)*(byte *)(iVar5 + 2);
          *(uint *)(&DAT_800d4f2c + iVar7) = param_2[0xe];
          *(uint *)(&DAT_800d4f30 + iVar7) = param_2[0xf];
          uVar6 = param_2[0x10];
          *(undefined2 *)(&DAT_800d4f2a + iVar7) = 1;
          *(uint *)(&DAT_800d4f34 + iVar7) = uVar6;
          iVar9 = iVar9 + 2;
        } while (uVar11 != 0);
      }
    }
  }
  return;
}


