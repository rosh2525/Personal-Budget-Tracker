#include "gui.h"
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#define CSV_FILE "expenses.csv" // expense list is stored in this file
#define BALANCE_FILE "balance.txt" // bank balance is stored in this .txt
#define BUDGET_FILE "budget_limits.txt" //budget limits is stored here

GtkWidget *expense_entry;
GtkWidget *category_combo;
GtkWidget *expense_scrolled_window;
GtkWidget *delete_button;
GtkListStore *store;
GtkWidget *sort_button;
GtkWidget *stats_button;
GtkWidget *add_income_button;
gdouble current_balance;
GtkWidget *window;
GtkWidget *budget_limit_button;
GtkWidget *add_category_button;

const gchar *categories[] = {"Groceries", "Transportation", "Entertainment", "Utilities"};

gdouble budget_limits[G_N_ELEMENTS(categories)];

// MAIN gui
GtkWidget *create_main_window() {
    load_budget_limits();
    GtkWidget *main_box;
    GtkWidget *label;
    GtkWidget *add_button;
    GtkWidget *clear_button;
    GtkWidget *tree_view;
    GtkWidget *reset_balance_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Personal Budget Tracker");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    expense_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(expense_entry), "Enter expense amount");
    gtk_box_pack_start(GTK_BOX(main_box), expense_entry, FALSE, FALSE, 0);

    g_signal_connect(expense_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    category_combo = gtk_combo_box_text_new();
    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), categories[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_box_pack_start(GTK_BOX(main_box), category_combo, FALSE, FALSE, 0);

    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), row, FALSE, FALSE, 0);

    delete_button = gtk_button_new_with_label("Delete");
    add_button = gtk_button_new_with_label("Add Expense");
    clear_button = gtk_button_new_with_label("Clear All");
    sort_button = gtk_button_new_with_label("Sort by Category");
    reset_balance_button = gtk_button_new_with_label("Reset Balance"); // New button

    gtk_box_pack_start(GTK_BOX(row), delete_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), add_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), clear_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), sort_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), reset_balance_button, FALSE, FALSE, 0); // Add reset balance button

    label = gtk_label_new("Expense List:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);

    expense_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(expense_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), expense_scrolled_window, TRUE, TRUE, 0);

    store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(expense_scrolled_window), tree_view);

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

    g_signal_connect(stats_button, "clicked", G_CALLBACK(on_stats_clicked), NULL);

    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), tree_view);
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_expense_clicked), NULL);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_all_clicked), tree_view);
    g_signal_connect(sort_button, "clicked", G_CALLBACK(on_sort_by_category_clicked), tree_view);
    //g_signal_connect(category_combo, "changed", G_CALLBACK(on_category_changed), NULL);

    add_income_button = gtk_button_new_with_label("Add Income");
    gtk_box_pack_start(GTK_BOX(row), add_income_button, FALSE, FALSE, 0);

    g_signal_connect(add_income_button, "clicked", G_CALLBACK(on_add_income_clicked), NULL);

    load_expenses_from_csv();
    load_current_balance();

    g_signal_connect(reset_balance_button, "clicked", G_CALLBACK(on_reset_balance_clicked), NULL);

    gtk_widget_show_all(window);

    budget_limit_button = gtk_button_new_with_label("Set Budget Limit"); // Create the button
    gtk_box_pack_start(GTK_BOX(row), budget_limit_button, FALSE, FALSE, 0); // Pack the button into the row
    g_signal_connect(budget_limit_button, "clicked", G_CALLBACK(on_set_budget_limit_clicked), NULL); // Connect signal handler for the button

    add_category_button = gtk_button_new_with_label("Add Category"); // Create the button
    gtk_box_pack_start(GTK_BOX(row), add_category_button, FALSE, FALSE, 0); // Pack the button into the row
    g_signal_connect(add_category_button, "clicked", G_CALLBACK(on_add_category_clicked), NULL); // Connect signal handler for the button

    update_window_title(NULL);

    return window;
}
// Function to add a new category
void on_add_category_clicked(GtkButton *button, gpointer data) {
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
            gboolean exists = FALSE;
            for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
                if (g_strcmp0(new_category, categories[i]) == 0) {
                    exists = TRUE;
                    break;
                }
            }
            if (!exists) {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), new_category);
                categories[G_N_ELEMENTS(categories)] = g_strdup(new_category);
                budget_limits[G_N_ELEMENTS(categories) - 1] = -1.0; // Set the default budget limit
                save_budget_limits();
            } else {
                GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Category already exists.");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
        }
    }
    gtk_widget_destroy(dialog);

    g_signal_handlers_disconnect_by_func(add_category_button, G_CALLBACK(on_add_category_clicked), NULL);
}

// function to set budget limits
void on_set_budget_limit_clicked(GtkButton *button, gpointer data) {
   const gchar *selected_category = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(category_combo));
    if (selected_category != NULL) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Set Budget Limit", NULL, GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "Set", GTK_RESPONSE_ACCEPT, NULL);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter budget limit");
        gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *limit_text = gtk_entry_get_text(GTK_ENTRY(entry));
            if (g_strcmp0(limit_text, "") != 0) {
                gdouble budget_limit = atof(limit_text);
                gint category_index = get_category_index(selected_category);
                if (category_index >= 0) {
                    budget_limits[category_index] = budget_limit;
                    save_budget_limits(); // Save budget limits to file
                }
            }
        }
        gtk_widget_destroy(dialog);
        g_free(selected_category);
    }
}

// finding index of category from array

gint get_category_index(const gchar *category) {
    for (gint i = 0; i < G_N_ELEMENTS(categories); ++i) {
        if (g_strcmp0(category, categories[i]) == 0) {
            return i;
        }
    }
    return -1; // means Category not found
}

// resets bank balance to 0
void on_reset_balance_clicked(GtkButton *button, gpointer data) {
    current_balance = 0.0;
    gchar *new_title = g_strdup_printf("Personal Budget Tracker (Balance: %.2f)", current_balance);
    gtk_window_set_title(GTK_WINDOW(window), new_title);
    g_free(new_title);
    save_current_balance();
}

// when add expense is clicked
void on_add_expense_clicked(GtkButton *button, gpointer data) {
    const gchar *expense_text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    const gchar *category_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(category_combo));

    // Check if the expense text is valid (contains only numeric characters)
    gboolean valid_expense = TRUE;
    for (int i = 0; expense_text[i] != '\0'; i++) {
        if (!isdigit(expense_text[i]) && expense_text[i] != '.') {
            valid_expense = FALSE;
            break;
        }
    }

    // Check if entry is empty, category is selected, and expense is valid
    if (g_strcmp0(expense_text, "") != 0 && category_text != NULL && valid_expense) {
        // Convert the expense text to a numerical value
        gdouble expense_amount = atof(expense_text);

        // Check if the conversion was successful
        if (expense_amount == 0.0 && strcmp(expense_text, "0.00") != 0) {
            // Handle conversion error
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid expense amount");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }

        // Check if the bank balance is negative
        if (current_balance < 0) {
            // Display an error message
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Bank balance is negative. Cannot add expense.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }

        // Check if the expense exceeds the budget limit for the selected category
        gint category_index = get_category_index(category_text);
        if (category_index >= 0 && budget_limits[category_index] > 0 && expense_amount > budget_limits[category_index]) {
            // Show a warning message
            GtkWidget *warning_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Expense exceeds budget limit for %s", category_text);
            gtk_dialog_run(GTK_DIALOG(warning_dialog));
            gtk_widget_destroy(warning_dialog);
            return; // Do not proceed with adding the expense if it exceeds the budget limit
        }

        // Check if the expense exceeds the current balance
        if (expense_amount > current_balance) {
            // Show an error message
            GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Insufficient funds to complete the transaction.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            return;
        }

        // Get current time
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        gchar time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Create a new row in the tree store model
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);

        // Set the values for the new row (price, time, reason)
        gtk_list_store_set(store, &iter, COLUMN_PRICE, expense_text, COLUMN_TIME, time_str, COLUMN_REASON, category_text, -1);

        // Clear the expense entry field
        gtk_entry_set_text(GTK_ENTRY(expense_entry), "");

        // Free the memory allocated for the category text (from combo box)
        g_free((gchar *) category_text);

        // Update bank balance
        current_balance -= expense_amount;

        // Update window title with current balance
        gchar *new_title = g_strdup_printf("Personal Budget Tracker (Balance: %.2f)", current_balance);
        gtk_window_set_title(GTK_WINDOW(window), new_title);
        g_free(new_title);

        // Save the updated expenses to the CSV file (replace with your implementation)
        save_expenses_to_csv();  // Placeholder for your function to save expenses

        // Save the current balance to the file (replace with your implementation)
        save_current_balance(); // Placeholder for your function to save balance
    } else {
        // Handle invalid expense entry
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid expense entry");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// when text box value is changed
void on_entry_changed(GtkEditable *editable, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(expense_entry));
    for (int i = 0; text[i] != '\0'; i++) {
        if (!isdigit(text[i]) && text[i] != '.') {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter only numbers.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            gtk_entry_set_text(GTK_ENTRY(expense_entry), "");
            break;
        }
    }
}
// when delete buton is clicked
void on_delete_clicked(GtkButton *button, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *price_str;
        gtk_tree_model_get(model, &iter, COLUMN_PRICE, &price_str, -1);
        gdouble expense_amount = atof(price_str);
        g_free(price_str);
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
        current_balance += expense_amount;
        update_window_title(NULL);
        save_current_balance();
    }
    save_expenses_to_csv();
}

// when clear all button is clicked
void on_clear_all_clicked(GtkButton *button, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
    gtk_list_store_clear(GTK_LIST_STORE(model));

    save_expenses_to_csv();
}
// load from csv file
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
                gchar *trimmed_reason = g_strstrip(g_strdup(reason));
                gchar *reason_lowercase = g_ascii_strdown(trimmed_reason, -1);

                gboolean found = FALSE;
                for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
                    gchar *category_lowercase = g_ascii_strdown(categories[i], -1);
                    if (g_strcmp0(reason_lowercase, category_lowercase) == 0) {
                        found = TRUE;
                        break;
                    }
                    g_free(category_lowercase);
                }

                if (!found) {
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), trimmed_reason);
                    g_free(trimmed_reason);
                }
                g_free(reason_lowercase);

                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter, COLUMN_PRICE, price, COLUMN_TIME, time, COLUMN_REASON, reason, -1);
            }
        }
        fclose(file);
    }
}

// save the treeview in a csv file
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
// when sort by category is clicked
void on_sort_by_category_clicked(GtkButton *button, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    gtk_tree_sortable_set_sort_column_id(sortable, COLUMN_REASON, GTK_SORT_ASCENDING);
}
// when category is changed in the combo box
void on_category_changed(GtkComboBox *combo_box, gpointer user_data) {
    gint active = gtk_combo_box_get_active(combo_box);

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
                    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(category_combo), G_N_ELEMENTS(categories) - 1, new_category);
                    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), G_N_ELEMENTS(categories) - 2);
                } else {
                    GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Category already exists.");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                }
            }
        }
        gtk_widget_destroy(dialog);
    }
}

// when on stats button is clicked
void on_stats_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Expense Stats", NULL, GTK_DIALOG_MODAL, "Close", GTK_RESPONSE_CLOSE, NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), vbox);

    gdouble total_expense = 0.0;
    gdouble total_expenses[G_N_ELEMENTS(categories)];
    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        total_expenses[i] = 0.0;
    }

    FILE *file = fopen(CSV_FILE, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char *price, *category;
            price = strtok(line, ",");
            strtok(NULL, ","); // Ignore the time
            category = strtok(NULL, ",");
            if (price && category) {
                gchar *trimmed_category = g_strstrip(g_strdup(category));
                gchar *category_lowercase = g_ascii_strdown(trimmed_category, -1);

                gboolean found = FALSE;
                for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
                    gchar *current_category_lowercase = g_ascii_strdown(categories[i], -1);
                    if (g_strcmp0(category_lowercase, current_category_lowercase) == 0) {
                        total_expenses[i] += atof(price);
                        total_expense += atof(price);
                        found = TRUE;
                        break;
                    }
                }

                if (!found) {
                    // This handles new categories
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), trimmed_category);
                    gint new_category_index = gtk_combo_box_get_active(GTK_COMBO_BOX(category_combo));
                    total_expenses[new_category_index] += atof(price);
                    total_expense += atof(price);
                }

                g_free(trimmed_category);
                g_free(category_lowercase);
            }
        }
        fclose(file);
    }

    GtkWidget *total_label = gtk_label_new(NULL);
    gchar *total_text = g_strdup_printf("Total Expense from all Categories: %.2f", total_expense);
    gtk_label_set_text(GTK_LABEL(total_label), total_text);
    g_free(total_text);
    gtk_box_pack_start(GTK_BOX(vbox), total_label, FALSE, FALSE, 0);

    for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        GtkWidget *category_label = gtk_label_new(categories[i]);
        gtk_box_pack_start(GTK_BOX(hbox), category_label, FALSE, FALSE, 0);
        gchar *stats_text = g_strdup_printf("Total Expense: %.2f", total_expenses[i]);
        GtkWidget *expense_label = gtk_label_new(stats_text);
        g_free(stats_text);
        gtk_box_pack_end(GTK_BOX(hbox), expense_label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}



// when add income button is clicked
void on_add_income_clicked(GtkButton *button, gpointer data) {
    const gchar *income_text = gtk_entry_get_text(GTK_ENTRY(expense_entry));

    if (g_strcmp0(income_text, "") != 0) {
        gdouble income = atof(income_text);
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        gchar time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COLUMN_PRICE, income_text, COLUMN_TIME, time_str, COLUMN_REASON, "Income", -1);
        current_balance += income;
        gchar *new_title = g_strdup_printf("Personal Budget Tracker (Balance: %.2f)", current_balance);
        gtk_window_set_title(GTK_WINDOW(window), new_title);
        g_free(new_title);

        save_current_balance();
        gtk_entry_set_text(GTK_ENTRY(expense_entry), "");
    }
}

// load the balance of bank from file
void load_current_balance() {
    FILE *file = fopen(BALANCE_FILE, "r");
    if (file) {
        fscanf(file, "%lf", &current_balance);
        fclose(file);
    } else {
        // Set default balance to 0 if file doesn't exist
        current_balance = 0.0;
    }
    update_window_title(NULL);
}

// Function to save current balance to file
void save_current_balance() {
    FILE *file = fopen(BALANCE_FILE, "w");
    if (file) {
        fprintf(file, "%.2f", current_balance);
        fclose(file);
    }
}

// FUnction to update window title when a transaction is made or income is added
void update_window_title(gpointer user_data) {
    gchar *new_title = g_strdup_printf("Personal Budget Tracker (Balance: %.2f)", current_balance);
    gtk_window_set_title(GTK_WINDOW(window), new_title);
    g_free(new_title);
}
// budget limits load from file
void load_budget_limits() {
    FILE *file = fopen(BUDGET_FILE, "r");
    if (file) {
        for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
            if (fscanf(file, "%lf", &budget_limits[i]) != 1) {
                budget_limits[i] = -1.0;
            }
        }
        fclose(file);
    } else {
        // Set default budget limits and save to file
        for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
            budget_limits[i] = -1.0;
        }
        save_budget_limits();
    }
}
// save budget limits when it is made
void save_budget_limits() {
    FILE *file = fopen(BUDGET_FILE, "w");
    if (file) {
        for (int i = 0; i < G_N_ELEMENTS(categories); ++i) {
            fprintf(file, "%.2f\n", budget_limits[i]);
        }
        fclose(file);
    } else {
        printf("Error: Could not open file for writing budget limits\n");
    }
}
