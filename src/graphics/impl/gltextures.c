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
 *   OpenGL texture management                                                *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/gltextures.h"

#include "graphics/impl/opengl.h"

#include "graphics/color.h"
#include "graphics/graphics.h"
#include "graphics/glsurface.h"
#include "graphics/impl/impl.h"

#include "core/string.h"
#include "core/core.h"
#include "core/var.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GlStaticTexture
{
    String name;
    GlSurface surf;
    OpenGLTexture texid;
    Bool xwrap;
    Bool ywrap;
    Bool filter;
    Float* mat;
};

typedef struct
{
    TexRenderInfo info;
    GlSurface surf;
    OpenGLTexture texid;
} TexPart;

struct pv_GlLinkedTexture
{
    GlSurface link;
    Uint16 nbpartx;
    Uint16 nbparty;
    Uint16 nbparts;
    Uint16 curpart;
    TexPart* parts;
};

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define TEX_MATERIAL_NB 17
GlStaticTexture GlStaticTexture_NULL;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define BYTEORDER GL_RGBA
#else
    #define BYTEORDER GL_BGRA
#endif

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static PtrArray _staticarray;
static PtrArray _linkedarray;
static GlTextureFilter _currentfilter;

static CoreID MOD_ID = 0;
static CoreID FUNC_SETFILTER = 0;
static CoreID FUNC_NBTEX = 0;
static CoreID FUNC_NBSTATICTEX = 0;

static Var _prefsvar;

static unsigned int _nbtextures;     /*effective number of OpenGL textures reserved*/

static void* _placedtex;             /*currently placed texture*/

static Gl2DSize _maxtexsize;

/******************************************************************************
 *############################################################################*
 *#                           Internal functions                             #*
 *############################################################################*
 ******************************************************************************/
static OpenGLTexture
reserveOGLTex()
{
    OpenGLTexture id;

    glGenTextures(1, &id);
    ASSERT_WARN(id != 0);
    _nbtextures++;
#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: OpenGL texture allocated: %3d, nbtex=%d.", id, _nbtextures);
#endif
    return id;
}

/*----------------------------------------------------------------------------*/
static void
deleteOGLTex(OpenGLTexture tex)
{
    ASSERT(tex != 0, return);

    glDeleteTextures(1, &tex);
    _nbtextures--;
#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: OpenGL texture released: %3d, nbtex=%d.", tex, _nbtextures);
#endif
}

/*----------------------------------------------------------------------------*/
static Uint16
betterTexturePartSize(Uint16 width, Uint16 height)
{
    Uint16 s, r;

    s = MIN(width, height);
    r = 1;
    while ((r < _maxtexsize) && (r < s))
    {
        r *= 2;
    }
    return r;
}

/*----------------------------------------------------------------------------*/
static Uint16
betterTextureSize(Uint16 width, Uint16 height)
{
    Uint16 s, r;

    s = MAX(width, height);
    r = _maxtexsize;
    while ((r > 1) && (r > s))
    {
        r /= 2;
    }
    return r;
}

/*----------------------------------------------------------------------------*/
static void
createParts(GlLinkedTexture tex)
{
    Uint16 gw, gh;      /*global size*/
    Uint16 lw, lh;      /*lasting size*/
    Uint16 x, y;
    Uint16 w, h;
    Uint16 partsize;
    GlSurface surf;
    unsigned int i;
    
    surf = tex->link;
    gw = GlSurface_getWidth(surf);
    gh = GlSurface_getHeight(surf);
    
    partsize = betterTexturePartSize(gw, gh);
    
    tex->nbpartx = (gw - 1) / partsize + 1;
    tex->nbparty = (gh - 1) / partsize + 1;
    
    /*allocate the parts*/
    tex->nbparts = tex->nbpartx * tex->nbparty;
    tex->parts = MALLOC(sizeof(TexPart) * tex->nbparts);
    
    /*fill the parts values*/
    i = 0;
    lh = gh;
    for (y = 0; y < tex->nbparty; y++)
    {
        lw = gw;
        for (x = 0; x < tex->nbpartx; x++)
        {
            tex->parts[i].info.rct.x = x * partsize;
            tex->parts[i].info.rct.y = y * partsize;
            w = tex->parts[i].info.rct.w = MIN(lw, partsize);
            h = tex->parts[i].info.rct.h = MIN(lh, partsize);
            
            tex->parts[i].info.texcoords = MALLOC(sizeof(TexCoord) * 8);
            tex->parts[i].info.texcoords[0] = 0.0f;
            tex->parts[i].info.texcoords[1] = 0.0f;
            tex->parts[i].info.texcoords[2] = (TexCoord)w / (TexCoord)partsize;
            tex->parts[i].info.texcoords[3] = 0.0f;
            tex->parts[i].info.texcoords[4] = (TexCoord)w / (TexCoord)partsize;
            tex->parts[i].info.texcoords[5] = (TexCoord)h / (TexCoord)partsize;
            tex->parts[i].info.texcoords[6] = 0.0f;
            tex->parts[i].info.texcoords[7] = (TexCoord)h / (TexCoord)partsize;
            
            tex->parts[i].surf = GlSurface_new(partsize, partsize, TRUE);
            tex->parts[i].texid = 0;
            lw -= partsize;
            i++;
        }
        lh -= partsize;
    }
}

/*----------------------------------------------------------------------------*/
static void
deleteParts(GlLinkedTexture tex)
{
    Uint16 i;
    
    if (tex->nbparts != 0)
    {
        for (i = 0; i < tex->nbparts; i++)
        {
            GlSurface_del(tex->parts[i].surf);
            if (tex->parts[i].texid != 0)
            {
                deleteOGLTex(tex->parts[i].texid);
            }
            FREE(tex->parts[i].info.texcoords);
        }
        FREE(tex->parts);
        tex->nbparts = 0;
    }
}

/*----------------------------------------------------------------------------*/
static void
GlStaticTexture_free(GlStaticTexture tex)
{
#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: Deleting static texture '%s'.", String_get(tex->name));
#endif
    GlSurface_del(tex->surf);
    if (tex->texid != 0)
    {
        deleteOGLTex(tex->texid);
    }
    FREE(tex->mat);
    String_del(tex->name);
    if (tex == (GlStaticTexture)_placedtex)
    {
        _placedtex = NULL;
    }
    FREE(tex);
}

/*----------------------------------------------------------------------------*/
static void
GlLinkedTexture_free(GlLinkedTexture tex)
{
#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: Deleting linked texture %p.", (void*)tex);
#endif
    deleteParts(tex);
    if (tex == (GlLinkedTexture)_placedtex)
    {
        _placedtex = NULL;
    }
    FREE(tex);
}

/*----------------------------------------------------------------------------*/
static int
GlStaticTexture_cmp(GlStaticTexture* tex1, GlStaticTexture* tex2)
{
    return String_cmp(&(*tex1)->name, &(*tex2)->name);
}

/*----------------------------------------------------------------------------*/
static void
GlStaticTexture_upload(GlStaticTexture tex)
{
    GlSurface surf;
    Gl2DSize w, h;

    if (tex->texid == 0)
    {
        tex->texid = reserveOGLTex();
    }
    glBindTexture(GL_TEXTURE_2D, tex->texid);

    surf = tex->surf;
    w = GlSurface_getWidth(surf);
    h = GlSurface_getHeight(surf);

    if (_currentfilter > GLTEX_BILINEAR)
    {
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, BYTEORDER, GL_UNSIGNED_BYTE, GlSurface_getPixels(surf));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (tex->xwrap) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (tex->ywrap) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, BYTEORDER, GL_UNSIGNED_BYTE, GlSurface_getPixels(surf));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (tex->xwrap) ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (tex->ywrap) ? GL_REPEAT : GL_CLAMP);
    }

    switch (_currentfilter)
    {
        case GLTEX_NONE:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case GLTEX_MIPMAPPED:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (tex->filter) ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (tex->filter) ? GL_LINEAR : GL_NEAREST);
            break;
        case GLTEX_TRILINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (tex->filter) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (tex->filter) ? GL_LINEAR : GL_NEAREST);
            break;
        default:    /*GLTEX_BILINEAR*/
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (tex->filter) ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (tex->filter) ? GL_LINEAR : GL_NEAREST);
    }

    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 3);*/

#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: Static texture '%s' uploaded.", String_get(tex->name));
#endif
}

/*----------------------------------------------------------------------------*/
static void
GlLinkedTexture_upload(GlLinkedTexture tex)
{
    GlSurface surf;
    Gl2DSize w, h;
    Uint16 i;

    for (i = 0; i < tex->nbparts; i++)
    {
        surf = tex->parts[i].surf;
        w = GlSurface_getWidth(tex->parts[i].surf);
        h = GlSurface_getHeight(tex->parts[i].surf);

        if (tex->parts[i].texid == 0)
        {
            tex->parts[i].texid = reserveOGLTex();
            glBindTexture(GL_TEXTURE_2D, tex->parts[i].texid);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, tex->parts[i].texid);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, BYTEORDER, GL_UNSIGNED_BYTE, GlSurface_getPixels(surf));
    }
#ifdef DEBUG_TEX
    shellPrintf(LEVEL_DEBUG, "TEX: Linked texture %p uploaded.", (void*)tex);
#endif
}

/*----------------------------------------------------------------------------*/
static void
setMaterial(float* mat, float shiny, GlColorRGBA* cols)
{
    mat[0] = flmin(flmax(shiny, 0.0), 128.0);
    mat[1] = ((float)cols[0].r) / 255.0;
    mat[2] = ((float)cols[0].g) / 255.0;
    mat[3] = ((float)cols[0].b) / 255.0;
    mat[4] = ((float)cols[0].a) / 255.0;
    mat[5] = ((float)cols[1].r) / 255.0;
    mat[6] = ((float)cols[1].g) / 255.0;
    mat[7] = ((float)cols[1].b) / 255.0;
    mat[8] = ((float)cols[1].a) / 255.0;
    mat[9] = ((float)cols[2].r) / 255.0;
    mat[10] = ((float)cols[2].g) / 255.0;
    mat[11] = ((float)cols[2].b) / 255.0;
    mat[12] = ((float)cols[2].a) / 255.0;
    mat[13] = ((float)cols[3].r) / 255.0;
    mat[14] = ((float)cols[3].g) / 255.0;
    mat[15] = ((float)cols[3].b) / 255.0;
    mat[16] = ((float)cols[3].a) / 255.0;
}

/*----------------------------------------------------------------------------*/
static void
gltexturesLoad(Var texvar)
{
    GlStaticTexture tex;
    VarValidator valid;
    String name;
    String file;
    Gl2DSize w, h, texsize;
    GlColorRGBA col[4];

    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "name", "");
    VarValidator_declareStringVar(valid, "file", "");
    VarValidator_declareIntVar(valid, "filter", 1);
    VarValidator_declareIntVar(valid, "xwrap", 0);
    VarValidator_declareIntVar(valid, "ywrap", 0);
    VarValidator_declareFloatVar(valid, "shiny", 0);
    VarValidator_declareArrayVar(valid, "col_ambient");
    VarValidator_declareArrayVar(valid, "col_diffuse");
    VarValidator_declareArrayVar(valid, "col_specular");
    VarValidator_declareArrayVar(valid, "col_emission");

    VarValidator_validate(valid, texvar);
    VarValidator_del(valid);

    tex = (GlStaticTexture)MALLOC(sizeof(pv_GlStaticTexture));

    name = Var_getValueString(Var_getArrayElemByCName(texvar, "name"));

#ifdef DEBUG_TEX
        shellPrintf(LEVEL_DEBUG, "TEX: Loading static texture '%s'.", String_get(name));
#endif

    file = Var_getValueString(Var_getArrayElemByCName(texvar, "file"));
    tex->filter = ((Var_getValueInt(Var_getArrayElemByCName(texvar, "filter"))) ? TRUE : FALSE);
    tex->xwrap = ((Var_getValueInt(Var_getArrayElemByCName(texvar, "xwrap"))) ? TRUE : FALSE);
    tex->ywrap = ((Var_getValueInt(Var_getArrayElemByCName(texvar, "ywrap"))) ? TRUE : FALSE);

    tex->mat = MALLOC(sizeof(float) * TEX_MATERIAL_NB);
    GlColorRGBA_makeFromVar(col, Var_getArrayElemByCName(texvar, "col_ambient"));
    GlColorRGBA_makeFromVar(col + 1, Var_getArrayElemByCName(texvar, "col_diffuse"));
    GlColorRGBA_makeFromVar(col + 2, Var_getArrayElemByCName(texvar, "col_specular"));
    GlColorRGBA_makeFromVar(col + 3, Var_getArrayElemByCName(texvar, "col_emission"));
    setMaterial(tex->mat, Var_getValueFloat(Var_getArrayElemByCName(texvar, "shiny")), col);

    tex->name = String_newByCopy(name);
    tex->surf = GlSurface_newFromFile(file);
    w = GlSurface_getWidth(tex->surf);
    h = GlSurface_getHeight(tex->surf);
    texsize = betterTextureSize(w, h);
    if ((w != texsize) || (h != texsize))
    {
        /*need resizing*/
#ifdef DEBUG_TEX
        shellPrintf(LEVEL_DEBUG, "TEX: Resizing static texture '%s' from %u*%u to %u*%u.", String_get(name), w, h, texsize, texsize);
#endif
        GlSurface_resize(tex->surf, texsize, texsize, SURFACE_RESAMPLE);
    }
    tex->texid = 0;

    GlStaticTexture_upload(tex);

    PtrArray_insertSorted(_staticarray, tex);
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* funct)
{
    if (funct->id == FUNC_SETFILTER)
    {
        int filter;

        filter = Var_getValueInt(funct->params[0]);
        gltexturesSetFilter((GlTextureFilter)filter);
        Var_setVoid(funct->ret);
    }
    else if (funct->id == FUNC_NBTEX)
    {
        Var_setInt(funct->ret, _nbtextures);
    }
    else if (funct->id == FUNC_NBSTATICTEX)
    {
        Var_setInt(funct->ret, PtrArray_SIZE(_staticarray));
    }
}

/*----------------------------------------------------------------------------*/
static void
prefsCallback(Var prefs)
{
    VarValidator valid;

    _prefsvar = prefs;

    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "texture_filter", (int)GLTEX_TRILINEAR);

    VarValidator_validate(valid, prefs);
    VarValidator_del(valid);

    gltexturesSetFilter((GlTextureFilter)Var_getValueInt(Var_getArrayElemByCName(prefs, "texture_filter")));
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
gltexturesInit()
{
    int i;

    _staticarray = PtrArray_newFull(20, 10, (PtrFunc)GlStaticTexture_free, (PtrCmpFunc)GlStaticTexture_cmp);
    _linkedarray = PtrArray_newFull(100, 50, (PtrFunc)GlLinkedTexture_free, NULL);
    _currentfilter = GLTEX_TRILINEAR;

    _nbtextures = 0;
    _placedtex = NULL;

    GlStaticTexture_NULL = MALLOC(sizeof(pv_GlStaticTexture));
    GlStaticTexture_NULL->name = String_new("");
    GlStaticTexture_NULL->texid = 0;
    GlStaticTexture_NULL->surf = GlSurface_new(1, 1, TRUE);
    GlStaticTexture_NULL->mat = MALLOC(sizeof(float) * TEX_MATERIAL_NB);
    /*TODO: default material and params*/
    GlStaticTexture_upload(GlStaticTexture_NULL);

    _prefsvar = NULL;
    MOD_ID = coreDeclareModule("tex", NULL, NULL, shellCallback, prefsCallback, NULL, NULL);
    FUNC_SETFILTER = coreDeclareShellFunction(MOD_ID, "setfilter", VAR_VOID, 1, VAR_INT);
    FUNC_NBTEX = coreDeclareShellFunction(MOD_ID, "nbtex", VAR_INT, 0);
    FUNC_NBSTATICTEX = coreDeclareShellFunction(MOD_ID, "nbstatictex", VAR_INT, 0);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
    shellPrintf(LEVEL_INFO, " -> maximal texture size: %d", i);
    _maxtexsize = (Gl2DSize)i;
}

/*----------------------------------------------------------------------------*/
void
gltexturesUninit()
{
    PtrArray_del(_staticarray);
    PtrArray_del(_linkedarray);
    
    GlStaticTexture_free(GlStaticTexture_NULL);
    
    shellPrint(LEVEL_INFO, "Textures module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
gltexturesLoadSet(Var v)
{
    PtrArrayPos i;

    PtrArray_clear(_staticarray);

    Var_resolveLink(v);
    if (Var_getType(v) != VAR_ARRAY)
    {
        Var_setArray(v);
    }

    for (i = 0; i < Var_getArraySize(v); i++)
    {
        gltexturesLoad(Var_getArrayElemByPos(v, i));
    }
}

/*----------------------------------------------------------------------------*/
GlStaticTexture
gltexturesGet(String name)
{
    pv_GlStaticTexture base;
    PtrArrayIterator i;

    base.name = name;
    i = PtrArray_findSorted(_staticarray, &base);
    if (i == NULL)
    {
#ifdef DEBUG_TEX
        shellPrintf(LEVEL_DEBUG, "Asked an unfound static texture: %s", String_get(name));
#endif
        return GlStaticTexture_NULL;
    }
    else
    {
        return (GlStaticTexture)(*i);
    }
}

/*----------------------------------------------------------------------------*/
void
gltexturesCheck(void)
{
    PtrArrayIterator i;
    Uint16 j;
    OpenGLTexture tex;
    GlStaticTexture stex;
    GlLinkedTexture ltex;

    if (_nbtextures == 0)
    {
        return;
    }

    tex = reserveOGLTex();
    deleteOGLTex(tex);

    if (tex == 1)
    {
        /*all textures have been cleared by the OpenGL implementation, need reloading*/
        shellPrint(LEVEL_INFO, "OpenGL textures were discarded, reloading them.");
        _nbtextures = 0;

        for (i = PtrArray_START(_staticarray); i != PtrArray_STOP(_staticarray); i++)
        {
            stex = (GlStaticTexture)(*i);
            stex->texid = 0;
        }
        PtrArray_foreach(_staticarray, (PtrFunc)GlStaticTexture_upload);
        
        for (i = PtrArray_START(_linkedarray); i != PtrArray_STOP(_linkedarray); i++)
        {
            ltex = (GlLinkedTexture)(*i);
            for (j = 0; j < ltex->nbparts; j++)
            {
                ltex->parts[j].texid = 0;
            }
        }
        PtrArray_foreach(_linkedarray, (PtrFunc)GlLinkedTexture_upload);
        
        GlStaticTexture_NULL->texid = 0;
        GlStaticTexture_upload(GlStaticTexture_NULL);
        
        _placedtex = NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
gltexturesSetFilter(GlTextureFilter filter)
{
    filter = MAX((int)filter, GLTEX_NONE);
    filter = MIN((int)filter, GLTEX_TRILINEAR);

    if (filter != _currentfilter)
    {
        _currentfilter = filter;

        /*reload static textures*/
        PtrArray_foreach(_staticarray, (PtrFunc)GlStaticTexture_upload);

        if (_prefsvar != NULL)
        {
            Var v;

            v = Var_getArrayElemByCName(_prefsvar, "texture_filter");
            ASSERT(v != NULL, return);
            Var_setInt(v, (Int)filter);
        }
    }
}

/******************************************************************************
 *############################################################################*
 *#                         Static textures functions                        #*
 *############################################################################*
 ******************************************************************************/
void
GlStaticTexture_use(GlStaticTexture tex)
{
    if (_placedtex != tex)
    {
        glBindTexture(GL_TEXTURE_2D, tex->texid);
        
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, tex->mat[0]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tex->mat + 1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tex->mat + 5);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tex->mat + 9);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, tex->mat + 13);

        _placedtex = tex;
    }
}

/******************************************************************************
 *############################################################################*
 *#                         Linked textures functions                        #*
 *############################################################################*
 ******************************************************************************/
GlLinkedTexture
GlLinkedTexture_new(GlSurface surf, Uint16* nbparts)
{
    GlLinkedTexture ret;
    
    ret = MALLOC(sizeof(pv_GlLinkedTexture));
    
    ret->link = surf;
    createParts(ret);
    ret->curpart = 0;
    *nbparts = ret->nbparts;
    
    GlLinkedTexture_updateLink(ret);  /*will fill the parts and upload them*/
    PtrArray_append(_linkedarray, ret);
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlLinkedTexture_del(GlLinkedTexture tex)
{
    PtrArray_removeFast(_linkedarray, tex);
}

/*----------------------------------------------------------------------------*/
TexRenderInfo*
GlLinkedTexture_use(GlLinkedTexture tex)
{
    TexRenderInfo* ret;
    
    glBindTexture(GL_TEXTURE_2D, tex->parts[tex->curpart].texid);
    ret = &tex->parts[tex->curpart].info;
    tex->curpart = (tex->curpart + 1) % tex->nbparts;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
Uint16
GlLinkedTexture_linkToSurf(GlLinkedTexture tex, GlSurface surf)
{
    deleteParts(tex);
    tex->link = surf;
    createParts(tex);
    GlLinkedTexture_updateLink(tex);  /*will fill the parts and upload them*/
    return tex->nbparts;
}

/*----------------------------------------------------------------------------*/
void
GlLinkedTexture_updateLink(GlLinkedTexture tex)
{
    GlRect rctsrc, rctdest;
    unsigned int i;
    unsigned int x, y;
    
    /*fill the parts*/
    rctdest.x = 0;
    rctdest.y = 0;
    i = 0;
    rctsrc.y = 0;
    for (y = 0; y < tex->nbparty; y++)
    {
        rctsrc.x = 0;
        for (x = 0; x < tex->nbpartx; x++)
        {
            rctsrc.w = rctdest.w = tex->parts[i].info.rct.w;
            rctsrc.h = rctdest.h = tex->parts[i].info.rct.h;
            GlSurface_copyRect(tex->link, tex->parts[i].surf, &rctsrc, &rctdest);
            rctsrc.x += rctsrc.w;
            i++;
        }
        rctsrc.y += rctsrc.h;
    }

    
    /*reupload them*/
    GlLinkedTexture_upload(tex);
}
