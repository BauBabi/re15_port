int sub06(void) {

	if(Ck(1, 1, 0)) {
		Message_on(0, 1024, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(4, 143, 0)) {
			Message_on(0, 1280, 0xFFFF);
			Evt_next();
			nop();
			if(Ck(11, 31, 0)) {
				goto Sub08();
				if(Ck(4, 91, 0)) {
					Se_on(2, 18, 0, 1, 0, 0, 0, 0);
					Message_on(0, 1536, 0xFFFF);
					Evt_next();
					nop();
					goto Sub09();
					goto(0x00007C02);
				} else {
					Se_on(2, 19, 0, 1, 0, 0, 0, 0);
					Message_on(0, 1792, 0xFFFF);
					Evt_next();
					Set(29, 2, 1);
					Set(4, 143, 1);
					Sce_espr_kill2(2);
					nop();
					Sce_espr_on2(0x02, 0x16, 24, 0, 0, 0x0A, 31260, 9976, 27286, 0);
					goto Sub09();
					nop();
					nop();
					Message_on(0, 2048, 0xFFFF);
					Evt_next();
					nop();
					if(Ck(11, 31, 0)) {
						goto Sub08();
						Se_on(2, 18, 0, 1, 0, 0, 0, 0);
						Message_on(0, 2304, 0xFFFF);
						Evt_next();
						nop();
						goto Sub09();
					} else {
						Se_on(4, 5, 0, 1, 0, 0, 0, 0);
						nop();
						nop();
					} else {
						Se_on(4, 5, 0, 1, 0, 0, 0, 0);
						nop();
						nop();
					} else {
						Message_on(0, 2048, 0xFFFF);
						Evt_next();
						nop();
						if(Ck(11, 31, 0)) {
							goto Sub08();
							Se_on(2, 18, 0, 1, 0, 0, 0, 0);
							Message_on(0, 2304, 0xFFFF);
							Evt_next();
							nop();
							goto Sub09();
						} else {
							Se_on(4, 5, 0, 1, 0, 0, 0, 0);
							nop();
							nop();
							nop();
							nop();
						} else {
							if(Ck(4, 142, 0)) {
								Message_on(0, 1024, 0xFFFF);
								Evt_next();
								nop();
							}
							if(Ck(29, 2, 0)) {
								if(Ck(4, 143, 0)) {
									Message_on(0, 1280, 0xFFFF);
									Evt_next();
									nop();
									if(Ck(11, 31, 0)) {
										goto Sub08();
										if(Ck(4, 91, 0)) {
											Se_on(2, 18, 0, 1, 0, 0, 0, 0);
											Message_on(0, 1536, 0xFFFF);
											Evt_next();
											nop();
											goto Sub09();
										} else {
											Se_on(2, 19, 0, 1, 0, 0, 0, 0);
											Message_on(0, 1792, 0xFFFF);
											Evt_next();
											Set(4, 143, 1);
											Sce_espr_kill2(2);
											nop();
											Sce_espr_on2(0x02, 0x16, 24, 0, 0, 0x0A, 31260, 9976, 27286, 0);
											goto Sub09();
											nop();
											nop();
											Message_on(0, 2048, 0xFFFF);
											Evt_next();
											nop();
											if(Ck(11, 31, 0)) {
												goto Sub08();
												Se_on(2, 18, 0, 1, 0, 0, 0, 0);
												Message_on(0, 2304, 0xFFFF);
												Evt_next();
												nop();
												goto Sub09();
											} else {
												Se_on(4, 5, 0, 1, 0, 0, 0, 0);
												nop();
												nop();
											} else {
												Se_on(4, 5, 0, 1, 0, 0, 0, 0);
												nop();
												nop();
											} else {
												Message_on(0, 2048, 0xFFFF);
												Evt_next();
												nop();
												if(Ck(11, 31, 0)) {
													goto Sub08();
													Se_on(2, 18, 0, 1, 0, 0, 0, 0);
													Message_on(0, 2304, 0xFFFF);
													Evt_next();
													nop();
													goto Sub09();
												} else {
													Se_on(4, 5, 0, 1, 0, 0, 0, 0);
													nop();
													nop();
													nop();
													nop();
												} else {
													if(Ck(4, 143, 0)) {
														Message_on(0, 2048, 0xFFFF);
														Evt_next();
														nop();
														if(Ck(11, 31, 0)) {
															goto Sub08();
															if(Ck(4, 91, 1)) {
																Se_on(2, 19, 0, 1, 0, 0, 0, 0);
																Message_on(0, 2560, 0xFFFF);
																Evt_next();
																Set(4, 143, 1);
																Set(4, 142, 1);
																Sce_espr_kill2(2);
																nop();
																Sce_espr_on2(0x02, 0x16, 8, 0, 0, 0x0C, 5150, 12536, -25450, 0);
																Aot_reset(3, 1, 49, 8886, 0, 0xA4C5);
																goto Sub09();
															} else {
																Se_on(2, 18, 0, 1, 0, 0, 0, 0);
																Message_on(0, 1536, 0xFFFF);
																Evt_next();
																nop();
																goto Sub09();
																nop();
																nop();
															} else {
																Se_on(4, 5, 0, 1, 0, 0, 0, 0);
																nop();
																nop();
															} else {
Message_on(0, 2816, 0xFFFF);
Evt_next();
nop();
nop();
nop();
nop();
nop();
nop();
nop();
Work_set(1, 0);
Plc_ret();
return 0;
}