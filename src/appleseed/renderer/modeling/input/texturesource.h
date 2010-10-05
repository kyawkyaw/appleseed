
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010 Francois Beaune
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef APPLESEED_RENDERER_MODELING_INPUT_TEXTURESOURCE_H
#define APPLESEED_RENDERER_MODELING_INPUT_TEXTURESOURCE_H

// appleseed.renderer headers.
#include "renderer/global/global.h"
#include "renderer/modeling/input/source.h"
#include "renderer/modeling/scene/textureinstance.h"

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/colorspace.h"

// Forward declarations.
namespace renderer      { class InputParams; }
namespace renderer      { class TextureCache; }
namespace renderer      { class TextureInstance; }

namespace renderer
{

//
// Texture source.
//

class TextureSource
  : public Source
{
  public:
    // Constructor.
    TextureSource(
        const foundation::UniqueID          assembly_uid,
        const TextureInstance&              texture_instance,
        const foundation::CanvasProperties& texture_props);

    // Evaluate the source at a given shading point.
    virtual void evaluate(
        TextureCache&                       texture_cache,
        const InputParams&                  params,
        double&                             scalar);
    virtual void evaluate(
        TextureCache&                       texture_cache,
        const InputParams&                  params,
        foundation::Color3f&                linear_rgb,
        Alpha&                              alpha);
    virtual void evaluate(
        TextureCache&                       texture_cache,
        const InputParams&                  params,
        Spectrum&                           spectrum,
        Alpha&                              alpha);

  private:
    const foundation::UniqueID              m_assembly_uid;
    const size_t                            m_texture_index;
    const TextureAddressingMode             m_addressing_mode;
    const TextureFilteringMode              m_filtering_mode;
    const float                             m_multiplier;
    const foundation::LightingConditions    m_lighting_conditions;
    const foundation::CanvasProperties      m_texture_props;
    const double                            m_scalar_canvas_width;
    const double                            m_scalar_canvas_height;
    const double                            m_max_x;
    const double                            m_max_y;

    // Retrieve a given texel. Return a color in the linear RGB color space.
    foundation::Color4f get_texel(
        TextureCache&                       texture_cache,
        const size_t                        ix,
        const size_t                        iy) const;

    // Retrieve a 2x2 block of texels. Texels are expressed in the linear RGB color space.
    void get_texels_2x2(
        TextureCache&                       texture_cache,
        const int                           ix,
        const int                           iy,
        foundation::Color4f&                t00,
        foundation::Color4f&                t10,
        foundation::Color4f&                t01,
        foundation::Color4f&                t11) const;

    // Sample the texture. Return a color in the linear RGB color space.
    foundation::Color4f sample_texture(
        TextureCache&                       texture_cache,
        const InputParams&                  params) const;
};


//
// TextureSource class implementation.
//

inline void TextureSource::evaluate(
    TextureCache&                           texture_cache,
    const InputParams&                      params,
    double&                                 scalar)
{
    foundation::Color4f color =
        sample_texture(texture_cache, params);

    color[0] *= m_multiplier;

    scalar = static_cast<double>(color[0]);
}

inline void TextureSource::evaluate(
    TextureCache&                           texture_cache,
    const InputParams&                      params,
    foundation::Color3f&                    linear_rgb,
    Alpha&                                  alpha)
{
    const foundation::Color4f color =
        sample_texture(texture_cache, params);

    linear_rgb = color.rgb();
    linear_rgb *= m_multiplier;

    alpha.set(color.a);
}

inline void TextureSource::evaluate(
    TextureCache&                           texture_cache,
    const InputParams&                      params,
    Spectrum&                               spectrum,
    Alpha&                                  alpha)
{
    const foundation::Color4f color =
        sample_texture(texture_cache, params);

    foundation::linear_rgb_to_spectrum(
        m_lighting_conditions,
        color.rgb(),
        spectrum);

    spectrum *= m_multiplier;

    alpha.set(color.a);
}

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_INPUT_TEXTURESOURCE_H
