#pragma once

#include <stdint.h>

#define MAX_RECIPES 10
#define MAX_RECENT_BREWS 20
#define MAX_ACTIVE_BEANS 10

struct RecentBrew {
    char bean_name[64];
    char grind_size[16];
    float grind_weight;
    float brew_temperature;
    float brew_beverage_quantity;
    int brew_time;  // seconds
};

struct ActiveBean {
    char name[64];
    char roaster[64];
};

// One row on the display: a bean with its most recent brew recipe
struct BeanRecipe {
    char roaster[64];
    char bean_name[64];
    char grind_size[16];
    float grind_weight;
    float brew_temperature;
    float brew_beverage_quantity;
    int brew_time;  // seconds
};

struct DisplayData {
    BeanRecipe recipes[MAX_RECIPES];
    int recipe_count;
};
