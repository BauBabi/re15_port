void OuterProduct12(VECTOR *$2,VECTOR *$3,VECTOR *v2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  uVar1 = gte_stR11R12();
  uVar2 = gte_stR22R23();
  uVar3 = gte_stR33();
  gte_ldR11R12($2->vx);
  gte_ldR22R23($2->vy);
  gte_ldR33($2->vz);
  gte_ldopv2($3);
  gte_op12_b();
  gte_stlvnl(v2);
  gte_ldR11R12(uVar1);
  gte_ldR22R23(uVar2);
  gte_ldR33(uVar3);
  return;
}
