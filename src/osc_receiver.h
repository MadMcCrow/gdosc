/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

#ifndef OSCRECEIVER_H
#define OSCRECEIVER_H


#include <deque>
#include <mutex>
#include <thread>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

#include "osc_msg.h"

// we must use this namespace if we want to compile against godot
using namespace godot;

namespace osc {

class OSCReceiver : public Node, public osc::OscPacketListener {
    GDCLASS(OSCReceiver, Node);

    public:

        static void _bind_methods();

        OSCReceiver();

        virtual ~OSCReceiver();

        void max_queue(int max_queue);

        void avoid_duplicate( bool enabled );

        bool setup(unsigned int port);

        bool start();

        void stop();

        bool has_message();

        Dictionary get_next();

    protected:

        int _port;
        bool _is_ready;
        bool _running;
        bool _swap_needed;
        bool _avoid_duplicate;

        // real processing stuff

        std::mutex _lmutex;
        std::thread _lthread;
        UdpListeningReceiveSocket* _lsocket;

        std::size_t _max_queue;

        std::deque<oscmsg_data>* _queue_write;
        std::deque<oscmsg_data>* _queue_read;
        std::size_t _qread_index;
        std::size_t _qread_len;

        void ProcessMessage(
                const osc::ReceivedMessage& m,
                const IpEndpointName& remoteEndpoint);

        void create_buffers();
        void purge_buffers();
        void swap_buffers();
        void check_queue();
        void purge_duplicates();

    };

};

#endif /* OSCRECEIVER_H */
