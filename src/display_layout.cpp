#include "display_layout.h"
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

using Display = GxEPD2_7C<GxEPD2_730c_GDEP073E01, GxEPD2_730c_GDEP073E01::HEIGHT / 4>;

static const int SCREEN_W = 800;
static const int SCREEN_H = 480;
static const int HEADER_H = 44;
static const int COL_HEADER_H = 30;
static const int ROW_H = 52;  // two lines per recipe row
static const int LEFT_MARGIN = 12;
static const int COL_GRIND_X = 380;
static const int COL_TEMP_X = 540;
static const int COL_RECIPE_X = 640;

static void formatTime(int seconds, char *buf, size_t len) {
    int m = seconds / 60;
    int s = seconds % 60;
    snprintf(buf, len, "%d:%02d", m, s);
}

static void drawHeader(Display &display) {
    display.fillRect(0, 0, SCREEN_W, HEADER_H, GxEPD_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(LEFT_MARGIN, 30);
    display.print("BeanConqueror");
}

static void drawColumnHeaders(Display &display) {
    int y = HEADER_H + 4;
    display.fillRect(0, HEADER_H, SCREEN_W, COL_HEADER_H, GxEPD_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(GxEPD_BLACK);

    display.setCursor(LEFT_MARGIN, y + 20);
    display.print("ROASTER / BEAN");
    display.setCursor(COL_GRIND_X, y + 20);
    display.print("GRIND");
    display.setCursor(COL_TEMP_X, y + 20);
    display.print("TEMP");
    display.setCursor(COL_RECIPE_X, y + 20);
    display.print("RECIPE");

    // Divider below headers
    display.drawLine(LEFT_MARGIN, HEADER_H + COL_HEADER_H, SCREEN_W - LEFT_MARGIN,
                     HEADER_H + COL_HEADER_H, GxEPD_BLACK);
}

static void drawRecipeRow(Display &display, const BeanRecipe &recipe, int rowIndex) {
    int yBase = HEADER_H + COL_HEADER_H + 4 + rowIndex * ROW_H;

    // Line 1: Roaster + brew params (bold)
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(LEFT_MARGIN, yBase + 18);
    display.print(recipe.roaster);

    // Grind: "18g @ 15"
    char grindBuf[32];
    snprintf(grindBuf, sizeof(grindBuf), "%.0fg @ %s", recipe.grind_weight, recipe.grind_size);
    display.setCursor(COL_GRIND_X, yBase + 18);
    display.print(grindBuf);

    // Temp
    char tempBuf[16];
    snprintf(tempBuf, sizeof(tempBuf), "%.0f°C", recipe.brew_temperature);
    display.setCursor(COL_TEMP_X, yBase + 18);
    display.print(tempBuf);

    // Recipe: "36g in 0:30"
    char timeBuf[16];
    formatTime(recipe.brew_time, timeBuf, sizeof(timeBuf));
    char recipeBuf[32];
    snprintf(recipeBuf, sizeof(recipeBuf), "%.0fg in %s", recipe.brew_beverage_quantity, timeBuf);
    display.setCursor(COL_RECIPE_X, yBase + 18);
    display.print(recipeBuf);

    // Line 2: Bean name (blue, regular)
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLUE);
    display.setCursor(LEFT_MARGIN + 8, yBase + 40);
    display.print(recipe.bean_name);

    // Divider
    display.drawLine(LEFT_MARGIN, yBase + ROW_H - 2, SCREEN_W - LEFT_MARGIN,
                     yBase + ROW_H - 2, GxEPD_BLACK);
}

void drawDisplay(Display &display, const DisplayData &data) {
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        drawHeader(display);
        drawColumnHeaders(display);

        for (int i = 0; i < data.recipe_count; i++) {
            drawRecipeRow(display, data.recipes[i], i);
        }
    } while (display.nextPage());
}

void drawError(Display &display, const char *message) {
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeSansBold12pt7b);
        display.setTextColor(GxEPD_RED);
        display.setCursor(SCREEN_W / 2 - 100, SCREEN_H / 2);
        display.print(message);
    } while (display.nextPage());
}
