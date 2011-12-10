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
 *   Part of a GlMesh                                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/glmeshpart.h"
#include "graphics/impl/opengl.h"
#include "graphics/impl/impl.h"
#include "graphics/impl/gltextures.h"

#include "tools/varvalidator.h"
#include "tools/fonct.h"
#include "core/string.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GlMeshPart
{
    unsigned int nbtriangles;
    unsigned int nbquads;
    GLenum shademode;       /*GL_FLAT or GL_SMOOTH*/
    Bool blended;           /*blended or not*/
    Bool twosided;          /*two-sided polygons mode*/
    GlStaticTexture tex;    /*texture identifier*/
    Gl3DCoord* vertices3;   /*vertex coordinates (3), grouped by triangles*/
    TexCoord* texcoords3;   /*texture coordinates (2), grouped by triangles*/
    Gl3DCoord* normals3;    /*normal vector coordinates (3), grouped by triangles*/
    Gl3DCoord* vertices4;   /*vertex coordinates (3), grouped by quads*/
    TexCoord* texcoords4;   /*texture coordinates (2), grouped by quads*/
    Gl3DCoord* normals4;    /*normal vector coordinates (3), grouped by quads*/
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
void
read3GlCoord(Var v, Gl3DCoord* c)
{
    if ((Var_getType(v) != VAR_ARRAY) || (Var_getArraySize(v) != 3))
    {
        shellPrintf(LEVEL_ERROR, "Incorrect 3d coordinates variable: %s", Var_gets(v));
        c[0] = 0.0;
        c[1] = 0.0;
        c[2] = 0.0;
    }
    else
    {
        int i;
        
        for (i = 0; i < 3; i++)
        {
            if (Var_getType(Var_getArrayElemByPos(v, i)) == VAR_FLOAT)
            {
                c[i] = Var_getValueFloat(Var_getArrayElemByPos(v, i));
            }
            else
            {
                c[i] = 0.0;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void
read2TexCoord(Var v, TexCoord* c)
{
    if ((Var_getType(v) != VAR_ARRAY) || (Var_getArraySize(v) != 2))
    {
        shellPrintf(LEVEL_ERROR, "Incorrect texture coordinates variable: %s", Var_gets(v));
        c[0] = 0.0;
        c[1] = 0.0;
    }
    else
    {
        int i;
        
        for (i = 0; i < 2; i++)
        {
            if (Var_getType(Var_getArrayElemByPos(v, i)) == VAR_FLOAT)
            {
                c[i] = Var_getValueFloat(Var_getArrayElemByPos(v, i));
            }
            else
            {
                c[i] = 0.0;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
GlMeshPart_setFromVar(GlMeshPart part, Var var)
{
    VarValidator valid;
    Gl3DCoord* vert;
    Gl3DCoord* norm;
    TexCoord* texc;
    Var v;
    unsigned int i, n;
    unsigned int vert_nb, norm_nb, texc_nb;
    
    /*TODO: some additional checks over array sizes in the variable*/
    
    /*validate*/
    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "tex", "");
    VarValidator_declareIntVar(valid, "flatshading", 0);
    VarValidator_declareIntVar(valid, "twosided", 0);
    VarValidator_declareIntVar(valid, "blended", 0);
    VarValidator_declareIntVar(valid, "normalsmode", 0);
    VarValidator_declareIntVar(valid, "texcoordsmode", 0);
    VarValidator_declareArrayVar(valid, "vertices");
    VarValidator_declareArrayVar(valid, "normals");
    VarValidator_declareArrayVar(valid, "texcoords");
    VarValidator_declareArrayVar(valid, "faces");
    VarValidator_validate(valid, var);
    VarValidator_del(valid);

    /*prepare error stack*/
    shellPrintf(LEVEL_ERRORSTACK, "In mesh part variable: %s", String_get(Var_getName(var)));
    
    /*rendering mode*/
    part->shademode = (Var_getValueInt(Var_getArrayElemByCName(var, "flatshading")) == 0) ? GL_SMOOTH : GL_FLAT;
    part->twosided = (Var_getValueInt(Var_getArrayElemByCName(var, "twosided")) == 0) ? FALSE : TRUE;
    part->blended = (Var_getValueInt(Var_getArrayElemByCName(var, "blended")) == 0) ? FALSE : TRUE;
    
    /*texture id*/
    part->tex = gltexturesGet(Var_getValueString(Var_getArrayElemByCName(var, "tex")));
    
    /*building temporary data arrays*/
    v = Var_getArrayElemByCName(var, "vertices");
    if ((vert_nb = Var_getArraySize(v)) != 0)
    {
        vert = MALLOC(sizeof(Gl3DCoord) * 3 * vert_nb);
        for (i = 0; i < vert_nb; i++)
        {
            read3GlCoord(Var_getArrayElemByPos(v, i), vert + i * 3);
        }
    }
    else
    {
        vert = NULL;
    }
    v = Var_getArrayElemByCName(var, "normals");
    if ((norm_nb = Var_getArraySize(v)) != 0)
    {
        norm = MALLOC(sizeof(Gl3DCoord) * 3 * norm_nb);
        for (i = 0; i < norm_nb; i++)
        {
            Gl3DCoord d;
            
            read3GlCoord(Var_getArrayElemByPos(v, i), norm + i * 3);
            
            /*normalize*/
            d = dist3d(0.0, 0.0, 0.0, norm[i * 3], norm[i * 3 + 1], norm[i * 3 + 2]);
            norm[i * 3] /= d;
            norm[i * 3 + 1] /= d;
            norm[i * 3 + 2] /= d;
        }
    }
    else
    {
        norm = NULL;
    }
    v = Var_getArrayElemByCName(var, "texcoords");
    if ((texc_nb = Var_getArraySize(v)) != 0)
    {
        texc = MALLOC(sizeof(TexCoord) * 2 * texc_nb);
        for (i = 0; i < texc_nb; i++)
        {
            read2TexCoord(Var_getArrayElemByPos(v, i), texc + i * 2);
        }
    }
    else
    {
        texc = NULL;
    }
    
    /*filling the whole structure while processing faces*/
    v = Var_getArrayElemByCName(var, "faces");
    n = Var_getArraySize(v);
    if (n != 0)
    {
        unsigned int mode_normals;
        unsigned int mode_texcoords;
        unsigned int k = 0;     /*total vertex count*/
        
        /*read modes*/
        mode_normals = Var_getValueInt(Var_getArrayElemByCName(var, "normalsmode"));
        if (mode_normals > 2)
        {
            shellPrintf(LEVEL_ERROR, "Wrong 'normalsmode' value %d, assuming 2 instead.", mode_normals);
            mode_normals = 2;
        }
        mode_texcoords = Var_getValueInt(Var_getArrayElemByCName(var, "texcoordsmode"));
        if (mode_texcoords > 1)
        {
            shellPrintf(LEVEL_ERROR, "Wrong 'texcoordsmode' value %d, assuming 1 instead.", mode_texcoords);
            mode_texcoords = 1;
        }
        
        for (i = 0; i < n; i++)
        {
            Var vf;
            unsigned int j, nvf;
            Gl3DCoord* p_vert = NULL;
            Gl3DCoord* p_norm = NULL;
            TexCoord* p_texc = NULL;
            
            /*some checks*/
            vf = Var_getArrayElemByPos(v, i);
            if (Var_getType(vf) != VAR_ARRAY)
            {
                shellPrintf(LEVEL_ERROR, "Face %d not of array type.", i);
                continue;
            }
            
            nvf = Var_getArraySize(vf);

            /*allocating*/
            if (nvf == 3)
            {
                if (part->nbtriangles == 0)
                {
                    part->nbtriangles = 1;
                    part->vertices3 = MALLOC(sizeof(Gl3DCoord) * 3 * 3);
                    part->normals3 = MALLOC(sizeof(Gl3DCoord) * 3 * 3);
                    part->texcoords3 = MALLOC(sizeof(TexCoord) * 3 * 2);
                }
                else
                {
                    part->nbtriangles++;
                    part->vertices3 = REALLOC(part->vertices3, sizeof(Gl3DCoord) * part->nbtriangles * 3 * 3);
                    part->normals3 = REALLOC(part->normals3, sizeof(Gl3DCoord) * part->nbtriangles * 3 * 3);
                    part->texcoords3 = REALLOC(part->texcoords3, sizeof(TexCoord) * part->nbtriangles * 3 * 2);
                }
                p_vert = part->vertices3 + (part->nbtriangles - 1) * 3 * 3;
                p_norm = part->normals3 + (part->nbtriangles - 1) * 3 * 3;
                p_texc = part->texcoords3 + (part->nbtriangles - 1) * 3 * 2;
            }
            else if (nvf == 4)
            {
                if (part->nbquads == 0)
                {
                    part->nbquads = 1;
                    part->vertices4 = MALLOC(sizeof(Gl3DCoord) * 4 * 3);
                    part->normals4 = MALLOC(sizeof(Gl3DCoord) * 4 * 3);
                    part->texcoords4 = MALLOC(sizeof(TexCoord) * 4 * 2);
                }
                else
                {
                    part->nbquads++;
                    part->vertices4 = REALLOC(part->vertices4, sizeof(Gl3DCoord) * part->nbquads * 4 * 3);
                    part->normals4 = REALLOC(part->normals4, sizeof(Gl3DCoord) * part->nbquads * 4 * 3);
                    part->texcoords4 = REALLOC(part->texcoords4, sizeof(TexCoord) * part->nbquads * 4 * 2);
                }
                p_vert = part->vertices4 + (part->nbquads - 1) * 4 * 3;
                p_norm = part->normals4 + (part->nbquads - 1) * 4 * 3;
                p_texc = part->texcoords4 + (part->nbquads - 1) * 4 * 2;
            }
            else
            {
                shellPrintf(LEVEL_ERROR, "Wrong number of vertices for face %d.", i);
                continue;
            }

            /*vertices loop*/
            for (j = 0; j < nvf; j++)
            {
                /*vertices coordinates*/
                Var vv;
                unsigned int vn;
                
                vv = Var_getArrayElemByPos(vf, j);
                if ((Var_getType(vv) != VAR_INT) || ((vn = Var_getValueInt(vv)) >= vert_nb))
                {
                    shellPrintf(LEVEL_ERROR, "Wrong vertex number at position %d of face %d : %s", j, i, Var_gets(vv));
                    p_vert[j * 3] = 0.0f;
                    p_vert[j * 3 + 1] = 0.0f;
                    p_vert[j * 3 + 2] = 0.0f;
                }
                else
                {
                    p_vert[j * 3] = vert[vn * 3];
                    p_vert[j * 3 + 1] = vert[vn * 3 + 1];
                    p_vert[j * 3 + 2] = vert[vn * 3 + 2];
                }
                
                /*normal*/
                if (mode_normals == 2)
                {
                    /*normals given for each vertex inside each face*/
                    if (k >= norm_nb)
                    {
                        shellPrintf(LEVEL_ERROR, "Normal vector not found for vertex number %d inside face %d.", j, i);
                        p_norm[j * 3] = 0.0f;
                        p_norm[j * 3 + 1] = 0.0f;
                        p_norm[j * 3 + 2] = 0.0f;
                    }
                    else
                    {
                        p_norm[j * 3] = norm[k * 3];
                        p_norm[j * 3 + 1] = norm[k * 3 + 1];
                        p_norm[j * 3 + 2] = norm[k * 3 + 2];
                    }
                }
                else if (mode_normals == 1)
                {
                    /*normals given for each vertex*/
                    Var vv;
                    unsigned int vn;
                    
                    vv = Var_getArrayElemByPos(vf, j);
                    if ((Var_getType(vv) != VAR_INT) || ((vn = Var_getValueInt(vv)) >= norm_nb))
                    {
                        shellPrintf(LEVEL_ERROR, "Normal vector not found for vertex number %d of face %d.", j, i);
                        p_norm[j * 3] = 0.0f;
                        p_norm[j * 3 + 1] = 0.0f;
                        p_norm[j * 3 + 2] = 0.0f;
                    }
                    else
                    {
                        p_norm[j * 3] = norm[vn * 3];
                        p_norm[j * 3 + 1] = norm[vn * 3 + 1];
                        p_norm[j * 3 + 2] = norm[vn * 3 + 2];
                    }
                }
                else /*mode_normals == 0*/
                {
                    /*normals given for each face*/
                    if (i >= norm_nb)
                    {
                        shellPrintf(LEVEL_ERROR, "Normal vector not found for face %d.", i);
                        p_norm[j * 3] = 0.0f;
                        p_norm[j * 3 + 1] = 0.0f;
                        p_norm[j * 3 + 2] = 0.0f;
                    }
                    else
                    {
                        p_norm[j * 3] = norm[i * 3];
                        p_norm[j * 3 + 1] = norm[i * 3 + 1];
                        p_norm[j * 3 + 2] = norm[i * 3 + 2];
                    }
                }
                
                /*texture coordinates*/
                if (mode_texcoords == 1)
                {
                    /*normals given for each vertex inside each face*/
                    if (k >= texc_nb)
                    {
                        shellPrintf(LEVEL_ERROR, "Texture coordinates not found for vertex number %d of face %d.", j, i);
                        p_texc[j * 2] = 0.0f;
                        p_texc[j * 2 + 1] = 0.0f;
                    }
                    else
                    {
                        p_texc[j * 2] = texc[k * 2];
                        p_texc[j * 2 + 1] = texc[k * 2 + 1];
                    }
                }
                else /*mode_texcoords == 0*/
                {
                    /*coordinates given for each vertex*/
                    Var vv;
                    unsigned int vn;
                    
                    vv = Var_getArrayElemByPos(vf, j);
                    if ((Var_getType(vv) != VAR_INT) || ((vn = Var_getValueInt(vv)) >= texc_nb))
                    {
                        shellPrintf(LEVEL_ERROR, "Texture coordinates not found for vertex number %d of face %d.", j, i);
                        p_texc[j * 2] = 0.0f;
                        p_texc[j * 2 + 1] = 0.0f;
                    }
                    else
                    {
                        p_texc[j * 2] = texc[vn * 2];
                        p_texc[j * 2 + 1] = texc[vn * 2 + 1];
                    }
                }
                
                k++;
            }
        }
    }

    if (vert_nb != 0)
    {
        FREE(vert);
    }
    if (norm_nb != 0)
    {
        FREE(norm);
    }
    if (texc_nb != 0)
    {
        FREE(texc);
    }
    shellPopErrorStack();
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GlMeshPart
GlMeshPart_new(Var v)
{
    GlMeshPart ret;
    
    ret = (GlMeshPart)MALLOC(sizeof(pv_GlMeshPart));
    ret->nbtriangles = 0;
    ret->nbquads = 0;
    GlMeshPart_setFromVar(ret, v);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlMeshPart_del(GlMeshPart part)
{
    if (part->nbtriangles != 0)
    {
        FREE(part->vertices3);
        FREE(part->texcoords3);
        FREE(part->normals3);
    }
    if (part->nbquads != 0)
    {
        FREE(part->vertices4);
        FREE(part->texcoords4);
        FREE(part->normals4);
    }
    FREE(part);
}

/*----------------------------------------------------------------------------*/
void
GlMeshPart_draw(GlMeshPart part)
{
    if (part->twosided)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }
    if (global_forceblend || part->blended)
    {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
    }
    else
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
    GlStaticTexture_use(part->tex);
    glShadeModel(part->shademode);
    if (part->nbtriangles != 0)
    {
        glVertexPointer(3, GL_FLOAT, 0, part->vertices3);
        glNormalPointer(GL_FLOAT, 0, part->normals3);
        glTexCoordPointer(2, GL_FLOAT, 0, part->texcoords3);
        glDrawArrays(GL_TRIANGLES, 0, part->nbtriangles * 3);
        openglCount(part->nbtriangles);
    }
    if (part->nbquads != 0)
    {
        glVertexPointer(3, GL_FLOAT, 0, part->vertices4);
        glNormalPointer(GL_FLOAT, 0, part->normals4);
        glTexCoordPointer(2, GL_FLOAT, 0, part->texcoords4);
        glDrawArrays(GL_QUADS, 0, part->nbquads * 4);
        openglCount(part->nbquads);
    }
}
