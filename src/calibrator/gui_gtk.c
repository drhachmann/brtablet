/*
 * Copyright (c) 2011 Diego Rodrigo Hachmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <string.h>
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <cairo.h>

#include "gui_common.h"
#include "util.h"
#include "io.h"
#include "gui_gtk.h"



void GuiInfo_init(GuiInfoPtr gui_info){
	
	memset(gui_info->recorded, NO_RECORDED, sizeof(gui_info->recorded));
	gui_info->current_X = gui_info->current_Y = 0;

	int i, j;
	for(i=0; i<N_ROW; i++){
		for(j=0; j<N_COLUMN; j++){
			gui_info->point_xy[i][j].x = (int)(i*((gui_info->win_width)/((double)N_ROW-1)));
			gui_info->point_xy[i][j].y = (int)(j*((gui_info->win_height)/((double)N_COLUMN-1)));

		}
	}
}


static void draw_target(cairo_t *cr, int px, int py, Point uv, int win_width, int win_height){

	cairo_move_to(cr, px-25, py-25);
	cairo_line_to(cr, px+25, py+25);
	cairo_move_to(cr, px-25, py+25);
	cairo_line_to(cr, px+25, py-25);
	cairo_stroke(cr);
	char temp[50];

	if(px>25)px-=30;
	else px+= 10;
	if(px>win_width-75)px-=70;;
	if(py>win_height-75)py-=80;

	cairo_move_to(cr, px, py+40);
	sprintf(temp, "U:%d V:%d", uv.x, uv.y);
	cairo_show_text(cr, temp);
	
}


static void draw_target_all(cairo_t *cr, GuiInfoPtr gui_info){
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	int i,j;	
	for(i=0; i<N_ROW; i++){
		for(j=0; j<N_COLUMN; j++){
			if(i==gui_info->current_X && j==gui_info->current_Y){
				cairo_set_line_width(cr, 5);
				cairo_set_source_rgb(cr, 0, 0, 255);
			}else{
				switch(gui_info->recorded[i][j]){
					case NO_RECORDED:
						cairo_set_line_width(cr, 2);
						cairo_set_source_rgb(cr, 0, 0, 0);
						break;
					case RECORDED:
						cairo_set_line_width(cr, 2);
						cairo_set_source_rgb(cr, 255, 0, 0);
						break;
				}
			}
			draw_target(cr, gui_info->point_xy[i][j].x, gui_info->point_xy[i][j].y,\
							gui_info->point_uv[i][j], gui_info->win_width, gui_info->win_height);
		}
	}
}


static gboolean
on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
  	cairo_t *cr;
  	GuiInfoPtr gui_info = (GuiInfoPtr)user_data;
  	cr = gdk_cairo_create (widget->window);
	draw_target_all(cr, gui_info);
	return FALSE;
}



static CommonEvent get_key_common_event(GdkEventKey *event){
	switch (event->keyval){
		case GDK_Up: return Key_Up;
		case GDK_Down: return Key_Down;
		case GDK_Right: return Key_Right;
		case GDK_Left: return Key_Left;
		case GDK_Return: return Key_Enter;
		case GDK_Escape: return Key_Esc;
		case GDK_w: return Key_w;
	}
	return Key_error;
}	

static gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	GuiInfoPtr gui_info = (GuiInfoPtr)user_data;
	//CommonEvent event_c= get_key_common_event(event);
	//key_common_handle(gui_info, event_c);
	printf("KEU PRESS");
	Point uv;
	switch (event->keyval){

		case GDK_Up:gui_info->current_Y--;break;
		
		case GDK_Down:gui_info->current_Y++;break;
		
		case GDK_Left:gui_info->current_X--;break;
		
		case GDK_Right:gui_info->current_X++;break;
		
		case GDK_Return: //enter
				get_device_current_coord(&uv);
				printf("%d %d\n", uv.x, uv.y);
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].x = uv.x;
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].y = uv.y;
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].z = uv.z;
				gui_info->recorded[gui_info->current_X][gui_info->current_Y] = RECORDED;
				break;	
		case GDK_Escape://esc
				gtk_main_quit();
				break;

		case GDK_w:	
				write_calib_mtx(gui_info->point_uv, gui_info->point_xy);
				write_calib_uvz(gui_info->point_uv);
				puts("CALIB");
				driver_calibration();		
				gtk_main_quit();
				break;
	}	
	if(gui_info->current_X==N_COLUMN)
			gui_info->current_X=0;
	else if(gui_info->current_X==-1)
			gui_info->current_X=N_COLUMN-1;

	if(gui_info->current_Y==N_ROW)
			gui_info->current_Y=0;
	if(gui_info->current_Y==-1)
		gui_info->current_Y=N_ROW-1;


	gtk_widget_queue_draw(widget);
	return FALSE; /* propogate event */
}

static gboolean configure_event( GtkWidget *widget, GdkEventConfigure *event, gpointer user_data){
	GuiInfoPtr gui_info = (GuiInfoPtr)user_data;
	if(gui_info->win_width != widget->allocation.width || gui_info->win_height != widget->allocation.height){
		gui_info->win_width = widget->allocation.width;
		gui_info->win_height = widget->allocation.height;
		GuiInfo_init(gui_info);
	}
	return TRUE;
}


int main_gtk(int argc, char *argv[]){
	GtkWidget *window;
	GtkWidget *darea;  
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_realize (window);
	gtk_window_fullscreen (GTK_WINDOW(window));
	darea = gtk_drawing_area_new();
	GuiInfoPtr gui_info = (GuiInfoPtr)malloc(sizeof(GuiInfo));

	g_signal_connect(darea, "expose-event", G_CALLBACK(on_expose_event), gui_info);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), gui_info);
	g_signal_connect (darea, "configure_event", (GtkSignalFunc) configure_event, gui_info);
	g_signal_connect (window, "key_press_event", (GtkSignalFunc)on_key_press, gui_info);
	gtk_widget_add_events(window,GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

	gtk_container_add(GTK_CONTAINER (window), darea);
	GuiInfo_init(gui_info);
  	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
