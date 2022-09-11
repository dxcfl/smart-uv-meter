/*
GUI.cpp

    Read more: https://https://github.com/dxcfl/uv-monitoring_wio-terminal#readme
    Git: https://github.com/dxcfl/uv-monitoring_wio-terminal
    (c) 2022 by dxcfl
*/

#include <lvgl.h>
#include <TFT_eSPI.h>

#define LVGL_TICK_PERIOD 5

static TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

/* add the following lines to lv_conf.h: */ /*

 // Fonts
 #define LV_FONT_MONTSERRAT_14 1
 #define LV_FONT_MONTSERRAT_16 1
 #define LV_FONT_MONTSERRAT_18 1
 #define LV_FONT_MONTSERRAT_22 1

// Miscellaneous
#define LV_SPRINTF_DISABLE_FLOAT 0

 */

class GUI
{
    /* GUI objects ... */
    inline static lv_obj_t *headline;
    inline static lv_obj_t *risk_meter;
    inline static lv_obj_t *risk_meter_label;
    inline static lv_obj_t *risk_label;
    inline static lv_obj_t *temp_label;
    inline static lv_obj_t *temp_value_label;
    inline static lv_obj_t *pressure_label;
    inline static lv_obj_t *press_value_label;
    inline static lv_obj_t *humidity_label;
    inline static lv_obj_t *humid_value_label;
    inline static lv_obj_t *uv_label;
    inline static lv_obj_t *uv_value_label;
    inline static lv_obj_t *uvi_label;
    inline static lv_obj_t *uvi_value_label;

public:
    /* Display flushing */
    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
        uint16_t c;

        tft.startWrite();                                                                            /* Start new TFT transaction */
        tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
        for (int y = area->y1; y <= area->y2; y++)
        {
            for (int x = area->x1; x <= area->x2; x++)
            {
                c = color_p->full;
                tft.writeColor(c, 1);
                color_p++;
            }
        }
        tft.endWrite();            /* terminate TFT transaction */
        lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
    }

    /* Interrupt driven periodic handler */
    static void lv_tick_handler(void)
    {
        lv_tick_inc(LVGL_TICK_PERIOD);
    }

    static void setup()
    {
        lv_init();

        tft.begin();        /* TFT init */
        tft.setRotation(3); /* Landscape orientation */

        lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

        /*Initialize the display*/
        lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = 320;
        disp_drv.ver_res = 240;
        disp_drv.flush_cb = my_disp_flush;
        disp_drv.buffer = &disp_buf;
        lv_disp_drv_register(&disp_drv);
    }

    static void screen()
    {
        lv_theme_t *theme = lv_theme_material_init(LV_COLOR_BLACK, LV_COLOR_WHITE, LV_THEME_MATERIAL_FLAG_DARK, &lv_font_montserrat_14, &lv_font_montserrat_16, &lv_font_montserrat_18, &lv_font_montserrat_22);
        lv_theme_set_act(theme);

        headline = lv_label_create(lv_scr_act(), NULL);
        risk_meter = lv_linemeter_create(lv_scr_act(), NULL);
        risk_meter_label = lv_label_create(lv_scr_act(), NULL);
        risk_label = lv_label_create(lv_scr_act(), NULL);
        temp_label = lv_label_create(lv_scr_act(), NULL);
        temp_value_label = lv_label_create(lv_scr_act(), NULL);
        pressure_label = lv_label_create(lv_scr_act(), NULL);
        press_value_label = lv_label_create(lv_scr_act(), NULL);
        humidity_label = lv_label_create(lv_scr_act(), NULL);
        humid_value_label = lv_label_create(lv_scr_act(), NULL);
        uv_label = lv_label_create(lv_scr_act(), NULL);
        uv_value_label = lv_label_create(lv_scr_act(), NULL);
        uvi_label = lv_label_create(lv_scr_act(), NULL);
        uvi_value_label = lv_label_create(lv_scr_act(), NULL);

        static lv_style_t large_style;
        lv_style_init(&large_style);
        lv_style_set_text_font(&large_style, LV_STATE_DEFAULT, lv_theme_get_font_title());

        lv_obj_add_style(headline, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(headline, 10); // 115
        lv_obj_set_y(headline, 10);
        lv_obj_set_height(headline, 20);
        lv_obj_set_width(headline, 50);
        lv_label_set_text(headline, "UV Exposure");

        lv_obj_set_x(risk_meter, 10);
        lv_obj_set_y(risk_meter, 75);
        lv_obj_set_width(risk_meter, 90);
        lv_obj_set_height(risk_meter, 90);
        lv_linemeter_set_range(risk_meter, 0, 4);
        lv_linemeter_set_scale(risk_meter, 240, 10);

        lv_obj_set_x(risk_meter_label, 40);
        lv_obj_set_y(risk_meter_label, 50);
        lv_obj_set_width(risk_meter_label, 60);
        lv_obj_set_height(risk_meter_label, 40);
        lv_label_set_text(risk_meter_label, "Risk");

        lv_obj_set_x(risk_label, 20);
        lv_obj_set_y(risk_label, 170);
        lv_obj_set_width(risk_label, 60);
        lv_obj_set_height(risk_label, 40);
        lv_label_set_text(risk_label, "");

        unsigned int line0_y = 50;
        unsigned int line_space = 40;
        unsigned int line = 0;

        lv_obj_set_x(temp_label, 115);
        lv_obj_set_y(temp_label, line0_y + line_space * line);
        lv_label_set_text(temp_label, "Temp (C°)");
        lv_obj_add_style(temp_value_label, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(temp_value_label, 245);
        lv_obj_set_y(temp_value_label, line0_y + line_space * line);
        lv_label_set_text(temp_value_label, "...");

        line++;
        lv_obj_set_x(pressure_label, 115);
        lv_obj_set_y(pressure_label, line0_y + line_space * line);
        lv_label_set_text(pressure_label, "Pressure (hPa)");
        lv_obj_add_style(press_value_label, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(press_value_label, 245);
        lv_obj_set_y(press_value_label, line0_y + line_space * line);
        lv_label_set_text(press_value_label, "...");

        line++;
        lv_obj_set_x(humidity_label, 115);
        lv_obj_set_y(humidity_label, line0_y + line_space * line);
        lv_label_set_text(humidity_label, "Humidity (%)");
        lv_obj_add_style(humid_value_label, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(humid_value_label, 245);
        lv_obj_set_y(humid_value_label, line0_y + line_space * line);
        lv_label_set_text(humid_value_label, "...");
        line++;
        lv_obj_set_x(uv_label, 115);
        lv_obj_set_y(uv_label, line0_y + line_space * line);
        lv_label_set_text(uv_label, "UV (uw/cm^2)");
        lv_obj_add_style(uv_value_label, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(uv_value_label, 245);
        lv_obj_set_y(uv_value_label, line0_y + line_space * line);
        lv_label_set_text(uv_value_label, "...");

        line++;
        lv_obj_set_x(uvi_label, 115);
        lv_obj_set_y(uvi_label, line0_y + line_space * line);
        lv_label_set_text(uvi_label, "UV Index");
        lv_obj_add_style(uvi_value_label, LV_LABEL_PART_MAIN, &large_style);
        lv_obj_set_x(uvi_value_label, 245);
        lv_obj_set_y(uvi_value_label, line0_y + line_space * line);
        lv_label_set_text(uvi_value_label, "...");
    }

    static void update()
    {
        lv_tick_handler();
        lv_task_handler(); /* let the GUI do its work */
        delay(5);
    }

    /* Updates the value labels ...
     */
    static void update_labels(float temperature, float pressure, float humidity, unsigned uv, unsigned uv_index)
    {
        lv_label_set_text_fmt(temp_value_label, "%.1f", temperature);
        lv_label_set_text_fmt(press_value_label, "%0.1f", pressure);
        lv_label_set_text_fmt(humid_value_label, "%0.1f", humidity);
        lv_label_set_text_fmt(uv_value_label, "%u", uv);
        lv_label_set_text_fmt(uvi_value_label, "%u", uv_index);
    }

    /* Updates the line meter and it's label  ...
     */
    static void update_meter(int8_t risk)
    {
        const char *rating[] = {"     low", "moderate", "    high", "very high", "extreme"};
        const lv_color_t bg_color[] = {LV_COLOR_GREEN, LV_COLOR_YELLOW, LV_COLOR_ORANGE, LV_COLOR_RED, LV_COLOR_PURPLE};
        if (risk > 4)
            return;
        lv_linemeter_set_value(risk_meter, risk);
        lv_label_set_text(risk_label, rating[risk]);
        lv_obj_set_style_local_bg_color(risk_meter, LV_LINEMETER_PART_MAIN, LV_STATE_DEFAULT, bg_color[risk]);
    }
};
