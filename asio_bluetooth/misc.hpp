//
//  asio_bluetooth/misc.hpp
//  author: Da Teng
//  email: dan0702@g.ucla.edu
//

#ifndef BOOST_ASIO_BLUETOOTH_MISC_HPP
#define BOOST_ASIO_BLUETOOTH_MISC_HPP

#include <unistd.h>
#include <sys/socket.h>

/* BD Address */
typedef struct {
	uint8_t b[6];
} __attribute__((packed)) bdaddr_t;

/* RFCOMM socket address */
struct sockaddr_rc {
	sa_family_t	rc_family;
	bdaddr_t	rc_bdaddr;
	uint8_t		rc_channel;
};

namespace boost {
namespace asio {
namespace detail {

typedef sockaddr_rc sockaddr_bt_type;

} // namespace detail
} // namespace asio
} // namespace boost

#endif
