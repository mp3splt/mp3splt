/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
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
#include <dlfcn.h>

#include "splt.h"

//sets the plugin scan directories
int splt_p_set_default_plugins_scan_dirs(splt_state *state)
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
  snprintf(temp,2048,SPLT_PLUGINS_DIR);
  pl->plugins_scan_dirs[0] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[0] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[0],strlen(temp)+1,temp);

  //we put the home libmp3splt home directory
  snprintf(temp,2048,"%s%c%s",getenv("HOME"),SPLT_DIRCHAR,".libmp3splt");
  pl->plugins_scan_dirs[1] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[1] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[1],strlen(temp)+1,temp);

  //we put the current directory
  memset(temp,'\0',2048);
  pl->plugins_scan_dirs[2] = malloc(sizeof(char) * (strlen(temp)+1));
  if (pl->plugins_scan_dirs[2] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->plugins_scan_dirs[2],strlen(temp)+1,temp);

  return SPLT_OK;
}

//function to filter the plugin files
int splt_p_filter_plugin_files(const struct dirent *de)
{
  int file_length = strlen(de->d_name);
  char *file = de->d_name;
  char *p = NULL;
  //if the name starts with splt_and contains .so or .sl or .dll
  if (strncmp(file,"splt_",5) == 0)
  {
    //find the last '.' character
    p = strrchr(file,'.');
    if (p != NULL)
    {
      if ((strcmp(p,".so") == 0) ||
          (strcmp(p,".sl") == 0) ||
          (strcmp(p,".dll") == 0))
      {
        return 1;
      }
    }
  }

  return 0;
}

//scans the directory *directory for plugins
int splt_p_scan_dir_for_plugins(splt_plugins *pl, char *directory)
{
  int return_value = SPLT_OK;

  struct dirent **files = NULL;
  int number_of_files = 0;
  //scan the directory
  number_of_files = scandir(directory, &files,
      splt_p_filter_plugin_files, alphasort);
  int directory_len = strlen(directory);
  int old_number_of_files = number_of_files;

  //ignore errors
  if (number_of_files >= 0)
  {
    //for all the filtered found plugins,
    //copy their name
    while (number_of_files--)
    {
      char *fname = files[number_of_files]->d_name;
      int fname_len = strlen(fname);
      int number_of_chars_to_copy = fname_len - SPLT_PLUGIN_EXT_SIZE;

      //allocate memory for the plugin infos
      if (pl->info == NULL)
      {
        pl->info = malloc(sizeof(splt_plugin_info) * (pl->number_of_plugins_found+1));
        if (pl->info == NULL)
        {
          return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          goto end;
        }
      }
      else
      {
        pl->info = realloc(pl->info,sizeof(splt_plugin_info) *
            (pl->number_of_plugins_found+1));
        if (pl->info == NULL)
        {
          return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          goto end;
        }
      }
      pl->info[pl->number_of_plugins_found].plugin_name = NULL;
      pl->info[pl->number_of_plugins_found].func = NULL;
      pl->info[pl->number_of_plugins_found].func = malloc(sizeof(splt_plugin_func));
      splt_plugin_func *pl_func = pl->info[pl->number_of_plugins_found].func;
      memset(pl_func,0,sizeof(splt_plugin_func));
      if (pl->info[pl->number_of_plugins_found].func == NULL)
      {
        return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto end;
      }
      pl->info[pl->number_of_plugins_found].plugin_handle = NULL;
      pl->info[pl->number_of_plugins_found].plugin_version = 0;
      pl->info[pl->number_of_plugins_found].plugin_filename = malloc(sizeof(char) *
          (fname_len+directory_len+2));
      if (pl->info[pl->number_of_plugins_found].plugin_filename == NULL)
      {
        return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto end;
      }
      //set the plugin name and the directory
      snprintf(pl->info[pl->number_of_plugins_found].plugin_filename,
          fname_len+directory_len+2,"%s%c%s",directory,SPLT_DIRCHAR,fname);
      pl->number_of_plugins_found++;
    }
    number_of_files = old_number_of_files;

end:
    //free memory
    while (number_of_files--)
    {
      free(files[number_of_files]);
      files[number_of_files] = NULL;
    }
    free(files);
    files = NULL;
  }

  return return_value;
}

//finds the plugins
//-returns SPLT_OK if no error and SPLT_ERROR_CANNOT_FIND_PLUGINS if
//no plugin was found
int splt_p_find_plugins(splt_state *state)
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
  for (i = 0;i < pl->number_of_dirs_to_scan;i++)
  {
    return_value = splt_p_scan_dir_for_plugins(pl, pl->plugins_scan_dirs[i]);
    if (return_value != SPLT_OK)
    {
      break;
    }
  }

  return return_value;
}

//function that gets information of each plugin
int splt_p_open_get_plugins_info(splt_state *state)
{
  splt_plugins *pl = state->plug;
  int i = 0;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    pl->info[i].plugin_handle = lt_dlopen(pl->info[i].plugin_filename);
    //error
    if (! pl->info[i].plugin_handle)
    {
      splt_u_print_debug("Error loading the plugin",0,pl->info[i].plugin_filename);
    }
    else
    {
      //get pointers to functions from the plugins
      *(void **) (&pl->info[i].func->get_plugin_name) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_get_plugin_name");
      *(void **) (&pl->info[i].func->get_plugin_version) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_get_plugin_version");
      *(void **) (&pl->info[i].func->check_plugin_is_for_file) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_check_plugin_is_for_file");
      *(void **) (&pl->info[i].func->search_syncerrors) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_search_syncerrors");
      *(void **) (&pl->info[i].func->dewrap) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_dewrap");
      *(void **) (&pl->info[i].func->set_total_time) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_set_total_time");
      *(void **) (&pl->info[i].func->simple_split) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_simple_split");
      *(void **) (&pl->info[i].func->scan_silence) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_scan_silence");
      *(void **) (&pl->info[i].func->free_plugin_state) =
        lt_dlsym(pl->info[i].plugin_handle, "splt_pl_free_plugin_state");
    }
  }
}

//main plugin function which finds the plugins and gets out the plugin
//info - returns possible error
//-for the moment should only be called once
int splt_p_find_get_plugins_info(splt_state *state)
{
  int return_value = SPLT_OK;

  //free old plugins
  splt_t_free_plugins(state);

  //find the plugins
  return_value = splt_p_find_plugins(state);

  //here we have the plugins found
  //debug
  splt_plugins *pl = state->plug;
  splt_u_print_debug("\nNumber of plugins found = ",pl->number_of_plugins_found,NULL);
  int i = 0;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    splt_u_print_debug("plugin filename = ",0,pl->info[i].plugin_filename);
  }
  splt_u_print_debug("",0,NULL);

  if (return_value != SPLT_OK)
  {
    return return_value;
  }
  else
  {
    //open the plugins
    return_value = splt_p_open_get_plugins_info(state);
  }

  return return_value;
}

/* plugin function wrappers */

float splt_p_get_plugin_version(splt_state *state)
{
}

char *splt_p_get_plugin_name(splt_state *state)
{
}

int splt_p_check_plugin_is_for_file(splt_state *state, char *filename, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->check_plugin_is_for_file != NULL)
  {
    return pl->info[state->current_plugin].func->check_plugin_is_for_file(filename,error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
    return SPLT_FALSE;
  }
}

void splt_p_search_syncerrors(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->search_syncerrors != NULL)
  {
    pl->info[state->current_plugin].func->search_syncerrors(state, error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
  }
}

void splt_p_dewrap(splt_state *state, int listonly, char *dir, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->dewrap != NULL)
  {
    pl->info[state->current_plugin].func->dewrap(state, listonly, dir, error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
  }
}

void splt_p_set_total_time(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->set_total_time != NULL)
  {
    pl->info[state->current_plugin].func->set_total_time(state, error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
  }
}

void splt_p_simple_split(splt_state *state, char *final_fname, double begin_point,
    double end_point, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->simple_split != NULL)
  {
    pl->info[state->current_plugin].func->simple_split(state, final_fname,
        begin_point, end_point, error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
  }
}

void splt_p_scan_silence(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->scan_silence != NULL)
  {
    pl->info[state->current_plugin].func->scan_silence(state, error);
  }
  else
  {
    *error = SPLT_ERROR_UNSUPPORTED_FEATURE;
  }
}


void splt_p_free_plugin_state(splt_state *state)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->free_plugin_state != NULL)
  {
    pl->info[state->current_plugin].func->free_plugin_state(state);
  }
}

void splt_p_set_original_tags(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  if (pl->info[state->current_plugin].func->set_original_tags != NULL)
  {
    pl->info[state->current_plugin].func->set_original_tags(state, error);
  }
}

