MATRIX * TransposeMatrix(MATRIX *m0,MATRIX *m1)

{
  short sVar1;
  short sVar2;
  
  m1->m[0][0] = m0->m[0][0];
  sVar1 = m0->m[2][0];
  m1->m[0][1] = m0->m[1][0];
  sVar2 = m0->m[0][1];
  m1->m[0][2] = sVar1;
  sVar1 = m0->m[1][1];
  m1->m[1][0] = sVar2;
  sVar2 = m0->m[2][1];
  m1->m[1][1] = sVar1;
  sVar1 = m0->m[0][2];
  m1->m[1][2] = sVar2;
  sVar2 = m0->m[1][2];
  m1->m[2][0] = sVar1;
  sVar1 = m0->m[2][2];
  m1->m[2][1] = sVar2;
  m1->m[2][2] = sVar1;
  return m1;
}
