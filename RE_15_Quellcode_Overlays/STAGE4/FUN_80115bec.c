/* FUN_80115bec @ 0x80115bec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115bec(void)

{
  int iVar1;
  char cVar2;
  int in_v1;
  int iVar3;
  uint *puVar4;
  
  cVar2 = *(char *)(in_v1 + -0x3517);
  if (*(char *)(in_v1 + -0x3517) == '<') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a2a8);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    *(uint *)(_DAT_800acbdc + 0x560) = *(uint *)(_DAT_800acbdc + 0x560) ^ 0x1062;
    *(undefined1 *)(_DAT_800acbdc + 0x5d5) = 0x14;
    *(undefined2 *)(_DAT_800acbdc + 0x5e2) = 0xff80;
    *(undefined1 *)(_DAT_800acbdc + 0x5d6) = 8;
    *(undefined2 *)(_DAT_800acbdc + 0x598) = 0;
    *(undefined2 *)(_DAT_800acbdc + 0x59a) = 0xff9c;
    *(undefined2 *)(_DAT_800acbdc + 0x59c) = 0;
    puVar4 = *(uint **)(_DAT_800ac784 + 0x188);
    puVar4[0x25] = 0x80;
    puVar4[0x26] = 0x20;
    *puVar4 = *puVar4 | 0x80;
    cVar2 = DAT_800acae9;
  }
  if (cVar2 == '=') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a2a8);
  }
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  iVar3 = (uint)DAT_800aca5a + iVar1;
  DAT_800aca5a = (byte)iVar3;
  *(short *)(iVar3 + 0x5fc) = (short)iVar1;
  *(undefined2 *)(iVar3 + 0x5f4) = 0;
  *(undefined2 *)(iVar3 + 0x5f6) = 0;
  *(undefined2 *)(iVar3 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001af5c(0,0,600,700);
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}


