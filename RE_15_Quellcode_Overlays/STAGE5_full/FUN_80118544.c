/* FUN_80118544 @ 0x80118544  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118544(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100414;
  local_14 = DAT_80100418;
  local_10 = DAT_8010041c;
  local_c = DAT_80100420;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      }
      goto LAB_80118774;
    }
    if (bVar1 != 0) goto LAB_80118774;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    func_0x800453d0(3);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) == ',') {
    iVar3 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                            &DAT_800aca54);
    uVar4 = 2;
    if (iVar3 == 0) {
      uVar4 = 6;
    }
    else {
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      _DAT_800aca58 = 6;
      _DAT_800acc1e = (undefined2)local_c;
      _DAT_800acc1c = (undefined2)local_10;
    }
    func_0x800453d0(uVar4);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
  }
LAB_80118774:
  FUN_8011bdd8(0,1);
  return;
}


