/* FUN_80114050 @ 0x80114050  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114050(undefined4 param_1,uint param_2)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint *puVar4;
  ulonglong uVar5;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  uVar5 = (ulonglong)CONCAT14(bVar1,(uint)(bVar1 < 2));
  if (bVar1 != 1) {
    puVar4 = _DAT_800ac784;
    if (bVar1 < 2 == 0) {
      uVar5 = 0x60100000002;
      if (bVar1 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
        _DAT_800ac784[1] = 0x601;
        if ((short)_DAT_800ac784[0x77] != 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x77) = 100;
          *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        }
        return;
      }
      goto LAB_80114a30;
    }
    if (bVar1 != 0) goto LAB_80114a30;
    *(byte *)((int)_DAT_800ac784 + 0x93) = *(byte *)((int)_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 4;
    if ((short)_DAT_800ac784[0x77] != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 7;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
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
  puVar4 = (uint *)0x0;
  param_2 = 1;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar2;
  uVar5 = FUN_80115bec();
LAB_80114a30:
  while (*puVar4 = (uint)uVar5, (int)(uVar5 >> 0x20) != 0) {
    puVar4 = (uint *)((param_2 & 0xff) * 0xac + _DAT_800ac784[0x62]);
    uVar5 = CONCAT44(param_2,*puVar4) & 0xffffffffff | 1;
    param_2 = param_2 - 1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x95) == '2') {
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
  return;
}


