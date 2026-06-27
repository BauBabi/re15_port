int sub10(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Set(2, 2, 1);
	Sleep(5120);
	Save(4, 0);
	Aot_reset(4, 0, 0, 0, 0, 0x00);
	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Aot_reset(6, 0, 0, 0, 0, 0x00);
	Sleep(7680);
	Cut_chg(0x03);
	Sleep(7680);
	goto Sub11();
	Sleep(2560);
	Se_on(2, 13, 0, 0, 0, 0, 0, 0);
	Work_set(4, 0);
	nop();
	Pos_set(0, 21176, 0, 31896);
	Dir_set(0, 4, 0, 0);
	Aot_reset(1, 2, 49, 18944, 256, 0x6C00);
	Sleep(2560);
	Work_set(1, 0);
	nop();
	Dir_set(0, 0, 252, 0);
	Cut_old();
	Work_set(1, 0);
	Plc_dest(0, 5409, 15602, -22016);
	goto Sub12();
	Sleep(2560);
	Set(4, 75, 1);
	Aot_reset(9, 5, 65, 65280, 6159, 0x00);
	Aot_reset(10, 5, 65, 65280, 6159, 0x00);
	goto Sub13();
	Set(2, 2, 0);
	return 0;
}