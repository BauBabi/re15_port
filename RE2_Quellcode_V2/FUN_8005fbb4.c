/* FUN_8005fbb4 @ 0x8005fbb4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8005fbe0) */
/* WARNING: Removing unreachable block (ram,0x8005fbe4) */
/* WARNING: Removing unreachable block (ram,0x8005fc30) */

void FUN_8005fbb4(int param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined4 local_60 [4];
  undefined1 local_50 [24];
  undefined1 auStack_38 [4];
  undefined1 auStack_34 [4];
  undefined1 auStack_30 [8];
  undefined1 auStack_28 [4];
  undefined1 auStack_24 [4];
  undefined1 auStack_20 [4];
  undefined1 local_1c;
  undefined1 local_1b;
  undefined1 auStack_18 [4];
  undefined1 local_14;
  
  puVar7 = local_60;
  puVar6 = &DAT_80011738;
  do {
    uVar3 = puVar6[1];
    uVar4 = puVar6[2];
    uVar5 = puVar6[3];
    *puVar7 = *puVar6;
    puVar7[1] = uVar3;
    puVar7[2] = uVar4;
    puVar7[3] = uVar5;
    puVar6 = puVar6 + 4;
    puVar7 = puVar7 + 4;
  } while (puVar6 != (undefined4 *)&DAT_80011758);
  *(undefined1 *)puVar7 = DAT_80011758;
  puVar1 = auStack_38 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001175c >> (3 - uVar2) * 8;
  auStack_38 = (undefined1  [4])DAT_8001175c;
  puVar1 = auStack_34 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011760 >> (3 - uVar2) * 8;
  auStack_34 = (undefined1  [4])DAT_80011760;
  puVar1 = auStack_30 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011764 >> (3 - uVar2) * 8;
  auStack_30._0_4_ = DAT_80011764;
  puVar1 = auStack_28 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118d0 >> (3 - uVar2) * 8;
  auStack_28 = (undefined1  [4])DAT_800118d0;
  puVar1 = auStack_24 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118d4 >> (3 - uVar2) * 8;
  auStack_24 = (undefined1  [4])DAT_800118d4;
  puVar1 = auStack_20 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118d8 >> (3 - uVar2) * 8;
  auStack_20 = (undefined1  [4])DAT_800118d8;
  local_1c = DAT_800118dc;
  local_1b = DAT_800118dd;
  puVar1 = auStack_18 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011774 >> (3 - uVar2) * 8;
  auStack_18 = (undefined1  [4])DAT_80011774;
  local_14 = DAT_80011778;
  FUN_80017054(*(int *)(param_1 + 0x198) + 4,*(byte *)(param_1 + 0x104) + 2,0,
               *(byte *)(param_1 + 0x105) + 0x1e2);
  iVar8 = *(int *)(param_1 + 0x198) + 0x564;
  FUN_800171d0(iVar8,*(byte *)(param_1 + 0x104) + 2,0xc0fffa,*(byte *)(param_1 + 0x105) + 0x1e2,
               local_60);
  FUN_800172f8(iVar8,*(byte *)(param_1 + 0x104) + 2,0xc0fffa,*(byte *)(param_1 + 0x105) + 0x1e2,
               auStack_38);
  FUN_800171d0(iVar8,*(byte *)(param_1 + 0x104) + 2,0x2f0000,*(byte *)(param_1 + 0x105) + 0x1e2,
               auStack_28);
  FUN_800172f8(iVar8,*(byte *)(param_1 + 0x104) + 2,0x2f0000,*(byte *)(param_1 + 0x105) + 0x1e2,
               auStack_18);
  FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
               *(byte *)(param_1 + 0x105) + 0x1e2);
  FUN_80017054(*(int *)(param_1 + 0x198) + 0x6bc,*(byte *)(param_1 + 0x104) + 2,0,
               *(byte *)(param_1 + 0x105) + 0x1e2);
  FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
               *(byte *)(param_1 + 0x105) + 0x1e2);
  FUN_80017054(*(int *)(param_1 + 0x198) + 0x8c0,*(byte *)(param_1 + 0x104) + 2,0,
               *(byte *)(param_1 + 0x105) + 0x1e2);
  return;
}


