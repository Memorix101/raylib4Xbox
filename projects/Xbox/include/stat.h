// stat.h — miniaudio on nxdk does #include <stat.h> (not <sys/stat.h>)
// Redirect to our sys/stat.h stub.
#include <sys/stat.h>
