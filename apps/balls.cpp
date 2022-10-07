#include <iostream>
#include <chrono>
#include <GRay/rtweekend.hpp>
#include <GRay/color.hpp>
#include <GRay/sphere.hpp>
#include <GRay/movingSphere.hpp>
#include <GRay/hittableList.hpp>
#include <GRay/camera.hpp>
#include <GRay/bvh.h>
#include <GRay/background.hpp>
#include <GRay/aarect.hpp>
#include <GRay/box.hpp>
#include <GRay/constantMedium.hpp>


using namespace GRay;

Math::Color rayColor(const Math::Ray& ray, const Solids::Background& background, const Solids::BvhNode& world, int depth)
{
    if (depth <= 0)
        return {0, 0, 0};

    Math::hitRecord rec;
    if (!world.hit(ray, 0.001, Utils::infinity, rec))
        return background.getValue(ray);
    
    Math::Ray scattered;
    Math::Color attenuation;
    Math::Color emited = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
        return emited;

    return emited + attenuation * rayColor(scattered, background, world, depth - 1);
}

Math::HittableList makeScene()
{
    Math::HittableList boxes1;
    auto ground = make_shared<Materials::Lambertian>(Math::Color(0.4, 0.4, 0.4));

    const int boxesPerSide = 20;
    for (int i = 0; i < boxesPerSide; ++i)
        for (int j = 0; j < boxesPerSide; ++j)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto z1 = z0 + w;
            auto y1 = Math::randomDouble(1,101);
            boxes1.add(make_shared<Solids::Box>(Math::Point3(x0, y0, z0), Math::Point3(x1, y1, z1), ground));
        }

    Math::HittableList objects;
    objects.add(make_shared<Solids::BvhNode>(boxes1, 0, 1));
    objects.add(make_shared<Solids::XYRect>(-1000, 1000, 0, 700, 1000, ground));
    objects.add(make_shared<Solids::YZRect>(0, 554, 0, 1000, -1000, ground));
    objects.add(make_shared<Solids::YZRect>(0, 554, 0, 1000, 1000, ground));
    objects.add(make_shared<Solids::XZRect>(-1000, 0, 0, 1000, 500, ground));
    objects.add(make_shared<Solids::XZRect>(200, 1000, 0, 1000, 500, ground));
    objects.add(make_shared<Solids::YZRect>(500, 700, 0, 1000, 0, ground));
    objects.add(make_shared<Solids::YZRect>(500, 700, 0, 1000, 200, ground));

    auto light = make_shared<Materials::DiffuseLight>(Math::Color(7, 7, 9));
    objects.add(make_shared<Solids::XZRect>(0, 200, 0, 1000, 700, light));

    // auto center1 = Math::Point3(400, 400, 200);
    // auto center2 = center1 + Math::Vec3(30, 0, 0);
    // auto movingSphereMaterial = make_shared<Materials::Lambertian>(Math::Color(0.7, 0.3, 0.1));
    // objects.add(make_shared<Solids::MovingSphere>(center1, center2, 0, 1, 50, movingSphereMaterial));

    // objects.add(make_shared<Solids::Sphere>(Math::Point3(260, 150, 45), 50, make_shared<Materials::Dialectric>(1.5)));
    // objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 150, 145), 50, make_shared<Materials::Metal>(Math::Color(0.8, 0.8, 0.9), 0.4)));

    // auto boundary = make_shared<Solids::Sphere>(Math::Point3(360, 150, 145), 70, make_shared<Materials::Dialectric>(1.5));
    // objects.add(boundary);
    // objects.add(make_shared<Solids::ConstantMedium>(boundary, 0.2, Math::Color(0.2, 0.4, 0.9)));
    // boundary = make_shared<Solids::Sphere>(Math::Point3(0, 0, 0), 5000, make_shared<Materials::Dialectric>(1.5));
    // objects.add(make_shared<Solids::ConstantMedium>(boundary, 0.0001, Math::Color(1, 1, 1)));

    // auto emat = make_shared<Materials::Lambertian>(make_shared<Materials::ImageTexture>("data/earthmap.jpg"));
    // objects.add(make_shared<Solids::Sphere>(Math::Point3(400, 200, 400), 100, emat));
    // auto pertext = make_shared<Materials::NoiseTexture>(0.05);
    // objects.add(make_shared<Solids::Sphere>(Math::Point3(220, 280, 300), 80, make_shared<Materials::Lambertian>(pertext)));

    // Math::HittableList boxes2;
    // auto white = make_shared<Materials::Lambertian>(Math::Color(0.73, 0.73, 0.73));
    // int ns = 1000;
    // for (int j = 0; j < ns; ++j)
    //     boxes2.add(make_shared<Solids::Sphere>(Math::Point3(Math::randomDouble(0, 165), 
    //                                                         Math::randomDouble(0, 165), 
    //                                                         Math::randomDouble(0, 165)), 
    //                                                         10, white));

    // objects.add(make_shared<Math::Translate>(make_shared<Math::RotateY>(make_shared<Solids::BvhNode>(boxes2, 0.0, 1.0), 15), Math::Vec3(-100, 270, 395)));
    return objects;
}

int main(int argc, char * argv[])
{
    //Image
    double aspectRatio = 3.0 / 2.0;
    int imageWidth = 256;
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    int samplesPerPixel = 100;
    int maxDepth = 10;

    //Camera
    Math::Point3 lookFrom = Math::Point3(478, 278, -600);
    Math::Point3 lookAt = Math::Point3(278, 278, 0);
    double distToFocus = 10;
    double vfov = 40.0;
    double aperture = 0.0;
    
    //World
    Math::HittableList world;
    world = makeScene();
    Solids::Background background(Math::Color(0.0, 0.0, 0.0));

    GRay::Solids::BvhNode bvhTree(world, 0, 1);
    Camera cam(lookFrom, lookAt, {0, 1, 0}, vfov, aspectRatio, aperture, distToFocus);
    //Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << "   " << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            Math::Color pixelColor(0, 0, 0);
            for (int s = 0; s < samplesPerPixel; ++s)
            {
                double u = (i + Utils::randomDouble()) / (imageWidth - 1);
                double v = (j + Utils::randomDouble()) / (imageHeight - 1);
                Math::Ray ray = cam.getRay(u, v);
                ray.tm = Math::randomDouble();
                pixelColor += rayColor(ray, background, bvhTree/*world*/, maxDepth);
            }
            Colors::writeColor(std::cout, pixelColor, samplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";

    return 0;
}