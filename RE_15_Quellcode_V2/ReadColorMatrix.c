void ReadColorMatrix(MATRIX *m)

{
  undefined4 uVar1;
  long lVar2;
  undefined4 uVar3;
  long lVar4;
  undefined4 uVar5;
  long lVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  uVar1 = gte_stLR1LR2();
  uVar3 = gte_stLR3LG1();
  uVar5 = gte_stLG2LG3();
  uVar7 = gte_stLB1LB2();
  uVar8 = gte_stLB3();
  *(undefined4 *)m->m[0] = uVar1;
  *(undefined4 *)(m->m[0] + 2) = uVar3;
  *(undefined4 *)(m->m[1] + 1) = uVar5;
  *(undefined4 *)m->m[2] = uVar7;
  *(undefined4 *)(m->m[2] + 2) = uVar8;
  lVar2 = gte_stRFC();
  lVar4 = gte_stGFC();
  lVar6 = gte_stBFC();
  m->t[0] = lVar2;
  m->t[1] = lVar4;
  m->t[2] = lVar6;
  return;
}
