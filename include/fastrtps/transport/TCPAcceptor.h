// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _TCP_ACCEPTOR_BASE_
#define _TCP_ACCEPTOR_BASE_

#include <asio.hpp>
#include <fastrtps/rtps/common/Locator.h>

namespace eprosima{
namespace fastrtps{
namespace rtps{

class TCPTransportInterface;

/**
 * Common class to wrap ASIO acceptors.
 */
class TCPAcceptor
{
protected:
    asio::ip::tcp::acceptor acceptor_;
    Locator_t locator_;
    asio::ip::tcp::endpoint endpoint_;
    std::vector<Locator_t> pending_out_locators_;
    asio::io_service* io_service_;

public:
    TCPAcceptor(
        asio::io_service& io_service,
        TCPTransportInterface* parent,
        const Locator_t& locator);

    TCPAcceptor(
        asio::io_service& io_service,
        const std::string& interface_name,
        const Locator_t& locator);

    const Locator_t& locator() const
    {
        return locator_;
    }

    Locator_t& locator()
    {
        return locator_;
    }

	virtual ~TCPAcceptor() = default;
};


} // namespace rtps
} // namespace fastrtps
} // namespace eprosima

#endif // _TCP_ACCEPTOR_BASE_
