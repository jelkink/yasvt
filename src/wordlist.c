/* This file is part of yasvt.
 * 
 * yasvt is free software: you can redistribute it and/or modify it under the terms 
 * of the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * 
 * yasvt is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with yasvt.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "wordlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

word *generate_wordlist(int *nwords, int start, char *filename)
{
  word *wordlist = NULL, *newlist = NULL;
  FILE *infile;
  char buf[80], *text;
  int i, l;

  /* Open file with word list */
  infile = fopen(filename, "r");
  if (infile == NULL)
    eprintf("Could not open file %s\n", filename);

  if (*nwords == 0) 
    printf("Will read all lines from line %d onwards.\n", start+1);
  else
    printf("Will read %d lines from line %d onwards.\n", *nwords, start+1);

  /* Loop through file and read word list */
  i = 0; l = 0;
  while ((*nwords == 0 || i < *nwords) && fgets(buf, 80, infile) != NULL)
    if (l >= start)
      {
	/* Reserve memory for word list and move list */
	newlist = malloc(sizeof(word) * (i+1));
	if (newlist == NULL)
	  eprintf("Failed to allocate memory for word list\n");
	
	if (i > 0)
	  memcpy(newlist, wordlist, sizeof(word) * i);
	
	free(wordlist);
	wordlist = newlist;
	
	/* Overwrite end of line mark with end of string */
	buf[strlen(buf)-1] = '\0';
	
	/* Read left word */
	text = strtok(buf, ";");
	
	if (text == NULL)
	  eprintf("Could not find first word at line %d\n", i+l+1);
	
	wordlist[i].lang[0] = malloc(strlen(text)+1);
	
	if (wordlist[i].lang[0] == NULL)
	  eprintf("Could not allocate memory for first word at line %d\n", i+l+1);
	
	strcpy(wordlist[i].lang[0], text);
	
	/* Read right word */
	text = strtok(NULL, ";");
	
	if (text == NULL)
	  eprintf("Could not find second word at line %d\n", i+1);
	
	wordlist[i].lang[1] = malloc(strlen(text)+1);
	
	if (wordlist[i].lang[1] == NULL)
	  eprintf("Could not allocate memory for second word at line %d\n", i+l+1);
	
	strcpy(wordlist[i].lang[1], text);

	/* Read comments */
	text = strtok(NULL, ";");

	if (text == NULL)
	  {
	    wordlist[i].comment = malloc(1);

	    if (wordlist[i].comment == NULL)
	      eprintf("Could not allocate memory for empty comment at line %d\n", i+l+1);

	    *wordlist[i].comment = '\0';
	  }
	else
	  {
	    wordlist[i].comment = malloc(strlen(text)+1);
	
	    if (wordlist[i].comment == NULL)
	      eprintf("Could not allocate memory for comment at line %d\n", i+l+1);
	
	    strcpy(wordlist[i].comment, text);
	  }
	
	/* Reset statistics */
	wordlist[i].nasked = 0;
	wordlist[i].ncorrect = 0;
	
	++i;
      }
    else
      ++l;

  /* Close in put file */
  fclose(infile);

  *nwords = i;

  return wordlist;
}

int priority_random(int nwords, word* wordlist, int max_addition)
{
  /* This algorithm is probably possible with only one loop! Example in 
     Kernighan & Pike, I think */

  int sum = 0, i, k, add;

  for (i = 0; i < nwords; ++i)
    {
      add = 2 - (wordlist[i].nasked > 0) 
	+ (wordlist[i].nasked - wordlist[i].ncorrect) * 2;
      sum += add > max_addition ? max_addition : add;
    }

  k = rand() % sum;

  sum = 0;
  for (i = 0; i < nwords; ++i)
    {
      add = 2 - (wordlist[i].nasked > 0) 
	+ (wordlist[i].nasked - wordlist[i].ncorrect) * 2;
      sum += add > max_addition ? max_addition : add;
      if (sum > k)
	return i;
    }

  return -1;
}

void print_wordlist(int nwords, word* wordlist)
{
  int c;

  /* Print entire wordlist with scores */
  printf("Overview of words:\n\n");

  printf("%30s   %30s   %3s   %3s   %5s\n", "", "", "ok", "ask", "score");
  printf("-----------------------------------------------------------------------------------\n");

  for (c = 0; c < nwords; ++c)
    printf("%30s | %30s | %3d | %3d | %3d %%\n", wordlist[c].lang[0],
	   wordlist[c].lang[1], wordlist[c].ncorrect, wordlist[c].nasked,
	   (wordlist[c].nasked ? wordlist[c].ncorrect * 100 / wordlist[c].nasked : 0));

  printf("----------------------------------------------------------------|-----|-----|------\n");
}
