#include "gui_common.h"


 void key_common_handle(GuiInfoPtr gui_info, CommonEvent event){

	Point uv;
	switch (event){

		case Key_Up:gui_info->current_Y--;break;
		
		case Key_Down:gui_info->current_Y++;break;
		
		case Key_Left:gui_info->current_X--;break;
		
		case Key_Right:gui_info->current_X++;break;
		
		case Key_Enter: //enter
				get_device_current_coord(&uv);
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].x = uv.x;
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].y = uv.y;
				gui_info->point_uv[gui_info->current_X][gui_info->current_Y].z = uv.z;
				gui_info->recorded[gui_info->current_X][gui_info->current_Y] = RECORDED;
				break;	
		case Key_Esc:
				gtk_main_quit();
				break;

		case Key_w:	
				write_calib_mtx(gui_info);
				write_calib_uvz(gui_info);
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

}

