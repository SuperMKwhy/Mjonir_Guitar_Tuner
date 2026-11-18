#include "FFTDevice.h"

FFTDevice::FFTDevice(Device* dev, int SS, int FS, int DF) : dev(dev), sample_size(SS), fs(FS), down_factor(DF) {}

void FFTDevice::open(){
    wind = (float*)heap_caps_aligned_alloc(16, sample_size * sizeof(float), MALLOC_CAP_8BIT);
    fft_data = (float*)heap_caps_aligned_alloc(16, sample_size * 2 * sizeof(float), MALLOC_CAP_8BIT);
    R = (float*)heap_caps_aligned_alloc(16, (sample_size/2) * sizeof(float), MALLOC_CAP_8BIT);
    baseline = (float*)heap_caps_aligned_alloc(16, sample_size/2 * sizeof(float), MALLOC_CAP_8BIT);
    input = (int16_t*)heap_caps_aligned_alloc(16, sample_size * sizeof(int16_t), MALLOC_CAP_8BIT);

    if (baseline != NULL) {
        memset(baseline, 0, sample_size/2 * sizeof(float));  // Set all bytes to 0
    }
    esp_err_t ret;
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret  != ESP_OK) {
        printf("Not possible to initialize FFT. Error = %i", ret);
        return;
    }

    dsps_wind_hann_f32(wind, sample_size);
    printf("FFTDevice initiated successfully!\n");
}

void FFTDevice::close(){
    dsps_fft2r_deinit_fc32();

    heap_caps_free(wind);
    heap_caps_free(fft_data);
    heap_caps_free(baseline);
    heap_caps_free(R);
    heap_caps_free(input);
    printf("FFTDevice closed successfully!\n");
}

void FFTDevice::write(int value){
    
}

int FFTDevice::ioctl(Command cmd, void* out) {
    switch(cmd) {
        case Command::read_freq: {
            dev->ioctl(Command::read_samples, input);

            for (int i = 0 ; i < sample_size ; i++) {
                //printf("gen %d = %d\n", i, gen[i]);
                fft_data[i * 2 + 0] = input[i] * wind[i];
                fft_data[i * 2 + 1] = 0.0f;
            }

            // FFT
            dsps_fft2r_fc32(fft_data, sample_size);
            // Bit reverse
            dsps_bit_rev_fc32(fft_data, sample_size);
            // Convert one complex vector to two complex vectors
            dsps_cplx2reC_fc32(fft_data, sample_size);

            // Correct magnitude calculation after dsps_cplx2reC_fc32
            for (int i = 0 ; i < sample_size / 2 ; i++) {
                fft_data[i] = 10 * log10f((fft_data[i * 2 + 0] * fft_data[i * 2 + 0] + fft_data[i * 2 + 1] * fft_data[i * 2 + 1]) / sample_size);
                R[i] = fmax(fft_data[i] - baseline[i], 0.0f);
            }

            int maxIndex = 0;
            double maxMag = 0;

            //printf("bin,Magnitude\n");  // Header
            for (int k = 1; k < sample_size / 2; ++k) {
                double mag = R[k];
                /*printf("k = %d -> mag = %d - %d = %d |", k, (int)fft_data[k], (int)baseline[k], (int)R[k]);
                for (int i = 0; i < abs((int)R[k]); i++) printf("#");
                printf("\n");*/
                //printf("%d,%.2f\n", k, R[k]);
                if (mag > maxMag) {
                maxMag = mag;
                maxIndex = k;
                }
            }
            // ใช้ fs ใหม่ = fs / down_factor
            
            *((double*)out)  = (double)maxIndex * (fs / down_factor) / sample_size;
            return 0;
        }
        case Command::calibrate: {
            for(int i = 0; i < calibration_time; i++){
                dev->ioctl(Command::read_samples, input);

                for (int i = 0 ; i < sample_size ; i++) {
                    //printf("gen %d = %d\n", i, gen[i]);
                    fft_data[i * 2 + 0] = input[i] * wind[i];
                    fft_data[i * 2 + 1] = 0.0f;
                }

                // FFT
                dsps_fft2r_fc32(fft_data, sample_size);
                // Bit reverse
                dsps_bit_rev_fc32(fft_data, sample_size);
                // Convert one complex vector to two complex vectors
                dsps_cplx2reC_fc32(fft_data, sample_size);

                // Correct magnitude calculation after dsps_cplx2reC_fc32
                for (int i = 0 ; i < sample_size / 2 ; i++) {
                    fft_data[i] = 10 * log10f((fft_data[i * 2 + 0] * fft_data[i * 2 + 0] + fft_data[i * 2 + 1] * fft_data[i * 2 + 1]) / sample_size);
                    baseline[i] += fmax(fft_data[i], 0.0f);
                }
                printf("Calibrated: %d/%d\n", i+1, calibration_time);
            }
            if(calibration_time == 0){
                printf("Calibration disabled\n");
            }else{
                for (int i = 0 ; i < sample_size / 2 ; i++) {
                        baseline[i] /= calibration_time;
                }
            }

            /*for (int k = 1; k < sample_size / 2; ++k) {
                printf("baseline: k = %d -> mag = %d |", k, (int)baseline[k]);
                for (int i = 0; i < abs((int)baseline[k]); i++) printf("#");
                printf("\n");
            }          */  
            return 0;
        }
        default:
            return -1;
    }
}

int FFTDevice::ioctl(Command cmd, int16_t* input, void* out) {
    switch(cmd) {
        case Command::read_samples: {
            for (int i = 0 ; i < sample_size ; i++) {
                //printf("gen %d = %d\n", i, gen[i]);
                fft_data[i * 2 + 0] = input[i] * wind[i];
                fft_data[i * 2 + 1] = 0.0f;
            }

            // FFT
            dsps_fft2r_fc32(fft_data, sample_size);
            // Bit reverse
            dsps_bit_rev_fc32(fft_data, sample_size);
            // Convert one complex vector to two complex vectors
            dsps_cplx2reC_fc32(fft_data, sample_size);

            // Correct magnitude calculation after dsps_cplx2reC_fc32
            float* out16 = (float*)out;
            for (int i = 0 ; i < sample_size / 2 ; i++) {
                fft_data[i] = 10 * log10f((fft_data[i * 2 + 0] * fft_data[i * 2 + 0] + fft_data[i * 2 + 1] * fft_data[i * 2 + 1]) / sample_size);
                R[i] = fmax(fft_data[i] - baseline[i], 0.0f);
                out16[i] = R[i];
            }

            // ใช้ fs ใหม่ = fs / down_factor
            //*((double*)out)  = (double)maxIndex * (fs / down_factor) / sample_size;
            return 0;
        }
        case Command::read_freq_bound: {
            dev->ioctl(Command::read_samples, this->input);

            for (int i = 0 ; i < sample_size ; i++) {
                //printf("gen %d = %d\n", i, gen[i]);
                fft_data[i * 2 + 0] = input[i] * wind[i];
                fft_data[i * 2 + 1] = 0.0f;
            }

            // FFT
            dsps_fft2r_fc32(fft_data, sample_size);
            // Bit reverse
            dsps_bit_rev_fc32(fft_data, sample_size);
            // Convert one complex vector to two complex vectors
            dsps_cplx2reC_fc32(fft_data, sample_size);

            // Correct magnitude calculation after dsps_cplx2reC_fc32
            for (int i = 0 ; i < sample_size / 2 ; i++) {
                fft_data[i] = 10 * log10f((fft_data[i * 2 + 0] * fft_data[i * 2 + 0] + fft_data[i * 2 + 1] * fft_data[i * 2 + 1]) / sample_size);
                R[i] = fmax(fft_data[i] - baseline[i], 0.0f);
            }

            int maxIndex = 0;
            double maxMag = 0;

            //printf("bin,Magnitude\n");  // Header
            int kMin = input[0] * sample_size * down_factor / fs;
            int kMax = input[1] * sample_size * down_factor / fs;
            for (int k = kMin; k < kMax; ++k) {
                double mag = R[k];
                /*printf("k = %d -> mag = %d - %d = %d |", k, (int)fft_data[k], (int)baseline[k], (int)R[k]);
                for (int i = 0; i < abs((int)R[k]); i++) printf("#");
                printf("\n");*/
                //printf("%d,%.2f\n", k, R[k]);
                if (mag > maxMag) {
                maxMag = mag;
                maxIndex = k;
                }
            }
            // ใช้ fs ใหม่ = fs / down_factor
            
            *((double*)out)  = (double)maxIndex * (fs / down_factor) / sample_size;
            return 0;
        }
        default:
            return -1;
    }
}