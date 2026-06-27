/* FUN_801109ec @ 0x801109ec  (Ghidra headless overlay RE) */

void FUN_801109ec(void)

{
  int in_v0;
  int in_v1;
  
  if (in_v0 != 0) {
    *(short *)(in_v1 + 0x1de) = *(short *)(in_v1 + 0x1de) + 1;
    FUN_8011120c();
    return;
  }
  *(undefined2 *)(in_v1 + 0x1de) = 0;
  return;
}


