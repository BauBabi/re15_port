int sub24(void) {

Work_set(1, 0);
Plc_motion(0, 2, 0);
nop();
Sleep(7680);
Plc_motion(0, 4, 0);
Sleep(6144);
return 0;
}