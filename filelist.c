/** 
    Copyright (c) 2013, Sergio Arroutbi Braojos
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    
      Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.
    
      Redistributions in binary form must reproduce the above copyright notice, this
      list of conditions and the following disclaimer in the documentation and/or
      other materials provided with the distribution.
    
      Neither the name of the {organization} nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>
#include "model.h"
#include "filelist.h"

// This method returns a GtkListStore* model 
// with all the img files considered to be files,
// as in IMG_REGEXP_EXPRESSION, defined in filelist.h
void compose_imgfile_list(GtkListStore *model, gchar *dir_path,
			  GdkPixbuf *folder_pixbuf, GdkPixbuf *img_pixbuf,
			  gchar *file_selected)
{
	GDir        *dir;
	const gchar *name;
	GtkTreeIter  iter;
	gboolean     img_selected;

	gtk_list_store_clear (model);
	dir = g_dir_open (dir_path, 0, NULL);

	if (!dir_path || !folder_pixbuf || !img_pixbuf) {
		return;
	}

	g_printf("Updating model, path:%s, selected_file:%s\n",
		 dir_path, file_selected ? file_selected : "NONE");

	if(file_selected) {
		img_selected = TRUE;
	}
	else {
		img_selected = FALSE;
	}
	name = g_dir_read_name (dir);
	while (name) {
		gchar*   path, *display_name;
		gboolean is_file = FALSE;
		gboolean is_img_file = FALSE;
		gboolean is_dir = FALSE;
		gboolean is_selected = FALSE;
		GRegex*  img_regex = NULL;
		GError*  regex_error = NULL;
		GMatchInfo* match_info;

		// Create a regex.
		img_regex = g_regex_new(IMG_REGEXP_EXPRESSION, G_REGEX_OPTIMIZE,
					0, &regex_error);

		/* We ignore hidden files that start with '.' */
		if (name[0] == '.') {
			name = g_dir_read_name (dir);
			continue;
		}

		path = g_build_filename (dir_path, name, NULL);

		is_file = g_file_test (path, G_FILE_TEST_IS_REGULAR);

		//Search the regex;
		if(TRUE == g_regex_match_full(img_regex, path, -1, 0, 0,
					      &match_info, &regex_error) && is_file)
		{
			is_img_file = TRUE;
		}

		is_dir = g_file_test (path, G_FILE_TEST_IS_DIR);

		// Only add to the model, those files that are images
		// or directories
		if(is_dir || is_img_file) {
			// If no image selected, first one to be selected
			// else, set selected to the specified on parameter
			if(!img_selected) {
				is_selected  = TRUE;
				img_selected = TRUE;
			}
			else if(file_selected && (!strcmp(path, file_selected))) {
				is_selected  = TRUE;
				img_selected = TRUE;
			}

			display_name = g_filename_to_utf8 (name, -1, NULL, NULL, NULL);

			gtk_list_store_append (model, &iter);
			gtk_list_store_set (model, &iter,
					    PATH_COLUMN, path,
					    FILENAME_COLUMN, display_name,
					    ICON_COLUMN, is_dir ? folder_pixbuf : img_pixbuf,
					    IS_DIR_COLUMN, is_dir,
    					    IS_SELECTED_COLUMN, is_selected,
					    -1);
			g_free (display_name);
		}
		g_free (path);
		name = g_dir_read_name (dir);
	}

	g_dir_close (dir);
}
