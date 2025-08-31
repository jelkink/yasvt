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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "wordlist.h"
#include "error.h"

void usage()
{
  puts("yasvt [-h] [-d <direction>] [-n <number>] [-s <number>] -f <file>\n");
  puts("   -h, -?           Display this usage information");
  puts("   -d <direction>   Direction of questions: 0 = forward (default); 1 = backward; 2 = bidirectional");
  puts("   -s <number>      Line to start reading words");
  puts("   -n <number>      Number of words to use from list");
  puts("   -f <file>        File that contains yasvt wordlist (required)");
  puts("\nWordlist format:\n");
  puts("<word1>;<word2>;<comments>\n");
  puts("whereby <word2> is the translation of <word1> and <comments> are printed after asking the question.");
  /* Should the be an option to add comments that are visible when the question is asked? e.g. for perfective
     vs imperfective verbs? */
}

int main(int argc, char** argv)
{
  word* wordlist;
  int nwords = 0, start = 0, c, nasked = 0, ncorrect = 0, direction = 0, q, i, correct, ch;
  char ans[80] = "";
  char *filename = NULL;

  /* Read command line options */
  while ((ch = getopt(argc, argv, "d:n:s:f:v?h")) != -1)
    switch (ch)
      {
	/* -f sets filename for wordlist */
      case 'f':
	filename = (char *) malloc(strlen(optarg)+1);
	strcpy(filename, optarg);
	printf("Wordlist file: %s\n", filename);
	break;
	/* -d sets direction of questions */
      case 'd':
	direction = atoi(optarg);
	if (direction < 0 || direction > 2)
	  {
	    usage();
	    exit(0);
	  }
	break;
      case 'n':
	nwords = atoi(optarg);
	if (nwords < 0)
	  eprintf("Cannot read a negative number of words!\n");
	break;
      case 's':
	start = atoi(optarg) - 1;
	if (start < 0)
	  eprintf("Cannot start reading lines before the first line!\n");
	break;
      case 'h':
      case '?':
      default:
	usage();
	exit(0);
      }

  /* If no filename set, quit program */
  if (filename == NULL)
    {
      printf("-f option to set a filename is required\n\n");
      usage();
      exit(0);
    }

  /* Continue with test */
  srand(time(NULL));

  /* Read in word list */
  wordlist = generate_wordlist(&nwords, start, filename);
  print_wordlist(nwords, wordlist);

  if (!nwords)
    eprintf("Cannot run vocabulary trainer with empty wordlist!\n");

  /* Test words until q is entered as answer */
  while (strcmp(ans, "q") != 0)
    {
      nasked++;

      /* c = rand() % nwords; */
      c = priority_random(nwords, wordlist, 16);

      if (c >= 0)
	{
	  q = direction == 2 ? rand() % 2 : direction;

	  printf("Translate: %s (%d/%d) %s\n", wordlist[c].lang[q],
		 wordlist[c].ncorrect, wordlist[c].nasked, (q ? "<--" : "-->"));
	  wordlist[c].nasked++;

	  fgets(ans, 80, stdin);
	  ans[strlen(ans)-1] = '\0';

	  correct = -1;

	  if (strcmp(ans, wordlist[c].lang[1-q]) == 0)
	    correct = c;

	  i = 0;
	  while (correct < 0 && i < nwords)
	    {
	      if (strcmp(ans, wordlist[i].lang[1-q]) == 0 
		  && strcmp(wordlist[c].lang[q], wordlist[i].lang[q]) == 0)
		correct = i;
	      ++i;
	    }

	  if (correct >= 0)
	    {
	      printf("CORRECT!\n\n");
	      ncorrect++;
	      wordlist[correct].ncorrect++;
	    }
	  else if (strcmp(ans, "q") == 0)
	    {
	      printf("\n");
	      nasked--;
	      wordlist[c].nasked--;
	    }
	  else
	    printf("\e[31mWRONG!\e[0m\n");

	  for (i = 0; i < nwords; ++i)
	    if (i == c 
		|| strcmp(ans, wordlist[i].lang[1-q]) == 0
		|| strcmp(wordlist[c].lang[1-q], wordlist[i].lang[1-q]) == 0
		|| strcmp(wordlist[c].lang[q], wordlist[i].lang[q]) == 0)
	      printf("   %25s -> %25s    (%3d/%3d)    %s\n", wordlist[i].lang[q], 
		     wordlist[i].lang[1-q], wordlist[i].ncorrect, wordlist[i].nasked, wordlist[i].comment);
	  printf("\n");

	  printf("Score: %d of %d (%.1f%%)\n\n", ncorrect, nasked, 
		 (double) ncorrect / (double) nasked * 100.0);
	}
      else
	printf("Error: incorrect random selection\n\n");
    }

  print_wordlist(nwords, wordlist);
  printf("%63s | %3d | %3d | %3d %%\n\n", "TOTAL", ncorrect, nasked,
	 (nasked ? ncorrect * 100 / nasked : 0));

  return 0;
}
