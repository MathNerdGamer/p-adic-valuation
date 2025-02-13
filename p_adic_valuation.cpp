#include <bit>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

#include "NTL/ZZ.h"

auto p_adic_valuation( NTL::ZZ, NTL::ZZ const & )    -> NTL::ZZ;
auto factorial( NTL::ZZ const & )                    -> NTL::ZZ;
auto legendre( NTL::ZZ const &, NTL::ZZ const & )    -> NTL::ZZ;
auto factorial_digit_sum( NTL::ZZ, NTL::ZZ const & ) -> NTL::ZZ;

auto main( int argc, char **argv ) -> int
{
    NTL::ZZ n{ 20 };

    if( argc > 1 )
    {
        std::istringstream input_stream( argv[1] );
        input_stream >> n;

        if( n == 0 )
        {
            std::cout << "All valuations are infinite.\n";
        }
    }

    std::cout << "Using p_adic_valuation:\n";
    std::cout << "The 2-adic valuation of " << factorial( n ) << " is "
              << p_adic_valuation( factorial( n ), NTL::ZZ{ 2 } ) << ".\n";
    std::cout << "The 3-adic valuation of " << factorial( n ) << " is "
              << p_adic_valuation( factorial( n ), NTL::ZZ{ 3 } ) << ".\n";
    std::cout << "The 5-adic valuation of " << factorial( n ) << " is "
              << p_adic_valuation( factorial( n ), NTL::ZZ{ 5 } ) << ".\n";

    std::cout << "\nUsing Legendre's formula:\n";
    std::cout << "The 2-adic valuation of " << factorial( n ) << " is "
              << legendre( n, NTL::ZZ{ 2 } ) << ".\n";
    std::cout << "The 3-adic valuation of " << factorial( n ) << " is "
              << legendre( n, NTL::ZZ{ 3 } ) << ".\n";
    std::cout << "The 5-adic valuation of " << factorial( n ) << " is "
              << legendre( n, NTL::ZZ{ 5 } ) << ".\n";

    return EXIT_SUCCESS;
}

auto p_adic_valuation( NTL::ZZ n, NTL::ZZ const &p ) -> NTL::ZZ
{
    NTL::ZZ valuation{ 0 };

    if( n == 0 )
    {
        throw std::invalid_argument( "Must be called with a non-zero integer.\n" );
    }

    if( p != 2 )
    {
        while( n % p == 0 )
        {
            ++valuation;
            n /= p;
        }
    }
    else
    {   // We have to get the raw bytes of our integer n.
        auto num_bytes{ NTL::NumBytes( n ) };
        std::vector<std::uint8_t> bytes(num_bytes, 0);

        // Returns integer in bytes in order from least significant to most significant byte.
        NTL::BytesFromZZ( bytes.data(), n, num_bytes );

        for( auto b : bytes )
        {
            if( b == 0 )
            {   // Skip the trailing 0 byte and add 8 to the valuation.
                valuation += 8;
            }
            else
            {   // Non-zero byte means we count the final trailing zeroes and end.
                valuation += std::countr_zero( b );
                break;
            }
        }
    }

    return valuation;
}

auto factorial( NTL::ZZ const &n ) -> NTL::ZZ
{   // n!
    if( n <= 1 )
    {
        return NTL::ZZ{ 1 };
    }

    auto fact{ n };

    for( auto i{ n - 1 }; i > 1; --i )
    {
        fact *= i;
    }

    return fact;
}

auto legendre( NTL::ZZ const &n, NTL::ZZ const &p ) -> NTL::ZZ
{   // Returns the p-adic valuation of n! using Legendre's formula
    // v(p; n!) = (n - s(p; n))/(p-1)
    return ( n - factorial_digit_sum( n, p ) ) / ( p - 1 );
}

auto factorial_digit_sum( NTL::ZZ n, NTL::ZZ const &p ) -> NTL::ZZ
{   // Returns the sum of the digits of n in base-p. This is s(p; n).
    auto sum{ NTL::ZZ{ 0 } };

    if( p != 2 )
    {
        while( n != 0 )
        {
            sum += n % p;
            n /= p;
        }
    }
    else
    {   // We have to get the raw bytes of our integer n.
        auto num_bytes{ NTL::NumBytes( n ) };
        std::vector<std::uint8_t> bytes( num_bytes, 0 );

        // Returns integer in bytes in order from least significant to most significant byte.
        NTL::BytesFromZZ( bytes.data(), n, num_bytes );

        for( auto b : bytes )
        {   // Population count = sum of bits = base-2 "digit" sum.
            sum += std::popcount( b );
        }
    }

    return sum;
}
