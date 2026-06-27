void ReadLightMatrix(MATRIX *m)

{
  undefined4 uVar1;
  long lVar2;
  undefined4 uVar3;
  long lVar4;
  undefined4 uVar5;
  long lVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  uVar1 = gte_stL11L12();
  uVar3 = gte_stL13L21();
  uVar5 = gte_stL22L23();
  uVar7 = gte_stL31L32();
  uVar8 = gte_stL33();
  *(undefined4 *)m->m[0] = uVar1;
  *(undefined4 *)(m->m[0] + 2) = uVar3;
  *(undefined4 *)(m->m[1] + 1) = uVar5;
  *(undefined4 *)m->m[2] = uVar7;
  *(undefined4 *)(m->m[2] + 2) = uVar8;
  lVar2 = gte_stRBK();
  lVar4 = gte_stGBK();
  lVar6 = gte_stBBK();
  m->t[0] = lVar2;
  m->t[1] = lVar4;
  m->t[2] = lVar6;
  return;
}
