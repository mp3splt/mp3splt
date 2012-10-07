#include <stdio.h>
#include <stdlib.h>

#include <libmp3splt/mp3splt.h>

static void print_confirmation_and_exit_if_error(splt_state *state, splt_code error);
static void print_message_from_library(const char *message, splt_message_type type, void *data);
static void print_split_filename(const char *filename, void *data);

//Main program
//Please note that not all errors are handled in this example.
//Compile with:
//    $ gcc minimal.c -o minimal -lmp3splt
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Please provide the input file to be split as the first argument.\n");
    fflush(stderr);
    return EXIT_FAILURE;
  }

  splt_code error = SPLT_OK;

  //initialisation of the main state
  splt_state *state = mp3splt_new_state(NULL);

  //register callback functions
  mp3splt_set_message_function(state, print_message_from_library, NULL);
  mp3splt_set_split_filename_function(state, print_split_filename, NULL);

  //look for the available plugins
  error = mp3splt_find_plugins(state);
  print_confirmation_and_exit_if_error(state, error);

  //set the input filename to be split
  mp3splt_set_filename_to_split(state, argv[1]);

  //append two splitpoints
  splt_point *first_point = mp3splt_point_new(0, NULL);
  mp3splt_append_splitpoint(state, first_point);
  splt_point *second_point = mp3splt_point_new(100 * 60 * 1, NULL);
  mp3splt_append_splitpoint(state, second_point);

  //do the effective split
  error = mp3splt_split(state);
  print_confirmation_and_exit_if_error(state, error);

  //free the memory of the main state
  mp3splt_free_state(state);

  return EXIT_SUCCESS;
}

//Callback function that handles error code from libmp3splt.
static void print_confirmation_and_exit_if_error(splt_state *state, splt_code error)
{
  char *message = mp3splt_get_strerror(state, error);
  if (!message)
  {
    return;
  }

  if (error < 0)
  {
    fprintf(stderr, "%s\n", message);
    fflush(stderr);
    mp3splt_free_state(state);
    exit(1);
  }
  else
  {
    fprintf(stdout, "%s\n", message);
    fflush(stdout);
  }

  free(message);
}

//Callback function printing any messages from libmp3splt.
static void print_message_from_library(const char *message, splt_message_type type, void *data)
{
  if (type == SPLT_MESSAGE_INFO)
  {
    fprintf(stdout, message);
    fflush(stdout);
    return;
  }

  fprintf(stderr, message);
  fflush(stderr);
}

//Callback function printing the created filenames.
static void print_split_filename(const char *filename, void *data)
{
  fprintf(stdout, "   %s created.\n", filename);
  fflush(stdout);
}

