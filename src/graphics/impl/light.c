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
 *   Lights                                                                   *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/light.h"

#include "graphics/color.h"
#include "graphics/impl/opengl.h"
#include "core/ptrarray.h"
#include "tools/fonct.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                 Structures                                 *
 ******************************************************************************/
struct pv_Light
{
    unsigned int nb;        /* Light number. */
    Bool changed;           /* The state of this light has changed. */
    Bool lit;               /* Specifies if the light is lit or not. */
    float col_amb[4];       /* Ambient color ([0.0-1.0] ranges). */
    float col_dif[4];       /* Diffuse color ([0.0-1.0] ranges). */
    float col_spec[4];      /* Specular color ([0.0-1.0] ranges). */
    float pos[4];           /* Absolute position. */
    float dir[3];           /* Direction. */
    LightFactor exponent;   /* Exponent. */
    LightFactor cutoff;     /* Cutoff. */
    LightFactor att_cons;   /* Constant attenuation factor. */
    LightFactor att_lin;    /* Linear attenuation factor. */
    LightFactor att_quad;   /* Quadratic attenuation factor. */
};

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Light* lights;
static unsigned int MAX_LIGHTS = 1;
static unsigned int firstfree = 0;

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
lightSetScene(void)
{
    unsigned int i, li;
    Light l;

    li = GL_LIGHT0;
    for (i = 0; i < MAX_LIGHTS; i++, li++)
    {
        l = lights[i];
        if (l == NULL)
        {
            glDisable(li);
        }
        else if (l->changed)
        {
            if (l->lit)
            {
                glEnable(li);
                
                glLightfv(li, GL_AMBIENT, l->col_amb);
                glLightfv(li, GL_DIFFUSE, l->col_dif);
                glLightfv(li, GL_SPECULAR, l->col_spec);
                
                glLightfv(li, GL_POSITION, l->pos);
                glLightfv(li, GL_SPOT_DIRECTION, l->dir);
                glLightf(li, GL_SPOT_EXPONENT, l->exponent);
                glLightf(li, GL_SPOT_CUTOFF, l->cutoff);
                glLightf(li, GL_CONSTANT_ATTENUATION, l->att_cons);
                glLightf(li, GL_LINEAR_ATTENUATION, l->att_lin);
                glLightf(li, GL_QUADRATIC_ATTENUATION, l->att_quad);
            }
            else
            {
                glDisable(li);
            }
        }
    }
}

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
lightInit()
{
    int i;
    
    shellPrint(LEVEL_INFO, "Lights module loaded.");
    glGetIntegerv(GL_MAX_LIGHTS, &i);
    MAX_LIGHTS = i;
    lights = MALLOC(sizeof(Light) * (MAX_LIGHTS + 1));
    firstfree = 0;
    shellPrintf(LEVEL_INFO, " -> maximal number of OpenGL lights: %d", i);
    for (i = 0; i <= (int)MAX_LIGHTS; i++)
    {
        lights[i] = NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
lightUninit()
{
    FREE(lights);
    
    shellPrint(LEVEL_INFO, "Lights module unloaded.");
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Light
lightAdd()
{
    Light ret = NULL;
    
    if (firstfree == MAX_LIGHTS)
    {
        shellPrintf(LEVEL_DEBUG, "Too much lights...");
    }
    else
    {
        ret = (Light)MALLOC(sizeof(pv_Light));
        ret->nb = firstfree;
        ret->changed = TRUE;
        ret->lit = FALSE;
        ret->col_amb[0] = 0.0f;
        ret->col_amb[1] = 0.0f;
        ret->col_amb[2] = 0.0f;
        ret->col_amb[3] = 0.0f;
        ret->col_dif[0] = 0.0f;
        ret->col_dif[1] = 0.0f;
        ret->col_dif[2] = 0.0f;
        ret->col_dif[3] = 0.0f;
        ret->col_spec[0] = 0.0f;
        ret->col_spec[1] = 0.0f;
        ret->col_spec[2] = 0.0f;
        ret->col_spec[3] = 0.0f;
        ret->pos[0] = 0.0f;
        ret->pos[1] = 0.0f;
        ret->pos[2] = 0.0f;
        ret->pos[3] = 0.0f;
        ret->dir[0] = 0.0f;
        ret->dir[1] = 1.0f;
        ret->dir[2] = 0.0f;
        ret->exponent = 0.0f;
        ret->cutoff = 180.0f;
        ret->att_cons = 1.0f;
        ret->att_lin = 0.0f;
        ret->att_quad = 0.0f;
        
        lights[firstfree++] = ret;
        while (lights[firstfree] != NULL)
        {
            firstfree++;
        }
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
lightDel(Light li)
{
    unsigned int nb;
    
    if (li == NULL)
    {
        return;
    }
    
    ASSERT(li->nb < MAX_LIGHTS, return);
    ASSERT(li == lights[li->nb], return);
    
    nb = li->nb;
    FREE(li);
    lights[nb] = NULL;
    if (firstfree > nb)
    {
        firstfree = nb;
    }
}

/*----------------------------------------------------------------------------*/
void
lightDelAll(void)
{
    unsigned int i;
    
    for (i = 0; i <= MAX_LIGHTS; i++)
    {
        if (lights[i] != NULL)
        {
            FREE(lights[i]);
            lights[i] = NULL;
        }
    }
    firstfree = 0;
}

/*----------------------------------------------------------------------------*/
void
lightToggle(Light li, Bool lit)
{
    if (li == NULL)
    {
        return;
    }
    
    li->lit = lit;
    li->changed = TRUE;
}

/*----------------------------------------------------------------------------*/
void
lightSetFromVar(Light li, Var v)
{
    VarValidator valid;
    GlColorRGB ca, cd, cs;
    
    if (li == NULL)
    {
        return;
    }

    li->changed = TRUE;
    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "col_ambient");
    VarValidator_declareArrayVar(valid, "col_diffuse");
    VarValidator_declareArrayVar(valid, "col_specular");
    VarValidator_declareFloatVar(valid, "posx", 0.0);
    VarValidator_declareFloatVar(valid, "posy", 0.0);
    VarValidator_declareFloatVar(valid, "posz", 0.0);
    VarValidator_declareFloatVar(valid, "angh", 0.0);
    VarValidator_declareFloatVar(valid, "angv", 0.0);
    VarValidator_declareIntVar(valid, "attmode", 0);
    VarValidator_declareFloatVar(valid, "attfactor", 1.0);
    VarValidator_declareFloatVar(valid, "exponent", 0.0);
    VarValidator_declareFloatVar(valid, "cutoff", 180.0);
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    GlColorRGB_makeFromVar(&ca, Var_getArrayElemByCName(v, "col_ambient"));
    GlColorRGB_makeFromVar(&cd, Var_getArrayElemByCName(v, "col_diffuse"));
    GlColorRGB_makeFromVar(&cs, Var_getArrayElemByCName(v, "col_specular"));
    lightSetColor(li, ca, cd, cs);
    
    lightSetPos(li,
                Var_getValueFloat(Var_getArrayElemByCName(v, "posx")),
                Var_getValueFloat(Var_getArrayElemByCName(v, "posy")),
                Var_getValueFloat(Var_getArrayElemByCName(v, "posz")));
    lightSetDirection(li,
                      Var_getValueFloat(Var_getArrayElemByCName(v, "angh")),
                      Var_getValueFloat(Var_getArrayElemByCName(v, "angv")));
    lightSetParams(li,
                   Var_getValueFloat(Var_getArrayElemByCName(v, "exponent")),
                   Var_getValueFloat(Var_getArrayElemByCName(v, "cutoff")),
                   Var_getValueFloat(Var_getArrayElemByCName(v, "attfactor")),
                   Var_getValueInt(Var_getArrayElemByCName(v, "attmode")));
}

/*----------------------------------------------------------------------------*/
void
lightSetColor(Light li, GlColorRGB col_ambient, GlColorRGB col_diffuse, GlColorRGB col_specular)
{
    if (li == NULL)
    {
        return;
    }
    
    li->col_amb[0] = ((float)col_ambient.r) / 255.0;
    li->col_amb[1] = ((float)col_ambient.g) / 255.0;
    li->col_amb[2] = ((float)col_ambient.b) / 255.0;
    li->col_amb[3] = 1.0;
    li->col_dif[0] = ((float)col_diffuse.r) / 255.0;
    li->col_dif[1] = ((float)col_diffuse.g) / 255.0;
    li->col_dif[2] = ((float)col_diffuse.b) / 255.0;
    li->col_dif[3] = 1.0;
    li->col_spec[0] = ((float)col_specular.r) / 255.0;
    li->col_spec[1] = ((float)col_specular.g) / 255.0;
    li->col_spec[2] = ((float)col_specular.b) / 255.0;
    li->col_spec[3] = 1.0;
    li->changed = TRUE;
}

/*----------------------------------------------------------------------------*/
void
lightSetPos(Light li, Gl3DCoord posx, Gl3DCoord posy, Gl3DCoord posz)
{
    if (li == NULL)
    {
        return;
    }
    
    /*FIXME: check coordinates system*/
    li->pos[0] = posx;
    li->pos[1] = posy;
    li->pos[2] = posz;
    li->changed = TRUE;
}

/*----------------------------------------------------------------------------*/
void
lightSetDirection(Light li, Gl3DCoord angh, Gl3DCoord angv)
{
    if (li == NULL)
    {
        return;
    }
    
    /*FIXME: correct direction*/
    li->dir[0] = cos(angh) * cos(angv);
    li->dir[1] = sin(angv);
    li->dir[2] = sin(angh) * cos(angv);
    li->changed = TRUE;
}

/*----------------------------------------------------------------------------*/
void
lightSetParams(Light li, LightFactor exponent, LightFactor cutoff, LightFactor attenuation, LightAttMode attmode)
{
    if (li == NULL)
    {
        return;
    }
    
    /*TODO: check values*/
    li->exponent = exponent;
    li->cutoff = cutoff;
    if (attmode == LIGHT_ATTCONSTANT)
    {
        li->att_cons = attenuation;
        li->att_lin = 0.0f;
        li->att_quad = 0.0f;
    }
    else if (attmode == LIGHT_ATTLINEAR)
    {
        li->att_cons = 0.0f;
        li->att_lin = attenuation;
        li->att_quad = 0.0f;
    }
    else /*attmode == LIGHT_ATTLINEAR*/
    {
        li->att_cons = 0.0f;
        li->att_lin = 0.0f;
        li->att_quad = attenuation;
    }
    li->changed = TRUE;
}
