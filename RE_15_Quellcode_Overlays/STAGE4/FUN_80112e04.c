/* FUN_80112e04 @ 0x80112e04  (Ghidra headless overlay RE) */

void FUN_80112e04(void)

{
  int in_at;
  int in_v0;
  
                    /* WARNING: Could not recover jumptable at 0x8011368c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(in_at + 0x30c + in_v0))();
  return;
}


