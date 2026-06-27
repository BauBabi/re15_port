int sub21(void) {

	Cut_chg(0x05);
	Work_set(3, 0);
	nop();
	Plc_dest(0, 2337, 672, 14519);
	Plc_neck(5, 1, 0, 0, 0, 24672);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 1056, 934, 9144);
	Sleep(5120);
	Work_set(3, 0);
	nop();
	Plc_dest(0, 1057, 672, 14519);
	Plc_neck(5, 1, 0, 0, 0, 24672);
	Cut_chg(0x02);
	Work_set(1, 0);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 24672);
	goto Sub30();
	return 0;
}