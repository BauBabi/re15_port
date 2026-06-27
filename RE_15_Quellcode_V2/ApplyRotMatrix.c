VECTOR * ApplyRotMatrix(SVECTOR *$2,VECTOR *$3)

{
  VECTOR *in_a2;
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1._0_2_ = $2->vx;
  uVar1._2_2_ = $2->vy;
  uVar2._0_2_ = $2->vz;
  uVar2._2_2_ = $2->pad;
  gte_ldVXY0(uVar1);
  gte_ldVZ0(uVar2);
  gte_rtv0_b();
  gte_stlvl($3);
  return in_a2;
}
