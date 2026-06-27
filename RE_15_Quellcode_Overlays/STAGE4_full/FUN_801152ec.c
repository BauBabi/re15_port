/* FUN_801152ec @ 0x801152ec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801152ec(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  byte bVar4;
  
  bVar4 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar4 != 1) {
    if (1 < bVar4) {
      if (bVar4 != 2) {
        return;
      }
      goto LAB_8011548c;
    }
    if (bVar4 != 0) {
      return;
    }
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 0;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 & 0xfffffffe;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 9) != 'B') {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_8011548c:
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
    bVar4 = 0x19;
    do {
      puVar3 = (uint *)((uint)bVar4 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar4 != 0;
      *puVar3 = *puVar3 | 1;
      bVar4 = bVar4 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '!') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar3 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar3 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar2 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}


