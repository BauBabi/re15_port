/* FUN_800600b8 @ 0x800600b8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800600f0) */
/* WARNING: Removing unreachable block (ram,0x800600f4) */
/* WARNING: Removing unreachable block (ram,0x80060140) */

void FUN_800600b8(int param_1)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  int iVar11;
  undefined4 local_88 [4];
  undefined1 local_78 [24];
  undefined1 auStack_60 [4];
  undefined1 auStack_5c [4];
  undefined1 auStack_58 [4];
  undefined1 auStack_54 [4];
  undefined1 local_50;
  undefined1 auStack_48 [4];
  undefined1 auStack_44 [4];
  undefined1 auStack_40 [4];
  undefined1 auStack_3c [4];
  undefined1 auStack_38 [4];
  undefined1 auStack_34 [4];
  undefined1 local_30;
  undefined1 auStack_28 [4];
  undefined1 auStack_24 [4];
  undefined1 local_20;
  undefined1 local_1f;
  
  puVar4 = &DAT_80011924;
  puVar5 = local_88;
  do {
    puVar10 = puVar5;
    puVar9 = puVar4;
    uVar6 = puVar9[1];
    uVar7 = puVar9[2];
    uVar8 = puVar9[3];
    *puVar10 = *puVar9;
    puVar10[1] = uVar6;
    puVar10[2] = uVar7;
    puVar10[3] = uVar8;
    puVar4 = puVar9 + 4;
    puVar5 = puVar10 + 4;
  } while (puVar9 + 4 != (undefined4 *)&DAT_80011944);
  uVar2 = *(undefined1 *)((int)puVar9 + 0x11);
  *(undefined1 *)(puVar10 + 4) = DAT_80011944;
  *(undefined1 *)((int)puVar10 + 0x11) = uVar2;
  puVar1 = auStack_60 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011948 >> (3 - uVar3) * 8;
  auStack_60 = (undefined1  [4])DAT_80011948;
  puVar1 = auStack_5c + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_8001194c >> (3 - uVar3) * 8;
  auStack_5c = (undefined1  [4])DAT_8001194c;
  puVar1 = auStack_58 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011950 >> (3 - uVar3) * 8;
  auStack_58 = (undefined1  [4])DAT_80011950;
  puVar1 = auStack_54 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011954 >> (3 - uVar3) * 8;
  auStack_54 = (undefined1  [4])DAT_80011954;
  local_50 = DAT_80011958;
  puVar1 = auStack_48 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_800118fc >> (3 - uVar3) * 8;
  auStack_48 = (undefined1  [4])DAT_800118fc;
  puVar1 = auStack_44 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011900 >> (3 - uVar3) * 8;
  auStack_44 = (undefined1  [4])DAT_80011900;
  puVar1 = auStack_40 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011904 >> (3 - uVar3) * 8;
  auStack_40 = (undefined1  [4])DAT_80011904;
  puVar1 = auStack_3c + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011908 >> (3 - uVar3) * 8;
  auStack_3c = (undefined1  [4])DAT_80011908;
  puVar1 = auStack_38 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_8001190c >> (3 - uVar3) * 8;
  auStack_38 = (undefined1  [4])DAT_8001190c;
  puVar1 = auStack_34 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011910 >> (3 - uVar3) * 8;
  auStack_34 = (undefined1  [4])DAT_80011910;
  local_30 = DAT_80011914;
  puVar1 = auStack_28 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80011918 >> (3 - uVar3) * 8;
  auStack_28 = (undefined1  [4])DAT_80011918;
  puVar1 = auStack_24 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_8001191c >> (3 - uVar3) * 8;
  auStack_24 = (undefined1  [4])DAT_8001191c;
  local_20 = DAT_80011920;
  local_1f = DAT_80011921;
  if ((*(ushort *)(param_1 + 0x10e) & 0x200) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfdff;
    iVar11 = *(int *)(param_1 + 0x198) + 4;
    FUN_800171d0(iVar11,*(byte *)(param_1 + 0x104) + 2,0x800000,*(byte *)(param_1 + 0x105) + 0x1e2,
                 local_88);
    FUN_800172f8(iVar11,*(byte *)(param_1 + 0x104) + 2,0x800000,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_60);
    iVar11 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar11,*(byte *)(param_1 + 0x104) + 2,0x34004e,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_48);
    FUN_800172f8(iVar11,*(byte *)(param_1 + 0x104) + 2,0x34004e,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_28);
  }
  return;
}


