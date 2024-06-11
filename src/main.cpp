#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include <cstdio>
#include <cstdlib>  // pour rand() et srand()
#include <ctime>    // pour time()

ThreadLvgl threadLvgl(30);
I2C i2c(PB_9, PB_8);  // SDA, SCL
Timer reaction_timer;
bool waiting_for_reaction = false;
BufferedSerial pc(USBTX, USBRX, 9600);
const int addr7bit = 0x48 << 1; 
//void scanI2C();

lv_obj_t * label; //pour afficher des messages sur l'écran
lv_obj_t * scr;// écran actuel sur lequel tous les autres objets sont placés
lv_obj_t * btn;
lv_timer_t * timer;

/*
void scanI2C()
    {
        char buffer[50];
        pc.write("Scanning I2C bus...\r\n", 20);
        for (int adress = 1; adress < 127;adress++){
            if (i2c.write(adress << 1, NULL, 0)==0)
            {// Si l'écriture réussit
            sprintf(buffer, "I2C device found at adress 0x%02X\r\n", adress);
            pc.write(buffer, strlen(buffer));
            }
        }
        pc.write("Scan complete. \r\n", 16);
    }
*/

void change_to_green(lv_timer_t * timer) {
    threadLvgl.lock();
    if (waiting_for_reaction) {
        lv_label_set_text(label, "Cliquez sur le bouton");
    }
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x00FF00), 0);  // Vert
    reaction_timer.reset();
    reaction_timer.start();
    waiting_for_reaction = true;
    threadLvgl.unlock();
}

void button_event_handler(lv_event_t * e) {
    if (waiting_for_reaction) {
        reaction_timer.stop();
        int reaction_time = reaction_timer.read_ms();

        threadLvgl.lock();
        char buffer[100];
        if (reaction_time > 1500) {
            snprintf(buffer, 100, "Temps de reaction : %d ms. Attention.\n Votre reflexe est diminue \npeut etre en raison de l'alcool.", reaction_time);
        } else {
            snprintf(buffer, 100, "Temps de reaction : %d ms. Votre temps de reaction \nest normal. Vous etes sobre.", reaction_time);
        }
        lv_label_set_text(label, buffer);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); // Permet au texte de se dérouler sur plusieurs lignes
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);  // Ré-aligner le label
        waiting_for_reaction = false;
        threadLvgl.unlock();
    }
}

int main() {
    //  char cmd[1] = {0x00};
    //  char data[2];
    //  char buffer[50];

    // Initialisation de la bibliothèque LVGL
    threadLvgl.lock();
    
    // Créer un nouvel écran et définir son style
    scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFF0000), 0);  // Rouge

    // Créer un label et y afficher un message initial
    label = lv_label_create(scr);  // Crée un label sur l'écran actuel
    lv_label_set_text(label, "Attendez le vert et cliquez sur le bouton");  // Définir le texte du label
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); // Permet au texte de se dérouler sur plusieurs lignes
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);  // Aligner le label au centre de l'écran

    // Créer un bouton
    btn = lv_btn_create(scr);  // Crée un bouton sur l'écran actuel
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 60);  // Aligner le bouton au centre de l'écran avec un offset
    lv_obj_t * btn_label = lv_label_create(btn);  // Crée un label pour le bouton
    lv_label_set_text(btn_label, "Cliquez ici");  // Définir le texte du label du bouton

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    int delay_ms = (rand() % 9000) + 6000;
    timer = lv_timer_create(change_to_green, delay_ms, NULL);

    threadLvgl.unlock();

    // creer un événements au bouton
    lv_obj_add_event_cb(btn, button_event_handler, LV_EVENT_CLICKED, NULL);

    while (1) {
    // int ack = i2c.write(addr7bit << 1, cmd, 1);
    //     if (ack == 0) {
    //         ack = i2c.read(addr7bit << 1, data, 2);
    //         if (ack == 0) {
    //             uint16_t raw_value = (data[0] << 8) | data[1];
    //             float voltage = raw_value * (5.0 / 65535.0); // Conversion en tension
    //             sprintf(buffer, "Val: %d\nVoltage: %.2fV", raw_value, voltage);

    //             // Mettre à jour l'affichage
    //             threadLvgl.lock();
    //             lv_label_set_text(label, buffer);       // Définir le texte du label
    //             lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); 
    //             threadLvgl.unlock();

    //             // Affichage via Serial pour debug
    //             pc.write(buffer, strlen(buffer));
    //         }
    //     }
        ThisThread::sleep_for(10ms);
    }
}