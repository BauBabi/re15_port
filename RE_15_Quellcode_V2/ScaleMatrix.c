MATRIX * ScaleMatrix(MATRIX *m,VECTOR *v)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = *(int *)m->m[0];
  iVar4 = v->vx;
  iVar5 = v->vy;
  iVar6 = v->vz;
  iVar2 = *(int *)(m->m[0] + 2);
  *(uint *)m->m[0] =
       (short)iVar1 * iVar4 >> 0xc & 0xffffU | ((iVar1 >> 0x10) * iVar5 >> 0xc) << 0x10;
  iVar1 = *(int *)(m->m[1] + 1);
  *(uint *)(m->m[0] + 2) =
       (short)iVar2 * iVar6 >> 0xc & 0xffffU | ((iVar2 >> 0x10) * iVar4 >> 0xc) << 0x10;
  iVar2 = *(int *)m->m[2];
  *(uint *)(m->m[1] + 1) =
       (short)iVar1 * iVar5 >> 0xc & 0xffffU | ((iVar1 >> 0x10) * iVar6 >> 0xc) << 0x10;
  uVar3 = *(undefined4 *)(m->m[2] + 2);
  *(uint *)m->m[2] =
       (short)iVar2 * iVar4 >> 0xc & 0xffffU | ((iVar2 >> 0x10) * iVar5 >> 0xc) << 0x10;
  *(int *)(m->m[2] + 2) = (short)uVar3 * iVar6 >> 0xc;
  return m;
}
