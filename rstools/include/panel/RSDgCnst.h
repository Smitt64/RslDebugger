/********************************************************************
   Created: 2002/11/14
   Created: 14:11:2002   16:11
   Filename: D:\Work\RSMsgBox\RSDgCnst.h
   Project: RS-Dialogs
   Author: Andrew Ushakov
*********************************************************************/

#ifndef __RSDGCNST_H
#define __RSDGCNST_H

////////////////////////////////////////////////////////////////
// Message box parameters
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// Indicate the buttons displayed in the message
// box.
//////////////////////////////////////////////////////
// The message box contains one push button: "OK". This is the default style.
#define  RSMB_OK                  0x00000000UL

// The message box contains two push buttons: "OK" and "Cancel".
#define  RSMB_OKCANCEL            0x00000001UL

// The message box contains three push buttons: "Abort", "Retry", and "Ignore".
#define  RSMB_ABORTRETRYIGNORE    0x00000002UL

// The message box contains three push buttons: "Yes", "No", and "Cancel".
#define  RSMB_YESNOCANCEL         0x00000003UL

// The message box contains two push buttons: "Yes" and "No".
#define  RSMB_YESNO               0x00000004UL

// The message box contains two push buttons: "Retry" and "Cancel".
#define  RSMB_RETRYCANCEL         0x00000005UL

// The message box contains three push buttons: "Cancel", "Try Again", "Continue".
#define  RSMB_CANCELTRYCONTINUE   0x00000006UL

// Adds a "Help" button to the message box.
#define  RSMB_HELP                0x00004000UL

#define  RSMB_ALL_BUTTON          (RSMB_OK|RSMB_OKCANCEL|RSMB_ABORTRETRYIGNORE|RSMB_YESNOCANCEL|\
                                   RSMB_YESNO|RSMB_RETRYCANCEL|RSMB_CANCELTRYCONTINUE|RSMB_HELP)

//////////////////////////////////////////////////////
// Indicate the default button
//////////////////////////////////////////////////////
// The first button is the default button. This is the default style.
#define  RSMB_DEFBUTTON1   0x00000000UL

// The second button is the default button.
#define  RSMB_DEFBUTTON2   0x00000100UL

// The third button is the default button.
#define  RSMB_DEFBUTTON3   0x00000200UL

// The fourth button is the default button.
#define  RSMB_DEFBUTTON4   0x00000300UL

#define  RSMB_ALL_DEF_BTNS  (RSMB_DEFBUTTON1|RSMB_DEFBUTTON2|RSMB_DEFBUTTON3|RSMB_DEFBUTTON4)

//////////////////////////////////////////////////////
// Type of the icon in the message box
//////////////////////////////////////////////////////
// A stop-sign icon appears in the message box.
#define  RSMB_ICONERROR         0x00000010UL

// A question-mark icon appears in the message box. 
#define  RSMB_ICONQUESTION      0x00000020UL

// An exclamation-point icon appears in the message box.
#define  RSMB_ICONWARNING       0x00000030UL

// An icon consisting of a lowercase letter i in a circle appears in the message box. 
#define  RSMB_ICONINFORMATION   0x00000040UL

#define  RSMB_ALL_ICONS         (RSMB_ICONERROR|RSMB_ICONQUESTION|RSMB_ICONWARNING|RSMB_ICONINFORMATION)

//////////////////////////////////////////////////////
// To specify other options,
// use one or more of the following values.
//////////////////////////////////////////////////////
// The text is left-justified. By default
#define  RSMB_LEFT           0x00000000UL

// The text is right-justified.
#define  RSMB_RIGHT          0x00010000UL

// The text is centered.
#define  RSMB_CENTER         0x00020000UL

// The text is in sunken border.
#define  RSMB_SUNKENBORDER   0x00040000UL

// Show error message box
#define  RSMB_ERRORBOX       0x00080000UL

// Use auto size
#define  RSMB_AUTO_SIZE      0x00100000UL

// Use min/max button
#define  RSMB_MINMAX_BTN     0x00200000UL

// No message beep
#define  RSMB_NO_BEEP        0x00400000UL

#define  RSMB_TOP_MOST       0x00800000UL

//////////////////////////////////////////////////////
// If the function succeeds, the return value
// is one of the following menu-item values. 
//////////////////////////////////////////////////////
// "OK" button was selected.
#define  RSIDOK           1

// "Cancel" button was selected.
#define  RSIDCANCEL       2

// "Abort" button was selected.
#define  RSIDABORT        3

// "Retry" button was selected.
#define  RSIDRETRY        4

// "Ignore" button was selected.
#define  RSIDIGNORE       5

// "Yes" button was selected.
#define  RSIDYES          6

// "No" button was selected.
#define  RSIDNO           7

// "Help" button was selected.
#define  RSIDHELP         9

// "Again" button was selected.
#define  RSIDTRYAGAIN    10

// "Continue" button was selected.
#define  RSIDCONTINUE    11


////////////////////////////////////////////////////////////////
// Indicator parameters
////////////////////////////////////////////////////////////////

// Show progress bar only (default style)
#define  RSIND_SHOW_PROGRESS   0x00000000UL

// Show time information
#define  RSIND_SHOW_TIME       0x00000001UL

// Show count information
#define  RSIND_SHOW_COUNT      0x00000002UL

////////////////////////////////////////////////////////////////
// RSDISPID's
////////////////////////////////////////////////////////////////
// Show message box
#define  CMD_MSG_BOX            1

// Indicator init
#define  CMD_INIT_PROGRESS      2

// Indicator use
#define  CMD_USE_PROGRESS       3

// Indicator done
#define  CMD_DONE_PROGRESS      4

// Add progress to panel
#define  CMD_ADD_PROGRESS       5

// Rem progress from panel
#define  CMD_REM_PROGRESS       6

// Reinit single indicator
#define  CMD_REINIT_PROGRESS    7

// Show input window
#define  CMD_INWIN_DLG          8

// Show open file dialog
#define  CMD_OPEN_FILE          9

// Clean dialog data
#define  CMD_CLEAN_DLG         10

// Browse Folder dialog
#define  CMD_BROWSE_FOLDER     11

// Choose Font dialog
#define  CMD_CHOOSE_FONT       12

// Choose Color dialog
#define  CMD_CHOOSE_COLOR      13


// Event about dialog closing
#define  EV_REMOVE_DLG   11

// Use parent as active window
#define  ACTIVE_WND       1

// Use parent as main window
#define  MAIN_WND         2

#endif // __RSDGCNST_H

/* <-- EOF --> */