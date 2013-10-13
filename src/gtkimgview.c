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
#include <string.h>
#include "model.h"
#include "filelist.h"
#include "treeview.h"
#include "imgview.h"

typedef struct 
{
	GtkListStore *model;
	GdkPixbuf    *folder_pixbuf;
	GdkPixbuf    *img_pixbuf;
} button_data_t;

static gboolean 
on_delete_event (GtkWidget *widget, GdkEvent*event, gpointer user_data)
{
	gtk_main_quit();
	return GDK_EVENT_PROPAGATE;
}

static void
up_button_clicked (GtkButton    *button,
		   gpointer userdata)
{
	button_data_t* bdata = (button_data_t*) userdata;
	gboolean valid;
	gboolean is_dir;
	GtkTreeIter iter;
	GtkTreeModel *tree_model;
        gchar *path;
	gchar *absolute_path;
        gchar *parent_path;

	if(!userdata || !bdata || !bdata->model) {
		return;
	}

	tree_model = GTK_TREE_MODEL (bdata->model);

	// Get, from the model, the path now, and obtain parent's one
	for (valid = gtk_tree_model_get_iter_first (tree_model, &iter);
	     valid; valid = gtk_tree_model_iter_next (tree_model, &iter))
	{
		gtk_tree_model_get (tree_model, &iter,
				    PATH_COLUMN, &path,
				    IS_DIR_COLUMN, &is_dir,
				    -1);
		absolute_path = g_path_get_dirname (path);
		g_free(path);
		break;
	}

	// Obtain parent path
	if(!strcmp(absolute_path, ".")) {
		parent_path = g_strdup("..");
	}
	else if(g_strstr_len(absolute_path, strlen(absolute_path), "..")) {
		parent_path = g_strconcat (absolute_path, "/..", NULL);
	}
	else {
		parent_path = g_path_get_dirname (absolute_path);
	}
	g_free (absolute_path);

	// Compose the model, again, with parent directory
	// selection set to NULL, for first image on parent path
	// to be selected
	compose_imgfile_list(bdata->model, parent_path,
			     bdata->folder_pixbuf, bdata->img_pixbuf,
			     NULL);
	g_free (parent_path);
}

gint main (gint argc, gchar **argv)
{
	// Parent window
	GtkWidget *window;

	// Subwindow to add rest of the childrens
	GtkWidget *subwindow;

	// The model to compose all the views
	GtkListStore *model;
	
	// The tree view
	GtkWidget *tree_view;
	
	// The path 
	gchar *now_path;

	// Icons
	GdkPixbuf *folder_pixbuf, *img_pixbuf;
	GtkIconTheme *icon_theme;

	// Image widget
	GtkWidget *image;

	// Box for packaging
	GtkWidget    *vbox, *topbar;

	// Up button
	GtkWidget    *up_button;

	// Initialize GTK
	gtk_init(&argc, &argv);

	// Initialize Icons
	icon_theme = gtk_icon_theme_get_default ();
	img_pixbuf = gtk_icon_theme_load_icon (icon_theme,
					       GTK_STOCK_FILE,
					       24, 0, NULL);
	folder_pixbuf = gtk_icon_theme_load_icon (icon_theme,
						  GTK_STOCK_DIRECTORY,
						  24, 0, NULL);

	// Specify the path
	if(argv[1])
		now_path = argv[1];
	else
		now_path = ".";

	// Create the model
	/* Create and fill the model */
	model = gtk_list_store_new (N_COLUMNS,
				    G_TYPE_STRING,
				    G_TYPE_STRING,
				    GDK_TYPE_PIXBUF,
				    G_TYPE_BOOLEAN,
				    G_TYPE_BOOLEAN,
				    G_TYPE_STRING,
				    G_TYPE_STRING);
	
	// Compose the model with an image list
	compose_imgfile_list(model, now_path, folder_pixbuf, img_pixbuf, NULL);

	// Two type of windows:
	// - Top level
	// - Pop up
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "A very simple image viewer");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	// Initialize the boxes, 
	// 0 px space for horizontal and vertical children
	topbar = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	vbox   = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	// Up Button
	up_button = gtk_button_new_from_stock (GTK_STOCK_GO_UP);
	button_data_t bdata;
	bdata.model = model;
	bdata.folder_pixbuf = folder_pixbuf;
	bdata.img_pixbuf = img_pixbuf;

	g_signal_connect (up_button, "clicked",
			  G_CALLBACK (up_button_clicked),
			  (gpointer)&bdata);

	gtk_box_pack_start (GTK_BOX (topbar), up_button, FALSE, FALSE, 0);
	gtk_widget_show (up_button);

	gtk_box_pack_start (GTK_BOX (vbox), topbar, FALSE, TRUE, 0);
	gtk_widget_show (topbar);

	// The Image View
	image = get_image_from_model (model);

	// A Tree view model to show now path dirs and image files
	tree_view = create_view_with_model (model, folder_pixbuf, img_pixbuf, image);
	
	subwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (subwindow),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (subwindow),
					     GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (subwindow), tree_view);
	gtk_widget_show (tree_view);

	gtk_box_pack_start (GTK_BOX (vbox), subwindow, TRUE, TRUE, 0);
	gtk_widget_show (subwindow);

	gtk_box_pack_start (GTK_BOX (vbox), image, TRUE, TRUE, 0);
	gtk_widget_show (image);
	
	// Set the connect
	g_signal_connect (window, "delete-event",
			  G_CALLBACK (on_delete_event), NULL);
	
	// Show all

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show (vbox);
	gtk_widget_show (window);
	
	// Initialize event loop
	gtk_main ();
	return 0;
}
