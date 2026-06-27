/* FUN_8005f634 @ 0x8005f634  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8005f734) */
/* WARNING: Removing unreachable block (ram,0x8005f780) */
/* WARNING: Removing unreachable block (ram,0x8005f678) */
/* WARNING: Removing unreachable block (ram,0x8005f67c) */
/* WARNING: Removing unreachable block (ram,0x8005f6c8) */

void FUN_8005f634(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  char cVar3;
  uint *puVar4;
  char *pcVar5;
  char *pcVar6;
  undefined4 uVar7;
  uint uVar8;
  undefined4 uVar9;
  uint uVar10;
  undefined4 uVar11;
  uint uVar12;
  char *pcVar13;
  uint *puVar14;
  char *pcVar15;
  uint *puVar16;
  int iVar17;
  char local_f0 [4];
  undefined4 local_ec [3];
  char local_e0 [56];
  uint local_a8 [10];
  undefined1 auStack_80 [4];
  undefined1 auStack_7c [4];
  undefined1 auStack_78 [4];
  undefined1 auStack_74 [4];
  undefined1 auStack_70 [4];
  undefined1 auStack_6c [4];
  undefined1 auStack_68 [4];
  undefined1 auStack_64 [4];
  undefined1 auStack_60 [4];
  undefined1 auStack_5c [4];
  undefined1 local_58;
  undefined1 auStack_50 [4];
  undefined1 auStack_4c [4];
  undefined1 auStack_48 [8];
  undefined1 auStack_40 [4];
  undefined1 local_3c;
  undefined1 local_3b;
  undefined1 local_3a;
  undefined1 auStack_38 [4];
  undefined1 local_34;
  undefined1 local_33;
  undefined1 auStack_30 [4];
  undefined1 local_2c;
  undefined1 local_2b;
  
  pcVar5 = "B";
  pcVar6 = local_f0;
  do {
    pcVar15 = pcVar6;
    pcVar13 = pcVar5;
    uVar7 = *(undefined4 *)(pcVar13 + 4);
    uVar9 = *(undefined4 *)(pcVar13 + 8);
    uVar11 = *(undefined4 *)(pcVar13 + 0xc);
    *(undefined4 *)pcVar15 = *(undefined4 *)pcVar13;
    *(undefined4 *)(pcVar15 + 4) = uVar7;
    *(undefined4 *)(pcVar15 + 8) = uVar9;
    *(undefined4 *)(pcVar15 + 0xc) = uVar11;
    pcVar5 = pcVar13 + 0x10;
    pcVar6 = pcVar15 + 0x10;
  } while (pcVar13 + 0x10 != "BCD");
  cVar2 = pcVar13[0x11];
  cVar3 = pcVar13[0x12];
  pcVar15[0x10] = s______________012456789__<_>__ABC_8001183a[0x1e];
  pcVar15[0x11] = cVar2;
  pcVar15[0x12] = cVar3;
  puVar14 = &DAT_8001185c;
  puVar4 = local_a8;
  do {
    puVar16 = puVar4;
    uVar8 = puVar14[1];
    uVar10 = puVar14[2];
    uVar12 = puVar14[3];
    *puVar16 = *puVar14;
    puVar16[1] = uVar8;
    puVar16[2] = uVar10;
    puVar16[3] = uVar12;
    puVar14 = puVar14 + 4;
    puVar4 = puVar16 + 4;
  } while (puVar14 != &DAT_8001187c);
  uVar8 = (int)puVar16 + 0x13U & 3;
  puVar4 = (uint *)(((int)puVar16 + 0x13U) - uVar8);
  *puVar4 = *puVar4 & -1 << (uVar8 + 1) * 8 | DAT_8001187c >> (3 - uVar8) * 8;
  puVar16[4] = DAT_8001187c;
  puVar1 = auStack_80 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011880 >> (3 - uVar8) * 8;
  auStack_80 = (undefined1  [4])DAT_80011880;
  puVar1 = auStack_7c + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011884 >> (3 - uVar8) * 8;
  auStack_7c = (undefined1  [4])DAT_80011884;
  puVar1 = auStack_78 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011888 >> (3 - uVar8) * 8;
  auStack_78 = (undefined1  [4])DAT_80011888;
  puVar1 = auStack_74 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_8001188c >> (3 - uVar8) * 8;
  auStack_74 = (undefined1  [4])DAT_8001188c;
  puVar1 = auStack_70 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011890 >> (3 - uVar8) * 8;
  auStack_70 = (undefined1  [4])DAT_80011890;
  puVar1 = auStack_6c + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011894 >> (3 - uVar8) * 8;
  auStack_6c = (undefined1  [4])DAT_80011894;
  puVar1 = auStack_68 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_80011898 >> (3 - uVar8) * 8;
  auStack_68 = (undefined1  [4])DAT_80011898;
  puVar1 = auStack_64 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_8001189c >> (3 - uVar8) * 8;
  auStack_64 = (undefined1  [4])DAT_8001189c;
  puVar1 = auStack_60 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118a0 >> (3 - uVar8) * 8;
  auStack_60 = (undefined1  [4])DAT_800118a0;
  puVar1 = auStack_5c + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118a4 >> (3 - uVar8) * 8;
  auStack_5c = (undefined1  [4])DAT_800118a4;
  local_58 = DAT_800118a8;
  puVar1 = auStack_50 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118ac >> (3 - uVar8) * 8;
  auStack_50 = (undefined1  [4])DAT_800118ac;
  puVar1 = auStack_4c + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118b0 >> (3 - uVar8) * 8;
  auStack_4c = (undefined1  [4])DAT_800118b0;
  puVar1 = auStack_48 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118b4 >> (3 - uVar8) * 8;
  auStack_48._0_4_ = DAT_800118b4;
  puVar1 = auStack_40 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118b8 >> (3 - uVar8) * 8;
  auStack_40 = (undefined1  [4])DAT_800118b8;
  local_3c = DAT_800118bc;
  local_3b = DAT_800118bd;
  local_3a = DAT_800118be;
  puVar1 = auStack_38 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118c0 >> (3 - uVar8) * 8;
  auStack_38 = (undefined1  [4])DAT_800118c0;
  local_34 = DAT_800118c4;
  local_33 = DAT_800118c5;
  puVar1 = auStack_30 + 3;
  uVar8 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar8) =
       *(uint *)(puVar1 + -uVar8) & -1 << (uVar8 + 1) * 8 | DAT_800118c8 >> (3 - uVar8) * 8;
  auStack_30 = (undefined1  [4])DAT_800118c8;
  local_2c = DAT_800118cc;
  local_2b = DAT_800118cd;
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    iVar17 = *(int *)(param_1 + 0x198) + 4;
    FUN_800171d0(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,local_f0
                );
    FUN_800172f8(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,local_a8
                );
    FUN_80017054(*(int *)(param_1 + 0x198) + 0xb0,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    iVar17 = *(int *)(param_1 + 0x198) + 0x15c;
    FUN_800171d0(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_38);
    FUN_800172f8(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_30);
    iVar17 = *(int *)(param_1 + 0x198) + 0x360;
    FUN_800171d0(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_38);
    FUN_800172f8(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_30);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x6bc,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x8c0,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    iVar17 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar17,*(byte *)(param_1 + 0x104) + 2,0xac0046,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_80);
    FUN_800172f8(iVar17,*(byte *)(param_1 + 0x104) + 2,0xac0046,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_60);
    iVar17 = *(int *)(param_1 + 0x198) + 4;
    FUN_800171d0(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_50);
    FUN_800172f8(iVar17,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_40);
  }
  return;
}


