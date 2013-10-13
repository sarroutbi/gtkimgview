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
#include <glib/gprintf.h>
#include "model.h"

/* Get the GtkWidget as Image Widget with the model */
GtkWidget* get_image_from_model (GtkListStore *model)
{
	gboolean valid;
	GtkTreeModel *list_store;
	GtkTreeIter iter;
	gint row_count = 0;
        gchar *path;
        gchar *file;
	GtkWidget* image_widget;

	list_store = GTK_TREE_MODEL (model);

	for (valid = gtk_tree_model_get_iter_first (list_store, &iter);
	     valid; valid = gtk_tree_model_iter_next (list_store, &iter))
	{
		gboolean is_selected;
	
		/* Make sure you terminate calls to gtk_tree_model_get()
		 * with a '-1' value
		 */
		gtk_tree_model_get (list_store, &iter,
				    PATH_COLUMN, &path,
				    FILENAME_COLUMN, &file,
				    IS_SELECTED_COLUMN, &is_selected,
				    -1);
		
		/* Break when selected image has been found */
		if(is_selected) {
			break;
		}
		row_count++;
	}
	g_printf("CREATING IMAGE WITH PATH:%s\n", path);
	image_widget = gtk_image_new_from_file(path);
	g_free (path);
	return image_widget;
}
