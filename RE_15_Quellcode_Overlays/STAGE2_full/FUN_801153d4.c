/* FUN_801153d4 @ 0x801153d4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801153d4(void)

{
  uint *puVar1;
  char cVar2;
  uint *puVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  case 1:
    puVar3 = _DAT_800ac784;
    puVar1 = _DAT_800ac784 + 0x3b;
    _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xdf809f;
    puVar3[0x3b] = *puVar1 & 0xff000000 | 0xdf809f;
    *(short *)(puVar3 + 0x2f) = (short)puVar3[0x2f] + 8;
    *(short *)((int)puVar3 + 0xbe) = *(short *)((int)puVar3 + 0xbe) + 8;
    break;
  default:
    goto switchD_8011540c_caseD_2;
  case 3:
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x78;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 4;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  case 4:
    puVar3 = _DAT_800ac784;
    puVar1 = _DAT_800ac784 + 0x3b;
    _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xdf809f;
    puVar3[0x3b] = *puVar1 & 0xff000000 | 0xdf809f;
    *(short *)(puVar3 + 0x2f) = (short)puVar3[0x2f] + 8;
    *(short *)((int)puVar3 + 0xbe) = *(short *)((int)puVar3 + 0xbe) + 8;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 2;
    func_0x800453d0(9);
  }
switchD_8011540c_caseD_2:
  return;
}


