/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

/*! \file

Loading and unloading of plug-ins
*/
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include <ltdl.h>

#ifdef __WIN32__
#include <direct.h>
#endif

#include "splt.h"
#include "plugins.h"

int splt_p_append_plugin_scan_dir(splt_state *state, const char *dir)
{
  if (dir == NULL)
  {
    return SPLT_OK;
  }

  splt_plugins *pl = state->plug;

  if (pl->plugins_scan_dirs == NULL)
  {
    pl->plugins_scan_dirs = malloc(sizeof(char *));
  }
  else
  {
    pl->plugins_scan_dirs = realloc(pl->plugins_scan_dirs,
        sizeof(char *) * (pl->number_of_dirs_to_scan + 1));
  }
  if (pl->plugins_scan_dirs == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }

  size_t dir_size = strlen(dir) + 1;

  pl->plugins_scan_dirs[pl->number_of_dirs_to_scan] = malloc(sizeof(char) * dir_size);
  if (pl->plugins_scan_dirs[pl->number_of_dirs_to_scan] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }

  snprintf(pl->plugins_scan_dirs[pl->number_of_dirs_to_scan], dir_size, "%s", dir);
  pl->number_of_dirs_to_scan++;

  return SPLT_OK;
}

#ifdef __WIN32__
static int splt_p_filter_plugin_files(const struct _wdirent *de)
{
  wchar_t *file = (wchar_t *) de->d_name;
  const wchar_t *p_end = NULL;
  const wchar_t *p_start = NULL;

  if (!file)
  {
    return 0;
  }

  if (wcslen(file) < 8)
  {
    return 0;
  }

  if (wcsncmp(file, L"libsplt_", 8) != 0)
  {
    return 0;
  }

  splt_d_print_debug(NULL, "Looking at the file _%s_\n", splt_w32_utf16_to_utf8(file));

  p_start = wcschr(file,'.');
  p_end = wcsrchr(file,'.');
  if ((p_end != NULL) && (p_start == p_end))
  {
    if (wcscmp(p_end, L".dll") == 0)
    {
      return 1;
    }
  }

  return 0;
}
#else
static int splt_p_filter_plugin_files(const struct dirent *de)
{
  char *file = (char *) de->d_name;
  const char *p_end = NULL;
  const char *p_start = NULL;

  if (!file)
  {
    return 0;
  }

  if (strlen(file) < 8)
  {
    return 0;
  }

  if (strncmp(file,"libsplt_", 8) != 0)
  {
    return 0;
  }

  splt_d_print_debug(NULL, "Looking at the file _%s_\n", file);

  p_start = strchr(file,'.');

#ifndef __WIN32__
  //gnu/linux .so.0
  p_end = strstr(file, ".so.0");
  if (p_end != NULL && (p_start == p_end) && (*(p_end+5) == '\0'))
  {
    return 1;
  }
#endif

  p_end = strrchr(file,'.');
  if ((p_end != NULL) && (p_start == p_end))
  {
    //windows .dll
#ifdef __WIN32__
    if (strcmp(p_end,".dll") == 0)
    {
      return 1;
    }
#else
    //bsd .sl & darwin .dylib
    if ((strcmp(p_end,".sl") == 0) || (strcmp(p_end,".dylib") == 0))
    {
      return 1;
    }
#endif
  }

  return 0;
}
#endif

static int splt_p_alloc_init_new_plugin(splt_plugins *pl)
{
  int return_value = SPLT_OK;

  if (pl->data == NULL)
  {
    pl->data = malloc(sizeof(splt_plugin_data) * (pl->number_of_plugins_found+1));
    if (pl->data == NULL)
    {
      return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return return_value;
    }
  }
  else
  {
    pl->data = realloc(pl->data,sizeof(splt_plugin_data) *
        (pl->number_of_plugins_found+1));
    if (pl->data == NULL)
    {
      return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return return_value;
    }
  }

  pl->data[pl->number_of_plugins_found].func = NULL;
  pl->data[pl->number_of_plugins_found].plugin_handle = NULL;
  pl->data[pl->number_of_plugins_found].info.version = 0;
  pl->data[pl->number_of_plugins_found].info.name = NULL;
  pl->data[pl->number_of_plugins_found].info.extension = NULL;
  pl->data[pl->number_of_plugins_found].info.upper_extension = NULL;
  pl->data[pl->number_of_plugins_found].plugin_filename = NULL;

  return return_value;
}

/*! scans the directory *directory for plugins

directory must not be NULL
*/
static int splt_p_scan_dir_for_plugins(splt_state *state, splt_plugins *pl, const char *directory)
{
  int return_value = SPLT_OK;

#ifdef __WIN32__
  struct _wdirent **files = NULL;
#else
  struct dirent **files = NULL;
#endif

  int number_of_files = 0;
  errno = 0;
#ifdef __WIN32__
  number_of_files = wscandir(directory, &files, splt_p_filter_plugin_files, walphasort);
#else
  number_of_files = scandir(directory, &files, splt_p_filter_plugin_files, alphasort);
#endif
  int new_number_of_files = number_of_files;

  if (number_of_files == -1)
  {
    if (errno == ENOMEM)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }
  else if (new_number_of_files >= 0)
  {
    char *dir_and_fname = NULL;

    //for all the filtered found plugins,
    //copy their name
    while (new_number_of_files--)
    {
#ifdef __WIN32__
      char *fname = splt_w32_utf16_to_utf8(files[new_number_of_files]->d_name);
#else
      char *fname = files[new_number_of_files]->d_name;
#endif

#ifdef __WIN32__
      splt_su_copy(fname, &dir_and_fname);
#else
      splt_su_copy(directory, &dir_and_fname);
      splt_su_append_str(&dir_and_fname, SPLT_DIRSTR, fname, NULL);
#endif

      int i = 0;
      int err = SPLT_OK;
      for (i = 0;i < pl->number_of_plugins_found;i++)
      {
        if (splt_check_is_the_same_file(state, dir_and_fname, pl->data[i].plugin_filename, &err))
        {
          goto loop_end;
        }

        if (err != SPLT_OK)
        {
          return_value = err;
          goto end;
        }
      }

      int alloc_err = splt_p_alloc_init_new_plugin(pl);
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
      memset(pl->data[pl->number_of_plugins_found].func, 0, sizeof(splt_plugin_func));

      splt_su_copy(dir_and_fname, 
          &pl->data[pl->number_of_plugins_found].plugin_filename);

      pl->number_of_plugins_found++;

loop_end:
      ;
    }

end:
    ;

    if (dir_and_fname)
    {
      free(dir_and_fname);
      dir_and_fname = NULL;
    }

    if (files)
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

  //for each scan directory, look for the files starting with 'splt' and
  //ending with '.so' on unix-like and '.dll' on windows
  int i = 0;

  char current_dir[3] = { '\0' };
  snprintf(current_dir,3,".%c",SPLT_DIRCHAR);

  for (i = 0;i < pl->number_of_dirs_to_scan;i++)
  {
    if (pl->plugins_scan_dirs[i] != NULL)
    {
      splt_d_print_debug(state,"Scanning plugins in the directory _%s_\n",
          pl->plugins_scan_dirs[i]);

      //don't check if directory exists if the directory is ./ on unix-like
      //OSes or .\\ on windows
      if (((strlen(pl->plugins_scan_dirs[i]) >= 2) &&
            strncmp(pl->plugins_scan_dirs[i], current_dir,2) == 0) ||
          splt_io_check_if_directory(pl->plugins_scan_dirs[i]))
      {
        return_value = splt_p_scan_dir_for_plugins(state, pl, pl->plugins_scan_dirs[i]);
        if (return_value != SPLT_OK)
        {
          return return_value;
        }
      }
    }
  }

  return return_value;
}

//! Used by splt_p_free_plugin_data
static void splt_p_free_plugin_data_info(splt_plugin_data *pl_data)
{
  if (pl_data->info.name)
  {
    free(pl_data->info.name);
    pl_data->info.name = NULL;
  }
  if (pl_data->info.extension)
  {
    free(pl_data->info.extension);
    pl_data->info.extension = NULL;
  }
  if (pl_data->info.upper_extension)
  {
    free(pl_data->info.upper_extension);
    pl_data->info.upper_extension = NULL;
  }
}

static void splt_p_free_plugin_data(splt_plugin_data *pl_data)
{
  splt_p_free_plugin_data_info(pl_data);
  if (pl_data->plugin_filename)
  {
    free(pl_data->plugin_filename);
    pl_data->plugin_filename = NULL;
  }
  if (pl_data->plugin_handle)
  {
    lt_dlclose(pl_data->plugin_handle);
    pl_data->plugin_handle = NULL;
  }
  if (pl_data->func)
  {
    free(pl_data->func);
    pl_data->func = NULL;
  }
}

int splt_p_move_replace_plugin_data(splt_state *state, int old, int new)
{
  splt_plugins *pl = state->plug;

  splt_p_free_plugin_data(&pl->data[new]);

  pl->data[new].func = malloc(sizeof(splt_plugin_func));
  if (pl->data[new].func == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  memset(pl->data[new].func,0,sizeof(splt_plugin_func));

  int plugin_fname_len = strlen(pl->data[old].plugin_filename) + 1;
  pl->data[new].plugin_filename = malloc(sizeof(char) * plugin_fname_len);
  if (pl->data[new].plugin_filename == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->data[new].plugin_filename, plugin_fname_len, "%s",
      pl->data[old].plugin_filename);

  splt_p_free_plugin_data(&pl->data[old]);

  return SPLT_OK;
}

//shifts all the elements to the left, starting at element index+1
static int splt_p_shift_left_plugins_data(splt_state *state, int index)
{
  int i = 0;
  splt_plugins *pl = state->plug;

  for (i = index+1;i < pl->number_of_plugins_found;i++)
  {
    int err = splt_p_move_replace_plugin_data(state, i, i - 1);
    if (err != SPLT_OK)
    {
      return err;
    }
  }

  return SPLT_OK;
}

//function that gets information of each plugin
static int splt_p_open_get_valid_plugins(splt_state *state)
{
  splt_plugins *pl = state->plug;

  int *plugin_index_to_remove = NULL;
  int number_of_plugins_to_remove = 0;

  int error = SPLT_OK;

  int i = 0;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    splt_d_print_debug(state,"\nTrying to open the plugin _%s_ ...\n",
        pl->data[i].plugin_filename);

    //ltdl currently does not supports windows unicode path/filename
    pl->data[i].plugin_handle = lt_dlopen(pl->data[i].plugin_filename);
    //error
    if (! pl->data[i].plugin_handle)
    {
      splt_d_print_debug(state,"Error loading the plugin _%s_\n", pl->data[i].plugin_filename);
      splt_d_print_debug(state," - error message from libltdl: _%s_\n", lt_dlerror());

      //keep the index of this failed plugin in order to remove it
      //afterwards
      if (! plugin_index_to_remove)
      {
        plugin_index_to_remove = malloc(sizeof(int));
      }
      else
      {
        plugin_index_to_remove = realloc(plugin_index_to_remove, sizeof(int) * (number_of_plugins_to_remove + 1));
      }
      plugin_index_to_remove[number_of_plugins_to_remove] = i;
      number_of_plugins_to_remove++;
    }
    else
    {
      splt_d_print_debug(state," - success !\n");

      pl->data[i].func->set_plugin_info =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_set_plugin_info");
      if (pl->data[i].func->set_plugin_info != NULL)
      {
        pl->data[i].func->set_plugin_info(&pl->data[i].info,&error);
      }

      //look if we already have a plugin with the same name
      int j = 0;
      splt_p_set_current_plugin(state, i);
      const char *current_plugin_name = splt_p_get_name(state, &error);
      for (j = 0;j < i;j++)
      {
        if (pl->data[j].plugin_handle)
        {
          splt_p_set_current_plugin(state, j);
          const char *plugin_name = splt_p_get_name(state, &error);
          //if we do have one with the same name,
          if (strcmp(plugin_name,current_plugin_name) == 0)
          {
            //keep unique index to remove afterwards
            int unique = SPLT_TRUE;
            int k = 0;
            for (k = 0;k < number_of_plugins_to_remove;k++)
            {
              if (i == plugin_index_to_remove[k])
              {
                unique = SPLT_FALSE;
                break;
              }
            }
            if (unique)
            {
              if (! plugin_index_to_remove)
              {
                plugin_index_to_remove = malloc(sizeof(int));
              }
              else
              {
                plugin_index_to_remove = realloc(plugin_index_to_remove,
                    sizeof(int) * (number_of_plugins_to_remove + 1));
              }
              plugin_index_to_remove[number_of_plugins_to_remove] = i;
              number_of_plugins_to_remove++;
            }
          }
        }
      }
    }
  }

  //remove the plugins that failed to open
  //-some plugins fail to open twice (on Windows for example)
  int left_shift = 0;
  //we shift to the left the plugin data in order to replace the bad
  //plugins with the others
  for (i = 0;i < number_of_plugins_to_remove;i++) 
  {
    int index_to_remove = plugin_index_to_remove[i] - left_shift;

    splt_d_print_debug(state,"Removing the plugin _%s_ at index %d\n",
        pl->data[index_to_remove].plugin_filename, index_to_remove);

    error = splt_p_shift_left_plugins_data(state, index_to_remove);
    if (error < SPLT_OK) { break; }

    pl->number_of_plugins_found--;
    left_shift++;
  }

  //free the plugin index to remove
  if (plugin_index_to_remove)
  {
    free(plugin_index_to_remove);
    plugin_index_to_remove = NULL;
  }
  number_of_plugins_to_remove = 0;

  return error;
}

//main plugin function which finds the plugins and gets out the plugin data
//-returns possible error
//-for the moment should only be called once
int splt_p_find_get_plugins_data(splt_state *state)
{
  int return_value = SPLT_OK;

  splt_d_print_debug(state,"\nSearching for plugins ...\n");

  //find the plugins
  return_value = splt_p_find_plugins(state);

  if (return_value != SPLT_OK)
  {
    return return_value;
  }
  else
  {
    //open the plugins
    return_value = splt_p_open_get_valid_plugins(state);
  }

  if (return_value >= 0)
  {
    splt_plugins *pl = state->plug;
    splt_d_print_debug(state,"\nNumber of plugins found: _%d_\n", pl->number_of_plugins_found);
    int i = 0;
    int err = 0;
    for (i = 0;i < pl->number_of_plugins_found;i++)
    {
      pl->data[i].plugin_handle = lt_dlopen(pl->data[i].plugin_filename);

      pl->data[i].func->check_plugin_is_for_file =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_check_plugin_is_for_file");
      pl->data[i].func->search_syncerrors =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_search_syncerrors");
      pl->data[i].func->dewrap =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_dewrap");
      pl->data[i].func->simple_split =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_simple_split");
      pl->data[i].func->split =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_split");
      pl->data[i].func->init =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_init");
      pl->data[i].func->end =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_end");
      pl->data[i].func->scan_silence =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_scan_silence");
      pl->data[i].func->scan_trim_silence =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_scan_trim_silence");
      pl->data[i].func->set_original_tags =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_set_original_tags");
      pl->data[i].func->clear_original_tags =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_clear_original_tags");
      pl->data[i].func->set_plugin_info =
        lt_dlsym(pl->data[i].plugin_handle, "splt_pl_set_plugin_info");
      if (pl->data[i].func->set_plugin_info != NULL)
      {
        splt_p_free_plugin_data_info(&pl->data[i]);
        pl->data[i].func->set_plugin_info(&pl->data[i].info,&err);
      }

      splt_p_set_current_plugin(state, i);
      if (pl->data[i].plugin_filename != NULL)
      {
        splt_d_print_debug(state,"plugin filename = _%s_\n", pl->data[i].plugin_filename);
      }

      const char *temp = splt_p_get_name(state, &err);
      splt_d_print_debug(state,"plugin name = _%s_\n", temp);

      float version = splt_p_get_version(state, &err);
      splt_d_print_debug(state,"plugin version = _%lf_\n", version);

      temp = splt_p_get_extension(state,&err);
      splt_d_print_debug(state,"extension = _%s_\n\n", temp);
    }
  }
  splt_p_set_current_plugin(state, -1);

  return return_value;
}

/* plugin info wrappers */

float splt_p_get_version(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
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

const char *splt_p_get_upper_extension(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return NULL;
  }
  else
  {
    return pl->data[current_plugin].info.upper_extension;
  }
}

/* plugin function wrappers */

int splt_p_check_plugin_is_for_file(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->search_syncerrors != NULL)
    {
      splt_se_serrors_free(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
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

double splt_p_split(splt_state *state, const char *final_fname, double begin_point,
    double end_point, int *error, int save_end_point)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return end_point;
  }
  else
  {
    int err = SPLT_OK;

    splt_d_print_debug(state, "split creating directories of final fname ... _%s_\n", final_fname);

    splt_io_create_output_dirs_if_necessary(state, final_fname, &err);
    if (err < 0) { *error = err; return end_point; }

    if (pl->data[current_plugin].func->split != NULL)
    {
      double new_end_point = pl->data[current_plugin].func->split(state, final_fname,
          begin_point, end_point, error, save_end_point);

      splt_d_print_debug(state, "New end point after split = _%lf_\n", new_end_point);

      return new_end_point;
    }
    else
    {
      *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    }
  }

  return end_point;
}

void splt_p_init(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
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
  int current_plugin = splt_p_get_current_plugin(state);
  int error = SPLT_OK;
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return error;
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
  int current_plugin = splt_p_get_current_plugin(state);
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

int splt_p_scan_trim_silence(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return 0;
  }
  else
  {
    if (pl->data[current_plugin].func->scan_trim_silence != NULL)
    {
      return pl->data[current_plugin].func->scan_trim_silence(state, error);
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
  int current_plugin = splt_p_get_current_plugin(state);
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

void splt_p_clear_original_tags(splt_state *state, int *error)
{
  splt_plugins *pl = state->plug;
  int current_plugin = splt_p_get_current_plugin(state);
  if ((current_plugin < 0) || (current_plugin >= pl->number_of_plugins_found))
  {
    *error = SPLT_ERROR_NO_PLUGIN_FOUND;
    return;
  }
  else
  {
    if (pl->data[current_plugin].func->clear_original_tags != NULL)
    {
      pl->data[current_plugin].func->clear_original_tags(&state->original_tags);
    }
  }
}

static int splt_p_set_default_plugins_scan_dirs(splt_state *state)
{
  int err = SPLT_OK;
  char *dir = NULL;

#ifndef __WIN32__
  err = splt_p_append_plugin_scan_dir(state, SPLT_PLUGINS_DIR);
  if (err < 0) { return err; }
#endif

  err = splt_su_append_str(&dir, getenv("HOME"), SPLT_DIRSTR, ".libmp3splt", NULL);
  if (err < 0) { goto end; }
  err = splt_p_append_plugin_scan_dir(state, dir);
  free(dir);
  dir = NULL;

  err = splt_su_append_str(&dir, ".", SPLT_DIRSTR, NULL);
  if (err < 0) { goto end; }
  err = splt_p_append_plugin_scan_dir(state, dir);

end:
  if (dir)
  {
    free(dir);
    dir = NULL;
  }

  return err;
}

int splt_p_set_default_values(splt_state *state)
{
  state->plug->plugins_scan_dirs = NULL;
  state->plug->number_of_plugins_found = 0;
  state->plug->data = NULL;
  state->plug->number_of_dirs_to_scan = 0;

  return splt_p_set_default_plugins_scan_dirs(state);
}

void splt_p_free_plugins(splt_state *state)
{
  splt_plugins *pl = state->plug;
  int i = 0;

  if (pl->plugins_scan_dirs)
  {
    for (i = 0;i < pl->number_of_dirs_to_scan;i++)
    {
      if (pl->plugins_scan_dirs[i])
      {
        free(pl->plugins_scan_dirs[i]);
        pl->plugins_scan_dirs[i] = NULL;
      }
    }
    free(pl->plugins_scan_dirs);
    pl->plugins_scan_dirs = NULL;
    pl->number_of_dirs_to_scan = 0;
  }
  if (pl->data)
  {
    for (i = 0;i < pl->number_of_plugins_found;i++)
    {
      splt_p_free_plugin_data(&pl->data[i]);
    }
    free(pl->data);
    pl->data = NULL;
    pl->number_of_plugins_found = 0;
  }
}

void splt_p_set_current_plugin(splt_state *state, int current_plugin)
{
  //-1 means no plugin
  if (current_plugin >= -1)
  {
    state->current_plugin = current_plugin;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT,__func__, current_plugin, NULL);
  }
}

int splt_p_get_current_plugin(splt_state *state)
{
  return state->current_plugin;
}

int splt_p_file_is_supported_by_plugins(splt_state *state, const char *fname)
{
  splt_plugins *pl = state->plug;

  int fname_length = strlen(fname);
  if (fname_length > 3)
  {
    char *ptr_to_compare = strrchr(fname, '.');
    if (ptr_to_compare)
    {
      int i = 0;
      for (i = 0;i < pl->number_of_plugins_found;i++)
      {
        char *pl_extension = pl->data[i].info.extension;
        char *pl_upper_extension = pl->data[i].info.upper_extension;

        if ((strcmp(ptr_to_compare, pl_extension) == 0) ||
            (strcmp(ptr_to_compare, pl_upper_extension) == 0))
        {
          return SPLT_TRUE;
        }
      }
    }
  }

  return SPLT_FALSE;
}

