int sub23(void) {

Sleep(2560);
Work_set(1, 0);
Plc_motion(0, 0, 8);
nop();
Sleep(1280);
Plc_stop();
nop();
return 0;
}