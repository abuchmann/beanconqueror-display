#pragma once

#include <stdint.h>

#define MAX_RECIPES 10

// One row on the display: a bean with its most recent brew recipe
struct BeanRecipe {
    char roaster[64];
    char bean_name[64];
    char grind_size[16];
    float grind_weight;
    float brew_temperature;
    float brew_beverage_quantity;
    float brew_quantity;
    int brew_time;  // seconds
    char ratio[16];
    char preparation_name[32];
};

struct DisplayData {
    BeanRecipe recipes[MAX_RECIPES];
    int recipe_count;
};
