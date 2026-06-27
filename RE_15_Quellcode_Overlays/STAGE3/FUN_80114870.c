/* FUN_80114870 @ 0x80114870  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114870(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  int extraout_v1;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      FUN_80115270();
      return;
    }
    if (bVar1 != 0) {
      FUN_80115270();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&LAB_8011ec64 + iVar3);
    local_1c = *(undefined4 *)(&LAB_8011ec68 + iVar3);
    local_18 = *(undefined4 *)(&LAB_8011ec6c + iVar3);
    local_14 = *(undefined4 *)(&LAB_8011ec70 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      *(char *)(extraout_v1 + 0x95) = (char)_DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80115b68(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) != '=') {
    return;
  }
  func_0x800453d0(1);
  FUN_80115270();
  return;
}


