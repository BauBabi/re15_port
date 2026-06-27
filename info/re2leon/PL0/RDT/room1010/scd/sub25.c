int sub25(void) {

Work_set(3, 0);
nop();
Plc_neck(5, 1, 0, 0, 0, 24672);
Plc_motion(0, 6, 0);
Plc_rot(1, 4);
Sleep(7680);
Plc_dest(0, 1057, -9052, 15526);
return 0;
}