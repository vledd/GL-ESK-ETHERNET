#include "stdio.h"
#include "stdlib.h"
//#include <stdint.h>
//#include <stdbool.h>
#include "string.h"
//#include <unistd.h>
#include "sys/types.h"
#include "sys/socket.h"
#include "main.h"
#include "cmsis_os.h"

#define PORT 5678U
#define BUFFER_SIZE 1024
#define LED_OFFSET 3
#define ASCII_NULL_OFFSET 48
#define MAX_STRING_SIZE 24
#define MAX_TOKENS_NO 2


// Socket descriptor
int sockfd;
// Socket structs
struct sockaddr_in server_addr = { 0, };
struct sockaddr_in client_addr = { 0, };
socklen_t client_len;
char* buffer = NULL;
char tokens[MAX_TOKENS_NO][MAX_STRING_SIZE] = { 0, };
const char status_info[] = "udp_srv_vladyslav_nechaiev_06052023\r\n";

static int init_udp_server(void)
{
    buffer = (char*)calloc(BUFFER_SIZE, 1);
    // Try to initialize socket
    // AF == IPv4 pool, DGRAM == UDP, 0 - automatic mode
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // In case of errors brk
    if (sockfd < 0)
    {
        printf("Error creating socket\n");
        return -1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET; // IPv4 pool
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any address free
    server_addr.sin_port = htons(PORT);

    // Bind socket to struct params and check for errors
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error binding socket\n");
        return -1;
    }

    printf("UDP Started on %d\n", PORT);
    return 0;
}

void StartUdpServerTask(void const * argument)
{
    osDelay(5000);

    if(init_udp_server() < 0) {
        printf("UDP Init ERR\n");
        return;
    }

    while (1) {
        // Recv data from test suite
        client_len = sizeof(client_addr);
        uint8_t tok_qty = 0; // accumulates amount of tokens
        memset(tokens, 0, sizeof(tokens));
        memset(buffer, 0, BUFFER_SIZE);

        ssize_t received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &client_len);

        // Get tokens
        char* ptr = strtok(buffer, " ");

        while (ptr != NULL)
        {
            printf("DEBUG: %d: %s\n", tok_qty, ptr);
            memcpy(tokens[tok_qty], ptr, strlen(ptr));
            ptr = strtok(NULL, " ");
            tok_qty++;
        }

        // Check for blank data
        if (received_bytes < 0) {
            printf("Error receiving data");
            return;
        }

        // buffer[received_bytes] = '\0';
        printf("DEBUG: Got %d\n", received_bytes);
        printf("DEBUGSTRING: Got: %s\n", buffer);
        printf("DEBUG: Got %d tokens\n", tok_qty);

        // Check that at least smth was sent
        if (received_bytes < 2)
        {
            printf("DEBUG: Nothing was sent!\n");
            sendto(sockfd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        // We know that max. 2 ars will be sent here so check for overflow (can be omitted)
        if (tok_qty > 3)
        {
            printf("DEBUG: Too much args!\n");
            sendto(sockfd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        // Check if it is a "sversion" command
        if (strncmp(tokens[0], "sversion", strlen("sversion")) == 0)
        {
            printf("DEBUG: Asked for status\n");
            sendto(sockfd, status_info, strlen(status_info), 0, (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        if (memcmp(tokens[0], "led", 3) == 0)
        {
            uint8_t led_no = strlen(tokens[0]) == 4 ? tokens[0][3] - ASCII_NULL_OFFSET: 0;
            Led_TypeDef led[4] = {LED3, LED4, LED5, LED6};

            GPIO_TypeDef* GPIO_PORT[LEDn] = {LED3_GPIO_PORT,
                                            LED4_GPIO_PORT,
                                            LED5_GPIO_PORT,
                                            LED6_GPIO_PORT};

            const uint16_t GPIO_PIN[LEDn] = {LED3_PIN,
                                            LED4_PIN,
                                            LED5_PIN,
                                            LED6_PIN};

            printf("DEBUG: LED command found. LED given: %d\n", led_no);

            if ((led_no < 3) || (led_no > 6))
            {
                printf("INCORRECT LED\n");
                sendto(sockfd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }

            if (strncmp(tokens[1], "on", strlen("on")) == 0)
            {
                printf("ON cmd for led%d\n", led_no);
                BSP_LED_On(led[led_no-LED_OFFSET]);
                sendto(sockfd, "OK\r\n", strlen("OK\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }
            else if (strncmp(tokens[1], "off", strlen("off")) == 0)
            {
                printf("OFF cmd for led%d\n", led_no);
                BSP_LED_Off(led[led_no-LED_OFFSET]);
                sendto(sockfd, "OK\r\n", strlen("OK\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }
            else if (strncmp(tokens[1], "toggle", strlen("toggle")) == 0)
            {
                printf("TOGGLE cmd for led%d\n", led_no);
                BSP_LED_Toggle(led[led_no-LED_OFFSET]);
                sendto(sockfd, "OK\r\n", strlen("OK\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }
            else if (strncmp(tokens[1], "status", strlen("status")) == 0)
            {
                char tosend[MAX_STRING_SIZE] = { 0, };
                uint8_t wrt_size = 0;
                printf("STATUS cmd for led%d\n", led_no);

                switch (HAL_GPIO_ReadPin(GPIO_PORT[led_no-LED_OFFSET], GPIO_PIN[led_no-LED_OFFSET]))
                {
                    case false:
                        wrt_size = snprintf(tosend, 0, "LED%d OFF\r\n", led_no);
                        snprintf(tosend, wrt_size + 1U, "LED%d OFF\r\n", led_no);
                        break;
                    case true:
                        wrt_size = snprintf(tosend, 0, "LED%d ON\r\n", led_no);
                        snprintf(tosend, wrt_size + 1U, "LED%d ON\r\n", led_no);
                        break;
                }
                sendto(sockfd, tosend, wrt_size, 0, (struct sockaddr *)&client_addr, client_len);
                sendto(sockfd, "OK\r\n", strlen("OK\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }
            else
            {
                printf("UNKNOWN cmd\n");
                sendto(sockfd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (struct sockaddr *)&client_addr, client_len);
                continue;
            }

        }
    }
    free(buffer);
    close(sockfd);
    osThreadTerminate(NULL);
}
