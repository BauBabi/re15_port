/* FUN_8010e96c @ 0x8010e96c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e96c(void)

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
      FUN_8010f42c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar5 & 0xfd;
    local_38 = DAT_8011fa1c;
    local_34 = DAT_8011fa20;
    local_30 = DAT_8011fa24;
    local_2c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar4 = func_0x8001a780(&DAT_800aca54), iVar4 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_8010f42c();
    return;
  }
  bVar5 = 1;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011faf1)[bVar5]) {
      bVar2 = true;
    }
    bVar1 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar1);
  if (((bVar2) && (DAT_800acae7 == '\0')) &&
     ((iVar4 = func_0x8001a804(5000,0x180,&DAT_800aca88), iVar4 < 0 &&
      (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
    DAT_00000014 = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    return;
  }
  bVar5 = 2;
  do {
    if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar5 + 0x8011faf0)) {
      bVar3 = true;
    }
    bVar2 = bVar5 != 0;
    bVar5 = bVar5 - 1;
  } while (bVar2);
  if (((bVar3) && (4000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 1 < 0x19) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


