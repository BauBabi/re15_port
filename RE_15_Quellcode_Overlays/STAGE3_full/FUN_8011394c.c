/* FUN_8011394c @ 0x8011394c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011394c(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  undefined1 uVar4;
  int iVar5;
  byte bVar6;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  bVar2 = false;
  bVar3 = false;
  bVar6 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar6 & 2) != 0) {
    if ((bVar6 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      goto LAB_80113c0c;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar6 & 0xfd;
    local_38 = DAT_8011ec84;
    local_34 = DAT_8011ec88;
    local_30 = DAT_8011ec8c;
    local_2c = DAT_8011ec90;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_38);
  }
  if ((((DAT_800acae7 == '\0') && (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000)) &&
      (iVar5 = func_0x8001a780(&DAT_800aca54), iVar5 != 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    uVar4 = 0xf;
  }
  else {
    bVar6 = 1;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == (&DAT_8011ed59)[bVar6]) {
        bVar2 = true;
      }
      bVar1 = bVar6 != 0;
      bVar6 = bVar6 - 1;
    } while (bVar1);
    if (((!bVar2) || (DAT_800acae7 != '\0')) ||
       ((iVar5 = func_0x8001a804(5000,0x180,&DAT_800aca88), -1 < iVar5 ||
        (*(short *)(_DAT_800ac784 + 0x1dc) != 0)))) {
      bVar6 = 2;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar6 + 0x8011ed58)) {
          bVar3 = true;
        }
        bVar2 = bVar6 != 0;
        bVar6 = bVar6 - 1;
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
      goto LAB_80113c0c;
    }
    uVar4 = 0xe;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80113c0c:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


