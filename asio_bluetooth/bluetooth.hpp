//
// bluetooth.hpp
//
//
//
//

#ifndef BOOST_ASIO_BLUETOOTH_HPP
#define BOOST_ASIO_BLUETOOTH_HPP

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/basic_socket_iostream.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/detail/socket_types.hpp>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

namespace boost {

namespace bluetooth {

/**
 *  The boost::asio::bluetooth contains class necessary for Bluetooth Sockets.
 *
 *
 */
class bluetooth
{
public:
  // Construct with a specific family
  explicit bluetooth(int family)
    : family_(family)
  {
    
  }


  // Obtain the type of the protocol, corresponding to the type argument
  // of POSIX socket()
  int type() const
  {
    return SOCK_STREAM;
  }


  // Obtain identifier of the protocol, corresponding to protocol argument
  // of POSIX socket()
  int protocol() const
  {
    return BTPROTO_RFCOMM;
  }


  // Obtain the identifier of the protocol family, corresponding to the
  // domain argument of POSIX socket()
  int family() const
  {
    return family_;
  }

  friend bool operator==(const bluetooth& p1, const bluetooth& p2)
  {
    return p1.family_ == p2.family_;
  }

  friend bool operator!=(const bluetooth& p1, const bluetooth& p2)
  {
    return p1.family_ != p2.family_;
  }


private:


  int family_;
};

} // namespace bluetooth

} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif
