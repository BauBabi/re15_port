int sub24(void) {

	while(Ck(5, 19, 1)) {
		Sce_espr_kill(22, 5, 0, 0);
		nop();
		Sce_espr3d_on(0, 0x16, 0x05, 0, 0, 0x08, -7509, 30448, -30495, 0, 4, 0, 0);
		Sleep(512);
		Sce_espr_kill(22, 5, 0, 0);
		nop();
		Sce_espr3d_on(0, 0x16, 0x05, 0, 0, 0x12, -7509, 30448, -30495, 0, 4, 0, 0);
		Sleep(512);
		Sce_espr_kill(22, 5, 0, 0);
		nop();
		Sce_espr3d_on(0, 0x16, 0x05, 0, 0, 0x14, -7509, 30448, -30495, 0, 4, 0, 0);
		Sleep(512);
		Sce_espr_kill(22, 5, 0, 0);
		nop();
		Sce_espr3d_on(0, 0x16, 0x05, 0, 0, 0x12, -7509, 30448, -30495, 0, 4, 0, 0);
		Sleep(512);
	}
	return 0;
}