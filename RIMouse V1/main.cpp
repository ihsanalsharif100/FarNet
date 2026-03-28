//#include <Windows.h> boosts includes it
#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <string>
#include <thread>

/*
you only need
-lws2_32
and the boost path in the compiler path
*/

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace std;//this is bad practice blah blah bla

const DWORD DownWords[] = { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_RIGHTDOWN };
const DWORD UpWords[]   = { MOUSEEVENTF_LEFTUP,   MOUSEEVENTF_MIDDLEUP,   MOUSEEVENTF_RIGHTUP   };
//why can't i send the codes like normal in js

void Keydown(INPUT* event, unsigned int Key){
    (*event).type = INPUT_MOUSE;
    (*event).mi.dwFlags = DownWords[Key];// key down (default)
    SendInput(1, event, sizeof(INPUT));
}

void Keyup(INPUT* event,unsigned int Key){
    (*event).type = INPUT_MOUSE;// i don't like the "->" thing it doesn't look good
    (*event).mi.dwFlags = UpWords[Key];
    SendInput(1, event, sizeof(INPUT));
}

void click(int32_t button,int32_t mode , INPUT* event){//do i need to make it int32 any way??
if(mode){
    Keydown(event , button);
}else{
    Keyup(event , button);
}
}

int main() {
    //here we set the variables
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(atoi("8081"));
    //POINT MousePos; not needed
    INPUT events;

    int32_t MouseData[3];
    int32_t XPos;
    int32_t YPos;
    int32_t EventData;//this was named Data but i changed it so no confusion will happen between Data and data

    const char* data = nullptr;

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};

    while(true){

            tcp::socket socket{ioc};
            acceptor.accept(socket);
            thread{bind([&, q{move(socket)}]() mutable {

                websocket::stream<tcp::socket> ws{move(const_cast<tcp::socket&>(q))};

                ws.set_option(websocket::stream_base::decorator([](websocket::response_type& res){
                    res.set(http::field::server,string(BOOST_BEAST_VERSION_STRING) +" websocket-server-sync");
                }));

                ws.accept();

                //sends size
                struct ScreenSize {
                    int width;
                    int height;
                };

                ScreenSize size = {
                    GetSystemMetrics(SM_CXSCREEN),
                    GetSystemMetrics(SM_CYSCREEN)
                };

                ws.binary(true);
                ws.write(net::buffer(&size, sizeof(ScreenSize)));
                //screen size sended

                while(true){
                    beast::flat_buffer buffer;
                    // Read a message
                    ws.read(buffer);

                    //this is the key part
                    data = static_cast<const char*>(buffer.cdata().data());
                    //int32_t KeyData[2];
                    memcpy(MouseData, data, sizeof(MouseData));

                    XPos = MouseData[0]; // 1 = keydown, 0 = keyup
                    YPos = MouseData[1]; //the code for the button that got pressed
                    EventData = MouseData[2]; //i could move around sending a 3rd value but it is batter this way
                    if(EventData){
                        cout << XPos << endl << YPos << endl << EventData << endl << endl;
                        SetCursorPos(XPos,YPos);
                    }else{
                        click(XPos , YPos , &events);//turn it off for debugging so the laptop doesn't crash
                        cout << "click :" << XPos << endl << YPos << EventData << endl << endl;
                    }
                    //GetCursorPos(&MousePos);
                    //std::cout << "mouse X: " << MousePos.x  << std::endl << "mouse Y: " << MousePos.y  << std::endl;
                }
            }
        )}.detach();
    }
    return 0;
}
