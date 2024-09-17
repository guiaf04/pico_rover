#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "web.h"
#include "ultrasonic.h"
#include "task.h"
#include "queue.h"
#include "wandering.h"

#define QUEUE_LENGTH 10

QueueHandle_t ultrasonic_queue;

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

int main() {
    stdio_init_all();
	//web_setup();

    ultrasonic_queue = xQueueCreate(QUEUE_LENGTH, sizeof(double));

    xTaskCreate(ultrasonic_task, "UltrasonicTask", 256, NULL, 1, NULL);
    xTaskCreate(wandering_task, "WanderingTask", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    // Code should never reach here
    while(1);
}
