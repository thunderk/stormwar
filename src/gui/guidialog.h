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
 *   Graphical dialog template                                                *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This file defines a GUI dialog.
 *
 * A GuiDialog is a template to display a dialog for user interaction.
 *
 * Here is an example of how to use such a template:
 * \code
 *      GuiDialog dialog;
 *
 *      dialog = GuiDialog_new(GUIDIALOG_OKCANCEL, dialogcallback);
 *          GuiDialog_addParagraph(dialog, "Hello World !");
 *          GuiDialog_addTextInput(dialog, "answer", "How are you today ?", NULL, NULL);
 *      GuiDialog_launch(dialog);
 * \endcode
 *
 * This example will create a message box with a simple text input and two buttons.
 *
 * The \a dialogcallback function will be called when the dialog closes, which
 * means the user clicked on a button. Here is an example of such a callback:
 * \code
 *  static void dialogcallback(GuiDialogButton button, Var vinput)
 *  {
 *      if (button == GUIDIALOGBUTTON_OK)
 *      {
 *          Var v = Var_getArrayElemByCName(vinput, "answer");
 *          printf("OK has been chosen and the anwser was %s.", String_get(Var_getValueString(v)));
 *      }
 *      else if (button == GUIDIALOGBUTTON_CANCEL)
 *      {
 *          printf("Cancel has been chosen.");
 *      }
 *  }
 * \endcode
 * You can see in this example that input fields of the dialog can be collected through
 * the variable returned by the callback.
 */

#ifndef _SW_GUIDIALOG_H_
#define _SW_GUIDIALOG_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure of a GuiDialog.
 */
typedef struct pv_GuiDialog pv_GuiDialog;

/*!
 * \brief Abstract type for a graphical dialog.
 */
typedef pv_GuiDialog* GuiDialog;

/*!
 * \brief Dialog type (will specify the dialog buttons).
 */
typedef enum
{
    GUIDIALOG_OK,
    GUIDIALOG_OKCANCEL,
    GUIDIALOG_YESNO,
    GUIDIALOG_YESNOCANCEL,
    GUIDIALOG_RETRYCANCEL
} GuiDialogType;

/*!
 * \brief Dialog buttons.
 */
typedef enum
{
    GUIDIALOGBUTTON_OK = 0,
    GUIDIALOGBUTTON_YES = 1,
    GUIDIALOGBUTTON_NO = 2,
    GUIDIALOGBUTTON_CANCEL = 3,
    GUIDIALOGBUTTON_RETRY = 4
} GuiDialogButton;

/*!
 * \brief Callback function that will be called on dialog closing.
 *
 * \param button - Button that caused the closing.
 * \param vinput - Named array containing dialog's input fields.
 */
typedef void (*GuiDialogCallback) (GuiDialog dialog, GuiDialogButton button, Var vinput);

/******************************************************************************
 *############################################################################*
 *#                            Shortcut functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Show a popup with a simple text and an 'OK' button.
 *
 * This function is a shortcut and will not raise a callback.
 * It is provided for convenience.
 * \param text - The text that will be displayed.
 */
void GuiDialog_popupMsg(const char* text);

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a dialog.
 *
 * A dialog can't be destroyed by calling a function. It only destroys itself.
 * The dialog won't be shown before GuiDialog_launch is called.
 * \param type - Type of dialog (specifies the buttons).
 * \param callback - Function that will be called when the user presses one of the buttons, can be NULL.
 * \return The newly allocated dialog.
 */
GuiDialog GuiDialog_new(GuiDialogType type, GuiDialogCallback callback);

/*!
 * \brief Destroy a dialog.
 *
 * This won't raise the dialog's callback.
 * \param dialog - The dialog to destroy.
 */
void GuiDialog_del(GuiDialog dialog);

/*!
 * \brief Effectively build a dialog.
 *
 * No GuiDialog_add... function can be called on this dialog after this.
 * This will show and center the dialog.
 * \param dialog - The dialog.
 */
void GuiDialog_launch(GuiDialog dialog);

/*!
 * \brief Add a text paragraph to the dialog.
 *
 * This must be called between GuiDialog_new and GuiDialog_launch.
 * \param dialog - The dialog.
 * \param text - Text paragraph, will be copied.
 */
void GuiDialog_addParagraph(GuiDialog dialog, const char* text);

/*!
 * \brief Add a text input field to the dialog.
 *
 * This must be called between GuiDialog_new and GuiDialog_launch.
 * This will fill a \ref VAR_STRING named '\a name' in the variable returned by the callback.
 * \param dialog - The dialog.
 * \param name - Identifier for the field (will be used in the \ref GuiDialogCallback variable).
 * \param label - Label that will be displayed before the input box.
 * \param tooltip - Tooltip to display, NULL if not.
 * \param defaulttext - Default text to fill in the box with, can be NULL.
 */
void GuiDialog_addTextInput(GuiDialog dialog, const char* name, const char* label, const char* tooltip, const char* defaulttext);

#endif
