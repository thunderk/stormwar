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
 *   Types for the graphical engine                                           *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_TYPES_H_
#define _SW_GRAPHICS_TYPES_H_ 1

/*!
 * \file
 * \brief Types for the graphical engine.
 *
 * \todo Multiple selection for 3D objects.
 * \todo Externalize selection handling.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private type for a 2D surface.
 */
typedef struct pv_GlSurface pv_GlSurface;

/*!
 * \brief Abstract type for 2D surfaces.
 */
typedef pv_GlSurface* GlSurface;

/*!
 * \brief Private structure of a 2D object.
 */
typedef struct pv_Gl2DObject pv_Gl2DObject;

/*!
 * \brief Abstract type for a 2D object.
 */
typedef pv_Gl2DObject* Gl2DObject;

/*!
 * \brief Type for 2D coordinates.
 */
typedef Sint16 Gl2DCoord;

/*!
 * \brief Type for 2D sizes.
 */
typedef Uint16 Gl2DSize;

/*!
 * \brief Private structure of a 3D object.
 */
typedef struct pv_Gl3DObject pv_Gl3DObject;

/*!
 * \brief Abstract type for a 3D object.
 */
typedef pv_Gl3DObject* Gl3DObject;

/*!
 * \brief Private structure of a 3D group.
 */
typedef struct pv_Gl3DGroup pv_Gl3DGroup;

/*!
 * \brief Abstract type for a 3D group.
 */
typedef pv_Gl3DGroup* Gl3DGroup;

/*!
 * \brief Type for 3D coordinates.
 */
typedef Float Gl3DCoord;

/*!
 * \brief Private structure for a GlMesh.
 */
typedef struct pv_GlMesh pv_GlMesh;

/*!
 * \brief Abstract type for 3d objects' graphical datas.
 */
typedef pv_GlMesh* GlMesh;

#ifndef _SDL_H
/*!
 * \brief Type for 2D rectangles.
 */
typedef struct
{
    Gl2DCoord x;    /*!< X coordinate of the top-left corner. */
    Gl2DCoord y;    /*!< Y coordinate of the top-left corner. */
    Gl2DSize  w;    /*!< Rectangle's width. */
    Gl2DSize  h;    /*!< Rectangle's height. */
} GlRect;
#else
typedef SDL_Rect GlRect;
#endif

/*!
 * \brief Type for RGB colours.
 */
typedef struct
{
    Uint8 r;    /*!< Red value (0-255) */
    Uint8 g;    /*!< Green value (0-255) */
    Uint8 b;    /*!< Blue value (0-255) */
} GlColorRGB;

/*!
 * \brief Type for RGBA colours.
 */
typedef struct
{
    Uint8 r;    /*!< Red value (0-255) */
    Uint8 g;    /*!< Green value (0-255) */
    Uint8 b;    /*!< Blue value (0-255) */
    Uint8 a;    /*!< Alpha value (0-255) */
} GlColorRGBA;

/*!
 * \brief Type for final rendering colours.
 */
typedef Uint32 GlColor;

/*!
 * \brief Filters for textures.
 */
typedef enum
{
    GLTEX_NONE = 0,
    GLTEX_BILINEAR = 1,
    GLTEX_MIPMAPPED = 2,
    GLTEX_TRILINEAR = 3
} GlTextureFilter;

/*!
 * \brief Type for a camera.
 */
typedef struct
{
    Gl3DCoord posx;
    Gl3DCoord posy;
    Gl3DCoord posz;
    Gl3DCoord lookx;
    Gl3DCoord looky;
    Gl3DCoord lookz;
} GlCamera;

/*!
 * \brief Enumeration of special keyboard keys.
 */
typedef enum
{
    KEY_NONE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_ESCAPE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_SHIFT,
    KEY_CTRL,
    KEY_ALT,
    KEY_MENU,   /*!< Menu key. */
    KEY_SYS     /*!< 'Window' key. */
} KeyboardKey;

/*!
 * \brief Keyboard event type.
 */
typedef enum
{
    KEYEVENT_PRESSED,       /*!< A key has been pressed. */
    KEYEVENT_RELEASED       /*!< A key has been released. */
} KeyEventType;

/*!
 * \brief Enumeration of mouse event types.
 */
typedef enum
{
    MOUSEEVENT_BUTTONPRESSED,       /*!< When a mouse button is pressed. */
    MOUSEEVENT_BUTTONRELEASED,      /*!< When a mouse button is released. */
    MOUSEEVENT_MOVE,                /*!< When the mouse moved. */
    MOUSEEVENT_WHEELUP,             /*!< When the wheel was rolled up. */
    MOUSEEVENT_WHEELDOWN,           /*!< When the wheel was rolled down. */
    MOUSEEVENT_IN,                  /*!< When the cursor enters inside an object. */
    MOUSEEVENT_OUT,                 /*!< When the cursor exits from an object. */
    MOUSEEVENT_CONTROL,             /*!< Mouse moved while mouse was in control mode. */
    MOUSEEVENT_EDGES                /*!< Mouse was inside active screen edges area. */
} MouseEventType;

/*!
 * \brief Enumeration of mouse buttons.
 */
typedef enum
{
    MOUSEBUTTON_NONE = 0,       /*!< No button. */
    MOUSEBUTTON_LEFT = 1,       /*!< Left button. */
    MOUSEBUTTON_RIGHT = 2,      /*!< Right button. */
    MOUSEBUTTON_MIDDLE = 3      /*!< Middle button (often the wheel). */
} MouseButton;

/*!
 * \brief Types for a 3D selection event.
 */
typedef enum
{
    SELECTIONEVENT_NONE,            /*!< No event occured. */
    SELECTIONEVENT_GROUNDMOVE,      /*!< Notifies a move on the ground plane. */
    SELECTIONEVENT_LEFTCLICK,       /*!< Notifies a left mouse click. */
    SELECTIONEVENT_RIGHTCLICK       /*!< Notifies a right mouse click. */
} SelectionEventType;

/*!
 * \brief Types of event that can be produced.
 *
 * Events that can't be catched by objects can still be catched by event collectors.
 */
typedef enum
{
    GLEVENT_NONE = 0x0000,          /*!< No event. */
    GLEVENT_MOUSE = 0x0001,         /*!< A mouse event occured on the object. If the callback returns FALSE, the event will continue downwards. */
    GLEVENT_KEYBOARD = 0x0002,      /*!< A keyboard event occured on the object. If the callback returns FALSE, the event will end in event collectors. */
    GLEVENT_RESIZE = 0x0004,        /*!< The screen has been resized. */
    GLEVENT_TIMER = 0x0008,         /*!< An internal timer produced a tick. */
    GLEVENT_DELETE = 0x0010,        /*!< The object will be deleted if the callback returns TRUE. */
    GLEVENT_DRAW = 0x0020,          /*!< The object has been drawn. */
    GLEVENT_CAMERA = 0x0040,        /*!< The camera has moved. */
    GLEVENT_SELECTION = 0x0080,     /*!< A 3D object has been selected or the ground selection has moved. */
    GLEVENT_TEXTURE = 0x0100        /*!< Texture links has been discarded and need to be made again. */
} GlEventType;

/*!
 * \brief Graphical event.
 */
typedef struct
{
    GlEventType type;       /*!< Event type. */
    union
    {
        CoreTime frameduration; /*!< For GLEVENT_DRAW. */

        struct
        {
            MouseEventType type;    /*!< Type of mouse event. */
            Gl2DCoord x;            /*!< Cursor's X coordinate (relative to the object pos). */
            Gl2DCoord y;            /*!< Cursor's Y coordinate (relative to the object pos). */
            MouseButton button;     /*!< Button that caused the event or button hold during the event. */
        } mouseevent;           /*!< Mouse event. */

        struct
        {
            SelectionEventType type;/*!< Type of selection event. */
            Gl3DObject* obj;        /*!< Object currently selected. */
            Sint16 groundx;         /*!< X position on the ground. */
            Sint16 groundy;         /*!< Y position on the ground. */
        } selectionevent;       /*!< Selection event. */

        struct
        {
            KeyEventType type;      /*!< Type of the event. */
            char keychar;           /*!< Character pressed if keyfunc == KEY_NONE. */
            KeyboardKey keyfunc;    /*!< Special key, KEY_NONE if not special. */
        } keyevent;             /*!< Keyboard event. */

        struct
        {
            Gl2DSize width;         /*!< New width. */
            Gl2DSize height;        /*!< New height. */
        } resizeevent;          /*!< Resize event. */

        struct
        {
            GlCamera oldcam;        /*!< Old camera. */
            GlCamera newcam;        /*!< New camera. */
        } camevent;             /*!< Camera event. */

    } event;                /*!< Event datas. */
} GlEvent;

/*!
 * \brief External pointer used by callback receivers to identify an object.
 */
typedef void* GlExtID;

/*!
 * \brief Callback for event catched.
 *
 * \param extid - External identifier of the object (or of the timer for a GLEVENT_TIMER event).
 * \param event - The event received. \readonly
 * \return TRUE if the event was treated, FALSE if it was ignored.
 */
typedef Bool (*GlEventCallback) (GlExtID extid, GlEvent* event);

#endif
