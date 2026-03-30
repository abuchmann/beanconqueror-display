#include "json_parser.h"
#include <ArduinoJson.h>
#include <string.h>

static void safeStrCopy(char *dest, const char *src, size_t maxLen) {
    if (src) {
        strncpy(dest, src, maxLen - 1);
        dest[maxLen - 1] = '\0';
    } else {
        dest[0] = '\0';
    }
}

bool parseDisplayData(const char *brewsJson, const char *beansJson, DisplayData &out) {
    memset(&out, 0, sizeof(out));

    // Parse active beans to build a roaster lookup
    ActiveBean activeBeans[MAX_ACTIVE_BEANS];
    int beanCount = 0;

    {
        JsonDocument beansDoc;
        DeserializationError err = deserializeJson(beansDoc, beansJson);
        if (err) {
            Serial.printf("JSON: beans parse error: %s\n", err.c_str());
            return false;
        }

        JsonArray arr = beansDoc.as<JsonArray>();
        for (JsonObject bean : arr) {
            if (beanCount >= MAX_ACTIVE_BEANS) break;
            safeStrCopy(activeBeans[beanCount].name, bean["name"], sizeof(ActiveBean::name));
            safeStrCopy(activeBeans[beanCount].roaster, bean["roaster"], sizeof(ActiveBean::roaster));
            beanCount++;
        }
    }

    // Parse recent brews
    RecentBrew recentBrews[MAX_RECENT_BREWS];
    int brewCount = 0;

    {
        JsonDocument brewsDoc;
        DeserializationError err = deserializeJson(brewsDoc, brewsJson);
        if (err) {
            Serial.printf("JSON: brews parse error: %s\n", err.c_str());
            return false;
        }

        JsonArray arr = brewsDoc["brews"].as<JsonArray>();
        for (JsonObject brew : arr) {
            if (brewCount >= MAX_RECENT_BREWS) break;
            RecentBrew &b = recentBrews[brewCount];
            safeStrCopy(b.bean_name, brew["bean_name"], sizeof(RecentBrew::bean_name));
            safeStrCopy(b.grind_size, brew["grind_size"], sizeof(RecentBrew::grind_size));
            b.grind_weight = brew["grind_weight"] | 0.0f;
            b.brew_temperature = brew["brew_temperature"] | 0.0f;
            b.brew_beverage_quantity = brew["brew_beverage_quantity"] | 0.0f;
            b.brew_time = brew["brew_time"] | 0;
            brewCount++;
        }
    }

    // Build recipes: most recent brew per bean (brews are already newest-first)
    out.recipe_count = 0;
    for (int i = 0; i < brewCount && out.recipe_count < MAX_RECIPES; i++) {
        const RecentBrew &brew = recentBrews[i];

        // Skip if we already have a recipe for this bean
        bool seen = false;
        for (int j = 0; j < out.recipe_count; j++) {
            if (strcmp(out.recipes[j].bean_name, brew.bean_name) == 0) {
                seen = true;
                break;
            }
        }
        if (seen) continue;

        BeanRecipe &recipe = out.recipes[out.recipe_count];
        safeStrCopy(recipe.bean_name, brew.bean_name, sizeof(BeanRecipe::bean_name));
        safeStrCopy(recipe.grind_size, brew.grind_size, sizeof(BeanRecipe::grind_size));
        recipe.grind_weight = brew.grind_weight;
        recipe.brew_temperature = brew.brew_temperature;
        recipe.brew_beverage_quantity = brew.brew_beverage_quantity;
        recipe.brew_time = brew.brew_time;

        // Look up roaster from active beans
        recipe.roaster[0] = '\0';
        for (int k = 0; k < beanCount; k++) {
            if (strcmp(activeBeans[k].name, brew.bean_name) == 0) {
                safeStrCopy(recipe.roaster, activeBeans[k].roaster, sizeof(BeanRecipe::roaster));
                break;
            }
        }

        out.recipe_count++;
    }

    Serial.printf("JSON: parsed %d recipes from %d brews and %d beans\n",
                  out.recipe_count, brewCount, beanCount);
    return out.recipe_count > 0;
}
