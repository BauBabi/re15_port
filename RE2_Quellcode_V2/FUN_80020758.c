/* FUN_80020758 @ 0x80020758  (Ghidra headless, raw MIPS overlay) */

void FUN_80020758(void)

{
  undefined1 *puVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18;
  undefined1 auStack_10 [4];
  undefined4 uStack_c;
  
  puVar1 = auStack_10 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 | DAT_80010910 >> (3 - uVar2) * 8;
  auStack_10 = (undefined1  [4])DAT_80010910;
  uVar2 = (int)&uStack_c + 3U & 3;
  puVar3 = (uint *)(((int)&uStack_c + 3U) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | DAT_80010914 >> (3 - uVar2) * 8;
  uStack_c = DAT_80010914;
  local_20 = (int)*(short *)(DAT_800dcbd0 + 0x34);
  local_1c = *(short *)(DAT_800dcbd0 + 0x36) + -100;
  local_18 = (int)*(short *)(DAT_800dcbd0 + 0x38);
  iVar4 = FUN_800470c0(&local_20,(int)*(short *)(DAT_800dcbd0 + 0x22),auStack_10,0x2002000a);
  if (iVar4 != 0) {
    FUN_80021970();
  }
  return;
}


