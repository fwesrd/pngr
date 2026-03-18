#include "global.h"

namespace NGlobal
{
    //scale
    float scale_time{ 1.F };
    float scale_distance{ 1E8F };
    float scale_distance_old{ 1.F };

    //time
    float time_calculate_rate{ 0.001F };
    float time_calculate_step{ NGlobal::time_calculate_rate * NGlobal::scale_time };
}