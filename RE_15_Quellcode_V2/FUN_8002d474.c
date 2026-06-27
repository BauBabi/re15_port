undefined4 FUN_8002d474(void)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  undefined *puVar4;
  uint *puVar5;
  int iVar6;
  int iVar7;
  undefined8 uVar8;
  short local_38;
  undefined1 auStack_35 [4];
  undefined1 uStack_31;
  int local_30 [2];
  int local_28;
  
  uVar1 = (uint)auStack_35 & 3;
  puVar5 = (uint *)(auStack_35 + -uVar1);
  *puVar5 = *puVar5 & -1 << (uVar1 + 1) * 8 | 800U >> (3 - uVar1) * 8;
  _local_38 = 800;
  uVar1 = (uint)&uStack_31 & 3;
  puVar5 = (uint *)(&uStack_31 + -uVar1);
  *puVar5 = *puVar5 & -1 << (uVar1 + 1) * 8 | 0U >> (3 - uVar1) * 8;
  stack0xffffffcc = 0;
  FUN_8004f008((int)(short)(DAT_800acabe + 0x200U & 0xfc00),&local_38,&local_38);
  local_30[0] = local_38 + DAT_800aca88;
  local_28 = (short)auStack_35._1_2_ + DAT_800aca90;
  bVar2 = FUN_8003b7f0(local_30,0,DAT_800acad6);
  uVar8 = FUN_8002d2c0(local_30);
  puVar4 = (undefined *)((ulonglong)uVar8 >> 0x20);
  if (((int)uVar8 != 0) || ((bVar2 & 1) != 0)) {
    iVar6 = *(byte *)(DAT_800ac778 + 2) - 1;
    if (-1 < iVar6) {
      iVar7 = iVar6 * 0x94;
      do {
        puVar5 = (uint *)((int)&DAT_800b3f98 + iVar7);
        iVar7 = iVar7 + -0x94;
        if ((*puVar5 & 1) != 0) {
          iVar3 = FUN_8002da4c(local_30,puVar5,200,DAT_800acad6 + '\x01');
          if (iVar3 != 0) {
            return 0;
          }
          iVar3 = FUN_8002da4c(local_30,puVar5,200,DAT_800acad6 + -2);
          if (iVar3 != 0) {
            return 0;
          }
        }
        iVar6 = iVar6 + -1;
      } while (-1 < iVar6);
      puVar4 = &UNK_800b0000;
    }
    DAT_800aca59 = 9;
    if ((int)uVar8 != 0) {
      puVar4[-0x35a6] = 0x81;
    }
    return 1;
  }
  iVar6 = *(byte *)(DAT_800ac778 + 2) - 1;
  if (-1 < iVar6) {
    iVar7 = iVar6 * 0x94;
    do {
      puVar5 = (uint *)((int)&DAT_800b3f98 + iVar7);
      iVar7 = iVar7 + -0x94;
      if ((*puVar5 & 1) != 0) {
        iVar3 = FUN_8002da4c(local_30,puVar5,0x1c2,DAT_800acad6 + -1);
        if (iVar3 != 0) {
          return 0;
        }
        if ((puVar5 != DAT_800ac788) &&
           (iVar3 = FUN_8002da4c(local_30,puVar5,0x1c2,DAT_800acad6), iVar3 != 0)) {
          return 0;
        }
      }
      iVar6 = iVar6 + -1;
    } while (-1 < iVar6);
  }
  if (((DAT_800aca3c & 0x4000) == 0) ||
     (iVar6 = FUN_8002d1e8(&DAT_800aca54,DAT_800ac788,1), iVar6 != 0)) {
    if ((bVar2 & 2) != 0) {
      iVar6 = FUN_8001c6e8(local_30,1,DAT_800acad6,0x100);
      if (DAT_800aca8c < iVar6) {
        DAT_800aca59 = 9;
        DAT_800aca5a = bVar2 & 0xe;
        DAT_800aca5b = 0;
        return 1;
      }
    }
  }
  else {
    iVar6 = FUN_8001c6e8(local_30,1,8,0x100);
    if (DAT_800aca8c < iVar6) {
      DAT_800aca59 = 9;
      DAT_800aca5a = 2;
      DAT_800aca5b = 0;
      return 1;
    }
  }
  return 0;
}
