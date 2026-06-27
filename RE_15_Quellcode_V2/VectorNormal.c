long VectorNormal(VECTOR *$2,VECTOR *$3)

{
  long lVar1;
  long lVar2;
  long lVar3;
  long lVar4;
  
  lVar2 = $2->vx;
  lVar3 = $2->vy;
  lVar4 = $2->vz;
  lVar1 = MSC02_OBJ_FC();
  $3->vx = lVar2;
  $3->vy = lVar3;
  $3->vz = lVar4;
  return lVar1;
}
