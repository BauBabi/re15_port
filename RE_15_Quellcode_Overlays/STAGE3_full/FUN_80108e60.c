/* FUN_80108e60 @ 0x80108e60  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108e60(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  uint uVar4;
  undefined4 uVar5;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *(undefined2 *)(_DAT_800ac784 + 1) = 7;
      }
      goto LAB_80109250;
    }
    if (bVar1 != 0) goto LAB_80109250;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    uVar4 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar4 & 1) != 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
    *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
    uVar4 = (ushort)((ushort)_DAT_800ac784[0x76] >> 6) & 8;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f4)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f5)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f6)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f7)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f8)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f9)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fa)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fb)[uVar4] * 0xac + _DAT_800ac784[0x62] + 0x40)
    ;
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)((int)_DAT_800ac784 + 0x1c6);
      uVar5 = 0x800b1058;
    }
    func_0x8004ef90(uVar5,uVar2);
  }
  cVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar3;
LAB_80109250:
  if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
    FUN_80109488(0,0);
  }
  return;
}


