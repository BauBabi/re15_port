int sub01(void) {

	if(Ck(1, 1, 1)) {
		Ck(4, 79, 1)
		Ck(4, 128, 0)
		if(Ck(6, 197, 1)) {
			Ck(5, 20, 0)
			if(Ck(6, 200, 0)) {
				Evt_exec(255, 0x1802);
				Set(6, 200, 1);
			}
			Sce_bgm_control(0, 5, 0, 0, 0);
			Set(5, 20, 1);
		}
	}
	return 0;
}