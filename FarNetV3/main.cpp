#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>

#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <filesystem>


///FarNet sound stream system Version 2
//changes from V1
//removed the timer for efficiency
//removed unneeded librares for faster use
//increased efficiency by removing unused and usless code

//-mwindows


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace std;

// ===================== USER DATA ======================
struct RecordingUserData {
    std::vector<float>* recordedData;
    bool running;
};

string status = "wait";
string data_file_name = "data.txt";


int quality = 44100;
//int Number = 0;
int NumberOfRepeat = 0;

HANDLE pipe;
RecordingUserData gUserData;
ma_device gDevice;
std::vector<float> gRecordedSamples;
bool gInitialized = false;

// ===================== STATUS WRITER ======================
void status_func(const string& stat) {
    ofstream file("status.txt");
    file << stat;
    status = stat;
}

// ===================== AUDIO CALLBACK =====================
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    RecordingUserData* data = (RecordingUserData*)pDevice->pUserData;

    if (data->recordedData != nullptr) {

        const float* f = (const float*)pInput;
        size_t count = frameCount * pDevice->capture.channels;

        for (size_t i = 0; i < count; i++) {
            data->recordedData->push_back(f[i]);
        }
    }

    (void)pOutput;
}


// ===================== AUDIO CONFIG =====================
struct AudioConfig {
    int sampleRate;
    ma_format format;
    int channels;
};

// ===================== INIT =====================
bool initRecorder(AudioConfig cfg) {
    gUserData.recordedData = &gRecordedSamples;

    ma_device_config devCfg = ma_device_config_init(ma_device_type_loopback);
    devCfg.capture.format   = cfg.format;
    devCfg.capture.channels = cfg.channels;
    devCfg.sampleRate       = cfg.sampleRate;
    devCfg.dataCallback     = data_callback;
    devCfg.pUserData        = &gUserData;

        // initialize device (this was missing)
    if (ma_device_init(NULL, &devCfg, &gDevice) != MA_SUCCESS) {
        cout << "ma_device_init failed\n";
        gInitialized = false;
        return false;
    }


    gInitialized = true;
    return true;
}

// ===================== RECORD CONTROL =====================
void startRecording() {
    if (!gInitialized) return;
    ma_device_start(&gDevice);
}

void stopRecording() {
    ma_device_uninit(&gDevice);

    gInitialized = false;
}

// ===================== DEFAULT CONFIG CREATOR =====================
void setup() {
    cout << "data.txt not found — creating default" << endl ;
    ofstream d(data_file_name);
    d << "10" << endl << "1000" << endl << "auto" << endl << "44100" << endl << "1000";
    d.close();

    ofstream s("data_sort.txt");
    s <<
        "RSystem cycles" << endl <<
        "Delay ms" << endl <<
        "Mode auto/manual" << endl <<
        "SampleRate" << endl <<
        "Record length ms";
    s.close();
}

//note: it is spelled "variable" \
================================= MAIN ==================================

int main(int argc, char* argv[]){

//----------------------audio and other stuff-----------------------------
    if (!filesystem::exists(data_file_name)) setup();
    //we should try and make most of the code here

    //this sets some thing up
    // Reload configuration
    ifstream file(data_file_name);
    vector<string> lines;
    string line;
    while (getline(file, line)) lines.push_back(line);
    int RSystem = stoi(lines[0]);
    int speed = stoi(lines[1]);
    string type = lines[2];
    quality = stoi(lines[3]);
    int recordMs = stoi(lines[4]);
    cout << "Reloading" << endl;

    AudioConfig cfg = { quality, ma_format_f32, 1 };
//-----------------------------end--------------------------------------


//--------------------setting up the websocket---------------------------
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi("8083"));

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    for(;;){

        tcp::socket socket{ioc};

        acceptor.accept(socket);

        std::thread{std::bind(
            //[q = std::move(socket)]() mutable { // socket will be const - mutable should be used
            //& means capture everything by reference
            [&, q{std::move(socket)}]() mutable { // socket will be const - mutable should be used



            websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))};

            // Set a decorator to change the Server of the handshake
            // no need to set. It ıs not necessary
            ws.set_option(websocket::stream_base::decorator([](websocket::response_type& res){

                    res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");

                }));

                    // Accept the websocket handshake
                    ws.accept();



//----------------------------------the loop----------------------------------------------------------------------
while (true){
        //Number++;
        NumberOfRepeat++;
        //status_func("writing");

        if (!initRecorder(cfg)) {
            cout << "Failed to init recorder, exiting\n";
        }

        // --- Start recording ---
        startRecording();
        // --- Timer thread ---
        if (RSystem > 0 && NumberOfRepeat >= RSystem) {
            NumberOfRepeat = 0;
            ifstream file(data_file_name);
            vector<string> lines;
            string line;
            while (getline(file, line)) lines.push_back(line);
                RSystem = stoi(lines[0]);
                speed = stoi(lines[1]);
                type = lines[2];
                quality = stoi(lines[3]);
                recordMs = stoi(lines[4]);
                cout << "Reload config\n";

        }


        //wait
        this_thread::sleep_for(chrono::milliseconds(recordMs));

        stopRecording();

        cout << "Samples collected: " << gRecordedSamples.size() << "\n";
        //cout << "Sending " << gRecordedSamples.size() * sizeof(int16_t) << " bytes\n";

        /*std::string message =  gRecordedSamples.size() * sizeof(int16_t);
        ws.text(true); // tell websocket this is text
        ws.write(net::buffer(message));
        */
        //(char*)gRecordedSamples.data(),

        ws.binary(true);
        ws.write(net::buffer(
            gRecordedSamples.data(),
            gRecordedSamples.size() * sizeof(float)
        ));

        // --- Clean up ---

        //status_func("done");

        gRecordedSamples.clear();
        gUserData.recordedData = &gRecordedSamples;   // <--- VERY IMPORTANT

        this_thread::sleep_for(chrono::milliseconds(speed));
        AudioConfig cfg = { quality, ma_format_f32, 1 };
}
//--------------------------------------end of the loop-----------------------------------------------------------
            }
        )}.detach();
        }

    return 0;
}
