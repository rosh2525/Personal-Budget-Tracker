#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <ctype.h>
#include <time.h>

#define CSV_FILE "expenses.csv"

enum {
    COLUMN_PRICE,
    COLUMN_TIME,
    COLUMN_REASON,
    NUM_COLUMNS
};

extern GtkWidget *expense_entry;
extern GtkWidget *category_combo;
extern GtkWidget *expense_scrolled_window;
extern GtkWidget *delete_button;
extern GtkWidget *sort_button;
extern GtkWidget *stats_button;
extern GtkListStore *store;
extern const gchar *categories[];

GtkWidget *create_main_window();
void on_add_expense_clicked(GtkButton *button, gpointer data);
void on_entry_changed(GtkEditable *editable, gpointer user_data);
void on_delete_clicked(GtkButton *button, gpointer data);
void on_clear_all_clicked(GtkButton *button, gpointer data);
void load_expenses_from_csv();
void save_expenses_to_csv();
void on_sort_by_category_clicked(GtkButton *button, gpointer data);
void on_category_changed(GtkComboBox *combo_box, gpointer user_data);
void on_stats_clicked(GtkButton *button, gpointer data);

#endif /* GUI_H */
