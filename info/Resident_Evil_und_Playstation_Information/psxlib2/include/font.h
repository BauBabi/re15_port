// font.h
// PSX lib font header file for font.c


void InitFont(TIMHEADER *fontTIM, int sx, int sy, int n);

void SetCharSize(PsxUInt8 width, PsxUInt8 height);

void SetCharLight(PsxUInt8 r, PsxUInt8 g, PsxUInt8 b);

void PrintChar(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos, PsxUInt16 chr);

void PrintString(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos, char *string);


// may have to move later...
void UpLoadImage(TIMHEADER *timhdr);
