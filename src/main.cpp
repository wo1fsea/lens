#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <array>
#include <tuple>
#include <thread>
#include <functional>

#include "rtweekend.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const int DEPTH = 10;

bool init();
void close();

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;

bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow(
			"Ray Tracing",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_ALWAYS_ON_TOP);

		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

color ray_color(const ray &r, const hittable &world, int depth)
{
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth == 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

color ray_color_rr(const ray &r, const hittable &world, double p_lost)
{
	hit_record rec;

	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * (random_double() < p_lost ? color(0, 0, 0) : ray_color_rr(scattered, world, p_lost) / (1. - p_lost));
		}
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

std::tuple<hittable_list, camera> scene_random()
{
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8)
				{
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95)
				{
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else
				{
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	const auto aspect_ratio = SCREEN_WIDTH / SCREEN_HEIGHT;
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	return std::make_tuple(world, cam);
}

std::tuple<hittable_list, camera> scene_3_ball(int idx)
{
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	const auto aspect_ratio = SCREEN_WIDTH / SCREEN_HEIGHT;

	camera cam;

	switch (idx)
	{
	default:
	case 0:
		cam = camera(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 90, aspect_ratio);
		break;

	case 1:
		cam = camera(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20, aspect_ratio);
		break;

	case 2:
		point3 lookfrom(3, 3, 2);
		point3 lookat(0, 0, -1);
		vec3 vup(0, 1, 0);
		auto dist_to_focus = (lookfrom - lookat).length();
		auto aperture = 2.0;

		cam = camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
		break;
	}
	return std::make_tuple(world, cam);
}

static std::array<std::array<uint32_t, SCREEN_HEIGHT>, SCREEN_WIDTH> sample_times{0};
static std::array<std::array<color, SCREEN_HEIGHT>, SCREEN_WIDTH> sample_values{color{0, 0, 0}};

bool signal_to_stop = false;
int block_size = 3;
// int num_threads = block_size * block_size;
int num_threads = std::thread::hardware_concurrency() - 1;
void ray_task(hittable_list world, camera cam, int task_idx)
{
	int iidx = task_idx / block_size;
	int jidx = task_idx % block_size;

	while (!signal_to_stop)
	{
		int i = random_int(0, SCREEN_WIDTH - 1);
		int j = random_int(0, SCREEN_HEIGHT - 1);


		i = i - i % block_size + task_idx;
		i = SCREEN_WIDTH - 1 > i ? i : SCREEN_WIDTH - 1;
		// i = i - i % block_size + iidx;
		// i = SCREEN_WIDTH - 1 > i ? i : SCREEN_WIDTH - 1;
		// j = j - j % block_size + jidx;
		// j = SCREEN_HEIGHT - 1 > j ? j : SCREEN_HEIGHT - 1;

		color &pixel_color = sample_values[i][j];
		uint32_t &pixel_sample = sample_times[i][j];

		auto u = (i + random_double()) / (SCREEN_WIDTH - 1);
		auto v = (j + random_double()) / (SCREEN_HEIGHT - 1);
		ray r = cam.get_ray(u, v);

		pixel_sample += 1;

		// pixel_color += ray_color_rr(r, world, 1. / DEPTH);
		pixel_color += ray_color(r, world, DEPTH);
	}
}

int main(int argc, char *args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
		close();
		return 1;
	}

	SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0x00, 0x00, 0x00));
	SDL_UpdateWindowSurface(gWindow);

	// World
	hittable_list world;
	camera cam;

	int idx = 3;
	if (argc >= 2)
	{
		idx = args[1][0] - '0';
	}

	switch (idx)
	{
	case 0:
	case 1:
	case 2:
		std::tie(world, cam) = scene_3_ball(idx);
		break;
	case 3:
	default:
		std::tie(world, cam) = scene_random();
		break;
	}

	signal_to_stop = false;

	std::cout << num_threads;

	std::vector<std::thread> threads(num_threads);

	for (int idx = 0; idx < num_threads; idx++)
	{
		threads[idx] = std::thread(ray_task, world, cam, idx);
	}

	SDL_Event e;
	while (true)
	{
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				auto c_tuple = get_color(sample_values[i][j], sample_times[i][j]);
				SDL_Rect rect{i, SCREEN_HEIGHT - 1 - j, 1, 1};
				SDL_FillRect(gScreenSurface, &rect, SDL_MapRGB(gScreenSurface->format, std::get<0>(c_tuple), std::get<1>(c_tuple), std::get<2>(c_tuple)));
			}

		SDL_UpdateWindowSurface(gWindow);
		if (SDL_PollEvent(&e) && e.type == SDL_QUIT)
		{
			signal_to_stop = true;
			break;
		}
	}

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	close();
	return 0;
}
