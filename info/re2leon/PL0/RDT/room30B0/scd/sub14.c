int sub14(void) {

	Work_set(4, 1);
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
	Se_on(2, 9, 1, 4, 0, 0, 0, 0);
	Dir_set(0, -8192, 0, -16370);
	Sleep(256);
	Pos_set(0, -7989, 0, 27374);
	Dir_set(0, 3074, 0, -16370);
	Sleep(256);
	Pos_set(0, 31947, 32511, -23827);
	Dir_set(0, 4, 0, -16370);
	Aot_reset(5, 2, 49, 5376, 1792, 0xC100);
	return 0;
}