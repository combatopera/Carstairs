#include "port.h"

#include <ladspa.h>

Port::Port(const char *name, LADSPA_PortDescriptor descriptor)
        : _name(name), _descriptor(descriptor) {
}
