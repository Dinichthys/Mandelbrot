vertices [ver_index].position = {(float) x_check + (float) point_index, (float) y_check};

vertices [ver_index].color.r = iteration_stop [point_index] - point_index;
vertices [ver_index].color.g = iteration_stop [point_index] + point_index;
vertices [ver_index].color.b = iteration_stop [point_index];
