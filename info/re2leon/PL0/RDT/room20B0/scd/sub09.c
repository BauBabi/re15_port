int sub09(void) {

	Work_set(4, 0);
	nop();
	if(Cmp(26 = 256)) {
		Pos_set(0, 8270, 8270, 8270);
	} else {
		Pos_set(0, 25813, 32491, 6315);
		nop();
		nop();
		Speed_set(4, 14080);
		Add_speed();
		Work_set(4, 1);
		Speed_set(4, 13056);
		Add_speed();
		Evt_next();
		goto(0x0000CEFF);
		return 0;
}