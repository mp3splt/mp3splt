/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2008 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "splt.h"
#include "plugins.h"

//mingw does not provide BSD functions 'scandir' and 'alphasort'
#ifdef __WIN32__

//-returns -1 for not enough memory, -2 for other errors
//-a positive (or 0) number if success
int scandir(const char *dir, struct dirent ***namelist,
		int(*filter)(const struct dirent *),
		int(*compar)(const struct dirent **, const struct dirent **))
{
  struct dirent **files = NULL;
  struct dirent *file = NULL;
  DIR *directory = NULL;
  int number_of_files = 0;

  directory = opendir(dir);
  if (directory == NULL)
  {
    return -2;
  }

  int free_memory = 0;
  int we_have_error = 0;

  while (file = readdir(directory))
  {
    if ((filter == NULL) || (filter(file)))
    {
      if (files == NULL)
      {
        files = malloc((sizeof *files));
      }
      else
      {
        files = realloc(files, (sizeof *files) * (number_of_files + 1));
      }
      if (files == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      files[number_of_files] = malloc(sizeof(DIR));
      if (files[number_of_files] == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      *files[number_of_files] = *file;
      number_of_files++;
    }
  }

  //we should have a valid 'namelist' argument
  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

  //-free memory if error
  if (free_memory)
  {
    while (number_of_files--)
    {
      if (files[number_of_files])
      {
        free(files[number_of_files]);
        files[number_of_files] = NULL;
      }
    }
    free(files);
    files = NULL;
  }

  if (closedir(directory) == -1)
  {
    return -2;
  }

  qsort(*namelist, number_of_files, sizeof **namelist,
      (int (*)(const void *, const void *)) compar);

  if (we_have_error)
  {
    return -1;
  }

  return number_of_files;
}

int alphasort(const struct dirent **a, const struct dirent **b)
{
  return strcoll((*a)->d_name, (*b)->d_name);
}

#endif

//sets the plugin scan directories
static int splt_p_set_default_plugins_scan_dirs(splt_state *state)
{
  splt_plugins *pl = state->plug;
  pl->number_of_dirs_to_scan = 3;
  //allocate memory
  pl->plugins_scan_dirs = malloc(sizeof(char *) * pl->number_of_dirs_to_scan);
  if (pl->plugins_scan_dirs == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  memset(pl->plugins_scan_dirs,0,sizeof(char *) * pl->number_of_dirs_to_scan);

  //temporary variable that we use to set the default directories
  char temp[2048] = { '\0' };

  //we put the default plugin directory
  snprintf(temp,2048,"%s",SPLT_PLUGINS_DIR);
  pl->plugins_scan_dirs[0] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[0] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[0],strlen(temp)+1,"%s",temp);

  //we put the home libmp3splt home directory
  snprintf(temp,2048,"%s%c%s",getenv("HOME"),SPLT_DIRCHAR,".libmp3splt");
  pl->plugins_scan_dirs[1] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[1] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[1],strlen(temp)+1,"%s",temp);

  //we put the current directory
  memset(temp,'\0',2048);
  snprintf(temp,2048,".%c",SPLT_DIRCHAR);
  pl->plugins_scan_dirs[2] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[2] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[2],strlen(temp)+1,"%s",temp);

  return SPLT_OK;
}

//function to filter the plugin files
static int splt_p_filter_plugin_files(const struct dirent *de)
{
  int file_length = strlen(de->d_name);
  char *file = (char *) de->d_name;
  char *p_end = NULL;
  char *p_start = NULL;
  if (strlen(file) >= 8)
  {
    //if the name starts with splt_and contains .so or .sl or .dll or .dylib
    if (strncmp(file,"libsplt_",8) == 0)
    {
      splt_u_print_debug("Looking at the file ",0, file);
      //find the last '.' character
      p_end = strrchr(file,'.');
      p_start = strchr(file,'.');
      //we only look at files containing only one dot
      if ((p_end != NULL) && (p_start == p_end))
      {
        if ((strcmp(p_end,".so") == 0) ||
            (strcmp(p_end,".sl") == 0) ||
            (strcmp(p_end,".dll") == 0) ||
            (strcmp(p_end,".dylib") == 0))
        {
          return 1;
        }
      }
    }
  }

  return 0;
}

//scans the directory *directory for plugins
//-directory must not be NULL
static int splt_p_scan_dir_for_plugins(splt_plugins *pl, const char *directory)
{
  int return_value = SPLT_OK;

  struct dirent **files = NULL;
  int number_of_files = 0;
  //scan the directory
  number_of_files = scandir(directory, &files,
      splt_p_filter_plugin_files, alphasort);
  int directory_len = strlen(directory);
  int new_number_of_files = number_of_files;

  if (number_of_files == -1)
  {
		return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  else if (new_number_of_files >= 0)
  {
    //for all the filtered found plugins,
    //copy their name
    while (new_number_of_files--)
    {
      char *fname = files[new_number_of_files]->d_name;
      int fname_len = strlen(fname);

      int alloc_err = splt_t_alloc_init_new_plugin(pl);
      if (alloc_err < 0)
      {
        return_value = alloc_err;
        goto end;
      }
    
      pl->data[pl->number_of_plugins_found].func = malloc(sizeof(splt_plugin_func));
      if (pl->data[pl->number_of_plugins_found].func == NULL)
      {
        return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto end;
      }
      memset(pl->data[pl->number_of_plugins_found].func,0,sizeof(splt_plugin_func));
      pl->data[pl->number_of_plugins_found].plugin_filename = malloc(sizeof(char) *
          (fname_len+directory_len+3));
      if (pl->data[pl->number_of_plugins_found].plugin_filename == NULL)
      {
        return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto end;
      }
      //set the plugin name and the directory
      snprintf(pl->data[pl->number_of_plugins_found].plugin_filename,
          fname_len+directory_len+2,"%s%c%s",directory,SPLT_DIRCHAR,fname);
      pl->number_of_plugins_found++;
    }

end:
    ;
    if (files)
    {
      //free memory
      while (number_of_files--)
      {
        if (files[number_of_files])
        {
          free(files[number_of_files]);
          files[number_of_files] = NULL;
        }
      }
      free(files);
      files = NULL;
    }
  }

  return return_value;
}

//finds the plugins
//-returns SPLT_OK if no error and SPLT_ERROR_CANNOT_FIND_PLUGINS if
//no plugin was found
static int splt_p_find_plugins(splt_state *state)
{
  int return_value = SPLT_OK;

  splt_plugins *pl = state->plug;

  //if we don't have any directory set for scanning
  if (pl->plugins_scan_dirs == NULL)
  {
    //set the default directories
    return_value = splt_p_set_default_plugins_scan_dirs(state);
    if (return_value != SPLT_OK)
    {
      return return_value;
    }
  }

  //for each scan directory, look for the files starting with 'splt' and
  //ending with '.so' on unix-like and '.dll' on windows
  int i = 0;

  char current_dir[3] = { '\0' };
  snprintf(current_dir,3,".%c",SPLT_DIRCHAR);

  for (i = 0;i < pl->number_of_dirs_to_scan;i++)
  {
    if (pl->plugins_scan_dirs[i] != NULL)
    {
      splt_u_print_debug("Scanning plugins in the directory ",0, pl->plugins_scan_dirs[i]);

      //don't check if directory exists if the directory is ./ on unix-like
      //OSes or .\\ on windows
      if (((strlen(pl->plugins_scan_dirs[i]) >= 2) &&
            strncmp(pl->plugins_scan_dirs[i], current_dir,2) == 0) ||
          splt_u_check_if_directory(pl->plugins_scan_dirs[i]))
      {
        return_value = splt_p_scan_dir_for_plugins(pl, pl->plugins_scan_dirs[i]);
        if (return_value != SPLT_OK)
        {
          return return_value;
        }
      }
    }
  }

  return return_value;
}

//function that gets information of each plugin
static int splt_p_open_get_plugins_data(splt_state *state)
{
  splt_plugins *pl = state->plug;
  int error = SPLT_OK;

  int i = 0;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    pl->data[i].plugin_handle = lt_dlopen(pl->data[i].plugin_filename);
    //error
    if (! pl->data[i].plugin_handle)
    {
      splt_u_print_debug("Error loading the plugin",0,pl->data[i].plugin_filename);
    }
    else
    {
      //get pointers to functions from the plugins
      //-this function must only be called once and here 
      *(void **) (&pl->data[i].func->check_plugin_is_for_file) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_check_plugin_is_for_file");
      *(void **) (&pl->data[i].func->search_syncerrors) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_search_syncerrors");
      *(void **) (&pl->data[i].func->dewrap) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_dewrap");
      *(void **) (&pl->data[i].func->simple_split) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_simple_split");
      *(void **) (&pl->data[i].func->split) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_split");
      *(void **) (&pl->data[i].func->init) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_init");
      *(void **) (&pl->data[i].func->end) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_end");
      *(void **) (&pl->data[i].func->scan_silence) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_scan_silence");
      *(void **) (&pl->data[i].func->set_original_tags) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_set_original_tags");
      *(void **) (&pl->data[i].func->set_plugin_info) =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_set_plugin_info");
      if (pl->data[i].func->set_plugin_info != NULL)
      {
        pl->data[i].func->set_plugin_info(&pl->data[i].info,&error);
      }
      else
      {
      }
    }
  }

  return error;
}

//main plugin function which finds the plugins and gets out the plugin data
//-returns possible error
//-for the moment should only be called once
int splt_p_find_get_plugins_data(splt_state *state)
{
  int return_value = SPLT_OK;

  //free old plugins
  splt_t_free_plugins(state);

  splt_u_print_debug("\nSearching for plugins ...",0,NULL);
  //find the plugins
  return_value = splt_p_find_plugins(state);

  if (return_value != SPLT_OK)
  {
    return return_value;
  }
  else
  {
    //open the plugins
    return_value = splt_p_open_get_plugins_data(state);
  }

  if (return_value >= 0)
  {
    //debug
    splt_plugins *pl = state->plug;
    splt_u_print_debug("\nNumber of plugins found = ",pl->number_of_plugins_found,NULL);
    splt_u_print_debug("",0,NULL);
    int i = 0;
    int err = 0;
    for (i = 0;i < pl->number_of_plugins_found;i++)
    {
      splt_t_set_current_plugin(state, i);
      if (pl->data[i].plugin_filename != NULL)
      {
        splt_u_print_debug("plugin filename = ",0,pl->data[i].plugin_filename);
      }
      const char *temp = splt_p_get_name(state, &err);
      splt_u_print_debug("plugin name = ",0,temp);
      float version = splt_p_get_version(state, &err);
      splt_u_print_debug("plugin version = ", version, NULL);
      temp = splt_p_get_extension(state,&err);
      splt_u_print_debug("extension = ",0,temp);
      splt_u_print_debug("",0,NULL);
    }
  }
  splt_t_set_current_plugin(state, -1);

  return return_value;
}

/* plugin info wrappers */

float splt_p_get_version(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return 0;
  }
  else
  {
    return pl->data[current_plugin].info.version;
  }
}

const char *splt_p_get_name(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return NULL;
  }
  else
  {
    return pl->data[current_plugin].info.name;
  }
}

const char *splt_p_get_extension(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return NULL;
  }
  else
  {
    return pl->data[current_plugin].info.extension;
  }
}

/* plugin function wrappers */

int splt_p_check_plugin_is_for_file(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return SPLT_FALSE;
  }
  else
  {
    if (pl->data[current_plugin].func->check_plugin_is_for_file != NULL)
    {
      return pl->data[current_plugin].func->check_plugin_is_for_file(state, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
      return SPLT_FALSE;
    }
  }
}

void splt_p_search_syncerrors(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->search_syncerrors != NULL)
    {
      //we free previous sync errors if necesssary
      splt_t_serrors_free(state);
      pl->data[current_plugin].func->search_syncerrors(state, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }
}

void splt_p_dewrap(splt_state *state, int listonly, const char *dir, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->dewrap != NULL)
    {
      pl->data[current_plugin].func->dewrap(state, listonly, dir, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }
}

void splt_p_split(splt_state *state, const char *final_fname, double begin_point,
    double end_point, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->split != NULL)
    {
      pl->data[current_plugin].func->split(state, final_fname,
          begin_point, end_point, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }
}

void splt_p_init(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->init != NULL)
    {
      pl->data[current_plugin].func->init(state, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }
}

void splt_p_end(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->end != NULL)
    {
      pl->data[current_plugin].func->end(state, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }
}


int splt_p_simple_split(splt_state *state, const char *output_fname, off_t begin,
    off_t end)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  int error = SPLT_OK;
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->simple_split != NULL)
    {
      error = pl->data[current_plugin].func->simple_split(state, output_fname, begin, end);
    }
    else
    {
      error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }

  return error;
}

int splt_p_scan_silence(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return 0;
  }
  else
  {
    if (pl->data[current_plugin].func->scan_silence != NULL)
    {
      return pl->data[current_plugin].func->scan_silence(state, error);
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }

  return 0;
}

void splt_p_set_original_tags(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_t_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->set_original_tags != NULL)
    {
      pl->data[current_plugin].func->set_original_tags(state, error);
    }
  }
}

