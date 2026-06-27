/* FUN_8005ecfc @ 0x8005ecfc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8005ed34) */
/* WARNING: Removing unreachable block (ram,0x8005ed38) */
/* WARNING: Removing unreachable block (ram,0x8005ed84) */

void FUN_8005ecfc(int param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 local_68 [4];
  undefined1 local_58 [24];
  undefined1 auStack_40 [4];
  undefined1 auStack_3c [4];
  undefined1 auStack_38 [8];
  undefined1 auStack_30 [4];
  undefined1 auStack_2c [4];
  undefined1 auStack_28 [8];
  undefined1 auStack_20 [4];
  undefined1 local_1c;
  
  puVar8 = local_68;
  puVar7 = &DAT_80011738;
  do {
    uVar4 = puVar7[1];
    uVar5 = puVar7[2];
    uVar6 = puVar7[3];
    *puVar8 = *puVar7;
    puVar8[1] = uVar4;
    puVar8[2] = uVar5;
    puVar8[3] = uVar6;
    puVar7 = puVar7 + 4;
    puVar8 = puVar8 + 4;
  } while (puVar7 != (undefined4 *)&DAT_80011758);
  *(undefined1 *)puVar8 = DAT_80011758;
  puVar1 = auStack_40 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001175c >> (3 - uVar2) * 8;
  auStack_40 = (undefined1  [4])DAT_8001175c;
  puVar1 = auStack_3c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011760 >> (3 - uVar2) * 8;
  auStack_3c = (undefined1  [4])DAT_80011760;
  puVar1 = auStack_38 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011764 >> (3 - uVar2) * 8;
  auStack_38._0_4_ = DAT_80011764;
  puVar1 = auStack_30 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011768 >> (3 - uVar2) * 8;
  auStack_30 = (undefined1  [4])DAT_80011768;
  puVar1 = auStack_2c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001176c >> (3 - uVar2) * 8;
  auStack_2c = (undefined1  [4])DAT_8001176c;
  puVar1 = auStack_28 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011770 >> (3 - uVar2) * 8;
  auStack_28._0_4_ = DAT_80011770;
  puVar1 = auStack_20 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011774 >> (3 - uVar2) * 8;
  auStack_20 = (undefined1  [4])DAT_80011774;
  local_1c = DAT_80011778;
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    iVar3 = *(int *)(param_1 + 0x198);
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    *(undefined4 *)(iVar3 + 0xac0) = 0;
    *(undefined4 *)(iVar3 + 0xb4) = *(undefined4 *)(iVar3 + 0xac8);
    *(undefined4 *)(iVar3 + 0xbc) = *(undefined4 *)(iVar3 + 0xad0);
    *(undefined4 *)(iVar3 + 0xb8) = *(undefined4 *)(iVar3 + 0xacc);
    *(undefined4 *)(iVar3 + 0xc0) = *(undefined4 *)(iVar3 + 0xad4);
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x158) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x204) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x2b0) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x35c) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x408) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x198) + 0x4b4) = 0;
    iVar3 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0xc0fffa,*(byte *)(param_1 + 0x105) + 0x1e2,
                 local_68);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0xc0fffa,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_40);
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0xad0034,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_30);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0xad0034,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_20);
    FUN_80017054(*(int *)(param_1 + 0x198) + 4,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x6bc,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x8c0,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
  }
  return;
}


