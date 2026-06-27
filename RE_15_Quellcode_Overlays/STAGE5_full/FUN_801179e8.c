/* FUN_801179e8 @ 0x801179e8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801179e8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),
                        (int)*(short *)(_DAT_800ac784 + 0x1be),0x20);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
          *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
        }
        cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
        *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
        if (cVar2 == '\0') {
          *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        }
      }
      goto LAB_80117be0;
    }
    if (bVar1 != 0) goto LAB_80117be0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_8011bdd8(1,1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x14;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
  }
  iVar3 = func_0x8001a9cc(&DAT_800aca88,0x400);
  if ((iVar3 != 0) && (*(char *)(_DAT_800ac784 + 0x8f) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
  }
LAB_80117be0:
  func_0x800245d8(0);
  return;
}


