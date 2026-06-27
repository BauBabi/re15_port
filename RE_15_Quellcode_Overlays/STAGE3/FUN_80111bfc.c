/* FUN_80111bfc @ 0x80111bfc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111bfc(void)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
        if ((bVar1 & 2) != 0) {
          if ((bVar1 & 0x40) != 0) {
            *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 2;
            *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            *(undefined1 *)(_DAT_800ac784 + 6) = 3;
            FUN_80112754();
            return;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
          local_28 = DAT_8011ec84;
          local_24 = DAT_8011ec88;
          local_20 = DAT_8011ec8c;
          local_1c = DAT_8011ec90;
          func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                          *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 600;
        iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                                (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar3 == 0) {
          if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
            return;
          }
          if (0x31 < _DAT_800acaee) {
            return;
          }
          if (DAT_800acae7 != '\0') {
            return;
          }
          iVar3 = func_0x8001a804(5000,0xc0,&DAT_800aca88);
          if (-1 < iVar3) {
            return;
          }
          if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
            return;
          }
          *(undefined1 *)(_DAT_800ac784 + 5) = 8;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          func_0x800453d0(6);
        }
        else {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(1);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        }
        goto LAB_8011276c;
      }
      if (bVar1 != 3) goto LAB_8011276c;
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        goto LAB_8011276c;
      }
      goto LAB_80111f64;
    }
    if (bVar1 != 0) goto LAB_8011276c;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0xb4;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 200;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    iVar3 = _DAT_800ac784;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(iVar3 + 0x94) = 0xc;
LAB_8011276c:
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1c;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
LAB_80111f64:
  func_0x800245d8(0);
  return;
}


