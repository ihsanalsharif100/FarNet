#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

//here we add the librarys
#include <iostream>
//this so we can take a screenshot
#include <windows.h>

//this is for the time.sleep
#include <thread>
#include <chrono>

//this is so we can have strings
#include <string>

//this writes file so we know the stats (only for config, not images)
#include <fstream>

//this is so the data works and gets sets
#include <vector>

//this is for the file_exsists
#include <filesystem>

#define TRUE 1
#define FALSE 0

//i don't know what does are but they are
#include <objidl.h>
#include <ole2.h>
#include <propidl.h>


//include gdi+
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace std;

///FarNet screen stream system Version 2
//changes from V1
//removed MakeTimeLine line from data.txt as it became useless
//increased speed by turning the screen into a jpg
//lowered network bottleneck by reducing sent image size to ~2-60% of original size
//made an adjustable quality setting
//removed useless parts of code
//removed screenshot variable

void setup(){
    cout << "there was no data file creating one"<< endl;

    ofstream data_check("data.txt");
    data_check
    << "10" << endl
    << "1000" << endl
    << "10" << endl
    << "1000" << endl
    << "8084";
    data_check.close();

    ofstream data_sort_check("data_sort.txt");
    data_sort_check
    << "RSystem - how many times it will run before reloading data.txt settings" << endl
    << "Speed - how many ms will it wait before running again" << endl
    << "quality - from 0 to 100 / 0 is the worst and 100 is the best" << endl
    << "Sleep time - the time the program stops between every reload of the settings in ms" << endl
    << "port - the port it will run at you need to restart the app every time you change it";
    data_sort_check.close();

    ofstream HowToUse("HowToUse.txt");
    HowToUse
    << "this program sends your current screen into a websocket server" << endl << endl
    << "to use this program you only need to run the file" << endl
    << "and it will set it self up automatically" << endl << endl
    << "but it will only run at the default settings to change the default settings" << endl
    << "open data.txt and to know what each line does you need to open data_sort.txt";

}


//----------------------------sets up the gdi------------------------------------------------------
ULONG_PTR gdiToken;

void InitGDIPlus(){
    GdiplusStartupInput input;
    GdiplusStartup(&gdiToken, &input, NULL);
}

void ShutdownGDIPlus()
{
    GdiplusShutdown(gdiToken);
}
//---------------------------------end of the code that sets up the gdi-----------------------------
//------------------- gets the jpg encoder??------------------------------------
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid){
    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);

    if (size == 0) return -1;

    vector<BYTE> buffer(size);
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)buffer.data();
    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            return j;
        }
    }

    return -1;
}
//the comments look missy
//------------------------------end of function------------------------------------

//--------------------------------capture jpg funcion---------------------------------
//the new function looks cleaner

//NEW: Captures screenshot and returns jpg data

//No files are ever written to disk (entirely in memory)
vector<unsigned char> captureScreenshotJPEG(int quality){
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    SelectObject(memDC, hBitmap);

    BitBlt(memDC, 0, 0, width, height, screenDC, 0, 0, SRCCOPY);

    // Convert HBITMAP → GDI+ Bitmap
    Bitmap bitmap(hBitmap, NULL);

    CLSID jpgClsid;
    GetEncoderClsid(L"image/jpeg", &jpgClsid);

    // Set JPEG quality
    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    ULONG q = quality;
    encoderParams.Parameter[0].Value = &q;

    // Create memory stream
    IStream* stream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &stream);

    bitmap.Save(stream, &jpgClsid, &encoderParams);

    // Get stream size
    STATSTG stat;
    stream->Stat(&stat, STATFLAG_NONAME);
    ULONG size = stat.cbSize.LowPart;

    vector<unsigned char> jpegData(size);

    LARGE_INTEGER pos = {};
    stream->Seek(pos, STREAM_SEEK_SET, NULL);
    stream->Read(jpegData.data(), size, NULL);

    stream->Release();

    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);

    return jpegData;
}
//-------------------------------------------------------end of function-------------------------


int main(){
    cout << "running" << endl;
    Sleep(1000);

    cout << "inilizing GDI" << endl << "inilizing GDI Plus" << endl;
    InitGDIPlus();
    //int screenshotNumber = 0;
    int NumberOfRepeat = 0;
    Sleep(2000);
    cout << "GDI inilized" << endl << "GDI Plus inilized";

    cout << "checking needed files" << endl;
    Sleep(3000);
    //make sure data file exsist
    if(filesystem::exists("data.txt")){
        cout << "all needed files found" << endl;
    }else{
        setup();
    }

    cout << "setting up the settings" << endl;
    Sleep(1000);
    ifstream file("data.txt");
    vector<string> lines;
    string line;

    while (getline(file, line)) {
        lines.push_back(line);
    }
    int RSystem = stoi(lines[0]);
    int speed = stoi(lines[1]);
    int quality = stoi(lines[2]);
    int SleepTime = stoi(lines[3]);
    int PortNumber = stoi(lines[4]);//this wont change while running
    cout << "settings setup complete" << endl;

    // =================================== ALL IMAGE DATA =======================================
    vector<unsigned char> lastScreenshotData;   // <--- this holds the full BMP \
    char (signed) — stores -128 to 127 — wastes half the range on useless negative numbers \
    unsigned char — stores 0 to 255 — perfectly maps to pixel values


    cout << "running websocket" << endl;
    Sleep(400);
//---------------------------------------setting up the websocket---------------------------
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(PortNumber);

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};

    for(;;){

        tcp::socket socket{ioc};

        acceptor.accept(socket);
        thread{bind(
            //[q = std::move(socket)]() mutable { // socket will be const - mutable should be used
            //& means capture everything by reference
            [&, q{move(socket)}]() mutable { // socket will be const - mutable should be used



            websocket::stream<tcp::socket> ws{move(const_cast<tcp::socket&>(q))};

            // Set a decorator to change the Server of the handshake
            // no need to set. It ıs not necessary
            ws.set_option(websocket::stream_base::decorator([](websocket::response_type& res){
                    res.set(http::field::server, string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");
            }));

                cout << "websocket on" << endl;
                // Accept the websocket handshake
                ws.accept();
                while(true){
                    NumberOfRepeat +=1;
                    //this isn't useful any more \
                    screenshotNumber += 1;

                    // Capture into the variable (no disk write!)
                    lastScreenshotData = captureScreenshotJPEG(quality);

                    ws.binary(true);
                    ws.write(net::buffer(
                        lastScreenshotData
                    ));

                    // If you still want the timeline concept, you could store copies here,
                    // but it will eat RAM very quickly:
                    // if(MakeTimeLine) timeline.push_back(lastScreenshotData);

                    if(RSystem > 0 && RSystem <= NumberOfRepeat){
                        ifstream reloadFile("data.txt");
                        lines.clear();
                        while (getline(reloadFile, line)){
                            lines.push_back(line);
                        }
                        RSystem = stoi(lines[0]);
                        speed = stoi(lines[1]);
                        quality = stoi(lines[2]);
                        SleepTime = stoi(lines[3]);
                        NumberOfRepeat = 0;
                        cout << "reloaded system settings";
                        this_thread::sleep_for(chrono::milliseconds(SleepTime));

                        cout << "security check";

                        //ios::binary flag makes it safe
                        ofstream security_file("output.jpg", ios::binary);  // binary flag is essential

                        security_file.write(
                            reinterpret_cast<const char*>(lastScreenshotData.data()),
                            lastScreenshotData.size()
                        );
                        security_file.close();
                    }

                    this_thread::sleep_for(chrono::milliseconds(speed));
                }
            }
        )}.detach();
    }
}
