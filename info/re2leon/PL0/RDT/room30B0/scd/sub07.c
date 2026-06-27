int sub07(void) {

	Cut_chg(0x03);
	Work_set(2, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Plc_dest(0, 1056, 13526, 2286);
	Plc_neck(0, 96, 215, 0, 12511, 0);
	goto Sub16();
	Plc_neck(1, 96, 215, 0, 12511, 0);
	Sleep(2560);
	Plc_dest(0, 2336, 15606, -22275);
	goto Sub16();
	Plc_neck(0, 0, 0, 0, 0, 8224);
	goto Sub17();
	goto Sub17();
	Plc_motion(10, 20, 0);
	Plc_flg(0, 0, 1);
	Plc_cnt(1);
	Sleep(23040);
	return 0;
}