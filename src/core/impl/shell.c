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
 *   Global game shell                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "kernel.h"
#include "core/shell.h"

#include "core/string.h"
#include "core/ptrarray.h"
#include "core/impl/impl.h"
#include "core/impl/shellfunction.h"

#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
/* Current display level of the shell */
static ShellLevel currentlevel = LEVEL_ERROR;

/* Strings associated with levels */
static char* LEVEL_STRING[7] = {"", "ERROR| ", "INFO | ", "DEBUG| ", "# ", "STACK| ", "SHOULDNOTHAPPEN "};

static ShellCallbackPrint printcb = NULL;

static FILE* logfile = NULL;

/* Error stack */
static PtrArray errorstack;
static PtrArrayPos stackprinted;        /* Error stack already printed */

/* Core things */
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_QUIT = CORE_INVALID_ID;       /* Try to quit the game. */
static CoreID FUNC_PRINT = CORE_INVALID_ID;      /* Print a string to the shell. */
static CoreID FUNC_CAT = CORE_INVALID_ID;        /* String concatenation. */
static CoreID FUNC_LEVEL = CORE_INVALID_ID;      /* Set the shell level. */

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
shellCallback(ShellFunction* funct)
{
    String s;
    
    if (funct->id == FUNC_PRINT)
    {
        shellPrint(LEVEL_USER, String_get(Var_getValueString(funct->params[0])));
        Var_setVoid(funct->ret);
    }
    else if (funct->id == FUNC_QUIT)
    {
        coreStop();
        Var_setVoid(funct->ret);
    }
    else if (funct->id == FUNC_CAT)
    {
        s = String_newByCopy(Var_getValueString(funct->params[0]));
        String_appendString(s, Var_getValueString(funct->params[1]));
        Var_setString(funct->ret, s);
        String_del(s);
    }
    else if (funct->id == FUNC_LEVEL)
    {
        int l;
        
        l = Var_getValueInt(funct->params[0]);
        if (l < LEVEL_USER)
        {
            l = LEVEL_USER;
        }
        if (l > LEVEL_HARDDEBUG)
        {
            l = LEVEL_HARDDEBUG;
        }
        shellSetLevel(l);
    }
}

/******************************************************************************
 *############################################################################*
 *#                           Internal functions                             #*
 *############################################################################*
 ******************************************************************************/
Bool
shellExecFromReader(Var ret, Reader reader)
{
    ReaderToken* cur;
    
    String modname = NULL;
    String funcname = NULL;
    
    ShellFunction* funcseek;
    ShellFunction func;
    CoreShellCallback callback;

    int i;
    String s;

    /*get first token*/
    cur = Reader_getCurrent(reader);
    if (cur->type != READER_NAME)
    {
        /*TODO: better error message*/
        shellPrint(LEVEL_ERROR, "Asked shell to execute a badly formatted command.");
        return TRUE;
    }
    
    /*assume we have a module name*/
    modname = String_newByCopy(cur->value.s);
    
    cur = Reader_forward(reader);
    if ((cur->type == READER_CHAR) && (cur->value.c == '.'))
    {
        /*this was a module name*/
        /*skip the '.'*/
        cur = Reader_forward(reader);
        
        /*look for a name*/
        if (cur->type != READER_NAME)
        {
            shellPrintf(LEVEL_ERROR, "Didn't find a function name after prefix '%s.'", String_get(modname));
            String_del(modname);
            return TRUE;
        }
        funcname = String_newByCopy(cur->value.s);
        cur = Reader_forward(reader);
    }
    else
    {
        /*this wasn't a module name, assume it was a function name*/
        funcname = modname;
        modname = NULL;
    }
    
    /*search for the function*/
    funcseek = coreFindShellFunction(modname, funcname, &callback);
    if (modname != NULL)
    {
        String_del(modname);
    }
    String_del(funcname);
    if (funcseek == NULL)
    {
        /*function was not found by the core*/
        return TRUE;
    }
    ASSERT(callback != NULL, return TRUE);
    ShellFunction_copy(funcseek, &func);
    
    /*read the parameters token*/
    i = 0;
    s = String_new("");
    if ((cur->type == READER_CHAR) && (cur->value.c == '('))
    {
        /*it seems there are parameters*/
        cur = Reader_forward(reader);       /*to skip '('*/
        if ((cur->type == READER_CHAR) && (cur->value.c == ')'))
        {
            /*no parameters inside*/
            cur = Reader_forward(reader);   /*to skip ')'*/
        }
        else
        {
            while (TRUE)
            {
                Var v;
                
                /*we have a new parameter*/
                if (i == func.nbparam)
                {
                    /*that's too many of them*/
                    /*TODO: better error message*/
                    ShellFunction_getPrototype(&func, s);
                    shellPrintf(LEVEL_ERROR, "Too many parameters for function '%s'", String_get(s));
                    String_del(s);
                    ShellFunction_del(&func);
                    return TRUE;
                }
                
                /*get the parameter*/
                v = Var_new();
                Var_setFromReader(v, reader);
                
                /*check its type*/
                if (Var_getType(v) != Var_getType(func.params[i]))
                {
                    /*TODO: better error message*/
                    ShellFunction_getPrototype(&func, s);
                    shellPrintf(LEVEL_ERROR, "Parameter number %d of wrong type for function '%s'", i + 1, String_get(s));
                    String_del(s);
                    ShellFunction_del(&func);
                    return TRUE;
                }
                
                /*setting in the function*/
                /*TODO: maybe check that the variable is unnamed*/
                Var_setFromVar(func.params[i], v);
                Var_del(v);
                
                i++;
                
                if (cur->type == READER_END)
                {
                    /*TODO: better error message*/
                    shellPrintf(LEVEL_ERROR, "Unexpected end of stream after %d parameter.", i);
                    String_del(s);
                    ShellFunction_del(&func);
                    return TRUE;
                }
                else if ((cur->type == READER_CHAR) && (cur->value.c == ')'))
                {
                    /*end of parameters*/
                    cur = Reader_forward(reader);   /*to skip ')'*/
                    break;
                }
                else if ((cur->type != READER_CHAR) || (cur->value.c != ','))
                {
                    /*TODO: better error message*/
                    shellPrintf(LEVEL_ERROR, "Excpected ',' or ')' after %d parameter.", i);
                    String_del(s);
                    ShellFunction_del(&func);
                    return TRUE;
                }
                cur = Reader_forward(reader);   /*to skip ','*/
            }
        }
    }
        
    if (i < func.nbparam)
    {
        ShellFunction_getPrototype(&func, s);
        shellPrintf(LEVEL_ERROR, "Too few parameters for function '%s'.", String_get(s));
        String_del(s);
        ShellFunction_del(&func);
        return TRUE;
    }
    
    /*finally, we call the function*/
    callback(&func);
    
    /*TODO: check that the returned type match the declared type*/
    
    /*and return the value*/
    Var_setFromVar(ret, func.ret);
    
    String_del(s);
    ShellFunction_del(&func);
    
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Shell functions                              #*
 *############################################################################*
 ******************************************************************************/
void
shellInit()
{
    printcb = NULL;

    errorstack = PtrArray_newFull(5, 3, (PtrFunc)String_del, NULL);
    stackprinted = 0;
    
    /*we declare the shell functions*/
    MOD_ID = coreDeclareModule("shell", NULL, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_QUIT = coreDeclareShellFunction(MOD_ID, "quit", VAR_VOID, 0);
    FUNC_PRINT = coreDeclareShellFunction(MOD_ID, "print", VAR_VOID, 1, VAR_STRING);
    FUNC_CAT = coreDeclareShellFunction(MOD_ID, "cat", VAR_STRING, 2, VAR_STRING, VAR_STRING);
    FUNC_LEVEL = coreDeclareShellFunction(MOD_ID, "setlevel", VAR_VOID, 1, VAR_INT);
}

/*----------------------------------------------------------------------------*/
void
shellUninit()
{
    PtrArray_del(errorstack);
    
    printcb = NULL;
    shellStopLogging();
    if (currentlevel >= LEVEL_INFO)
    {
        printf("Shell unloaded.\n");
    }
}

/*----------------------------------------------------------------------------*/
void
shellSetPrintCallback(ShellCallbackPrint _printcb)
{
    printcb = _printcb;
}

/*----------------------------------------------------------------------------*/
void
shellStartLogging(char* file)
{
    shellStopLogging();
    logfile = fopen(file, "a");
    if (logfile == NULL)
    {
        printf("Unable to open log file for writing: %s\n", file);
    }
    else
    {
        printf("*** Starting shell logging to file: %s ***\n", file);
    }
}

/*----------------------------------------------------------------------------*/
void
shellStopLogging()
{
    if (logfile != NULL)
    {
        shellPrintf(LEVEL_INFO, "*** End of log ***");
        fclose(logfile);
    }
}

/*----------------------------------------------------------------------------*/
void
shellSetLevel(ShellLevel level)
{
    /*ASSERT(level >= LEVEL_USER,);*/
    ASSERT(level <= LEVEL_HARDDEBUG, return);
    
    currentlevel = level;
}

/*----------------------------------------------------------------------------*/
void
shellPopErrorStack()
{
    if (PtrArray_SIZE(errorstack) == 0)
    {
        shellPrint(LEVEL_ERROR, "Error stack underrun.");
    }
    else
    {
        PtrArray_removePosFast(errorstack, PtrArray_SIZE(errorstack) - 1);
    }
    if (PtrArray_SIZE(errorstack) < stackprinted)
    {
        stackprinted = 0;
    }
}

/*----------------------------------------------------------------------------*/
void
shellPrint(ShellLevel level, const char* message)
{
    if (level == LEVEL_ERRORSTACK)
    {
        unsigned int i;
        String s;
    
        s = String_new("");
        for (i = 0; i < PtrArray_SIZE(errorstack); i++)
        {
            String_appendChar(s, ' ');
        }
        String_append(s, message);
        PtrArray_append(errorstack, s);
        return;
    }

    if (level == LEVEL_ERROR)
    {
        PtrArrayIterator it;
        
        for (it = PtrArray_START(errorstack) + stackprinted; it != PtrArray_STOP(errorstack); it++)
        {
            shellPrint(LEVEL_ERRORNOSTACK, String_get((String)*it));
        }
        stackprinted = PtrArray_SIZE(errorstack);
    }

    if (level <= currentlevel)
    {
        if (printcb != NULL)
        {
            printcb(level, message);
        }
    }

    if (logfile != NULL)
    {
        fprintf(logfile, "%s%s\n", LEVEL_STRING[level], message);
    }
}

/*----------------------------------------------------------------------------*/
void
shellPrintf(ShellLevel level, const char* format, ...)
{
    /*static char buf[10000];*/
    String s;
    va_list(ap);
    va_start(ap, format);
    
    /*vsprintf(buf, format, ap);*/
    s = String_new("");
    String_vprintf(s, format, ap);
    shellPrint(level, String_get(s));
    String_del(s);
    
    va_end(ap);
}






/******************************************************************************
 *############################################################################*
 *#                           ShellExec functions                            #*
 *############################################################################*
 ******************************************************************************/
Bool
shellExecFunction(String com, Var retvar)
{
    Bool b;
    Reader r;
    
    r = Reader_newFromString(String_get(com));
    b = shellExecFromReader(retvar, r);
    Reader_del(r);
    
    /*TODO: maybe check that the reader is at READER_END*/
    
    return b;
}

/*----------------------------------------------------------------------------*/
void
shellExec(String command)
{
    Var var;
    
    var = Var_new();
    
    /* this will automatically call shellExecFromReader */
    Var_setFromString(var, command);
    
    if (Var_getType(var) != VAR_VOID)
    {
        shellPrintf(LEVEL_USER, _("Shell execution returned: %s"), Var_gets(var));
    }
    
    Var_del(var);
}
