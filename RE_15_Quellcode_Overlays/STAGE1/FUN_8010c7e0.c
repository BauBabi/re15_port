/* FUN_8010c7e0 @ 0x8010c7e0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7e0(void)

{
  byte bVar1;
  short sVar2;
  undefined1 uVar3;
  ushort uVar4;
  undefined2 uVar5;
  int iVar6;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    iVar6 = _DAT_800ac784;
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        return;
      }
      goto code_r0x8010d0a4;
    }
    if (bVar1 != 0) goto code_r0x8010d0a4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0xff) + 0x1e;
  }
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  iVar6 = 0;
code_r0x8010d0a4:
  uVar5 = func_0x8003b0a4(iVar6 + 0x34,*(undefined2 *)(*(int *)(iVar6 + 0x78) + 6),4);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
  uVar3 = 1;
  if ((*(byte *)(_DAT_800ac784 + 4) < 2) && (uVar3 = 0x10, 1 < *(byte *)(_DAT_800ac784 + 5) - 1)) {
    uVar3 = 0x12;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = uVar3;
  func_0x8001b4e4();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


