/* FUN_801086a8 @ 0x801086a8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801086a8(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80109298();
        return;
      }
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *(undefined2 *)(_DAT_800ac784 + 1) = 7;
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        FUN_801094d0(0,0);
      }
      return;
    }
    if (bVar1 != 0) {
      FUN_80109298();
      return;
    }
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    uVar3 = func_0x8001af20();
    uVar4 = 8;
    if ((uVar3 & 1) != 0) {
      uVar4 = 5;
    }
    func_0x800453d0(uVar4);
    *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
    uVar3 = (ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_1)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_2)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011e834_3)[uVar3] * 0xac + _DAT_800ac784[0x62] +
                             0x40);
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e838)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e839)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83a)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011e83b)[uVar3] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109234(0x800b1038,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)((int)_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  FUN_80109298();
  return;
}


