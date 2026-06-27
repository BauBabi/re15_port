int sub12(void) {

	Aot_reset(6, 0, 0, 0, 0, 0x00);
	Cut_chg(0x07);
	Cut_auto(1);
	Work_set(1, 0);
	nop();
	Pos_set(0, 26588, 0, -15155);
	Work_set(3, 0);
	nop();
	Member_set(23, 512);
	Se_on(2, 10, 1, 3, 0, 0, 0, 0);
	Sleep(10240);
	Cut_old();
	nop();
	return 0;
}