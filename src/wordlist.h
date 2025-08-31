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

#ifndef _WORDLIST_H
#define _WORDLIST_H

#include <stdio.h>

typedef struct s_word
{
  char *lang[2];
  char *comment;
  int nasked;
  int ncorrect;
} word;

word* generate_wordlist(int *nwords, int start, char *filename);
int priority_random(int nwords, word *wordlist, int max_addition);
void print_wordlist(int nwords, word *wordlist);

#endif
