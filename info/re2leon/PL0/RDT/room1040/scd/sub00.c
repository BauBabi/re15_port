int sub00(void) {

	Set(5, 0, 1);
	Evt_exec(255, 0x1803);
	if(Cmp(26 = 0)) {
		Evt_exec(255, 0x1802);
	}
	return 0;
}