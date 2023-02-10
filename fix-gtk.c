/*
 * Flash is an abandoned proprietary application, and more recent versions
 * of glib make it crash when older versions just threw warnings. Those
 * warnings were legitimate, but we can't easily patch the application
 * without side-effects, so work-around the problems in glib and gtk instead.
 * 
 * The crash is due to g_signal_handlers_disconnect_matched() being called
 * on a destroyed widget. Keep track of that widget from GTK so we can
 * avoid manipulating the freed memory when the function is called.
 * 
 * (flashplayer:2): GLib-GObject-WARNING **: instance with invalid (NULL) class pointer
 * (flashplayer:2): GLib-GObject-CRITICAL **: g_signal_handlers_disconnect_matched: assertion 'G_TYPE_CHECK_INSTANCE (instance)' failed
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#if DEBUG > 0
#define D(x...) printf(x)
#else
#define D(x...)
#endif

static guint (*func_g_signal_handlers_disconnect_matched) (gpointer instance,
    GSignalMatchType mask, guint signal_id, GQuark detail, GClosure *closure,
    gpointer func, gpointer data);
static void (*func_gtk_widget_destroy) (GtkWidget *widget);

static void
init (void)
{
  func_g_signal_handlers_disconnect_matched =
    dlsym (RTLD_NEXT, "g_signal_handlers_disconnect_matched");
  func_gtk_widget_destroy =
    dlsym (RTLD_NEXT, "gtk_widget_destroy");
}

static gpointer crash_workaround = NULL;

guint
g_signal_handlers_disconnect_matched (gpointer instance, GSignalMatchType mask,
    guint signal_id, GQuark detail, GClosure *closure, gpointer func, gpointer data)
{
  if (!func_g_signal_handlers_disconnect_matched)
    init();

  if (instance == crash_workaround) {
    D("Skipping dangling pointer, %p\n", instance);
    crash_workaround = NULL;
    return 0;
  }

  return (*func_g_signal_handlers_disconnect_matched)
    (instance, mask, signal_id, detail, closure, func, data);
}

void gtk_widget_destroy (GtkWidget *widget)
{
  if (!func_gtk_widget_destroy)
    init();

  g_return_if_fail (GTK_IS_WIDGET (widget));

  if (GTK_IS_MENU_BAR (widget)) {
    D("Destoying menu bar, %p\n", widget);
    crash_workaround = widget;
  }

  return (*func_gtk_widget_destroy) (widget);
}
