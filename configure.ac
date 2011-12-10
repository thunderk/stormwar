dnl Process this file with autoconf to produce a configure script.
AC_INIT(stormwar, 0.15.1)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([src/main.c])

AC_CANONICAL_SYSTEM


AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])

AC_CONFIG_SRCDIR(config.h.in)
AM_CONFIG_HEADER(config.h)

AC_GNU_SOURCE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

CFLAGS=""








dnl Checks for libraries.
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION, [],
	AC_MSG_ERROR([***** SDL version $SDL_VERSION or better not found *****]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB(m, matherr, [],
	AC_MSG_ERROR([***** Standard library libm not found! *****]))

AC_CHECK_LIB(SDL_image, IMG_Load, [],
	AC_MSG_ERROR([***** Library SDL_image not found *****]))
LIBS="$LIBS -lSDL_image -lm"

have_sdlmixer="no"
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, have_sdlmixer="yes",
	AC_MSG_WARN([***** Library SDL_mixer not found. Sound disabled *****]))
if test "x${have_sdlmixer}" = "xyes"
then
    LIBS="$LIBS -lSDL_mixer"
fi

AC_CHECK_LIB(GL, glBegin, [],
    AC_MSG_ERROR([***** Library GL (OpenGL) not found *****]))
AC_CHECK_LIB(GLU, gluGetString, [],
    AC_MSG_ERROR([***** Library GLU >= 1.1 (OpenGL) not found *****]))
LIBS="$LIBS -lGL -lGLU"





dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h strings.h time.h])
### TODO : checks for SDL_image and SDL_mixer headers





dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST





dnl Checks for library functions.
AC_CHECK_FUNCS([pow sqrt strcmp memcpy vasprintf round])









dnl *** Configure basic options ***
AC_ARG_ENABLE(sound, AC_HELP_STRING([--disable-sound], [Disable sound support]),use_sound="no",use_sound="yes")
if test "x${use_sound}" = "xyes"
then
	use_sound="no"
	if test "x${have_sdlmixer}" = "xyes"
	then
		use_sound="yes"
		AC_DEFINE(USE_SOUND, 1, [Define to 1 to enable sound.])
    fi
fi

AC_ARG_ENABLE(i18n, AC_HELP_STRING([--disable-i18n], [Disable internationalization]),use_i18n="no",use_i18n="yes")
if test "x${use_i18n}" = "xyes"
then
	AC_DEFINE(USE_I18N, 1, [Define to 1 to enable internationalization.])
fi


dnl *** Configure expert options ***
AC_ARG_ENABLE(dist_w32, AC_HELP_STRING([--enable-dist_w32], ["make dist" will build a win32 package]),dist_w32="yes",dist_w32="no")
AM_CONDITIONAL(DIST_W32, test "x${dist_w32}" = "xyes")

AC_ARG_ENABLE(debug_compile, AC_HELP_STRING([--enable-debug_compile], [Strict compiling and debug options]),debug_compile="yes",debug_compile="no")
AM_CONDITIONAL(COMPILE_DEBUG, test "x${debug_compile}" = "xyes")

AC_ARG_ENABLE(debug_mem, AC_HELP_STRING([--enable-debug_mem], [Performs memory allocation tracing]),debug_mem="yes",debug_mem="no")
if test "x${debug_mem}" = "xyes"
then
	AC_DEFINE(DEBUG_MEM, 1, [Define to 1 if memory allocations and freeings are traced.])
fi

AC_ARG_ENABLE(debug_varset, AC_HELP_STRING([--enable-debug_varset], [Print debug information about variables validation]), debug_varset="yes", debug_varset="no")
if test "x${debug_varset}" = "xyes"
then
	AC_DEFINE(DEBUG_VARSET, 1, [Define to 1 if variables are to be described on debugging output when they are validated.])
fi

AC_ARG_ENABLE(debug_tex, AC_HELP_STRING([--enable-debug_tex], [Enable debugging of textures]), debug_tex="yes", debug_tex="no")
if test "x${debug_tex}" = "xyes"
then
	AC_DEFINE(DEBUG_TEX, 1, [Define to 1 to print debugging information about textures for OpenGL drawing.])
fi

AC_ARG_ENABLE(debug_opengl, AC_HELP_STRING([--enable-debug_opengl], [Enable debugging of OpenGL rendering]), debug_opengl="yes", debug_opengl="no")
if test "x${debug_opengl}" = "xyes"
then
	AC_DEFINE(DEBUG_OPENGL, 1, [Define to 1 to print debugging information about OpenGL rendering.])
fi

AC_ARG_ENABLE(debug_i18n, AC_HELP_STRING([--enable-debug_i18n], [Enable debugging of translations]), debug_i18n="yes", debug_i18n="no")
if test "x${debug_i18n}" = "xyes"
then
	AC_DEFINE(DEBUG_I18N, 1, [Define to 1 to print debugging information about internationalization.])
fi

AC_ARG_ENABLE(asserts, AC_HELP_STRING([--enable-asserts], [Enable expensive asserts checks]), asserts="yes", asserts="no")
if test "x${asserts}" = "xyes"
then
	AC_DEFINE(ASSERTS, 1, [Define to 1 to check expensive asserts.])
fi






dnl End
AC_CONFIG_FILES(stormwar.spec config_w32.h config_osx.h Makefile src/Makefile data/Makefile m4/Makefile)
AC_OUTPUT

dnl Summing up options
echo
echo "Basic options :"
echo "---------------"
echo "Sound support via SDL_mixer:          ${use_sound}"
echo "Internationalization support:         ${use_i18n}"
echo
echo "Advanced options :"
echo "------------------"
echo "Include win32 stuff in dist package:  ${dist_w32}"
echo "Strict compiling with debug info:     ${debug_compile}"
echo "Debug memory functions:               ${debug_mem}"
echo "Debug OpenGL rendering:               ${debug_opengl}"
echo "Debug variables validation:           ${debug_varset}"
echo "Debug OpenGL textures:                ${debug_tex}"
echo "Debug internationalization:           ${debug_i18n}"
echo "Perform expensive checks (asserts):   ${asserts}"
echo
echo "You can now type 'make' to compile the program."
echo
