/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2000 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 *
 * Prototypes involving X
 *
 */
#ifndef __XPROTOS_H_
#define __XPROTOS_H_

#include <Xm/Xm.h>

#include "defines.h"

int initialize_gui(int *argc, char **argv);
void startup_gui(void);

void set_left_footer(char *s);

void xdrawgraph(void);
void expose_resize(Widget w, XtPointer client_data, XmDrawingAreaCallbackStruct *cbs);

void setpointer(VPoint vp);

void select_line(int x1, int y1, int x2, int y2, int erase);
void select_region(int x1, int y1, int x2, int y2, int erase);
void slide_region(view bbox, int shift_x, int shift_y, int erase);
void reset_crosshair(void);
void crosshair_motion(int x, int y);

void draw_focus(int gno);
void switch_current_graph(int gto);

char *display_name(void);

void xunregister_rti(XtInputId);
void xregister_rti(Input_buffer *ib);

int yesnowin(char *msg1, char *msg2, char *s1, char *help_anchor);

void create_file_popup(void *data);
void create_netcdfs_popup(void *data);
void create_rparams_popup(void *data);
void create_wparam_frame(void *data);
void create_saveproject_popup(void);
void create_openproject_popup(void);
void do_hotupdate_proc(void *data);

void create_eblock_frame(int gno);

void create_printer_setup(void *data);

void open_command(void *data);

void create_eval_frame(void *data);
void create_load_frame(void *data);
void create_histo_frame(void *data);
void create_fourier_frame(void *data);
void create_run_frame(void *data);
void create_reg_frame(void *data);
void create_diff_frame(void *data);
void create_seasonal_frame(void *data);
void create_interp_frame(void *data);
void create_int_frame(void *data);
void create_xcor_frame(void *data);
void create_samp_frame(void *data);
void create_prune_frame(void *data);
void create_digf_frame(void *data);
void create_lconv_frame(void *data);
void create_leval_frame(void *data);
void create_geom_frame(void *data);

void create_write_popup(void *data);
void create_hotlinks_popup(void *data);
void update_hotlinks(void);
void create_saveall_popup(void *data);

void create_points_frame(void *data);

void create_region_frame(void *data);
void create_define_frame(void *data);
void create_clear_frame(void *data);
void create_reporton_frame(void *data);
void create_area_frame(void *data);

void define_region(int nr, int rtype);

void define_status_popup(void *data);
void create_about_grtool(void *data);

void update_set_lists(int gno);

void updatesymbols(int gno, int value);
void updatelegends(int gno);
void update_view(int gno);

void define_symbols_popup(void *data);

void update_ticks(int gno);
void create_axes_dialog(int axisno);
void create_axes_dialog_cb(void *data);

void create_graph_frame(void *data);

void create_world_frame(void *data);
void create_arrange_frame(void *data);
void create_overlay_frame(void *data);
void create_autos_frame(void *data);

void define_objects_popup(void *data);

void update_locator_items(int gno);
void create_locator_frame(void *data);

void create_graphapp_frame(int gno);
void create_graphapp_frame_cb(void *data);

void create_monitor_frame_cb(void *data);
void stufftextwin(char *s);

void HelpCB(void *data);

void create_nonl_frame(void *data);
void update_nonl_frame(void);
void update_prune_frame(void);

void update_misc_items(void);
void create_plot_frame(void);
void create_plot_frame_cb(void *data);
void create_props_frame(void *data);

void create_fonttool(Widget w);
void create_fonttool_cb(void *data);

void set_wait_cursor(void);
void unset_wait_cursor(void);
void set_cursor(int c);
void init_cursors(void);
int init_option_menus(void);

void sync_canvas_size(unsigned int *w, unsigned int *h, int inv);

void box_edit_popup(int no);
void ellipse_edit_popup(int no);
void line_edit_popup(int no);
void string_edit_popup(int no);
int object_edit_popup(int type, int id);

void set_title(char *ts);

void set_pagelayout(int layout);
int get_pagelayout(void);

void errwin(const char *s);

void create_datasetprop_popup(void *data);
void create_datasetop_popup(void *data);
void create_setop_popup(void *data);

void create_featext_frame(void *data);

void create_ss_frame(int gno, int setno);
void update_ss_editors(int gno);
void do_ext_editor(int gno, int setno);

void set_graph_selectors(int gno);

void update_props_items(void);
void update_all(void);
void update_all_cb(void *data);

void set_barebones(int onoff);

#endif /* __XPROTOS_H_ */
