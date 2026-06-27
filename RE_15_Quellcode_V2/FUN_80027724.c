uint FUN_80027724(char param_1)

{
  uint *puVar1;
  uint uVar2;
  undefined2 uStack_10;
  char local_e;
  undefined4 uStack_d;
  
  uVar2 = (uint)&uStack_d & 3;
  puVar1 = (uint *)((int)&uStack_d + -uVar2);
  *puVar1 = *puVar1 & -1 << (uVar2 + 1) * 8 | 0x30307562U >> (3 - uVar2) * 8;
  uStack_d._1_1_ = 0x3a;
  uStack_d._2_1_ = 0;
  _uStack_10 = CONCAT13(0x30,CONCAT12(param_1 + '0',0x7562));
  uVar2 = format(&uStack_10);
  return uVar2 ^ 1;
}
