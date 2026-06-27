/* FUN_8010e89c @ 0x8010e89c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e89c(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      }
      else if (bVar1 != 3) {
        FUN_8010f3b0();
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0xe;
        uVar5 = (*(ushort *)(_DAT_800ac784 + 0x1ea) & 0xc) >> 2;
        do {
          *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + 0x708;
          uVar4 = uVar5 & 0xff;
          *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
          uVar5 = uVar5 - 1;
        } while (uVar4 != 0);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x509;
        func_0x800245d8(0);
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3f8;
        *(short *)(_DAT_800ac784 + 0x9c) =
             *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
      }
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0xe < 6) {
        iVar3 = (int)*(short *)(_DAT_800ac784 + 0x9c);
        if (iVar3 < 0) {
          iVar3 = iVar3 + 7;
        }
        *(int *)(_DAT_800ac784 + 0x38) = (iVar3 >> 3) + *(int *)(_DAT_800ac784 + 0x38);
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
        func_0x800245d8(0);
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
        *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      return;
    }
    if (bVar1 != 0) {
      FUN_8010f3b0();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar2 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  FUN_8010f3b0();
  return;
}


