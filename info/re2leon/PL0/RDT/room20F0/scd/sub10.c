int sub10(void) {

	if(Ck(1, 1, 0)) {
		if(Ck(4, 193, 1)) {
			goto Sub11();
			Set(5, 2, 1);
		} else {
			if(Ck(4, 153, 1)) {
				if(Ck(29, 10, 0)) {
					goto Sub11();
					Set(5, 2, 1);
				}
			}
			nop();
			nop();
		} else {
			if(Ck(4, 153, 1)) {
				if(Ck(8, 119, 0)) {
					if(Ck(29, 10, 1)) {
						goto Sub11();
						Set(5, 2, 1);
					} else {
						if(Ck(29, 9, 0)) {
							goto Sub11();
							Set(5, 2, 1);
						}
						nop();
						nop();
					} else {
						if(Ck(29, 10, 1)) {
							goto Sub12();
							Set(5, 2, 1);
						} else {
							if(Ck(29, 9, 0)) {
								goto Sub12();
								Set(5, 2, 1);
							}
							nop();
							nop();
							nop();
							nop();
						} else {
							Sce_em_set(0, 0, 0x20, 4, 0, 0, 12, 0, 94, 6076, 0, 17867, 8196, 0, 0);
							Set(5, 1, 1);
							nop();
							nop();
							nop();
							nop();
							return 0;
}