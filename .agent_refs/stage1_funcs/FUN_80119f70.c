/* FUN_80119f70 @ 0x80119f70  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119f70(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  byte bVar5;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  bVar2 = false;
  bVar3 = false;
  bVar5 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar5 & 2) != 0) {
    if ((bVar5 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      goto LAB_8011a1c8;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = DAT_801213a8;
    local_34 = DAT_801213ac;
    local_30 = DAT_801213b0;
    local_2c = DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8012147d)[bVar5]) {
      bVar2 = true;
    }
    bVar1 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar1);
  if ((((bVar2) && (DAT_800acae7 == '\0')) &&
      (iVar4 = func_0x8001a804(5000,0x180,&DAT_800aca88), iVar4 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  else {
    bVar5 = 2;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x8012147c)) {
        bVar3 = true;
      }
      bVar2 = bVar5 != 0;
      bVar5 = bVar5 - 1;
    } while (bVar2);
    if (!bVar3) {
      return;
    }
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0xfa1) {
      return;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 1 < 0x19) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    }
  }
LAB_8011a1c8:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


