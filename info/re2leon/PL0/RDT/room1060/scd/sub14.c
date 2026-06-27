int sub14(void) {

	Work_set(4, 2);
	nop();
	Speed_set(3, 25600);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}