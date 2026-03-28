//#include <windows.h> boosts includes it himself
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


unsigned int Keydown(INPUT* event, unsigned int Key){
    (*event).type = INPUT_KEYBOARD;
    (*event).ki.wVk = Key;
    (*event).ki.dwFlags = 0;// key down (default)
    SendInput(1, event, sizeof(INPUT));
    return Key;
}

unsigned int Keyup(INPUT* event,unsigned int Key){
    (*event).type = INPUT_KEYBOARD;// i don't like the "->" thing it doesn't look good
    (*event).ki.wVk = Key;
    (*event).ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, event, sizeof(INPUT));
    return Key;
}

int main() {
    // Prepare the variable
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(atoi("8082"));
    INPUT events;

    int32_t KeyData[2];
    int32_t keyCode;
    int32_t isPressed;

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
                while(true){
                    beast::flat_buffer buffer;
                    // Read a message
                    ws.read(buffer);

                    //this is the key part
                    data = static_cast<const char*>(buffer.cdata().data());
                    //int32_t KeyData[2];
                    memcpy(KeyData, data, sizeof(KeyData));

                    isPressed = KeyData[0]; // 1 = keydown, 0 = keyup
                    keyCode = KeyData[1]; //the code for the button that got pressed

                    if(isPressed){
                        Keydown(&events,keyCode);
                    }else{
                        Keyup(&events,keyCode);
                    }
                }
            }
        )}.detach();
    }
}
