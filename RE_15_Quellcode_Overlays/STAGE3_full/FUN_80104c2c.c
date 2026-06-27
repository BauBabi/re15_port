/* FUN_80104c2c @ 0x80104c2c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104c2c(void)

{
  char cVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = DAT_80100068;
  local_1c = DAT_8010006c;
  local_18 = DAT_80100070;
  local_14 = DAT_80100074;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x9c) + *(int *)(_DAT_800ac784 + 0x38);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x1e;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      uVar2 = func_0x8001af20();
      if ((uVar2 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      }
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 1;
    func_0x800453d0(0);
  case 3:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x170);
    uVar4 = *(undefined4 *)(_DAT_800ac784 + 0x174);
    uVar5 = 0x200;
    goto LAB_80104fc8;
  case 4:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x158);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x204);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x35c);
    FUN_80108df0(*(int *)(_DAT_800ac784 + 0x188) + 0x408);
    func_0x80019700(0x5002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xec,&local_20);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar6 + 0x140) = 0x80;
    *(undefined4 *)(iVar6 + 0x144) = 0x20;
    *(uint *)(iVar6 + 0xac) = *(uint *)(iVar6 + 0xac) | 0x80;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 1) != 0) {
      *(undefined2 *)(iVar6 + 0x134) = 0x5f4;
      *(undefined2 *)(iVar6 + 0x136) = 0xc18;
      *(undefined2 *)(iVar6 + 0x138) = 0x5f4;
      *(uint *)(iVar6 + 0xac) = *(uint *)(iVar6 + 0xac) | 0x400;
    }
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar6 + 0x344) = 0x80;
    *(undefined4 *)(iVar6 + 0x348) = 0x20;
    *(uint *)(iVar6 + 0x2b0) = *(uint *)(iVar6 + 0x2b0) | 0x80;
    uVar2 = func_0x8001af20();
    if ((uVar2 & 1) != 0) {
      *(undefined2 *)(iVar6 + 0x338) = 0x5f4;
      *(undefined2 *)(iVar6 + 0x33a) = 0xc18;
      *(undefined2 *)(iVar6 + 0x33c) = 0x5f4;
      *(uint *)(iVar6 + 0x2b0) = *(uint *)(iVar6 + 0x2b0) | 0x400;
    }
  case 6:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x170);
    uVar4 = *(undefined4 *)(_DAT_800ac784 + 0x174);
    uVar5 = 0x100;
LAB_80104fc8:
    cVar1 = func_0x8001f314(uVar3,uVar4,0,uVar5);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 7:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
    *(undefined4 *)(_DAT_800ac784 + 4) = 1;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x80;
  }
  return;
}


