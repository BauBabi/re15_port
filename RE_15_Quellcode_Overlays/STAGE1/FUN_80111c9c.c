/* FUN_80111c9c @ 0x80111c9c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c9c(void)

{
  char cVar1;
  undefined2 in_v0;
  
  *(undefined2 *)(_DAT_800ac784 + 0x77) = in_v0;
  *(short *)(_DAT_800ac784 + 0x7b) = _DAT_800aca8c - (short)_DAT_800ac784[0xe];
  if ((_DAT_800aca50 & 0xff00) != 0) {
    FUN_80116068();
  }
  (**(code **)(&DAT_8012113c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121184 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((short)_DAT_800ac784[0x7b] < 0xfa1) {
    if (799 < (short)_DAT_800ac784[0x7b]) {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
      FUN_80115f70();
      if ((_DAT_800aca50 & 0x800) != 0) {
        if (*(char *)((int)_DAT_800ac784 + 0x1db) == '\0') {
          _DAT_800aca50 = _DAT_800aca50 & 0xf0ff;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1db) = 1;
        }
      }
      return;
    }
    func_0x80012974(6000);
  }
  else {
    func_0x80012a0c(6000);
  }
  func_0x8001a8f8();
  cVar1 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x79) = ((byte)_DAT_800ac784[0x75] & 0x3f) * (short)cVar1;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}


