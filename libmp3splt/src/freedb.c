/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
 *
 *********************************************************/

/**********************************************************
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
 * 02111-1307, USA.
 *********************************************************/

/*! \file 

All functions that are needed in order to do a Freedb search

Don't use these functions directly. The version of these functions
that is meant to be used directly are all in mp3splt.c.
*/

#include <string.h>
#include <unistd.h>

#ifdef __WIN32__
#include <conio.h>
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "splt.h"

#include "freedb.h"

#if defined(__BEOS__) && !defined (HAS_GETPASS)
//used for proxy (proxy not implemented)
//#warning Faking getpass() !!!
//char *getpass(char *p)
//{
//      char *ret;
//      ret = malloc(30);
//      if (!ret)
//              return NULL;
//      puts(p);
//      fgets(ret,30,stdin);
//      return ret;
//}
#endif
#if defined(__BEOS__) && (IPPROTO_UDP==1)
// net_server has a weird order for IPPROTO_
#else
#define closesocket close
#endif

// The alphabet fpr the base64 algorithm - for proxy (proxy not implemented)
//
// Base64 Algorithm: Base64.java v. 1.3.6 by Robert Harder
// Ported and optimized for C by Matteo Trotta
//
//const char alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//char *encode3to4 (unsigned char *source, int srcoffset, int num, char *destination, int destoffset)
//{
//
//    int inbuff=(num>0?(source[srcoffset]<<16):0)|(num>1?(source[srcoffset+1]<<8):0)|(num>2?(source[srcoffset+2]):0);
//    switch(num)
//    {
//      case 3:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = alphabet[(inbuff>>6)&0x3f];
//          destination[destoffset+3] = alphabet[(inbuff)&0x3f];
//          return destination;
//
//      case 2:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = alphabet[(inbuff>>6)&0x3f];
//          destination[destoffset+3] = '=';
//          return destination;
//
//      case 1:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = '=';
//          destination[destoffset+3] = '=';
//          return destination;
//      default:
//          return destination;
//    }
//}

//used for proxy (proxy not implemented)
//char *b64 (unsigned char *source, int len)
//{
//      char *out;
//      int d, e=0;
//      d = ((len*4/3)+((len%3)>0?4:0));
//      
//      out = malloc(d+1);
//      
//      memset(out, 0x00, d+1);
//      for(d=0;d<(len-2);d+=3,e+=4)
//              out = encode3to4(source, d, 3, out, e);
//      if(d<len)
//              out = encode3to4(source, d, len-d, out, e);
//
//      return out;
//}
// End of Base64 Algorithm

//! we analyse the freedb2 buffer for the CDs results
static int splt_freedb2_analyse_cd_buffer (char *buf, int size,
    splt_state *state, int *error)
{
  //temporary pointer
  char *temp = buf, *temp2 = NULL;

  //we replace the \r with \n
  while ((temp = strchr(temp,'\r')) != NULL)
  {
    *temp = '\n';
  }

  temp = NULL;
  do
  {
    //genre
    buf = strchr(buf, '\n');

    if (buf != NULL)
    {
      buf += 1;
      buf++;

      //disc id
      temp = strchr(buf, ' ');
      if (temp != NULL)
      {
        temp++;

        //artist / album
        //temp2 is the end of the line
        temp2 = strchr(temp+8,'\n');
        if (temp2 != NULL)
        {
          temp2++;

          splt_fu_freedb_set_disc(state, splt_fu_freedb_get_found_cds(state), 
              temp, buf, temp-buf);

          char *full_artist_album = malloc(temp2-(temp+8)-1);
          if (full_artist_album)
          {
            int max_chars = temp2-(temp+8)-1;
            snprintf(full_artist_album,max_chars,"%s",temp+9);
            //snprintf seems buggy
#ifdef __WIN32__					
            full_artist_album[max_chars-1] = '\0';
#endif
            splt_d_print_debug(state,"Setting the full artist album name _%s_\n", full_artist_album);

            //i!=-1 means that it's not a revision
            int i=0;
            int err = SPLT_OK;
            //here we have in album_name the name of the current album
            err = splt_fu_freedb_append_result(state, full_artist_album, i);
            if (err < 0)
            {
              if (full_artist_album)
              {
                free(full_artist_album);
                full_artist_album = NULL;
              }
              *error = err;
              return -2;
            }

            //free memory
            free(full_artist_album);
            full_artist_album = NULL;
          }
          else
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            return -2;
          }

          //next cd
          splt_fu_freedb_found_cds_next(state);
        }
        else
        {
          return -1;
        }
      }
    }
    else
    {
      return 0;
    }

  } while (((strstr(buf,"/"))!= NULL) &&
      ((strchr(buf,'\n'))!= NULL) &&
      (splt_fu_freedb_get_found_cds(state) < SPLT_MAXCD));

  return splt_fu_freedb_get_found_cds(state);
}

//char *login (char *s)
//{
//      char *pass, junk[130];
//      fprintf (stdout, "Username: ");
//      fgets(junk, 128, stdin);
//      junk[strlen(junk)-1]='\0';
//      pass = getpass("Password: ");
//      sprintf (s, "%s:%s", junk, pass);
//      memset (pass, 0x00, strlen(pass));
//      free(pass);
//      return s;
//}

static splt_addr splt_freedb_useproxy(splt_proxy *proxy, splt_addr dest,
    const char search_server[256], int port)
{
  dest.proxy=0;
  memset(dest.hostname, 0, 256);
  //memset(line, 0, 270);

  //if (proxy->use_proxy)
  if (proxy)
  {
    /*
    //TODO
    strncpy(dest.hostname, proxy->hostname, 255);
    dest.port = proxy->port;
    dest.proxy = proxy->use_proxy;

    fprintf(stderr, "Using Proxy: %s on Port %d\n", dest.hostname, dest.port);

    dest.auth = malloc(strlen(line)+1);
    if (dest.auth==NULL)
    {
      perror("malloc");
      exit(1);
    }
    memset(dest.auth, 0x0, strlen(line)+1);
    strncpy(dest.auth, line, strlen(line));
    //dest.auth = b64(line, strlen(line));*/
  }

  if (!dest.proxy) 
  {
    //we put the hostname
    if (strlen(search_server) == 0)
    {
      //by default we use freedb2.org
      strncpy(dest.hostname, SPLT_FREEDB2_SITE, 255);
    }
    else
    {
      strncpy(dest.hostname, search_server, 255);
    }

    //we put the port
    if (port == -1)
    {
      //by default we put the port 80
      //to use it with cddb.cgi
      dest.port = SPLT_FREEDB_CDDB_CGI_PORT;
    }
    else
    {
      dest.port = port;
    }      
  }

  return dest;
}

/*! search the freedb according to "search"

\param state The central structure this library keeps all its data in
\param search_string The string that is to be searched for
\param error The error code this action returns in
\param search_type the type of the search. Can be ert to
SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI
\param search_server The URL of the search server or NULL to select
the default which currently means freedb2.org
\param port The port on the server. -1 means default (Which should be
80). 
*/
int splt_freedb_process_search(splt_state *state, char *search,
                               int search_type, const char search_server[256],
                               int port)
{
  //we take the cgi path of the search_server
  //if we have one
  char cgi_path[256] = { '\0' };
  if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
  {
    char *temp = strchr(search_server,'/');
    if (temp != NULL)
    {
      snprintf(cgi_path,255,"%s",temp);
      *temp = '\0';
    }
  }
  //default cgi path
  if (strlen(search_server) == 0)
  {
    snprintf(cgi_path,255,"%s","/~cddb/cddb.cgi");
  }

  //possible error that we will return
  int error = SPLT_FREEDB_OK;
  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h = NULL;
  splt_addr dest;
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c = NULL, *e=NULL;
  int i = 0, tot=0;
  //the message delivered to the server
  char *message = NULL;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE] = { '\0' };

  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
  if (winsockinit != 0)
  {
    splt_e_clean_strerror_msg(state);
    error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
    return error;
  }
#else
  int fd;
#endif

  //transform ' ' to '+'
  int string_length = strlen(search);
  for (i = 0; i < string_length; i++)
  {
    if (search[i] == ' ')
    {
      search[i] = '+';
    }
  }

  //dest = splt_freedb_useproxy(&state->proxy, dest, search_server, port);
  dest = splt_freedb_useproxy(NULL, dest, search_server, port);

  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
  {
    splt_e_set_strherror_msg(state);
    error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
    splt_e_set_error_data(state,dest.hostname);
#ifdef __WIN32__
    WSACleanup();
#endif
    return error;
  }
  else
  {
    splt_e_set_error_data(state,dest.hostname);

    //we prepare socket
    memset(&host, 0x0, sizeof(host));
    host.sin_family=AF_INET;
    host.sin_addr.s_addr=((struct in_addr *) (h->h_addr)) ->s_addr;
    host.sin_port=htons(dest.port);

    //initialize socket
    if((fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
      splt_e_set_strerror_msg(state);
      error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
#ifdef __WIN32__
      WSACleanup();
#endif
      return error;
    }
    else
    {
      //make connection
      if ((connect(fd, (void *)&host, sizeof(host))) < 0)
      {
        splt_e_set_strerror_msg(state);
        error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
        closesocket(fd);
#ifdef __WIN32__
        WSACleanup();
#endif
        return error;
      }
      else
      {
        //prepare message to send
        //proxy not supported for now
        //if (dest.proxy) {
        //                sprintf(message,
        //                "GET http://www.freedb.org"SPLT_SEARCH" "PROXYDLG, search);
        //                if (dest.auth!=NULL)
        //                sprintf (message, "%s"AUTH"%s\n", message, dest.auth);
        //                strncat(message, "\n", 1);
        //                }
        //                else 
        int malloc_number = 0;
        //freedb2 search
        if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
        {
          malloc_number = strlen(search)+
            strlen(SPLT_FREEDB2_SEARCH)+strlen(cgi_path)+3;

          //we allocate the memory for the query string
          if ((message = malloc(malloc_number)) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            //we write the search query
            snprintf(message, malloc_number,
                SPLT_FREEDB2_SEARCH,cgi_path,search);

            //fprintf(stdout,"message = _%s_\n",message);
            //fflush(stdout);

            //message sent
            if((send(fd, message, strlen(message), 0))==-1)
            {
              splt_e_set_strerror_msg(state);
              error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
            }
            else
            {
              memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);

              splt_fu_freedb_free_search(state);

              int init_err = splt_fu_freedb_init_search(state);
              if (init_err == SPLT_OK)
              {
                //we read what we receive from the server
                do {
                  tot=0;
                  c = buffer;

                  do {
                    i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                    if (i == -1) 
                    {
                      splt_e_set_strerror_msg(state);
                      error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                      goto function_end1;
                    }

                    tot += i;
                    buffer[tot]='\0';
                    c += i;
                  } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)
                      &&((e=strstr(buffer, "\n."))==NULL));

                  //fprintf(stdout,"buffer = %s\n",buffer);
                  //fflush(stdout);

                  //we analyse the buffer
                  tot = splt_freedb2_analyse_cd_buffer(buffer, tot, state,&error);
                  if (error < 0)
                  {
                    goto function_end1;
                  }

                  if (tot == -1) continue;
                  if (tot == -2) break;

                } while ((i>0)&&(e==NULL)&&
                    (splt_fu_freedb_get_found_cds(state)<SPLT_MAXCD));

                //no cd found
                if (splt_fu_freedb_get_found_cds(state)==0) 
                {
                  error = SPLT_FREEDB_NO_CD_FOUND;
                  goto function_end1;
                }
                //erroror occured while getting freedb infos
                if (splt_fu_freedb_get_found_cds(state)==-1) 
                {
                  error = SPLT_FREEDB_ERROR_GETTING_INFOS;
                  goto function_end1;
                }
                //max cd number reached
                if (splt_fu_freedb_get_found_cds(state)==SPLT_MAXCD) 
                {
                  error = SPLT_FREEDB_MAX_CD_REACHED;
                  goto function_end1;
                }
              }
              else
              {
                error = init_err;
                goto function_end1;
              }
            }

function_end1:
            //free memory
            free(message);
            message = NULL;
#ifdef __WIN32__
            WSACleanup();
#endif
          }
        }
        //we will put the new web html freedb search
        /* TODO when freedb.org releases the web search */
        else 
        {
          error = SPLT_FREEDB_ERROR_GETTING_INFOS;
#ifdef __WIN32__
          WSACleanup();
#endif
          closesocket(fd);
          return error;
          /*if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB)
            {
            }*/
        }
      }
      closesocket(fd);
    }
  }

  return error;
}

/*! must only be called after process_freedb_search

returns the cddb file content corresponding to the last search, for
the disc_id i (parameter of the function)

\param state The central structure that keeps all data this library
uses 
\param error Is set to the error code this action results in
\param disc_id The freedb disc ID.
\param cddb_get_type specifies the type of the get:
  it can be SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI (that works for both
  freedb and freedb2 at the moment - 18_10_06)
  or SPLT_FREEDB_GET_FILE_TYPE_CDDB (that only work for freedb at
  the moment - 18_10_06)

\todo see when we don't have a valid port or get_type
*/
char *splt_freedb_get_file(splt_state *state, int i, int *error,
    int get_type, const char cddb_get_server[256], int port)
{
  //we take the cgi path of the search_server
  //if we have one
  char cgi_path[256] = { '\0' };
  if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
  {
    char *temp = strchr(cddb_get_server,'/');
    if (temp != NULL)
    {
      snprintf(cgi_path,256,"%s",temp);
      *temp = '\0';
    }
  }
  //default cgi path
  if (strlen(cddb_get_server) == 0)
  {
    snprintf(cgi_path,255,"%s","/~cddb/cddb.cgi");
  }

  //possible error that we will return
  *error = SPLT_FREEDB_FILE_OK;
  //the freedb file that we will return
  char *output = NULL;

  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h;
  splt_addr dest;
  char *message = NULL;
  int tot=0;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE] = { '\0' };
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c = NULL, *e=NULL;

  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
  if (winsockinit != 0)
  {
    splt_e_clean_strerror_msg(state);
    *error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
    return output;
  }
#else
  int fd = 0;
#endif

  //dest = splt_freedb_useproxy(&state->proxy, dest, cddb_get_server, port);
  dest = splt_freedb_useproxy(NULL, dest, cddb_get_server, port);

  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
  {
    splt_e_set_strherror_msg(state);
    *error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
    splt_e_set_error_data(state,dest.hostname);
#ifdef __WIN32__
    WSACleanup();
#endif
    return NULL;
  }
  else
  {
    splt_e_set_error_data(state,dest.hostname);

    //we prepare socket
    memset(&host, 0x0, sizeof(host));
    host.sin_family=AF_INET;
    host.sin_addr.s_addr=((struct in_addr *) (h->h_addr)) ->s_addr;
    host.sin_port=htons(dest.port);

    //prepare message to send
    //proxy not yet supported
    //      if (dest.proxy) {
    //        sprintf(message, "GET "FREEDBHTTP"cmd=cddb+read+%s+%s&hello=nouser+mp3splt.net+"PACKAGE_NAME"+"PACKAGE_VERSION"&proto=5 "PROXYDLG, 
    //        cdstate->discs[i].category, cdstate->discs[i].discid);
    //        if (dest.auth!=NULL) {
    //        sprintf (message, "%s"AUTH"%s\n", message, dest.auth);
    //        memset(dest.auth, 0x00, strlen(dest.auth));
    //        free(dest.auth);
    //        }
    //        strncat(message, "\n", 1);
    //        }
    //        else 
    const char *cd_category = splt_fu_freedb_get_disc_category(state, i);
    const char *cd_id = splt_fu_freedb_get_disc_id(state, i);

    int malloc_number = 0;
    if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB)
    {
      malloc_number = strlen(cd_category)+strlen(cd_id)+
        strlen(SPLT_FREEDB_GET_FILE);
    }
    else
    {
      //if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
      malloc_number = strlen(cd_category) + strlen(cd_id) +
        strlen(SPLT_FREEDB_CDDB_CGI_GET_FILE) + strlen(cgi_path);
    }
    message = malloc(malloc_number);
    if (message != NULL)
    {
      //CDDB protocol (usually port 8880)
      if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB)
      {
        snprintf(message, malloc_number, SPLT_FREEDB_GET_FILE, cd_category, cd_id);

        //open socket
        if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
          splt_e_set_strerror_msg(state);
          *error = SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET;
          free(message);
          goto end_function;
        }
        else
        {
          //connect to host
          if ((connect(fd, (void *)&host, sizeof(host)))==-1)
          {
            splt_e_set_strerror_msg(state);
            *error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
            goto bloc_end;
          }
          else
          {
            //possible errors + proxy
            if (!dest.proxy) 
            {
              i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);
              if (i == -1)
              {
                splt_e_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                goto bloc_end;
              }
              buffer[i]='\0';

              if (strncmp(buffer,"201",3)!=0)  
              {
                *error = SPLT_FREEDB_ERROR_SITE_201;
                goto bloc_end;
              }

              //send hello message
              if((send(fd, SPLT_FREEDB_HELLO, strlen(SPLT_FREEDB_HELLO), 0))==-1)
              {
                splt_e_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                goto bloc_end;
              }
              i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);

              if (i == -1)
              {
                splt_e_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                goto bloc_end;
              }
              buffer[i]='\0';

              if (strncmp(buffer,"200",3)!=0)  
              {
                *error = SPLT_FREEDB_ERROR_SITE_200;
                goto bloc_end;
              }
            }

            //we send the message
            if((send(fd, message, strlen(message), 0))==-1)
            {
              splt_e_set_strerror_msg(state);
              *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
              goto bloc_end;
            }
            else
            {
              memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);
              c = buffer;
              tot=0;

              //we read 
              do {
                i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                if (i == -1)
                {
                  splt_e_set_strerror_msg(state);
                  *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                  goto bloc_end;
                }

                //if errors
                if (tot == 0)
                {
                  if ((strncmp(buffer,"50",2) == 0)
                      || (strncmp(buffer,"40",2) == 0))
                  {
                    //if "No such CD entry in database"
                    if (strncmp(buffer,"401",3) == 0)
                    {
                      *error = SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE;
                    }
                    else
                    {
                      *error = SPLT_FREEDB_ERROR_SITE;
                    }
                    goto bloc_end;
                  }
                }

                tot += i;
                buffer[tot]='\0';
                c += i;
              } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)&&
                  ((e=strstr(buffer, "\n."))==NULL));

              //we quit
              if (!dest.proxy)
              {
                if((send(fd, "quit\n", 5, 0))==-1)
                {
                  splt_e_set_strerror_msg(state);
                  *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                  goto bloc_end;
                }
              }
            }
          }

bloc_end:
          free(message);
          message = NULL;
          closesocket(fd);
        }

        //if we don't have an error
        if (*error >= 0)
        {
          if (tot==0) 
          {
            *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
            goto end_function;
          }

          if (e!=NULL)
          {
            buffer[e-buffer+1]='\0';
          }

          //if invalid server answer
          if ((strstr(buffer, "database entry follows"))==NULL)
          {
            if ((c = strchr(buffer, '\n'))!=NULL)
            {
              buffer[c-buffer]='\0';
            }
            *error = SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER;
            goto end_function;
          }
          else
          {
            if ((c = strchr(buffer, '#'))==NULL)
            {
              output = NULL;
              *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
              goto end_function;
            }
            else
            {
              output = malloc(strlen(c)+1);
              if (output != NULL)
              {
                sprintf(output,c);
#ifdef __WIN32__
                WSACleanup();
#endif
                return output;
              }
              else
              {
                *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                goto end_function;
              }
            }
          }
        }
        else
        {
          goto end_function;
        }
      }
      //cddb.cgi script (usually port 80)
      else
      {
        if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
        {
          snprintf(message, malloc_number, SPLT_FREEDB_CDDB_CGI_GET_FILE, 
              cgi_path, cd_category, cd_id);

          //open socket
          if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
          {
            splt_e_set_strerror_msg(state);
            *error = SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET;
            free(message);
            goto end_function;
          }
          else
          {
            //connect to host
            if ((connect(fd, (void *)&host, sizeof(host)))==-1)
            {
              splt_e_set_strerror_msg(state);
              *error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
              goto bloc_end2;
            }
            else
            {
              //we send the message
              if((send(fd, message, strlen(message), 0))==-1)
              {
                splt_e_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                goto bloc_end2;
              }
              else
              {
                memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);
                c = buffer;
                tot=0;

                //we read 
                //we read what we receive from the server
                do {
                  tot=0;
                  c = buffer;

                  do {
                    i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                    if (i == -1) 
                    {
                      splt_e_set_strerror_msg(state);
                      *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                      goto bloc_end2;
                    }

                    //if errors
                    if (tot == 0)
                    {
                      if ((strncmp(buffer,"50",2) == 0)
                          || (strncmp(buffer,"40",2) == 0))
                      {
                        //if "No such CD entry in database"
                        if (strncmp(buffer,"401",3) == 0)
                        {
                          *error = SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE;
                        }
                        else
                        {
                          *error = SPLT_FREEDB_ERROR_SITE;
                        }
                        goto bloc_end2;
                      }
                    }

                    tot += i;
                    buffer[tot]='\0';
                    c += i;
                  } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)
                      &&((e=strstr(buffer, "\n."))==NULL));

                  if (error < 0)
                  {
                    goto bloc_end2;
                  }

                } while ((i>0)&&(e==NULL));
              }
            }

bloc_end2:
            free(message);
            message = NULL;
            closesocket(fd);

            //if we don't have an error
            if (*error >= 0)
            {
              if (tot==0) 
              {
                *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
                goto end_function;
              }

              if ((c = strchr (buffer, '#'))==NULL)
              {
                output = NULL;
                *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
                goto end_function;
              }
              else
              {
                output = malloc(strlen(c)+1);
                if (output != NULL)
                {
                  //we write the output
                  sprintf(output,c);
#ifdef __WIN32__
                  WSACleanup();
#endif
                  return output;
                }
                else
                {
                  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                  goto end_function;
                }
              }
            }
            else
            {
              goto end_function;
            }
          }
        }
        else
        {
          //invalid get file type
          *error = SPLT_FREEDB_ERROR_GETTING_INFOS;
          goto end_function;
        }
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end_function;
    }

end_function:
    ;
#ifdef __WIN32__
    WSACleanup();
#endif
    return NULL;
  }
}

  //deprecated, and not in use
  //but may useful for the implementation of the proxy
  /*int search_freedb (splt_state *state)
    {
    char *c, *e=NULL;
    FILE *output = NULL;
    struct sockaddr_in host;
    struct hostent *h;
    struct splt_addr dest;

    if ((c=getenv("HOME"))!=NULL) sprintf(message, "%s/"PROXYCONFIG, c);
    else strncpy(message, PROXYCONFIG, strlen(PROXYCONFIG));

    if (!(output=splt_io_fopen(message, "r"))) {
    if (!(output=splt_io_fopen(message, "w+"))) {
    fprintf(stderr, "\nWARNING Can't open config file ");
    perror(message);
    }
    else {
    fprintf (stderr, "Will you use a proxy? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    fprintf (stderr, "Proxy Address: ");
    fgets(junk, 200, stdin);
    fprintf (output, "PROXYADDR=%s", junk);
    fprintf (stderr, "Proxy Port: ");
    fgets(junk, 200, stdin);
    fprintf (output, "PROXYPORT=%s", junk);
    fprintf (stderr, "Need authentication? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    fprintf (output, "PROXYAUTH=1\n");
    fprintf (stderr, "Would you like to save password (insecure)? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    login (message);
    e = b64(message, strlen(message));
    fprintf (output, "%s\n", e);
    memset(message, 0x00, strlen(message));
    memset(e, 0x00, strlen(e));
    free(e);
    }
    }
    }
    }
    }
        
    if (splt_fu_freedb_get_found_cds(state)<=0) {
    if (dest.proxy) {
    if (strstr(buffer, "HTTP/1.0")!=NULL) {
    if ((c = strchr (buffer, '\n'))!=NULL)
    buffer[c-buffer]='\0';
    fprintf (stderr, "Proxy Reply: %s\n", buffer);
    }
    }
    }
    return 0;
    }*/
