#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <utility>
using namespace cimg_library;

int main(int argc, char *argv[])
{
    CImg<double> input(argv[1]);
    CImg<double> lab = input.RGBtoLab();
    Eigen::Vector3d *image = new Eigen::Vector3d[input.width() * input.height()];

    // Copy image data to Eigen::Vector3d array
    for (unsigned int i = 0; i < input.width(); i++)
    {
        for (unsigned int j = 0; j < input.height(); j++)
        {
            image[i * input.height() + j][0] = lab(i, j, 0);
            image[i * input.height() + j][1] = lab(i, j, 1);
            image[i * input.height() + j][2] = lab(i, j, 2);
        }
    }

    // Compute energy using a simple gradient-based approach
    float energy[input.width() * input.height()];

    for (unsigned int i = 0; i < input.width(); i++)
    {
        for (unsigned int j = 0; j < input.height(); j++)
        {
            // Compute gradient in x and y directions
            float dx = 0, dy = 0;

            if (i > 0)
                dx += (image[i * input.height() + j] - image[(i - 1) * input.height() + j]).norm();
            if (i < input.width() - 1)
                dx += (image[i * input.height() + j] - image[(i + 1) * input.height() + j]).norm();

            if (j > 0)
                dy += (image[i * input.height() + j] - image[i * input.height() + j - 1]).norm();
            if (j < input.height() - 1)
                dy += (image[i * input.height() + j] - image[i * input.height() + j + 1]).norm();

            energy[i * input.height() + j] = dx + dy;
        }
    }

    // Carve
    CImg<double> output(atoi(argv[3]), atoi(argv[4]), input.depth(), input.spectrum(), 0);

    // Find number of vert_seam/hor_seam to remove
    int vert_seam = input.width() - output.width();
    int hor_seam = input.height() - output.height();

    // Remove vertical seams
    while (vert_seam)
    {
        std::vector<std::pair<int, int>> seam;
        int x;
        float min_e = 10000.0;

        // Find the minimum energy pixel in the first row
        for (unsigned int i = 0; i < input.width(); i++)
        {
            if (energy[i * input.height()] < min_e)
            {
                min_e = energy[i * input.height()];
                x = i;
            }
        }
        seam.push_back(std::make_pair(x, 0));

        // Traverse the seam to find the lowest energy path
        for (unsigned int j = 1; j < input.height(); j++)
        {
            // Logic to find the next pixel in the seam based on energy values
            if (x > 0 && energy[(x - 1) * input.height() + j] < energy[x * input.height() + j])
            {
                x--;
            }
            else if (x < input.width() - 1 && energy[(x + 1) * input.height() + j] < energy[x * input.height() + j])
            {
                x++;
            }
            seam.push_back(std::make_pair(x, j));

            // Mark the seam for debugging
            image[x * input.height() + j][0] = 0.0;
            image[x * input.height() + j][1] = 255.0;
            image[x * input.height() + j][2] = 255.0;
        }

        // Remove the seam by shifting pixels
        for (unsigned int j = 0; j < input.height(); j++)
        {
            for (unsigned int i = seam[j].first; i < input.width() - 1; i++)
            {
                image[i * input.height() + j] = image[(i + 1) * input.height() + j];
            }
        }

        // Update energy values only for non-removed pixels
        for (unsigned int j = 0; j < input.height(); j++)
        {
            for (unsigned int i = std::max(0, seam[j].first - 1); i < std::min(input.width(), seam[j].first + 1); i++)
            {
                // Recalculate energy for nearby pixels
                float dx = 0, dy = 0;

                if (i > 0)
                    dx += (image[i * input.height() + j] - image[(i - 1) * input.height() + j]).norm();
                if (i < input.width() - 1)
                    dx += (image[i * input.height() + j] - image[(i + 1) * input.height() + j]).norm();

                if (j > 0)
                    dy += (image[i * input.height() + j] - image[i * input.height() + j - 1]).norm();
                if (j < input.height() - 1)
                    dy += (image[i * input.height() + j] - image[i * input.height() + j + 1]).norm();

                energy[i * input.height() + j] = dx + dy;
            }
        }

        seam.clear();
        vert_seam--;
    }
    // Transpose the image
    CImg<double> transposed = output.get_transpose();
    // Remove horizontal seams (height carving)
    while (hor_seam)
    {
        std::vector<std::pair<int, int>> seam;
        int y;
        float min_e = 10000.0;

        // Find the minimum energy pixel in the first column
        for (unsigned int j = 0; j < transposed.height(); j++)
        {
            if (energy[j * transposed.width()] < min_e)
            {
                min_e = energy[j * transposed.width()];
                y = j;
            }
        }
        seam.push_back(std::make_pair(0, y));

        // Traverse the seam to find the lowest energy path
        for (unsigned int i = 1; i < transposed.width(); i++)
        {
            // Logic to find the next pixel in the seam based on energy values
            if (y > 0 && energy[i * transposed.height() + y - 1] < energy[i * transposed.height() + y])
            {
                y--;
            }
            else if (y < transposed.height() - 1 && energy[i * transposed.height() + y + 1] < energy[i * transposed.height() + y])
            {
                y++;
            }
            seam.push_back(std::make_pair(i, y));

            // Mark the seam for debugging
            image[i * transposed.height() + y][0] = 0.0;
            image[i * transposed.height() + y][1] = 255.0;
            image[i * transposed.height() + y][2] = 255.0;
        }

        // Remove the seam by shifting pixels
        for (unsigned int i = 0; i < transposed.width(); i++)
        {
            for (unsigned int j = seam[i].second; j < transposed.height() - 1; j++)
            {
                image[i * transposed.height() + j] = image[i * transposed.height() + j + 1];
            }
        }

        // Update energy values
        for (unsigned int i = 0; i < transposed.width(); i++)
        {
            for (unsigned int j = std::max(0, seam[i].second - 1); j < std::min(transposed.height(), seam[i].second + 2); j++)
            {
                // Recalculate energy for nearby pixels
                float dx = 0, dy = 0;

                if (i > 0)
                    dx += (image[i * transposed.height() + j] - image[(i - 1) * transposed.height() + j]).norm();
                if (i < transposed.width() - 1)
                    dx += (image[i * transposed.height() + j] - image[(i + 1) * transposed.height() + j]).norm();

                if (j > 0)
                    dy += (image[i * transposed.height() + j] - image[i * transposed.height() + j - 1]).norm();
                if (j < transposed.height() - 1)
                    dy += (image[i * transposed.height() + j] - image[i * transposed.height() + j + 1]).norm();

                energy[i * transposed.height() + j] = dx + dy;
            }
        }

        seam.clear();
        hor_seam--;
    }
    // Transpose the result back to the original orientation
    CImg<double> final_result = transposed.get_transpose();
    // Create output image
    for (unsigned int i = 0; i < output.width(); i++)
    {
        for (unsigned int j = 0; j < output.height(); j++)
        {
            output(i, j, 0) = image[i * output.height() + j][0];
            output(i, j, 1) = image[i * output.height() + j][1];
            output(i, j, 2) = image[i * output.height() + j][2];
        }
    }

    // Output image
    CImg<double> rgb = output.LabtoRGB();
    if (strstr(argv[2], "png"))
        rgb.save_png(argv[2]);
    else if (strstr(argv[2], "jpg"))
        rgb.save_jpeg(argv[2]);

    delete[] image;
    return 0;
}