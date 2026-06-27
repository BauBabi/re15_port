/* FUN_8011089c @ 0x8011089c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011089c(void)

{
  char cVar1;
  ushort uVar2;
  int in_v0;
  int iVar3;
  uint uVar4;
  
  if (in_v0 == 0) {
    cVar1 = func_0x8001af20();
    if ((cVar1 == '\0') && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 6;
    }
    if (((*(short *)(_DAT_800ac784 + 0x1d4) < 7000) &&
        (iVar3 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar3 == 0)) &&
       (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 6) = (uVar2 & 1) + 6;
    }
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar4 = func_0x8001af20(), (uVar4 & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if ((DAT_800acae7 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
  }
  return;
}


