/* FUN_8011a268 @ 0x8011a268  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a268(void)

{
  int iVar1;
  code *pcVar2;
  int in_v1;
  int iVar3;
  int iVar4;
  undefined2 unaff_s0;
  undefined2 unaff_s1;
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  int unaff_s4;
  
  DAT_800aca5a = 8;
  DAT_800acae8 = 0;
  _DAT_800acaf0 = 0x4b;
  if (in_v1 < 0x1e) {
    _DAT_800acaf0 = 0x113;
  }
  if (in_v1 < 10) {
    _DAT_800acaf0 = _DAT_800acaf0 + 0x17c;
  }
  func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  iVar1 = func_0x80037024();
  iVar1 = (_DAT_800acaf0 - 4) + iVar1 * -9;
  _DAT_800acaf0 = (ushort)iVar1;
  if (iVar1 * 0x10000 < 0) {
    DAT_800aca5a = '\t';
    *(uint *)(unaff_s4 + 0x35c) = *(uint *)(unaff_s4 + 0x35c) | 0x80;
    *(undefined4 *)(unaff_s4 + 0x3f0) = unaff_s3;
    *(undefined4 *)(unaff_s4 + 0x3f4) = unaff_s2;
    *(undefined2 *)(unaff_s4 + 0x3e4) = unaff_s0;
    *(undefined2 *)(unaff_s4 + 0x3e6) = unaff_s1;
    *(undefined2 *)(unaff_s4 + 1000) = unaff_s0;
    *(uint *)(unaff_s4 + 0x35c) = *(uint *)(unaff_s4 + 0x35c) | 0x2000;
    func_0x80019700(0x2800,0,unaff_s4 + 0x39c);
    iVar1 = _DAT_800acbdc;
    iVar3 = (int)_DAT_800acc1c - *(int *)(_DAT_800acbdc + 0x100);
    iVar4 = (int)_DAT_800acc1e - *(int *)(_DAT_800acbdc + 0x108);
    iVar3 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    if (300 < iVar3) {
      *(undefined4 *)(iVar1 + 0x140) = 0x70;
      *(undefined4 *)(iVar1 + 0x144) = 0x20;
      *(undefined2 *)(iVar1 + 0x134) = 6000;
      *(undefined2 *)(iVar1 + 0x138) = 6000;
      *(uint *)(iVar1 + 0xac) = *(uint *)(iVar1 + 0xac) | 0x80;
      *(undefined2 *)(iVar1 + 0x136) = 3000;
      *(uint *)(iVar1 + 0xac) = *(uint *)(iVar1 + 0xac) | 0x2000;
      func_0x80019700(0x3000,0,iVar1 + 0xec);
    }
    iVar1 = _DAT_800acbdc;
    iVar3 = (int)_DAT_800acc1c - *(int *)(_DAT_800acbdc + 0x5b4);
    iVar4 = (int)_DAT_800acc1e - *(int *)(_DAT_800acbdc + 0x5bc);
    iVar3 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    if (300 < iVar3) {
      *(undefined4 *)(iVar1 + 0x5f4) = 0x70;
      *(undefined4 *)(iVar1 + 0x5f8) = 0x20;
      *(undefined2 *)(iVar1 + 0x5e8) = 6000;
      *(undefined2 *)(iVar1 + 0x5ec) = 6000;
      *(uint *)(iVar1 + 0x560) = *(uint *)(iVar1 + 0x560) | 0x80;
      *(undefined2 *)(iVar1 + 0x5ea) = 3000;
      *(uint *)(iVar1 + 0x560) = *(uint *)(iVar1 + 0x560) | 0x2000;
      func_0x80019700(0x3000,0,iVar1 + 0x5a0);
    }
    pcVar2 = (code *)func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
    DAT_800aca5a = DAT_800aca5a + (char)pcVar2;
    (*pcVar2)();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(iVar1 + 0x14,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
    func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
    return;
  }
  func_0x800245d8(0x800);
  return;
}


