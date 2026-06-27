/* FUN_8010da9c @ 0x8010da9c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010da9c(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1b')) {
    func_0x800453d0(6);
  }
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
  }
  uVar3 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar3 & 0xf) + 1;
  func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80110350(0,*(byte *)(_DAT_800ac784 + 0x95) < 0xd);
  func_0x800245d8(0x800);
  return;
}


