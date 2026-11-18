#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_dsp.h"

#include <iostream>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>
#include <stack>
#include <fstream>
#include <complex>

#include "../components/tunerLib/include/I2SDevice.h"
#include "../components/tunerLib/include/FFTDevice.h"
#include "../components/tunerLib/include/Command.h"

#include "../components/display/include/Display.h"

#include "../components/page/include/PageCore.h"

#define SDA_PIN 21
#define SCL_PIN 22

/*#include "esp_heap_trace.h"
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS];*/

std::stack<Page*> pageStack;

extern "C" void app_main(void)
{
    Display* ssd1306 = new Display(SDA_PIN, SCL_PIN);
    ssd1306->open();
    
    System* system = new System();
    system->open();

    UpperBar* bar = new UpperBar(ssd1306, system);

    Page* current = new MenuPage(ssd1306, system);
    current->open();
    pageStack.push(current);

    while(1) {
        system->updateInput();
        system->updateCursor();

        if(pageStack.empty()) break;

        current = pageStack.top();
        current->tick();

        bar->tick();

        if(current->wantsToExit()){
            current->close();
            delete current;
            pageStack.pop();
        }else {
            Page* next = current->nextPage();
            if(next){
                current->close();
                next->open();
                pageStack.push(next);
            }
        }
        //printf("%d\n", system->readCursor());
        //printf("Page Stack Size: %d\n", pageStack.size());
        //System::check_heap_usage();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*  heap_trace_init_standalone(trace_record, NUM_RECORDS);
    heap_trace_start(HEAP_TRACE_LEAKS);

    // ทำการเปิด page ที่คุณสงสัยว่ามี memory leak
    Page* test = new GuitarTunerPage(ssd1306, system);
    test->open();  // ตัวอย่าง
    test->close();
    delete test;

    heap_trace_stop();
    heap_trace_dump(); */