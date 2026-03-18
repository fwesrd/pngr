#pragma once
#ifndef _CONST_H_
#define _CONST_H_

#include "C:\\MyDirC\\Code\\cpp\\usetool\\usetool7.1\\usetool.h"

namespace NConst
{
    //body
    static constexpr UINT bodyMaxCount{ 10U };
    static constexpr IWinUI::ObjectType body_type{ IWinUI::ObjectType::TRIANGLEED };
    static constexpr IWinUI::Index body_sphere_fineness{ 20U };
    static constexpr IWinUI::ObjectType trace_type{ IWinUI::ObjectType::POINTED };
    static constexpr IWinUI::Color trace_color{ 1.F, 1.F, 0.F, 1.F };
    static constexpr unsigned int trace_maxcount{ 500U };

    //particle
    static constexpr UINT particleMaxCount{ 30U };
    static constexpr IWinUI::ObjectType particle_type{ IWinUI::ObjectType::POINTED };
    static constexpr IWinUI::Color particle_color{ 1.F, 1.F, 1.F, 1.F };
    static constexpr UINT particle_init_count{ 0U }; //total = (count * 2 + 1) ^ 3
    static constexpr float particle_init_distance{ 0.5F };
    static constexpr float particle_init_offset_x{ 0.F };
    static constexpr float particle_init_offset_y{ 0.F };
    static constexpr float particle_init_offset_z{ 0.F };

    //black hole
    static constexpr IWinUI::Index disk_fineness{ 100U };
    static constexpr IWinUI::Color disk_color{ 1.F, 0.8F, 0.6F, 1.F };
    static constexpr float disk_rate_r_min{ 6.F };
    static constexpr float disk_rate_r_max{ 20.F };
    static constexpr float disk_rate_h_min{ 0.01F };
    static constexpr float disk_rate_h_max{ 0.01F };
    static constexpr double sun_mass{ 1.9891E30 };

    //light
    static constexpr unsigned int light_init_step_count{ 150U };
    static constexpr float light_init_step_min{ 1E6F };
    static constexpr float light_init_step_max{ 1E7F };

    //sky
    static constexpr long long sky_init_density{ 20ULL };
    static constexpr UINT sky_width{ 2000U };
    static constexpr UINT sky_height{ 2000U };
}

#endif // !_CONST_H_