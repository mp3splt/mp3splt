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
  
  //we search
  mp3splt_get_freedb_search(state,"eminem",&error,
			    SPLT_SEARCH_TYPE_FREEDB2,"\0",-1);
  fprintf(stdout,"we get the file..\n");
  //we get the file
  mp3splt_write_freedb_file_result(state, 2,
				   "test.cddb", &error,
				   //for now cddb.cgi get file type
				   SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI,
				   "\0",-1);
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
