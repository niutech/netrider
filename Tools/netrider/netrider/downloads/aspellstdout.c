/* This file is derived from a file that is part of The New Aspell
 * Copyright (C) 2000-2001 by Kevin Atkinson under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/. 
 */
/*
 * This is a sample program that reads a document and outputs spelling errors
 * to stdout using a format similar to the GNU gcc C/C++ and 
 * mingw C/C++ compiler suites.  The purpose is for easier integration of 
 * spell-checking into programming editors such as SciTE.
 * The original program this was derived from is example-c.  It was written by 
 * Kevin Atkinson and is part of the aspell distribution (http://aspell.net).
 *
 * I would like to encourage other developers to make and share improvements 
 * on this code (such as addition of document type filters) and to create other
 * tools that would integrate in a similar fashion with programming editors
 * that support compiler error display.
 *
 * This program (based on example-c) with modifications for use by 
 * programming editors was created by Laura Michaels in May 2008.
 *
 * All new or original code copyright 2008.  These modifications are likewise
 * licensed under GNU LGPL.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aspell.h"

// Buffer size for how many characters (or bytes) of a line from a file can be
// read in and processed during each read request.
#define MAX_LINESIZE 8192


static void print_word_list(AspellSpeller * speller, 
			    const AspellWordList *wl) 
{
  if (wl == 0) {
     fputs ("?", stdout);
  } else {
    AspellStringEnumeration * els = aspell_word_list_elements(wl);
    const char * word;
    while ( (word = aspell_string_enumeration_next(els)) != 0) {
      fputs(word, stdout);
      putc(' ', stdout);
    }
    delete_aspell_string_enumeration(els);
  }
}


static int check_document(AspellSpeller * speller, const char * filename)
{   
   AspellCanHaveError * ret;
   AspellDocumentChecker * checker;
   AspellToken token;
   FILE * doc;
   char line[MAX_LINESIZE];
   char * word_begin;
   long linecount;
   int linewidth;
   int x;

   linecount = 0;
   
   /* Open the file */
   doc = fopen(filename, "r");
   if (doc <= 0) 
   {
      printf("Error:  Unable to open the file \"%s\" for reading.", filename);
      return 3;
   }

   /* Set up the document checker */
   ret = new_aspell_document_checker(speller);
   if (aspell_error(ret) != 0) 
   {
      printf("Error:  %s\n",aspell_error_message(ret));
      fclose(doc);
      return 4;
   }
   
   checker = to_aspell_document_checker(ret);

   while (fgets(line, MAX_LINESIZE, doc)) 
   {
      linewidth = strlen (line);
      if (line[linewidth - 1] == '\n')
         linecount++;
      /* First process the line */
      aspell_document_checker_process(checker, line, -1);

      /* Now find the misspellings in the line */
      while (token = aspell_document_checker_next_misspelling(checker), token.len != 0)
      {
         /* Print out the misspelling and possible replacements */	 
         word_begin = line + token.offset ;
         
         printf ("%s:%ld: Locate: ", filename, linecount);
         for (x = 0; x < token.len; x++)
            putc(*(word_begin + x), stdout);
         fputs (" | Try: ", stdout);
         print_word_list(speller, aspell_speller_suggest(speller, word_begin, token.len)); 
         printf ("\n");
       
      }
   }

   delete_aspell_document_checker(checker);

   fclose(doc);

   return 0;
}


int main(int argc, const char *argv[]) 
{
   AspellCanHaveError * ret;
   AspellSpeller * speller;
   AspellConfig * config;
   char * word_end;
   int retval;
   
   retval = 0;

   if (argc < 2) 
   {
      printf("Usage:  %s filename [language]\n", argv[0]);
      return 1;
   }

   config = new_aspell_config();

   if (argc < 3)
      aspell_config_replace(config, "lang", "en_US");
   else      
      aspell_config_replace(config, "lang", argv[2]);

   ret = new_aspell_speller(config);

   delete_aspell_config(config);

   if (aspell_error(ret) != 0) 
   {
      delete_aspell_can_have_error(ret);
      return 2;
   }
  
   speller = to_aspell_speller(ret);
   config = aspell_speller_config(speller);

   retval = check_document(speller, argv[1]);

   delete_aspell_speller(speller);
   return (retval);
}

