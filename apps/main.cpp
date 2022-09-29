#include <iostream>
#include <GRay/rtweekend.hpp>
#include <GRay/color.hpp>
#include <GRay/sphere.hpp>
#include <GRay/hittableList.hpp>
#include <GRay/camera.hpp>

GRay::Math::Color rayColor(const GRay::Math::Ray& ray, const GRay::Math::Hittable& world, int depth)
{
    if (depth <= 0)
        return {0, 0, 0};

    GRay::Math::hitRecord rec;
    if (world.hit(ray, 0.001, GRay::Utils::infinity, rec))
    {
        GRay::Math::Ray scattered;
        GRay::Math::Color attenuation;
        if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
            return attenuation * rayColor(scattered, world, depth - 1);
        return {0, 0, 0};
    }

    GRay::Math::Vec3 unitDirection = GRay::Math::unitVector(ray.direction());
    double t = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - t) * GRay::Math::Color(1.0, 1.0, 1.0) + t * GRay::Math::Color(0.5, 0.7, 1.0);
}

int main(int argc, char * argv[])
{
    //Image
    const double aspectRatio = 16.0 / 9.0;
    const int imageWidth = 512;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int samplesPerPixel = 100;
    const int maxDepth = 50;

    //World
    GRay::Math::HittableList world;
    auto materialGround = make_shared<GRay::Materials::Lambertian>(GRay::Math::Color(0.8, 0.8, 0.0));
    auto materialCenter = make_shared<GRay::Materials::Dialectric>(1.5);
    auto materialLeft = make_shared<GRay::Materials::Dialectric>(1.5);
    auto materialRight = make_shared<GRay::Materials::Metal>(GRay::Math::Color(0.8, 0.6, 0.2), 1.0);
    world.add(make_shared<GRay::Solids::Sphere>(GRay::Math::Point3(0, -100.5, -1), 100, materialGround));
    world.add(make_shared<GRay::Solids::Sphere>(GRay::Math::Point3(0, 0, -1), 0.5, materialCenter));
    world.add(make_shared<GRay::Solids::Sphere>(GRay::Math::Point3(-1, 0, -1), 0.5, materialLeft));
    world.add(make_shared<GRay::Solids::Sphere>(GRay::Math::Point3(1, 0, -1), 0.5, materialRight));

    //Camera
    GRay::Camera cam;
    //Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            GRay::Math::Color pixelColor(0, 0, 0);
            for (int s = 0; s < samplesPerPixel; ++s)
            {
                double u = (i + GRay::Utils::randomDouble()) / (imageWidth - 1);
                double v = (j + GRay::Utils::randomDouble()) / (imageHeight - 1);
                GRay::Math::Ray ray = cam.getRay(u, v);
                pixelColor += rayColor(ray, world, maxDepth);
            }
            GRay::Color::writeColor(std::cout, pixelColor, samplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";

    return 0;
}