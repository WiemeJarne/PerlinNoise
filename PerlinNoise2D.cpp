#include "PerlinNoise2D.h"
#include <numeric>
#include <random>

PerlinNoise2D::PerlinNoise2D(unsigned int seed, int permutationTableSize)
    : m_PermutationTableSize{ permutationTableSize }
{
    std::mt19937 rng( seed );

    m_PermutationTable.resize(m_PermutationTableSize);

    //fill the vector with numbers form 0 - 255 inclusive
    std::iota(m_PermutationTable.begin(), m_PermutationTable.end(), 0);

    //shuffle the vector using the random number generator
    std::shuffle(m_PermutationTable.begin(), m_PermutationTable.end(), rng);
}

double PerlinNoise2D::GetNoise(double x, double y) const
{
    //calculate the unit square where the given point is in
    //the left bounds are [x, y; x + 1, y + 1] with x and y being and int and clamped to 255 inclusive
    
    int xu{ std::clamp(static_cast<int>(x), 0, m_PermutationTableSize - 1) }; //make sure xu and yu do not get larger then m_PermutationTableSize - 1
    int yu{ std::clamp(static_cast<int>(y), 0, m_PermutationTableSize - 1) };

    //get the fractional parts of x and y these represent the coordinate of the given point inside the unit square
    double fractX{ x - static_cast<int>(x) };
    double fractY{ y - static_cast<int>(y) };

    double u{ Fade(fractX) };
    double v{ Fade(fractY) };

    const int hash1{ GetHashValue(xu, yu) };
    const int hash2{ GetHashValue(xu, Increment(yu, m_PermutationTableSize - 1)) };
    const int hash3{ GetHashValue(Increment(xu, m_PermutationTableSize - 1), yu) };
    const int hash4{ GetHashValue(Increment(xu, m_PermutationTableSize - 1), Increment(yu, m_PermutationTableSize - 1)) };

    double x1{ std::lerp(Gradient(hash1, fractX, fractY), Gradient(hash3, fractX - 1, fractY), u) };
    double x2{ std::lerp(Gradient(hash2, fractX, fractY - 1), Gradient(hash4, fractX - 1, fractY - 1), u) };
    
    return std::lerp(x1, x2, v);
}

double PerlinNoise2D::GetNoise(double x, double y, int amountOfOctaves, double persistence)
{
    double total{};
    double frequency{ 1 };
    double amplitude{ 1 };
    double maxValue{};

    for (int index{}; index < amountOfOctaves; ++index)
    {
        total += GetNoise(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;

        frequency *= 2;
    }

    return total / maxValue;
}

double PerlinNoise2D::Fade(double value) const
{
    return value * value * value * (value * (value * 6 - 15) + 10); //= 6*value^5 - 15*value^4 + 10*value^3 defined by Ken Perlin
}

int PerlinNoise2D::GetHashValue(int x, int y) const
{
    return m_PermutationTable[std::clamp(m_PermutationTable[x] + y, 0, m_PermutationTableSize - 1)];
}

int PerlinNoise2D::Increment(int value, int maxValue) const
{
    auto result{ ++value };

    if (result > maxValue)
        return std::abs(result - value);
    
    return result;
}

double PerlinNoise2D::Gradient(int hash, double x, double y) const
{
    int h = hash & 3; // Convert low two bits to integer
    double u = h < 2 ? x : y; // If h is even then u=x else u=y
    double v = h < 2 ? y : x; // If h is even then v=y else v=x
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Add or subtract u and v based on h
}