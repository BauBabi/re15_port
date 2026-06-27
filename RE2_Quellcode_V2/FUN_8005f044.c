/* FUN_8005f044 @ 0x8005f044  (Ghidra headless, raw MIPS overlay) */

void FUN_8005f044(int param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 auStack_80 [4];
  undefined1 auStack_7c [4];
  undefined1 auStack_78 [4];
  undefined1 auStack_74 [4];
  undefined1 auStack_70 [4];
  undefined1 auStack_6c [4];
  undefined1 local_68;
  undefined1 local_67;
  undefined1 auStack_60 [4];
  undefined1 local_5c;
  undefined1 local_5b;
  undefined1 auStack_58 [8];
  undefined1 auStack_50 [4];
  undefined1 local_4c;
  undefined1 auStack_48 [4];
  undefined1 auStack_44 [4];
  undefined1 local_40;
  undefined1 auStack_38 [4];
  undefined1 auStack_34 [4];
  undefined1 auStack_30 [4];
  undefined1 local_2c;
  undefined1 local_2b;
  
  puVar1 = auStack_80 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001177c >> (3 - uVar2) * 8;
  auStack_80 = (undefined1  [4])DAT_8001177c;
  puVar1 = auStack_7c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011780 >> (3 - uVar2) * 8;
  auStack_7c = (undefined1  [4])DAT_80011780;
  puVar1 = auStack_78 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011784 >> (3 - uVar2) * 8;
  auStack_78 = (undefined1  [4])DAT_80011784;
  puVar1 = auStack_74 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011788 >> (3 - uVar2) * 8;
  auStack_74 = (undefined1  [4])DAT_80011788;
  puVar1 = auStack_70 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001178c >> (3 - uVar2) * 8;
  auStack_70 = (undefined1  [4])DAT_8001178c;
  puVar1 = auStack_6c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011790 >> (3 - uVar2) * 8;
  auStack_6c = (undefined1  [4])DAT_80011790;
  local_68 = DAT_80011794;
  local_67 = DAT_80011795;
  puVar1 = auStack_60 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011798 >> (3 - uVar2) * 8;
  auStack_60 = (undefined1  [4])DAT_80011798;
  local_5c = DAT_8001179c;
  local_5b = DAT_8001179d;
  puVar1 = auStack_58 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117a0 >> (3 - uVar2) * 8;
  auStack_58._0_4_ = DAT_800117a0;
  puVar1 = auStack_50 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117a4 >> (3 - uVar2) * 8;
  auStack_50 = (undefined1  [4])DAT_800117a4;
  local_4c = DAT_800117a8;
  puVar1 = auStack_48 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117ac >> (3 - uVar2) * 8;
  auStack_48 = (undefined1  [4])DAT_800117ac;
  puVar1 = auStack_44 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117b0 >> (3 - uVar2) * 8;
  auStack_44 = (undefined1  [4])DAT_800117b0;
  local_40 = DAT_800117b4;
  puVar1 = auStack_38 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117b8 >> (3 - uVar2) * 8;
  auStack_38 = (undefined1  [4])DAT_800117b8;
  puVar1 = auStack_34 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117bc >> (3 - uVar2) * 8;
  auStack_34 = (undefined1  [4])DAT_800117bc;
  puVar1 = auStack_30 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800117c0 >> (3 - uVar2) * 8;
  auStack_30 = (undefined1  [4])DAT_800117c0;
  local_2c = DAT_800117c4;
  local_2b = DAT_800117c5;
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 4,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    iVar3 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0x850000,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_80);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0x850000,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_60);
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0x50,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_58);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0x50,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_50);
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0x380044,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_48);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0x380044,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_38);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
  }
  return;
}


