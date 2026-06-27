/* FUN_80104780 @ 0x80104780  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104780(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  short unaff_s0;
  short unaff_s1;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 9) = 0;
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 4;
        if (*(char *)(_DAT_800ac784 + 0x9f) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
          *(undefined4 *)(_DAT_800ac784 + 4) = 1;
          *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
        }
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        return;
      }
      goto LAB_80105114;
    }
    if (bVar2 != 0) goto LAB_80105114;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 1;
    }
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),
                          (int)*(char *)(_DAT_800ac784 + 0x9f),0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
LAB_80105114:
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_8011fb50 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (unaff_s1 - (uVar4 & 1) * unaff_s0 * 2);
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_8010939c(0,0);
    return;
  }
  FUN_80105a04(0,1);
  return;
}


