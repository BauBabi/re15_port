int sub14(void) {

	Set(4, 24, 0);
	Evt_next();
	Work_set(4, 4);
	Member_set(12, 251);
	Set(25, 1, 1);
	Speed_set(1, 0);
	Speed_set(7, -1025);
	Se_on(2, 13, 1, 4, 4, 0, 0, 0);
	while(Member_cmp(12 >= 17656)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, 17656);
	Set(25, 1, 0);
	return 0;
}