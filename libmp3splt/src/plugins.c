/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifdef __WIN32__
#include <direct.h>
#endif

int splt_p_append_plugin_scan_dir(splt_state *state, char *dir)
{
  splt_plugins *pl = state->plug;

  //allocate memory for another char * pointer
  if (pl->plugins_scan_dirs == NULL)
  {
    //allocate memory
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
  pl->plugins_scan_dirs[pl->number_of_dirs_to_scan] = NULL;

  //allocate memory for this directory name
  pl->plugins_scan_dirs[pl->number_of_dirs_to_scan] = malloc(sizeof(char) * (strlen(dir)+1));
  if (pl->plugins_scan_dirs[pl->number_of_dirs_to_scan] == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }

  snprintf(pl->plugins_scan_dirs[pl->number_of_dirs_to_scan],strlen(dir)+1,"%s",dir);

  pl->number_of_dirs_to_scan++;

  return SPLT_OK;
}

//sets the plugin scan directories
int splt_p_set_default_plugins_scan_dirs(splt_state *state)
{
  int err = SPLT_OK;

  //temporary variable that we use to set the default directories
  char temp[2048] = { '\0' };

#ifndef __WIN32__
  //we put the default plugin directory
  snprintf(temp,2048,"%s",SPLT_PLUGINS_DIR);
  if ((err = splt_p_append_plugin_scan_dir(state, temp)) != SPLT_OK) { return err; }
#endif

  //we put the home libmp3splt home directory
  snprintf(temp,2048,"%s%c%s",getenv("HOME"),SPLT_DIRCHAR,".libmp3splt");
  if ((err = splt_p_append_plugin_scan_dir(state, temp)) != SPLT_OK) { return err; }

  //we put the current directory
  memset(temp,'\0',2048);
  snprintf(temp,2048,".%c",SPLT_DIRCHAR);
  if ((err = splt_p_append_plugin_scan_dir(state, temp)) != SPLT_OK) { return err; }

  return err;
}

//function to filter the plugin files
#ifdef __WIN32__
static int splt_p_filter_plugin_files(const struct _wdirent *de)
#else
static int splt_p_filter_plugin_files(const struct dirent *de)
#endif
{
#ifdef __WIN32__
  char *file = splt_u_win32_utf16_to_utf8(de->d_name);
#else
  char *file = (char *) de->d_name;
#endif
  char *p_end = NULL;
  char *p_start = NULL;
  if (file)
  {
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
#ifdef __WIN32__
            if (file)
            {
              free(file);
              file = NULL;
            }
#endif
            return 1;
          }
        }
      }
    }
  }

#ifdef __WIN32__
  if (file)
  {
    free(file);
    file = NULL;
  }
#endif

  return 0;
}

//scans the directory *directory for plugins
//-directory must not be NULL
static int splt_p_scan_dir_for_plugins(splt_state *state, splt_plugins *pl, const char *directory)
{
  int return_value = SPLT_OK;

#ifdef __WIN32__
  struct _wdirent **files = NULL;
#else
  struct dirent **files = NULL;
#endif

  int number_of_files = 0;
  //scan the directory
  number_of_files = scandir(directory, &files, splt_p_filter_plugin_files,
      alphasort);
  int directory_len = strlen(directory);
  int new_number_of_files = number_of_files;

  if (number_of_files == -1)
  {
		return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  else if (new_number_of_files >= 0)
  {
    char *fname = NULL;

    //for all the filtered found plugins,
    //copy their name
    while (new_number_of_files--)
    {
#ifdef __WIN32__
      fname = splt_u_win32_utf16_to_utf8(files[new_number_of_files]->d_name);
#else
      fname = files[new_number_of_files]->d_name;
#endif
      int fname_len = strlen(fname);

      //get the full directory + filename
      int dir_and_fname_len = fname_len + directory_len + 3;
      char *dir_and_fname = malloc(sizeof(char) * dir_and_fname_len);
      if (dir_and_fname == NULL)
      {
        return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto end;
      }
      snprintf(dir_and_fname, dir_and_fname_len - 1, "%s%c%s",directory,SPLT_DIRCHAR,fname);

      //check if we already have a plugin with the same file
      int we_already_have_this_plugin_file = SPLT_FALSE;
      int i = 0;
      int err = SPLT_OK;
      for (i = 0;i < pl->number_of_plugins_found;i++)
      {
        if (splt_check_is_the_same_file(state, dir_and_fname, pl->data[i].plugin_filename, &err))
        {
          we_already_have_this_plugin_file = SPLT_TRUE;
          break;
        }

        if (err != SPLT_OK)
        {
          return_value = err;
          goto end;
        }
      }

      //if we don't have a plugin from the same file, add this new plugin
      if (! we_already_have_this_plugin_file)
      {
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
            dir_and_fname_len);
        if (pl->data[pl->number_of_plugins_found].plugin_filename == NULL)
        {
          return_value = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          goto end;
        }
        //set the plugin name and the directory
        snprintf(pl->data[pl->number_of_plugins_found].plugin_filename,
            dir_and_fname_len - 1,"%s%c%s",directory,SPLT_DIRCHAR,fname);

        pl->number_of_plugins_found++;
      }

      //free some memory
      if (dir_and_fname)
      {
        free(dir_and_fname);
        dir_and_fname = NULL;
      }

#ifdef __WIN32__
      if (fname)
      {
        free(fname);
        fname = NULL;
      }
#endif
    }

end:
#ifdef __WIN32__
    if (fname)
    {
      free(fname);
      fname = NULL;
    }
#endif
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

int splt_p_move_replace_plugin_data(splt_state *state, int old, int new)
{
  splt_plugins *pl = state->plug;

  splt_t_free_plugin_data(pl->data[new]);

  pl->data[new].func = malloc(sizeof(splt_plugin_func));
  if (pl->data[new].func == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  memcpy(pl->data[new].func, pl->data[old].func, sizeof(pl->data[old].func));

  int plugin_fname_len = strlen(pl->data[old].plugin_filename) + 1;
  pl->data[new].plugin_filename = malloc(sizeof(char) * plugin_fname_len);
  if (pl->data[new].plugin_filename == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  snprintf(pl->data[new].plugin_filename, plugin_fname_len, "%s",
      pl->data[old].plugin_filename);

  splt_t_free_plugin_data(pl->data[old]);

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
static int splt_p_open_get_plugins_data(splt_state *state)
{
  splt_plugins *pl = state->plug;

  int *plugin_index_to_remove = NULL;
  int number_of_plugins_to_remove = 0;

  int error = SPLT_OK;

  int i = 0;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    splt_u_print_debug("\nTrying to open the plugin ...",0,pl->data[i].plugin_filename);

    //ltdl currently does not supports windows unicode path/filename
    pl->data[i].plugin_handle = lt_dlopen(pl->data[i].plugin_filename);
    //error
    if (! pl->data[i].plugin_handle)
    {
      splt_u_print_debug("Error loading the plugin",0,pl->data[i].plugin_filename);
      splt_u_print_debug(" - error message from libltdl : ",0,lt_dlerror());

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
      splt_u_print_debug(" - success !",0,NULL);

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
        /*splt_t_set_current_plugin(state, i);
        const char *p_name = splt_p_get_name(state, &error);
        fprintf(stdout," Plugin NAME = _%s_\n",p_name);
        fflush(stdout);*/
      }
      else
      {
      }

      //look if we already have a plugin with the same name
      int j = 0;
      splt_t_set_current_plugin(state, i);
      const char *current_plugin_name = splt_p_get_name(state, &error);
      for (j = 0;j < i;j++)
      {
        if (pl->data[j].plugin_handle)
        {
          splt_t_set_current_plugin(state, j);
          const char *plugin_name = splt_p_get_name(state, &error);
          //if we do have one with the same name,
          if (strcmp(plugin_name,current_plugin_name) == 0)
          {
            //keep its index to remove it afterwards
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

    splt_u_print_debug("Removing the plugin ",0, pl->data[index_to_remove].plugin_filename);

    error = splt_p_shift_left_plugins_data(state, index_to_remove);
    if (error != SPLT_OK)
    {
      goto end;
    }

    pl->number_of_plugins_found--;
    left_shift++;
  }

end:
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
    double end_point, int *error, int save_end_point)
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
          begin_point, end_point, error, save_end_point);
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

