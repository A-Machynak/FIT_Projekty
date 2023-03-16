#include <stdio.h>
#include <string.h>
#include <ssd1306.h>
#include <sht3x/sht3x.h>

#include "display_imgs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "driver/i2c.h"

#define SSD1306_SCL_GPIO 17
#define SSD1306_SDA_GPIO 16

typedef struct
{
	SSD1306_t * Display;     ///< Displej
	sht3x_sensor_t * Sensor; ///< Temp/hum. senzor
	bool SwitchView;         ///< Jestli je zobrazena teplota nebo vlhkost
	float Temperature;
	float Humidity;
} ctx_t;

ctx_t * init_components()
{
	ctx_t * ctx = malloc(sizeof(ctx_t));
	if (!ctx) {
		return NULL;
	}

	ctx->Display = malloc(sizeof(SSD1306_t));
	if (!ctx->Display) {
		free(ctx);
		return NULL;
	}

	// inicializace i2c
	i2c_master_init(ctx->Display, SSD1306_SDA_GPIO, SSD1306_SCL_GPIO, -1);

	/*** Inicializace displeje ***/
	ssd1306_init(ctx->Display, 128, 64);
	ssd1306_clear_screen(ctx->Display, false);
	ssd1306_contrast(ctx->Display, 0xff);

	/*** Inicializace sensoru ***/
	ctx->Sensor = sht3x_init_sensor(0, SHT3x_ADDR_1);
	if (!ctx->Sensor) {
		free(ctx->Display);
		free(ctx);
		return NULL;
	}

	return ctx;
}

void display_temperature_icon(SSD1306_t * display)
{
	ssd1306_bitmaps(display, 92, 16, temperature_icon_img, 32, 48, false);
}

void display_humidity_icon(SSD1306_t * display)
{
	ssd1306_bitmaps(display, 92, 16, humidity_icon_img, 32, 48, false);
}

void display_temp(ctx_t * ctx)
{
	char buff[16];
	int len = sprintf(buff, "    %.2fC ", ctx->Temperature);

	display_temperature_icon(ctx->Display);
	ssd1306_display_text(ctx->Display, 0, "  Temperature   ", 16, false);
	ssd1306_display_text(ctx->Display, 4, buff, len, false);
}

void display_humidity(ctx_t * ctx)
{
	char buff[16];
	int len = sprintf(buff, "    %.2f%% ", ctx->Humidity);

	display_humidity_icon(ctx->Display);
	ssd1306_display_text(ctx->Display, 0, "    Humidity    ", 16, false);
	ssd1306_display_text(ctx->Display, 4, buff, len, false);
}

void display_switch_task(void *pvParameters)
{
	ctx_t * ctx = (ctx_t*)pvParameters;

	while(1) {
		vTaskDelay(5000 / portTICK_RATE_MS);
		// ctx->SwitchView = !ctx->SwitchView; switchne pouze 1 bit
		if (ctx->SwitchView) {
			ctx->SwitchView = false;
		}
		else {
			ctx->SwitchView = true;
		}
	}
}

void sensor_task(void *pvParameters)
{
	ctx_t * ctx = (ctx_t*)pvParameters;

	while (1) {
		if (!sht3x_measure(ctx->Sensor, &ctx->Temperature, &ctx->Humidity)) {
			// error při měření
			ctx->Temperature = -1.0;
			ctx->Humidity = -1.0;
		}

		// Update displeje
		if (ctx->SwitchView) {
			display_temp(ctx);
		}
		else {
			display_humidity(ctx);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void app_main()
{
	// Inicializace nevolatilní paměti
	ESP_ERROR_CHECK(nvs_flash_init());

	ctx_t * ctx = init_components();
	if (ctx != NULL) {
		xTaskCreate(sensor_task, "sensor_task", configMINIMAL_STACK_SIZE + 2048, ctx, 5, NULL);
		xTaskCreate(display_switch_task, "display_task", configMINIMAL_STACK_SIZE + 2048, ctx, 5, NULL);
	}
}