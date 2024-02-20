#ifndef SRC_VIEWER_H
#define SRC_VIEWER_H

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265

typedef enum {
  RESULT_OK,
  RESULT_ERROR,
  RESULT_FILE_NOT_FOUND,
  RESULT_MALLOC_ERROR
} ResultCode;

typedef struct ObjData_t {
  int vertex_count;
  int facet_count;
  double *vertex_array;
  int *facet_array;
  double max_verter;
  int facet_count_ind;
} ObjData;

int parserObj(const char *file_obj, ObjData *data_obj);
int countVertexAndFacet(FILE *file_ptr, ObjData *data_obj);
int readVertexAndFacet(FILE *file_ptr, ObjData *data_obj);
int parserVertex(char *str, ObjData *data_obj, int vertex_count);
int parserFacet(char *str, ObjData *data_obj, int *facet_count,
                int *vertex_count);
int centerObject(ObjData *data_obj);
void removeData(ObjData *data_obj);

int move_obj(ObjData *data_obj, double x, double y, double z);
int scale_obj(ObjData *data_obj, double x);
int rotate_obj(ObjData *data_obj, double x, double y, double z);

#endif
