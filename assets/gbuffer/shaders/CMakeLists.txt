set(INCLUDE_FILES
    "${PPX_DIR}/assets/gbuffer/shaders/Config.hlsli"
    "${PPX_DIR}/assets/gbuffer/shaders/GBuffer.hlsli")

generate_rules_for_shader("shader_gbuffer_vertex_shader"
    SOURCE "${PPX_DIR}/assets/gbuffer/shaders/VertexShader.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs")

generate_rules_for_shader("shader_gbuffer_deferred_light"
    SOURCE "${PPX_DIR}/assets/gbuffer/shaders/DeferredLight.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_gbuffer_draw_attributes"
    SOURCE "${PPX_DIR}/assets/gbuffer/shaders/DrawGBufferAttribute.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_gbuffer_deferred_render"
    SOURCE "${PPX_DIR}/assets/gbuffer/shaders/DeferredRender.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "ps")

add_custom_target("shader_gbuffer"
    DEPENDS
    "shader_gbuffer_vertex_shader"
    "shader_gbuffer_deferred_light"
    "shader_gbuffer_draw_attributes"
    "shader_gbuffer_deferred_render"
)
