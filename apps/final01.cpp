#include <iostream>
#include <GRay/rtweekend.hpp>
#include <GRay/color.hpp>
#include <GRay/sphere.hpp>
#include <GRay/hittableList.hpp>
#include <GRay/camera.hpp>
#include <GRay/bvh.h>

using namespace GRay;

// Math::Color rayColor(const Math::Ray& ray, const Math::Hittable& world, int depth)
// {
//     if (depth <= 0)
//         return {0, 0, 0};

//     Math::hitRecord rec;
//     if (world.hit(ray, 0.001, Utils::infinity, rec))
//     {
//         Math::Ray scattered;
//         Math::Color attenuation;
//         if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
//             return attenuation * rayColor(scattered, world, depth - 1);
//         return {0, 0, 0};
//     }

//     Math::Vec3 unitDirection = Math::unitVector(ray.direction());
//     double t = 0.5 * (unitDirection.y() + 1.0);
//     return (1.0 - t) * Math::Color(1.0, 1.0, 1.0) + t * Math::Color(0.5, 0.7, 1.0);
// }

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

Math::HittableList randomScene()
{
    HittableList world;
    auto groundMaterial = make_shared<Materials::Lambertian>(Math::Color(0.5, 0.5, 0.5));
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
    const int samplesPerPixel = 1000;
    const int maxDepth = 50;

    //World
    auto  world = randomScene();

    GRay::Solids::BvhNode bvhTree(world, 0, 0);
    
    //Camera
    Math::Point3 lookFrom(13, 2, 3);
    Math::Point3 lookAt(0, 0, 0);
    double distToFocus = 10;
    Camera cam(lookFrom, lookAt, {0, 1, 0}, 20, aspectRatio, 0.1, distToFocus);
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