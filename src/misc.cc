#include "misc.h"
#include <cmath>

double ratio_to_db(double gain)
{
    return 10 * log10(gain);
}

double db_to_ratio(double db)
{
    return pow(10, db/10);
}

