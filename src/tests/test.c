#include <check.h>

#include "../viewer.h"

void createTempObjFile() {
  FILE *file_ptr = fopen("temp_obj.obj", "w");
  if (file_ptr != NULL) {
    fprintf(file_ptr, "v 4.0 4.0 4.0\n");
    fprintf(file_ptr, "v 4.0 4.0 4.0\n");
    fprintf(file_ptr, "v 4.0 4.0 4.0\n");
    fprintf(file_ptr, "f 1 2 3\n");
    fclose(file_ptr);
  }
}

void removeTempObjFile() { remove("temp_obj.obj"); }

START_TEST(test_parser_and_centerObject) {
  createTempObjFile();
  ObjData test_obj;
  ResultCode code = parserObj("temp_obj.obj", &test_obj);
  ck_assert_msg(code == RESULT_OK, "Failed  parse ");
  code = centerObject(&test_obj);
  ck_assert_msg(code == RESULT_OK, "Failed to center ");
  for (int i = 0; i < test_obj.vertex_count * 3; i += 3) {
    ck_assert_double_eq(test_obj.vertex_array[i], 0);
    ck_assert_double_eq(test_obj.vertex_array[i + 1], 0);
    ck_assert_double_eq(test_obj.vertex_array[i + 2], 0);
  }
  removeData(&test_obj);
}
END_TEST

START_TEST(test_move_obj) {
  ObjData file_obj;
  file_obj.vertex_count = 3;
  file_obj.facet_count = 3;
  file_obj.vertex_array = (double *)malloc(3 * 3 * sizeof(double));
  file_obj.facet_array = (int *)malloc(3 * 2 * sizeof(int));

  file_obj.vertex_array[0] = 1.0;
  file_obj.vertex_array[1] = 2.0;
  file_obj.vertex_array[2] = 3.0;

  ResultCode code = move_obj(&file_obj, 1.0, 2.0, 3.0);
  ck_assert_msg(code == RESULT_OK, "FAILED move ");

  ck_assert_double_eq(file_obj.vertex_array[0], 2.0);
  ck_assert_double_eq(file_obj.vertex_array[1], 4.0);
  ck_assert_double_eq(file_obj.vertex_array[2], 6.0);

  free(file_obj.vertex_array);
  free(file_obj.facet_array);
}
END_TEST

START_TEST(test_scale_obj) {
  ObjData file_obj;
  file_obj.vertex_count = 3;
  file_obj.facet_count = 3;
  file_obj.vertex_array = (double *)malloc(3 * 3 * sizeof(double));
  file_obj.facet_array = (int *)malloc(3 * 2 * sizeof(int));

  file_obj.vertex_array[0] = 1.0;
  file_obj.vertex_array[1] = 2.0;
  file_obj.vertex_array[2] = 3.0;

  ResultCode code = scale_obj(&file_obj, 2.0);
  ck_assert_msg(code == RESULT_OK, "FAILED scale ");

  ck_assert_double_eq(file_obj.vertex_array[0], 2.0);
  ck_assert_double_eq(file_obj.vertex_array[1], 4.0);
  ck_assert_double_eq(file_obj.vertex_array[2], 6.0);

  free(file_obj.vertex_array);
  free(file_obj.facet_array);
}
END_TEST

START_TEST(test_rotate_obj) {
  double vertex_array[24] = {1.0, 1.0, 1.0, 0.5, 0.5, 2.0, 0.0, 2.0,
                             0.0, 0.5, 2.5, 2.0, 2.0, 0.0, 0.0, 2.5,
                             0.5, 2.0, 2.0, 2.0, 0.0, 2.5, 2.5, 2.0};

  double expected_vertex_array[24] = {
      1.207107,  1.207107, 0.292893,  1.883883, 0.383883, 0.896447,
      -0.292893, 1.707107, 1.000000,  1.590990, 2.090990, 1.896447,
      1.000000,  1.000000, -1.414214, 2.883883, 1.383883, -0.517767,
      0.707107,  2.707107, -0.414214, 2.590990, 3.090990, 0.482233};

  ObjData obj;
  obj.vertex_count = 8;
  obj.facet_count = 0;
  obj.vertex_array = vertex_array;

  rotate_obj(&obj, 45, 45, 45);

  for (int i = 0; i < obj.vertex_count * 3; i++) {
    ck_assert_msg(fabsl(expected_vertex_array[i] - obj.vertex_array[i]) < 1e-6,
                  "index %d: expected %f, actual %f", i,
                  expected_vertex_array[i], obj.vertex_array[i]);
  }
}
END_TEST

Suite *test_viewer(void) {
  Suite *suite = suite_create("Test Viewer");
  TCase *tCase = tcase_create("Test Parser");

  tcase_add_test(tCase, test_parser_and_centerObject);
  tcase_add_test(tCase, test_move_obj);
  tcase_add_test(tCase, test_scale_obj);
  tcase_add_test(tCase, test_rotate_obj);
  suite_add_tcase(suite, tCase);

  return suite;
}

int main() {
  Suite *suite = test_viewer();
  SRunner *sRunner = srunner_create(suite);
  srunner_set_fork_status(sRunner, CK_NOFORK);
  srunner_run_all(sRunner, CK_VERBOSE);
  int total_test = srunner_ntests_run(sRunner);
  int failed_tests = srunner_ntests_failed(sRunner);
  srunner_free(sRunner);
  printf("\nAll test: %d\n", total_test);
  printf("Fail test: %d\n", failed_tests);

  return (failed_tests == 0) ? 0 : 1;
}