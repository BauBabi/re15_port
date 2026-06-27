/* FUN_8011b198 @ 0x8011b198  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b198(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_8011bb8c();
      return;
    }
    if (bVar1 != 0) {
      FUN_8011bb8c();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar3 + -0x7fedec74);
    local_18 = *(undefined4 *)(iVar3 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar3 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
    return;
  }
  func_0x800453d0(2);
  FUN_8011bb8c();
  return;
}


