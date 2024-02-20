#include "viewer.h"

int move_obj(ObjData *data_obj, double x, double y, double z) {
  ResultCode code = RESULT_OK;

  if (data_obj == NULL || data_obj->vertex_array == NULL) {
    code = RESULT_ERROR;
  } else {
    double *p = data_obj->vertex_array;
    int n = data_obj->vertex_count * 3;
    while (n > 0) {
      *p += x;
      p++;
      *p += y;
      p++;
      *p += z;
      p++;
      n -= 3;
    }
  }
  return code;
}

int scale_obj(ObjData *data_obj, double x) {
  ResultCode code = RESULT_OK;
  if (data_obj == NULL || data_obj->vertex_array == NULL || x <= 0.0) {
    code = RESULT_ERROR;
  } else {
    double *p = data_obj->vertex_array;
    int n = data_obj->vertex_count * 3;
    while (n > 0) {
      *p *= x;
      p++;
      n--;
    }
  }
  return code;
}

int rotate_obj(ObjData *data_obj, double x, double y, double z) {
  ResultCode code = RESULT_OK;
  if (data_obj == NULL || data_obj->vertex_array == NULL) {
    code = RESULT_ERROR;
  } else {
    x *= PI / 180;
    y *= PI / 180;
    z *= PI / 180;

    double cos_x = cos(x);
    double sin_x = sin(x);
    double cos_y = cos(y);
    double sin_y = sin(y);
    double cos_z = cos(z);
    double sin_z = sin(z);
    double tmp[3];

    for (int i = 0; i < data_obj->vertex_count * 3; i += 3) {
      memcpy(tmp, data_obj->vertex_array + i, sizeof(tmp));
      data_obj->vertex_array[i + 1] = tmp[1] * cos_x - tmp[2] * sin_x;
      data_obj->vertex_array[i + 2] = tmp[1] * sin_x + tmp[2] * cos_x;

      memcpy(tmp, data_obj->vertex_array + i, sizeof(tmp));
      data_obj->vertex_array[i] = tmp[0] * cos_y + tmp[2] * sin_y;
      data_obj->vertex_array[i + 2] = -tmp[0] * sin_y + tmp[2] * cos_y;

      memcpy(tmp, data_obj->vertex_array + i, sizeof(tmp));
      data_obj->vertex_array[i] = tmp[0] * cos_z - tmp[1] * sin_z;
      data_obj->vertex_array[i + 1] = tmp[0] * sin_z + tmp[1] * cos_z;
    }
  }
  return code;
}