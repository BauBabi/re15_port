/* FUN_80116070 @ 0x80116070  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116070(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = _DAT_800ac784;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x1d4);
    *(short *)(_DAT_800ac784 + 0x1d4) = sVar2 + -1;
    if (sVar2 != 0) {
      return;
    }
    if (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      return;
    }
    if (7 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      FUN_80116bec();
    }
    *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
    bVar1 = *(byte *)(_DAT_800ac784 + 6);
  }
  else {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        FUN_80116c68();
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        return;
      }
      FUN_8011696c();
      return;
    }
    if (bVar1 != 0) {
      FUN_8011696c();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0x2c;
    FUN_80116d00();
    bVar1 = *(byte *)(_DAT_800ac784 + 6);
  }
  if (bVar1 != 0xffffffff) {
    *(short *)(uVar4 + 0x166) = (short)((((_DAT_800ac784 & 0xf) + 10) * 0x1000) / 10);
  }
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  uVar4 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
  func_0x8001af5c(0,0,uVar4 + 100,uVar4 + 200,_DAT_800ac784 + 0xb0,0x808080);
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(int *)(iVar3 + 0xc84) = iVar3 + 0xec;
  *(int *)(iVar3 + 0xca8) = iVar3 + 0xac;
  *(undefined4 *)(iVar3 + 0xc44) = 0x66;
  *(undefined4 *)(iVar3 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(iVar3 + 0xc4c) = 0;
  *(undefined2 *)(iVar3 + 0xc78) = 0;
  *(undefined2 *)(iVar3 + 0xc7a) = 0;
  *(undefined2 *)(iVar3 + 0xc7c) = 0;
  func_0x80068098(iVar3 + 0xc78,iVar3 + 0xc30);
  return;
}


