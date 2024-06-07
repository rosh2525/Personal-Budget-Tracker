#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>

enum {
    COLUMN_PRICE,
    COLUMN_TIME,
    COLUMN_REASON,
    NUM_COLUMNS
};

GtkWidget *create_main_window();
void on_add_expense_clicked(GtkButton *button, gpointer data);
void on_entry_changed(GtkEditable *editable, gpointer user_data);
void on_delete_clicked(GtkButton *button, gpointer data);
void on_clear_all_clicked(GtkButton *button, gpointer data);
void on_sort_by_category_clicked(GtkButton *button, gpointer data);
void on_category_changed(GtkComboBox *combo_box, gpointer user_data);
void on_stats_clicked(GtkButton *button, gpointer data);
void on_add_income_clicked(GtkButton *button, gpointer data);
void save_current_balance();
void load_current_balance();
void on_add_income_clicked(GtkButton *button, gpointer data);
void load_expenses_from_csv();
void on_reset_balance_clicked(GtkButton *button, gpointer data);
void save_expenses_to_csv();
void update_window_title(gpointer user_data);
void on_set_budget_limit_clicked(GtkButton *button, gpointer data);
gint get_category_index(const gchar *category);
gdouble current_expense_in_category(gint category_index);
void load_budget_limits();
void save_budget_limits();
void on_add_category_clicked(GtkButton *button, gpointer data);

#endif
