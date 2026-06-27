int sub17(void) {

	Aot_reset(0, 1, 49, 18846, 0, 0x18F5);
	if(Ck(5, 0, 0)) {
		Evt_kill(5);
	}
	Sleep(1280);
	Aot_on(0);
	return 0;
}