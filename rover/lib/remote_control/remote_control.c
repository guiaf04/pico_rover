#include "remote_control.h"

void remote_control_init()
{
    gpio_init(MOVE_FORWARD_PIN);    gpio_set_dir(MOVE_FORWARD_PIN, GPIO_IN);
    gpio_init(ROTATE_LEFT_PIN);     gpio_set_dir(ROTATE_LEFT_PIN, GPIO_IN);
    gpio_init(ROTATE_RIGHT_PIN);    gpio_set_dir(ROTATE_RIGHT_PIN, GPIO_IN);
    gpio_init(STOP_PIN);            gpio_set_dir(STOP_PIN, GPIO_IN);
}

bool remote_control_control()
{
    return (gpio_get(MOVE_FORWARD_PIN) == 1 && gpio_get(STOP_PIN) == 1) ? true : false;
}