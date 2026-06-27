int sub00(void) {

	Evt_exec(255, 0x180B);
	Evt_exec(255, 0x180C);
	Evt_exec(255, 0x1803);
	Evt_exec(255, 0x1802);
	if(Ck(4, 27, 0)) {
		Aot_reset(0, 5, 49, 65280, 6153, 0x00);
		Aot_set(5, 5, 65, 0, 0, 15272, -7356, -32756, 28695, 255, 0, 24);
	}
	return 0;
}