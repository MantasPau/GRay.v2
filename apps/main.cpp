#include <iostream>
#include <GRay.v2/vec3.hpp>
#include <GRay.v2/color.hpp>
#include <GRay.v2/ray.hpp>

GRay::Math::Color rayColor(const GRay::Math::Ray& ray)
{
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

    //Camera
    double viewportHeight = 2.0;
    double viewportWidth = aspectRatio * viewportHeight;
    double focalLength = 1.0;

    GRay::Math::Point3 origin = GRay::Math::Point3(0, 0, 0);
    GRay::Math::Vec3 horizontal = GRay::Math::Vec3(viewportWidth, 0, 0);
    GRay::Math::Vec3 vertical = GRay::Math::Vec3(0, viewportHeight, 0);
    GRay::Math::Vec3 lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - GRay::Math::Vec3(0, 0, focalLength);

    //Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            double u = double(i) / (imageWidth - 1);
            double v = double(j) / (imageHeight - 1);
            GRay::Math::Ray ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
            GRay::Math::Color pixelColor = rayColor(ray);
            GRay::Color::writeColor(std::cout, pixelColor);
        }
    }

    std::cerr << "\nDone.\n";

    return 0;
}