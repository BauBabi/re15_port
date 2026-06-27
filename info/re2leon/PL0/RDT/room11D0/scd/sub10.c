int sub10(void) {

	Sleep(256);
	Work_set(1, 0);
	nop();
	Plc_neck(2, 0, 0, 0, -32513, 20560);
	Sleep(5120);
	Plc_neck(1, 217, 159, 12536, -17827, 20560);
	Sleep(20480);
	Plc_neck(0, 0, 0, 0, 0, 7710);
	Sleep(15360);
	Plc_ret();
	nop();
	return 0;
}