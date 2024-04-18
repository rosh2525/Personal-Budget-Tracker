#include "gui.h"
#include <ctype.h>
#include <time.h>

// Global variables
GtkWidget *expense_entry;
GtkWidget *category_combo;
GtkWidget *expense_scrolled_window;
GtkWidget *delete_button;
GtkListStore *store;
extern GtkWidget *expense_entry;
// Array of predefined expense categories
const gchar *categories[] = {"Groceries", "Transportation", "Entertainment", "Utilities"};

// Function to create the main window
GtkWidget *create_main_window() {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *label;
    GtkWidget *add_button;
    GtkWidget *clear_button; // New button for clearing all expenses
    GtkWidget *tree_view;

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Personal Expense Tracker");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a main vertical box
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Create an expense entry field
    expense_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(expense_entry), "Enter expense amount");
    gtk_box_pack_start(GTK_BOX(main_box), expense_entry, FALSE, FALSE, 0);

    // Connect signal handler for checking if the entered text is numeric
    g_signal_connect(expense_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Create a combo box for expense categories
    category_combo = gtk_combo_box_text_new();
    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), categories[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_box_pack_start(GTK_BOX(main_box), category_combo, FALSE, FALSE, 0);

    // Create a row to hold buttons
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), row, FALSE, FALSE, 0);

    // Create a delete button
    delete_button = gtk_button_new_with_label("Delete");
    gtk_box_pack_start(GTK_BOX(row), delete_button, FALSE, FALSE, 0);

    // Create an "Add Expense" button
    add_button = gtk_button_new_with_label("Add Expense");
    gtk_box_pack_start(GTK_BOX(row), add_button, FALSE, FALSE, 0);

    // Create a "Clear All" button
    clear_button = gtk_button_new_with_label("Clear All");
    gtk_box_pack_start(GTK_BOX(row), clear_button, FALSE, FALSE, 0);

    // Create a label for the expense list
    label = gtk_label_new("Expense List:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);

    // Create a scrolled window for the tree view
    expense_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(expense_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), expense_scrolled_window, TRUE, TRUE, 0);

    // Create the list store
    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Create the tree view
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(expense_scrolled_window), tree_view);

    // Add columns to the tree view
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Price", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Reason", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Connect signal handlers
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), tree_view);
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_expense_clicked), NULL);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_all_clicked), tree_view); // Connect Clear All button

    // Show all widgets
    gtk_widget_show_all(window);

    return window;
}

// Callback function for adding an expense
void on_add_expense_clicked(GtkButton *button, gpointer data) {
    const gchar *expense_text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    const gchar *category_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(category_combo));

    if (g_strcmp0(expense_text, "") != 0 && category_text != NULL) {
        // Get current time
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        gchar time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Append the new expense to the store
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, expense_text, 1, time_str, 2, category_text, -1);

        // Clear the entry field
        gtk_entry_set_text(GTK_ENTRY(expense_entry), "");

        // Free memory allocated for category_text
        g_free((gchar *) category_text);
    }
}

// Callback function for checking if the entered text is numeric
void on_entry_changed(GtkEditable *editable, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    for (int i = 0; text[i] != '\0'; i++) {
        if (!isdigit(text[i])) {
            // Display popup message
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter only numbers.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            // Clear the entry field
            gtk_entry_set_text(GTK_ENTRY(expense_entry), "");
            break;
        }
    }
}

void on_delete_clicked(GtkButton *button, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void on_clear_all_clicked(GtkButton *button, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
    gtk_list_store_clear(GTK_LIST_STORE(model));
}
