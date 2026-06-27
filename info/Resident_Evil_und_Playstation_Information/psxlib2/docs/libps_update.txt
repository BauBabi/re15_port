PSX MIPSGCC Library Update
~~~~~~~~~~~~~~~~~~~~~~~~~~

23 April 2000


This is a combination of a slightly updated version of Rob Withey's
PSX library for MIPSGCC and my own PSX SPU library.

New features:
- Text/Font handling stuff.				* done
- Added a few more primitives (eg: dot, 8x8 block)	* done
- Integrated my SPU lib (version 2)			* done


Tools:
1. bin2src (updated, user-friendly version :)
2. bin2o
3. modconv (MOD to HIT converter)
4. bmp/pcx to tim converter (Sony's is nice :)
5. wav to vag converter (friendly version)



Details:
~~~~~~~~

5 new GPU functions:

	1. GPU_SortDot
	2. GPU_SortRect8
	3. GPU_SortRect16
	4. GPU_SortSprite8
	5. GPU_SortSprite16

PALorNTSC function:

// (returns 1 for PAL, 0 for NTSC)
int GPU_PALorNTSC(void);

+ GTE_AverageZ3 & GTE_AverageZ4 fixed.


Font/Text functions:

1. InitFont(TIMHEADER *fontTIM, int dx, int dy, int n);
2. SetCharSize(PsxUInt8 width, PsxUInt8 height);
3. SetCharLight(PsxUInt8 r, PsxUInt8 g, PsxUInt8 b);
4. PrintChar(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos, PsxUInt16 chr);
5. PrintString(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos, char *string);


SPU functions:

1. Control SPU general parameters.
2. Control SPU voice parameters.
3. Upload and play samples.
4. Upload VAB and play VAB samples.
5. Set and read ADSR envelopes.
6. Noise and FM modes (not properly tested).
7. 4/6/8 track MOD player (plays HITMOD format MOD's)
8. Some other stuff.
