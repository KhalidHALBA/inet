//
// Copyright (C) 2017 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/networklayer/common/L3Tools.h"

#ifdef WITH_IPv4
#include "inet/networklayer/ipv4/IPv4Header.h"
#endif
#ifdef WITH_IPv6
#include "inet/networklayer/ipv6/IPv6Header.h"
#endif
#ifdef WITH_GENERIC
#include "inet/networklayer/generic/GenericDatagram.h"
#endif

namespace inet {

const Protocol *findNetworkProtocol(Packet *packet)
{
    auto networkProtocolInd = packet->getTag<NetworkProtocolInd>();
    return networkProtocolInd == nullptr ? nullptr : networkProtocolInd->getProtocol();
}

const Protocol& getNetworkProtocol(Packet *packet)
{
    auto protocol = findNetworkProtocol(packet);
    if (protocol == nullptr)
        throw cRuntimeError("Network protocol not found");
    else
        return *protocol;
}

const Ptr<const NetworkHeaderBase> findNetworkProtocolHeader(Packet *packet)
{
    auto networkProtocolInd = packet->getTag<NetworkProtocolInd>();
    return networkProtocolInd == nullptr ? nullptr : std::dynamic_pointer_cast<const NetworkHeaderBase>(networkProtocolInd->getNetworkProtocolHeader());
}

const Ptr<const NetworkHeaderBase> getNetworkProtocolHeader(Packet *packet)
{
    const auto& header = findNetworkProtocolHeader(packet);
    if (header == nullptr)
        throw cRuntimeError("Network protocol header not found");
    else
        return header;
}

const Ptr<const NetworkHeaderBase> peekNetworkProtocolHeader(Packet *packet, const Protocol& protocol)
{
#ifdef WITH_IPv4
    if (protocol == Protocol::ipv4)
        return packet->peekHeader<IPv4Header>();
#endif
#ifdef WITH_IPv6
    if (protocol == Protocol::ipv6)
        return packet->peekHeader<IPv6Header>();
#endif
#ifdef WITH_GENERIC
    if (protocol == Protocol::gnp)
        return packet->peekHeader<GenericDatagramHeader>();
#endif
    // TODO: add other L3 protocols
    throw cRuntimeError("Unknown protocol: %s", protocol.getName());
}

void insertNetworkProtocolHeader(Packet *packet, const Protocol& protocol, const Ptr<NetworkHeaderBase>& header)
{
    auto networkProtocolInd = packet->ensureTag<NetworkProtocolInd>();
    networkProtocolInd->setProtocol(&protocol);
    networkProtocolInd->setNetworkProtocolHeader(header);
    insertProtocolHeader(packet, protocol, header);
}

const Ptr<NetworkHeaderBase> removeNetworkProtocolHeader(Packet *packet, const Protocol& protocol)
{
#ifdef WITH_IPv4
    if (protocol == Protocol::ipv4)
        return removeNetworkProtocolHeader<IPv4Header>(packet);
#endif
#ifdef WITH_IPv6
    if (protocol == Protocol::ipv6)
        return removeNetworkProtocolHeader<IPv6Header>(packet);
#endif
#ifdef WITH_GENERIC
    if (protocol == Protocol::gnp)
        return removeNetworkProtocolHeader<GenericDatagramHeader>(packet);
#endif
    // TODO: add other L3 protocols
    throw cRuntimeError("Unknown protocol: %s", protocol.getName());
}

} // namespace inet

