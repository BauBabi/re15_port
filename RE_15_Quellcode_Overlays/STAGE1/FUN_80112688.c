/* FUN_80112688 @ 0x80112688  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112688(void)

{
  char cVar1;
  byte bVar2;
  uint *puVar3;
  ushort uVar4;
  uint uVar5;
  undefined4 *extraout_v1;
  int extraout_v1_00;
  ushort *puVar6;
  int iVar7;
  undefined8 uVar8;
  
  uVar5 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (uVar5 != 1) {
    if (1 < uVar5) {
      uVar8 = CONCAT44(uVar5,3);
      if (uVar5 == 2) {
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 3;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        if (*(short *)((int)_DAT_800ac784 + 0x1ba) + -0x2ee < (int)_DAT_800ac784[0xe]) {
          *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
          *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xc;
        }
        FUN_80115e24();
        uVar8 = CONCAT44(extraout_v1,*extraout_v1);
      }
      else if (uVar5 == 3) {
        *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 9;
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -9;
        _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 == '\0') {
          FUN_80115d74(8);
        }
        func_0x800245d8(0);
        return;
      }
      goto code_r0x801130f0;
    }
    uVar8 = CONCAT44(uVar5,0x8c);
    if (uVar5 != 0) goto code_r0x801130f0;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x8c;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x15;
    FUN_80115d94(5);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 3;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + 8;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar8 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
code_r0x801130f0:
  *(uint *)((ulonglong)uVar8 >> 0x20) = (uint)uVar8 | 0x40;
  cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  iVar7 = 0;
  puVar6 = (ushort *)_DAT_800ac784[0x21];
  func_0x8001f314(puVar6,_DAT_800ac784[0x5b],0,0x200);
  bVar2 = *(byte *)(extraout_v1_00 + 6);
  *(byte *)(extraout_v1_00 + 6) = bVar2 + 1;
  uVar4 = bVar2 + 2;
  *puVar6 = uVar4;
  if (*(char *)(iVar7 + 0x1db) != '\0') {
    *puVar6 = uVar4 & 0xf0ff | 0x800;
  }
  func_0x80019700(0x8032000,(int)*(short *)(iVar7 + 0x6a),iVar7 + 0x20,&DAT_8012110c);
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
  puVar3 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  *puVar3 = *puVar3 | 0x40;
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


