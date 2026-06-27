int sub26(void) {

	Sleep(2560);
	Work_set(3, 0);
	Plc_motion(0, 24, 16);
	nop();
	Sleep(3840);
	Plc_neck(5, 1, 0, 0, 0, 4112);
	Sleep(6400);
	Xa_on(0, 6656);
	Sleep(512);
	Message_on(0, 1536, 0x0000);
	Sleep(768);
	Plc_neck(2, 0, 0, 0, -32768, 8224);
	Sleep(1280);
	Plc_neck(2, 0, 0, 0, 0, 8224);
	Wsleep();
	Wsleeping();
	Work_set(1, 0);
	nop();
	Plc_neck(2, 0, 0, 0, -32768, 5140);
	Sleep(1536);
	Plc_neck(5, 3, 0, 0, 0, 5140);
	return 0;
}