int sub14(void) {

	Work_set(1, 0);
	nop();
	Plc_neck(2, 0, 0, -16130, 0, 6430);
	Sleep(3584);
	Plc_neck(2, 0, 0, -30465, 0, 6430);
	Sleep(2048);
	Plc_neck(2, 0, 0, -16130, 0, 6430);
	Sleep(2048);
	return 0;
}