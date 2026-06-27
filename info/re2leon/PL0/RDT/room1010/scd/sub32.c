int sub32(void) {

Work_set(3, 4);
nop();
Member_copy(16, 42);
nop();
Calc(16 || 16384)
Member_set2(42, 16);
nop();
Sce_parts_bomb(/* WIP */);
Sce_parts_down(/* WIP */);
Sce_espr_on(0x00, 0x00, 0, 204, 4, 0x1D, 0, 0, 0, 21251);
Sce_espr_on(0x00, 0x00, 0, 204, 4, 0x10, 19711, 0, 0, 21251);
Sce_espr_on(0x00, 0x00, 0, 204, 4, 0x08, 0, 0, 19711, 21251);
for(n = 0; n < 5120; n++) {
Sce_espr_on(0x00, 0x00, 0, 192, 4, 0x10, 0, 16126, -3071, -7156);
Sleep(1280);
}
return 0;
}