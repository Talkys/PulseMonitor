#include <pulse/simple.h>
#include <pulse/error.h>
#include <iostream>
#include <thread>
#include <vector>

constexpr size_t BUFFER_SIZE = 256;
constexpr size_t NUM_BUFFERS = 4;

pa_simple *captura_handle;
pa_simple *reproducao_handle;

std::vector<char> buffer(BUFFER_SIZE * NUM_BUFFERS);
size_t read_index = 0;
size_t write_index = 0;

void capturarAudio() {
    int err;

    while (true) {
        if (write_index - read_index < BUFFER_SIZE) {
            // Captura de áudio
            if (pa_simple_read(captura_handle, &buffer[write_index % buffer.size()], BUFFER_SIZE, &err) < 0) {
                std::cerr << "Erro ao ler do dispositivo de captura: " << pa_strerror(err) << std::endl;
                return;
            }

            write_index += BUFFER_SIZE;
        }
    }
}

void reproduzirAudio() {
    int err;

    while (true) {
        if (write_index - read_index >= BUFFER_SIZE) {
            // Reprodução de áudio
            if (pa_simple_write(reproducao_handle, &buffer[read_index % buffer.size()], BUFFER_SIZE, &err) < 0) {
                std::cerr << "Erro ao escrever no dispositivo de reprodução: " << pa_strerror(err) << std::endl;
                return;
            }

            read_index += BUFFER_SIZE;
        }
    }
}

int main() {
    int err;
    pa_sample_spec captura_spec;
    pa_sample_spec reproducao_spec;

    // Configuração do dispositivo de captura
    captura_spec.format = PA_SAMPLE_S16LE;
    captura_spec.rate = 22050;
    captura_spec.channels = 1; // Altera para 1 canal (mono)

    if (!(captura_handle = pa_simple_new(NULL, "Monitor de Áudio", PA_STREAM_RECORD, NULL, "Captura", &captura_spec, NULL, NULL, &err))) {
        std::cerr << "Erro ao inicializar o dispositivo de captura: " << pa_strerror(err) << std::endl;
        return 1;
    }

    // Configuração do dispositivo de reprodução
    reproducao_spec.format = PA_SAMPLE_S16LE;
    reproducao_spec.rate = 22050;
    reproducao_spec.channels = 1; // Altera para 1 canal (mono)

    if (!(reproducao_handle = pa_simple_new(NULL, "Monitor de Áudio", PA_STREAM_PLAYBACK, NULL, "Reprodução", &reproducao_spec, NULL, NULL, &err))) {
        std::cerr << "Erro ao inicializar o dispositivo de reprodução: " << pa_strerror(err) << std::endl;
        return 1;
    }

    // Iniciar threads de captura e reprodução
    std::thread capturaThread(capturarAudio);
    std::thread reproducaoThread(reproduzirAudio);

    // Aguardar término das threads
    capturaThread.join();
    reproducaoThread.join();

    // Fechando os dispositivos
    pa_simple_free(captura_handle);
    pa_simple_free(reproducao_handle);

    return 0;
}
