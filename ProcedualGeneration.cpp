// ProcedualGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "sdl.h"
#include <chrono>
#include "PerlinNoise2D.h"

unsigned int hash(unsigned int x, unsigned int y, unsigned int seed) {
	unsigned int h = seed + x * 374761393 + y * 668265263;
	h = (h ^ 61) ^ (h >> 16);
	h = h + (h << 3);
	h = h ^ (h >> 4);
	h = h * 0x27d4eb2d;
	h = h ^ (h >> 15);
	return h;
}

// Function to generate deterministic noise between -1 and 1
double noise(double x, double y, unsigned int seed) {
	unsigned int intX = static_cast<unsigned int>(std::floor(x));
	unsigned int intY = static_cast<unsigned int>(std::floor(y));
	unsigned int h = hash(intX, intY, seed);

	auto result{ (2.0 * static_cast<double>(h) / UINT_MAX) - 1.0 };
	
	return (result + 1) / 2;
}

int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

void Render(SDL_Renderer* renderer)
{
	const auto& color = SDL_Color(0u, 0u, 0u);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);

	
}

bool ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return false;
		}
		if (e.type == SDL_KEYDOWN) {

		}
		if (e.type == SDL_MOUSEBUTTONUP) {

		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	constexpr int windowWidth{ 512 };
	constexpr int windowHeight{ windowWidth };

	auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	auto renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED);

	const float desiredFPS{ 144.f };
	std::chrono::duration<long long, std::milli> msPerFrame{ static_cast<long long>(1000.f / desiredFPS) };

	bool running{ true };
	
	auto perlin{ PerlinNoise2D(1) };

	const float devider{ 128 };

	int origin{ 0 };

	while (running)
	{
		const auto start = std::chrono::high_resolution_clock::now();

		running = ProcessInput();
		Render(renderer);

		for (int rowNr{}; rowNr < windowWidth; ++rowNr)
		{
			for (int colNr{}; colNr < windowHeight; ++colNr)
			{
				//double noise{ std::abs(perlin.GetNoise(rowNr / devider, colNr / devider)) };
				//std::cout << (rowNr + origin) / devider << ' ' << (colNr + origin) / devider << '\n';
				double noise{ (perlin.GetNoise((rowNr + origin) / devider, (colNr + origin) / devider, 1, 0.55f) + 1) / 2.f };

				if (noise < 0.45f)
				{
					double test = (noise) * 255;
					Uint8 col{ static_cast<Uint8>(test) };
					const auto& color = SDL_Color(0, 255u, 0);
					
					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
					SDL_RenderDrawPoint(renderer, rowNr, colNr);
				}
				else
				{
					double test = (noise) * 255;
					Uint8 col{ static_cast<Uint8>(test) };
					const auto& color = SDL_Color(0, 0, 255u);
				
					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
					SDL_RenderDrawPoint(renderer, rowNr, colNr);
				}

				//double test = (noise) * 255;
				//Uint8 col{ static_cast<Uint8>(test) };
				//const auto& color = SDL_Color(col, col, col);

				//SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
				//SDL_RenderDrawPoint(renderer, rowNr, colNr);
			}
		}

		SDL_RenderPresent(renderer);

		const auto sleepTime{ start + std::chrono::milliseconds(msPerFrame) - std::chrono::high_resolution_clock::now() };
		std::this_thread::sleep_for(sleepTime);
	}

    return 0;
}