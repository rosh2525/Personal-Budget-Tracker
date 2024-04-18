#include <gtk/gtk.h>
#include "gui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = create_main_window();

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
