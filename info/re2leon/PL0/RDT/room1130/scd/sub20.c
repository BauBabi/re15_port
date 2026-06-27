int sub20(void) {

	Work_set(1, 0);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Sleep(1280);
	Plc_dest(0, 2336, 29337, -21071);
	goto Sub27();
	Plc_motion(0, 23, 0);
	Wsleep();
	Wsleeping();
	return 0;
}