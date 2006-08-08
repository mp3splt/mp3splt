#include <stdio.h>
#include <stdlib.h>

#include <libmp3splt/mp3splt.h>

//get splitpoint tests
void get_splitpoints_test(splt_state *state)
{
  int number_of_splits = 0;
  splt_point *points = mp3splt_get_splitpoints(state, &number_of_splits);
  
  int i = 0;
  for (i = 0; i < number_of_splits; i++)
    {
      fprintf(stdout, "%d) %s %ld\n",
	      i,points[i].name,points[i].value);
    }
  
}

//get splitpoint tests
void get_tags_test(splt_state *state)
{
  int number_of_tags = 0;
  splt_tags *tags = 
    mp3splt_get_tags(state, &number_of_tags);
  
  int i = 0;
  for (i = 0; i < number_of_tags; i++)
    {
      fprintf(stdout, "\ntags number %d :\n"
	      "title = %s\n"
	      "artist = %s\n"
	      "album = %s\n"
	      "performer = %s\n"
	      "year = %s\n"
	      "comment = %s\n"
	      "track = %d\n"
	      "genre = %u\n",
	      i,tags[i].title,tags[i].artist,tags[i].album,
	      tags[i].performer, tags[i].year, tags[i].comment,
	      tags[i].track, tags[i].genre);
    }
  
}

/** 
 * 
 * 
 * @param state 
 */
void append_splitpoints_tags_test(splt_state *state)
{
  int error = SPLT_OK;
  
  //append 1
  error = mp3splt_append_splitpoint(state, 100 * 60 * 1,
				    "split1");
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending = %d\n",error);
    }
  
  error = mp3splt_append_tags(state, 
			      "bobo", "rita",
			      "new 200", "perfo",
			      "2004", "this is a commen",
			      2, 12);
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending tags = %d\n",error);
    }
  
  error = SPLT_OK;
  //append 2
  error = mp3splt_append_splitpoint(state, 100 * 60 * 2,
				    NULL);
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending = %d\n",error);
    }
  error = mp3splt_append_tags(state, 
			      "bobo2", "rita2",
			      "new 2002", "perfo2",
			      "2002", "this is a commen2",
			      5, 10);
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending tags = %d\n",error);
    }
  
  error = SPLT_OK;
  //append 3
  error = mp3splt_append_splitpoint(state, -20, NULL);
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending = %d\n",error);
    }
  error = mp3splt_append_tags(state, 
			      "bobo3", "rita3",
			      "new 2003", "perfo3",
			      "2003", "this is a commen3",
			      7, 11);
  if (error != SPLT_OK)
    {
      fprintf(stdout,"error appending tags = %d\n",error);
    }
}

/*********************************/
/* main function  */
int main (int argc, char *argv[])
{
  int error = SPLT_OK;
  
  /*********/
  /* initialisation */
  
  splt_state *state = mp3splt_new_state(&error);
  //error
  if (error < 0)
    {
      fprintf(stdout,"create new state error = %d\n",error);
    }
  
  /*********/
  /* tests */
  
  //append_splitpoints_tags_test(state);
  //get_splitpoints_test(state);
  //get_tags_test(state);
  
  /*mp3splt_set_filename_to_split(state,
    "song.mp3");
				   
  error = mp3splt_split(state);
  if (error != SPLT_OK)
    {
      fprintf(stdout, "err = %d\n",error);
      }*/
  
  /*********/
  /* free variables */
  
  mp3splt_free_state(state);
  
  return 0;
}
