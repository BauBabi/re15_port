/* FUN_8005f354 @ 0x8005f354  (Ghidra headless, raw MIPS overlay) */

void FUN_8005f354(int param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 auStack_78 [4];
  undefined1 auStack_74 [4];
  undefined1 auStack_70 [4];
  undefined1 auStack_6c [4];
  undefined1 auStack_68 [4];
  undefined1 auStack_64 [4];
  undefined1 auStack_60 [4];
  undefined1 auStack_5c [4];
  undefined1 auStack_58 [4];
  undefined1 auStack_54 [4];
  undefined1 auStack_50 [4];
  undefined1 local_4c;
  undefined1 local_4b;
  undefined1 auStack_48 [4];
  undefined1 auStack_44 [4];
  undefined1 auStack_40 [8];
  undefined1 auStack_38 [4];
  undefined1 local_34;
  undefined1 auStack_30 [4];
  undefined1 auStack_2c [4];
  undefined1 local_28;
  undefined1 local_27;
  
  puVar1 = auStack_78 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117c8 >> (3 - uVar2) * 8;
  auStack_78 = (undefined1  [4])DAT_800117c8;
  puVar1 = auStack_74 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117cc >> (3 - uVar2) * 8;
  auStack_74 = (undefined1  [4])DAT_800117cc;
  puVar1 = auStack_70 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117d0 >> (3 - uVar2) * 8;
  auStack_70 = (undefined1  [4])DAT_800117d0;
  puVar1 = auStack_6c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117d4 >> (3 - uVar2) * 8;
  auStack_6c = (undefined1  [4])DAT_800117d4;
  puVar1 = auStack_68 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117d8 >> (3 - uVar2) * 8;
  auStack_68 = (undefined1  [4])DAT_800117d8;
  puVar1 = auStack_64 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117dc >> (3 - uVar2) * 8;
  auStack_64 = (undefined1  [4])DAT_800117dc;
  puVar1 = auStack_60 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117e0 >> (3 - uVar2) * 8;
  auStack_60 = (undefined1  [4])DAT_800117e0;
  puVar1 = auStack_5c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117e4 >> (3 - uVar2) * 8;
  auStack_5c = (undefined1  [4])DAT_800117e4;
  puVar1 = auStack_58 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117e8 >> (3 - uVar2) * 8;
  auStack_58 = (undefined1  [4])DAT_800117e8;
  puVar1 = auStack_54 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117ec >> (3 - uVar2) * 8;
  auStack_54 = (undefined1  [4])DAT_800117ec;
  puVar1 = auStack_50 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117f0 >> (3 - uVar2) * 8;
  auStack_50 = (undefined1  [4])DAT_800117f0;
  local_4c = DAT_800117f4;
  local_4b = DAT_800117f5;
  puVar1 = auStack_48 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117f8 >> (3 - uVar2) * 8;
  auStack_48 = (undefined1  [4])DAT_800117f8;
  puVar1 = auStack_44 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117fc >> (3 - uVar2) * 8;
  auStack_44 = (undefined1  [4])DAT_800117fc;
  puVar1 = auStack_40 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011800 >> (3 - uVar2) * 8;
  auStack_40._0_4_ = DAT_80011800;
  puVar1 = auStack_38 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011804 >> (3 - uVar2) * 8;
  auStack_38 = (undefined1  [4])DAT_80011804;
  local_34 = DAT_80011808;
  puVar1 = auStack_30 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001180c >> (3 - uVar2) * 8;
  auStack_30 = (undefined1  [4])DAT_8001180c;
  puVar1 = auStack_2c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011810 >> (3 - uVar2) * 8;
  auStack_2c = (undefined1  [4])DAT_80011810;
  local_28 = DAT_80011814;
  local_27 = DAT_80011815;
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    iVar3 = *(int *)(param_1 + 0x198) + 4;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_78);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_58);
    iVar3 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0xac0046,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_48);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0xac0046,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_38);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0xffda0004,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_30);
  }
  return;
}


