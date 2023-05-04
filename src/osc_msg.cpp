/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>


#include "osc_msg.h"
// Variants
#include <godot_cpp/variant/variant.hpp>

using namespace osc;

void OSCMsg::_bind_methods() {
    // register all methods
    ClassDB::bind_method(D_METHOD("buffer_size", "size"), &OSCMsg::buffer_size);
    ClassDB::bind_method(D_METHOD("address", "address"),  &OSCMsg::address);
    ClassDB::bind_method(D_METHOD("add", "var"),          &OSCMsg::add);
    ClassDB::bind_method(D_METHOD("close"),               &OSCMsg::close);
    ClassDB::bind_method(D_METHOD("data"),                &OSCMsg::data);
    ClassDB::bind_method(D_METHOD("size"),                &OSCMsg::size);
    ClassDB::bind_method(D_METHOD("ready"),               &OSCMsg::ready);
    ClassDB::bind_method(D_METHOD("closed"),              &OSCMsg::closed);
    ClassDB::bind_method(D_METHOD("is_oscmsg"),           &OSCMsg::is_oscmsg);
}

OSCMsg::OSCMsg() :
_buffer_size(1024),
_buffer(0), _packet(0),
_packet_closed(false) {

    std::cout << this << std::endl;
    std::cout << this->_owner << std::endl;

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
    std::string std_address(address.utf8().get_data());

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
        case godot::Variant::Type::RID:
        case godot::Variant::Type::OBJECT:
        {
            godot::String s = var;
            (*_packet) << s.utf8().get_data();
        }
            break;
        case godot::Variant::Type::INT:
        {
            int i = var;
            (*_packet) << i;
        }
            break;
        case godot::Variant::Type::FLOAT:
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
        case godot::Variant::Type::QUATERNION:
        {
            godot::Quaternion q = var;
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
