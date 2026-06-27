/* FUN_80119524 @ 0x80119524  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119524(void)

{
  char cVar1;
  uint uVar2;
  
  *(byte *)(_DAT_800ac784 + 0x6e) = (byte)_DAT_800ac784[0x6e] | 0x12;
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x78;
  case 1:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    if (0x46 < (short)_DAT_800ac784[0x27]) {
      FUN_8011b5c4(1,1);
      *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
      func_0x800245d8(0);
    }
    if ((short)_DAT_800ac784[0x27] == 0x46) {
      func_0x8004ef90(0x800b1028,0x1f);
    }
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x96;
  case 3:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '[') {
      func_0x800453d0(6);
      uVar2 = _DAT_800ac784[0x62];
      *(undefined4 *)(uVar2 + 0xa1c) = *(undefined4 *)(uVar2 + 0xac8);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar2 + 0xad0);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar2 + 0xacc);
      *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar2 + 0xad4);
      func_0x8004ef90(0x800b1028,0x1e);
    }
    break;
  case 4:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x46;
  case 5:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    FUN_8011b5c4(1,1);
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
    func_0x800245d8(0);
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x28;
      func_0x800453d0(10);
    }
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xd;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '<') {
      func_0x800453d0(3);
    }
  case 7:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    FUN_8011b5c4(0,1);
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 9;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
  case 9:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    if (0x11 < *(byte *)((int)_DAT_800ac784 + 0x95)) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 10;
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 2;
    }
    break;
  case 10:
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xb;
    }
    break;
  case 0xb:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x1e;
  case 0xc:
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x40);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
    FUN_8011b5c4(1,1);
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 10;
    func_0x800245d8(0);
    uVar2 = _DAT_800ac784[0x27];
    *(short *)(_DAT_800ac784 + 0x27) = (short)uVar2 + -1;
    if ((short)uVar2 == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0xd;
    }
    break;
  case 0xd:
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *(byte *)((int)_DAT_800ac784 + 9) = *(byte *)((int)_DAT_800ac784 + 9) & 0xfe;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffff7;
  }
  return;
}


