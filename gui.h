#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

GtkWidget *create_main_window();

void on_add_expense_clicked(GtkButton *button, gpointer data);

void on_delete_clicked(GtkButton *button, gpointer data);

void on_clear_all_clicked(GtkButton *button, gpointer data);

void on_entry_changed(GtkEditable *editable, gpointer user_data);

#endif
