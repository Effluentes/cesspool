#pragma once
#include <platform/bsp/startup_result/InitializeError.hh>
#include <cstdint>
#include <vector>

namespace platform::driver
{
    using I2CPortNumber = std::uint8_t;
    using DeviceAddress = std::size_t;
    using SCLSpeed = std::size_t;
    using RegisterAddress = std::uint8_t;
    using Buffer = std::vector<std::uint8_t>;

    namespace interface
    {
        class GPIO;

        struct I2CConfiguration
        {
            const I2CPortNumber portNumber;
            const GPIO &sda;
            const GPIO &scl;
            const DeviceAddress deviceAddress;
            const SCLSpeed sclSpeed;
        };

        template <typename Derived>
        class I2C
        {
        public:
            platform::bsp::InitializeError initialize()
            {
                return static_cast<Derived *>(this)->initializeImpl();
            }

            void write(const Buffer &data)
            {
                static_cast<Derived *>(this)->writeImpl(data);
            }

            Buffer read()
            {
                return static_cast<Derived *>(this)->readImpl();
            }

            void writeRegister(const RegisterAddress registerAddress, const Buffer &data)
            {
                static_cast<Derived *>(this)->writeRegisterImpl(registerAddress, data);
            }

            Buffer readRegister(const RegisterAddress registerAddress)
            {
                return static_cast<Derived *>(this)->readRegisterImpl(registerAddress);
            }

            bool isDeviceConnected()
            {
                return static_cast<Derived *>(this)->isDeviceConnectedImpl();
            }

        protected:
            // Konstruktor chroniony zapobiega przypadkowemu tworzeniu samej bazy
            I2C() = default;
            ~I2C() = default;
        };
    }
}
