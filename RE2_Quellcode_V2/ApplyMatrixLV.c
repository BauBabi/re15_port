/* ApplyMatrixLV @ 0x8008d7d4  (Ghidra headless, raw MIPS overlay) */

VECTOR * ApplyMatrixLV(MATRIX *m,VECTOR *_2,VECTOR *_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  gte_ldR11R12(*(undefined4 *)m->m[0]);
  gte_ldR13R21(*(undefined4 *)(m->m[0] + 2));
  gte_ldR22R23(*(undefined4 *)(m->m[1] + 1));
  gte_ldR31R32(*(undefined4 *)m->m[2]);
  gte_ldR33(*(undefined4 *)(m->m[2] + 2));
  uVar1 = _2->vx;
  uVar2 = _2->vy;
  uVar3 = _2->vz;
  iVar4 = (int)uVar1 >> 0xf;
  if ((int)uVar1 < 0) {
    iVar4 = -((int)-uVar1 >> 0xf);
    uVar1 = -(-uVar1 & 0x7fff);
  }
  else {
    uVar1 = uVar1 & 0x7fff;
  }
  iVar5 = (int)uVar2 >> 0xf;
  if ((int)uVar2 < 0) {
    iVar5 = -((int)-uVar2 >> 0xf);
    uVar2 = -(-uVar2 & 0x7fff);
  }
  else {
    uVar2 = uVar2 & 0x7fff;
  }
  iVar6 = (int)uVar3 >> 0xf;
  if ((int)uVar3 < 0) {
    iVar6 = -((int)-uVar3 >> 0xf);
    uVar3 = -(-uVar3 & 0x7fff);
  }
  else {
    uVar3 = uVar3 & 0x7fff;
  }
  gte_ldsv_(iVar4,iVar5,iVar6);
  gte_rtir_sf0_b();
  read_mt(iVar4,iVar5,iVar6);
  gte_ldsv_(uVar1,uVar2,uVar3);
  gte_rtir_b();
  if (iVar4 < 0) {
    iVar4 = iVar4 * 8;
  }
  else {
    iVar4 = iVar4 << 3;
  }
  if (iVar5 < 0) {
    iVar5 = iVar5 * 8;
  }
  else {
    iVar5 = iVar5 << 3;
  }
  if (iVar6 < 0) {
    iVar6 = iVar6 * 8;
  }
  else {
    iVar6 = iVar6 << 3;
  }
  read_mt(uVar1,uVar2,uVar3);
  _3->vx = uVar1 + iVar4;
  _3->vy = uVar2 + iVar5;
  _3->vz = uVar3 + iVar6;
  return _3;
}


