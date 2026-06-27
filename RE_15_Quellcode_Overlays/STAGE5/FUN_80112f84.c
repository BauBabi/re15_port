/* FUN_80112f84 @ 0x80112f84  (Ghidra headless overlay RE) */

void FUN_80112f84(void)

{
  int in_at;
  int in_v0;
  
                    /* WARNING: Could not recover jumptable at 0x8011380c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(in_at + 0x30c + in_v0))();
  return;
}


