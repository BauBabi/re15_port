void data_ready_callback(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 *puVar3;
  
  puVar3 = (undefined2 *)(DAT_800bb4c8 * 0x20 + DAT_800bbda4);
  *puVar3 = 2;
  uVar1 = (int)puVar3 + 0x1fU & 3;
  uVar2 = (uint)(puVar3 + 0xe) & 3;
  DAT_8008fe20 = (*(int *)(((int)puVar3 + 0x1fU) - uVar1) << (3 - uVar1) * 8 |
                 0xffffffffU >> (uVar1 + 1) * 8 & 2) & -1 << (4 - uVar2) * 8 |
                 *(uint *)((int)(puVar3 + 0xe) - uVar2) >> uVar2 * 8;
  DAT_8008fe24 = *(undefined4 *)(puVar3 + 4);
  DAT_800bb4c8 = DAT_800bb4c4;
  if (DAT_800b24b0 != (code *)0x0) {
    (*DAT_800b24b0)();
  }
  DAT_800b5220 = 0;
  return;
}
