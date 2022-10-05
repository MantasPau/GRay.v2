#include <iostream>
#include <GRay/rtweekend.hpp>
#include <GRay/color.hpp>
#include <GRay/sphere.hpp>
#include <GRay/hittableList.hpp>
#include <GRay/camera.hpp>
#include <GRay/bvh.h>
#include <GRay/background.hpp>
#include <GRay/aarect.hpp>
#include <GRay/box.hpp>

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

Math::HittableList twoSpheresEarth()
{
    Math::HittableList objects;
    auto checker = make_shared<Materials::CheckerTexture>(Math::Color(0.2, 0.3, 0.1), Math::Color(0.9, 0.9, 0.9));
    auto earthTexture = make_shared<Materials::ImageTexture>("data/earthmap.jpg");

    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, -1000, 0), 1000, make_shared<Materials::Lambertian>(checker)));
    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 1, 0), 1, make_shared<Materials::Lambertian>(earthTexture)));

    return objects;
}

Math::HittableList simpleLight()
{
    Math::HittableList objects;
    auto pertext = make_shared<Materials::NoiseTexture>(4);

    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, -1000, 0), 1000, make_shared<Materials::Lambertian>(pertext)));
    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 2, 0), 2, make_shared<Materials::Lambertian>(pertext)));

    auto diffLight = make_shared<Materials::DiffuseLight>(Math::Color(4, 4, 4), 2);
    objects.add(make_shared<Solids::XYRect>(3, 5, 1, 3, -2, diffLight));
    objects.add(make_shared<Solids::Sphere>(Math::Point3(0, 6, 0), 1, diffLight));

    return objects;
}

Math::HittableList cornelBox()
{
    Math::HittableList objects;
    auto red = make_shared<Materials::Lambertian>(Math::Color(0.6, 0.05, 0.05));
    auto white = make_shared<Materials::Lambertian>(Math::Color(0.73, 0.73, 0.73));
    auto green = make_shared<Materials::Lambertian>(Math::Color(0.12, 0.45, 0.15));
    auto light = make_shared<Materials::DiffuseLight>(Math::Color(15, 15, 15));

    objects.add(make_shared<Solids::YZRect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<Solids::YZRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<Solids::XZRect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<Solids::XZRect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<Solids::XZRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<Solids::XYRect>(0, 555, 0, 555, 555, white));

    shared_ptr<Math::Hittable> box1 = make_shared<Solids::Box>(Math::Point3(0, 0, 0), Math::Point3(165, 330, 165), white);
    shared_ptr<Math::Hittable> box2 = make_shared<Solids::Box>(Math::Point3(0, 0, 0), Math::Point3(165, 165, 165), white);
    box1 = make_shared<Math::RotateY>(box1, 15);
    box1 = make_shared<Math::Translate>(box1, Math::Vec3(265, 0, 295));
    objects.add(box1);
    box2 = make_shared<Math::RotateY>(box2, -18);
    box2 = make_shared<Math::Translate>(box2, Math::Vec3(130, 0, 65));
    objects.add(box2);

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
    double aspectRatio = 3.0 / 2.0;
    int imageWidth = 512;
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    int samplesPerPixel = 10;
    int maxDepth = 50;

    //World
    Math::HittableList world;
    //Camera
    Math::Point3 lookFrom(13, 2, 3);
    Math::Point3 lookAt(0, 0, 0);
    double distToFocus = 10;
    double vfov = 40.0;
    double aperture = 0.0;
    Solids::Background background(Math::Color(0.7, 0.8, 1.0));
    //std::static_pointer_cast<Materials::ImageTextureHDRI>(std::static_pointer_cast<Materials::DiffuseLight>(background.mat_ptr)->emit)->subSampling = true;

    switch(6)
    {
        case 1:
            world = randomScene();
            background = Solids::Background(make_shared<Materials::ImageTextureHDRI>("data/clarens_midday_4k.hdr"), 0.5);
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        case 2:
            world = twoSpheres();
            background = Solids::Background(Math::Color(0.7, 0.8, 1.0));
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 3:
            world = twoPerlinSpheres();
            background = Solids::Background(Math::Color(0.7, 0.8, 1.0));
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 4:
            world = twoSpheresEarth();
            background = Solids::Background(Math::Color(0.7, 0.8, 1.0));
            lookFrom = Math::Point3(13, 2, 3);
            lookAt = Math::Point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 5:
            world = simpleLight();
            background = Solids::Background(Math::Color(0.0, 0.0, 0.0));
            samplesPerPixel = 10;
            lookFrom = Math::Point3(26, 3, 6);
            lookAt = Math::Point3(0, 2, 0);
            vfov = 20.0;
            break;
        case 6:
            world = cornelBox();
            background = Solids::Background(Math::Color(0.0, 0.0, 0.0));
            aspectRatio = 1;
            imageWidth = 600;
            imageHeight = imageWidth;
            samplesPerPixel = 100;
            lookFrom = Math::Point3(278, 278, -800);
            lookAt = Math::Point3(278, 278, 0);
            vfov = 40.0;
            break;
        default:
        case 7:
            background = Solids::Background(Math::Color(0.0, 0.0, 0.0));
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
                pixelColor += rayColor(ray, background, bvhTree/*world*/, maxDepth);
            }
            Colors::writeColor(std::cout, pixelColor, samplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";

    return 0;
}