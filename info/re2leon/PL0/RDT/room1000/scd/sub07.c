int sub07(void) {

Sce_espr_kill(13, 0, 0, 0);
Sce_espr_kill(14, 0, 0, 0);
Sce_espr_kill(4, 18, 0, 0);
nop();
Sce_espr_on(0x00, 0x0D, 0, 0, 0, 0x1E, 27130, 9466, -13589, 254);
Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x18, 4861, 23801, 32487, 0);
Sce_espr_on(0x00, 0x04, 18, 0, 0, 0x20, -25865, -3335, 19691, 2);
Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x18, -31494, -30470, 29670, 0);
Set(5, 6, 1);
Set(5, 5, 0);
return 0;
}