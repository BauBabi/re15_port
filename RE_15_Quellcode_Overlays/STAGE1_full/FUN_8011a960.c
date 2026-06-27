/* FUN_8011a960 @ 0x8011a960  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a960(void)

{
  char cVar1;
  undefined2 uVar2;
  ushort extraout_var;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) + 0x640;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x800453d0(7);
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 1;
  case 1:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if ((*(char *)((int)_DAT_800ac784 + 0x95) == '\x04') &&
       (iVar3 = func_0x8001a780(&DAT_800aca54), iVar3 == 0)) {
      local_20 = _DAT_80072d60;
      local_1c = _DAT_80072d64;
      local_18 = _DAT_80072d68;
      local_14 = _DAT_80072d6c;
      func_0x8001a804(3000,0x100,&DAT_800aca88);
      *(ushort *)(_DAT_800ac784 + 0x76) = extraout_var >> 0xf;
      if ((DAT_800acae7 != 0) || ((short)_DAT_800ac784[0x76] == 0)) goto LAB_8011aaf0;
LAB_8011abac:
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    }
    else {
LAB_8011aaf0:
      if (*(char *)((int)_DAT_800ac784 + 0x95) != '\x0f') {
        return;
      }
      iVar3 = func_0x8001a780(&DAT_800aca54);
      if (iVar3 != 0) {
        local_20 = _DAT_80072d60;
        local_1c = _DAT_80072d64;
        local_18 = _DAT_80072d68;
        local_14 = _DAT_80072d6c;
        uVar2 = func_0x8001bff8(_DAT_800ac784[0x62] + 0x39c,&local_20,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar2;
        if ((DAT_800acae7 == 0) && ((short)_DAT_800ac784[0x76] != 0)) goto LAB_8011abac;
      }
      if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x0f') {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
        *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x23;
      }
    }
    break;
  case 2:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    func_0x800453d0(10);
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    DAT_800aca59 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca5a = 0;
    DAT_800aca5b = 0;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 4:
    *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x16;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    func_0x800453d0(4);
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
  case 5:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '1') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 6;
    }
    break;
  case 6:
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0xff;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    break;
  case 7:
    iVar3 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],1,0x200);
    if (iVar3 != 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
      _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 6;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x1e;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0x5a;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}


