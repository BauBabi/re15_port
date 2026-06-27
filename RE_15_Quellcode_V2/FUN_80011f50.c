undefined4 FUN_80011f50(byte param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  ushort uVar2;
  undefined2 uVar3;
  ushort uVar4;
  short sVar5;
  int iVar6;
  byte bVar7;
  uint uVar8;
  byte bVar9;
  int iVar10;
  uint *puVar11;
  uint *puVar12;
  uint *puVar13;
  int unaff_s3;
  int unaff_s7;
  int iVar14;
  int iVar15;
  byte local_50 [24];
  undefined1 auStack_38 [4];
  undefined1 auStack_34 [4];
  undefined4 local_30;
  
  puVar12 = (uint *)0x0;
  local_30 = param_2;
  puVar1 = auStack_38 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  auStack_38 = (undefined1  [4])0x0;
  puVar1 = auStack_34 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | 0U >> (3 - uVar8) * 8;
  auStack_34 = (undefined1  [4])0x0;
  DAT_800aca54 = (uint)DAT_800acaec << 0x10 | DAT_800aca54 & 0x1fffffff;
  if (DAT_800aca4e == 0) {
    return 0;
  }
  puVar11 = &DAT_800acc2c;
  bVar7 = 0;
  bVar9 = DAT_800aca4e;
  do {
    if ((*puVar11 & 1) != 0) {
      bVar9 = bVar9 - 1;
      local_50[bVar9] = bVar7;
    }
    bVar7 = bVar7 + 1;
  } while ((bVar9 != 0) && (puVar11 = puVar11 + 0x7d, bVar7 < 0x14));
  DAT_8008f5e0 = 0x7fffffff;
  uVar3 = *(undefined2 *)
           (&UNK_8006e5a0 + (uint)param_1 * 4 + (uint)((DAT_800aca5c & 4) != 0) * 0x58);
  if (DAT_800aca4e != 0) {
    bVar9 = DAT_800aca4e;
    do {
      puVar11 = &DAT_800acc2c + (uint)local_50[0] * 0x7d;
      bVar9 = bVar9 - 1;
      if (((*puVar11 & DAT_800aca54 & 0xe0000000) != 0) &&
         ((puVar13 = puVar12, ((&DAT_800accbc)[(uint)local_50[0] * 0x7d] & 0x3000000) == 0x3000000
          || (iVar6 = (*(code *)(&PTR_LAB_8006e548)[param_1])(uVar3,puVar11,local_30),
             puVar13 = puVar11, iVar6 != 0)))) {
        puVar12 = puVar13;
      }
      local_50[0] = local_50[bVar9];
    } while (bVar9 != 0);
  }
  if (puVar12 == (uint *)0x0) {
    return 0;
  }
  iVar6 = FUN_8001b9b4(puVar12 + 0xd);
  if (iVar6 != 0) {
    return 0;
  }
  if (0x3f < (byte)puVar12[2]) {
    return 0;
  }
  iVar6 = puVar12[0xe] - DAT_800aca8c;
  if (iVar6 < 1) {
    iVar6 = -DAT_800aca8c - puVar12[0xe];
  }
  if ((iVar6 < 0x1f5) || (iVar6 = FUN_8001bafc(puVar12 + 0xd), iVar6 == 0)) goto LAB_80012370;
  sVar5 = FUN_8001bf04(&DAT_800aca88,auStack_38,*(undefined4 *)(DAT_800ac784 + 0x1b4));
  if (sVar5 == 0x800) {
    uVar4 = **(ushort **)(DAT_800ac784 + 0x1b4);
    uVar2 = (*(ushort **)(DAT_800ac784 + 0x1b4))[2];
    unaff_s7 = puVar12[0xd] - DAT_800aca88;
    iVar6 = DAT_800aca88;
LAB_80012280:
    unaff_s3 = ((uint)uVar4 + (int)(short)uVar2) - iVar6;
  }
  else {
    if (sVar5 < 0x801) {
      if (sVar5 != 0) {
        if (sVar5 == 0x400) {
          uVar4 = *(ushort *)(*(int *)(DAT_800ac784 + 0x1b4) + 2);
          uVar2 = *(ushort *)(*(int *)(DAT_800ac784 + 0x1b4) + 6);
          unaff_s7 = puVar12[0xf] - DAT_800aca90;
          iVar6 = DAT_800aca90;
          goto LAB_80012280;
        }
        goto LAB_800122d4;
      }
LAB_800122b0:
      sVar5 = *(short *)(*(int *)(DAT_800ac784 + 0x1b4) + 4);
      unaff_s7 = puVar12[0xd] - DAT_800aca88;
      iVar6 = DAT_800aca88;
    }
    else {
      if (sVar5 != 0xc00) {
        if (sVar5 != 0xf00) goto LAB_800122d4;
        goto LAB_800122b0;
      }
      sVar5 = *(short *)(*(int *)(DAT_800ac784 + 0x1b4) + 6);
      unaff_s7 = puVar12[0xf] - DAT_800aca90;
      iVar6 = DAT_800aca90;
    }
    unaff_s3 = sVar5 - iVar6;
  }
LAB_800122d4:
  iVar6 = 0x9c4;
  uVar8 = puVar12[0xe];
  iVar10 = 1;
  if ((int)uVar8 < DAT_800aca8c) {
    iVar6 = uVar8 - DAT_800aca8c;
    iVar10 = 0xff;
    if (iVar6 < 1) {
      iVar6 = -DAT_800aca8c - uVar8;
    }
    iVar6 = iVar6 + -0xd5c;
  }
  iVar14 = (puVar12[0xe] - DAT_800aca8c) * unaff_s3;
  if (iVar14 < 1) {
    iVar14 = (DAT_800aca8c - puVar12[0xe]) * unaff_s3;
  }
  iVar15 = iVar6 * unaff_s7;
  if (iVar15 < 1) {
    iVar15 = -iVar6 * unaff_s7;
  }
  if (iVar10 * iVar15 < iVar10 * iVar14) {
    return 0;
  }
LAB_80012370:
  *(byte *)((int)puVar12 + 0x93) = *(byte *)((int)puVar12 + 0x93) & 1;
  if (((param_1 == 8) && (DAT_8008f5e0 < 3000)) || (param_1 == 7)) {
    *(byte *)((int)puVar12 + 0x93) = *(byte *)((int)puVar12 + 0x93) | 0x40;
  }
  if (param_1 == 1) {
    FUN_80045024(0x1080001,&DAT_800aca88);
  }
  iVar6 = FUN_8001a780(puVar12);
  if (iVar6 != 0) {
    *(byte *)((int)puVar12 + 0x93) = *(byte *)((int)puVar12 + 0x93) | 0x80;
  }
  if ((*(byte *)((int)puVar12 + 0x93) & 1) == 0) {
    *(undefined1 *)((int)puVar12 + 7) = 0;
    *(undefined *)((int)puVar12 + 6) = (&DAT_8006f410)[DAT_800aca54 >> 0x1d];
    if ((DAT_800aca8c < (int)puVar12[0xe]) && (*(char *)((int)puVar12 + 6) == '\0')) {
      *(undefined1 *)((int)puVar12 + 6) = 1;
    }
    if (((int)puVar12[0xe] < DAT_800aca8c) && (*(char *)((int)puVar12 + 6) == '\x02')) {
      *(undefined1 *)((int)puVar12 + 6) = 1;
    }
    *(byte *)((int)puVar12 + 5) = param_1;
    bVar9 = *(byte *)((int)puVar12 + 0x93);
    sVar5 = *(short *)(&UNK_8006e0d0 + (uint)param_1 * 4 + (uint)(byte)puVar12[2] * 0x58);
    *(byte *)((int)puVar12 + 0x93) = bVar9 | 1;
    *(short *)((int)puVar12 + 0x9a) = *(short *)((int)puVar12 + 0x9a) - sVar5;
    if (((bVar9 & 0x40) != 0) && ((byte)puVar12[2] < 0x20)) {
      *(undefined2 *)((int)puVar12 + 0x9a) = 0xffff;
    }
    *(undefined1 *)(puVar12 + 1) = 2;
    if (*(short *)((int)puVar12 + 0x9a) < 0) {
      *(undefined1 *)(puVar12 + 1) = 3;
    }
  }
  else {
    *(byte *)((int)puVar12 + 0x93) = *(byte *)((int)puVar12 + 0x93) | 2;
    FUN_80011f50(param_1,local_30);
  }
  return 1;
}
