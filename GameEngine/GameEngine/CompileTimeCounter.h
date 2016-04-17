#pragma once

/**
* Found on stackoverflow, user Potatoswatter
* http://stackoverflow.com/questions/6166337/does-c-support-compile-time-counters
*
* Usage example:

struct mycounter{};
constexpr int k = COUNTER_READ(mycounter);
COUNTER_INC(mycounter);

**/

#define COUNTER_READ_CRUMB( TAG, RANK, ACC ) counter_crumb( TAG(), constant_index< RANK >(), constant_index< ACC >() )
#define COUNTER_READ( TAG ) COUNTER_READ_CRUMB( TAG, 1, COUNTER_READ_CRUMB( TAG, 2, COUNTER_READ_CRUMB( TAG, 4, COUNTER_READ_CRUMB( TAG, 8, \
    COUNTER_READ_CRUMB( TAG, 16, COUNTER_READ_CRUMB( TAG, 32, COUNTER_READ_CRUMB( TAG, 64, COUNTER_READ_CRUMB( TAG, 128, 0 ) ) ) ) ) ) ) )

#define COUNTER_INC( TAG ) \
constant_index< COUNTER_READ( TAG ) + 1 > \
constexpr counter_crumb( TAG, constant_index< ( COUNTER_READ( TAG ) + 1 ) & ~ COUNTER_READ( TAG ) >, \
          					constant_index< ( COUNTER_READ( TAG ) + 1 ) & COUNTER_READ( TAG ) > ) { return {}; }

#define COUNTER_LINK_NAMESPACE( NS ) using NS::counter_crumb;

#define REGISTERED_CLASS( TAG, KEY, NAME ) KEY NAME &register_type( TAG, decltype( COUNTER_READ( TAG ) ) ); \
	COUNTER_INC( TAG ) KEY NAME /* class definition follows */

#include <utility>

template< std::size_t n >
struct constant_index : std::integral_constant< std::size_t, n > {};

template< typename id, std::size_t rank, std::size_t acc >
constexpr constant_index< acc > counter_crumb(id, constant_index< rank >, constant_index< acc >) { return{}; } // found by ADL via constant_index