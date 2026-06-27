/* FUN_8005fe6c @ 0x8005fe6c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005fe6c(int param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 auStack_68 [4];
  undefined1 auStack_64 [4];
  undefined1 auStack_60 [4];
  undefined1 auStack_5c [4];
  undefined1 local_58;
  undefined1 local_57;
  undefined1 local_56;
  undefined1 auStack_50 [4];
  undefined1 local_4c;
  undefined1 local_4b;
  undefined1 local_4a;
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
  
  puVar1 = auStack_68 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118e0 >> (3 - uVar2) * 8;
  auStack_68 = (undefined1  [4])DAT_800118e0;
  puVar1 = auStack_64 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118e4 >> (3 - uVar2) * 8;
  auStack_64 = (undefined1  [4])DAT_800118e4;
  puVar1 = auStack_60 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118e8 >> (3 - uVar2) * 8;
  auStack_60 = (undefined1  [4])DAT_800118e8;
  puVar1 = auStack_5c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118ec >> (3 - uVar2) * 8;
  auStack_5c = (undefined1  [4])DAT_800118ec;
  local_58 = DAT_800118f0;
  local_57 = DAT_800118f1;
  local_56 = DAT_800118f2;
  puVar1 = auStack_50 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118f4 >> (3 - uVar2) * 8;
  auStack_50 = (undefined1  [4])DAT_800118f4;
  local_4c = DAT_800118f8;
  local_4b = DAT_800118f9;
  local_4a = DAT_800118fa;
  puVar1 = auStack_48 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_800118fc >> (3 - uVar2) * 8;
  auStack_48 = (undefined1  [4])DAT_800118fc;
  puVar1 = auStack_44 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011900 >> (3 - uVar2) * 8;
  auStack_44 = (undefined1  [4])DAT_80011900;
  puVar1 = auStack_40 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011904 >> (3 - uVar2) * 8;
  auStack_40 = (undefined1  [4])DAT_80011904;
  puVar1 = auStack_3c + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011908 >> (3 - uVar2) * 8;
  auStack_3c = (undefined1  [4])DAT_80011908;
  puVar1 = auStack_38 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001190c >> (3 - uVar2) * 8;
  auStack_38 = (undefined1  [4])DAT_8001190c;
  puVar1 = auStack_34 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011910 >> (3 - uVar2) * 8;
  auStack_34 = (undefined1  [4])DAT_80011910;
  local_30 = DAT_80011914;
  puVar1 = auStack_28 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80011918 >> (3 - uVar2) * 8;
  auStack_28 = (undefined1  [4])DAT_80011918;
  puVar1 = auStack_24 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_8001191c >> (3 - uVar2) * 8;
  auStack_24 = (undefined1  [4])DAT_8001191c;
  local_20 = DAT_80011920;
  local_1f = DAT_80011921;
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    iVar3 = *(int *)(param_1 + 0x198) + 4;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_68);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_50);
    iVar3 = *(int *)(param_1 + 0x198) + 0x564;
    FUN_800171d0(iVar3,*(byte *)(param_1 + 0x104) + 2,0x34004e,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_48);
    FUN_800172f8(iVar3,*(byte *)(param_1 + 0x104) + 2,0x34004e,*(byte *)(param_1 + 0x105) + 0x1e2,
                 auStack_28);
  }
  return;
}


