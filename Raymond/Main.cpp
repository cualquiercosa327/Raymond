#include <iostream>
#include "SDL.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Source/Color.h"
#include "Source/Camera.h"
#include "Source/Sphere.h"
#include "Source/Renderer.h"
#include "Source/Plane.h"
#include "Source/Box.h"
#include "Source/Material.h"
#include "Source/Scene.h"
#include "Source/Light.h"
#include "Source/Sensor.h"
#include "Source/Defines.h"

using namespace std;
using namespace glm;
using namespace Raymond;

class Checkerboard : public SolidTexture
{
public:
	~Checkerboard() override = default;
	vec3 GetColor(const vec3& position) const override;
};

void Putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

SDL_Surface* CreateSurface(Sensor* sensor)
{
	auto surface = SDL_CreateRGBSurfaceWithFormatFrom(
		sensor->Pixels,
		sensor->Width,
		sensor->Height,
		32,
		sensor->Width * 4,
		SDL_PIXELFORMAT_RGBA32);
	ASSERT(surface);
	return surface;
}

void UpdateSurface(SDL_Surface* surface, Sensor* sensor)
{
	ASSERT(surface->w == sensor->Width);
	ASSERT(surface->h == sensor->Height);

	for (int x = 0; x < sensor->Width; ++x)
		for (int y = 0; y < sensor->Height; ++y)
			Putpixel(surface, x, y, sensor->GetPixel(x, y).Int);
}

int kWidth = 300;
int kHeight = 300;

Scene* CreateCornellBox()
{
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);
	auto sphere = make_shared<Sphere>(vec3(-0.5f, 0.2f, 0.25f), 0.25f);
	vec3 zero(0.0f, 0.0f, 0.0f);

	vec3 bottomPos(0.0f, 0.0f, -0.2f);
	auto bottom = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, bottomPos);
	bottom->SetTransform(transform);

	auto plane = make_shared<Plane>();

	vec3 topPos(0.0f, 0.0f, 2.2f);
	auto top = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, topPos);
	top->SetTransform(transform);

	vec3 backPos(0.0f, -1.2f, 1.0f);
	auto back = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.2f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, backPos);
	back->SetTransform(transform);

	vec3 leftPos(1.2f, 0.0f, 1.0f);
	auto left = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, leftPos);
	left->SetTransform(transform);

	vec3 rightPos(-1.2f, 0.0f, 1.0f);
	auto right = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, rightPos);
	right->SetTransform(transform);

	vec3 boxPos(0.25f, -0.3f, 0.6f);
	auto box = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.25f, 0.25f, 0.6f));
	//auto box = make_shared<Sphere>(vec3(0.25f, 0.25f, 1.0f), 0.20f);
	transform = mat4(1.0f);
	//transform = rotate(transform, 0.2f, vec3(1.0f, 0.0f, 0.0f));
	transform = rotate(transform, 0.35f, vec3(0.0f, 0.0f, 1.0f));
	transform = translate(transform, boxPos);
	box->SetTransform(transform);


	vec3 lightPos(0.0f, 0.0f, 1.8f);
	auto mainLight = make_shared<Light>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 0.5f;
	mainLight->Position = lightPos;
	mainLight->Type = LightType::Point;

	auto lightSphere = make_shared<Sphere>(lightPos, 0.05f);
	
	Material* red = new Material();
	red->Color = vec3(1.0f, 0.0f, 0.0f);
	left->SetMaterial(red);

	Material* green = new Material();
	green->Color = vec3(0.0f, 1.0f, 0.0f);
	right->SetMaterial(green);

	Material* blue = new Material();
	blue->Color = vec3(0.0f, 0.0f, 1.0f);
	blue->Specular = 0.2f;
	sphere->SetMaterial(blue);

	Material* lightMaterial = new Material();
	lightMaterial->Color = vec3(1.0f, 1.0f, 1.0f);
	lightMaterial->Emissive = 1.0f;
	lightSphere->SetMaterial(lightMaterial);

	scene->Objects.push_back(sphere);
	scene->Objects.push_back(bottom);
	scene->Objects.push_back(top);
	scene->Objects.push_back(back);
	scene->Objects.push_back(left);
	scene->Objects.push_back(right);
	scene->Objects.push_back(box);
	//scene->Objects.push_back(plane);
	scene->Objects.push_back(lightSphere);
	scene->Lights.push_back(mainLight);


	return scene;
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Raymond", 100, 100,
		kWidth, kHeight, SDL_WINDOW_SHOWN);

	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// The surface contained by the window
	SDL_Surface* screenSurface = nullptr;
	SDL_Event event;
	bool quit = false;	
	float theta = pi<float>() * 0.5f;

	/*
	Sphere sphere(vec3(0.0f, 0.0f, 1.0f), 1.0f);
	Sphere sphere2(vec3(1.5f, 0.0f, 0.5f), 0.5f);
	mat4 transform = mat4(1.0f);
	//transform = scale(transform, vec3(1.0f, 1.0f, 5.0f));
	sphere2.SetTransform(transform);
	Plane plane;
	Box box(vec3(0.0f, 0.0f, 0.0f), vec3(0.6f, 0.6f, 2.5f));
	transform = mat4(1.0f);
	transform = rotate(transform, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
	transform = translate(transform, vec3(1.0f, 0.0f, 0.0f));
	//transform = scale(transform, vec3(2.0f, 2.0f, 2.0f));
	box.SetTransform(transform);

	Light mainLight;
	mainLight.Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight.Intensity = 0.9f;
	mainLight.Position = vec3(5.0f, 10.f, 10.0f);
	mainLight.Type = LightType::Point;

	Light fillLight;
	fillLight.Color = vec3(1.0f, 1.0f, 1.0f);
	fillLight.Intensity = 0.2f;
	fillLight.Position = vec3(-10.0f, 10.f, 15.0f);
	fillLight.Type = LightType::Point;

	Light fillLight2;
	fillLight2.Color = vec3(1.0f, 1.0f, 1.0f);
	fillLight2.Intensity = 0.3f;
	fillLight2.Position = vec3(10.0f, -10.f, 15.0f);
	fillLight2.Type = LightType::Point;

	Material blue;
	blue.Color = vec3(0.0f, 0.0f, 1.0f);
	box.SetMaterial(&blue);

	Material red;
	red.Color = vec3(1.0f, 0.0f, 0.0f);
	red.Specular = 0.7f;
	red.SpecularPower = 32.0f;
	sphere.SetMaterial(&red);

	Material green;
	green.Color = vec3(0.0f, 1.0f, 0.0f);
	sphere2.SetMaterial(&green);

	Material checker;
	checker.Texture = new Checkerboard();
	plane.SetMaterial(&checker);

	Renderer renderer;
	renderer.Scene.push_back(&plane);
	renderer.Scene.push_back(&sphere);
	renderer.Scene.push_back(&sphere2);
	renderer.Scene.push_back(&box);
	renderer.Lights.push_back(&mainLight);
	renderer.Lights.push_back(&fillLight);
	renderer.Lights.push_back(&fillLight2);
	*/

	Renderer renderer;

	renderer.Scene = CreateCornellBox();
	renderer.Sensor = make_shared<Sensor>(kWidth, kHeight);
	float r = 2.8f;
	renderer.Scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));
	renderer.Render();

	screenSurface = SDL_GetWindowSurface(win);
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}		
		
		SDL_LockSurface(screenSurface);	
		UpdateSurface(screenSurface, renderer.Sensor.get());
		SDL_UnlockSurface(screenSurface);
		SDL_UpdateWindowSurface(win);
	}
			
	return 0;
}

vec3 Checkerboard::GetColor(const vec3 & position) const
{
	vec3 white(1, 1, 1);
	vec3 black(0, 0, 0);
	return (int(position.x) % 2 ^ int(position.y) % 2) ? white : black;
}
