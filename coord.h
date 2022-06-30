
#include<iostream>
#include<vector>
#include "cell.h"

#ifndef coord_class
#define coord_class
class coord 
{
    private:
        size_t x_coord;
        size_t y_coord;
    public:
        coord(size_t x, size_t y) : x_coord{x}, y_coord{y} {};
        size_t x() {return x_coord;};
        size_t y() {return y_coord;};
};

#endif