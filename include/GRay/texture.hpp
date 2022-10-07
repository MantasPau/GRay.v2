#pragma once

#include <GRay/rtweekend.hpp>
#include <GRay/perlin.hpp>
#include <iostream>
#include <stb/rtw_stb_image.hpp>

namespace GRay
{
    namespace Materials
    {
        class Texture
        {
        public:
            virtual GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const = 0;
        };

        class SolidColor : public Texture
        {
        public:
            SolidColor() {}
            SolidColor(GRay::Math::Color c) : colorValue{ c } {}
            SolidColor(double red, double green, double blue) : SolidColor(GRay::Math::Color(red, green, blue)) {}
            GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override { return colorValue; }
        public:
            GRay::Math::Color colorValue;
        };

        class CheckerTexture : public Texture
        {
        public:
            CheckerTexture() {}
            CheckerTexture(shared_ptr<Texture> _even, shared_ptr<Texture> _odd) : odd{ _odd }, even{ _even } {}
            CheckerTexture(GRay::Math::Color c1, GRay::Math::Color c2) : even{ make_shared<SolidColor>(c1) }, odd{ make_shared<SolidColor>(c2) } {}
            GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override
            {
                auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
                if (sines < 0)
                    return odd->value(u, v, p);
                else
                    return even->value(u, v, p);
            }

        public:
            shared_ptr<Texture> odd;
            shared_ptr<Texture> even;
        };

        class NoiseTexture : public Texture
        {
        public:
            NoiseTexture() {}
            NoiseTexture(double sc) : scale{ sc } {}
            GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override
            {
                //return GRay::Math::Color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p)); //simple
                return GRay::Math::Color(1, 1, 1) * noise.turb(scale * p); //turbulence
                //return GRay::Math::Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p))); //marble
            }
        public:
            Perlin noise;
            double scale;
        };

        class ImageTexture : public Texture
        {
        public:
            const static int bytesPerPixel = 3;

            ImageTexture() : data{ nullptr }, width{ 0 }, height{ 0 }, bytesPerScanline{ 0 } {}
            ImageTexture(const char* fileName)
            {
                auto componentsPerPixel = bytesPerPixel;
                data = stbi_load(fileName, &width, &height, &componentsPerPixel, componentsPerPixel);
                if (!data)
                {
                    std::cerr << "ERROR: Could not load texture image file '" << fileName << "'.\n";
                    width = height = 0;
                }
                bytesPerScanline = bytesPerPixel * width;
            }

            ~ImageTexture()
            {
                delete data;
            }

            GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override
            {
                if (data == nullptr)
                    return Math::Color(0, 1, 1);

                u = Utils::clamp(u, 0.0, 1.0);
                v = 1.0 - Utils::clamp(v, 0.0, 1.0);

                int i = static_cast<int>(u * width);
                int j = static_cast<int>(v * height);

                if (i >= width) i = width - 1;
                if (j >= height) j = height - 1;

                const auto colorScale = 1.0 / 255;
                auto pixel = data + j * bytesPerScanline + i * bytesPerPixel;

                return Math::Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
            }

        private:
            unsigned char* data;
            int width, height;
            int bytesPerScanline;
        };

        class ImageTextureHDRI : public Texture
        {
        public:
            const static int partsPerPixel = 3;
            bool subSampling;
            double subSamplingRadius;
            double subSamplingRaysCnt;
            bool useClamping;
            double remapingFactor;

            ImageTextureHDRI() : data{ nullptr }, width{ 0 }, height{ 0 }, partsPerScanline{ 0 },
                subSampling{ false }, subSamplingRadius{ 0.01 }, subSamplingRaysCnt{ 100 },
                useClamping{ false }, remapingFactor{ 1 / 2.2 } {}
            ImageTextureHDRI(const char* fileName)
            {
                auto componentsPerPixel = partsPerPixel;
                data = stbi_loadf(fileName, &width, &height, &componentsPerPixel, componentsPerPixel);
                if (!data)
                {
                    std::cerr << "ERROR: Could not load texture image file '" << fileName << "'.\n";
                    width = height = 0;
                }
                partsPerScanline = partsPerPixel * width;
                subSampling = false;
                subSamplingRadius = 0.01;
                subSamplingRaysCnt = 100;
                useClamping = false;
                remapingFactor = 1 / 2.2;
            }

            ~ImageTextureHDRI()
            {
                delete data;
            }

            GRay::Math::Color value(double u, double v, const GRay::Math::Point3& p) const override
            {
                if (data == nullptr)
                    return Math::Color(0, 1, 1);

                u = Utils::clamp(u, 0.0, 1.0);
                v = 1.0 - Utils::clamp(v, 0.0, 1.0);

                Math::Color res;
                if (subSampling)
                {
                    for (int k = 0; k < subSamplingRaysCnt; k++)
                    {
                        Math::Vec3 vv = Math::randomInUnitDisc() * subSamplingRadius;
                        vv[0] += u;
                        vv[1] += v;
                        if (vv[0] < 0) vv[0] += 1;
                        if (vv[1] < 0) vv[1] += 1;
                        if (vv[0] > 1) vv[0] -= 1;
                        if (vv[1] > 1) vv[1] -= 1;

                        int i = static_cast<int>(vv[0] * width);
                        int j = static_cast<int>(vv[1] * height);

                        if (i >= width) i = width - 1;
                        if (j >= height) j = height - 1;

                        //std::cerr << i << " " << j << "\n";
                        auto pixel = data + j * partsPerScanline + i * partsPerPixel;
                        res += Math::Color(pixel[0], pixel[1], pixel[2]);
                    }
                    res /= subSamplingRaysCnt;
                }
                else
                {
                    int i = static_cast<int>(u * width);
                    int j = static_cast<int>(v * height);
                    if (i >= width) i = width - 1;
                    if (j >= height) j = height - 1;
                    auto pixel = data + j * partsPerScanline + i * partsPerPixel;
                    if (useClamping)
                        res = Math::Color(pow(pixel[0], remapingFactor), pow(pixel[1], remapingFactor), pow(pixel[2], remapingFactor));
                    else
                        res = Math::Color(pixel[0], pixel[1], pixel[2]);
                }
                return res;
            }

        private:
            float* data;
            int width, height;
            int partsPerScanline;
        };
    }
}