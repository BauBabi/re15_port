int sub11(void) {

Work_set(3, 0);
nop();
Plc_neck(5, 3, 0, 1024, 0, -32640);
Plc_dest(0, 2337, -12084, -29764);
Plc_rot(0, 16384);
goto Sub21();
Plc_motion(0, 0, 8);
Sleep(1024);
Plc_neck(4, 4, 0, 0, 0, -32640);
Sleep(3584);
Plc_neck(5, 3, 0, 1024, 0, -32640);
Plc_motion(1, 1, 8);
Sleep(1280);
Plc_stop();
nop();
return 0;
}