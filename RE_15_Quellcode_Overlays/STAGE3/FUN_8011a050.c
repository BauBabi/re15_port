/* FUN_8011a050 @ 0x8011a050  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a050(uint param_1)

{
  uint *puVar1;
  char cVar2;
  undefined1 *in_v0;
  code *pcVar3;
  int iVar4;
  byte bVar5;
  uint *unaff_s0;
  undefined4 in_stack_0000001c;
  
  *in_v0 = 1;
  DAT_800acae8 = 0xd;
  DAT_800acae9 = 6;
  DAT_800acae3 = 3;
  _DAT_800acae0 = 0x4b0;
  DAT_800acc0c = 1;
  _DAT_800aca3c = _DAT_800aca3c | 0x40;
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x80045024(param_1 | 1);
  cVar2 = func_0x8001f314(_DAT_800acad8,_DAT_800acbc0,0,0x400);
  bVar5 = DAT_800aca5a + cVar2;
  _DAT_800acae0 = _DAT_800acae0 + -0x96;
  _DAT_800aca58 = CONCAT12(bVar5,_DAT_800aca58);
  if (_DAT_800acae0 < 400) {
    _DAT_800acae0 = 400;
  }
  if ((_DAT_800aca54 & 0x10) == 0) {
    func_0x800245d8(0x800);
    return;
  }
  in_stack_0000001c = 5;
  if (bVar5 != 1) {
    pcVar3 = (code *)(uint)(bVar5 < 2);
    if (pcVar3 == (code *)0x0) {
      pcVar3 = (code *)0x7;
      if (bVar5 == 2) {
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x8011b5a0;
    }
    if (bVar5 != 0) goto code_r0x8011b5a0;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 3;
    DAT_800acae8 = 1;
    DAT_800acc0c = 1;
    DAT_800acae9 = 0;
    _DAT_800acae0 = 0;
    _DAT_800aca3c = _DAT_800aca3c | 0x40;
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x80045024(0x4030001,&DAT_800aca88);
    puVar1 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x15]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x17]));
    if (300 < iVar4) {
      *(undefined2 *)((int)puVar1 + 0x8a) = 3000;
      *(undefined2 *)(puVar1 + 0x22) = 5000;
      *(undefined2 *)(puVar1 + 0x23) = 5000;
      puVar1[0x25] = 0x70;
      *puVar1 = *puVar1 | 0x2000;
      puVar1[0x26] = 0x20;
      *puVar1 = *puVar1 | 0x80;
      func_0x80019700(0x3000,0,puVar1 + 0x10,&stack0x00000010);
    }
    puVar1 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x6b]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x6d]));
    if (300 < iVar4) {
      puVar1[0x56] = puVar1[0x56] | 0x80;
      puVar1[0x7b] = 0x70;
      puVar1[0x7c] = 0x20;
      *(undefined2 *)(puVar1 + 0x78) = 5000;
      *(undefined2 *)((int)puVar1 + 0x1e2) = 3000;
      *(undefined2 *)(puVar1 + 0x79) = 5000;
      puVar1[0x56] = puVar1[0x56] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0x66,&stack0x00000010);
      puVar1[0xd7] = puVar1[0xd7] | 0x80;
      puVar1[0xfc] = 0x70;
      puVar1[0xfd] = 0x20;
      *(undefined2 *)(puVar1 + 0xf9) = 5000;
      *(undefined2 *)((int)puVar1 + 0x3e6) = 3000;
      *(undefined2 *)(puVar1 + 0xfa) = 5000;
      puVar1[0xd7] = puVar1[0xd7] | 0x2000;
      func_0x80019700(0x2800,0,puVar1 + 0xe7,&stack0x00000010);
    }
    puVar1 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x40]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x42]));
    if (300 < iVar4) {
      puVar1[0x50] = 0x70;
      puVar1[0x51] = 0x20;
      *(undefined2 *)(puVar1 + 0x4d) = 6000;
      *(undefined2 *)(puVar1 + 0x4e) = 6000;
      puVar1[0x2b] = puVar1[0x2b] | 0x80;
      *(undefined2 *)((int)puVar1 + 0x136) = 3000;
      puVar1[0x2b] = puVar1[0x2b] | 0x2000;
      func_0x80019700(0x3000,0,puVar1 + 0x3b,&stack0x00000010);
    }
    unaff_s0 = _DAT_800acbdc;
    iVar4 = func_0x80065f60(((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) *
                            ((int)_DAT_800acc1c - _DAT_800acbdc[0x16d]) +
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]) *
                            ((int)_DAT_800acc1e - _DAT_800acbdc[0x16f]));
    if (300 < iVar4) {
      unaff_s0[0x17d] = 0x70;
      unaff_s0[0x17e] = 0x20;
      *(undefined2 *)(unaff_s0 + 0x17a) = 6000;
      *(undefined2 *)(unaff_s0 + 0x17b) = 6000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x80;
      *(undefined2 *)((int)unaff_s0 + 0x5ea) = 3000;
      unaff_s0[0x158] = unaff_s0[0x158] | 0x2000;
      func_0x80019700(0x3000,0,unaff_s0 + 0x168,&stack0x00000010);
    }
  }
  pcVar3 = (code *)func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + (char)pcVar3,_DAT_800aca58);
code_r0x8011b5a0:
  (*pcVar3)();
  func_0x8002b498(_DAT_800ac784);
  func_0x8002aec4(unaff_s0 + 5,_DAT_800ac784);
  func_0x8002b544();
  func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  func_0x80037358();
  func_0x8001b38c();
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


