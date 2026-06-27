MATRIX * SetMulMatrix(MATRIX *m0,MATRIX *m1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  
  gte_ldR11R12(*(undefined4 *)m0->m[0]);
  gte_ldR13R21(*(undefined4 *)(m0->m[0] + 2));
  gte_ldR22R23(*(undefined4 *)(m0->m[1] + 1));
  gte_ldR31R32(*(undefined4 *)m0->m[2]);
  gte_ldR33(*(undefined4 *)(m0->m[2] + 2));
  gte_ldVXY0((uint)(ushort)m1->m[0][0] | *(uint *)(m1->m[0] + 2) & 0xffff0000);
  gte_ldVZ0(*(undefined4 *)m1->m[2]);
  gte_rtv0_b();
  uVar4 = gte_stIR1();
  iVar5 = gte_stIR2();
  uVar6 = gte_stIR3();
  gte_ldVXY0((uint)(ushort)m1->m[0][1] | *(int *)(m1->m[1] + 1) << 0x10);
  gte_ldVZ0((int)m1->m[2][1]);
  gte_rtv0_b();
  iVar7 = gte_stIR1();
  uVar8 = gte_stIR2();
  iVar9 = gte_stIR3();
  gte_ldVXY0((uint)(ushort)m1->m[0][2] | *(uint *)(m1->m[1] + 1) & 0xffff0000);
  gte_ldVZ0(*(undefined4 *)(m1->m[2] + 2));
  gte_rtv0_b();
  uVar1 = gte_stIR1();
  iVar2 = gte_stIR2();
  uVar3 = gte_stIR3();
  gte_ldR11R12(iVar7 << 0x10 | uVar4 & 0xffff);
  gte_ldR13R21(uVar1 & 0xffff | iVar5 << 0x10);
  gte_ldR22R23(iVar2 << 0x10 | uVar8 & 0xffff);
  gte_ldR31R32(iVar9 << 0x10 | uVar6 & 0xffff);
  gte_ldR33(uVar3);
  return m0;
}
