void _spu_setReverbAttr(uint *param_1)

{
  bool bVar1;
  uint uVar2;
  
  uVar2 = *param_1;
  bVar1 = uVar2 == 0;
  if ((bVar1) || ((uVar2 & 1) != 0)) {
    _DAT_1f801dc0 = (undefined2)param_1[1];
  }
  if ((bVar1) || ((uVar2 & 2) != 0)) {
    _DAT_1f801dc2 = *(undefined2 *)((int)param_1 + 6);
  }
  if ((bVar1) || ((uVar2 & 4) != 0)) {
    _DAT_1f801dc4 = (undefined2)param_1[2];
  }
  if ((bVar1) || ((uVar2 & 8) != 0)) {
    _DAT_1f801dc6 = *(undefined2 *)((int)param_1 + 10);
  }
  if ((bVar1) || ((uVar2 & 0x10) != 0)) {
    _DAT_1f801dc8 = (undefined2)param_1[3];
  }
  if ((bVar1) || ((uVar2 & 0x20) != 0)) {
    _DAT_1f801dca = *(undefined2 *)((int)param_1 + 0xe);
  }
  if ((bVar1) || ((uVar2 & 0x40) != 0)) {
    _DAT_1f801dcc = (undefined2)param_1[4];
  }
  if ((bVar1) || ((uVar2 & 0x80) != 0)) {
    _DAT_1f801dce = *(undefined2 *)((int)param_1 + 0x12);
  }
  if ((bVar1) || ((uVar2 & 0x100) != 0)) {
    _DAT_1f801dd0 = (undefined2)param_1[5];
  }
  if ((bVar1) || ((uVar2 & 0x200) != 0)) {
    _DAT_1f801dd2 = *(undefined2 *)((int)param_1 + 0x16);
  }
  if ((bVar1) || ((uVar2 & 0x400) != 0)) {
    _DAT_1f801dd4 = (undefined2)param_1[6];
  }
  if ((bVar1) || ((uVar2 & 0x800) != 0)) {
    _DAT_1f801dd6 = *(undefined2 *)((int)param_1 + 0x1a);
  }
  if ((bVar1) || ((uVar2 & 0x1000) != 0)) {
    _DAT_1f801dd8 = (undefined2)param_1[7];
  }
  if ((bVar1) || ((uVar2 & 0x2000) != 0)) {
    _DAT_1f801dda = *(undefined2 *)((int)param_1 + 0x1e);
  }
  if ((bVar1) || ((uVar2 & 0x4000) != 0)) {
    _DAT_1f801ddc = (undefined2)param_1[8];
  }
  if ((bVar1) || ((uVar2 & 0x8000) != 0)) {
    _DAT_1f801dde = *(undefined2 *)((int)param_1 + 0x22);
  }
  if ((bVar1) || ((uVar2 & 0x10000) != 0)) {
    _DAT_1f801de0 = (undefined2)param_1[9];
  }
  if ((bVar1) || ((uVar2 & 0x20000) != 0)) {
    _DAT_1f801de2 = *(undefined2 *)((int)param_1 + 0x26);
  }
  if ((bVar1) || ((uVar2 & 0x40000) != 0)) {
    _DAT_1f801de4 = (undefined2)param_1[10];
  }
  if ((bVar1) || ((uVar2 & 0x80000) != 0)) {
    _DAT_1f801de6 = *(undefined2 *)((int)param_1 + 0x2a);
  }
  if ((bVar1) || ((uVar2 & 0x100000) != 0)) {
    _DAT_1f801de8 = (undefined2)param_1[0xb];
  }
  if ((bVar1) || ((uVar2 & 0x200000) != 0)) {
    _DAT_1f801dea = *(undefined2 *)((int)param_1 + 0x2e);
  }
  if ((bVar1) || ((uVar2 & 0x400000) != 0)) {
    _DAT_1f801dec = (undefined2)param_1[0xc];
  }
  if ((bVar1) || ((uVar2 & 0x800000) != 0)) {
    _DAT_1f801dee = *(undefined2 *)((int)param_1 + 0x32);
  }
  if ((bVar1) || ((uVar2 & 0x1000000) != 0)) {
    _DAT_1f801df0 = (undefined2)param_1[0xd];
  }
  if ((bVar1) || ((uVar2 & 0x2000000) != 0)) {
    _DAT_1f801df2 = *(undefined2 *)((int)param_1 + 0x36);
  }
  if ((bVar1) || ((uVar2 & 0x4000000) != 0)) {
    _DAT_1f801df4 = (undefined2)param_1[0xe];
  }
  if ((bVar1) || ((uVar2 & 0x8000000) != 0)) {
    _DAT_1f801df6 = *(undefined2 *)((int)param_1 + 0x3a);
  }
  if ((bVar1) || ((uVar2 & 0x10000000) != 0)) {
    _DAT_1f801df8 = (undefined2)param_1[0xf];
  }
  if ((bVar1) || ((uVar2 & 0x20000000) != 0)) {
    _DAT_1f801dfa = *(undefined2 *)((int)param_1 + 0x3e);
  }
  if ((bVar1) || ((uVar2 & 0x40000000) != 0)) {
    _DAT_1f801dfc = (undefined2)param_1[0x10];
  }
  if ((bVar1) || ((int)uVar2 < 0)) {
    _DAT_1f801dfe = *(undefined2 *)((int)param_1 + 0x42);
  }
  return;
}
