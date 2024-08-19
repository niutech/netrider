//
// "$Id: Fl_I18n.H 9718 2012-11-13 13:03:20Z manolo $"
//
// Internationalization support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2013 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
#include <FL/Fl_Preferences.H>

/** Support for application internationalization.
 An application-specific directory needs be created and populated with locale-specific subdirectories
 each containing a file with the translation of all GUI strings for this locale.
 The path to this application-specific directory varies with the platform as follows:
 \li MWSindows: <tt>c:/Documents and Settings/'user'/Application Data/'vendor'/'application'/</tt>
 \li Unix/Linux: <tt>$HOME/.fltk/'vendor'/'application'/</tt>.
 \li Mac OS: <tt>'application bundle'/Contents/Resources/</tt>
 
 where <tt>vendor</tt> and <tt>application</tt> are the arguments of the Fl_I18n constructor.
 The locale-specific subdirectories placed therein are named 'language'.lproj (e.g., <tt>German.lproj,
 French.lproj</tt>). In each of these subdirectories a UTF-8 text file called <tt>GUI.strings</tt>
 is placed. These files contain a series of lines of text. Each line is of the form:
 \verbatim
 "reference string" = "translated string";
 \endverbatim
 where 'reference string' is the GUI string that the program source code refers, and 'translated string'
 is its translation to the corresponding locale. The <tt>German.lproj/GUI.strings</tt> file, 
 for example, may contain:
 \verbatim
 "File" = "Ablage";
 "Open..." = "Öffnen…";
 "Quit"="Beenden";
 \endverbatim 
 
 For example, an "editor" application could be localized to French with this in the source code:
 \verbatim
 #include <FL/Fl_I18n.H>
 #include <FL/fl_ask.H>
 
 Fl_I18n locale("fltk.org", "editor"); // allocate a global locale object
 
 Fl_Menu_Item menuitems[] = {
 { locale.translate("File"),              0, 0, 0, FL_SUBMENU },
 { locale.translate("Open File..."),    FL_COMMAND + 'o', open_cb },
 { locale.translate("Save"),            FL_COMMAND + 's', save_cb },
 { locale.translate("Quit"),            FL_COMMAND + 'q', quit_cb },
 {0},
 {0}
 };
 
 int main(int argc, char **argv) 
 {
   int code=0;
   char *message = locale.translate("Alert code %d");
   fl_alert(message, code);
   free(message);
 }
 \endverbatim
 and this in file <tt>French.lproj/GUI.strings</tt>:
 \verbatim
 "File" = "Fichier";
 "Open File..." = "Ouvrir un fichier…";
 "Save" = "Sauver";
 "Quit" = "Quitter";
 "Alert code %d" = "Code d'alerte: %d";
 \endverbatim
 
 On the Mac OS platform, the locale is determined by the current user's preferred language, as for
 any other application. 
 
 On other platforms, the application's locale is determined by the
 FLTK preference mechanism. One procedure is to equip the program with a dialog asking its user
 to select his/her preferred language, among the list of existing 'language'.lproj directories,
 and calling Fl_I18n::locale(const char* where) so this choice is saved for the next program run
 (don't forget to delete the Fl_I18n object if it's been dynamically allocated, or preference
 changes are lost). Another procedure is to preset the application's current locale. This can be done
 putting a file named 'application'.prefs next to the application-specific directory containing,
 for example if the Spanish locale is targetted:
 \verbatim
 ; FLTK preferences file format 1.0
 [.]
 [./Internationalization]
 Language:Spanish
 \endverbatim

 If the requested 'language'.lproj subdirectory does not exist, or if the corresponding
 <tt>GUI.strings</tt> file does not contain the translation of the requested string,
 the Fl_I18n::translate(const char *from) function returns a copy of \p from.
 */


class Fl_I18n {

private:
#ifndef __APPLE__
  char *translation;
#endif

public:
  Fl_I18n(const char *pathptr, const char *vendor, const char *application);
  ~Fl_I18n();
  static char current_language[128];

  char *locale();
  char *translate(const char *from);
  void reload(const char *pathptr, const char *vendor, const char *application);  
};

//
// End of "$Id: Fl_I18n.H 9718 2012-11-13 13:03:20Z manolo $".
//
