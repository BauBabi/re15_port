int sub23(void) {

	Xa_on(0, 5888);
	Sleep(2304);
	Message_on(0, 256, 0x0000);
	Sleep(2560);
	Work_set(3, 0);
	Plc_motion(0, 24, 0);
	nop();
	Sleep(1280);
	Work_set(1, 0);
	Plc_motion(0, 23, 0);
	Plc_cnt(13);
	nop();
	Sleep(2560);
	Plc_neck(5, 3, 0, 0, 0, 4112);
	Plc_motion(0, 16, 0);
	Sleep(6400);
	Plc_neck(2, 0, 0, 0, -24576, 8224);
	Sleep(1280);
	Plc_neck(5, 3, 0, 0, 0, 8224);
	Sleep(2048);
	Work_set(3, 0);
	Plc_motion(0, 24, 0);
	Plc_cnt(13);
	nop();
	Wsleep();
	Wsleeping();
	return 0;
}