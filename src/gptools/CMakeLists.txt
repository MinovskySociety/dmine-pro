add_executable(gptools "gptdef.h" "select.h" "select.cc" "fold_unfold.h" "fold_unfold.cc" "action.h" "yaml_config.h" "yaml_config.cc" "gptools.h" "gptools.cc")
target_link_libraries(gptools PRIVATE graph_package  ${YAML_LIBRARIES}  ${GFLAGS_LIBRARIES})
set_target_properties(gptools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/app)
