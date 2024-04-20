#include "gui.h"
#include <ctype.h>
#include <time.h>
#define CSV_FILE "expenses.csv"

GtkWidget *expense_entry;
GtkWidget *category_combo;
GtkWidget *expense_scrolled_window;
GtkWidget *delete_button;
GtkListStore *store;
GtkWidget *sort_button;
GtkWidget *stats_button;

const gchar *categories[] = {"Groceries", "Transportation", "Entertainment", "Utilities", "Others"};

GtkWidget *create_main_window() {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *label;
    GtkWidget *add_button;
    GtkWidget *clear_button;
    GtkWidget *tree_view;

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Personal Budget Tracker");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a main vertical box
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Create an expense entry field
    expense_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(expense_entry), "Enter expense amount");
    gtk_box_pack_start(GTK_BOX(main_box), expense_entry, FALSE, FALSE, 0);

    // Connect signal handler for entry field
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

    // Create buttons
    delete_button = gtk_button_new_with_label("Delete");
    add_button = gtk_button_new_with_label("Add Expense");
    clear_button = gtk_button_new_with_label("Clear All");
    sort_button = gtk_button_new_with_label("Sort by Category");

    // Pack buttons into the row
    gtk_box_pack_start(GTK_BOX(row), delete_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), add_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), clear_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), sort_button, FALSE, FALSE, 0);

    // Create a label for the expense list
    label = gtk_label_new("Expense List:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);

    // Create a scrolled window for the tree view
    expense_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(expense_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), expense_scrolled_window, TRUE, TRUE, 0);

    // Create the list store
    store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Create the tree view
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(expense_scrolled_window), tree_view);

    // Add columns to the tree view
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Price", renderer, "text", COLUMN_PRICE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", COLUMN_TIME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Reason", renderer, "text", COLUMN_REASON, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    stats_button = gtk_button_new_with_label("Stats");
    gtk_box_pack_start(GTK_BOX(row), stats_button, FALSE, FALSE, 0);

    // Connect signal handler for the "Stats" button
    g_signal_connect(stats_button, "clicked", G_CALLBACK(on_stats_clicked), NULL);


    // Load expenses from CSV file
    load_expenses_from_csv();

    // Connect signal handlers
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), tree_view);
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_expense_clicked), NULL);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_all_clicked), tree_view);
    g_signal_connect(sort_button, "clicked", G_CALLBACK(on_sort_by_category_clicked), tree_view);
    g_signal_connect(category_combo, "changed", G_CALLBACK(on_category_changed), NULL);

    // Show all widgets
    gtk_widget_show_all(window);

    return window;
}

void on_add_expense_clicked(GtkButton *button, gpointer data) {
    const gchar *expense_text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    const gchar *category_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(category_combo));

    if (g_strcmp0(expense_text, "") != 0 && category_text != NULL) {
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        gchar time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, expense_text, 1, time_str, 2, category_text, -1);

        gtk_entry_set_text(GTK_ENTRY(expense_entry), "");

        g_free((gchar *) category_text);
    }

    save_expenses_to_csv();
}

void on_entry_changed(GtkEditable *editable, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    for (int i = 0; text[i] != '\0'; i++) {
        if (!isdigit(text[i])) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter only numbers.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

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

    save_expenses_to_csv();
}

void on_clear_all_clicked(GtkButton *button, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
    gtk_list_store_clear(GTK_LIST_STORE(model));

    save_expenses_to_csv();
}

void load_expenses_from_csv() {
    FILE *file = fopen(CSV_FILE, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char *price, *time, *reason;
            price = strtok(line, ",");
            time = strtok(NULL, ",");
            reason = strtok(NULL, ",");
            if (price && time && reason) {
                // Trim leading and trailing whitespace from reason
                gchar *trimmed_reason = g_strstrip(g_strdup(reason));

                // Convert the reason to lowercase for comparison
                gchar *reason_lowercase = g_ascii_strdown(trimmed_reason, -1);

                gboolean found = FALSE;
                // Search for a matching category
                for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
                    // Convert category to lowercase for comparison
                    gchar *category_lowercase = g_ascii_strdown(categories[i], -1);
                    if (g_strcmp0(reason_lowercase, category_lowercase) == 0) {
                        found = TRUE;
                        // Break if a matching category is found
                        break;
                    }
                    g_free(category_lowercase);
                }

                if (!found) {
                    // Add the trimmed reason as a new category
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), trimmed_reason);
                    g_free(trimmed_reason);
                }

                // Free the memory allocated for the lowercase reason
                g_free(reason_lowercase);

                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter, COLUMN_PRICE, price, COLUMN_TIME, time, COLUMN_REASON, reason, -1);
            }
        }
        fclose(file);
    }
}


void save_expenses_to_csv() {
    FILE *file = fopen(CSV_FILE, "w");
    if (file) {
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
        while (valid) {
            gchar *price, *time, *reason;
            gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, COLUMN_PRICE, &price, COLUMN_TIME, &time, COLUMN_REASON, &reason, -1);
            fprintf(file, "%s,%s,%s\n", price, time, reason);
            g_free(price);
            g_free(time);
            g_free(reason);
            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
        fclose(file);
    }
}

void on_sort_by_category_clicked(GtkButton *button, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    gtk_tree_sortable_set_sort_column_id(sortable, COLUMN_REASON, GTK_SORT_ASCENDING);
}

void on_category_changed(GtkComboBox *combo_box, gpointer user_data) {
    gint active = gtk_combo_box_get_active(combo_box);

    // Check if "Others" is selected
    if (active == G_N_ELEMENTS(categories) - 1) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Add New Category", NULL, GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "Add", GTK_RESPONSE_ACCEPT, NULL);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter new category");
        gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *new_category = gtk_entry_get_text(GTK_ENTRY(entry));
            if (g_strcmp0(new_category, "") != 0) {
                // Check if the category already exists
                gboolean exists = FALSE;
                for (int i = 0; i < G_N_ELEMENTS(categories) - 1; ++i) {
                    if (g_strcmp0(new_category, categories[i]) == 0) {
                        exists = TRUE;
                        break;
                    }
                }
                if (!exists) {
                    // Add the new category
                    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(category_combo), G_N_ELEMENTS(categories) - 1, new_category);
                    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), G_N_ELEMENTS(categories) - 2);
                } else {
                    // Inform the user that the category already exists
                    GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Category already exists.");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                }
            }
        }
        gtk_widget_destroy(dialog);
    }
}

void on_stats_clicked(GtkButton *button, gpointer data) {
    // Create a dialog for displaying stats
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Expense Stats", NULL, GTK_DIALOG_MODAL, "Close", GTK_RESPONSE_CLOSE, NULL);

    // Create a vertical box for the content area
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), vbox);

    // Initialize an array to store total expenses for each category
    gdouble total_expenses[G_N_ELEMENTS(categories)];
    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        total_expenses[i] = 0.0;
    }

    // Initialize the total expense
    gdouble total_expense = 0.0;

    // Open the CSV file
    FILE *file = fopen(CSV_FILE, "r");
    if (file) {
        // Read the CSV file and calculate total expenses for each category
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char *price, *category;
            price = strtok(line, ",");
            strtok(NULL, ","); // Ignore the time
            category = strtok(NULL, ",");
            if (price && category) {
                // Trim leading and trailing whitespace from category
                gchar *trimmed_category = g_strstrip(g_strdup(category));
                // Convert the category to lowercase for comparison
                gchar *category_lowercase = g_ascii_strdown(trimmed_category, -1);

                gboolean found = FALSE;
                // Find the category index
                for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
                    // Convert predefined category to lowercase for comparison
                    gchar *current_category_lowercase = g_ascii_strdown(categories[i], -1);
                    if (g_strcmp0(category_lowercase, current_category_lowercase) == 0) {
                        // Add the price to the total expense for this category
                        total_expenses[i] += atof(price);
                        found = TRUE;
                        break;
                    }
                    g_free(current_category_lowercase);
                }

                // If category not found in predefined categories, add it
                if (!found) {
                    // Add the category to the list of categories
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), trimmed_category);
                    // Add the expense to the newly added category
                    total_expenses[G_N_ELEMENTS(categories) - 1] += atof(price);
                }

                // Free memory
                g_free(trimmed_category);
                g_free(category_lowercase);

                // Add the price to the total expense
                total_expense += atof(price);
            }
        }
        fclose(file);
    }

    // Display total expense
    GtkWidget *total_label = gtk_label_new(NULL);
    gchar *total_text = g_strdup_printf("Total Expense from all Categories: %.2f", total_expense);
    gtk_label_set_text(GTK_LABEL(total_label), total_text);
    g_free(total_text);
    gtk_box_pack_start(GTK_BOX(vbox), total_label, FALSE, FALSE, 0);

    // Display stats for each category
    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        // Create a horizontal box for each category
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

        // Create a label for category name
        GtkWidget *category_label = gtk_label_new(categories[i]);
        gtk_box_pack_start(GTK_BOX(hbox), category_label, FALSE, FALSE, 0);

        // Create a label for total expense
        gchar *stats_text = g_strdup_printf("Total Expense: %.2f", total_expenses[i]);
        GtkWidget *expense_label = gtk_label_new(stats_text);
        g_free(stats_text);
        gtk_box_pack_end(GTK_BOX(hbox), expense_label, FALSE, FALSE, 0);

        // Add the horizontal box to the dialog
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
