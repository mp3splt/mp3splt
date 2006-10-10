#include <stdio.h>
#include <stdlib.h>

#include <libmp3splt/mp3splt.h>

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
      exit(0);
    }
  
  /*********/
  /* tests */
  
  mp3splt_get_freedb_search(state,"eminem",&error);
  //error
  if (error < 0)
    {
      fprintf(stdout,"freedb search error = %d\n",error);
      exit(0);
    }
  
  mp3splt_free_state(state,&error);
  //error
  if (error < 0)
    {
      fprintf(stdout,"free state error = %d\n",error);
      exit(0);
    }
  
  return 0;
}
