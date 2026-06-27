void FUN_8002c444(int param_1)

{
  byte bVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int local_40;
  undefined4 local_3c;
  int local_38;
  
  sVar2 = DAT_800b527c;
  sVar3 = DAT_800b526c;
  bVar1 = *(byte *)(param_1 + 5);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        if (*(short *)(param_1 + 0x88) == 0) {
          FUN_80045024(0x2060000,param_1 + 0x34);
        }
        sVar2 = DAT_800b527c;
        sVar3 = DAT_800b526c;
        iVar5 = (int)DAT_800b526c;
        iVar4 = (int)DAT_800b527c;
        *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + -0x1e;
        *(int *)(param_1 + 0x34) = iVar5 * 0x14 + *(int *)(param_1 + 0x34);
        *(int *)(param_1 + 0x3c) = iVar4 * 0x14 + *(int *)(param_1 + 0x3c);
        DAT_8008f698 = sVar2 * -0x1e;
        DAT_8008f69c = sVar3 * 0x1e;
        *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
      }
      else if (bVar1 == 3) {
        iVar5 = (int)DAT_800b526c;
        iVar4 = (int)DAT_800b527c;
        *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + 0x1e;
        *(int *)(param_1 + 0x34) = iVar5 * 0x14 + *(int *)(param_1 + 0x34);
        *(int *)(param_1 + 0x3c) = iVar4 * 0x14 + *(int *)(param_1 + 0x3c);
        DAT_8008f698 = sVar2 * 0x1e;
        DAT_8008f69c = sVar3 * -0x1e;
        *(undefined1 *)(param_1 + 4) = 0;
      }
      goto LAB_8002ca8c;
    }
    if (bVar1 != 0) goto LAB_8002ca8c;
    DAT_8008f680 = 0xd;
    DAT_8008f67c = 0xd;
    DAT_8008f688 = 8;
    DAT_8008f690 = 0;
    DAT_8008f684 = 0;
    DAT_8008f69a = 0;
    DAT_8008f68c = 0;
    DAT_8008f6a0 = '\0';
    *(undefined1 *)(param_1 + 5) = 1;
    *(undefined1 *)(param_1 + 6) = 0;
  }
  sVar2 = DAT_800b527c;
  sVar3 = DAT_800b526c;
  iVar4 = (int)DAT_8008f67c;
  iVar5 = (int)DAT_800b527c;
  *(int *)(param_1 + 0x34) = iVar4 * DAT_800b526c + *(int *)(param_1 + 0x34);
  *(int *)(param_1 + 0x3c) = iVar4 * iVar5 + *(int *)(param_1 + 0x3c);
  if (DAT_8008f690 < 0x400) {
    iVar4 = (int)DAT_8008f690 + (uint)DAT_8008f684;
    DAT_8008f690 = (short)iVar4;
    if (0x400 < iVar4 * 0x10000 >> 0x10) {
      DAT_8008f684 = (DAT_8008f684 + 0x400) - DAT_8008f690;
    }
    DAT_8008f698 = DAT_8008f684 * -sVar2;
    DAT_8008f69c = sVar3 * DAT_8008f684;
  }
  else {
    DAT_8008f69c = 0;
    DAT_8008f698 = 0;
  }
  DAT_8008f684 = DAT_8008f684 + DAT_8008f688;
  DAT_8008f67c = DAT_8008f67c + DAT_8008f680;
  *(char *)(param_1 + 6) = *(char *)(param_1 + 6) + '\x01';
  if (*(byte *)(param_1 + 6) < 0xd) {
    *(int *)(param_1 + 0x38) =
         (((int)DAT_8008f67c / 3) * 0x10000 >> 0x10) + *(int *)(param_1 + 0x38);
  }
  else {
    iVar4 = (int)DAT_8008f68c;
    DAT_8008f680 = 0;
    DAT_8008f68c = DAT_8008f68c + 0xa0;
    *(int *)(param_1 + 0x38) = iVar4 + *(int *)(param_1 + 0x38);
    if (*(char *)(param_1 + 6) == '\r') {
      sVar3 = FUN_8001c6e8(param_1 + 0x34,0,8,0x200);
      DAT_8008f694 = sVar3;
      *(undefined2 *)(param_1 + 0x40) = *(undefined2 *)(param_1 + 0x34);
      *(undefined2 *)(param_1 + 0x44) = *(undefined2 *)(param_1 + 0x3c);
      *(char *)(param_1 + 0x82) =
           (char)(sVar3 >> 0xf) - ((char)((int)sVar3 / 0x708) + (char)(sVar3 >> 7));
    }
    if (((DAT_8008f6a0 == '\0') && (*(short *)(param_1 + 0x88) != 0)) &&
       ((int)*(short *)(param_1 + 0x88) < *(int *)(param_1 + 0x38))) {
      DAT_8008f6a0 = '\x01';
      FUN_80045024(0x2080000,param_1 + 0x34);
      local_3c = *(undefined4 *)(param_1 + 0x38);
      local_40 = *(int *)(param_1 + 0x34) + 900;
      local_38 = *(int *)(param_1 + 0x3c) + 900;
      FUN_80019700(0x6022000,0x200,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + 0xe1;
      FUN_80019700(0x6022000,0x400,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -0xe1;
      FUN_80019700(0x6022000,0x400,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -900;
      FUN_80019700(0x6022000,0x600,&DAT_80072d4c,&local_40);
      local_40 = *(int *)(param_1 + 0x34) + 0xe1;
      local_38 = *(int *)(param_1 + 0x3c) + 900;
      FUN_80019700(0x6022000,0,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -900;
      FUN_80019700(0x6022000,0x800,&DAT_80072d4c,&local_40);
      local_40 = *(int *)(param_1 + 0x34) + -0xe1;
      local_38 = *(int *)(param_1 + 0x3c) + 900;
      FUN_80019700(0x6022000,0,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -900;
      FUN_80019700(0x6022000,0x800,&DAT_80072d4c,&local_40);
      local_40 = *(int *)(param_1 + 0x34) + -900;
      local_38 = *(int *)(param_1 + 0x3c) + 900;
      FUN_80019700(0x6022000,0xe18,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + 0xe1;
      FUN_80019700(0x6022000,0xc18,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -0xe1;
      FUN_80019700(0x6022000,0xc18,&DAT_80072d4c,&local_40);
      local_38 = *(int *)(param_1 + 0x3c) + -900;
      FUN_80019700(0x6022000,0xa00,&DAT_80072d4c,&local_40);
    }
    if ((int)DAT_8008f694 < *(int *)(param_1 + 0x38)) {
      *(int *)(param_1 + 0x38) = (int)DAT_8008f694;
      *(undefined1 *)(param_1 + 5) = 2;
    }
  }
LAB_8002ca8c:
  FUN_8002dacc(param_1,&DAT_8008f698);
  return;
}
