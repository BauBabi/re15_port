int sub08(void) {

	Work_set(1, 0);
	nop();
	goto Sub16();
	goto Sub16();
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Plc_dest(0, 1057, 3796, 15086);
	Plc_neck(0, 1, 0, 0, 0, 24576);
	goto Sub17();
	Plc_dest(0, 2337, 24816, 9441);
	goto Sub17();
	Plc_motion(10, 30, 0);
	Plc_flg(0, 0, 1);
	Plc_cnt(1);
	Sleep(2560);
	Member_copy(16, 0);
	nop();
	Calc(16 || 4)
	Member_set2(0, 16);
	nop();
	Sleep(8704);
	Se_on(2, 12, 1, 4, 0, 0, 0, 0);
	Sleep(17920);
	Cut_chg(0x06);
	Sleep(22016);
	Sce_fade_set(2, 2, 7, 128, 1);
	Sleep(7680);
	Set(5, 0, 1);
	return 0;
}