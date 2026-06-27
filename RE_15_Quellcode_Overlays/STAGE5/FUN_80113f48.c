/* FUN_80113f48 @ 0x80113f48  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f48(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801148e0();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_801148e0();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    if ((*(short *)(_DAT_800ac784 + 0x1dc) == 100) &&
       (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x400), iVar3 == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\x05') {
      func_0x80019700(0xd002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fcc0);
      *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80115d6c(0,1);
  FUN_801148e0();
  return;
}


