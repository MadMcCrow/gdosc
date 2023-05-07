/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

#ifndef OSCMSG_H
#define OSCMSG_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

// TODO : IWYU

#include <OscTypes.h>
#include <UdpSocket.h>
#include <PacketListener.h>
#include <OscReceivedElements.h>
#include <OscPacketListener.h>
#include <OscOutboundPacketStream.h>

using namespace godot;

namespace osc {

    // TODO : possibly move this to CPP
    class oscmsg_data {
    public:

        bool valid;
        // TODO : replace with more sensible data structure 
        Dictionary data;

        oscmsg_data() : valid(false) {
        }

        // TODO : move to CPP
        oscmsg_data(
                const osc::ReceivedMessage& m,
                const IpEndpointName& rep) {

            data[ "address" ] = String(m.AddressPattern());
            data[ "typetag" ] =   String(m.TypeTags());
            char endpointHost[IpEndpointName::ADDRESS_STRING_LENGTH];
            rep.AddressAsString(endpointHost);
            data[ "ip" ] =   String(endpointHost);
            data[ "port" ] = rep.port;

              Array args;
            try {
                for (::osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin(); arg != m.ArgumentsEnd();
                        ++arg) {
                    if (arg->IsInt32()) {
                        args.append(arg->AsInt32Unchecked());
                    } else if (arg->IsFloat()) {
                        args.append(arg->AsFloatUnchecked());
                    } else if (arg->IsString()) {
                        args.append(  String(arg->AsStringUnchecked()));
                    } else if (arg->IsBool()) {
                        args.append(arg->AsBool());
                    } else {
                        args.append(false);
                    }
                }

            } catch (osc::Exception& e) {
                // any parsing errors such as unexpected argument types, or
                // missing arguments get thrown as exceptions.
                  String s = "oscmsg_data, error while parsing message ";
                s += m.AddressPattern();
                s += ": ";
                s += e.what();
                    ERR_FAIL_MSG(s);
            }

            data[ "args" ] = args;
            data[ "arg_num" ] = args.size();
            valid = true;

        }

        oscmsg_data(const oscmsg_data& src) {
            valid = src.valid;
            data = src.data;
        }

        inline void operator=(const oscmsg_data& src) {
            valid = src.valid;
            data = src.data;
        }

    };

    class OSCMsg : public Object {
        GDCLASS(OSCMsg, Object);

    public:

        static void _bind_methods();

        OSCMsg();

        virtual ~OSCMsg();

        void buffer_size(int size);

        void address(String address);

        void add(Variant var);

        void close();

        inline bool ready() const {
            return _packet != 0;
        }

        inline const bool& closed() const {
            return _packet_closed;
        }

        const   PackedByteArray& data() const {
            return _array;
        }

        inline int size() const {
            return _buffer_size;
        }

        inline void is_oscmsg() const {
        }

    protected:

        std::size_t _buffer_size;
        char* _buffer;
        osc::OutboundPacketStream* _packet;
        bool _packet_closed;

        PackedByteArray _array;

        void reset();

    };

};

#endif /* OSCMSG_H */
