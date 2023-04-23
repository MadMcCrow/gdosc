/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

#include "osc_sender.h"

using namespace osc;

void OSCSender::_register_methods() {
    register_method("setup", &OSCSender::setup);
    register_method("start", &OSCSender::start);
    register_method("stop", &OSCSender::stop);
    register_method("msg", &OSCSender::msg);
    register_method("add", &OSCSender::add);
    register_method("send", &OSCSender::send);
}

OSCSender::OSCSender() :
_port(0), _buffer_size(1024),
_ready(false), _running(false),
_socket(0), _buffer(0), _packet(0), _packet_closed(false) {
}

OSCSender::~OSCSender() {

    reset_message();
    stop();

}

bool OSCSender::setup(godot::String ip, unsigned int port) {

    std::wstring ws = ip.unicode_str();
    std::string s(ws.begin(), ws.end());
    unsigned int p = port;

    if (s.empty()) {
        WARN_PRINT_ED("Invalid osc::OSCSender setup: invalid ip");
        return false;
    }
    if (p <= 0) {
        WARN_PRINT_ED("Invalid osc::OSCSender setup: invalid port");
        return false;
    }

    bool autorestart = false;
    if (_running) {
        stop();
        autorestart = true;
    }

    _ip = s;
    _port = p;
    _endpoint = ip + ":" + godot::String::num(_port);

    _ready = true;

    if (autorestart) {
        return start();
    }

    return true;

}

bool OSCSender::start() {

    if (!_ready) {
        WARN_PRINT_ED("OSCSender::start, failed to start");
        return false;

    }

    stop();

    try {

        IpEndpointName _message_target = IpEndpointName(_ip.c_str(), _port);
        _socket = new UdpTransmitSocket(_message_target);
        WARN_PRINT_ED("OSCSender::start, started successfully on " + _endpoint);

    } catch (std::exception& e) {

        WARN_PRINT_ED("OSCSender::start, failed to start on " + _endpoint);
        //        std::cout << "scsender::stafailed to startrt, failed to starty" << e.what() << std::endl;

    }




    _running = true;
    return true;

}

void OSCSender::stop() {

    if (_socket) {
        delete _socket;
        _socket = 0;
<<<<<<< HEAD
        Godot::print("OSCSender::stop, socket closed on " + _endpoint);
=======
        UtilityFunctions::print("OSCSender::stop, socket closed on " + _endpoint);
>>>>>>> 5b7c248 (WIP build fixes)
    }
    _running = false;

}

void OSCSender::reset_message() {

    if (_packet) {
        delete _packet;
        _packet = 0;
        _packet_closed = false;
    }
    if (_buffer) {
        delete[] _buffer;
        _buffer = 0;
    }

}

void OSCSender::msg(godot::String address) {

    reset_message();

    std::wstring ws = address.unicode_str();
    std::string std_address(ws.begin(), ws.end());

    _buffer = new char[_buffer_size];
    _packet = new osc::OutboundPacketStream(_buffer, _buffer_size);
    (*_packet) << osc::BeginBundleImmediate
            << osc::BeginMessage(std_address.c_str());

}

void OSCSender::add(godot::Variant var) {

    if (!_packet) {
        WARN_PRINT_ED("OSCSender::add, message is not ready! "
                "call 'start_message' first!");
        return;
    }

    switch (var.get_type()) {
        case godot::Variant::Type::BOOL:
        {
            bool b = var;
            (*_packet) << b;
        }
            break;
        case godot::Variant::Type::STRING:
        case godot::Variant::Type::NODE_PATH:
        case godot::Variant::Type::_RID:
        case godot::Variant::Type::OBJECT:
        {
            godot::String s = var;
            (*_packet) << s.alloc_c_string();
        }
            break;
        case godot::Variant::Type::INT:
        {
            int i = var;
            (*_packet) << i;
        }
            break;
        case godot::Variant::Type::REAL:
        {
            float f = var;
            (*_packet) << f;
        }
            break;
        case godot::Variant::Type::VECTOR2:
        {
            godot::Vector2 v = var;
            (*_packet) << float( v.x);
            (*_packet) << float( v.y);
        }
            break;
        case godot::Variant::Type::VECTOR3:
        {
            godot::Vector3 v = var;
            (*_packet) << float( v.x);
            (*_packet) << float( v.y);
            (*_packet) << float( v.z);
        }
            break;
        case godot::Variant::Type::QUAT:
        {
            godot::Quat q = var;
            (*_packet) << float( q.x);
            (*_packet) << float( q.y);
            (*_packet) << float( q.z);
            (*_packet) << float( q.w);
        }
            break;
        case godot::Variant::Type::COLOR:
        {
            godot::Color c = var;
            (*_packet) << float( c.r );
            (*_packet) << float( c.g );
            (*_packet) << float( c.b );
            (*_packet) << float( c.a );
        }
            break;
        default:
            WARN_PRINT_ED("OSCSender::add, unable to add "
                    "this kind of data!");
            break;

    }

}

void OSCSender::send() {

    if (!_packet) {
        WARN_PRINT_ED("OSCSender::send, message is not ready! "
                "call 'start_message' first!");
        return;
    }

    if (!_packet_closed) {
        (*_packet) << osc::EndMessage << osc::EndBundle;
        _packet_closed = true;
    }
    _socket->Send(_packet->Data(), _packet->Size());
}
