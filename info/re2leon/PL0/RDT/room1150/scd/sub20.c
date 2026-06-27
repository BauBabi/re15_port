int sub20(void) {

	Evt_next();
	Work_set(1, 0);
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Sleep(1280);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2336, 934, 9144);
	Sleep(3840);
	Xa_on(0, 5632);
	Sleep(512);
	Message_on(0, 0, 0x0000);
	Work_set(3, 0);
	Plc_motion(0, 15, 0);
	nop();
	Sleep(3840);
	Plc_neck(2, 0, 0, 0, 4, 8224);
	Sce_bgm_control(0, 1, 0, 0, 0);
	Sleep(2560);
	Plc_neck(2, 0, 0, 0, 8192, 8224);
	Wsleep();
	Wsleeping();
	return 0;
}