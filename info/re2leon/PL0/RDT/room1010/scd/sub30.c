int sub30(void) {

Work_set(3, 0);
nop();
Plc_neck(5, 3, 0, 1024, 0, -32640);
Plc_dest(0, 5409, -18431, 16384);
Plc_rot(0, 16384);
goto Sub21();
Plc_motion(0, 0, 8);
Sleep(1024);
Plc_neck(4, 4, 0, 0, 0, -32640);
Sleep(1024);
Plc_motion(0, 11, 8);
Plc_neck(5, 3, 0, 1024, 0, -32640);
Sleep(1792);
Evt_exec(255, 0x181F);
Plc_motion(0, 12, 0);
return 0;
}