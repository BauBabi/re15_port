void _SsSndStop(short param_1,short param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  piVar3 = &DAT_800bb500 + param_1;
  iVar1 = param_2 * 0xac;
  iVar4 = iVar1 + *piVar3;
  *(uint *)(iVar4 + 0x90) = *(uint *)(iVar4 + 0x90) & 0xfffffffe;
  *(uint *)(iVar1 + *piVar3 + 0x90) = *(uint *)(iVar1 + *piVar3 + 0x90) & 0xfffffffd;
  *(uint *)(iVar1 + *piVar3 + 0x90) = *(uint *)(iVar1 + *piVar3 + 0x90) & 0xfffffff7;
  *(uint *)(iVar1 + *piVar3 + 0x90) = *(uint *)(iVar1 + *piVar3 + 0x90) | 4;
  SpuVmSeqKeyOff((int)param_2 << 8 | (int)param_1);
  iVar2 = 0;
  *(undefined1 *)(iVar4 + 0x2b) = 0;
  *(undefined4 *)(iVar4 + 0x80) = 0;
  *(undefined1 *)(iVar4 + 0x27) = 0;
  *(undefined1 *)(iVar4 + 0x13) = 0;
  *(undefined1 *)(iVar4 + 0x14) = 0;
  *(undefined1 *)(iVar4 + 0x29) = 0;
  *(undefined1 *)(iVar4 + 0x15) = 0;
  *(undefined1 *)(iVar4 + 0x16) = 0;
  *(undefined1 *)(iVar4 + 0x2a) = 0;
  *(undefined1 *)(iVar4 + 0x12) = 0;
  *(undefined2 *)(iVar4 + 0x48) = 0;
  *(undefined1 *)(iVar4 + 0x27) = 0;
  *(undefined1 *)(iVar4 + 0x28) = 0;
  *(undefined1 *)(iVar4 + 0x10) = 0;
  *(undefined1 *)(iVar4 + 0x11) = 0;
  *(undefined4 *)(iVar4 + 0x88) = *(undefined4 *)(iVar4 + 0x7c);
  *(undefined4 *)(iVar4 + 0x8c) = *(undefined4 *)(iVar4 + 0x84);
  *(undefined2 *)(iVar4 + 0x70) = *(undefined2 *)(iVar4 + 0x72);
  *(undefined4 *)(iVar4 + 4) = *(undefined4 *)(iVar4 + 8);
  *(undefined4 *)(iVar4 + 0xc) = *(undefined4 *)(iVar4 + 8);
  iVar1 = iVar4;
  do {
    *(char *)(iVar4 + iVar2 + 0x2c) = (char)iVar2;
                    /* Possible PsyQ macro: setLineF2() */
    *(undefined1 *)(iVar4 + iVar2 + 0x17) = 0x40;
    *(undefined2 *)(iVar1 + 0x4e) = 0x7f;
    iVar2 = iVar2 + 1;
    iVar1 = iVar1 + 2;
  } while (iVar2 < 0x10);
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
  *(undefined2 *)(iVar4 + 0x78) = 0x7f;
  *(undefined2 *)(iVar4 + 0x7a) = 0x7f;
  return;
}
