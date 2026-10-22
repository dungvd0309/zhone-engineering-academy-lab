#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BTN_PRESSED '0'
#define BTN_RELEASED '1'

char led_device_name[] = "/dev/dungvd_led";
char btn_device_name[] = "/dev/dungvd_btn";

char instructions[][4] = {"1000", "0100", "0010", "0001", "0010", "0100"};

int main()
{   
    int led_fd = open(led_device_name, O_WRONLY);

    if (led_fd < 0) {
        perror("led open");
        return 1;
    }

    int btn_fd = open(btn_device_name, O_RDONLY);

    if (btn_fd < 0) {
        perror("btn open");
        return 1;
    }

    int ins_size = sizeof(instructions) / 4;

    int i = 0;

    char btn_state[3];

    while (1)
    {
        ssize_t n = read(btn_fd, &btn_state, 3);
            
        if (n < 0) {
            perror("read");
            break;
        }

        usleep(100000);

        if (btn_state[0] == BTN_PRESSED) {
            printf("Button pressed, sending instruction: %s\n", instructions[i]);
            write(led_fd, instructions[i], 4);
            i = (i + 1) % ins_size;

            do {
                if (read(btn_fd, &btn_state, 1) != 1) {
                    perror("read button");
                    goto cleanup;
                }
            } while (btn_state[0] == BTN_PRESSED);
        }
    }

cleanup:
    close(led_fd);
    close(btn_fd);
}