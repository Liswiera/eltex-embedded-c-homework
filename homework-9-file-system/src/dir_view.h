#pragma once
#include <dirent.h>
#include <malloc.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dir_walker.h"

/*
    dir_view owns the following data:
        wnd (only if it's not stdscr), walker, dir_entries
    Therefore the dir_view struct must be deallocated using dir_view_free()
    in order to avoid memory leaks.
*/

struct dir_view {
    WINDOW *wnd;
    struct dir_walker *walker;
    struct dirent **entries;

    int entry_count;
    int entry_selected_id;
    int page_size;
    int page_start_id;
};

struct dir_view* dir_view_create(WINDOW *wnd);
struct dir_view* dir_view_from_path(WINDOW *wnd, const char *path);
void dir_view_free(struct dir_view *view);
void dir_view_print_entries(const struct dir_view *view);

int dir_view_open_selected_dir(struct dir_view *view);

// Returns the offset by which the current position has changed
int dir_view_move_selected_entry(struct dir_view *view, int offset);

int dir_view_move_one_up(struct dir_view *view);
int dir_view_move_one_down(struct dir_view *view);
int dir_view_move_page_up(struct dir_view *view);
int dir_view_move_page_down(struct dir_view *view);
int dir_view_move_top(struct dir_view *view);
int dir_view_move_bottom(struct dir_view *view);
