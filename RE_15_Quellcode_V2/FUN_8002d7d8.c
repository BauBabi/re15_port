undefined4 FUN_8002d7d8(int param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  int iVar5;
  int iVar6;
  short local_38;
  undefined1 auStack_35 [4];
  undefined1 uStack_31;
  int local_30 [2];
  int local_28;
  
  uVar1 = (uint)auStack_35 & 3;
  puVar4 = (uint *)(auStack_35 + -uVar1);
  *puVar4 = *puVar4 & -1 << (uVar1 + 1) * 8 | 800U >> (3 - uVar1) * 8;
  _local_38 = 800;
  uVar1 = (uint)&uStack_31 & 3;
  puVar4 = (uint *)(&uStack_31 + -uVar1);
  *puVar4 = *puVar4 & -1 << (uVar1 + 1) * 8 | 0U >> (3 - uVar1) * 8;
  stack0xffffffcc = 0;
  FUN_8004f008((int)(short)(*(short *)(param_1 + 0x6a) + 0x200U & 0xfc00),&local_38,&local_38);
  local_30[0] = (int)local_38 + *(int *)(param_1 + 0x34);
  local_28 = (int)(short)auStack_35._1_2_ + *(int *)(param_1 + 0x3c);
  uVar1 = FUN_8003b7f0(local_30,0,*(undefined1 *)(param_1 + 0x82));
  if ((uVar1 & 1) == 0) {
    iVar5 = *(byte *)(DAT_800ac778 + 2) - 1;
    if (-1 < iVar5) {
      iVar6 = iVar5 * 0x94;
      do {
        puVar4 = (uint *)((int)&DAT_800b3f98 + iVar6);
        iVar6 = iVar6 + -0x94;
        if ((*puVar4 & 1) != 0) {
          iVar2 = FUN_8002da4c(local_30,puVar4,*(undefined2 *)(*(int *)(param_1 + 0x78) + 6),
                               *(undefined1 *)(param_1 + 0x82));
          if (iVar2 != 0) goto LAB_8002da20;
          iVar2 = FUN_8002da4c(local_30,puVar4,*(undefined2 *)(*(int *)(param_1 + 0x78) + 6),
                               *(char *)(param_1 + 0x82) + -1);
          if (iVar2 != 0) {
            return 0;
          }
        }
        iVar5 = iVar5 + -1;
      } while (-1 < iVar5);
    }
    if (((uVar1 & 2) != 0) &&
       (iVar5 = FUN_8001c6e8(local_30,1,*(undefined1 *)(param_1 + 0x82),0x100),
       *(int *)(param_1 + 0x38) < iVar5)) {
      return 2;
    }
LAB_8002da20:
    uVar3 = 0;
  }
  else {
    iVar5 = *(byte *)(DAT_800ac778 + 2) - 1;
    if (-1 < iVar5) {
      iVar6 = iVar5 * 0x94;
      do {
        puVar4 = (uint *)((int)&DAT_800b3f98 + iVar6);
        if ((*puVar4 & 1) != 0) {
          iVar2 = FUN_8002da4c(local_30,puVar4,*(ushort *)(*(int *)(&DAT_800b4010 + iVar6) + 6) >> 1
                               ,*(char *)(param_1 + 0x82) + '\x01');
          if (iVar2 != 0) {
            return 0;
          }
          iVar2 = FUN_8002da4c(local_30,puVar4,*(ushort *)(*(int *)(&DAT_800b4010 + iVar6) + 6) >> 1
                               ,*(char *)(param_1 + 0x82) + -2);
          if (iVar2 != 0) {
            return 0;
          }
        }
        iVar5 = iVar5 + -1;
        iVar6 = iVar6 + -0x94;
      } while (-1 < iVar5);
    }
    uVar3 = 1;
  }
  return uVar3;
}
