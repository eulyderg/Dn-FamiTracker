
#include "Types.h"

chan_id_t operator++(chan_id_t& c, int) {
	chan_id_t old_c = c;
	c = static_cast<chan_id_t>(c + 1);
	return old_c;
}

chan_id_t& operator++(chan_id_t& c) {
	c = static_cast<chan_id_t>(c + 1);
	return c;
}