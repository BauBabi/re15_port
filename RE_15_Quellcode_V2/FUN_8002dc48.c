void FUN_8002dc48(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  short local_28;
  undefined1 auStack_25 [4];
  undefined1 uStack_21;
  int local_20 [2];
  int local_18;
  
  uVar1 = (uint)auStack_25 & 3;
  puVar4 = (uint *)(auStack_25 + -uVar1);
  *puVar4 = *puVar4 & -1 << (uVar1 + 1) * 8 | 0x2e4U >> (3 - uVar1) * 8;
  _local_28 = 0x2e4;
  uVar1 = (uint)&uStack_21 & 3;
  puVar4 = (uint *)(&uStack_21 + -uVar1);
  *puVar4 = *puVar4 & -1 << (uVar1 + 1) * 8 | 0U >> (3 - uVar1) * 8;
  stack0xffffffdc = 0;
  if ((DAT_800aca3c & 0x4000) != 0) {
    _local_28 = 0x122;
  }
  FUN_8004f008((int)*(short *)(param_1 + 0x6a),&local_28,&local_28);
  local_20[0] = (int)local_28 + *(int *)(param_1 + 0x34);
  local_18 = (int)(short)auStack_25._1_2_ + *(int *)(param_1 + 0x3c);
  if ((DAT_800aca3c & 0x4000) == 0) {
    iVar2 = *(byte *)(DAT_800ac778 + 2) - 1;
    if (-1 < iVar2) {
      puVar4 = &DAT_800b3f98 + iVar2 * 0x25;
      do {
        iVar2 = iVar2 + -1;
        if ((((*puVar4 & 1) != 0) && ((*puVar4 & 0x100) != 0)) &&
           (iVar3 = FUN_8002da4c(local_20,puVar4,0xfffffefc,*(char *)(param_1 + 0x82) + -1),
           iVar3 != 0)) {
          DAT_800ac78c = puVar4;
          *(undefined1 *)(param_1 + 5) = 0xd;
          *(undefined1 *)(param_1 + 6) = 1;
          return;
        }
        puVar4 = puVar4 + -0x25;
      } while (-1 < iVar2);
    }
  }
  else {
    iVar2 = FUN_8001c6e8(local_20,0,8,0x10100);
    if (*(int *)(param_1 + 0x38) == iVar2) {
      *(undefined1 *)(param_1 + 5) = 0xd;
      *(undefined1 *)(param_1 + 6) = 0;
    }
  }
  return;
}
