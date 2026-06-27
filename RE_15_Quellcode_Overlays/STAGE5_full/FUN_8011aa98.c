/* FUN_8011aa98 @ 0x8011aa98  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011aa98(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x1dc) = (bVar1 & 3) + 3;
    uVar3 = func_0x8001af20();
    uVar4 = 1;
    if ((uVar3 & 1) != 0) {
      uVar4 = 3;
    }
    func_0x800453d0(uVar4);
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  return;
}


