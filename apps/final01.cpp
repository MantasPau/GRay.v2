#include <iostream>
#include <GRay/rtweekend.hpp>
#include <GRay/color.hpp>
#include <GRay/sphere.hpp>
#include <GRay/hittableList.hpp>
#include <GRay/camera.hpp>
#include <GRay/bvh.h>

using namespace GRay;

Math::Color rayColor(const Math::Ray& ray, const Solids::BvhNode& world, int depth)
{
    if (depth <= 0)
        return {0, 0, 0};

    Math::hitRecord rec;
    if (world.hit(ray, 0.001, Utils::infinity, rec))
    {
        Math::Ray scattered;
        Math::Color attenuation;
        if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
            return attenuation * rayColor(scattered, world, depth - 1);
        return {0, 0, 0};
    }

    Math::Vec3 unitDirection = Math::unitVector(ray.direction());
    double t = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - t) * Math::Color(1.0, 1.0, 1.0) + t * Math::Color(0.5, 0.7, 1.0);
}

Math::HittableList twoSpheres()
{
    Math::HittableList objects;
    auto checker = make_shared<Materials::CheckerTexture>(Math::Color(0.2, 0.3, 0.1), Math::Color(0.9, 0.9, 0.9));

    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, -10, 0), 10, make_shared<Materials::Lambertian>(checker)));
    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 10, 0), 10, make_shared<Materials::Lambertian>(checker)));

    return objects;
}

Math::HittableList twoPerlinSpheres()
{
    Math::HittableList objects;
    auto pertext = make_shared<Materials::NoiseTexture>(4);

    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, -1000, 0), 1000, make_shared<Materials::Lambertian>(pertext)));
    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 2, 0), 2, make_shared<Materials::Lambertian>(pertext)));

    return objects;
}

Math::HittableList randomScene()
{
    HittableList world;
    auto checker = make_shared<Materials::CheckerTexture>(Math::Color(0.2, 0.3, 0.1), Math::Color(0.9, 0.9, 0.9));
    auto groundMaterial = make_shared<Materials::Lambertian>(checker);
    world.add(make_shared<Solids::Sphere>(Math::Point3(0, -1000, 0), 1000, groundMaterial));
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            auto chooseMat = Utils::randomDouble();
            Math::Point3 center(a + 0.9 * Utils::randomDouble(), 0.2, b + 0.9 * Utils::randomDouble());
            if ((center - Math::Point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<Material> sphereMaterial;
                if(chooseMat < 0.8)
                {
                    //diffuse
                    auto albedo = Math::random() * Math::random();
                    sphereMaterial = make_shared<Materials::Lambertian>(albedo);
                    world.add(make_shared<Solids::Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.95)
                {
                    //metal
                    auto albedo = Math::random(0.5, 1);
                    auto fuzz = Utils::randomDouble(0, 0.5);
                    sphereMaterial = make_shared<Materials::Metal>(albedo, fuzz);
                    world.add(make_shared<Solids::Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    //glass
                    sphereMaterial = make_shared<Materials::Dialectric>(1.5);
                    world.add(make_shared<Solids::Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = make_shared<Materials::Dialectric>(1.5);
    world.add(make_shared<Solids::Sphere>(Math::Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Materials::Lambertian>(Math::Color(0.4, 0.2, 0.1));
    world.add(make_shared<Solids::Sphere>(Math::Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Materials::Metal>(Math::Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Solids::Sphere>(Math::Point3(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, char * argv[])
{
    //Image
    const double aspectRatio = 3.0 / 2.0;
    const int imageWidth = 512;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int samplesPerPixel = 100;
    const int maxDepth = 50;

    //World
    Math::HittableList world;
    //Camera
    Math::Point3 lookFrom(13, 2, 3);
    Math::Point3 lookAt(0, 0, 0);
    double distToFocus = 10;
    double vfov = 40.0;
    double aperture = 0.0;

    switch(3)
    {
        case 1:
            world = randomScene();
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        case 2:
            world = twoSpheres();
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 3:
            world = twoPerlinSpheres();
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            break;
    }

    GRay::Solids::BvhNode bvhTree(world, 0, 0);
    Camera cam(lookFrom, lookAt, {0, 1, 0}, vfov, aspectRatio, aperture, distToFocus);
    //Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            Math::Color pixelColor(0, 0, 0);
            for (int s = 0; s < samplesPerPixel; ++s)
            {
                double u = (i + Utils::randomDouble()) / (imageWidth - 1);
                double v = (j + Utils::randomDouble()) / (imageHeight - 1);
                Math::Ray ray = cam.getRay(u, v);
                pixelColor += rayColor(ray, bvhTree/*world*/, maxDepth);
            }
            Colors::writeColor(std::cout, pixelColor, samplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";

    return 0;
}