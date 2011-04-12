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
			if ((int)text[a+1]==-126 && (int)text[a+2]==-84)line+=irr::core::stringw("€");
			a++;
			a++;
		}
		else if ((int)text[a]==-59)
		{
			// Special codes
			//printf("\nAccent found: 1:-59, 2:%d\n",(int)text[a+1]);
			if ((int)text[a+1]==-72)	line+=irr::core::stringw("Ÿ");
			if ((int)text[a+1]==-109)	line+=irr::core::stringw("œ");
			if ((int)text[a+1]==-110)	line+=irr::core::stringw("Œ");
			a++;
		}
		else if ((int)text[a]==-61)
		{
			// Extended character set
			//printf("\nAccent found: 1:-61, 2:%d\n",(int)text[a+1]);
			//printf ("\nAccent found: 1:-61, 2:%x",(int)text[a+1]);
			if ((int)text[a+1]==-65)	line+=irr::core::stringw("ÿ");
			if ((int)text[a+1]==-68)	line+=irr::core::stringw("ü");
			if ((int)text[a+1]==-69)	line+=irr::core::stringw("û");
			if ((int)text[a+1]==-70)	line+=irr::core::stringw("ú");
			if ((int)text[a+1]==-71)	line+=irr::core::stringw("ù");
			if ((int)text[a+1]==-74)	line+=irr::core::stringw("ö");
			if ((int)text[a+1]==-75)	line+=irr::core::stringw("õ");
			if ((int)text[a+1]==-76)	line+=irr::core::stringw("ô");
			if ((int)text[a+1]==-77)	line+=irr::core::stringw("ó");
			if ((int)text[a+1]==-81)	line+=irr::core::stringw("ï");
			if ((int)text[a+1]==-82)	line+=irr::core::stringw("î");
			if ((int)text[a+1]==-83)	line+=irr::core::stringw("í");
			if ((int)text[a+1]==-84)	line+=irr::core::stringw("ì");
			if ((int)text[a+1]==-85)	line+=irr::core::stringw("ë");
			if ((int)text[a+1]==-86)	line+=irr::core::stringw("ê");
			if ((int)text[a+1]==-87)	line+=irr::core::stringw("é");
			if ((int)text[a+1]==-88)	line+=irr::core::stringw("è");
			if ((int)text[a+1]==-89)	line+=irr::core::stringw("ç");
			if ((int)text[a+1]==-92)	line+=irr::core::stringw("ä");
			if ((int)text[a+1]==-93)	line+=irr::core::stringw("ã");
			if ((int)text[a+1]==-94)	line+=irr::core::stringw("â");
			if ((int)text[a+1]==-95)	line+=irr::core::stringw("á");
			if ((int)text[a+1]==-96)	line+=irr::core::stringw("à");
			if ((int)text[a+1]==-97)	line+=irr::core::stringw("ß");
			if ((int)text[a+1]==-100)	line+=irr::core::stringw("Ü");
			if ((int)text[a+1]==-101)	line+=irr::core::stringw("Û");
			if ((int)text[a+1]==-102)	line+=irr::core::stringw("Ú");
			if ((int)text[a+1]==-103)	line+=irr::core::stringw("Ù");
			if ((int)text[a+1]==-106)	line+=irr::core::stringw("Ö");
			if ((int)text[a+1]==-107)	line+=irr::core::stringw("Õ");
			if ((int)text[a+1]==-108)	line+=irr::core::stringw("Ô");
			if ((int)text[a+1]==-109)	line+=irr::core::stringw("Ò");
			if ((int)text[a+1]==-113)	line+=irr::core::stringw("Ï");
			if ((int)text[a+1]==-114)	line+=irr::core::stringw("Î");
			if ((int)text[a+1]==-115)	line+=irr::core::stringw("Í");
			if ((int)text[a+1]==-116)	line+=irr::core::stringw("Ì");
			if ((int)text[a+1]==-117)	line+=irr::core::stringw("Ë");
			if ((int)text[a+1]==-118)	line+=irr::core::stringw("Ê");
			if ((int)text[a+1]==-119)	line+=irr::core::stringw("É");
			if ((int)text[a+1]==-120)	line+=irr::core::stringw("È");
			if ((int)text[a+1]==-121)	line+=irr::core::stringw("Ç");
			if ((int)text[a+1]==-124)	line+=irr::core::stringw("Ä");
			if ((int)text[a+1]==-125)	line+=irr::core::stringw("Ã");
			if ((int)text[a+1]==-126)	line+=irr::core::stringw("Â");
			if ((int)text[a+1]==-127)	line+=irr::core::stringw("Á");
			if ((int)text[a+1]==-128)	line+=irr::core::stringw("À");
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
