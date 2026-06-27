/* FUN_80113b1c @ 0x80113b1c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113b1c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  uint *puVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  ushort *puVar6;
  
  puVar6 = (ushort *)(uint)*(byte *)((int)_DAT_800ac784 + 6);
  sVar3 = 1;
  if (puVar6 == (ushort *)0x0) {
    puVar6 = (ushort *)&DAT_00000004;
    FUN_80115d94();
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0xb4;
    *(char *)((int)_DAT_800ac784 + 0x1d6) = *(char *)((int)_DAT_800ac784 + 0x1d6) + '\x01';
    *(ushort *)(_DAT_800ac784 + 0x79) = (byte)_DAT_800ac784[0x75] & 0x3f;
    if ((short)_DAT_800ac784[0x7b] < 2000) {
      *(short *)(_DAT_800ac784 + 0x79) = -(short)_DAT_800ac784[0x79];
    }
    sVar3 = *(byte *)((int)_DAT_800ac784 + 6) + 1;
    *(char *)((int)_DAT_800ac784 + 6) = (char)sVar3;
  }
  else {
    param_3 = 0;
    if (puVar6 == (ushort *)0x1) {
      iVar4 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      if (iVar4 != 0) {
        *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + -1;
      }
      _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
      func_0x8001a8f8(&DAT_800aca88,0x32);
      if ((char)_DAT_800ac784[0x74] != '\0') {
        FUN_80115d74(0x10);
      }
      func_0x800245d8(0);
      return;
    }
  }
  *puVar6 = sVar3 + 1U;
  if (*(char *)(param_3 + 0x1db) != '\0') {
    *puVar6 = sVar3 + 1U & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(param_3 + 0x6a),param_3 + 0x20,&DAT_8012110c);
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar5 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar5) {
    return;
  }
  _DAT_800ac784[0xe] = uVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar2 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar2 = *puVar2 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_801157b4();
  return;
}


