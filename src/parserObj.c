#include "viewer.h"

int parserObj(const char *file_obj, ObjData *data_obj) {
  ResultCode code = RESULT_OK;

  if (file_obj == NULL || data_obj == NULL) {
    code = RESULT_ERROR;
  } else {
    FILE *file_ptr = fopen(file_obj, "r");
    if (file_ptr == NULL) {
      code = RESULT_FILE_NOT_FOUND;
    } else {
      data_obj->max_verter = 0.0;
      data_obj->facet_count = 0;
      data_obj->vertex_array = NULL;
      data_obj->facet_array = NULL;

      countVertexAndFacet(file_ptr, data_obj);
      if (code == RESULT_OK && data_obj->vertex_count > 0 &&
          data_obj->facet_count > 0) {
        data_obj->vertex_array =
            (double *)malloc(data_obj->vertex_count * 3 * sizeof(double));
        data_obj->facet_array =
            (int *)malloc(data_obj->facet_count * 3 * sizeof(int));

        if (data_obj->vertex_array == NULL || data_obj->facet_array == NULL) {
          code = RESULT_MALLOC_ERROR;
          free(data_obj->vertex_array);
          free(data_obj->facet_array);
        } else {
          fseek(file_ptr, 0, SEEK_SET);
          code = readVertexAndFacet(file_ptr, data_obj);
        }
      }
      fclose(file_ptr);
    }
  }
  return code;
}

int countVertexAndFacet(FILE *file_ptr, ObjData *data_obj) {
  ResultCode code = RESULT_OK;
  if (file_ptr == NULL || data_obj == NULL) {
    code = RESULT_ERROR;
  } else {
    int vertex_count = 0;
    int facet_count = 0;
    int facet_count_ind = 0;
    char str[500] = "";

    while (fgets(str, 500, file_ptr) != NULL) {
      if (str[0] == 'v' && str[1] == ' ') {
        vertex_count++;
      } else if (str[0] == 'f' && str[1] == ' ') {
        facet_count_ind++;
        int spaces = 0;
        for (int i = 1; str[i] != '\n'; i++) {
          if (str[i] == ' ') {
            spaces++;
          }
        }
        facet_count += spaces;
      }
    }
    data_obj->vertex_count = vertex_count;
    data_obj->facet_count = facet_count;
    data_obj->facet_count_ind = facet_count_ind;
  }
  return code;
}

int readVertexAndFacet(FILE *file_ptr, ObjData *data_obj) {
  ResultCode code = RESULT_OK;

  if (file_ptr == NULL || data_obj == NULL) {
    code = RESULT_ERROR;
  } else {
    int vertex_count = 0;
    int facet_count = 0;
    char str[500] = "";

    while (fgets(str, 500, file_ptr) != NULL) {
      if (str[0] == 'v' && str[1] == ' ') {
        parserVertex(str, data_obj, vertex_count);
        vertex_count++;
      } else if (str[0] == 'f' && str[1] == ' ') {
        code = parserFacet(str, data_obj, &facet_count, &vertex_count);
        if (code != RESULT_OK) {
          code = RESULT_ERROR;
        }
      }
    }
    if (code == RESULT_OK) {
      centerObject(data_obj);
    }
  }
  return code;
}

int parserVertex(char *str, ObjData *data_obj, int vertex_count) {
  ResultCode code = RESULT_OK;

  if (str == NULL || data_obj == NULL || vertex_count < 0) {
    code = RESULT_ERROR;
  } else {
    int counter = 0;
    char tmp[512] = "";

    for (size_t i = 1; i < strlen(str); i++) {
      if (isdigit(str[i]) || str[i] == '-') {
        int dlina_str = strcspn(str + i, " ");
        strncpy(tmp, str + i, dlina_str);
        tmp[dlina_str] = '\0';

        double tmp_double = atof(tmp);
        data_obj->vertex_array[vertex_count * 3 + counter] = tmp_double;

        if (fabs(tmp_double) > data_obj->max_verter) {
          data_obj->max_verter = fabs(tmp_double);
        }

        counter++;
        i += dlina_str - 1;
      }
    }
  }
  return code;
}

int parserFacet(char *str, ObjData *data_obj, int *facet_count,
                int *vertex_count) {
  ResultCode code = RESULT_OK;
  if (str == NULL || data_obj == NULL || facet_count == NULL ||
      *facet_count < 0) {
    code = RESULT_ERROR;
  } else {
    int counter = 0;
    int tmp_first_index = 0;

    for (size_t i = 1; i < strlen(str); i++) {
      if (isdigit(str[i]) || (str[i] == '-' && isdigit(str[i + 1]))) {
        char tmp[500] = "";
        int count_char = strcspn(str + i, " /");
        strncpy(tmp, str + i, count_char);
        tmp[count_char] = '\0';
        int num = atoi(tmp) - 1;

        if (num < 0) {
          num += (*vertex_count) + 1;
        }

        if (num >= 0 && num < data_obj->vertex_count) {
          if (counter == 0) {
            tmp_first_index = num;
            data_obj->facet_array[*facet_count + counter] = num;
            counter++;
          } else {
            data_obj->facet_array[*facet_count + counter] = num;
            counter++;
            data_obj->facet_array[*facet_count + counter] = num;
            counter++;
          }
        }

        i += count_char - 1;
      } else if (str[i] == '/') {
        int count_char = strcspn(str + i, " ");
        i += count_char - 1;
      }
    }
    if (!code) {
      data_obj->facet_array[*facet_count + counter] = tmp_first_index;
      counter++;

      (*facet_count) += counter;
    }
  }

  return code;
}

int centerObject(ObjData *data_obj) {
  ResultCode code = RESULT_OK;
  if (data_obj == NULL) {
    code = RESULT_ERROR;
  } else {
    double min_X = 100.0;
    double max_X = -100.0;
    double min_Y = 100.0;
    double max_Y = -100.0;
    double min_Z = 100.0;
    double max_Z = -100.0;

    if (data_obj->max_verter > 1.0) {
      for (int i = 0; i < data_obj->vertex_count * 3; i += 3) {
        data_obj->vertex_array[i] /= data_obj->max_verter;
        data_obj->vertex_array[i + 1] /= data_obj->max_verter;
        data_obj->vertex_array[i + 2] /= data_obj->max_verter;

        min_X = fmin(min_X, data_obj->vertex_array[i]);
        max_X = fmax(max_X, data_obj->vertex_array[i]);
        min_Y = fmin(min_Y, data_obj->vertex_array[i + 1]);
        max_Y = fmax(max_Y, data_obj->vertex_array[i + 1]);
        min_Z = fmin(min_Z, data_obj->vertex_array[i + 2]);
        max_Z = fmax(max_Z, data_obj->vertex_array[i + 2]);
      }
    }

    double mid_X = (min_X + max_X) / 2;
    double mid_Y = (min_Y + max_Y) / 2;
    double mid_Z = (min_Z + max_Z) / 2;

    double scale_factor = 2;
    for (int i = 0; i < data_obj->vertex_count * 3; i += 3) {
      data_obj->vertex_array[i] =
          (data_obj->vertex_array[i] - mid_X) * scale_factor;
      data_obj->vertex_array[i + 1] =
          (data_obj->vertex_array[i + 1] - mid_Y) * scale_factor;
      data_obj->vertex_array[i + 2] =
          (data_obj->vertex_array[i + 2] - mid_Z) * scale_factor;
    }
  }
  return code;
}

void removeData(ObjData *data_obj) {
  free(data_obj->vertex_array);
  free(data_obj->facet_array);
  data_obj->vertex_array = NULL;
  data_obj->facet_array = NULL;
}

// int main() {
//   ObjData test_obj;

//   parserObj("box.obj", &test_obj);
//   for (int i = 0; i < 6; i++) {
//     printf("%f ", test_obj.vertex_array[i]);
//   }
//   printf("\n");
//   printf("%d ", test_obj.facet_count);
//   printf("%d ", test_obj.vertex_count);
//   printf("\n");
//   for (int i = 0; i < 48; i++) {
//     printf("%d ", test_obj.facet_array[i]);
//   }
//   printf("\n");

//   removeData(&test_obj);

//   return 0;
// }
