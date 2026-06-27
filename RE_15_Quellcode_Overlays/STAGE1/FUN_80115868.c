/* FUN_80115868 @ 0x80115868  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115868(void)

{
  bool bVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)_DAT_800aca50;
  uVar3 = uVar5 & 0x4000;
  if (((_DAT_800aca50 & 0x8000) != 0) &&
     (uVar3 = uVar5 & 0x4000, *(byte *)(_DAT_800ac784 + 9) < 0x80)) {
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
      bVar1 = _DAT_800ac784 != 0;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff;
      *(short *)(uVar5 + 0x1d4) = (short)_DAT_800ac784 + -1;
      if ((bVar1) || (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0))) {
        return;
      }
      if (7 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
        FUN_80116bec();
      }
      *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
      uVar2 = _DAT_800aca50;
      goto code_r0x80116148;
    }
    if (4 < *(byte *)(_DAT_800ac784 + 5)) {
      FUN_80115d74(0xf);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    uVar5 = (uint)_DAT_800aca50;
    uVar3 = uVar5 & 0x4000;
  }
  if (uVar3 != 0) {
    uVar2 = (ushort)uVar5 & 0xfff;
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
code_r0x80116148:
      _DAT_800aca50 = uVar2;
      if (*(byte *)(_DAT_800ac784 + 6) != 0xffffffff) {
        *(short *)(uVar5 + 0x166) = (short)((((_DAT_800ac784 & 0xf) + 10) * 0x1000) / 10);
      }
      *(undefined4 *)(_DAT_800ac784 + 0x78) =
           *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
      uVar3 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
      func_0x8001af5c(0,0,uVar3 + 100,uVar3 + 200,_DAT_800ac784 + 0xb0,0x808080);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(int *)(iVar4 + 0xc84) = iVar4 + 0xec;
      *(int *)(iVar4 + 0xca8) = iVar4 + 0xac;
      *(undefined4 *)(iVar4 + 0xc44) = 0x66;
      *(undefined4 *)(iVar4 + 0xc48) = 0xfffffcd6;
      *(undefined4 *)(iVar4 + 0xc4c) = 0;
      *(undefined2 *)(iVar4 + 0xc78) = 0;
      *(undefined2 *)(iVar4 + 0xc7a) = 0;
      *(undefined2 *)(iVar4 + 0xc7c) = 0;
      func_0x80068098(iVar4 + 0xc78,iVar4 + 0xc30);
      return;
    }
    if (3 < *(byte *)(_DAT_800ac784 + 5)) {
      FUN_80115d74(0xe);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
  }
  if ((_DAT_800aca50 & 0x2000) != 0) {
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x1000;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    if ((3 < *(byte *)(_DAT_800ac784 + 5)) && (*(char *)(_DAT_800ac784 + 9) != -0x80)) {
      FUN_80115d74(0x11);
    }
  }
  return;
}


