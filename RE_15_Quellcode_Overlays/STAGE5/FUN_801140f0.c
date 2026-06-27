/* FUN_801140f0 @ 0x801140f0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801140f0(void)

{
  bool bVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  byte bVar5;
  
  if (((((short)_DAT_800ac784[0x77] == 0) ||
       ((&LAB_8011fd10)[*(byte *)((int)_DAT_800ac784 + 5)] == '\0')) ||
      (*(char *)((int)_DAT_800ac784 + 6) != '\x01')) ||
     ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0)) {
    (**(code **)(&LAB_8011fd28 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
    return;
  }
  _DAT_800ac784[1] = 0x601;
  puVar3 = (uint *)&DAT_0000000b;
  func_0x800453d0();
  puVar4 = _DAT_800ac784;
  *(short *)((int)_DAT_800ac784 + 0x9a) = (short)_DAT_800ac784[0x77];
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  if (puVar4 != (uint *)0x1) {
    if (1 < (int)puVar4) {
      if (puVar4 != (uint *)0x2) {
        return;
      }
      goto LAB_80114b4c;
    }
    if (puVar4 != (uint *)0x0) {
      return;
    }
    *puVar3 = *puVar3 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    puVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 0;
    *(undefined2 *)((int)puVar4 + 0xbe) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    bVar5 = 0x19;
    do {
      puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar5 != 0;
      *puVar4 = *puVar4 & 0xfffffffe;
      bVar5 = bVar5 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 9) != 'B') {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_80114b4c:
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
    bVar5 = 0x19;
    do {
      puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
      bVar1 = bVar5 != 0;
      *puVar4 = *puVar4 | 1;
      bVar5 = bVar5 - 1;
    } while (bVar1);
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '2') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
    puVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
    *(undefined2 *)((int)puVar4 + 0xbe) = 0x44c;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  iVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  if (iVar2 != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  return;
}


