undefined4 FUN_800299a4(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_80013b60(*(undefined4 *)(&DAT_80073bdc + param_2 * 0xc),
                       *(undefined4 *)(&PTR_PTR_80073be4)[param_2 * 3],0);
  FUN_80029a98(param_1,(&PTR_DAT_80073be0)[param_2 * 3]);
  return uVar1;
}
