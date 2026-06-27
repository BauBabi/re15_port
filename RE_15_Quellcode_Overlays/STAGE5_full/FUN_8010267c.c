/* FUN_8010267c @ 0x8010267c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010267c(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03';
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800aca58 = (*(byte *)((int)_DAT_800ac784 + 5) - 3) * 0x100 | 5;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 1;
    func_0x800453d0(4);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03' + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x6e;
    if ((_DAT_800aca50 & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 5;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 100;
    _DAT_800acaee = _DAT_800acaee + -10;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    iVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    if (iVar3 != 0) {
      _DAT_800acaee = _DAT_800acaee + -5;
      iVar3 = _DAT_800ac784[0x62] + 0x6b8;
      if ((byte)_DAT_800ac784[2] - 0x13 < 2) {
        iVar3 = _DAT_800ac784[0x62] + 0x764;
      }
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_8011e864);
    }
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -5;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      _DAT_800aca58 = CONCAT12(4,_DAT_800aca58);
    }
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      _DAT_800ac784[1] = (*(byte *)((int)_DAT_800ac784 + 5) + 2) * 0x100 | 1;
    }
    break;
  case 4:
    *(char *)(_DAT_800ac784 + 0x25) = (*(char *)((int)_DAT_800ac784 + 5) + -3) * '\x03' + '\x02';
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(7);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    _DAT_800aca50 = _DAT_800aca50 | 1;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xfffe;
    func_0x800453d0(7);
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x18') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    }
    if (((((*(byte *)((int)_DAT_800ac784 + 0x1c2) & 2) != 0) &&
         (7 < *(byte *)((int)_DAT_800ac784 + 0x95))) &&
        (uVar4 = _DAT_800ac784[0x6b], -1 < *(short *)(uVar4 + 0x9a))) &&
       (((*(byte *)(uVar4 + 9) & 0x80) == 0 && ((*(ushort *)(uVar4 + 0x1d8) & 1) == 0)))) {
      *(undefined4 *)(uVar4 + 4) = 0xb01;
    }
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x11;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x32;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 2;
  case 7:
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],1,0x100);
    *(char *)((int)_DAT_800ac784 + 0x9f) = *(char *)((int)_DAT_800ac784 + 0x9f) - cVar1;
    if (*(char *)((int)_DAT_800ac784 + 0x9f) == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 8;
    }
    func_0x800245d8(0x800);
    *(short *)(_DAT_800ac784 + 0x23) =
         (short)_DAT_800ac784[0x23] - (short)*(char *)((int)_DAT_800ac784 + 0x9e);
    if ((short)_DAT_800ac784[0x23] < 10) {
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 10;
    }
    break;
  case 8:
    _DAT_800ac784[1] = 0x201;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  }
  return;
}


