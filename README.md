data-injector is data-over-dab-example, but with less dependencies.

libtins is from `github.com/mfontanini/libtins`

Build instructions: see `build.sh`

1. only generator, not receiver
1. uses `boost::asio` instead of `asio` directly
1. links against statical libtins
1. reads an ini file for configuration
