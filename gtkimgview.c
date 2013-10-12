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
#include <string.h>
#include "model.h"
#include "filelist.h"
#include "treeview.h"
#include "imgview.h"

static gboolean 
on_delete_event (GtkWidget *widget, GdkEvent*event, gpointer user_data)
{
	gtk_main_quit();
	return GDK_EVENT_PROPAGATE;
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
	GtkWidget    *vbox, *hbox;

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
				    G_TYPE_BOOLEAN);
	
	// Compose the model with an image list
	compose_imgfile_list(model, now_path, folder_pixbuf, img_pixbuf, NULL);

	// Two type of windows:
	// - Top level
	// - Pop up
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "A very simple image viewer");
	gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);

	// Initialize the boxes, 
	// 0 px space for horizontal children
	// 12 px space for horizontal children
	hbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);

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

	gtk_box_pack_start (GTK_BOX (hbox), subwindow, TRUE, TRUE, 0);
	gtk_widget_show (subwindow);

	gtk_box_pack_start (GTK_BOX (hbox), image, TRUE, TRUE, 0);
	gtk_widget_show (image);
	
	// Set the connect
	g_signal_connect (window, "delete-event",
			  G_CALLBACK (on_delete_event), NULL);
	
	// Show all
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show (hbox);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show (vbox);
	gtk_widget_show (window);
	
	// Initialize event loop
	gtk_main ();
	return 0;
}
