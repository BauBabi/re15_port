/* FUN_80104e50 @ 0x80104e50  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104e50(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 uVar6;
  short sVar7;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 7) + 8;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar5 = func_0x8001af20();
    if ((uVar5 & 3) == 0) {
      uVar5 = func_0x8001af20();
      uVar6 = 5;
      if ((uVar5 & 1) != 0) {
        uVar6 = 4;
      }
      func_0x800453d0(uVar6);
    }
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar2 & 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  }
  sVar7 = (short)*(char *)(_DAT_800ac784 + 0x9e) *
          *(short *)(&LAB_80117c54_2 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                  (int)sVar7);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    cVar3 = *(char *)(_DAT_800ac784 + 0x9f) + '\x01';
    *(char *)(_DAT_800ac784 + 0x9f) = cVar3;
    if ('\x0f' < cVar3) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) =
         *(undefined2 *)(&LAB_80117c54 + *(char *)(_DAT_800ac784 + 0x9f) * 4);
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + (sVar7 - (uVar4 & 1) * (short)((int)sVar7 << 1));
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80105898(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}


