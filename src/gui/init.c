#include <gtk/gtk.h>

static void init (GtkApplication *app, gpointer user_data) {
	/* Builder initialisation */
	GtkBuilder* builder;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "app.glade", NULL);

	/*Prepare Main UI*/
	window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	gtk_window_set_application (GTK_WINDOW(window), app);

	g_object_unref(builder); /*Destroys builder*/
	gtk_widget_show_all(window);
}