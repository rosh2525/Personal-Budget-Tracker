#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

// Function to create the main window
GtkWidget *create_main_window();

// Callback function for adding an expense
void on_add_expense_clicked(GtkButton *button, gpointer data);

// Callback function for deleting an expense
void on_delete_clicked(GtkButton *button, gpointer data);

// Callback function for clearing all expenses
void on_clear_all_clicked(GtkButton *button, gpointer data);

// Callback function to check if the entered text is numeric
void on_entry_changed(GtkEditable *editable, gpointer user_data);

#endif // GUI_H
