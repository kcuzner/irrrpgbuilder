#include "convert.h"

//Tries to convert  accented characters from UT8
irr::core::stringw convert(irr::core::stringc text)
{
	irr::core::stringw line = "";
	for (int a=0; a<(int)text.size(); a++)
	{
		// Found out that -61 is the marker for a accent
		// The next character define what type of accent
		//if ((int)text[a]<0 && (int)text[a]!=-61)
			//printf("Found another control code: %d\n",(int)text[a]);
		if ((int)text[a]==-30)
		{
			// Euro Symbol (Might not be available in the font)
			if ((int)text[a+1]==-126 && (int)text[a+2]==-84) line+=L"€";
			a++;
			a++;
		}
		else if ((int)text[a]==-59)
		{
			// Special codes
			//printf("\nAccent found: 1:-59, 2:%d\n",(int)text[a+1]);
			if ((int)text[a+1]==-72)	line+=L"Ÿ";
			if ((int)text[a+1]==-109)	line+=L"œ";
			if ((int)text[a+1]==-110)	line+=L"Œ";
			a++;
		}
		else if ((int)text[a]==-61)
		{
			// Extended character set
			//printf("\nAccent found: 1:-61, 2:%d\n",(int)text[a+1]);
			if ((int)text[a+1]==-65)	line+=L"ÿ";
			if ((int)text[a+1]==-68)	line+=L"ü";
			if ((int)text[a+1]==-69)	line+=L"û";
			if ((int)text[a+1]==-70)	line+=L"ú";
			if ((int)text[a+1]==-71)	line+=L"ù";
			if ((int)text[a+1]==-74)	line+=L"ö";
			if ((int)text[a+1]==-75)	line+=L"õ";
			if ((int)text[a+1]==-76)	line+=L"ô";
			if ((int)text[a+1]==-77)	line+=L"ó";
			if ((int)text[a+1]==-81)	line+=L"ï";
			if ((int)text[a+1]==-82)	line+=L"î";
			if ((int)text[a+1]==-83)	line+=L"í";
			if ((int)text[a+1]==-84)	line+=L"ì";
			if ((int)text[a+1]==-85)	line+=L"ë";
			if ((int)text[a+1]==-86)	line+=L"ê";
			if ((int)text[a+1]==-87)	line+=L"é";
			if ((int)text[a+1]==-88)	line+=L"è";
			if ((int)text[a+1]==-89)	line+=L"ç";
			if ((int)text[a+1]==-92)	line+=L"ä";
			if ((int)text[a+1]==-93)	line+=L"ã";
			if ((int)text[a+1]==-94)	line+=L"â";
			if ((int)text[a+1]==-95)	line+=L"á";
			if ((int)text[a+1]==-96)	line+=L"à";
			if ((int)text[a+1]==-97)	line+=L"ß";
			if ((int)text[a+1]==-100)	line+=L"Ü";
			if ((int)text[a+1]==-101)	line+=L"Û";
			if ((int)text[a+1]==-102)	line+=L"Ú";
			if ((int)text[a+1]==-103)	line+=L"Ù";
			if ((int)text[a+1]==-106)	line+=L"Ö";
			if ((int)text[a+1]==-107)	line+=L"Õ";
			if ((int)text[a+1]==-108)	line+=L"Ô";
			if ((int)text[a+1]==-109)	line+=L"Ò";
			if ((int)text[a+1]==-113)	line+=L"Ï";
			if ((int)text[a+1]==-114)	line+=L"Î";
			if ((int)text[a+1]==-115)	line+=L"Í";
			if ((int)text[a+1]==-116)	line+=L"Ì";
			if ((int)text[a+1]==-117)	line+=L"Ë";
			if ((int)text[a+1]==-118)	line+=L"Ê";
			if ((int)text[a+1]==-119)	line+=L"É";
			if ((int)text[a+1]==-120)	line+=L"È";
			if ((int)text[a+1]==-121)	line+=L"Ç";
			if ((int)text[a+1]==-124)	line+=L"Ä";
			if ((int)text[a+1]==-125)	line+=L"Ã";
			if ((int)text[a+1]==-126)	line+=L"Â";
			if ((int)text[a+1]==-127)	line+=L"Á";
			if ((int)text[a+1]==-128)	line+=L"À";
			a++;
		}
		else
		{
			line.append(text[a]);
		}
		//printf ("%c",text[a]);
	}
	return line;
}