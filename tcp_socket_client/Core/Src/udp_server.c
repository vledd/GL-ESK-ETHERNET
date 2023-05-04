#include "simple_http_server.h"
#include "main.h"
#include "lwip.h"
#include "sockets.h"
#include "cmsis_os.h"
#include <string.h>

#define PORTNUM 5678U
#define VERSION "udp_server_Vladyslav_Nechaiev_21042023\n"

#if (USE_UDP_SERVER_PRINTF == 1)
#include <stdio.h>
#define UDP_SERVER_PRINTF(...) do { printf("[udp_server.c: %s: %d]: ",__func__, __LINE__);printf(__VA_ARGS__); } while (0)
#else
#define UDP_SERVER_PRINTF(...)
#endif

static struct sockaddr_in serv_addr, client_addr;
static int socket_fd;
static uint16_t nport;

void clear(char buffer[], size_t n)
{
	for (uint32_t i = 0; i < n; i++)
		{buffer[i] = '\0';}
}
static int udpServerInit(void)
{
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		UDP_SERVER_PRINTF("socket() error\n");
		return -1;
	}

	nport = PORTNUM;
	nport = htons((uint16_t)nport);

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = nport;

	if(bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1) {
		UDP_SERVER_PRINTF("bind() error\n");
		close(socket_fd);
		return -1;
	}

	UDP_SERVER_PRINTF("Server is ready\n");

	return 0;
}

void StartUdpServerTask(void const * argument)
{
	osDelay(5000);
	if(udpServerInit() < 0) {
		UDP_SERVER_PRINTF("udpSocketServerInit() error\n");
		osThreadTerminate(NULL);
	}

	for(;;)
	{
	  bzero(&client_addr, sizeof(client_addr));

	  int nbytes;
	  const size_t buf_len=256;
	  size_t buf_cmd_len = 0;
	  char buffer[buf_len];
		for (uint32_t i=0; i<buf_len; i++)
			{buffer[i]='\0';}
	  memset(buffer, 8, buf_len);
	  socklen_t addrlen = sizeof(client_addr);

	  while ( (nbytes = recvfrom(socket_fd, buffer, (size_t)sizeof(buffer),
			  0, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) > 0 )
	  {
		 buf_cmd_len = strlen(buffer);
		 buffer[buf_cmd_len] = '\0';
		 char* parts[2];
		 uint8_t part_index = 0;
		 parts[part_index] = strtok((char*) buffer, " ");
		 while (parts[part_index] != NULL && part_index < 1)
		 {
			 parts[++part_index] = strtok(NULL, " ");
		 }

		if (strcmp(parts[0], "exit") == 0)
		{
			sendto(socket_fd, "Session was successfully terminated\n", strlen("Session was successfully terminated\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
			clear(buffer, buf_len);
			break;
		}

		else if (strcmp(parts[0], "sversion") == 0)
		{
			sendto(socket_fd, VERSION, strlen(VERSION), 0, (const struct sockaddr*)&client_addr, addrlen);
			sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
			clear(buffer, buf_len);
		    continue;
		}

		else if (strcmp(parts[1], "on") == 0)
		{
			if (strcmp(parts[0], "led3") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led4") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led5") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led6") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}
		}

		else if (strcmp(parts[1], "off") == 0)
		{
			if (strcmp(parts[0], "led3") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led4") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led5") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led6") == 0)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}
		}

		else if (strcmp(parts[1], "toggle") == 0)
		{
			if (strcmp(parts[0], "led3") == 0)
			{
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led4") == 0)
			{
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led5") == 0)
			{
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led6") == 0)
			{
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
				sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				clear(buffer, buf_len);
				continue;
			}
		}

		else if (strcmp(parts[1], "status") == 0)
		{
			if (strcmp(parts[0], "led3") == 0)
			{
				GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13);
				if (pinState==GPIO_PIN_SET)
				{
					sendto(socket_fd, "LED3 ON\r\n", strlen("LED3 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (pinState==GPIO_PIN_RESET)
				{
					sendto(socket_fd, "LED3 OFF\r\n", strlen("LED3 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led4") == 0)
			{
				GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12);
				if (pinState==GPIO_PIN_SET)
				{
					sendto(socket_fd, "LED4 ON\r\n", strlen("LED4 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (pinState==GPIO_PIN_RESET)
				{
					sendto(socket_fd, "LED4 OFF\r\n", strlen("LED4 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led5") == 0)
			{
				GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14);
				if (pinState==GPIO_PIN_SET)
				{
					sendto(socket_fd, "LED5 ON\r\n", strlen("LED5 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (pinState==GPIO_PIN_RESET)
				{
					sendto(socket_fd, "LED5 OFF\r\n", strlen("LED5 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				clear(buffer, buf_len);
				continue;
			}

			else if (strcmp(parts[0], "led6") == 0)
			{
				GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15);
				if (pinState==GPIO_PIN_SET)
				{
					sendto(socket_fd, "LED6 ON\r\n", strlen("LED6 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (pinState==GPIO_PIN_RESET)
				{
					sendto(socket_fd, "LED6 OFF\r\n", strlen("LED6 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
					sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				clear(buffer, buf_len);
				continue;
			}
		}

		else
		{
			sendto(socket_fd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
			clear(buffer, buf_len);
		}

		if (sendto(socket_fd, buffer, nbytes, 0, (const struct sockaddr*)&client_addr, addrlen) < 0)
		{
			UDP_SERVER_PRINTF("send() error\n");
			clear(buffer, buf_len);
		}

	  }
			close(socket_fd);
	}
}
