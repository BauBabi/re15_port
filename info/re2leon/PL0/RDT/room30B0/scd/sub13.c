int sub13(void) {

	Work_set(4, 0);
	nop();
	Sleep(5888);
	Se_on(2, 17, 1, 4, 0, 0, 0, 0);
	Pos_set(0, -4895, 18676, -14355);
	Dir_set(0, 0, 0, 6);
	Speed_set(0, 19711);
	Speed_set(1, -770);
	Speed_set(6, -7425);
	Speed_set(7, -4865);
	for(n = 0; n < 1536; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Dir_set(0, -32767, 0, -16370);
	Speed_set(0, 19711);
	Speed_set(1, 30720);
	Speed_set(6, -7425);
	Speed_set(7, 15360);
	for(n = 0; n < 2816; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Pos_set(0, 3277, 0, 1774);
	Se_on(2, 13, 1, 4, 0, 0, 0, 0);
	Sleep(256);
	Pos_set(0, -7989, 0, 27374);
	Sleep(256);
	Pos_set(0, 31947, 0, -23827);
	if(Ck(1, 1, 1)) {
		Aot_reset(4, 2, 49, 23552, 768, 0x7C00);
	} else {
		Aot_reset(4, 2, 49, 23552, 768, 0x7C00);
		nop();
		nop();
		return 0;
}