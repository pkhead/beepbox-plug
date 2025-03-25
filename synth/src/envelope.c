#include "envelope.h"

double secs_fade_in(double setting) {
    return 0.0125 * (0.95 * setting + 0.05 * setting * setting);
}