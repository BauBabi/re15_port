/* FUN_8010c470 @ 0x8010c470  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c470(undefined4 param_1,int param_2)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    iVar5 = _DAT_800ac784;
    if (1 < bVar1) {
      iVar4 = 2;
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
        return;
      }
      goto LAB_8010cd60;
    }
    iVar4 = 1;
    if (bVar1 != 0) goto LAB_8010cd60;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  param_2 = *(int *)(_DAT_800ac784 + 0x16c);
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),param_2,0,0x200);
  iVar5 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar4;
LAB_8010cd60:
  if (iVar4 < 0x400) {
    *(char *)(param_2 + 0x9f) = (char)iVar5;
    *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    iVar5 = _DAT_800ac784;
    sVar3 = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  else {
    sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
    *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
    if ((sVar3 == 0) ||
       ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
        (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    iVar5 = _DAT_800ac784;
    sVar3 = *(short *)(_DAT_800ac784 + 0x1da);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar3;
  }
  *(short *)(iVar5 + 0x6a) = sVar3 + 0x800;
  *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
  if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}


