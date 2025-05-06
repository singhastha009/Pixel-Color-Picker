#include <gtk/gtk.h>

typedef struct {
    GtkWidget *picture;
    GtkWidget *color_label;
    GtkWidget *color_display;
    GtkWidget *entry_x;
    GtkWidget *entry_y;
    GdkPixbuf *pixbuf;
    char *image_path;
} AppData;

void update_color_display(AppData *data, guint8 r, guint8 g, guint8 b) {
    char css[128];
    snprintf(css, sizeof(css), "#color-display { background-color: rgb(%d, %d, %d); }", r, g, b);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);

    GtkStyleContext *context = gtk_widget_get_style_context(data->color_display);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

void get_pixel_color(AppData *data, int x, int y) {
    if (!data->pixbuf) return;

    int img_width = gdk_pixbuf_get_width(data->pixbuf);
    int img_height = gdk_pixbuf_get_height(data->pixbuf);

    if (x < 0 || y < 0 || x >= img_width || y >= img_height) {
        g_print("Out of bounds\n");
        gtk_label_set_text(GTK_LABEL(data->color_label), "Out of bounds");
        return;
    }

    int rowstride = gdk_pixbuf_get_rowstride(data->pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(data->pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(data->pixbuf);
    guchar *pixel = pixels + y * rowstride + x * n_channels;
    guint8 r = pixel[0], g = pixel[1], b = pixel[2];

    char color_text[64];
    snprintf(color_text, sizeof(color_text), "RGB: R=%d, G=%d, B=%d", r, g, b);
    gtk_label_set_text(GTK_LABEL(data->color_label), color_text);

    update_color_display(data, r, g, b);
}

void on_image_clicked(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    get_pixel_color(data, (int)x, (int)y);
}

void on_get_color_clicked(GtkWidget *button, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    const char *x_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(data->entry_x)));
    const char *y_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(data->entry_y)));
    int x = atoi(x_text);
    int y = atoi(y_text);
    get_pixel_color(data, x, y);
}

void on_app_activate(GtkApplication *app, gpointer user_data) {
    AppData *data = (AppData *)user_data;

    if (!data->image_path) {
        g_print("No image path provided.\n");
        return;
    }

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Pixel Color Picker");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GError *error = NULL;
    data->pixbuf = gdk_pixbuf_new_from_file(data->image_path, &error);
    if (!data->pixbuf) {
        g_print("Failed to load image: %s\n", error->message);
        g_error_free(error);
        return;
    }

    data->picture = gtk_picture_new_for_pixbuf(data->pixbuf);
    gtk_box_append(GTK_BOX(box), data->picture);

    GtkGesture *gesture = gtk_gesture_click_new();
    g_signal_connect(gesture, "pressed", G_CALLBACK(on_image_clicked), data);
    gtk_widget_add_controller(data->picture, GTK_EVENT_CONTROLLER(gesture));

    data->color_label = gtk_label_new("Click on the image or enter coordinates");
    gtk_box_append(GTK_BOX(box), data->color_label);

    data->color_display = gtk_frame_new(NULL);
    gtk_widget_set_name(data->color_display, "color-display");
    gtk_widget_set_size_request(data->color_display, 200, 50);
    gtk_box_append(GTK_BOX(box), data->color_display);

    GtkWidget *coord_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    data->entry_x = gtk_entry_new();
    data->entry_y = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry_x), "X Coordinate");
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry_y), "Y Coordinate");
    GtkWidget *button = gtk_button_new_with_label("Get Color");
    g_signal_connect(button, "clicked", G_CALLBACK(on_get_color_clicked), data);
    
    gtk_box_append(GTK_BOX(coord_box), data->entry_x);
    gtk_box_append(GTK_BOX(coord_box), data->entry_y);
    gtk_box_append(GTK_BOX(coord_box), button);
    gtk_box_append(GTK_BOX(box), coord_box);

    gtk_window_present(GTK_WINDOW(window));
}

void on_open(GtkApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    if (n_files > 0) {
        gchar *file_path = g_file_get_path(files[0]);
        data->image_path = g_strdup(file_path);
        g_free(file_path);
    }
    on_app_activate(app, data);
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.colorpicker", G_APPLICATION_HANDLES_OPEN);

    AppData data = {0};

    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), &data);
    g_signal_connect(app, "open", G_CALLBACK(on_open), &data);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}