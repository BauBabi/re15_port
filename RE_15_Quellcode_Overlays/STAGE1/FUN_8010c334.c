/* FUN_8010c334 @ 0x8010c334  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c334(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_8010c47c:
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x9c) + (int)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x28e) = (short)*(char *)(iVar5 + 0x27a) + *(short *)(iVar5 + 0x28e);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    uVar3 = *(ushort *)(iVar5 + 0x1e2);
    sVar4 = *(short *)(iVar5 + 0x1da);
    *(ushort *)(iVar5 + 0x1e2) = (short)*(char *)(iVar5 + 0x1ce) + uVar3;
    *(short *)(iVar5 + 0x1da) = sVar4 + -1;
    if (sVar4 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      iVar5 = *(int *)(uVar3 - 0x387c);
      *(char *)(iVar5 + 7) = *(char *)(iVar5 + 7) + '\x02';
      FUN_8010d5c0();
      return;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) goto LAB_8010ce34;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      bVar1 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 5;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x1e0) = 0x1000;
      *(undefined2 *)(iVar5 + 0x1e2) = 0x1000;
      *(undefined2 *)(iVar5 + 0x1e4) = 0x1000;
      *(uint *)(iVar5 + 0x158) = *(uint *)(iVar5 + 0x158) | 0x400;
      uVar3 = func_0x8001af20();
      sVar4 = (uVar3 & 0xff) + 100;
      *(short *)(iVar5 + 400) = sVar4;
      *(short *)(iVar5 + 0x1da) = sVar4;
      bVar1 = func_0x8001af20();
      *(byte *)(iVar5 + 0x1ce) = (bVar1 & 7) + 5;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x28c) = 0x1000;
      *(undefined2 *)(iVar5 + 0x28e) = 0x1000;
      *(undefined2 *)(iVar5 + 0x290) = 0x1000;
      *(uint *)(iVar5 + 0x204) = *(uint *)(iVar5 + 0x204) | 0x400;
      bVar1 = func_0x8001af20();
      *(byte *)(iVar5 + 0x27a) = bVar1 & 7;
      goto LAB_8010c47c;
    }
    if (bVar1 != 2) {
      if (bVar1 == 3) {
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) & 0xfffffbff;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) & 0xfffffbff;
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        uVar3 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar3 & 0x1f) + 0x1e;
        return;
      }
      goto LAB_8010ce34;
    }
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x28e) = *(short *)(iVar5 + 0x28e) + *(char *)(iVar5 + 0x27a) * -2;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(short *)(iVar5 + 0x1e2) = *(short *)(iVar5 + 0x1e2) + *(char *)(iVar5 + 0x1ce) * -2;
    if (*(short *)(iVar5 + 400) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    }
    *(short *)(iVar5 + 400) = *(short *)(iVar5 + 400) + -2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
LAB_8010ce34:
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),iVar5 + 0xec,&LAB_8011fe84);
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
  *(undefined4 *)(iVar5 + 0x114) = 0x8f;
  *(undefined2 *)(iVar5 + 0x142) = 0xffce;
  *(undefined2 *)(iVar5 + 0x140) = 0;
  *(undefined2 *)(iVar5 + 0x144) = 0;
  *(undefined2 *)(iVar5 + 0x146) = 3;
  *(undefined2 *)(iVar5 + 0x148) = 0;
  *(undefined2 *)(iVar5 + 0x14a) = 0;
  *(uint *)(iVar5 + 0xac) = *(uint *)(iVar5 + 0xac) | 0x4a;
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  func_0x800245d8(0x800);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x1e;
  iVar5 = _DAT_800ac784;
  sVar4 = *(short *)(_DAT_800ac784 + 0x8c);
  if (-1 < sVar4) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(char *)(sVar4 + 5) = (char)iVar5;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


