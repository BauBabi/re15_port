int sub09(void) {

Work_set(3, 0);
nop();
Plc_dest(0, 1057, -2877, 457);
goto Sub21();
Plc_dest(0, 2337, -11307, -27960);
goto Sub21();
Plc_motion(0, 0, 0);
Sleep(5120);
Se_on(2, 10, 1, 0, 0, -10045, -1800, -14648);
Sleep(2560);
Plc_motion(0, 0, 192);
Sleep(1280);
Plc_motion(0, 0, 64);
Plc_cnt(10);
Sleep(1280);
Plc_stop();
nop();
Sleep(6400);
Plc_dest(0, 2337, 1985, 16334);
goto Sub21();
Plc_motion(0, 4, 0);
Plc_neck(2, 0, 0, 0, 2, 23130);
Set(5, 2, 1);
return 0;
}