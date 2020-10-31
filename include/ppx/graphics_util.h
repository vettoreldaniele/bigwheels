#ifndef ppx_graphics_util_h
#define ppx_graphics_util_h

#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_texture.h"
#include "ppx/fs.h"
#include "ppx/geometry.h"

namespace ppx {

/*
 
Cross Horizontal Left:
           _____
          |  0  |
     _____|_____|_____ _____
    |  1  |  2  |  3  |  4  |
    |_____|_____|_____|_____|
          |  5  |
          |_____|

Cross Horizontal Right:
                 _____
                |  0  |
     ___________|_____|_____ 
    |  1  |  2  |  3  |  4  |
    |_____|_____|_____|_____|
                |  5  |
                |_____|

Cross Vertical Top:
           _____
          |  0  |
     _____|_____|_____ 
    |  1  |  2  |  3  |
    |_____|_____|_____|
          |  4  |
          |_____|
          |  5  |
          |_____|

Cross Vertical Bottom:
           _____
          |  0  |
          |_____|
          |  1  |
     _____|_____|_____
    |  2  |  3  |  4  |
    |_____|_____|_____|
          |  5  |
          |_____|

Lat Long Horizontal:
     _____ _____ _____ 
    |  0  |  1  |  2  |
    |_____|_____|_____|
    |  3  |  4  |  5  |
    |_____|_____|_____|

Lat Long Vertical:
     _____ _____ 
    |  0  |  1  |
    |_____|_____|
    |  2  |  3  |
    |_____|_____|
    |  4  |  5  |
    |_____|_____|

Strip Horizontal:
     _____ _____ _____ _____ _____ _____ 
    |  0  |  1  |  2  |  3  |  4  |  5  |
    |_____|_____|_____|_____|_____|_____|


Strip Vertical:
     _____ 
    |  0  |
    |_____|
    |  1  |
    |_____|
    |  2  |
    |_____|
    |  3  |
    |_____|
    |  4  |
    |_____|
    |  5  |
    |_____|

*/
enum CubeImageLayout
{
    CUBE_IMAGE_LAYOUT_UNDEFINED              = 0,
    CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL_LEFT  = 1,
    CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL_RIGHT = 2,
    CUBE_IMAGE_LAYOUT_CROSS_VERTICAL_TOP     = 3,
    CUBE_IMAGE_LAYOUT_CROSS_VERTICAL_BOTTOM  = 4,
    CUBE_IMAGE_LAYOUT_LAT_LONG_HORIZONTAL    = 5,
    CUBE_IMAGE_LAYOUT_LAT_LONG_VERTICAL      = 6,
    CUBE_IMAGE_LAYOUT_STRIP_HORIZONTAL       = 7,
    CUBE_IMAGE_LAYOUT_STRIP_VERTICAL         = 8,
    CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL       = CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL_LEFT,
    CUBE_IMAGE_LAYOUT_CROSS_VERTICAL         = CUBE_IMAGE_LAYOUT_CROSS_VERTICAL_TOP,
    CUBE_IMAGE_LAYOUT_LAT_LONG               = CUBE_IMAGE_LAYOUT_LAT_LONG_HORIZONTAL,
    CUBE_IMAGE_LAYOUT_STRIP                  = CUBE_IMAGE_LAYOUT_STRIP_HORIZONTAL,
};

//! @enum CubeImageLoadOp
//!
//! Rotation is always clockwise.
//!
enum CubeFaceOp
{
    CUBE_FACE_OP_NONE              = 0,
    CUBE_FACE_OP_ROTATE_90         = 1,
    CUBE_FACE_OP_ROTATE_180        = 2,
    CUBE_FACE_OP_ROTATE_270        = 3,
    CUBE_FACE_OP_INVERT_HORIZONTAL = 4,
    CUBE_FACE_OP_INVERT_VERTICAL   = 5,
};

//! @struct CubeMapCreateInfo
//!
//! See enum CubeImageLayout for explanation of layouts
//!
//! Example - Use subimage 0 with 90 degrees CW rotation for posX face:
//!   layout = CUBE_IMAGE_LAYOUT_CROSS_HORIZONTAL;
//!   posX   = PPX_ENCODE_CUBE_FACE(0, CUBE_FACE_OP_ROTATE_90, :CUBE_FACE_OP_NONE);
//!
#define PPX_CUBE_OP_MASK           0xFF
#define PPX_CUBE_OP_SUBIMAGE_SHIFT 0
#define PPX_CUBE_OP_OP1_SHIFT      8
#define PPX_CUBE_OP_OP2_SHIFT      16

#define PPX_ENCODE_CUBE_FACE(SUBIMAGE, OP1, OP2)              \
    (SUBIMAGE & PPX_CUBE_OP_MASK) |                           \
        ((OP1 & PPX_CUBE_OP_MASK) << PPX_CUBE_OP_OP1_SHIFT) | \
        ((OP1 & PPX_CUBE_OP_MASK) << PPX_CUBE_OP_OP2_SHIFT)

#define PPX_DECODE_CUBE_FACE_SUBIMAGE(FACE) (FACE >> PPX_CUBE_OP_SUBIMAGE_SHIFT) & PPX_CUBE_OP_MASK
#define PPX_DECODE_CUBE_FACE_OP1(FACE)      (FACE >> PPX_CUBE_OP_OP1_SHIFT) & PPX_CUBE_OP_MASK
#define PPX_DECODE_CUBE_FACE_OP2(FACE)      (FACE >> PPX_CUBE_OP_OP2_SHIFT) & PPX_CUBE_OP_MASK

//! @struct CubeMapCreateInfo
//!
//!
struct CubeMapCreateInfo
{
    ppx::CubeImageLayout layout = ppx::CUBE_IMAGE_LAYOUT_UNDEFINED;
    uint32_t             posX   = PPX_VALUE_IGNORED;
    uint32_t             negX   = PPX_VALUE_IGNORED;
    uint32_t             posY   = PPX_VALUE_IGNORED;
    uint32_t             negY   = PPX_VALUE_IGNORED;
    uint32_t             posZ   = PPX_VALUE_IGNORED;
    uint32_t             negZ   = PPX_VALUE_IGNORED;
};

//! @fn CreateTextureFromFile
//!
//!
Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Image**                ppImage,
    const grfx::ImageUsageFlags& additionalImageUsage = grfx::ImageUsageFlags());

//! @fn CreateTextureFromFile
//!
//!
Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Texture**              ppTexture,
    const grfx::ImageUsageFlags& additionalImageUsage = grfx::ImageUsageFlags());

//! @fn CreateCubeMapFromFile
//!
//!
Result CreateCubeMapFromFile(
    grfx::Queue*                  pQueue,
    const fs::path&               path,
    const ppx::CubeMapCreateInfo* pCreateInfo,
    grfx::Image**                 ppImage,
    const grfx::ImageUsageFlags&  additionalImageUsage = grfx::ImageUsageFlags());

//! @fn CreateModelFromGeometry
//!
//!
Result CreateModelFromGeometry(
    grfx::Queue*    pQueue,
    const Geometry* pGeometry,
    grfx::Model**   ppMode);

} // namespace ppx

#endif // ppx_graphics_util_h
