#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "web.h"
#include "ultrasonic.h"
#include "task.h"
#include "queue.h"
#include "wandering.h"
#include "ldr.h"
#include "dht11.h"
#include "rain.h"

#define QUEUE_LENGTH 10

QueueHandle_t ultrasonic_queue;

TaskHandle_t xDHT11TaskHandle;
TaskHandle_t xUltrasonicTaskHandle;
TaskHandle_t xWanderingTaskHandle;

void ultrasonic_task()
{
    ultrasonic_init();
    while(1)
    {
        double distance = ultrasonic_get_distance();
        if (xQueueSend(ultrasonic_queue, &distance, portMAX_DELAY) != pdPASS)
        {
            printf("[ERROR 0101]: Failed to send Ultrasonic Distance Data to respective queue!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void wandering_task()
{
    wandering_setup();
    wandering_loop(ultrasonic_queue);
}

void dht11_task()
{
    int temperature;
    int humidity;
    dht11_init();
    while(1)
    {
        if (dht11_read_data(&temperature, &humidity) == 0)
            printf("TEMPERATURE: %d, HUMIDITY: %d\n", temperature, humidity);
        else
            printf("[ERROR 0301]: Could not Read Data from DHT11\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ldr_task(void *pvParameters)
{
    ldr_init();
    uint16_t ldr_data;
    while(1)
    {
        ldr_data = ldr_read();
        printf("LDR Light Intensity: %d\n", ldr_data);
        if (ldr_data > 3800)        ldr_headlight_toggle(1);
        else                    ldr_headlight_toggle(0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void rain_task(void *pvParameters)
{
    rain_init();
    while(1)
    {
        if (rain_read() == 1)
        {
            printf("[ALERT] IT IS CURRENTLY RAINING!\n");
        }
    }
}

void pwm_test_task() {
    movement_init();
    movement_set_speed(47);
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Print the name of the task who overflowed
    printf("Stack overflow detected in task: %s\n", pcTaskName);
}

int main() {
    stdio_init_all();
    movement_set_speed(100);
	//web_setup();
    //printf("WEB successfully setup\n");

    ultrasonic_queue = xQueueCreate(QUEUE_LENGTH, sizeof(double));
    //
    xTaskCreate(dht11_task, "dht11Task", 256, NULL, 1, &xDHT11TaskHandle);
    xTaskCreate(wandering_task, "WanderingTask", 1024, NULL, 1, &xWanderingTaskHandle);
    xTaskCreate(ultrasonic_task, "UltrasonicTask", 1024, NULL, 1, &xUltrasonicTaskHandle);

    //xTaskCreate(pwm_test_task, "pwmTestTask", 256, NULL, 1, NULL);
    //xTaskCreate(web_test_task, "webTestTask", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    // Code should never reach here
    while(1);
}
