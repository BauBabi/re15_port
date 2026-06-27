/* FUN_8010d350 @ 0x8010d350  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d350(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  uVar4 = (ushort)bVar1;
  uVar3 = (ushort)(bVar1 < 2);
  if (bVar1 != 1) {
    iVar5 = _DAT_800ac784;
    if (bVar1 >= 2) {
      uVar3 = 2;
      if (bVar1 == 2) {
        sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
        if (sVar2 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      goto LAB_8010dc78;
    }
    if (bVar1 != 0) goto LAB_8010dc78;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  iVar5 = _DAT_800ac784;
  uVar4 = *(byte *)(_DAT_800ac784 + 6) + uVar3;
  *(char *)(_DAT_800ac784 + 6) = (char)uVar4;
LAB_8010dc78:
  *(ushort *)(iVar5 + 0x1ba) = (uVar3 + uVar4) * -8;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  FUN_8010e510();
  return;
}


