/* FUN_80100000 @ 0x80100000  (Ghidra headless overlay RE) */

void FUN_80100000(void)

{
  int in_at;
  undefined4 in_v0;
  
  *(undefined4 *)(in_at + -0x439c) = in_v0;
  func_0x8011b104(0,0);
  if (sRam8011bcde == 0x23) {
    uRam8011bc64 = 0xff000;
    func_0x8011b104(1,0);
  }
  if (sRam8011bcde == 0x24) {
    uRam8011bc64 = 0xff000;
    func_0x8011b104(0,0);
  }
  if (sRam8011bcde == 0x2d) {
    uRam8011bc64 = 0xf0000;
    func_0x8011b104(1,0);
  }
  if (sRam8011bcde == 0x2e) {
    uRam8011bc64 = 0xf0000;
    func_0x8011b104(0,0);
  }
  if (sRam8011bcde == 0x32) {
    uRam8011bc64 = 0xf000;
    func_0x8011b104(1,0);
  }
  if (sRam8011bcde == 0x33) {
    uRam8011bc64 = 0xf000;
    func_0x8011b104(0,0);
  }
  if (sRam8011bcde == 0x37) {
    uRam8011bc64 = 0xf0000;
    func_0x8011b104(1,0);
  }
  if (sRam8011bcde == 0x39) {
    uRam8011bc64 = 0xf0000;
    func_0x8011b104(0,0);
  }
  if (sRam8011bcde == 0x3c) {
    uRam8011bc64 = 0xff000;
    func_0x8011b104(1,0);
  }
  sRam8011bcde = sRam8011bcde + 1;
  return;
}


