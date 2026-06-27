void FUN_80029b48(int param_1)

{
  if ((&DAT_800b2924)[param_1 * 0x40] != 0) {
    (&DAT_800b2924)[param_1 * 0x40] = 0;
    FUN_8006e170();
    CloseTh(*(undefined4 *)(&DAT_800b292c + param_1 * 0x80));
    FUN_8006e468();
  }
  return;
}
