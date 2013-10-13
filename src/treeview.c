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
#include "filelist.h"
#include "imgview.h"
#include "treeview.h"

/* Tree View */
static void
tree_view_item_activated (GtkTreeView       *tree_view,
			  GtkTreePath       *path,
			  GtkTreeViewColumn *column,
			  gpointer           user_data)
{
	GtkTreeModel *tree_model;
	GtkTreeIter iter;
	gchar      *item_path;
	gboolean   is_dir;
	gchar      *dir_name;
	tree_view_data_t *mydata;

	tree_model = gtk_tree_view_get_model (tree_view);
	gtk_tree_model_get_iter (tree_model, &iter, path);
	gtk_tree_model_get (tree_model, &iter,
			    IS_DIR_COLUMN, &is_dir,
			    PATH_COLUMN, &item_path,
			    -1);
	dir_name = g_path_get_dirname  (item_path);
	mydata = user_data;
	if(user_data && mydata) {
		// Update the model with file or subfolder
		gchar* new_path;
		if(is_dir) {
			new_path = item_path;
		}
		else {
			new_path = dir_name;
		}
		compose_imgfile_list(GTK_LIST_STORE(tree_model),
				     new_path, mydata->folder_pixbuf, 
				     mydata->img_pixbuf, item_path);
		if(!is_dir && mydata->img_widget) {
			set_image_from_file(mydata->img_widget, item_path);
		}
	}
	g_free (dir_name);
	g_free (item_path);
	// g_free (mydata);
}

GtkWidget* create_view_with_model(GtkListStore *model,
				  GdkPixbuf *folder_pixbuf,
				  GdkPixbuf *img_pixbuf,
				  GtkWidget *image_widget)
{
	GtkCellRenderer     *renderer;
	GtkTreeModel        *tree_model;
	GtkWidget           *view;
	tree_view_data_t    *mydata;

	if(!model || !folder_pixbuf || !img_pixbuf) {
		return NULL;
	}

	tree_model = GTK_TREE_MODEL(model);

	view = gtk_tree_view_new ();

	mydata = (tree_view_data_t*)g_malloc(sizeof(tree_view_data_t));
	mydata->folder_pixbuf = folder_pixbuf;
	mydata->img_pixbuf = img_pixbuf;
	mydata->img_widget = image_widget;

	/* ICON */	
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "Icon",
	                                             renderer,
	                                             "pixbuf", ICON_COLUMN,
	                                             NULL);
	/* PATH */	
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Path",  
	                                             renderer,
	                                             "text", FILENAME_COLUMN,
	                                             NULL);
	/* SIZE */
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Size",
	                                             renderer,
	                                             "text", STR_SIZE_COLUMN,
	                                             NULL);
	/* DATE */	
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Last Modified",
	                                             renderer,
	                                             "text", STR_DATE_COLUMN,
	                                             NULL);
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), tree_model);

	g_signal_connect (view, "row-activated",
			  G_CALLBACK (tree_view_item_activated),
			  mydata);
	
	/* The tree view has acquired its own reference to the
	 *  model, so we can drop ours. That way the model will
	 *  be freed automatically when the tree view is destroyed */
	g_object_unref (model);
	
	return view;
}
