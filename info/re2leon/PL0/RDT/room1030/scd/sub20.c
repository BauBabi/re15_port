int sub20(void) {

	if(Ck(6, 195, 1)) {
		Ck(5, 20, 0)
		Set(5, 20, 1);
		Aot_reset(31, 2, 49, 21248, 256, 0x9C00);
	}
	Evt_next();
	nop();
	goto(0x0000E2FF);
	return 0;
}