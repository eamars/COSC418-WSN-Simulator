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

double cef(double x)
{
    return 0.5 * erfc(-x * M_SQRT1_2);
}

