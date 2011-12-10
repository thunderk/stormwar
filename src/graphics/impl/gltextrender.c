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
 *   Text renderer                                                            *
 *                                                                            *
  ***************************************************************************/


/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/gltextrender.h"

#include "graphics/glsurface.h"
#include "graphics/gliterator.h"
#include "graphics/color.h"

#include "core/ptrarray.h"
#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    String      name;       /* Font's name.                                   */
    GlSurface   surf;       /* Graphical representation of the font.          */
    Gl2DSize    w;          /* Normal size of a character.                    */
    Gl2DSize    h;          /* Normal size of a character.                    */
    Gl2DSize*   wcrop;      /* Horizontally cropped width of each character.  */
    Gl2DSize    wspacing;   /* Horizontal standard space size (between words).*/
    GlColor     lastcolor;  /* Last color applied on the surface.             */
} pv_GlFont;

typedef pv_GlFont* GlFont;

typedef struct
{
    GlColor color;          /* Rendering color.                                    */
    Bool monospace;         /* Specifies if the font is rendered monospace or not. */
} GlFontOptions;

struct pv_GlTextRender
{
    String fontname;        /* Asked font name.                                 */
    GlFont font;            /* Font linked, NULL if unvalidated.                */
    GlFontOptions opt;      /* Rendering options.                               */
    String render;          /* Text previously renderered.                      */
    Gl2DSize renderpos;     /* Position in text rendering.                      */
    Gl2DSize wlimit;        /* Width limit.                                     */
};

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static PtrArray _fonts;
static PtrArray _textrenders;

/******************************************************************************
 *                             Static constants                               *
 ******************************************************************************/
static const GlFontOptions GlFontOptions_STD = {0xFFFFFFFF, FALSE};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GlFont_del(GlFont font)
{
    PtrArrayIterator it;
    
    /*check if any renderer is linked to this font*/
    for (it = PtrArray_START(_textrenders); it != PtrArray_STOP(_textrenders); it++)
    {
        if (((GlTextRender)(*it))->font == font)
        {
            ((GlTextRender)(*it))->font = NULL;
        }
    }
    
    /*delete*/
    String_del(font->name);
    GlSurface_del(font->surf);
    FREE(font->wcrop);
    FREE(font);
}

/*----------------------------------------------------------------------------*/
static int
GlFont_cmp(GlFont* font1, GlFont* font2)
{
    return String_cmp(&((*font1)->name), &((*font2)->name));
}

/*----------------------------------------------------------------------------*/
static void
GlTextRender_delFinal(GlTextRender tr)
{
    if (tr->fontname != NULL)
    {
        String_del(tr->fontname);
    }
    FREE(tr);
}

/*----------------------------------------------------------------------------*/
/*This function creates the wcrop array for the given font and
treat the surface to be usable*/
static void
performAutoCrop(GlFont font)
{
    unsigned int cx, cy;    /*position in the character table*/
    GlRect clip;            /*rectangle clipping around a character*/
    GlIterator it;
    Gl2DSize crop = 0;
    Uint16 pos = 0;         /*position in the array*/
    Uint8 channel;
    GlColor col;
    
    GlIterator_init(&it, font->surf);
    
    clip.x = 0;
    clip.y = 0;
    clip.w = font->w;
    clip.h = font->h;
    for (cy = 0; cy < 16; cy++)
    {
        for (cx = 0; cx <16; cx++)
        {
            GlIterator_setClipRect(&it, &clip, CORNER_TOPLEFT);
            crop = 0;
            while (!GlIterator_endReached(it))
            {
                col = GlIterator_getColor(it);
                
                channel = GlColor_GETR(col);
                if (channel != 0x00)
                {
                    crop = MAX(crop, GlIterator_getXclipped(it));
                }
                GlColor_SETA(col, channel);
                
                /*to respect the lastcolor attribute:*/
                GlColor_SETR(col, 0x00);
                GlColor_SETG(col, 0x00);
                GlColor_SETB(col, 0x00);
                
                GlIterator_setColor(it, col);
                
                GlIterator_fwd(&it);
            }
            clip.x += clip.w;
            font->wcrop[pos++] = (Gl2DSize)MIN(crop + 1, font->w);     /*we let one more column for spacing*/
        }
        clip.x = 0;
        clip.y += clip.h;
    }
}

/*----------------------------------------------------------------------------*/
/*This function draws a single character*/
static Gl2DSize
drawChar(GlFont font, GlSurface surf, GlRect rect, Bool monospace, unsigned char c, Gl2DCoord offsetx, Gl2DCoord offsety)
{
    GlRect src;
    
    src.x = font->w * (c % 16);
    src.y = font->h * (c / 16);
    src.h = MIN(rect.h, font->h);
    if (monospace)
    {
        src.w = MIN(rect.w, font->w);
    }
    else
    {
        if (c == 32)
        {
            return font->wspacing;
        }
        src.w = MIN(rect.w, font->wcrop[(unsigned int)c]);
    }
    rect.x += offsetx;
    rect.y += offsety;
    GlSurface_doBlit(font->surf, surf, &src, &rect);
    
    return src.w;
}

/*----------------------------------------------------------------------------*/
/*This function gets the width of a single character*/
static Gl2DSize
sizeChar(unsigned char c, GlFont font, Bool monospace)
{
    if (monospace)
    {
        return font->w;
    }
    if (c == 32)
    {
        return font->wspacing;
    }
    return font->wcrop[(unsigned int)c];
}

/*----------------------------------------------------------------------------*/
static void
gltextAddFont(Var vfont)
{
    VarValidator varvalid;
    GlFont font;
    PtrArrayIterator it;
    
    varvalid = VarValidator_new();
    
    /*default values*/
    VarValidator_declareStringVar(varvalid, "font_name", "");
    VarValidator_declareStringVar(varvalid, "font_picture", "");
    VarValidator_declareIntVar(varvalid, "char_width", 1);
    VarValidator_declareIntVar(varvalid, "char_height", 1);
    VarValidator_declareIntVar(varvalid, "hor_spacing", 4);
    
    /*validate*/
    VarValidator_validate(varvalid, vfont);
    VarValidator_del(varvalid);

    /*we create the font*/
    font = (GlFont)MALLOC(sizeof(pv_GlFont));
    font->name = String_newByCopy(Var_getValueString(Var_getArrayElemByCName(vfont, "font_name")));

    shellPrintf(LEVEL_ERRORSTACK, "For font '%s'", String_get(font->name));
    
    font->surf = GlSurface_newFromFile(Var_getValueString(Var_getArrayElemByCName(vfont, "font_picture")));
    font->w = Var_getValueInt(Var_getArrayElemByCName(vfont, "char_width"));
    font->h = Var_getValueInt(Var_getArrayElemByCName(vfont, "char_height"));
    font->wspacing = Var_getValueInt(Var_getArrayElemByCName(vfont, "hor_spacing"));
    font->wcrop = MALLOC(sizeof(Gl2DSize) * 256);
    
    font->lastcolor = GlColor_NULL;
    
    /*some checks*/
    if ((GlSurface_getWidth(font->surf) != font->w * 16)
        || GlSurface_getHeight(font->surf) != font->h * 16)
    {
        shellPrintf(LEVEL_ERROR, "Sizes don't match. Font not added.");
        GlFont_del(font);
    }
    else
    {
        /*we crop for non-monospace drawing*/
        performAutoCrop(font);
        
        /*if the font already exists, we delete the old one*/
        it = PtrArray_findSorted(_fonts, font);
        if (it != NULL)
        {
            shellPrintf(LEVEL_ERROR, "Font already exists, will be replaced by the new one.");
            PtrArray_removeIt(_fonts, it);
        }
    
        /*and add the new one to the font set*/
        PtrArray_insertSorted(_fonts, font);
        
        /*correct the renderers that need this font*/
        for (it = PtrArray_START(_textrenders); it != PtrArray_STOP(_textrenders); it++)
        {
            if ((((GlTextRender)(*it))->fontname != NULL)
             && (String_cmp(&((GlTextRender)(*it))->fontname, &font->name) == 0))
            {
                ((GlTextRender)(*it))->font = font;
            }
        }
    }
    
    shellPopErrorStack();
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
gltextInit()
{
    _fonts = PtrArray_newFull(3, 2, (PtrFunc)GlFont_del, (PtrCmpFunc)GlFont_cmp);
    _textrenders = PtrArray_newFull(10, 5, (PtrFunc)GlTextRender_delFinal, NULL);
    
    shellPrint(LEVEL_INFO, "Text module loaded.");
}

/*----------------------------------------------------------------------------*/
void
gltextUninit()
{
    PtrArray_clear(_textrenders);       /*to avoid callbacks on font deleting*/
    PtrArray_del(_fonts);
    PtrArray_del(_textrenders);
    
    shellPrint(LEVEL_INFO, "Text module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
gltextClearFonts()
{
    PtrArray_clear(_fonts);
}

/*----------------------------------------------------------------------------*/
void
gltextSetFonts(Var vfonts)
{
    unsigned int i;
    
    shellPrintf(LEVEL_ERRORSTACK, "In font set loading from variable: %s", Var_gets(vfonts));
    
    Var_resolveLink(vfonts);
    if (Var_getType(vfonts) != VAR_ARRAY)
    {
        shellPrint(LEVEL_ERROR, "Font set variable not of array type.");
        shellPopErrorStack();
        return;
    }
    
    for (i = 0; i < Var_getArraySize(vfonts); i++)
    {
        gltextAddFont(Var_getArrayElemByPos(vfonts, i));
    }

    shellPopErrorStack();
}

/*----------------------------------------------------------------------------*/
GlTextRender
GlTextRender_new()
{
    GlTextRender ret;
    
    ret = MALLOC(sizeof(pv_GlTextRender));
    ret->fontname = NULL;
    ret->font = NULL;
    ret->opt = GlFontOptions_STD;
    ret->render = NULL;
    ret->renderpos = 0;
    ret->wlimit = 0;
    
    PtrArray_append(_textrenders, ret);
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_del(GlTextRender tr)
{
    PtrArray_removeFast(_textrenders, tr);
    /*this will call GlTextRender_delFinal automatically*/
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_setFont(GlTextRender tr, String fontname)
{
    pv_GlFont f;
    PtrArrayIterator it;
    
    if (tr->fontname != NULL)
    {
        String_del(tr->fontname);
    }
    f.name = tr->fontname = String_newByCopy(fontname);
    it = PtrArray_findSorted(_fonts, &f);
    if (it == NULL)
    {
        tr->font = NULL;
    }
    else
    {
        tr->font = *((GlFont*)it);
    }
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_setOptions(GlTextRender tr, Bool monospace)
{
    tr->opt.monospace = monospace;
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_setOptionsFromVar(GlTextRender tr, Var vopt)
{
    VarValidator varvalid;
    GlColorRGBA c;
    
    /*init*/
    varvalid = VarValidator_new();
    
    /*declares*/
    VarValidator_declareIntVar(varvalid, "color_red", 255);
    VarValidator_declareIntVar(varvalid, "color_green", 255);
    VarValidator_declareIntVar(varvalid, "color_blue", 255);
    VarValidator_declareIntVar(varvalid, "monospace", FALSE);
    
    /*load*/
    VarValidator_validate(varvalid, vopt);
    VarValidator_del(varvalid);
    
    /*set*/
    c.r = Var_getValueInt(Var_getArrayElemByCName(vopt, "color_red"));
    c.g = Var_getValueInt(Var_getArrayElemByCName(vopt, "color_green"));
    c.b = Var_getValueInt(Var_getArrayElemByCName(vopt, "color_blue"));
    c.a = 0xFF;
    tr->opt.color = GlColorRGBA_to_GlColor(c);
    tr->opt.monospace = Var_getValueInt(Var_getArrayElemByCName(vopt, "monospace"));
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_setColor(GlTextRender tr, GlColor col)
{
    tr->opt.color = col;
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_setWidthLimit(GlTextRender tr, Gl2DSize widthlimit)
{
    tr->wlimit = widthlimit;
}

/*----------------------------------------------------------------------------*/
Uint16
GlTextRender_getLineHeight(GlTextRender tr)
{
    if (tr->font == NULL)
    {
        return 0;
    }
    else
    {
        return tr->font->h;
    }
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_guessSize(GlTextRender tr, String text, Gl2DSize* r_width, Gl2DSize* r_height)
{
    char* st = String_get(text);
    Uint16 x;
    Uint16 y;
    Uint16 ax;
    
    if (tr->font == NULL)
    {
        *r_width = 0;
        *r_height = 0;
        return;
    }
    
    *r_width = x = 0;
    y = tr->font->h;
    
    while (*st != '\0')
    {
        if (*st == '\n')
        {
            if (x > *r_width)
            {
                *r_width = x;
            }
            x = 0;
            y += tr->font->h;
        }
        else if (*st == '\r')
        {
            if (x > *r_width)
            {
                *r_width = x;
            }
            x = 0;
        }
        else
        {
            if (tr->opt.monospace)
            {
                ax = tr->font->w;
            }
            else
            {
                if (*st == 32)
                {
                    ax = tr->font->wspacing;
                }
                else
                {
                    ax = tr->font->wcrop[(unsigned char)*st];
                }
            }
            
            if ((tr->wlimit != 0) && (x + ax > tr->wlimit))
            {
                /*left limit reached*/
                if (x > *r_width)
                {
                    *r_width = x;
                }
                x = 0;
                y += tr->font->h;
                continue;
            }
            x += ax;
        }
        st++;
    }

    if (x > *r_width)
    {
        *r_width = x;
    }
    *r_height = y;
}

/*----------------------------------------------------------------------------*/
GlTextRenderStatus
GlTextRender_render(GlTextRender tr, String text, GlSurface surf, Gl2DCoord posx, Gl2DCoord posy, Uint16 heightlimit)
{
    GlTextRenderStatus ret;
    char* st;
    GlFont font;
    GlIterator it;
    GlRect rct;
    Gl2DSize dw;
    GlColor col;
    
    ret.width = 0;
    ret.height = 0;
    
    if (tr->font == NULL)
    {
        ret.breakevent = GLTEXTRENDER_END;
        return ret;
    }
    font = tr->font;
    
    st = String_get(text);
    
    rct.x = posx;
    rct.y = posy;
    if (tr->wlimit == 0)
    {
        rct.w = GlSurface_getWidth(surf) - posx;
    }
    else
    {
        rct.w = tr->wlimit;
    }
    if (heightlimit == 0)
    {
        rct.h = GlSurface_getHeight(surf) - posy;
    }
    else
    {
        rct.h = heightlimit;
    }
    
    if ((rct.w == 0) || (rct.h == 0))
    {
        ret.breakevent = GLTEXTRENDER_END;
        return ret;
    }
    
    /*we fill the font surface with the drawing color (without altering alpha channel)*/
    if (tr->opt.color != font->lastcolor)
    {
        GlIterator_init(&it, font->surf);
        while (!GlIterator_endReached(it))
        {
            col = GlIterator_getColor(it) & GlColor_Amask;
            col |= (tr->opt.color & (~GlColor_Amask));
            GlIterator_setColor(it, col);
            GlIterator_fwd(&it);
        }
        font->lastcolor = tr->opt.color;
    }
    
    /*draw characters*/
    if (tr->render == text)
    {
        ASSERT(tr->renderpos < String_getLength(text), tr->renderpos = 0);
        st += tr->renderpos;
    }
    else
    {
        tr->render = NULL;
    }
    ret.height = MIN(font->h, rct.h);
    while (*st != '\0')
    {
        if (*st == '\n')
        {
            tr->render = text;
            tr->renderpos = st - String_get(text) + 1;
            ret.breakevent = GLTEXTRENDER_NEWLINE;
            return ret;
        }
        if (*st == '\r')
        {
            tr->render = text;
            tr->renderpos = st - String_get(text) + 1;
            ret.breakevent = GLTEXTRENDER_RETURN;
            return ret;
        }
        if ((tr->wlimit != 0) && (sizeChar(*st, font, tr->opt.monospace) > rct.w))
        {
            tr->render = text;
            tr->renderpos = st - String_get(text);
            ret.breakevent = GLTEXTRENDER_NEWLINE;
            return ret;
        }
        dw = drawChar(font, surf, rct, tr->opt.monospace, *st, 0, 0);
        ret.width += dw;
        rct.x += dw;
        rct.w -= dw;
        st++;
    }
    tr->render = NULL;
    ret.breakevent = GLTEXTRENDER_END;
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlTextRender_directRender(GlTextRender tr, String text, GlSurface surf, GlRect rect, Gl2DCoord offsetx, Gl2DCoord offsety)
{
    GlFont font;
    char* st;
    Gl2DCoord x, y;
    GlIterator it;
    GlColor col;
    
    if (tr->font == NULL)
    {
        return;
    }
    font = tr->font;
    tr->render = NULL;

    st = String_get(text);
    
    if ((offsety <= -font->h) || (offsety > rect.h) || (offsetx > rect.w))
    {
        return;
    }
    x = offsetx;
    y = offsety;
    
    /*we fill the font surface with the drawing color (without altering alpha channel)*/
    GlIterator_init(&it, font->surf);
    while (!GlIterator_endReached(it))
    {
        col = GlIterator_getColor(it) & GlColor_Amask;
        col |= (tr->opt.color & (~GlColor_Amask));
        GlIterator_setColor(it, col);
        GlIterator_fwd(&it);
    }
    
    /*draw characters*/
    while (*st != '\0')
    {
        if (*st == '\n')
        {
            y += tr->font->h;
            x = offsetx;
            st++;
            continue;
        }
        
        if (*st == '\r')
        {
            x = offsetx;
            st++;
            continue;
        }
        
        if ((tr->wlimit != 0) && (offsetx + sizeChar(*st, font, tr->opt.monospace) > rect.w))
        {
            x = offsetx;
            y += tr->font->h;
        }
        
        x += drawChar(font, surf, rect, tr->opt.monospace, *st, x, y);
        st++;
    }
}
