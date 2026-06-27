void FUN_8001a50c(void)

{
  byte bVar1;
  byte bVar2;
  
  bVar1 = DAT_800aca4e;
  bVar2 = 0;
  DAT_800ac784 = &DAT_800acc2c;
  if (DAT_800aca4e != 0) {
    do {
      if ((*DAT_800ac784 & 1) != 0) {
        bVar2 = bVar2 + 1;
        (**(code **)(&DAT_80072bac + (uint)(byte)DAT_800ac784[2] * 4))();
        *(undefined2 *)(DAT_800ac784 + 0x22) = 0;
      }
      DAT_800ac784 = DAT_800ac784 + 0x7d;
    } while (bVar2 < bVar1);
  }
  return;
}
