/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

// std
#include <map>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

// godot
#include <godot_cpp/variant/utility_functions.hpp>

// gdosc
#include "osc_receiver.h"

using namespace std;
using namespace osc;

void OSCReceiver::_bind_methods() {

    ClassDB::bind_method(D_METHOD("max_queue", "max_queue"), &OSCReceiver::max_queue);
    ClassDB::bind_method(D_METHOD("avoid_duplicate", "enable"), &OSCReceiver::avoid_duplicate);
    ClassDB::bind_method(D_METHOD("setup", "port"), &OSCReceiver::setup);
    ClassDB::bind_method(D_METHOD("start"), &OSCReceiver::start);
    ClassDB::bind_method(D_METHOD("stop"), &OSCReceiver::stop);
    ClassDB::bind_method(D_METHOD("has_message"), &OSCReceiver::has_message);
    ClassDB::bind_method(D_METHOD("get_next"), &OSCReceiver::get_next);
    // TODO : define properties
}

OSCReceiver::OSCReceiver() :
_port(0),
_is_ready(false), _running(false),
_swap_needed(true), _avoid_duplicate(false),
_lsocket(0),
_max_queue(100),
_queue_write(0),
_queue_read(0),
_qread_index(0), _qread_len(0) {
}

OSCReceiver::~OSCReceiver() {
    stop();
}

void OSCReceiver::max_queue(int max_queue) {

    if (max_queue < 1) return;
    _lmutex.lock();
    _max_queue = (std::size_t)max_queue;
    _lmutex.unlock();
    check_queue();

}

void OSCReceiver::avoid_duplicate(bool enabled) {

    _lmutex.lock();
    _avoid_duplicate = enabled;
    _lmutex.unlock();

}

bool OSCReceiver::setup(unsigned int port) {

    if (port <= 0) {
        WARN_PRINT_ED("Invalid osc::OSCReceiver setup: invalid port");
        return false;
    }

    bool autorestart = false;
    if (_running) {
        stop();
        autorestart = true;
    }

    _port = port;

    _is_ready = true;

    if (autorestart) {
        return start();
    }

    return true;

}

bool OSCReceiver::start() {

    if (!_is_ready) {
        WARN_PRINT_ED("OSCReceiver::start, receiver is not ready, call setup() first!");
        return false;
    }

    stop();

    try {
        IpEndpointName name(IpEndpointName::ANY_ADDRESS, _port);
        _lsocket = new UdpListeningReceiveSocket(name, this);
    } catch (const std::exception& e) {
        godot::String s = "OSCReceiver::start, failed to start on ";
        s += godot::String::num(_port);
        s += "\n";
        s += e.what();
        WARN_PRINT_ED(s);
        return false;
    }

    create_buffers();

    _lthread = std::thread([this] {
        while (_lsocket) {
            try {
                _lsocket->Run();
                std::this_thread::yield();
            } catch (std::exception& e) {
                godot::String s = "OSCReceiver::_lthread, cannot listen ";
                s += e.what();
                WARN_PRINT_ED(s);
            }
        }
        godot::String s = "OSCReceiver::_lthread, thread is quitting on ";
        s += godot::String::num(_port);
        UtilityFunctions::print(s);
    });

    _lthread.detach();

    _running = true;
    godot::String s = "OSCReceiver::start, successfully started on ";
    s += godot::String::num(_port);
    UtilityFunctions::print(s);

    return true;

}

void OSCReceiver::stop() {

    if (_lsocket) {
        _lsocket->Break();
        delete _lsocket;
        _lsocket = 0;
    }

    if (_running) {

        purge_buffers();

        _running = false;

        godot::String s = "OSCReceiver::stop, stopped on ";
        s += godot::String::num(_port);
        UtilityFunctions::print(s);

    }
}

void OSCReceiver::create_buffers() {

    _lmutex.lock();
    if (!_queue_write) {
        _queue_write = new std::deque<oscmsg_data>();
        _queue_read = new std::deque<oscmsg_data>();
    }
    _lmutex.unlock();

}

void OSCReceiver::purge_buffers() {

    if (_queue_write) {
        _queue_write->clear();
        _queue_read->clear();
        delete _queue_write;
        delete _queue_read;
        _queue_write = 0;
        _queue_read = 0;
    }

}

void OSCReceiver::swap_buffers() {

    _queue_read->clear();

    _lmutex.lock();
    std::deque<oscmsg_data>* tmp = _queue_write;
    _queue_write = _queue_read;
    _queue_read = tmp;
    _lmutex.unlock();

    _qread_index = 0;
    _qread_len = _queue_read->size();

}

void OSCReceiver::ProcessMessage(
        const osc::ReceivedMessage& m,
        const IpEndpointName& rep) {

    oscmsg_data msg(m, rep);
    if (msg.valid) {
        _lmutex.lock();
        _queue_write->push_back(msg);
        _lmutex.unlock();
        check_queue();
    }

}

void OSCReceiver::check_queue() {

    _lmutex.lock();
    if (_queue_write && _queue_write->size() > _max_queue) {
        _queue_write->resize(_max_queue);
    }
    _lmutex.unlock();

}

void OSCReceiver::purge_duplicates() {

    if ( _queue_read->size() < 2 ) {
        return;
    }

    std::map< godot::String, oscmsg_data > map;

    std::deque<oscmsg_data> tmp;
    tmp.swap(*_queue_read);
    size_t tmp_size = tmp.size();

    std::deque<oscmsg_data>::iterator it = tmp.begin();
    std::deque<oscmsg_data>::iterator ite = tmp.end();
    for (; it != ite; ++it) {
        map[ (*it).data["address"] ] = (*it);
    }

    std::map< godot::String, oscmsg_data >::iterator mi = map.begin();
    std::map< godot::String, oscmsg_data >::iterator mie = map.end();
    for (; mi != mie; ++mi) {
        _queue_read->push_back(mi->second);
    }

    // updating the indices
    _qread_index = 0;
    _qread_len = _queue_read->size();

}

bool OSCReceiver::has_message() {

    if (_swap_needed) {
        swap_buffers();
        if (_avoid_duplicate) {
            purge_duplicates();
        }
        _swap_needed = false;
    }

    if (_qread_index == _qread_len) {
        _swap_needed = true;
        return false;
    }

    return true;

}

godot::Dictionary OSCReceiver::get_next() {

    if (_qread_index == _qread_len) {
        return godot::Dictionary();
    }
    return _queue_read->at(_qread_index++).data;

}
