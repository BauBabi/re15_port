/* FUN_80100424 @ 0x80100424  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100424(void)

{
  byte bVar1;
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c(0,0,600,700);
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar1 == 4) || (bVar1 == 7)) || (bVar1 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 5) || (bVar1 == 8)) || (bVar1 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if ((bVar1 == 1) || (bVar1 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 4) || (bVar1 == 7)) || (bVar1 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar1 == 5) || (bVar1 == 8)) || (bVar1 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    }
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 6) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x20c01;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xb) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xd) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xe) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x2a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  func_0x8001ee68(_DAT_800ac784);
  return;
}


