/* FUN_80112ffc @ 0x80112ffc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112ffc(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  uint uVar4;
  uint extraout_v1;
  uint *puVar5;
  
  uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    puVar5 = _DAT_800ac784;
    if (1 < uVar4) {
      if (uVar4 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(5);
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
        }
        *(ushort *)((int)_DAT_800ac784 + 0x1de) = *(ushort *)((int)_DAT_800ac784 + 0x1de) + 2;
        func_0x8001a8f8(&DAT_800aca88,*(ushort *)((int)_DAT_800ac784 + 0x1de));
        *(ushort *)(_DAT_800ac784 + 0x23) =
             (ushort)_DAT_800ac784[0x23] + *(ushort *)((int)_DAT_800ac784 + 0x1e6);
        *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
        if ((((ushort)_DAT_800ac784[0x77] < 600) && ((ushort)_DAT_800ac784[0x7b] - 1 < 0xe0f)) &&
           (DAT_800acae7 == '\0')) {
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 6;
          *(ushort *)((int)_DAT_800ac784 + 0x1e6) = 0xffec;
          func_0x800453d0(4);
          DAT_800aca58 = 2;
          _DAT_800acaee = _DAT_800acaee + -4;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          _DAT_800aca5a = 0;
          if (_DAT_800acaee < 0) {
            DAT_800aca58 = 3;
            _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
            DAT_800aca59 = '\0';
            _DAT_800aca5a = 0;
            *(undefined1 *)(_DAT_800ac784 + 0x76) = 1;
          }
          FUN_801161e8();
        }
        if (*(byte *)((int)_DAT_800ac784 + 0x1d7) != 0) {
          func_0x80019700((uint)*(byte *)((int)_DAT_800ac784 + 0x1d7) << 0xb,
                          (int)(short)*(ushort *)((int)_DAT_800ac784 + 0x6a),
                          _DAT_800ac784[0x62] + 0x198,&DAT_8012110c);
          *(char *)((int)_DAT_800ac784 + 0x1d7) = *(char *)((int)_DAT_800ac784 + 0x1d7) + -1;
        }
        _DAT_800ac784[0xe] = (int)(short)(ushort)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        func_0x800245d8(0);
        return;
      }
      goto code_r0x80113bd8;
    }
    if (uVar4 != 0) goto code_r0x80113bd8;
    FUN_80115d94(4);
    *(ushort *)(_DAT_800ac784 + 0x23) = 0x50;
    *(ushort *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(ushort *)(_DAT_800ac784 + 0x79) = 200;
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 9;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(ushort *)((int)_DAT_800ac784 + 0x1de));
  *(ushort *)(_DAT_800ac784 + 0x23) = (ushort)_DAT_800ac784[0x23] + 0x14;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
  param_3 = 0;
  puVar5 = (uint *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar5,_DAT_800ac784[0x5b],0,0x200);
  uVar4 = extraout_v1;
code_r0x80113bd8:
  bVar1 = *(byte *)(uVar4 + 6);
  *(byte *)(uVar4 + 6) = bVar1 + 1;
  uVar3 = bVar1 + 2;
  *(ushort *)puVar5 = uVar3;
  if (*(char *)(param_3 + 0x1db) != '\0') {
    *(ushort *)puVar5 = uVar3 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(param_3 + 0x6a),param_3 + 0x20,&DAT_8012110c);
  *(ushort *)(_DAT_800ac784 + 0x1b) = (ushort)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)(ushort)_DAT_800ac784[0x1b]) {
    *(ushort *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(ushort *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] + (ushort)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)(ushort)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar4 = (int)(short)*(ushort *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar4) {
    return;
  }
  _DAT_800ac784[0xe] = uVar4;
  *(ushort *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar5 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar5 = *puVar5 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(ushort *)(_DAT_800ac784 + 0x23) = (ushort)_DAT_800ac784[0x23] + 0x14;
  *(ushort *)(_DAT_800ac784 + 0x79) = (ushort)_DAT_800ac784[0x79] - 7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}


