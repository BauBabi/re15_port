/* FUN_80115070 @ 0x80115070  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115070(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,0);
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011ec64 + iVar4);
    local_1c = *(undefined4 *)(&DAT_8011ec68 + iVar4);
    local_18 = *(undefined4 *)(&DAT_8011ec6c + iVar4);
    local_14 = *(undefined4 *)(&DAT_8011ec70 + iVar4);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1058;
    }
    func_0x8004ef90(uVar5,uVar2);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  FUN_80115b68(0,0);
  if (*(char *)(_DAT_800ac784 + 0x95) == '=') {
    func_0x800453d0(1);
  }
  return;
}


