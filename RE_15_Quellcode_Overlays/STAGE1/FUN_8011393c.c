/* FUN_8011393c @ 0x8011393c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011393c(undefined4 param_1,uint param_2,int param_3,undefined2 param_4)

{
  uint *puVar1;
  char cVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  undefined2 in_t0;
  uint in_t1;
  
  puVar4 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 6);
  if (puVar4 != (uint *)0x1) {
    puVar5 = _DAT_800ac784;
    if ((uint *)0x1 < puVar4) {
      if (puVar4 != (uint *)0x2) goto LAB_8011430c;
      cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(9);
      }
      goto LAB_80113aa0;
    }
    if (puVar4 != (uint *)0x0) goto LAB_8011430c;
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0xb4;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x20;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x3c;
    if ((_DAT_800ac784[0x75] & 0x80) != 0) {
      *(short *)((int)_DAT_800ac784 + 0x1de) = -*(short *)((int)_DAT_800ac784 + 0x1de);
    }
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)((int)_DAT_800ac784 + 0x6a) =
       *(short *)((int)_DAT_800ac784 + 0x6a) + *(short *)((int)_DAT_800ac784 + 0x1de);
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    puVar4 = _DAT_800ac784;
    uVar3 = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x5a;
    puVar5 = (uint *)0x0;
    while( true ) {
      *puVar4 = uVar3 | 0x4a;
      *(char *)((int)_DAT_800ac784 + 0x1d5) = (char)param_3;
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      param_2 = param_2 + 1;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      puVar1 = _DAT_800ac784;
      puVar4 = _DAT_800ac784;
      if (0xc < (param_2 & 0xffff)) break;
      puVar4 = (uint *)((param_2 & 0xffff) * 0xac + _DAT_800ac784[0x62]);
      uVar3 = *puVar4;
      puVar4[0x1a] = in_t1;
      *(undefined2 *)(puVar4 + 0x25) = 0;
      *(undefined2 *)((int)puVar4 + 0x96) = in_t0;
      *(undefined2 *)(puVar4 + 0x26) = 0;
      *(undefined2 *)((int)puVar4 + 0x9a) = param_4;
      *(undefined2 *)(puVar4 + 0x27) = 0;
      *(undefined2 *)((int)puVar4 + 0x9e) = 0;
      puVar5 = puVar1;
    }
LAB_8011430c:
    *(char *)((int)puVar4 + 7) = *(char *)((int)puVar4 + 7) + '\x01';
    func_0x80019700(puVar5,param_2,param_3 + 0x198);
    return;
  }
LAB_80113aa0:
  cVar2 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x79) = ((byte)_DAT_800ac784[0x75] & 0x3f) * (short)cVar2;
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}


