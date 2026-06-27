int sub07(void) {

	if(Ck(8, 55, 0)) {
		if(Ck(1, 0, 0)) {
			Aot_reset(2, 2, 49, 15616, 256, 0x3700);
			Work_set(4, 9);
			nop();
			Pos_set(0, 12430, 4836, 2195);
			Dir_set(0, 16628, 16624, -32764);
		} else {
			Aot_reset(2, 2, 49, 14592, 256, 0x3700);
			Work_set(4, 9);
			nop();
			Pos_set(0, 12430, 4836, 2195);
			Dir_set(0, 16628, 16624, -32764);
			nop();
			nop();
		}
		return 0;
}