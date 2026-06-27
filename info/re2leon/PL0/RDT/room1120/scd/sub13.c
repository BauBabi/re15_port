int sub13(void) {

	Cut_chg(0x0D);
	Set(2, 7, 1);
	Evt_next();
	nop();
	Se_on(2, 10, 1, 4, 4, 0, 0, 0);
	Work_set(4, 4);
	nop();
	Speed_set(1, 0);
	Speed_set(7, 1280);
	Sleep(1280);
	Se_on(2, 13, 1, 4, 4, 0, 0, 0);
	while(Member_cmp(12 <= 251)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, -8020);
	Sleep(3840);
	Set(2, 7, 0);
	Cut_old();
	nop();
	return 0;
}