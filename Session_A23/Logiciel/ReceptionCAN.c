#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/twai.h"

#define POT_PIN 0 // Remplacez par le numéro de broche GPIO réelle à laquelle le potentiomètre est connecté

void potentiometer_can_task(void *pvParameter) {
    // Initialisation de la configuration CAN
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_1, GPIO_NUM_6, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Installation du pilote CAN
    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        printf("Échec de l'installation du pilote CAN\n");
        vTaskDelete(NULL);
    }

    // Démarrage du pilote CAN
    if (twai_start() != ESP_OK) {
        printf("Échec du démarrage du pilote CAN\n");
        vTaskDelete(NULL);
    }

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

    while (1) {
        // Mesurer la tension du potentiomètre
        int pot_value = adc1_get_raw(ADC1_CHANNEL_0);
        float voltage = pot_value * (3.3 / 4095.0);
        printf("Valeur du potentiomètre : %d, Tension : %.2fV\n", pot_value, voltage);

        // Convertir la valeur en une chaîne de caractères (4 digits)
        char bufferPotValue[5]; // 4 digits + le caractère de fin de chaîne
        snprintf(bufferPotValue, sizeof(bufferPotValue), "%04d", pot_value);

        // Configurer le message CAN à transmettre
        twai_message_t adc_message;
        adc_message.identifier = 0xF;
        adc_message.data_length_code = 8;
        adc_message.rtr = 0;

        // Copier chaque caractère du buffer dans le message CAN
        for (int i = 0; i < 4; i++) {
            adc_message.data[i] = bufferPotValue[i];
        }

        adc_message.data[4] = 0; // Ajoutez d'autres données si nécessaire
        adc_message.data[5] = 0;
        adc_message.data[6] = 0;
        adc_message.data[7] = 0;

        // Mettre en file d'attente le message pour la transmission
        if (twai_transmit(&adc_message, pdMS_TO_TICKS(1000)) == ESP_OK) {
            printf("Message mis en file d'attente pour la transmission\n");
        } else {
            printf("Échec de la mise en file d'attente du message pour la transmission\n");
        }

        // Délai de 2 secondes entre chaque mesure
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main() {
    xTaskCreate(potentiometer_can_task, "potentiometer_can_task", 2048, NULL, 5, NULL);
}


/*#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main()
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_1, GPIO_NUM_6, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n");
    } else {
        printf("Failed to install driver\n");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK) {
        printf("Driver started\n");
    } else {
        printf("Failed to start driver\n");
        return;
    }

    // Delay for 2000 ms
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Configure message to transmit
    twai_message_t message;
    message.identifier = 0xAAAA;
    message.data_length_code = 8;
    message.rtr = 0;
    message.data[0] = 'h';
    message.data[1] = 'e';
    message.data[2] = 'l';
    message.data[3] = 'l';
    message.data[4] = 'o';
    message.data[5] = 'w';
    message.data[6] = 'o';
    message.data[7] = 'r';

    // Queue message for transmission
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        printf("Message queued for transmission\n");
    } else {
        printf("Failed to queue message for transmission\n");
    }
}*/

/*#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define POT_PIN GPIO_NUM_0 // Utilisation du canal ADC correspondant à GPIO_NUM_34

void adc_task(void *arg) {
    int adc_reading;
    int voltage;

    // Configuration de l'ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT_PIN, ADC_ATTEN_DB_11);

    while (1) {
        // Lecture de la valeur ADC
        adc_reading = adc1_get_raw(POT_PIN);

        // Conversion de la valeur ADC en tension
        voltage = adc_reading * (3.3 / 4095.0);

        // Impression des résultats sur la console
        printf(adc_reading);
        printf(voltage);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Attendre 1 seconde entre chaque lecture
    }
}

void app_main() {
    // Création de la tâche ADC
    xTaskCreate(adc_task, "adc_task", 2048, NULL, 5, NULL);
}
*/


/*
#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const int potPin3 = 34;  // GPIO 34 (Analog ADC1_CH6)
const int uartTxPin = GPIO_NUM_1;
const int uartRxPin = GPIO_NUM_6;

void setup_potentiometer() {
    // Configure the ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    // Configure UART communication
#include "driver/twai.h"
#include "freertos/task.h"


void app_main() 
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_0, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) 
    {
        printf("Driver installed\n");
    }
    else 
    {
        printf("Failed to install driver\n");
    }
    if (twai_start() == ESP_OK) 
    {
        printf("Driver started\n");
    }
    else 
    {
        printf("Failed to start driver\n");
    }

    while(1)
    {
        twai_message_t message;

        if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) //tester avec un else message si c'est le nombre de temps qu'il attend pour recevoir avant d'aller au else
        {
            printf("\n\n Message received \n");

            if (message.extd) 
            {
                printf("Message is in Extended Format\n");
            } 
            else 
            {
                printf("Message is in Standard Format\n");
            }
            printf("ID is %ld\n", message.identifier);

            for(int i=0; i< 4; i++)
            {
                printf("%c",(char)message.data[i]);
            }
        }
        else
        {
            printf("\n\n Message wasn't received\n");
        }
    }
}