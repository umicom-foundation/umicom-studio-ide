// Replace deprecated GTK loop
while (g_main_context_pending(NULL)) {
    g_main_context_iteration(NULL, FALSE);
}
