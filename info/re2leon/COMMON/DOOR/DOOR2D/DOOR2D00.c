int DOOR2D00(void) {

	switch(12) {
	case 0:
	case 512:
	case 1024:
		goto Sub01();
	break;
	case 256:
	case 768:
	case 1280:
		goto Sub02();
	break;
	}
	return 0;
}