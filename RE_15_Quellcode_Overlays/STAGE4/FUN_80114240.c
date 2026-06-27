/* FUN_80114240 @ 0x80114240  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114240(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  uint *puVar4;
  byte bVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar5 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar5 != 1) {
    puVar4 = _DAT_800ac784;
    if (1 < bVar5) {
      if (bVar5 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
        _DAT_800ac784[1] = 0x701;
        *(undefined2 *)(_DAT_800ac784 + 0x77) = 100;
        return;
      }
      goto LAB_80114c58;
    }
    if (bVar5 != 0) goto LAB_80114c58;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 8;
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 9;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)((int)_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar3 + -0x7fee5e98);
    local_1c = *(undefined4 *)(iVar3 + -0x7fee5e94);
    local_18 = *(undefined4 *)(iVar3 + -0x7fee5e90);
    local_14 = *(undefined4 *)(iVar3 + -0x7fee5e8c);
    func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  if (((*(byte *)((int)_DAT_800ac784 + 0x95) < 0xf) || ((char)_DAT_800ac784[0x25] != '\b')) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x95) < 0x11 || ((char)_DAT_800ac784[0x25] != '\t')))) {
    puVar4 = (uint *)0x0;
    FUN_80115bec(0,1);
  }
  else {
    puVar4 = (uint *)0x0;
    FUN_80115bec(0,0);
  }
LAB_80114c58:
  if (*(char *)((int)puVar4 + 9) == 'B') {
    *(undefined1 *)((int)puVar4 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x06') {
      bVar5 = 0x19;
      do {
        puVar4 = (uint *)((uint)bVar5 * 0xac + _DAT_800ac784[0x62]);
        bVar1 = bVar5 != 0;
        *puVar4 = *puVar4 | 1;
        bVar5 = bVar5 - 1;
      } while (bVar1);
    }
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '!') {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      puVar4 = _DAT_800ac784;
      *(undefined2 *)(_DAT_800ac784 + 0x2f) = 1000;
      *(undefined2 *)((int)puVar4 + 0xbe) = 0x44c;
      *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
    }
    iVar3 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (iVar3 != 0) {
      _DAT_800ac784[1] = 0x101;
    }
  }
  return;
}


