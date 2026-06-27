/* MatrixNormal_0 @ 0x8008ce30  (Ghidra headless, raw MIPS overlay) */

void MatrixNormal_0(MATRIX *m,MATRIX *n)

{
  VECTOR local_40;
  VECTOR local_30;
  VECTOR local_20;
  
  local_40.vx = (long)m->m[0][0];
  local_40.vy = (long)m->m[0][1];
  local_40.vz = (long)m->m[0][2];
  local_30.vx = (long)m->m[1][0];
  local_30.vy = (long)m->m[1][1];
  local_30.vz = (long)m->m[1][2];
  OuterProduct12(&local_40,&local_30,&local_20);
  OuterProduct12(&local_30,&local_20,&local_40);
  VectorNormal(&local_40,&local_40);
  VectorNormal(&local_30,&local_30);
  VectorNormal(&local_20,&local_20);
  n->m[0][0] = (short)local_40.vx;
  n->m[0][1] = (short)local_40.vy;
  n->m[0][2] = (short)local_40.vz;
  n->m[1][0] = (short)local_30.vx;
  n->m[1][1] = (short)local_30.vy;
  n->m[1][2] = (short)local_30.vz;
  n->m[2][0] = (short)local_20.vx;
  n->m[2][1] = (short)local_20.vy;
  n->m[2][2] = (short)local_20.vz;
  return;
}


