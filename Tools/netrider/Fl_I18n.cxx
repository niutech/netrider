//
// "$Id: Fl_I18n.cxx 9718 2012-11-13 13:03:20Z manolo $"
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
//#include <FL/Fl_I18n.H>
#include "Fl_I18n.H" //not in include FL yet

/** Constructor.
 The \p vendor, \p application, and \p root arguments are as in 
 Fl_Preferences::Fl_Preferences(Root root, const char *vendor, const char *application).
 */
Fl_I18n::Fl_I18n(const char *pathptr, const char *vendor, const char *application)
{
#ifndef __APPLE__
  char path[FL_PATH_MAX], stringsfile[FL_PATH_MAX];
#if defined (__MINGW32__)
sprintf(stringsfile, "./netrider/%s.lproj/GUI.strings", locale());
#else  
  sprintf(stringsfile, "%s/netrider/%s.lproj/GUI.strings", getenv("HOME"), locale());
#endif  
  FILE *in = fopen(stringsfile, "r");
  if (in) {    
    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    fseek(in, 0, SEEK_SET);    
    translation = (char*)malloc(fsize + 1);
    size_t result = fread(translation, 1, fsize, in);
    translation[fsize] = 0;    
    fclose(in);
  }
  else translation = strdup("");
#endif
}

/** Destructor */
Fl_I18n::~Fl_I18n()
{
#ifndef __APPLE__
  free(translation);
#endif
}

//define static variable
char Fl_I18n::current_language[128]={'\0'};

/** Returns the name of the current internationalization locale as a malloc'ed UTF-8 string.
*/
char *Fl_I18n::locale() {
#ifdef __APPLE__
  CFArrayRef all = CFBundleCopyBundleLocalizations(CFBundleGetMainBundle());
  CFArrayRef array = CFBundleCopyPreferredLocalizationsFromArray(all);
  CFRelease(all);
  CFStringRef lss = (CFStringRef)CFArrayGetValueAtIndex(array, 0);
  CFIndex l = CFStringGetMaximumSizeForEncoding(CFStringGetLength(lss), kCFStringEncodingUTF8);
  char *c = (char*)malloc(l+1);
  if (c) {
    CFStringGetCString(lss, c, l+1, kCFStringEncodingUTF8);
    l = strlen(c);
    c = (char*)realloc(c, l+1);
    }
  CFRelease(array);
  return c;
#else
  if (strlen(current_language)>0) return current_language;
  //could test for LANGUAGE environment variable now - todo
  return current_language;
#endif
};

/** Translates a character string to the current internationalization locale.
 \return A malloc'ed, UTF8-encoded string that translates the string transmitted as argument.
 If no translation is found, returns a copy of the argument. 
 Returns NULL if there's not enough memory.
*/
char *Fl_I18n::translate(const char *from)
{
#ifdef __APPLE__
  CFStringRef ss = CFStringCreateWithCString(NULL, from, kCFStringEncodingUTF8);
  if (!ss) return NULL;
  CFStringRef lss = CFBundleCopyLocalizedString(CFBundleGetMainBundle(),
						ss, ss, CFSTR("GUI"));
  CFRelease(ss);
  CFIndex l = CFStringGetMaximumSizeForEncoding(CFStringGetLength(lss), kCFStringEncodingUTF8);
  char *c = (char*)malloc(l+1);
  if (c) CFStringGetCString(lss, c, l+1, kCFStringEncodingUTF8);
  CFRelease(lss);
  if (c) {
    l = strlen(c);
    c = (char*)realloc(c, l+1);
  }
  return c;
#else
  char *p = translation, *q;
  while (true) {
    //while (isspace(*p) || *p == '"') p++;
    while (*p == '"') p++;
    //p++; //*p == '"'
    q = strchr(p, '"'); if (q == NULL) return strdup(from);
    *q = 0;
    if (strcmp(p, from) == 0) { *q = '"'; break; }
    *q = '"';
    p = strchr(p, '\n');
    if (p == NULL) return strdup(from);
    p++;
    } //true
  p = strchr(q + 1, '"');
  if (p == NULL) return strdup(from);
  p++;
  q = strchr(p, '"'); *q = 0;
  p = strdup(p);
  *q = '"';
  return p;
#endif
}

/** Loads a new translation string from the GUI.Strings file for the currently set language
*/
void Fl_I18n::reload(const char *pathptr, const char *vendor, const char *application)
{
#ifndef __APPLE__
  char path[FL_PATH_MAX], stringsfile[FL_PATH_MAX];
  char *translation_tmp;
#if defined (__MINGW32__)
sprintf(stringsfile, "./netrider/%s.lproj/GUI.strings", locale());
#else  
  sprintf(stringsfile, "%s/netrider/%s.lproj/GUI.strings", getenv("HOME"), locale());
#endif  
  FILE *in = fopen(stringsfile, "r");
  if (in) {    
    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    fseek(in, 0, SEEK_SET);    
    translation_tmp = (char*)realloc(translation,fsize + 1);
    translation=translation_tmp;
    size_t result = fread(translation, 1, fsize, in);
    translation[fsize] = 0;    
    fclose(in);
  }
  else sprintf(translation,"%s",""); 
#endif
}


//
// End of "$Id: Fl_I18n.cxx 9718 2012-11-13 13:03:20Z manolo $".
//
