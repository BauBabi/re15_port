/* FUN_8010d188 @ 0x8010d188  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d188(void)

{
  short sVar1;
  byte bVar2;
  int iVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x93) >> 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&LAB_8011fe84);
    (*(code *)(&PTR_LAB_80120c40)[*(byte *)(_DAT_800ac784 + 5)])();
  case 1:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x400);
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    func_0x800245d8(0x800);
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(short *)(_DAT_800ac784 + 0x8c);
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x14;
    if (*(short *)(_DAT_800ac784 + 0x8c) < 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 2:
    iVar3 = (int)*(short *)(_DAT_800ac784 + 0x9c);
    if (iVar3 < 0) {
      iVar3 = iVar3 + 3;
    }
    *(short *)(_DAT_800ac784 + 0x8c) = (short)(iVar3 >> 2);
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 4;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar2 & 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    func_0x800245d8(0);
    break;
  case 4:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  }
  return;
}


