#include "include/GuitarCustomPage.h"

GuitarCustomPage::GuitarCustomPage(Display* display, System* system) : display(display), screen(display->getInst()), system(system) {}

void GuitarCustomPage::open() {
    system->setCursor(0);
    printf("opened GuitarCustomPage\n");
}

void GuitarCustomPage::close() {
    printf("closed GuitarCustomPage!\n");
}

void GuitarCustomPage::tick() {
    frame++;
    system->resetCursor(0, 7);

    ssd1306_clear_screen(screen, 0x00);
    
    for(int i = 0; i < 6; i++){
        ssd1306_draw_line(screen, 20, 15 + 8 * i, 100, 15 + 8 * i);
        if(i % 2 == 0){
            ssd1306_draw_string(screen, 105, 9 + 8 * i, (uint8_t*)(stringNote[5-i].c_str()), 12, 1);
        }else{
            ssd1306_draw_string(screen, 5, 9 + 8 * i, (uint8_t*)(stringNote[5-i].c_str()), 12, 1);
        }
    }
    
    ssd1306_draw_string(screen, 80, 50, (uint8_t*)"Confirm", 12, 1);

    switch(system->readCursor()){
        case 0:{
            if(!selecting) ssd1306_draw_string(screen, 105, 9 + 8 * 0, (uint8_t*)(stringNote[5].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 105, 9 + 8 * 0, (uint8_t*)(stringNote[5].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 105, 9 + 8 * 0, (uint8_t*)(stringNote[5].c_str()), 12, 1);
            }
            break;
        }case 1:{
            if(!selecting) ssd1306_draw_string(screen, 5, 9 + 8 * 1, (uint8_t*)(stringNote[4].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 5, 9 + 8 * 1, (uint8_t*)(stringNote[4].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 5, 9 + 8 * 1, (uint8_t*)(stringNote[4].c_str()), 12, 1);
            }
            break;
        }case 2:{
            if(!selecting) ssd1306_draw_string(screen, 105, 9 + 8 * 2, (uint8_t*)(stringNote[3].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 105, 9 + 8 * 2, (uint8_t*)(stringNote[3].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 105, 9 + 8 * 2, (uint8_t*)(stringNote[3].c_str()), 12, 1);
            }
            break;
        }case 3:{
            if(!selecting) ssd1306_draw_string(screen, 5, 9 + 8 * 3, (uint8_t*)(stringNote[2].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 5, 9 + 8 * 3, (uint8_t*)(stringNote[2].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 5, 9 + 8 * 3, (uint8_t*)(stringNote[2].c_str()), 12, 1);
            }
            break;
        }case 4:{
            if(!selecting) ssd1306_draw_string(screen, 105, 9 + 8 * 4, (uint8_t*)(stringNote[1].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 105, 9 + 8 * 4, (uint8_t*)(stringNote[1].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 105, 9 + 8 * 4, (uint8_t*)(stringNote[1].c_str()), 12, 1);
            }
            break;
        }case 5:{
            if(!selecting) ssd1306_draw_string(screen, 5, 9 + 8 * 5, (uint8_t*)(stringNote[0].c_str()), 12, 0);
            else {
                if(frame % 2 == 0) ssd1306_draw_string(screen, 5, 9 + 8 * 5, (uint8_t*)(stringNote[0].c_str()), 12, 0);
                else ssd1306_draw_string(screen, 5, 9 + 8 * 5, (uint8_t*)(stringNote[0].c_str()), 12, 1);
            }
            break;
        }case 6:{
            ssd1306_draw_string(screen, 80, 50, (uint8_t*)"Confirm", 12, 0);
            break;
        }case 7:{
            //ssd1306_draw_string(screen, 5, 9 + 8 * 1, (uint8_t*)(stringNote[i].c_str()), 12, 0);
            break;
        }
        default:{}
    }
    //ssd1306_refresh_gram(screen);

    if (selecting) {
        //printf("selecting String!\n");
        system->disableCursor();
        
        if (system->pressedUp()) {
            currentMidi = std::min(currentMidi + 1, 75); // สาย 6 บน
        } else if (system->pressedDown()) {
            currentMidi = std::max(currentMidi - 1, 32); // สาย 1 ล่าง
        } else if (system->pressedOK()) {
            selecting = false;
        } else if (system->pressedBack()) {
            selecting = false;
            currentMidi = backTemp;
        }
        stringNote[5-system->readCursor()] = System::midiToNote(currentMidi);
    }else{
        system->enableCursor();
        if(system->pressedOK()) {
            switch(system->readCursor()){
                case 0:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[5]);
                    backTemp = currentMidi;
                    break;
                }case 1:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[4]);
                    break;
                }case 2:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[3]);
                    break;
                }case 3:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[2]);
                    break;
                }case 4:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[1]);
                    break;
                }case 5:{
                    selecting = true;
                    currentMidi = System::noteToMidi(stringNote[0]);
                    break;
                }case 6:{
                    next = new GuitarTunerPage(display, system, &stringNote[0]);
                    break;
                }
                default:{}
            }
        }else if(system->pressedBack()) {
            exit = true;
        }
    }
}

bool GuitarCustomPage::wantsToExit() {
    return exit;
}

Page* GuitarCustomPage::nextPage() {
    Page* temp = next;
    next = nullptr;
    return temp;
}