/* FUN_80103b48 @ 0x80103b48  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103b48(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100068;
  local_14 = DAT_8010006c;
  local_10 = DAT_80100070;
  local_c = DAT_80100074;
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1b;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800aca58 = (*(byte *)((int)_DAT_800ac784 + 5) + 1) * 0x100 | 5;
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
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    if ((_DAT_800aca50 & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x27) = 10;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x5a;
    _DAT_800acaee = _DAT_800acaee + -5;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    iVar3 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    if (iVar3 != 0) {
      _DAT_800acaee = _DAT_800acaee + -1;
      iVar3 = _DAT_800ac784[0x62] + 0x6b8;
      if ((byte)_DAT_800ac784[2] - 0x13 < 2) {
        iVar3 = _DAT_800ac784[0x62] + 0x764;
      }
      func_0x80019700(0x1500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),iVar3 + 0x40,&DAT_80118cfc);
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
      *(char *)((int)_DAT_800ac784 + 5) = *(char *)((int)_DAT_800ac784 + 5) + '\x02';
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    _DAT_800aca50 = _DAT_800aca50 | 1;
    func_0x800453d0(7);
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x5c],_DAT_800ac784[0x5d]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if ((DAT_800aca5c & 4) == 0) {
      if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x19') {
        *(undefined1 *)(_DAT_800ac784 + 0x6e) = 1;
        puVar5 = (uint *)((uint)(byte)(&DAT_80118cac)[(byte)_DAT_800ac784[2]] * 0xac +
                         _DAT_800ac784[0x62]);
        func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),puVar5 + 0x10,&local_18);
        puVar5[0x1a] = 0x6f;
        *(undefined2 *)(puVar5 + 0x25) = 0;
        *(undefined2 *)((int)puVar5 + 0x96) = 0;
        *(undefined2 *)(puVar5 + 0x26) = 0;
        *(undefined2 *)((int)puVar5 + 0x9a) = 0;
        *(undefined2 *)(puVar5 + 0x27) = 0;
        *(undefined2 *)((int)puVar5 + 0x9e) = 0;
        *puVar5 = *puVar5 | 0x4a;
        func_0x800453d0(9);
      }
    }
    else if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x19') {
      *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 2;
      func_0x800453d0(9);
      uVar4 = func_0x8001af20();
      if ((uVar4 & 1) == 0) {
        func_0x800453d0(8);
      }
      else {
        func_0x800453d0(5);
      }
    }
    break;
  case 6:
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 1) = 7;
  }
  return;
}


