int sub04(void) {

	if(Ck(1, 0, 0)) {
		if(Ck(4, 61, 0)) {
			if(Ck(26, 0, 4)) {
				Work_set(1, 0);
				nop();
				if(Member_cmp(9 = 2304)) {
					Work_set(3, 1);
					nop();
					if(Ck(6, 80, 0)) {
						Set(4, 61, 1);
						Evt_exec(255, 0x180B);
					}
				}
			}
		} else {
			if(Ck(4, 114, 0)) {
				if(Ck(6, 80, 0)) {
					Work_set(3, 1);
					nop();
					if(Member_cmp(23 = 256)) {
						Member_set(23, 0);
						if(Member_cmp(9 = 1280)) {
							if(Ck(5, 3, 0)) {
								Set(5, 3, 1);
								Evt_exec(255, 0x180C);
							}
						}
					}
				}
				if(Ck(6, 79, 0)) {
					Work_set(3, 0);
					nop();
					if(Member_cmp(23 = 256)) {
						Member_set(23, 0);
						if(Member_cmp(9 = 1536)) {
							if(Ck(5, 4, 0)) {
								Set(5, 4, 1);
								Evt_exec(255, 0x180D);
							}
						}
					}
				}
			}
			nop();
			nop();
			if(Ck(4, 114, 1)) {
				if(Ck(6, 79, 1)) {
					if(Ck(6, 80, 1)) {
						if(Ck(5, 1, 0)) {
							if(Ck(5, 0, 0)) {
								Set(5, 0, 1);
								Sce_bgm_control(1, 5, 0, 0, 0);
							}
						}
					}
				}
			}
		}
		if(Ck(4, 59, 0)) {
			if(Cmp(0 = 2816)) {
				if(Cmp(2 = 19200)) {
					Set(4, 59, 1);
					Evt_exec(255, 0x180E);
				}
			}
		}
		if(Ck(6, 80, 1)) {
			Aot_reset(7, 0, 0, 0, 0, 0x00);
		}
		if(Ck(6, 79, 1)) {
			Aot_reset(8, 0, 0, 0, 0, 0x00);
		}
		if(Ck(4, 114, 1)) {
			Aot_reset(7, 0, 0, 0, 0, 0x00);
			Aot_reset(8, 0, 0, 0, 0, 0x00);
		}
		return 0;
}