#include "include/System.h"

System::System() {};

void System::open() {
    dpadSetup();
    batterySetup();
}

void System::dpadSetup() {
    gpio_config_t io_conf = {};
    /*io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_2);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);*/

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << UpButton);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << RightButton);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << DownButton);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << LeftButton);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
}

void System::batterySetup() {
    adc1_config_width(ADC_WIDTH_BIT_12); // ความละเอียด 12 บิต (0 - 4095)
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11); // GPIO35
}

void System::updateInput() {
    upPrevState = upState;  
    rightPrevState = rightState;
    leftPrevState = leftState;
    downPrevState = downState;

    upState = gpio_get_level(UpButton);
    rightState = gpio_get_level(RightButton);
    downState = gpio_get_level(DownButton);
    leftState = gpio_get_level(LeftButton);

    //printf("%d, %d, %d, %d\n", upState, rightState, downState, leftState);
}

bool System::pressedOK() {
    return rightState == 0 && rightPrevState == 1;
}

bool System::pressedBack() {
    return leftState == 0 && leftPrevState == 1;
}

bool System::pressedUp() {
    return upState == 0 && upPrevState == 1;
}

bool System::pressedDown() {
    return downState == 0 && downPrevState == 1;
}
void System::updateCursor() {
    if(disabled == false){
        if(upState == 0 && upPrevState == 1) {
            if(cursor == max) return;
            cursor++;
        }
        if(downState == 0 && downPrevState == 1) {
            if(cursor == min) return;
            cursor--;
        }
    }else{
        //printf("disabled");
    }
}

int System::readCursor() {
    //printf("cursor: %d\n", cursor);
    return cursor;
}
    
void System::resetCursor(int min, int max) {
    this->min = min;
    this->max = max;
    //cursor = 0;
}

void System::setCursor(int val) {
    cursor = val;
}

void System::disableCursor() {
    disabled = true;
}

void System::enableCursor() {
    disabled = false;
}

void System::check_heap_usage() {
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t largest_block = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);

    printf("Free heap: %u bytes, ", free_heap);
    printf("Largest free block: %u bytes\n", largest_block);
}

float System::noteToFreq(std::string noteStr, float a4) {
    // Map of note names to semitone offsets
    static const std::unordered_map<std::string, int> noteOffsets = {
        {"C", 0},  {"C#", 1}, {"Db", 1},
        {"D", 2},  {"D#", 3}, {"Eb", 3},
        {"E", 4},
        {"F", 5},  {"F#", 6}, {"Gb", 6},
        {"G", 7},  {"G#", 8}, {"Ab", 8},
        {"A", 9},  {"A#",10}, {"Bb",10},
        {"B",11}
    };

    // Extract the note (e.g. "C#", "Ab") and the octave (e.g. "4")
    std::string notePart;
    int i = 0;
    while (i < noteStr.size() && (std::isalpha(noteStr[i]) || noteStr[i] == '#' || noteStr[i] == 'b')) {
        notePart += noteStr[i++];
    }

    if (noteOffsets.find(notePart) == noteOffsets.end()) {
        return -1;
    }

    std::string octavePart = noteStr.substr(i);
    int octave = std::stoi(octavePart);

    // MIDI note formula
    int midiNote = 12 * (octave + 1) + noteOffsets.at(notePart);

    return a4 * std::pow(2.0f, (midiNote - 69) / 12.0f);
}

std::string System::midiToNote(int midi) {
    char buf[16];
    int quotient = midi / 12;
    int reminder = midi % 12;
    switch(reminder){
        case 0:{
            sprintf(buf, "%s%d", "C", quotient - 1);
            break;
        }case 1:{
            sprintf(buf, "%s%d", "Db", quotient - 1);
            break;
        }case 2:{
            sprintf(buf, "%s%d", "D", quotient - 1);
            break;
        }case 3:{
            sprintf(buf, "%s%d", "Eb", quotient - 1);
            break;
        }case 4:{
            sprintf(buf, "%s%d", "E", quotient - 1);
            break;
        }case 5:{
            sprintf(buf, "%s%d", "F", quotient - 1);
            break;
        }case 6:{
            sprintf(buf, "%s%d", "Gb", quotient - 1);
            break;
        }case 7:{
            sprintf(buf, "%s%d", "G", quotient - 1);
            break;
        }case 8:{
            sprintf(buf, "%s%d", "Ab", quotient - 1);
            break;
        }case 9:{
            sprintf(buf, "%s%d", "A", quotient - 1);
            break;
        }case 10:{
            sprintf(buf, "%s%d", "Bb", quotient - 1);
            break;
        }case 11:{
            sprintf(buf, "%s%d", "B", quotient - 1);
            break;
        }default:{
            sprintf(buf, "%s%d", "x", 0);
            break;
        }
    }
    std::string bufstr(buf);
    return bufstr;
}

int System::noteToMidi(std::string note) {
    if (note.empty()) return -1;

    // Unordered map for faster lookup
    static const std::unordered_map<std::string, int> noteMap = {
        {"C", 0}, {"C#", 1}, {"Db", 1},
        {"D", 2}, {"D#", 3}, {"Eb", 3},
        {"E", 4},
        {"F", 5}, {"F#", 6}, {"Gb", 6},
        {"G", 7}, {"G#", 8}, {"Ab", 8},
        {"A", 9}, {"A#", 10}, {"Bb", 10},
        {"B", 11}
    };

    std::string key;
    int i = 0;

    key += note[i++];
    if (i < note.size() && (note[i] == '#' || note[i] == 'b')) {
        key += note[i++];
    }

    auto it = noteMap.find(key);
    if (it == noteMap.end()) return -1;

    // Manually parse the rest as octave
    int octave = 0;
    if (i >= note.size() || !std::isdigit(note[i])) return -1;

    // Convert remaining digits to int
    while (i < note.size()) {
        if (!std::isdigit(note[i])) return -1;
        octave = octave * 10 + (note[i] - '0');
        ++i;
    }

    return (octave + 1) * 12 + it->second;
}

int System::freqToMidi(float freq) {
    float processed = 69 + 12 * log2(freq / 440);
    if(processed - (int)processed <= 0.4) return (int)processed;
    else return (int)processed + 1;
}

void System::setupNoteMap(float a4) {
    std::string noteNames[] = {"C", "Db", "D", "Eb", "E", "F", 
                               "Gb", "G", "Ab", "A", "Bb", "B"};
    
    for (int midi = 0; midi <= 127; ++midi) {
        double freq = a4 * std::pow(2.0, (midi - 69) / 12.0);
        // Must cover custom-page range (MIDI 32..75) plus +-1 semitone scale bounds (up to E5 ~659 Hz)
        if (freq < 30.0 || freq > 700.0) continue;

        int octave = (midi / 12) - 1;
        std::string note = noteNames[midi % 12] + std::to_string(octave);
        noteFreq[note] = freq;
    }
}

void System::deleteNoteMap() {
    noteFreq.clear();
}

int System::mapOnScale(int scaleMin, int scaleMax, float freqMin, float freqMax, float input){
    return (int)(((float)(scaleMax - scaleMin)) * (input - freqMin) / (freqMax - freqMin) + scaleMin);
}

int System::map(int x1, int x2, int y1, int y2, int input){
    return (int)(((float)(x2 - x1)) * (input - y1) / (y2 - y1) + x1);
}

int System::readBattery() {
    int adc_reading = adc1_get_raw(ADC1_CHANNEL_7); // อ่านค่า ADC
    //printf("ADC Reading: %d\n", adc_reading);
    battery = map(0, 100, 2647, 3474, adc_reading);
    return battery;
}