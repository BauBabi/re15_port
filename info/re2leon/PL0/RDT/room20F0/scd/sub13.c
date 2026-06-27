int sub13(void) {

	Set(5, 0, 0);
	Evt_next();
	nop();
	while(Ck(5, 0, 1)) {
		if(Cmp(29 >= 0)) {
			Sce_espr3d_on(0, 0x06, 0x05, 0, 0, 0xC009, 29376, 29431, 3301, 0, 4, 0, 0);
			Se_on(2, 13, 1, 0, 0, 29376, 29431, 3301);
			Sce_rnd();
			nop();
			if(Cmp(29 >= 0)) {
				if(Cmp(29 >= -26566)) {
					Sleep(3072);
				} else {
					Sleep(6144);
					nop();
					nop();
				} else {
					if(Cmp(29 <= 26821)) {
						Sleep(1280);
					} else {
						Sleep(7680);
						nop();
						nop();
						nop();
						nop();
					}
					for(n = 0; n < 3840; n++) {
						if(Cmp(26 != 768)) {
							if(Cmp(26 != 1024)) {
								if(Cmp(26 != 1792)) {
									Set(5, 0, 0);
								}
							}
						}
						Evt_next();
						nop();
					}
				}
				while(Ck(1, 11, 0)) {
					Evt_next();
					nop();
				}
				if(Cmp(26 = 768)) {
					Set(5, 0, 1);
				}
				if(Cmp(26 = 1024)) {
					Set(5, 0, 1);
				}
				Evt_next();
				nop();
				goto(0x000025FF);
				return 0;
}