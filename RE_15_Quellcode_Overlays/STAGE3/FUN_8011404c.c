/* FUN_8011404c @ 0x8011404c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011404c(void)

{
  int iVar1;
  uint uVar2;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        }
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
        return;
      }
      goto LAB_80114a30;
    }
    if (uVar2 != 0) goto LAB_80114a30;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar1 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar1);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar1);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar1);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
LAB_80114a30:
  if (uVar2 != 0x3d) {
    return;
  }
  func_0x800453d0(1);
  FUN_80115270();
  return;
}


