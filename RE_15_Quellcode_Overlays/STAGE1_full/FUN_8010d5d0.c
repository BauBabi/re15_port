/* FUN_8010d5d0 @ 0x8010d5d0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d5d0(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar3 + 0xec,&LAB_8011fe84);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined4 *)(iVar3 + 0x114) = 0x8f;
    *(undefined2 *)(iVar3 + 0x142) = 0xffce;
    *(undefined2 *)(iVar3 + 0x140) = 0;
    *(undefined2 *)(iVar3 + 0x144) = 0;
    *(undefined2 *)(iVar3 + 0x146) = 3;
    *(undefined2 *)(iVar3 + 0x148) = 0;
    *(undefined2 *)(iVar3 + 0x14a) = 0;
    *(uint *)(iVar3 + 0xac) = *(uint *)(iVar3 + 0xac) | 0x4a;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  if (*(short *)(_DAT_800ac784 + 0x8c) < 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  }
  return;
}


