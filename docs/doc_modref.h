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
 *   Documentation : Mod reference                                            *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief Documentation : \ref doc_modref
 *
 * \todo New animation system.
 * \todo Specify implementation things (coordinate system...).
 */

/*!
 * \page doc_modref Mod data tree reference
 *
 * \section Introduction
 *
 * This file describes the contents of the StormWar data tree.
 * You should respect these specifications while creating a mod.
 *
 * - To the root of the data folder must be a 'stormwar_data' file that
 *   lists all available mods.
 * - Mods are stored in the data folder, in a sub-directory named after the mod name.
 *   A file '\ref mod_def "mod_def"' in the mod folder contains the mod variable (an array that contains
 *   all mod datas).
 * - Paths must always be given relatively to the mod folder (for pictures, sounds, musics and links to file).
 * - Each paragraph of this document is the content description of a named array used as a structure
 *   (see \ref var.h for help on named arrays).
 * - Links between these structures are given in CAPITALS.
 *
 * The best way to understand how this works is to have a look at the 'default' mod provided in the
 * data tree.
 *
 * \section mod_def mod_def content
 * <pre>
 *  \#gui = (\ref GUI_DEF)                            // GUI definition
 *  \#env = (\ref ENV_DEF)                            // environment definition
 *  \#ground = (\ref GROUND_DEF)                      // ground definition
 *  \#sounds = [array of \ref SOUND_DEF]              // array of sounds
 *  \#music = (\ref MUSIC_DEF)                        // music definition
 *  \#i18n = [array of \ref I18N_DEF]                 // internationalization definition (array of languages)
 *  \#game = (\ref GAME_DEF)                          // game definition
 *  \#graph = (\ref GRAPHICS_DEF)                     // graphics definition
 * </pre>
 *
 * \section FONT_DEF FONT_DEF
 * <pre>
 *  \#font_name = (string)                       // font name
 *  \#font_picture = (string)                    // picture containing the 8*8 table of characters
 *  \#char_width = (int)                         // width of one character in the table
 *  \#char_height = (int)                        // height of one character in the table
 *  \#hor_spacing = (int)                        // horizontal size of a space between words
 * </pre>
 *
 * \section GUI_DEF GUI_DEF
 * <pre>
 *  \#sidepanel = (\ref SIDEPANEL_DEF)                // sidepanel definition
 *  \#menubar = (\ref MENUBAR_DEF)                    // menubar definition
 *  \#shell = (\ref SHELL_DEF)                        // graphical shell definition
 *  \#worldmap = (\ref WORLDMAP_DEF)                  // worldmap definition
 *  \#themes = [array of \ref THEME_DEF]              // array of themes
 *  \#menu = [array of \ref MENU_DEF]                 // array of menus
 * </pre>
 *
 * \section SIDEPANEL_DEF SIDEPANEL_DEF
 * <pre>
 *  \#width = (int)                              // panel width
 *  \#texture = (\ref GUITEXTURE_DEF)                 // background texture
 *  \#entity_w = (int)                           // width of the entity selector (width of an entity picture)
 *  \#entity_h = (int)                           // height of an entity picture
 *  \#entities_bleft = (int)                     // distance between the left border and the entity selector
 *  \#entities_btop = (int)                      // distance between the top border and the entity selector
 *  \#entities_bbottom = (int)                   // distance between the bottom border and the entity selector
 *  \#entities_pict = [array of string]          // array of entity pictures in the format \#name_of_the_entity="path_to_the_picture"
 * </pre>
 *
 * \section GUITEXTURE_DEF GUITEXTURE_DEF
 * <pre>
 *  \#picture = (string)                         // composed background picture
 *  \#border_left = (int)                        // left border size
 *  \#border_right = (int)                       // right border size
 *  \#border_top = (int)                         // top border size
 *  \#border_bottom = (int)                      // bottom border size
 * </pre>
 *
 * \section MENUBAR_DEF MENUBAR_DEF
 * <pre>
 *  \#height = (int)                             // menubar height
 *  \#texture = (\ref GUITEXTURE_DEF)                 // background texture
 *  \#fontname = (string)                        // name of the \ref FONT_DEF used
 *  \#fontoptions = (\ref FONTOPTION_DEF)             // font options
 *  \#startx = (int)                             // X offset of entries
 *  \#starty = (int)                             // Y offset of entries
 *  \#entryspace = (int)                         // space size between entries
 *  \#entries = [array of \ref MENUENTRY_DEF]         // array of entries
 * </pre>
 *
 * \section MENUENTRY_DEF MENUENTRY_DEF
 * <pre>
 *  \#name = (string)                            // name (text displayed)
 *  \#com = (string)                             // shell command to execute or name of a \ref MENU_DEF preceeded by a '\%' sign
 * </pre>
 *
 * \section FONTOPTION_DEF FONTOPTION_DEF
 * <pre>
 *  \#color_red = (int)                          // red component of the font (between 0 and 255)
 *  \#color_green = (int)                        // green component of the font (between 0 and 255)
 *  \#color_blue = (int)                         // blue component of the font (between 0 and 255)
 *  \#monospace = (int)                          // monospace rendering (1) or not (0)
 * </pre>
 *
 * \section MENU_DEF MENU_DEF
 * <pre>
 *  \#name = (string)                            // name identifier
 *  \#entries = [array of \ref MENUENTRY_DEF]         // menu entries
 * </pre>
 *
 * \section SHELL_DEF SHELL_DEF
 * <pre>
 *  \#screen_layout = (\ref LAYOUT_DEF)               // layout on screen
 *  \#bg = (string)                              // backgroung picture
 *  \#rect_output = (\ref RECT_DEF)                   // rectangle occupied by the output area
 *  \#rect_input = (\ref RECT_DEF)                    // rectangle occupied by the input area
 *  \#output = (\ref GUIOUTPUT_DEF)                   // output area definition
 *  \#shellcolor_user = (\ref RGBACOLOR_DEF)          // color of user-level messages
 *  \#shellcolor_error = (\ref RGBACOLOR_DEF)         // color of error-level messages
 *  \#shellcolor_info = (\ref RGBACOLOR_DEF)          // color of info-level messages
 *  \#shellcolor_debug = (\ref RGBACOLOR_DEF)         // color of debug-level messages
 *  \#shellcolor_harddebug = (\ref RGBACOLOR_DEF)     // color of harddebug-level messages
 * </pre>
 *
 * \section RECT_DEF RECT_DEF
 * <pre>
 *  \#x = (int)                                  // top-left corner X position
 *  \#y = (int)                                  // top-left corner Y position
 *  \#w = (int)                                  // horizontal size
 *  \#h = (int)                                  // vertical size
 * </pre>
 *
 * \section RGBACOLOR_DEF RGBACOLOR_DEF
 * <pre>
 *  \#r = (int)                                  // red component (between 0 and 255)
 *  \#g = (int)                                  // green component (between 0 and 255)
 *  \#b = (int)                                  // blue component (between 0 and 255)
 *  \#a = (int)                                  // alpha value (between 0 and 255)
 * </pre>
 *
 * \section RGBCOLOR_DEF RGBCOLOR_DEF
 * <pre>
 *  \#r = (int)                                  // red component (between 0 and 255)
 *  \#g = (int)                                  // green component (between 0 and 255)
 *  \#b = (int)                                  // blue component (between 0 and 255)
 * </pre>
 *
 * \section GUIOUTPUT_DEF GUIOUTPUT_DEF
 * <pre>
 *  \#bgcolor = (\ref RGBACOLOR_DEF)                  // background color
 *  \#font = (string)                            // name of the \ref FONT_DEF to use
 *  \#fontopt = (\ref FONTOPTION_DEF)                 // font options
 *  scrollbar_width = (int)                     // width of the scrollbar
 * </pre>
 *
 * \section LAYOUT_DEF LAYOUT_DEF
 * <pre>
 *  \#dock = (int)                               // dock position (between 1 and 9, explicitely linked to keyboard numpad layout)
 *  \#offsetx = (int)                            // X offset to the dock position
 *  \#offsety = (int)                            // Y offset to the dock position
 *  \#layer = (int)                              // Z layer (altitude)
 * </pre>
 *
 * \section WORLDMAP_DEF WORLDMAP_DEF
 * <pre>
 *  \#width = (int)                              // map width
 *  \#height = (int)                             // map height
 *  \#layout = (\ref LAYOUT_DEF)                      // screen layout
 *  \#col_void = (\ref RGBACOLOR_DEF)                 // color of empty plots
 *  \#col_camera = (\ref RGBACOLOR_DEF)               // color of camera plots
 *  \#col_ground = (\ref RGBACOLOR_DEF)               // color of ground plots
 *  \#col_path = (\ref RGBACOLOR_DEF)                 // color of path plots
 *  \#col_staticunit = (\ref RGBACOLOR_DEF)           // color of static unit plots
 *  \#col_dynunit = (\ref RGBACOLOR_DEF)              // color of dynamic unit plots
 *  \#col_mainunit = (\ref RGBACOLOR_DEF)             // color of main unit plots
 * </pre>
 *
 * \section THEME_DEF THEME_DEF
 * <pre>
 *  name of this structure is the name of the GUI element to theme:
 *     dialog, menu, tooltip, button, buttonpushed, scrollbar or input
 *
 *  \#bgtex = (\ref GUITEXTURE_DEF)                   // background texture
 *  \#fontname = (string)                        // name of the \ref FONT_DEF to use
 *  \#fontoptions = (\ref FONTOPTION_DEF)             // font options
 *  \#border_left = (int)                        // left border size (border of the active area)
 *  \#border_right = (int)                       // right border size (border of the active area)
 *  \#border_top = (int)                         // top border size (border of the active area)
 *  \#border_bottom = (int)                      // bottom border size (border of the active area)
 * </pre>
 *
 * \section GROUND_DEF GROUND_DEF
 * <pre>
 *  \#mesh_full = (\ref MESH_DEF)                     // mesh of the ground itself
 *  \#mesh_side = (\ref MESH_DEF)                     // mesh of a side
 *  \#mesh_cornerint = (\ref MESH_DEF)                // mesh of an internal corner
 *  \#mesh_cornerout = (\ref MESH_DEF)                // mesh of an external corner
 * </pre>
 *
 * \section ENV_DEF ENV_DEF
 * <pre>
 *  \#thunderbolts = [array of \ref THUNDER_DEF]      // thunderbolts definition
 *  \#skybox = (\ref SKYBOX_DEF)                      // skybox definition
 *  \#flocks = [array of \ref FLOCK_DEF]              // array of environmental flocks
 *  \#lights = [array of \ref LIGHT_DEF]              // array of global lights
 * </pre>
 *
 * \section LIGHT_DEF LIGHT_DEF
 * <pre>
 *  \#col_ambient = (\ref RGBCOLOR_DEF)               // ambient color
 *  \#col_diffuse = (\ref RGBCOLOR_DEF)               // diffuse color
 *  \#col_specular = (\ref RGBCOLOR_DEF)              // specular color
 *  \#posx = (float)                             // X position of the light
 *  \#posy = (float)                             // Y position of the light
 *  \#posz = (float)                             // Z position of the light
 *  \#angh = (float)                             // horizontal angle of the light
 *  \#angv = (float)                             // vertical angle of the light
 *  \#attmode = (int)                            // attenuation mode (0 = constant, 1 = linear, 2 = quadratic)
 *  \#attfactor = (float)                        // attenuation factor [0.0...+inf]          
 *  \#exponent = (float)                         // exponent factor [0.0...128.0]          
 *  \#cutoff = (float)                           // cutoff angle [0.0...90.0] or 180.0          
 * </pre>
 *
 * \section THUNDER_DEF THUNDER_DEF
 * <pre>
 *  \#frequency = (int)                          // frequency factor (between 0 and 1000)
 *  \#sound = (string)                           // name of the \ref SOUND_DEF to use
 * </pre>
 *
 * \section SKYBOX_DEF SKYBOX_DEF
 * <pre>
 *  \#mesh = (\ref MESH_DEF)                          // mesh of the background skybox
 *  \#anim = (string)                            // name of the mesh anim to use
 *  \#col = (\ref RGBACOLOR_DEF)                      // normal color of the skybox
 *  \#collightning = (\ref RGBACOLOR_DEF)             // color of the skybox while lightning
 *  \#lightningfadein = (int)                    // lightning fade-in duration (transition effect)
 *  \#lightningfadeout = (int)                   // lightning fade-out duration (transition effect)
 * </pre>
 *
 * \section FLOCK_DEF FLOCK_DEF
 * <pre>
 *  \#number = (int)                             // number of 'boids' in the flock
 *  \#anim = (string)                            // name of the \ref MESHANIM_DEF to use
 *  \#xrel = (int)                               // are xmin and xmax relative to the world (1 or 0)?
 *  \#zrel = (int)                               // are zmin and zmax relative to the world (1 or 0)?
 *  \#xmin = (float)                             // X minimal limit of the authorized flocking area (relative or not, depending of xrel)
 *  \#xmax = (float)                             // X maximal limit of the authorized flocking area (relative or not, depending of xrel)
 *  \#zmin = (float)                             // Z minimal limit of the authorized flocking area (relative or not, depending of zrel)
 *  \#zmax = (float)                             // Z maximal limit of the authorized flocking area (relative or not, depending of zrel)
 *  \#ymin = (float)                             // minimal authorized altitude of the flocking area
 *  \#ymax = (float)                             // maximal authorized altitude of the flocking area
 *  \#response = (float)                         // responsiveness factor to flocking algorithms (between 0.0 and 1.0)
 *  \#xmul = (float)                             // X attenuation factor (1.0 by default, less to reduce movement among X axis)
 *  \#ymul = (float)                             // Y attenuation factor (1.0 by default, less to reduce movement among Y axis)
 *  \#zmul = (float)                             // Z attenuation factor (1.0 by default, less to reduce movement among Z axis)
 *  \#mesh = (\ref MESH_DEF)                          // mesh of a 'boid'
 * </pre>
 *
 * \section TEXTURE_DEF TEXTURE_DEF
 * <pre>
 *  \#name = (string)                            // name identifier
 *  \#file = (string)                            // texture picture
 *  \#filter = (int)                             // texture is filtered (1) or not (0)
 *  \#xwrap = (int)                              // texture wraps horizontally (1) or not (0)
 *  \#ywrap = (int)                              // texture wraps vertically (1) or not (0)
 *  \#shiny = (float)                            // shininess factor (between 0.0 and 128.0)
 *  \#col_ambient = (\ref RGBACOLOR_DEF)              // ambient color
 *  \#col_diffuse = (\ref RGBACOLOR_DEF)              // diffuse color
 *  \#col_specular = (\ref RGBACOLOR_DEF)             // specular (when shiny>0.0) color
 *  \#col_emission = (\ref RGBACOLOR_DEF)             // emission color
 * </pre>
 *
 * \section SOUND_DEF SOUND_DEF
 * <pre>
 *  \#name = (string)                            // name identifier
 *  \#file = (string)                            // sound file
 * </pre>
 *
 * \section MUSIC_DEF MUSIC_DEF
 * <pre>
 *  \#file = (string)                            // music file
 *  \#fadein = (int)                             // fade-in time in milliseconds
 *  \#fadeout = (int)                            // fade-out time in milliseconds
 * </pre>
 *
 * \section I18N_DEF I18N_DEF
 * <pre>
 *  The name of this structure is the language code coded in 2 characters (en, es, fr...)
 *  It contains an array of translation elements.
 *  A translation element is like this:
 *  [ \#o = "Original string", \#t = "Translated string" ]
 * </pre>
 *
 * \section GAME_DEF GAME_DEF
 * <pre>
 *  \#sound_entity_select = (string)             // name of the \ref SOUND_DEF to use for entity selection
 *  \#sound_entity_drop = (string)               // name of the \ref SOUND_DEF to use for entity dropping
 *  \#particle_select_tex = (string)             // name of the \ref TEXTURE_DEF to use for the selection marker
 *  \#particle_range_tex = (string)              // name of the \ref TEXTURE_DEF to use for a range display
 *  \#entities = [array of \ref ENTITY_DEF]           // array of entities
 * </pre>
 *
 * \section ENTITY_DEF ENTITY_DEF
 * <pre>
 *  The name of this variable is used as entity identifier.
 *  \#mesh = (\ref MESH_DEF)                          // entity mesh
 *  \#width = (int)                              // width (in world coordinates)
 *  \#height = (int)                             // height (in world coordinates)
 *  \#rangedist = (float)                        // distance of the action range
 *  \#rangeangle = (float)                       // angle of the action range
 *  \#rules_drop = (\ref DROPRULES_DEF)               // rules to allow piece dropping
 *  \#keepdrop = (int)                           // 1 if this entity should stay in drop mode, 0 to stop dropping after one drop
 *  \#static = (int)                             // 1 if the entity is static, else 0
 *  \#selectable = (int)                         // 1 if the entity can be selected, else 0
 *  \#blended = (int)                            // 1 if the entity is transparent (needs z-sorting), else 0
 * </pre>
 *
 * \section GRAPHICS_DEF GRAPHICS_DEF
 * <pre>
 *  \#loading_pict = (string)                    // path to a picture used while loading
 *  \#cursors = [array of \ref CURSOR_DEF]            // array of cursors
 *  \#fonts = [array of \ref FONT_DEF]                // array of fonts
 *  \#textures = [array of \ref TEXTURE_DEF]          // array of textures
 *  \#gamma = (float)                            // gamma correction factor (default is 1.0)
 *  \#opengl = (\ref OPENGL_DEF)                      // OpenGL parameters
 * </pre>
 *
 * \section OPENGL_DEF OPENGL_DEF
 * <pre>
 *  \#clearcolor = (\ref RGBACOLOR_DEF)               // clearing color
 *  \#view_x = (int)                             // top-left corner X position of the 3d scene area
 *  \#view_y = (int)                             // top-left corner Y position of the 3d scene area
 *  \#view_wrel = (int)                          // width of the 3d scene area is relative (1) or not (0) to screen size
 *  \#view_hrel = (int)                          // height of the 3d scene area is relative (1) or not (0) to screen size
 *  \#view_w = (int)                             // width of the 3d scene area
 *  \#view_h = (int)                             // height of the 3d scene area
 *  \#fog = (\ref FOG_DEF)                            // fog definition
 * </pre>
 *
 * \section FOG_DEF FOG_DEF
 * <pre>
 *  \#fog = (int)                                // fog is present (1) or not (0)
 *  \#color = (\ref RGBACOLOR_DEF)                    // fog color
 *  \#density = (float)                          // fog density (between 0.0 and 1.0)
 * </pre>
 *
 * \section CURSOR_DEF CURSOR_DEF
 * <pre>
 *  The name of this structure is the name of the cursor specified:
 *   normal
 *   bottom, bottom2, bottom3
 *   top, top2, top3
 *   topleft, topleft2, topleft3...
 *  \#pict = (string)                            // cursor picture
 *  \#hotspot_x = (int)                          // X position of the hotspot (pointer spot) in the picture
 *  \#hotspot_y = (int)                          // Y position of the hotspot (pointer spot) in the picture
 * </pre>
 *
 * \section DROPRULES_DEF DROPRULES_DEF
 * <pre>
 *  \#needground = (int)                         // need ground under this entity (1) or not (0)
 *  \#denyground = (int)                         // ground is forbidden under this entity (1) or not (0)
 *  \#groundconnex = (int)                          // ground connexity required (1) or not (0)
 *  \#ground_entites = [array of strings]        // list of entities that can play the role of ground
 *  \#allowed_entities = [array of strings]      // list of entities allowed in the same location that the one to drop
 * </pre>
 *
 * \section MESH_DEF MESH_DEF
 * <pre>
 *  \#anim = [array of \ref MESHANIM_DEF]             // animations
 *  \#parts = [array of \ref MESHPART_DEF)            // parts data
 *  \#controlpoints = [array of floats]          // array of control points (used for selection and visibility) [x1,y1,z1,x2,y2,z2...]
 * </pre>
 *
 * \section MESHANIM_DEF MESHANIM_DEF
 * <pre>
 *  \#name = (string)                            // animation name
 *  \#keyframes = [array of \ref KEYFRAMES_DEF]       // keyframes (the list order is important)
 * </pre>
 *
 * \section KEYFRAMES_DEF KEYFRAMES_DEF
 * <pre>
 *  \#time = (float)                             // base time for the animation in seconds
 *  \#parts = [array of \ref ANIMPARTS_DEF]           // parts placement at this keyframe
 * </pre>
 *
 * \section ANIMPARTS_DEF ANIMPARTS_DEF
 * <pre>
 *  \#part = (int)                               // number of the \ref MESHPART_DEF to define (position inside the \ref MESH_DEF parts list)
 *  \#posx = (float)                             // X position
 *  \#posy = (float)                             // Y position
 *  \#posz = (float)                             // Z position
 *  \#angh = (float)                             // horizontal rotation angle
 *  \#angv = (float)                             // vertical rotation angle
 * </pre>
 *
 * \section MESHPART_DEF MESHPART_DEF
 * <pre>
 *  \#tex = (string)                             // name of the \ref TEXTURE_DEF to use in this part
 *  \#normalsmode = (int)                        // 0 if a normal is given for each face
 *                                               // 1 if a normal is given for each vertex (shared between faces, continuous)
 *                                               // 2 if a normal is given for each vertex of each face
 *  \#texcoordsmode = (int)                      // 0 if texture coordinates are given for each vertex (shared between faces, continuous)
 *                                               // 1 if texture coordinates are given for each vertex of each face
 *  \#flatshading = (int)                        // 1 = flat, 0 = smooth
 *  \#twosided = (int)                           // 1 = two-sided mode, 0 = normal culling
 *  \#blended = (int)                            // 1 = transparent mode, 0 = solid mode
 *  \#vertices = [array of \ref MESHVERTEX_DEF]       // vertex list
 *  \#normals = [array of \ref MESHNORMAL_DEF]        // normal vectors, depending on \a normalsmode value.
 *  \#texcoords = [array of \ref MESHTEXCOORD_DEF]    // texture coordinates, depending on \a texcoordsmode value.
 *  \#faces = [array of \ref MESHFACE_DEF]            // face list
 * </pre>
 *
 * \section MESHVERTEX_DEF MESHVERTEX_DEF
 * <pre>
 *  This is an array containing 3 vertex coordinates like:
 *      [x (float), y (float), z (float)]
 * </pre>
 *
 * \section MESHNORMAL_DEF MESHNORMAL_DEF
 * <pre>
 *  This is an array containing normal vector coordinates like:
 *      [dx (float), dy (float), dz (float)]
 * </pre>
 *
 * \section MESHTEXCOORD_DEF MESHTEXCOORD_DEF
 * <pre>
 *  This is an array containing UV texture coordinates:
 *      [u (float between 0.0 and 1.0), v (float between 0.0 and 1.0)]
 * </pre>
 *
 * \section MESHFACE_DEF MESHFACE_DEF
 * <pre>
 *  This is an array containing the 3 or 4 vertices of the face given by their number (position in the vertex list in \ref MESHPART_DEF):
 *      [v1 (int), v2 (int), v3 (int)]
 *      [v1 (int), v2 (int), v3 (int), v4 (int)]
 * </pre>
 */
