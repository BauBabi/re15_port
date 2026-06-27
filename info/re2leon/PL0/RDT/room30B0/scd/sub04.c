int sub04(void) {

	Work_set(2, 0);
	nop();
	Pos_set(0, -31294, 0, 10477);
	Dir_set(0, 0, 4, 0);
	Work_set(1, 0);
	nop();
	Pos_set(0, 21960, 0, -17175);
	Dir_set(0, 0, -4081, 0);
	Set(2, 7, 1);
	Cut_chg(0x02);
	Sleep(2560);
	Set(1, 27, 1);
	Evt_exec(255, 0x1805);
	Evt_exec(255, 0x1806);
	return 0;
}