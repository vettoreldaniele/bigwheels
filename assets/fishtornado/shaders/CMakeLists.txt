set(INCLUDE_FILES
    "${PPX_DIR}/assets/fishtornado/shaders/Config.hlsli"
    "${PPX_DIR}/assets/fishtornado/shaders/Lighting.hlsli")

generate_rules_for_shader("shader_debug_draw"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/DebugDraw.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_flocking_render"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/FlockingRender.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_ocean_beam"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/OceanBeam.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_ocean_floor"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/OceanFloor.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_ocean_surface"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/OceanSurface.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_shark"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/Shark.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs" "ps")

generate_rules_for_shader("shader_flocking_shadow"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/FlockingShadow.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs")

generate_rules_for_shader("shader_shark_shadow"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/SharkShadow.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "vs")

generate_rules_for_shader("shader_flocking_position"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/FlockingPosition.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")

generate_rules_for_shader("shader_flocking_velocity"
    SOURCE "${PPX_DIR}/assets/fishtornado/shaders/FlockingVelocity.hlsl"
    INCLUDES ${INCLUDE_FILES}
    STAGES "cs")


add_custom_target("shader_gbuffer"
    DEPENDS
    "shader_debug_draw"
    "shader_flocking_render"
    "shader_ocean_beam"
    "shader_ocean_floor"
    "shader_ocean_surface"
    "shader_shark"
    "shader_flocking_shadow"
    "shader_shark_shadow"
    "shader_flocking_position"
    "shader_flocking_velocity"
)
