datadir = @datadir@
localedir = $(datadir)/locale

if COMPILE_DEBUG
AM_CFLAGS = -O0 -ggdb3 -pg -ansi -pedantic -W -Wall -Wdisabled-optimization -Wfloat-equal -Wchar-subscripts -DLOCALEDIR=\"$(localedir)\" -DDATA_DIR="\"$(datadir)/${PACKAGE}\""
else
AM_CFLAGS = -O2 -DLOCALEDIR=\"$(localedir)\" -DDATA_DIR="\"$(datadir)/${PACKAGE}\""
endif

bin_PROGRAMS = stormwar

EXTRA_DIST = main.h \
kernel.h \
test.h \
fake.h \
core/types.h \
core/comp.h \
core/constants.h \
core/core.h \
core/i18n.h \
core/macros.h \
core/ptrarray.h \
core/shell.h \
core/string.h \
core/reader.h \
core/var.h \
core/impl/impl.h \
core/impl/shellfunction.h \
tools/tools.h \
tools/anim.h \
tools/completion.h \
tools/fonct.h \
tools/varvalidator.h \
tools/internal/texturizer.h \
graphics/camera.h \
graphics/color.h \
graphics/gl2dobject.h \
graphics/gl3dobject.h \
graphics/gliterator.h \
graphics/glmesh.h \
graphics/glrect.h \
graphics/glsurface.h \
graphics/gltextrender.h \
graphics/graphics.h \
graphics/input.h \
graphics/light.h \
graphics/particle.h \
graphics/types.h \
graphics/impl/cursor.h \
graphics/impl/glmeshpart.h \
graphics/impl/glscreen.h \
graphics/impl/gltextures.h \
graphics/impl/impl.h \
graphics/impl/keyboard.h \
graphics/impl/opengl.h \
sound/sound.h \
game/game.h \
game/internal/droprules.h \
game/internal/game.h \
game/internal/entity.h \
game/internal/player.h \
game/internal/piece.h \
game/internal/register.h \
gui/gui.h \
gui/guidialog.h \
gui/worldmap.h \
gui/guitexture.h \
gui/internal/guitheme.h \
gui/internal/guibutton.h \
gui/internal/guiwidget.h \
gui/internal/guioutput.h \
gui/internal/guiinput.h \
gui/internal/guiscrollbar.h \
gui/internal/guishell.h \
gui/internal/guipopupmenu.h \
gui/internal/guitooltip.h \
gui/internal/gamedialogs.h \
gui/internal/menubar.h \
gui/internal/sidepanel.h \
world/world.h \
world/ground.h \
world/env.h \
world/internal/skybox.h \
world/internal/flocking.h \
world/internal/thunderbolt.h \
system/mem.h

stormwar_SOURCES = main.c \
kernel.c \
test.c \
core/impl/comp.c \
core/impl/core.c \
core/impl/i18n.c \
core/impl/ptrarray.c \
core/impl/shell.c \
core/impl/shellfunction.c \
core/impl/string.c \
core/impl/reader.c \
core/impl/var.c \
tools/anim.c \
tools/tools.c \
tools/completion.c \
tools/fonct.c \
tools/varvalidator.c \
tools/internal/texturizer.c \
graphics/impl/camera.c \
graphics/impl/color.c \
graphics/impl/cursor.c \
graphics/impl/gltextures.c \
graphics/impl/gl2dobject.c \
graphics/impl/gl3dobject.c \
graphics/impl/gliterator.c \
graphics/impl/glmesh.c \
graphics/impl/glmeshpart.c \
graphics/impl/glscreen.c \
graphics/impl/glsurface.c \
graphics/impl/gltextrender.c \
graphics/impl/graphics.c \
graphics/impl/input.c \
graphics/impl/keyboard.c \
graphics/impl/light.c \
graphics/impl/opengl.c \
graphics/impl/particle.c \
graphics/impl/types.c \
sound/sound.c \
game/game.c \
game/internal/gamecamera.c \
game/internal/droprules.c \
game/internal/entity.c \
game/internal/player.c \
game/internal/piece.c \
game/internal/register.c \
gui/gui.c \
gui/guidialog.c \
gui/worldmap.c \
gui/guitexture.c \
gui/internal/guitheme.c \
gui/internal/guibutton.c \
gui/internal/guiwidget.c \
gui/internal/guioutput.c \
gui/internal/guiinput.c \
gui/internal/guiscrollbar.c \
gui/internal/guishell.c \
gui/internal/guitooltip.c \
gui/internal/sidepanel.c \
gui/internal/menubar.c \
gui/internal/guipopupmenu.c \
gui/internal/gamedialogs.c \
world/world.c \
world/ground.c \
world/env.c \
world/internal/skybox.c \
world/internal/flocking.c \
world/internal/thunderbolt.c \
system/mem.c
