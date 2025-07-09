#include "Color.h"

ostream &operator<<(ostream &out, const Color &color) {
    out << "Color(" << color.r << ", " << color.g << ", " << color.b << ")";
    return out;
}
