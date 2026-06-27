/* FUN_80109f38 @ 0x80109f38  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109f38(void)

{
  char cVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  uint *puVar6;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar2 & 1) + 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar3 & 1) == 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x46;
    }
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x180),
                            *(undefined4 *)(_DAT_800ac784 + 0x184),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (*(char *)(_DAT_800ac784 + 0x95) == 'K') {
      uVar3 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar3 & 1) == 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined4 *)(iVar4 + 0x548) = 0x80;
      *(undefined4 *)(iVar4 + 0x54c) = 0x20;
      *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
      puVar6 = *(uint **)(_DAT_800ac784 + 0x188);
      puVar6[0x26] = 0x20;
      puVar6[0x25] = 0x80;
      *(undefined2 *)(puVar6 + 0x22) = 0x5f4;
      *(undefined2 *)(puVar6 + 0x23) = 0x5f4;
      *puVar6 = *puVar6 | 0x80;
      *(undefined2 *)((int)puVar6 + 0x8a) = 0xc18;
      *puVar6 = *puVar6 | 0x400;
      func_0x80019700(0x2800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar6 + 0x10,&DAT_8011d924);
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0xac) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0xac) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x2b0) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x2b0) | 0x40;
      *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x35c) =
           *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x35c) | 0x40;
    }
    break;
  case 2:
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar2 & 0x1f) + 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
  case 3:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9f);
    *(char *)(_DAT_800ac784 + 0x9f) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 3) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 9) = 0x83;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x501;
    }
  }
  return;
}


