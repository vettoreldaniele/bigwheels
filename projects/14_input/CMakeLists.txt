cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(14_input)

add_samples_for_all_apis(
    SAMPLE_NAME ${PROJECT_NAME}
    ADDITIONAL_FILES ${HLSL_FILES}
)
