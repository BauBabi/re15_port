/* FUN_80114cb0 @ 0x80114cb0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114cb0(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    iVar2 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011a168 + iVar2);
    local_1c = *(undefined4 *)(&DAT_8011a16c + iVar2);
    local_18 = *(undefined4 *)(&DAT_8011a170 + iVar2);
    local_14 = *(undefined4 *)(&DAT_8011a174 + iVar2);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(2);
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    if (((*(byte *)(_DAT_800ac784 + 0x95) < 0xf) || (*(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
       ((*(byte *)(_DAT_800ac784 + 0x95) < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
      uVar3 = 1;
    }
    else {
      uVar3 = 0;
    }
    FUN_80115bec(0,uVar3);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x18') {
      func_0x800453d0(7);
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    break;
  case 4:
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011a188);
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  }
  return;
}


