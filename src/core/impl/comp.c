/******************************************************************************
 *                   StormWar, a Real Time Strategy game                      *
 *                   Copyright (C) 2005  LEMAIRE Michael                      *
 *----------------------------------------------------------------------------*
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                            *
 *  Read the full terms of this license in the "COPYING" file.                *
  ****************************************************************************
 *                                                                            *
 *   Platform dependent compatibility                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "core/comp.h"

#ifdef __W32
    #include <windows.h>
#endif
#include <stdlib.h>

/******************************************************************************
 *############################################################################*
 *#                              Public functions                            #*
 *############################################################################*
 ******************************************************************************/
const char*
compGuessLocalLanguage()
{
    const char* language;
    
#ifdef __W32
    LANGID id;
    
    id = GetUserDefaultLangID();
    switch (id)
    {
        case 0x0409:
        case 0x0809:
        case 0x0c09:
        case 0x1009:
        case 0x1409:
        case 0x1809:
        case 0x1c09:
        case 0x2009:
        case 0x2409:
        case 0x2809:
        case 0x2c09:
            language = "en";
            break;
        case 0x040c:
        case 0x080c:
        case 0x0c0c:
        case 0x100c:
        case 0x140c:
            language = "fr";
            break;
        case 0x0416:
        case 0x0816:
            language = "pt";
            break;
        case 0x0415:
            language = "pl";
            break;
        default:
            language = "en";
    }
/*0x0401	Arabic (Saudi Arabia)
0x0801	Arabic (Iraq)
0x0c01	Arabic (Egypt)
0x1001	Arabic (Libya)
0x1401	Arabic (Algeria)
0x1801	Arabic (Morocco)
0x1c01	Arabic (Tunisia)
0x2001	Arabic (Oman)
0x2401	Arabic (Yemen)
0x2801	Arabic (Syria)
0x2c01	Arabic (Jordan)
0x3001	Arabic (Lebanon)
0x3401	Arabic (Kuwait)
0x3801	Arabic (U.A.E.)
0x3c01	Arabic (Bahrain)
0x4001	Arabic (Qatar)
0x0402	Bulgarian
0x0403	Catalan
0x0404	Chinese (Taiwan)
0x0804	Chinese (PRC)
0x0c04	Chinese (Hong Kong)
0x1004	Chinese (Singapore)
0x0405	Czech
0x0406	Danish
0x0407	German (Standard)
0x0807	German (Swiss)
0x0c07	German (Austrian)
0x1007	German (Luxembourg)
0x1407	German (Liechtenstein)
0x0408	Greek
0x0409	English (United States)
0x0809	English (United Kingdom)
0x0c09	English (Australian)
0x1009	English (Canadian)
0x1409	English (New Zealand)
0x1809	English (Ireland)
0x1c09	English (South Africa)
0x2009	English (Jamaica)
0x2409	English (Caribbean)
0x2809	English (Belize)
0x2c09	English (Trinidad)
0x040a	Spanish (Traditional Sort)
0x080a	Spanish (Mexican)
0x0c0a	Spanish (Modern Sort)
0x100a	Spanish (Guatemala)
0x140a	Spanish (Costa Rica)
0x180a	Spanish (Panama)
0x1c0a	Spanish (Dominican Republic)
0x200a	Spanish (Venezuela)
0x240a	Spanish (Colombia)
0x280a	Spanish (Peru)
0x2c0a	Spanish (Argentina)
0x300a	Spanish (Ecuador)
0x340a	Spanish (Chile)
0x380a	Spanish (Uruguay)
0x3c0a	Spanish (Paraguay)
0x400a	Spanish (Bolivia)
0x440a	Spanish (El Salvador)
0x480a	Spanish (Honduras)
0x4c0a	Spanish (Nicaragua)
0x500a	Spanish (Puerto Rico)
0x040b	Finnish
0x040c	French (Standard)
0x080c	French (Belgian)
0x0c0c	French (Canadian)
0x100c	French (Swiss)
0x140c	French (Luxembourg)
0x040d	Hebrew
0x040e	Hungarian
0x040f	Icelandic
0x0410	Italian (Standard)
0x0810	Italian (Swiss)
0x0411	Japanese
0x0412	Korean
0x0812	Korean (JoHab)
0x0413	Dutch (Standard)
0x0813	Dutch (Belgian)
0x0414	Norwegian (Bokmal)
0x0814	Norwegian (Nynorsk)
0x0415	Polish
0x0416	Portuguese (Brazilian)
0x0816	Portuguese (Standard)
0x0418	Romanian
0x0419	Russian
0x041a	Croatian
0x0c1a	Serbian
0x041b	Slovak
0x041c	Albanian
0x041d	Swedish
0x081d	Swedish (Finland)
0x041e	Thai
0x041f	Turkish
0x0421	Indonesian
0x0422	Ukrainian
0x0423	Belarusian
0x0424	Slovenian
0x0425	Estonian
0x0426	Latvian
0x0427	Lithuanian
0x081a	Serbian
0x0429	Farsi
0x042d	Basque
0x0436	Afrikaans
0x0438	Faeroese*/
#else
    language = getenv("LANG");
    if (language == NULL)
    {
        language = getenv("LANGUAGE");
    }
    if (language == NULL)
    {
        language = getenv("LC_MESSAGES");
    }
    if (language == NULL)
    {
        language = "en";
    }
#endif
    
    return language;
}

/*----------------------------------------------------------------------------*/
void
compErrorMessage(const char* title, const char* message)
{
#ifdef __W32
    MessageBox(NULL, message, title, MB_OK | MB_ICONERROR | MB_SETFOREGROUND
#ifdef MB_TASKMODAL
    | MB_TASKMODAL
#endif
#ifdef MB_TOPMOST
    | MB_TOPMOST
#endif
    );
#else /*__W32*/
    (void)title;
    (void)message;
#endif
}
