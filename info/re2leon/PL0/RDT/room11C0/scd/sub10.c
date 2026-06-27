int sub10(void) {

	Work_set(4, 0);
	nop();
	Speed_set(1, 1280);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, -1025);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, 1280);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, 1536);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, -1281);
	for(n = 0; n < 15360; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, 1536);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, 1792);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, -1537);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(1, 1792);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	goto(0x00006FFF);
	return 0;
}