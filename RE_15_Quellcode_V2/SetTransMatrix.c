void SetTransMatrix(MATRIX *m)

{
  gte_ldtr(m->t[0],m->t[1],m->t[2]);
  return;
}
