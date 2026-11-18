#include "include/GuitarTunerPage.h"

GuitarTunerPage::GuitarTunerPage(Display* display, System* system) : display(display), screen(display->getInst()), system(system) {}

GuitarTunerPage::GuitarTunerPage(Display* display, System* system, std::string* noteList) {
    this->display = display;
    this->screen = display->getInst();
    this->system = system;

    for(int i = 0; i < 6; i++){
        stringNote[i] = noteList[i];
    }
}

void GuitarTunerPage::open() {
    system->resetCursor(0, 1);
    system->setCursor(0);

    inmp441 = new I2SDevice(I2S_WS, I2S_SCK, I2S_SD, sample_size, fs, down_factor);
    inmp441->open();
    
    fft = new FFTDevice(inmp441, sample_size, fs, down_factor);
    fft->open();

    fft->ioctl(Command::calibrate, nullptr);

    system->setupNoteMap(440);

    motor = new Control(A1_PIN, A2_PIN);
    motor->open();

    motor->stopSoft();

    printf("opened GuitarTunerPage\n");
}

void GuitarTunerPage::close() {
    system->deleteNoteMap();
    fft->close();
    delete fft;
    inmp441->close();
    delete inmp441;

    delete motor;
    printf("closed GuitarTunerPage!\n");
}

void GuitarTunerPage::tick() {
    frame++;
    switch(mode){
        case 0:{
            double freq=0;
            int rms=0;
            fft->ioctl(Command::read_freq, &freq);
            //printf("%.2f\n", freq);

            float stringFreq[6];
            int lowestIdx = 0, lowestVal = 1000;
            for(int i = 0; i < 6; i++){
                stringFreq[i] = abs(system->noteFreq[stringNote[i]] - freq);
                if(stringFreq[i] < lowestVal){
                    lowestIdx = i;
                    lowestVal = stringFreq[i];
                }
            }
            currentString = lowestIdx + 1;
            std::string detectedNote = stringNote[currentString - 1];
            int detectedMidi = System::noteToMidi(detectedNote);

            float error = (system->noteFreq[detectedNote] - freq) / system->noteFreq[detectedNote];

            if(!selectingString) motor->moveTo(error);
            else motor->stopSoft();

            //inmp441->ioctl(Command::read_rms, &rms);

            //printf("rms: %d, freq: %.2f\n", rms, freq);

            
            /*char buf[16];
            float test = System::midiToFreq("Bb3", 440.0f);
            //printf("%.2f\n", test);
            sprintf(buf, "loudness: %d", rms); 
            ssd1306_draw_string(screen, 8, 44, (uint8_t*)buf, 12, 1);
            sprintf(buf, "freq: %.2f", freq); 
            ssd1306_draw_string(screen, 8, 54, (uint8_t*)buf, 12, 1);*/
            //printf("%s\n", detectNote.c_str());

            ssd1306_clear_screen(screen, 0x00);
            ssd1306_draw_string(screen, 8, 54, (uint8_t*)"Mode: ", 12, 1);
            /*char buf[16];
            sprintf(buf, "Mode: %s", modeName[mode].c_str()); */
            ssd1306_draw_string(screen, 45, 54, (uint8_t*)(modeName[mode].c_str()), 12, 1); //mode

            //scale view
            ssd1306_draw_line(screen, 10, 30, 110, 30);
            ssd1306_draw_line(screen, 10, 40, 110, 40);
            ssd1306_draw_line(screen, 10, 30, 10, 40);
            ssd1306_draw_line(screen, 60, 30, 60, 40);
            ssd1306_draw_line(screen, 110, 30, 110, 40);

            std::string upBound = System::midiToNote(detectedMidi + 1);
            std::string downBound = System::midiToNote(detectedMidi - 1);
            ssd1306_draw_string(screen, 60, 15, (uint8_t*)(detectedNote.c_str()), 12, 1);
            ssd1306_draw_string(screen, 10, 15, (uint8_t*)(downBound.c_str()), 12, 1);
            ssd1306_draw_string(screen, 100, 15, (uint8_t*)(upBound.c_str()), 12, 1);

            float maxAllow = system->noteFreq[detectedNote] * (1.0 + maxErrorPercent);
            float minAllow = system->noteFreq[detectedNote] * (1.0 - maxErrorPercent);
            int maxAllowOnScale = System::mapOnScale(10, 110, system->noteFreq[downBound], system->noteFreq[upBound], maxAllow);
            int minAllowOnScale = System::mapOnScale(10, 110, system->noteFreq[downBound], system->noteFreq[upBound], minAllow);
            ssd1306_draw_line(screen, maxAllowOnScale, 30, maxAllowOnScale, 40);
            ssd1306_draw_line(screen, minAllowOnScale, 30, minAllowOnScale, 40);

            int freqOnScale = (110.0 - 10.0)/(system->noteFreq[System::midiToNote(detectedMidi + 1)] - system->noteFreq[System::midiToNote(detectedMidi - 1)]) * (freq - system->noteFreq[System::midiToNote(detectedMidi - 1)])  + 10;

            /*printf("%s = %.2f\n",System::midiToNote(detectedMidi-1).c_str(), system->noteFreq[System::midiToNote(detectedMidi-1)]);
            printf("%s = %.2f\n",System::midiToNote(detectedMidi).c_str(), system->noteFreq[System::midiToNote(detectedMidi)]);
            printf("%s = %.2f\n",System::midiToNote(detectedMidi+1).c_str(), system->noteFreq[System::midiToNote(detectedMidi+1)]);
            printf("%d\n", freqOnScale);*/
            if(freqOnScale < 10) display->triangleLeft(10, 43, 5);
            else if(freqOnScale > 110) display->triangleRight(110, 43, 5);
            else display->triangleUp(freqOnScale, 41, 5);

            char buf[2];
            sprintf(buf, "%d", currentString); 
            ssd1306_draw_string(screen, 100,50 , (uint8_t*)buf, 16, 1); //current string

            switch(system->readCursor()){
                case 0:{
                    ssd1306_draw_string(screen, 100, 50, (uint8_t*)buf, 16, 0);
                    break;
                }case 1:{
                    ssd1306_draw_string(screen, 45, 54, (uint8_t*)(modeName[mode].c_str()), 12, 0);
                    break;
                }default:{}
            }
            break;
        }
        case 1:{
            double freq=0;
            int rms=0;
            fft->ioctl(Command::read_freq, &freq);

            std::string selectedNote = stringNote[currentString - 1];
            int midi = System::noteToMidi(selectedNote);

            float error = (system->noteFreq[selectedNote] - freq) / system->noteFreq[selectedNote];

            if(!selectingString) motor->moveTo(error);
            else motor->stopSoft();

            ssd1306_clear_screen(screen, 0x00);
            ssd1306_draw_string(screen, 8, 54, (uint8_t*)"Mode: ", 12, 1);
            ssd1306_draw_string(screen, 45, 54, (uint8_t*)(modeName[mode].c_str()), 12, 1); //mode

            //scale view
            ssd1306_draw_line(screen, 10, 30, 110, 30);
            ssd1306_draw_line(screen, 10, 40, 110, 40);
            ssd1306_draw_line(screen, 10, 30, 10, 40);
            ssd1306_draw_line(screen, 60, 30, 60, 40);
            ssd1306_draw_line(screen, 110, 30, 110, 40);

            std::string upBound = System::midiToNote(midi + 1);
            std::string downBound = System::midiToNote(midi - 1);
            ssd1306_draw_string(screen, 60, 15, (uint8_t*)(selectedNote.c_str()), 12, 1);
            ssd1306_draw_string(screen, 10, 15, (uint8_t*)(downBound.c_str()), 12, 1);
            ssd1306_draw_string(screen, 100, 15, (uint8_t*)(upBound.c_str()), 12, 1);

            float maxAllow = system->noteFreq[selectedNote] * (1.0 + maxErrorPercent);
            float minAllow = system->noteFreq[selectedNote] * (1.0 - maxErrorPercent);
            int maxAllowOnScale = System::mapOnScale(10, 110, system->noteFreq[downBound], system->noteFreq[upBound], maxAllow);
            int minAllowOnScale = System::mapOnScale(10, 110, system->noteFreq[downBound], system->noteFreq[upBound], minAllow);
            ssd1306_draw_line(screen, maxAllowOnScale, 30, maxAllowOnScale, 40);
            ssd1306_draw_line(screen, minAllowOnScale, 30, minAllowOnScale, 40);

            int freqOnScale = System::mapOnScale(10, 110, system->noteFreq[downBound], system->noteFreq[upBound], freq);

            /*printf("%s = %.2f\n",System::midiToNote(detectedMidi-1).c_str(), system->noteFreq[System::midiToNote(detectedMidi-1)]);
            printf("%s = %.2f\n",System::midiToNote(detectedMidi).c_str(), system->noteFreq[System::midiToNote(detectedMidi)]);
            printf("%s = %.2f\n",System::midiToNote(detectedMidi+1).c_str(), system->noteFreq[System::midiToNote(detectedMidi+1)]);
            printf("%d\n", freqOnScale);*/
            if(freqOnScale < 10) display->triangleLeft(10, 43, 5);
            else if(freqOnScale > 110) display->triangleRight(110, 43, 5);
            else display->triangleUp(freqOnScale, 41, 5);

            char buf[2];
            sprintf(buf, "%d", currentString); 
            ssd1306_draw_string(screen, 100,50 , (uint8_t*)buf, 16, 1); //current string
            
            switch(system->readCursor()){
                case 0:{
                    if(!selectingString) ssd1306_draw_string(screen, 100, 50, (uint8_t*)buf, 16, 0);
                    else {
                        if(frame % 2 == 0) ssd1306_draw_string(screen, 100, 50, (uint8_t*)buf, 16, 0);
                        else ssd1306_draw_string(screen, 100, 50, (uint8_t*)buf, 16, 1);
                    }
                    break;
                }case 1:{
                    ssd1306_draw_string(screen, 45, 54, (uint8_t*)(modeName[mode].c_str()), 12, 0);
                    break;
                }default:{}
            }
            break;
        }default: {}
    }
    
    //ssd1306_refresh_gram(screen);

    if (selectingString) {
        //printf("selecting String!\n");
        system->disableCursor();
        
        if (system->pressedUp()) {
            currentString = std::min(currentString + 1, 6); // สาย 6 บน
        } else if (system->pressedDown()) {
            currentString = std::max(currentString - 1, 1); // สาย 1 ล่าง
        } else if (system->pressedOK()) {
            selectingString = false; // ยืนยันการเลือกสาย
        } else if (system->pressedBack()) {
            selectingString = false; // ยกเลิกการเลือก
        }
    }else{
        system->enableCursor();
        if(system->pressedOK()) {
            switch(system->readCursor()){
            case 0:{
                selectingString = true;
                break;
            }case 1:{
                if(mode == 0) mode = 1;
                else mode = 0;
            }default:{}
        }
        }else if(system->pressedBack()) {
            exit = true;
        }
    }
}

bool GuitarTunerPage::wantsToExit() {
    return exit;
}

Page* GuitarTunerPage::nextPage() {
    return next;
}