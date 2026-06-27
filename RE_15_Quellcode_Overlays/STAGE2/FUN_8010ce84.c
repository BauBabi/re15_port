/* FUN_8010ce84 @ 0x8010ce84  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ce84(void)

{
  char cVar1;
  byte bVar2;
  ushort uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar3 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 0x3c;
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(char *)(_DAT_800ac784 + 0x1e0) == '\x02') &&
     (cVar1 = *(char *)(_DAT_800ac784 + 0x1e1), *(char *)(_DAT_800ac784 + 0x1e1) = cVar1 + -1,
     cVar1 == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
    if (*(int *)(_DAT_800ac784 + 0x38) < -0x4b0) {
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    }
  }
  func_0x800245d8(0);
  return;
}


