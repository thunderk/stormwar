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
 *   GUI object                                                               *
 *                                                                            *
  ***************************************************************************/


/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiwidget.h"
#include "gui/internal/guitooltip.h"
#include "core/string.h"
#include "core/ptrarray.h"
#include "tools/fonct.h"

#include "graphics/color.h"
#include "graphics/glrect.h"
#include "graphics/glsurface.h"
#include "graphics/gl2dobject.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define CHILD_NONE -1
#define CHILD_SELF -2

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GuiWidget
{
    GuiWidget parent;               /*parent widget, NULL if top-level*/
    void* holder;                   /*pointer to the holder*/
    String tooltip;                 /*tooltip text, NULL if not*/
    unsigned int childnb;           /*identifier used by the parent*/
    Gl2DObject globj;               /*the top-level object, only the parent can modify it*/
    GlSurface parentsurf;           /*surface where the widget will be blitted*/
    GlSurface savesurf;             /*surface to save the 'background', NULL if the widget is a container*/
    GlSurface drawsurf;             /*surface where the widget will be drawn, NULL if the widget is a container*/
    GlRect rect;                    /*rectangle effectively occupied inside the parent surface (top level)*/
    Sint16 alt;                     /*altitude (top-level)*/
    Bool contentchanged;            /*TRUE if the content of drawsurf changed*/
    GuiEventCallback upcallback;    /*callback called when an event is received from the parent*/
    GlEventCallback downcallback;   /*callback called when an event is received from a child*/
    unsigned int nbchildren;        /*number of children*/
    GuiWidget* children;            /*childrens, a child can be NULL*/
    int focus;                      /*child that has the focus*/
    int holdmouse;                  /*child that hold a mouse button*/
};

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static PtrArray _autolevel;     /*Widgets with automatic altitude.*/
static Sint16 _nextlevel;       /*Next available level for automatic altitude.*/

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
refocus(GuiWidget widget)
{
    /*we search for the first not null child*/
    widget->focus = 0;
    while ((widget->focus < (int)widget->nbchildren) && (widget->children[widget->focus] == NULL))
    {
        widget->focus++;
    }
    if (widget->focus == (int)widget->nbchildren)
    {
        widget->focus = CHILD_NONE;
    }
}

/*----------------------------------------------------------------------------*/
static void
correctChildrenParentSurf(GuiWidget widget, GlSurface parentsurf)
{
    unsigned int i;
        
    for (i = 0; i < widget->nbchildren; i++)
    {
        if (widget->children[i] != NULL)
        {
            widget->children[i]->parentsurf = parentsurf;
            correctChildrenParentSurf(widget->children[i], parentsurf);
        }
    }
}

/*----------------------------------------------------------------------------*/
static Bool
GuiWidget_draw(GuiWidget widget, Bool parentchanged)
{
    unsigned int i;
    Bool redraw;
    
    if (parentchanged)
    {
        if (widget->savesurf != NULL)
        {
            /*save the parent surface*/
            GlSurface_copyRect(widget->parentsurf, widget->savesurf, &widget->rect, NULL);
            
            /*blit the drawing surface*/
            GlSurface_doBlit(widget->drawsurf, widget->parentsurf, NULL, &widget->rect);
        }
        
        /*pass to children*/
        for (i = 0; i < widget->nbchildren; i++)
        {
            if (widget->children[i] != NULL)
            {
                GuiWidget_draw(widget->children[i], TRUE);
            }
        }
        
        redraw = TRUE;
    }
    else
    {
        redraw = FALSE;
        
        if (widget->contentchanged)
        {
            ASSERT(widget->savesurf != NULL, return FALSE);
            
            /*restore the parent*/
            GlSurface_copyRect(widget->savesurf, widget->parentsurf, NULL, &widget->rect);
            
            /*blit the new content*/
            GlSurface_doBlit(widget->drawsurf, widget->parentsurf, NULL, &widget->rect);
            
            redraw = TRUE;
        }
        
        /*pass to children*/
        for (i = 0; i < widget->nbchildren; i++)
        {
            if ((widget->children[i] != NULL) && (GuiWidget_draw(widget->children[i], widget->contentchanged)))
            {
                redraw = TRUE;  /*at least one child had to redraw itself*/
            }
        }
    }
    
    widget->contentchanged = FALSE;
    return redraw;
}

/*----------------------------------------------------------------------------*/
/*returns TRUE if the size has effectively changed*/
static Bool
GuiWidget_setRect(GuiWidget widget, GlRect rct)
{
    Bool ret = FALSE;
    
    if ((rct.w != widget->rect.w) | (rct.h != widget->rect.h))
    {
        if (widget->savesurf != NULL)       /*not parent and not container*/
        {
            /*resize storing surfaces*/
            GlSurface_resize(widget->savesurf, rct.w, rct.h, SURFACE_UNDEFINED);
        }
        
        /*resize drawing surface*/
        GlSurface_resize(widget->drawsurf, rct.w, rct.h, SURFACE_UNDEFINED);
        GlSurface_clear(widget->drawsurf, GlColor_MAKE(0x00, 0x00, 0x00, 0xFF));
        
        /*tell the children if the top-level surface changed*/
        if (widget->parent == NULL)
        {
            correctChildrenParentSurf(widget, widget->drawsurf);
            Gl2DObject_relink(widget->globj, widget->drawsurf);
        }
        widget->contentchanged = TRUE;
        ret = TRUE;
    }

    /*set the new absolute rectangle*/
    if (widget->parent == NULL)
    {
        Gl2DObject_setPos(widget->globj, rct.x, rct.y, 0);
        GlRect_MAKE(widget->rect, 0, 0, rct.w, rct.h);
    }
    else
    {
        widget->rect = rct;
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
static Bool
GuiWidget_processEventDown(GuiWidget widget, GlEvent* event)
{
    GlEvent ev, evf;
    unsigned int i;
    Sint16 posx, posy;
    Bool r;
    
    ASSERT(widget != NULL, return FALSE);
    ASSERT(widget->downcallback != NULL, return FALSE);

    /*Unlike upward events, we can assume here that the sender is always the parent.*/
    
    switch (event->type)
    {
        case GLEVENT_DELETE:
            /*forward to the holder*/
            ev.type = GLEVENT_DELETE;
            widget->downcallback(widget, &ev);
        
            if (widget->tooltip != NULL)
            {
                String_del(widget->tooltip);
            }
            if (widget->parent == NULL)
            {
                if (widget->alt < 0)
                {
                    PtrArrayIterator it;
                    GuiWidget w;
                    
                    PtrArray_removeFast(_autolevel, widget);
                    for (it = PtrArray_START(_autolevel); it != PtrArray_STOP(_autolevel); it++)
                    {
                        w = (GuiWidget)*it;
                        if (w->alt < widget->alt)
                        {
                            w->alt--;
                            Gl2DObject_setAlt(w->globj, ABS(w->alt));
                        }
                    }
                    _nextlevel++;
                }
                GlSurface_del(widget->drawsurf);
            }
            else
            {
                if (widget->drawsurf != NULL)
                {
                    GlSurface_del(widget->drawsurf);
                    GlSurface_del(widget->savesurf);
                }
            }
            FREE(widget);
            return TRUE;
    
        case GLEVENT_DRAW:
            r = FALSE;
            for (i = 0; i < widget->nbchildren; i++)
            {
                if ((widget->children[i] != NULL) && GuiWidget_draw(widget->children[i], widget->contentchanged))
                {
                    r = TRUE;
                }
            }
            if (r | widget->contentchanged)
            {
                Gl2DObject_redraw(widget->globj);
            }
            widget->contentchanged = FALSE;
            break;
            
        case GLEVENT_MOUSE:
            /*make coordinates relative*/
            ev = *event;
            if (widget->parent == NULL)
            {
                ev.event.mouseevent.x -= Gl2DObject_getX(widget->globj);
                ev.event.mouseevent.y -= Gl2DObject_getY(widget->globj);
            }
            evf = ev;
            
            /*tooltip*/
            if (widget->tooltip != NULL)
            {
                GlRect rct = widget->rect;
                rct.x += Gl2DObject_getX(widget->globj);
                rct.y += Gl2DObject_getY(widget->globj);
                guitooltipSet(widget->tooltip, &rct, widget);
            }
            
            /*automatic altitude*/
            if ((widget->alt < 0) && (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED))
            {
                PtrArrayIterator it;
                GuiWidget w;
                
                for (it = PtrArray_START(_autolevel); it != PtrArray_STOP(_autolevel); it++)
                {
                    w = (GuiWidget)*it;
                    if (w->alt < widget->alt)
                    {
                        w->alt++;
                        Gl2DObject_setAlt(w->globj, ABS(w->alt));
                    }
                }
                widget->alt = _nextlevel + 1;
                Gl2DObject_setAlt(widget->globj, ABS(widget->alt));
            }
            
            /*forward downwards to the child that holds the mouse or to the child below the cursor*/
            if (widget->holdmouse == CHILD_SELF)
            {
                /*the widget itself has the mouse*/
                if (ev.event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
                {
                    widget->holdmouse = CHILD_NONE;
                }
                
                /*forward to the holder*/
                widget->downcallback(widget, &ev);
            }
            else if (widget->holdmouse >= 0)
            {
                ASSERT(widget->holdmouse < (int)widget->nbchildren, break);
                ASSERT(widget->children[widget->holdmouse] != NULL, break);
                
                ev.event.mouseevent.x += widget->rect.x - widget->children[widget->holdmouse]->rect.x;
                ev.event.mouseevent.y += widget->rect.y - widget->children[widget->holdmouse]->rect.y;
                GuiWidget_processEventDown(widget->children[widget->holdmouse], &ev);
                
                if (ev.event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
                {
                    widget->holdmouse = CHILD_NONE;
                }
            }
            else
            {
                for (i = 0; i < widget->nbchildren; i++)
                {
                    if (widget->children[i] != NULL)
                    {
                        posx = ev.event.mouseevent.x + widget->rect.x;
                        posy = ev.event.mouseevent.y + widget->rect.y;
                        if (isInRect(&widget->children[i]->rect, posx, posy))
                        {
                            ev.event.mouseevent.x = posx - widget->children[i]->rect.x;
                            ev.event.mouseevent.y = posy - widget->children[i]->rect.y;
                            GuiWidget_processEventDown(widget->children[i], &ev);
                            
                            if (ev.event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
                            {
                                widget->holdmouse = i;
                                widget->focus = i;
                                break;
                            }
                        }
                    }
                }
                if (i == widget->nbchildren)
                {
                    /*no child got the mouse, forward to the holder*/
                    widget->downcallback(widget, &ev);
                    if (ev.event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
                    {
                        widget->holdmouse = CHILD_SELF;
                        widget->focus = CHILD_NONE;
                    }
                }
            }
            return TRUE;
            
        case GLEVENT_KEYBOARD:
            /*forward downwards to the child that owns the keyboard focus*/
            if (widget->focus >= 0)
            {
                ASSERT(widget->focus < (int)widget->nbchildren, break);
                ASSERT(widget->children[widget->focus] != NULL, break);
                return GuiWidget_processEventDown(widget->children[widget->focus], event);
            }
            else
            {
                /*forward to the holder*/
                return widget->downcallback(widget, event);
            }
            
        case GLEVENT_RESIZE:
            /*the widget's rect has already been modified by the parent*/
        
            /*forward to the holder*/
            return widget->downcallback(widget, event);
        
        default:
            ;
    }
    
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
guiwidgetInit(void)
{
    _autolevel = PtrArray_new();
    _nextlevel = -((GL2D_HIGHEST + GL2D_LOWEST) / 3);
}

/*----------------------------------------------------------------------------*/
void
guiwidgetUninit(void)
{
    PtrArray_del(_autolevel);
}

/*----------------------------------------------------------------------------*/
GuiWidget
GuiWidget_new(GuiWidget parent, void* holder, const char* tooltip, Bool container, GuiEventCallback upcallback, GlEventCallback downcallback)
{
    GuiWidget ret;
    GlColorRGBA col;
    
    ret = MALLOC(sizeof(pv_GuiWidget));
    ret->parent = parent;
    ret->holder = holder;
    if (tooltip == NULL)
    {
        ret->tooltip = NULL;
    }
    else
    {
        ret->tooltip = String_new(tooltip);
    }
    ret->contentchanged = FALSE;
    
    if (parent == NULL)
    {
        ret->drawsurf = GlSurface_new(10, 10, TRUE);
        GlSurface_clear(ret->drawsurf, GlColor_MAKE(0x00, 0x00, 0x00, 0xFF));
        ret->globj = Gl2DObject_new(ret->drawsurf, ret, (GlEventCallback)GuiWidget_processEventDown);
        ret->alt = GL2D_LOWEST;
        Gl2DObject_setAlt(ret->globj, GL2D_LOWEST);
        GlRect_MAKE(ret->rect, Gl2DObject_getX(ret->globj), Gl2DObject_getY(ret->globj), Gl2DObject_getWidth(ret->globj), Gl2DObject_getHeight(ret->globj));
        ret->parentsurf = NULL;
        ret->savesurf = NULL;

        /*effect*/
        GlColorRGBA_MAKE(col, 0xFF, 0xFF, 0xFF, 0x00);
        Gl2DObject_setColor(ret->globj, col, 0);
        col.a = 0xFF;
        Gl2DObject_setColor(ret->globj, col, 500);
    }
    else
    {
        GlRect_MAKE(ret->rect, 0, 0, 1, 1);
        if (parent->parent == NULL)
        {
            /*the parent is top-level*/
            ret->parentsurf = parent->drawsurf;
        }
        else
        {
            ret->parentsurf = parent->parentsurf;
        }
        if (container)
        {
            ret->drawsurf = NULL;
            ret->savesurf = NULL;
        }
        else
        {
            ret->savesurf = GlSurface_new(ret->rect.w, ret->rect.h, TRUE);
            ret->drawsurf = GlSurface_new(ret->rect.w, ret->rect.h, TRUE);
        }
        ret->globj = NULL;
    }
    ret->childnb = 0;
    ret->downcallback = downcallback;
    ret->upcallback = upcallback;
    ret->nbchildren = 0;
    ret->children = NULL;
    ret->focus = CHILD_NONE;
    ret->holdmouse = CHILD_NONE;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_del(GuiWidget widget)
{
    /*delete children immediately*/
    if (widget->nbchildren != 0)
    {
        FREE(widget->children);
        widget->nbchildren = 0;
    }
    
    /*delete the rest*/
    if (widget->parent == NULL)
    {
        /*later*/
        Gl2DObject_del(widget->globj);
    }
    else
    {
        /*now*/
        GlEvent ev;
        
        ev.type = GLEVENT_DELETE;
        GuiWidget_processEventDown(widget, &ev);
    }
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_topLevelSet(GuiWidget widget, Sint16 alt, GlRect rct)
{
    PtrArrayIterator it;
    GuiWidget w;
    
    ASSERT(widget->parent == NULL, return);
    
    /*automatic altitude management*/
    if (widget->alt < 0)
    {
        if (alt >= 0)
        {
            PtrArray_removeFast(_autolevel, widget);
            for (it = PtrArray_START(_autolevel); it != PtrArray_STOP(_autolevel); it++)
            {
                w = (GuiWidget)*it;
                if (w->alt < widget->alt)
                {
                    w->alt++;
                    Gl2DObject_setAlt(w->globj, ABS(w->alt));
                }
            }
            _nextlevel++;
        }
    }
    else
    {
        if (alt < 0)
        {
            PtrArray_append(_autolevel, widget);
            widget->alt = _nextlevel--;
            Gl2DObject_setAlt(widget->globj, ABS(widget->alt));
        }
    }

    if (alt > 0)
    {        
        widget->alt = alt;
        Gl2DObject_setAlt(widget->globj, ABS(widget->alt));
    }
    GuiWidget_setRect(widget, rct);
}

/*----------------------------------------------------------------------------*/
void*
GuiWidget_getHolder(GuiWidget widget)
{
    return widget->holder;
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_show(GuiWidget widget, Bool show)
{
    if (widget->parent == NULL)
    {
        Gl2DObject_show(widget->globj, show);
    }
    else
    {
        /*TODO*/
        ;
    }
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_setNbChild(GuiWidget widget, unsigned int nb)
{
    unsigned int i;
    
    if (nb != widget->nbchildren)
    {
        if (nb == 0)
        {
            FREE(widget->children);
        }
        else
        {
            if (widget->nbchildren == 0)
            {
                widget->children = MALLOC(sizeof(GuiWidget) * nb);
            }
            else
            {
                widget->children = REALLOC(widget->children, sizeof(GuiWidget) * nb);
            }
        }
        
        /*define newly allocated children to NULL*/
        for (i = widget->nbchildren; i < nb; i++)
        {
            widget->children[i] = NULL;
        }
        
        widget->nbchildren = nb;

        /*correct values*/
        if (widget->focus >= (int)nb)
        {
            refocus(widget);
        }
        if (widget->holdmouse >= (int)nb)
        {
            widget->holdmouse = CHILD_NONE;
        }
    }
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_setChild(GuiWidget widget, unsigned int childnb, GuiWidget child)
{
    GlRect rct;
    GlEvent event;
    
    ASSERT(childnb < widget->nbchildren, return);
    ASSERT(child->parent == widget, return);
    
    widget->children[childnb] = child;
    if (child == NULL)
    {
        if (widget->focus == (int)childnb)
        {
            refocus(widget);
        }
        if (widget->holdmouse == (int)childnb)
        {
            widget->holdmouse = CHILD_NONE;
        }
        return;
    }
    
    child->childnb = childnb;
    
    /*check the child's allocation*/
    rct = child->rect;
    GlRect_clip(&rct, &widget->rect);
    GuiWidget_setChildRect(widget, childnb, rct);
    
    if (widget->focus == CHILD_NONE)
    {
        widget->focus = childnb;
    }
    
    child->globj = widget->globj;

    /*tell the child it's size*/
    event.type = GLEVENT_RESIZE;
    event.event.resizeevent.width = rct.w;
    event.event.resizeevent.height = rct.h;
    GuiWidget_processEventDown(child, &event);
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_setChildRect(GuiWidget widget, unsigned int childnb, GlRect rect)
{
    GlEvent event;
    
    ASSERT(childnb < widget->nbchildren, return);
    ASSERT(widget->children[childnb] != NULL, return);
    
    if (widget->parent != NULL)
    {
        /*coordinates are given inside the allocated area*/
        rect.x += widget->rect.x;
        rect.y += widget->rect.y;
    }

    /*clip the rect*/
    GlRect_clip(&rect, &widget->rect);
    if ((rect.w == 0) | (rect.h == 0))
    {
        rect = widget->rect;
    }

    /*change the widget*/
    if (GuiWidget_setRect(widget->children[childnb], rect))
    {
        /*tell the child if the size changed*/
        event.type = GLEVENT_RESIZE;
        event.event.resizeevent.width = rect.w;
        event.event.resizeevent.height = rect.h;
        GuiWidget_processEventDown(widget->children[childnb], &event);
    }
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_throwEvent(GuiWidget widget, GuiEvent event)
{
    if ((widget->parent != NULL) && (widget->parent->upcallback != NULL))
    {
        /*forward upwards*/
        widget->parent->upcallback(widget->parent, widget->childnb, event);
    }
}

/*----------------------------------------------------------------------------*/
void
GuiWidget_redraw(GuiWidget widget)
{
    widget->contentchanged = TRUE;
}

/*----------------------------------------------------------------------------*/
GlSurface
GuiWidget_getDrawingSurface(GuiWidget widget)
{
    return widget->drawsurf;
}
