/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>


#include "osc_msg.h"

using namespace osc;

void OSCMsg::_register_methods() {
    register_method("buffer_size", &OSCMsg::buffer_size);
    register_method("address", &OSCMsg::address);
    register_method("add", &OSCMsg::add);
    register_method("close", &OSCMsg::close);
    register_method("data", &OSCMsg::data);
    register_method("size", &OSCMsg::size);
    register_method("ready", &OSCMsg::ready);
    register_method("closed", &OSCMsg::closed);
    register_method("is_oscmsg", &OSCMsg::is_oscmsg);
}

OSCMsg::OSCMsg() :
_buffer_size(1024),
_buffer(0), _packet(0),
_packet_closed(false) {

    std::cout << this << std::endl;
    std::cout << this->owner << std::endl;

}

OSCMsg::~OSCMsg() {
}

void OSCMsg::reset() {

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

void OSCMsg::buffer_size(int size) {

    if (size <= 0) {
        WARN_PRINT_ED("OSCMsg::buffer_size, wrong buffer_size!");
        return;
    }

    reset();

    _buffer_size = size;

}

void OSCMsg::address(godot::String address) {

    reset();

    std::wstring ws = address.unicode_str();
    std::string std_address(ws.begin(), ws.end());

    _buffer = new char[_buffer_size];
    _packet = new osc::OutboundPacketStream(_buffer, _buffer_size);
    (*_packet) << osc::BeginBundleImmediate
            << osc::BeginMessage(std_address.c_str());

}

void OSCMsg::add(godot::Variant var) {

    if (!_packet) {
        WARN_PRINT_ED("OSCMsg::add, message is not ready! "
                "call 'address' first!");
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
            (*_packet) << float( c.r);
            (*_packet) << float( c.g);
            (*_packet) << float( c.b);
            (*_packet) << float( c.a);
        }
            break;
        default:
            WARN_PRINT_ED("OSCSender::add, unable to add "
                    "this kind of data!");
            break;

    }

}

void OSCMsg::close() {

    if (!_packet_closed) {

        (*_packet) << osc::EndMessage << osc::EndBundle;

        _array.resize(0);
        for ( std::size_t i = 0; i < _buffer_size; ++i ) {
            _array.push_back( (unsigned char) _packet->Data()[i] );
        }

        _packet_closed = true;

    }

}
