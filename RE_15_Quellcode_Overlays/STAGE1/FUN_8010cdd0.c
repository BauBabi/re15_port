/* FUN_8010cdd0 @ 0x8010cdd0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cdd0(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar3 != 1) {
    if (1 < uVar3) {
      iVar2 = 7;
      if (uVar3 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8010d754;
    }
    iVar2 = 1;
    if (uVar3 != 0) goto LAB_8010d754;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    iVar2 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar2 + 0xec,&LAB_8011fe84);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined4 *)(iVar2 + 0x114) = 0x8f;
    *(undefined2 *)(iVar2 + 0x142) = 0xffce;
    *(undefined2 *)(iVar2 + 0x140) = 0;
    *(undefined2 *)(iVar2 + 0x144) = 0;
    *(undefined2 *)(iVar2 + 0x146) = 3;
    *(undefined2 *)(iVar2 + 0x148) = 0;
    *(undefined2 *)(iVar2 + 0x14a) = 0;
    *(uint *)(iVar2 + 0xac) = *(uint *)(iVar2 + 0xac) | 0x4a;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  uVar3 = _DAT_800ac784;
  iVar2 = (int)*(short *)(_DAT_800ac784 + 0x8c);
  if (-1 < iVar2) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
LAB_8010d754:
  *(char *)(iVar2 + 5) = (char)uVar3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


