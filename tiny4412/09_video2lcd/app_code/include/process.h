
#ifndef _PROCESS_H
#define _PROCESS_H

#include <disp_manager.h>

int pic_merge(int x, int y, p_pixel_datas small_pic, p_pixel_datas big_pic);
int pic_merge_region(int start_x_new_pic, int start_y_new_pic, int start_x_old_pic, int start_y_old_pic, 
                             int width, int height, p_pixel_datas new_pic, p_pixel_datas old_pic);
int pic_zoom(p_pixel_datas origin_pic, p_pixel_datas zoom_pic);


#endif /* _RENDER_H */


