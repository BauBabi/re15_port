int sub05(void) {

	if(Ck(5, 0, 1)) {
		if(Ck(5, 7, 0)) {
			if(Cmp(26 != 1280)) {
				if(Cmp(26 != 512)) {
					if(Cmp(26 != 2304)) {
						Set(5, 7, 1);
						Work_set(3, 0);
						nop();
						Pos_set(0, -8273, 0, 14006);
						Dir_set(0, 0, 16635, 0);
					}
				}
			}
		}
	}
	if(Ck(5, 7, 1)) {
		if(Ck(5, 8, 0)) {
			if(Cmp(26 = 512)) {
				Set(5, 8, 1);
				Evt_exec(255, 0x1806);
			}
		}
	}
	return 0;
}