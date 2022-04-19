#define WIN32
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#define MAX_STR 255
#include <locale.h>
#include <string.h>
#include <wchar.h>

void Nullify(uint8_t buf[256])
{
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
}

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    (void)argc;
    (void)argv;

	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	struct hid_device_info *devs, *cur_dev;

	printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
		printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		printf("Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

    // находим все устройства USB HID, печатаем содержимое дескриптора устройства, доступное через драйвер
    devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x1234, 0x0001, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}


	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n\n", wstr);
    system("pause");
	Nullify(buf);
	uint8_t menu;
    uint16_t button;
    uint8_t point_x;
    uint8_t point_y;
    uint8_t color;
    bool running = true;
	while(running)
    {
        system("cls");
        printf("1.Вывести показания реостата и изменить яркость светодиода\n2.Вывести на экран точку\n3)Закрасить экран\n4)Обработать нажатие кнопок\n5)Выход\n\nВыберите пункт меню: ");
        scanf("%d",&menu);
        switch(menu)
        {
        case 1:
            {
             while(1)
            {
            system("cls");
            buf[0] = 0x03;
            res = hid_get_feature_report(handle,buf,sizeof(buf));
            int32_t result;
            for(int i = 1; i< res; i++)
            memcpy(&result,&buf[1],4);
            printf("Показания реостата: %d",result);
            buf[0] = 0x02;
            buf[3] = buf[1];
            buf[4] = buf[2];
            buf[5] = buf[1];
            buf[6] = buf[2];
            res = hid_send_feature_report(handle,buf,7);
            Nullify(buf);
            buf[0] = 0x1;
            res = hid_get_feature_report(handle, buf, sizeof(buf));
            if (res < 0) {
                printf("Unable to get a feature report.\n");
                printf("%ls", hid_error(handle));
            }
            else
            {
                memcpy(&button,&buf[1],2);
                printf("\nНажмите кнопку 4, чтобы вернуться в главное меню.",button);
                if(button==8)
                {
                    button = 0;
                    break;
                }
            }
            }
            break;
            }
        case 2:
            {
                system("cls");
                printf("Введите x: ");
                scanf("%d",&point_x);
                memcpy(&buf[1],&point_x,1);
                printf("Введите y: ");
                scanf("%d",&point_y);
                memcpy(&buf[2],&point_y,1);
                buf[0] = 0x04;
                printf("Каким цветом закрасить?\n1)Белым\n2)Чёрным\n\nВыберите цвет: ");
                scanf("%d",&color);
                switch(color)
                {
                case 1:
                    {
                        buf[3] = 0x01;
                        break;
                    }
                case 2:
                    {
                        buf[3] = 0x00;
                        break;
                    }
                }
                hid_send_feature_report(handle,buf,4);
                printf("\nТочка закрашена.\n");
                Nullify(buf);
                system("pause");
                break;
            }
        case 3:
            {
                system("cls");
                buf[0] = 0x04;
                printf("Каким цветом закрасить?\n1)Белым\n2)Чёрным\n\nВыберите цвет: ");
                scanf("%d",&color);
                switch(color)
                {
                case 1:
                    {
                        buf[3] = 0x01;
                        break;
                    }
                case 2:
                    {
                        buf[3] = 0x00;
                        break;
                    }
                }
                for(uint8_t i =0; i<64;i++)
                {
                    for(uint8_t j=0;j<128;j++)
                    {
                        memcpy(&buf[1],&j,1);
                        memcpy(&buf[2],&i,1);
                        hid_send_feature_report(handle,buf,4);
                    }
                }
                Nullify(buf);
                printf("Экран закрашен.\n");
                system("pause");
                break;
            }
        case 4:
            {
                system("cls");
                printf("\nНажмите кнопку 4, чтобы вернуться в главное меню.\nНажмите кнопку 1, чтобы зажечь все светодиоды или кнопку 2, чтобы погасить их.\n\n",button);
                while(1)
                {
                buf[0] = 0x1;
                hid_get_feature_report(handle, buf, sizeof(buf));
                memcpy(&button,&buf[1],2);
                if(button==8)
                {
                    button = 0;
                    Nullify(buf);
                    break;
                }
                if(button==1)
                {
                    buf[0] = 0x02;
                    buf[1] = 0xff;
                    buf[2] = 0xff;
                    buf[3] = 0xff;
                    buf[4] = 0xff;
                    buf[5] = 0xff;
                    buf[6] = 0xff;
                    hid_send_feature_report(handle,buf,7);
                    Nullify(buf);
                    printf("Светодиод зажжён.\n");
                    button = 0;
                    system("pause");
                }
                if(button==2)
                {
                    buf[0] = 0x02;
                    Nullify(buf);
                    hid_send_feature_report(handle,buf,7);
                    printf("Светодиод потушен.\n");
                    button = 0;
                    system("pause");
                }
                }
                break;
            }
        case 5:
            {
                running = false;
                break;
            }
        }
    }


    return 0;
}
