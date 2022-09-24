generate_rules_for_shader("shader_benchmarks_compute_filter_3"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter3.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")

generate_rules_for_shader("shader_benchmarks_compute_filter_5"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter5.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")

generate_rules_for_shader("shader_benchmarks_compute_filter_7"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/ComputeFilter7.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")

generate_rules_for_shader("shader_benchmarks_passthrough_pos"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/PassThroughPos.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_benchmarks_compute_buffer_increment"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/ComputeBufferIncrement.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")

generate_rules_for_shader("shader_benchmarks_texture_sample"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/TextureSample.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_benchmarks_texture_sample_explicit_early_z"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/TextureSample_ExplicitEarlyZ.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_benchmarks_multiple_rt"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/MultipleRT.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_benchmarks_texture_load"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/TextureLoad.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_benchmarks_texture_load_4_textures"
    SOURCE "${PPX_DIR}/assets/benchmarks/shaders/TextureLoad4Textures.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")
