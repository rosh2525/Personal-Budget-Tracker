#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <ctype.h>
#include <time.h>

// Define CSV file name
#define CSV_FILE "expenses.csv"

// Define column indices
enum {
    COLUMN_PRICE,
    COLUMN_TIME,
    COLUMN_REASON,
    NUM_COLUMNS
};

// Function prototypes
GtkWidget *create_main_window();
void on_add_expense_clicked(GtkButton *button, gpointer data);
void on_entry_changed(GtkEditable *editable, gpointer user_data);
void on_delete_clicked(GtkButton *button, gpointer data);
void on_clear_all_clicked(GtkButton *button, gpointer data);
void load_expenses_from_csv();
void save_expenses_to_csv();
void on_category_changed(GtkComboBox *widget, gpointer user_data);
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);
void on_sort_by_category_clicked(GtkButton *button, gpointer data);

#endif /* GUI_H */
