# Color LED Deck Firmware

LED controller firmware for the Color LED expansion deck designed for the [Crazyflie](https://www.bitcraze.io/documentation/system/platform/) drone platform.

## Highlights

- **Safety-first design** - Hardware thermal protection prevents damage through automatic brightness throttling
- **Simple I2C protocol** - Three commands: get version, set color, check thermal status
- **Building blocks philosophy** - Provides core capabilities; users implement their own LED patterns and control logic

## Getting Started

- **Building** - See [docs/building-and-flashing/build.md](docs/building-and-flashing/build.md)
- **Usage** - See [docs/userguides/usage.md](docs/userguides/usage.md)

## Documentation

Complete documentation is available in the [docs](docs/) folder:
- [Building and Flashing](docs/building-and-flashing/) - Development environment setup
- [User Guides](docs/userguides/) - Usage and integration examples
- [Functional Areas](docs/functional-areas/) - Technical details (I2C protocol, thermal protection, color format)

## Contributing

This project is part of the Bitcraze ecosystem. For contribution guidelines, see the [Bitcraze Contribution Guidelines](https://www.bitcraze.io/development/contribute/general-guidelines/).

## License

This project is licensed under the GNU General Public License version 3 (GPLv3) or later. See the [LICENSE](LICENSE) file for details.

This project uses the STM32 HAL library, which is licensed under BSD 3-Clause.
