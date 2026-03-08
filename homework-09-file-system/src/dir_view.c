#include "dir_view.h"

static int dir_comparator(const struct dirent **left, const struct dirent **right) {
    const char *left_name = (*left)->d_name;
    const char *right_name = (*right)->d_name;

    if (strcmp(left_name, ".") == 0) return -1;
    if (strcmp(right_name, ".") == 0) return 1;

    if (strcmp(left_name, "..") == 0) return -1;
    if (strcmp(right_name, "..") == 0) return 1;

    if (left_name[0] != '.' && right_name[0] == '.') return -1;
    if (left_name[0] == '.' && right_name[0] != '.') return 1;

    return alphasort(left, right);
}

// Deallocate dir entries
static void dir_view_free_entries(struct dir_view *view) {
    if (view->entry_count == -1) return;

    for (int i = 0; i < view->entry_count; i++) {
        free(view->entries[i]);
    }
    free(view->entries);

    view->entry_count = -1;
    view->entries = NULL;
}

// Rescan the directory using the path value in the view
static int dir_view_refresh_dir_listing(struct dir_view *view) {
    view->entry_selected_id = 0;
    view->page_start_id = 0;
    
    dir_view_free_entries(view);
    return view->entry_count = scandir(view->walker->path, &view->entries, NULL, dir_comparator);
}

// The ownership of *wnd is NOT taken if the returned value is NULL
struct dir_view* dir_view_create(WINDOW *wnd) {
    struct dir_view *view = malloc(sizeof(struct dir_view));
    if (view == NULL) {
        return NULL;
    }

    view->walker = dir_walker_from_cwd();
    if (view->walker == NULL) {
        free(view);
        return NULL;
    }

    view->entry_count = -1;
    dir_view_refresh_dir_listing(view);
    if (view->entry_count == -1) {
        free(view);
        return NULL;
    }

    view->wnd = wnd;
    view->page_size = getmaxy(wnd);

    return view;
}

static int dir_view_get_entry_color_id(const struct dir_view *view, int entry_id) {
    int selected_val = (view->entry_selected_id == entry_id) ? 1 : 0;

    int dir_val = 0;
    switch (view->entries[entry_id]->d_type) {
        case DT_DIR:
            dir_val = 2;
            break;
        case DT_LNK:
            dir_val = 4;
            break;
    }

    return 1 + selected_val + dir_val;
}

static int dir_view_get_entry_attr(const struct dir_view *view, int entry_id) {
    int color_id = dir_view_get_entry_color_id(view, entry_id);
    int bold_attr = (view->entry_selected_id == entry_id) ? A_BOLD : 0;

    return COLOR_PAIR(color_id) | bold_attr;
}

static void dir_view_update_entry_attr_at(const struct dir_view *view, int entry_id) {
    int attr = dir_view_get_entry_attr(view, entry_id);
    int color_id = dir_view_get_entry_color_id(view, entry_id);

    int row_id = entry_id - view->page_start_id;
    
    mvwchgat(view->wnd, row_id, 0, -1, attr, color_id, NULL);
}




// The *wnd is NOT deallocated if the returned value is NULL
// This method is almost identical to dir_view_create
struct dir_view* dir_view_from_path(WINDOW *wnd, const char *path) {
    struct dir_view *view = malloc(sizeof(struct dir_view));
    if (view == NULL) {
        return NULL;
    }

    view->walker = dir_walker_from_path(path);
    if (view->walker == NULL) {
        free(view);
        return NULL;
    }

    view->entry_count = -1;
    dir_view_refresh_dir_listing(view);
    if (view->entry_count == -1) {
        free(view);
        return NULL;
    }

    view->wnd = wnd;
    view->page_size = getmaxy(wnd);

    return view;
}

void dir_view_free(struct dir_view *view) {
    if (view->wnd != stdscr) {
        delwin(view->wnd);
    }

    dir_walker_free(view->walker);
    dir_view_free_entries(view);
    free(view);
}

void dir_view_print_entries(const struct dir_view *view) {
    wclear(view->wnd);
    wmove(view->wnd, 0, 0);

    for (int i = 0; i < view->page_size; i++) {
        int entry_id = view->page_start_id + i;
        if (entry_id >= view->entry_count) break;

        int attr = dir_view_get_entry_attr(view, entry_id);

        int old_attr = wattrset(view->wnd, attr);
        mvwaddnstr(view->wnd, i, 0, view->entries[entry_id]->d_name, getmaxx(view->wnd) - 1);
        wprintw(view->wnd, "\n");
        wattrset(view->wnd, old_attr);
    }
    
    wrefresh(view->wnd);

    for (int i = 0; i < view->page_size; i++) {
        int entry_id = view->page_start_id + i;
        if (entry_id >= view->entry_count) break;

        dir_view_update_entry_attr_at(view, entry_id);
    }

    wrefresh(view->wnd);
}

int dir_view_open_selected_dir(struct dir_view *view) {
    const struct dirent *entry = view->entries[view->entry_selected_id];
    const char *path = entry->d_name;
    const int type = entry->d_type;

    if ((type != DT_DIR) && (type != DT_LNK)) {
        return 0;
    }

    size_t status;

    if (strcmp(path, ".") == 0) {
        status = 0;
    }
    else if (strcmp(path, "..") == 0) {
        status = dir_walker_go_up(view->walker);
    }
    else {
        status = dir_walker_go_into(view->walker, path);
    }

    if (status > 0) {
        dir_view_refresh_dir_listing(view);
        dir_view_print_entries(view);
    }

    return status > 0;
}

static int dir_view_update_page(struct dir_view *view) {
    int *entry_id = &view->entry_selected_id;
    int *page_start_id = &view->page_start_id;

    int page_size = view->page_size;
    int page_end_id = *page_start_id + page_size - 1;

    int offset = 0;
    if (*entry_id < *page_start_id) {
        offset = *entry_id - *page_start_id;
    }
    else if (*entry_id > page_end_id) {
        offset = *entry_id - page_end_id;
    }

    *page_start_id += offset;
    return offset;
}
int dir_view_move_selected_entry(struct dir_view *view, int offset) {
    int new_position = view->entry_selected_id + offset;

    if (new_position < 0) new_position = 0;
    else if (new_position >= view->entry_count) new_position = view->entry_count - 1;

    offset = new_position - view->entry_selected_id;
    if (offset == 0)  {
        return 0;
    }

    int old_position = view->entry_selected_id;
    view->entry_selected_id = new_position;

    int page_offset = dir_view_update_page(view);
    if (page_offset == 0) {
        dir_view_update_entry_attr_at(view, old_position);
        dir_view_update_entry_attr_at(view, new_position);
    }
    else {
        // Entire page has been shifted, redraw everything
        dir_view_print_entries(view);
    }

    return offset;
}

int dir_view_move_one_up(struct dir_view *view) {
    return dir_view_move_selected_entry(view, -1);
}

int dir_view_move_one_down(struct dir_view *view) {
    return dir_view_move_selected_entry(view, 1);
}

int dir_view_move_page_up(struct dir_view *view) {
    return dir_view_move_selected_entry(view, -view->page_size);
};

int dir_view_move_page_down(struct dir_view *view) {
    return dir_view_move_selected_entry(view, view->page_size);
};

int dir_view_move_top(struct dir_view *view) {
    return dir_view_move_selected_entry(view, -view->entry_selected_id);
};

int dir_view_move_bottom(struct dir_view *view) {
    int offset = (view->entry_count - 1) - view->entry_selected_id;
    return dir_view_move_selected_entry(view, offset);
};
