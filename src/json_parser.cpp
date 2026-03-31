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

bool parseDisplayData(const char *recipesJson, DisplayData &out) {
    memset(&out, 0, sizeof(out));

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, recipesJson);
    if (err) {
        Serial.printf("JSON: parse error: %s\n", err.c_str());
        return false;
    }

    JsonArray arr = doc.as<JsonArray>();
    out.recipe_count = 0;

    for (JsonObject recipe : arr) {
        if (out.recipe_count >= MAX_RECIPES) break;
        BeanRecipe &r = out.recipes[out.recipe_count];
        safeStrCopy(r.bean_name, recipe["bean_name"], sizeof(r.bean_name));
        safeStrCopy(r.roaster, recipe["roaster"], sizeof(r.roaster));
        safeStrCopy(r.grind_size, recipe["grind_size"], sizeof(r.grind_size));
        safeStrCopy(r.ratio, recipe["ratio"], sizeof(r.ratio));
        safeStrCopy(r.preparation_name, recipe["preparation_name"], sizeof(r.preparation_name));
        r.grind_weight = recipe["grind_weight"] | 0.0f;
        r.brew_temperature = recipe["brew_temperature"] | 0.0f;
        r.brew_beverage_quantity = recipe["brew_beverage_quantity"] | 0.0f;
        r.brew_quantity = recipe["brew_quantity"] | 0.0f;
        r.brew_time = recipe["brew_time"] | 0;
        out.recipe_count++;
    }

    Serial.printf("JSON: parsed %d bean recipes\n", out.recipe_count);
    return out.recipe_count > 0;
}
