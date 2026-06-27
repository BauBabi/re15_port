int sub34(void) {

	Work_set(4, 5);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	Work_set(4, 11);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	Work_set(1, 0);
	nop();
	Member_set(43, -32768);
	Plc_motion(1, 1, 0);
	Sleep(1536);
	Plc_stop();
	nop();
	Plc_neck(2, 0, 0, 254, 0, 16448);
	Sleep(5120);
	Plc_neck(2, 0, 0, 2, 0, 10280);
	Sleep(10240);
	return 0;
}