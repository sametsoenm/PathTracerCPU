add_executable(PathTracer
    "main.cpp"
    "framebuffer/framebuffer.cpp"
    "output/image_output.h" 
    "output/image_output.cpp" 
    "core/ray.h" 
    "camera/camera.h" "camera/camera.cpp" 
    "rendersettings.h" 
    "renderer/renderer.h" "renderer/renderer.cpp" 
    "intersectables/sphere.h" "intersectables/sphere.cpp" 
    "intersectables/intersectable.h"  
    "intersectables/intersectable_list.h" "intersectables/intersectable_list.cpp" 
    "scene/scene.h" 
     
    "material/material.h"  
    "util/onb.h" 
    "util/random.h" 
    "util/sampling.h" 
    "util/constants.h" 
    "material/lambert_diffuse.h" "material/lambert_diffuse.cpp" 
    "material/mirror.cpp" 
    "renderer/pbr_renderer/tracer.cpp" 
    "renderer/pbr_renderer/closest_hit.cpp" 
    "renderer/pbr_renderer/miss.cpp" 
    "renderer/pbr_renderer/volume_event.cpp" 
    "intersectables/triangle.h" "intersectables/triangle.cpp" 
    "scene/scene.cpp" 
    "material/diffuse_emissive.h" 
    "light/light.h" "light/light.cpp" 
     
    "material/specular_microfacet.h" "material/specular_microfacet.cpp" 
    "material/dielectric.h" "material/dielectric.cpp" 
    "core/intersection.h" 
    "util/texture.h" "util/texture.cpp" 
    "app/path_tracer_app.h" "app/path_tracer_app.cpp" 
    "platform/window.h" "platform/window.cpp" 
    "platform/display.h" 
    "platform/shader_program.h" 
    "platform/texture2d.h" 
    "platform/screen_quad.h" 
    "platform/screen_quad.cpp" 
    "platform/shader_program.cpp" 
    "platform/texture2d.cpp" 
    "platform/display.cpp" 
    "util/image.h" 
    "gui/gui_layer.h" "gui/gui_layer.cpp"
    "gui/imgui.cpp"
    "gui/imgui_draw.cpp"
    "gui/imgui_tables.cpp"
    "gui/imgui_widgets.cpp"
    "gui/imgui_demo.cpp"
    "gui/imgui_internal.h" 
    "gui/backends/imgui_impl_glfw.cpp"
    "gui/backends/imgui_impl_opengl3.cpp"
 "renderer/simple_renderer/closest_hit.cpp" "renderer/simple_renderer/miss.cpp" "renderer/simple_renderer/tracer.cpp"  "util/aabb.h" "intersectables/bvh.h" "intersectables/bvh.cpp" "material/material_funcs.h" "scene/load_obj.cpp" "material/rough_dielectric.h" "material/rough_dielectric.cpp")

target_include_directories(PathTracer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(PathTracer 
    PRIVATE 
        glm
        glfw
        glad)


if (MSVC)
    target_compile_options(PathTracer PRIVATE /W4 /permissive- /EHsc)
    target_compile_definitions(PathTracer PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    target_compile_options(PathTracer PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    target_link_libraries(PathTracer PRIVATE opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    target_link_libraries(PathTracer PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(PathTracer PRIVATE OpenGL::GL)
endif()

add_custom_command(TARGET PathTracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:PathTracer>/assets
)