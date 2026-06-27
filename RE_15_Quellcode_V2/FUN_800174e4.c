void FUN_800174e4(uint param_1,uint param_2)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  
  *(char *)((int)DAT_800b52c4 + 0x6f) = *(char *)((int)DAT_800b52c4 + 0x6f) + '\x01';
  uVar4 = DAT_800b52c4[0x20];
  puVar6 = (uint *)((uint)*(byte *)((int)DAT_800b52c4 + 0x6f) * 0x28 + uVar4);
  uVar2 = ((uint)puVar6 | (uint)DAT_800b52c4) & 3;
  puVar8 = puVar6 + 8;
  puVar7 = DAT_800b52c4;
  if (uVar2 == 0) {
    do {
      uVar2 = *puVar6;
      uVar4 = puVar6[1];
      uVar5 = puVar6[2];
      uVar3 = puVar6[3];
      *puVar7 = uVar2;
      puVar7[1] = uVar4;
      puVar7[2] = uVar5;
      puVar7[3] = uVar3;
      puVar6 = puVar6 + 4;
      puVar7 = puVar7 + 4;
    } while (puVar6 != puVar8);
  }
  else {
    do {
      uVar5 = (int)puVar6 + 3U & 3;
      uVar3 = (uint)puVar6 & 3;
      uVar2 = (*(int *)(((int)puVar6 + 3U) - uVar5) << (3 - uVar5) * 8 |
              uVar2 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
              *(uint *)((int)puVar6 - uVar3) >> uVar3 * 8;
      uVar5 = (int)puVar6 + 7U & 3;
      uVar3 = (uint)(puVar6 + 1) & 3;
      uVar4 = (*(int *)(((int)puVar6 + 7U) - uVar5) << (3 - uVar5) * 8 |
              uVar4 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
              *(uint *)((int)(puVar6 + 1) - uVar3) >> uVar3 * 8;
      uVar5 = (int)puVar6 + 0xbU & 3;
      uVar3 = (uint)(puVar6 + 2) & 3;
      param_1 = (*(int *)(((int)puVar6 + 0xbU) - uVar5) << (3 - uVar5) * 8 |
                param_1 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                *(uint *)((int)(puVar6 + 2) - uVar3) >> uVar3 * 8;
      uVar5 = (int)puVar6 + 0xfU & 3;
      uVar3 = (uint)(puVar6 + 3) & 3;
      param_2 = (*(int *)(((int)puVar6 + 0xfU) - uVar5) << (3 - uVar5) * 8 |
                param_2 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                *(uint *)((int)(puVar6 + 3) - uVar3) >> uVar3 * 8;
      uVar5 = (int)puVar7 + 3U & 3;
      puVar1 = (uint *)(((int)puVar7 + 3U) - uVar5);
      *puVar1 = *puVar1 & -1 << (uVar5 + 1) * 8 | uVar2 >> (3 - uVar5) * 8;
      uVar5 = (uint)puVar7 & 3;
      *(uint *)((int)puVar7 - uVar5) =
           *(uint *)((int)puVar7 - uVar5) & 0xffffffffU >> (4 - uVar5) * 8 | uVar2 << uVar5 * 8;
      uVar5 = (int)puVar7 + 7U & 3;
      puVar1 = (uint *)(((int)puVar7 + 7U) - uVar5);
      *puVar1 = *puVar1 & -1 << (uVar5 + 1) * 8 | uVar4 >> (3 - uVar5) * 8;
      uVar5 = (uint)(puVar7 + 1) & 3;
      puVar1 = (uint *)((int)(puVar7 + 1) - uVar5);
      *puVar1 = *puVar1 & 0xffffffffU >> (4 - uVar5) * 8 | uVar4 << uVar5 * 8;
      uVar5 = (int)puVar7 + 0xbU & 3;
      puVar1 = (uint *)(((int)puVar7 + 0xbU) - uVar5);
      *puVar1 = *puVar1 & -1 << (uVar5 + 1) * 8 | param_1 >> (3 - uVar5) * 8;
      uVar5 = (uint)(puVar7 + 2) & 3;
      puVar1 = (uint *)((int)(puVar7 + 2) - uVar5);
      *puVar1 = *puVar1 & 0xffffffffU >> (4 - uVar5) * 8 | param_1 << uVar5 * 8;
      uVar5 = (int)puVar7 + 0xfU & 3;
      puVar1 = (uint *)(((int)puVar7 + 0xfU) - uVar5);
      *puVar1 = *puVar1 & -1 << (uVar5 + 1) * 8 | param_2 >> (3 - uVar5) * 8;
      uVar5 = (uint)(puVar7 + 3) & 3;
      puVar1 = (uint *)((int)(puVar7 + 3) - uVar5);
      *puVar1 = *puVar1 & 0xffffffffU >> (4 - uVar5) * 8 | param_2 << uVar5 * 8;
      puVar6 = puVar6 + 4;
      puVar7 = puVar7 + 4;
    } while (puVar6 != puVar8);
  }
  uVar5 = (int)puVar6 + 3U & 3;
  uVar3 = (uint)puVar6 & 3;
  uVar3 = (*(int *)(((int)puVar6 + 3U) - uVar5) << (3 - uVar5) * 8 |
          uVar2 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
          *(uint *)((int)puVar6 - uVar3) >> uVar3 * 8;
  uVar2 = (int)puVar6 + 7U & 3;
  uVar5 = (uint)(puVar6 + 1) & 3;
  uVar4 = (*(int *)(((int)puVar6 + 7U) - uVar2) << (3 - uVar2) * 8 |
          uVar4 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar5) * 8 |
          *(uint *)((int)(puVar6 + 1) - uVar5) >> uVar5 * 8;
  uVar2 = (int)puVar7 + 3U & 3;
  puVar6 = (uint *)(((int)puVar7 + 3U) - uVar2);
  *puVar6 = *puVar6 & -1 << (uVar2 + 1) * 8 | uVar3 >> (3 - uVar2) * 8;
  uVar2 = (uint)puVar7 & 3;
  *(uint *)((int)puVar7 - uVar2) =
       *(uint *)((int)puVar7 - uVar2) & 0xffffffffU >> (4 - uVar2) * 8 | uVar3 << uVar2 * 8;
  uVar2 = (int)puVar7 + 7U & 3;
  puVar6 = (uint *)(((int)puVar7 + 7U) - uVar2);
  *puVar6 = *puVar6 & -1 << (uVar2 + 1) * 8 | uVar4 >> (3 - uVar2) * 8;
  uVar2 = (uint)(puVar7 + 1) & 3;
  puVar7 = (uint *)((int)(puVar7 + 1) - uVar2);
  *puVar7 = *puVar7 & 0xffffffffU >> (4 - uVar2) * 8 | uVar4 << uVar2 * 8;
  return;
}
