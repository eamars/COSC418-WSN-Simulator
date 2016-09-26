#ifndef MISC_H_
#define MISC_H_

double ratio_to_db(double gain);
double db_to_ratio(double db);

// complementary error function
// ref: http://stackoverflow.com/a/18786808/4444357
double cef(double x);

#endif
