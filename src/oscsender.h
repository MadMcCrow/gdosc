/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

#ifndef OSCSENDER_H
#define OSCSENDER_H


#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

#include "oscmsg.h"

// potential namespace pollution
// using namespace godot;
// check that building with GODOT-CPP works that way

namespace osc {

    class oscsender : public godot::Node {
        GODOT_CLASS(oscsender, Node)

    public:

        static void _register_methods();

        oscsender();

        virtual ~oscsender();

        bool setup(godot::String ip, unsigned int port);

        bool start();

        void stop();

        void msg( godot::String address );

        void add( godot::Variant var );

        void send();

    protected:

        std::string _ip;
        int _port;
        std::size_t _buffer_size;
        godot::String _endpoint;
        bool _ready;
        bool _running;

        UdpTransmitSocket* _socket;

        char* _buffer;
        osc::OutboundPacketStream* _packet;
        bool _packet_closed;

        void reset_message();

    };
};

#endif /* OSCSENDER_H */
