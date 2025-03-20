# rack_algorithm

## Overview

This program utilizes a greedy algorithm to handle and process batch data efficiently. Given a number of objects (racks) with a value (sample number) ranging from 1 to 95, it combines the objects into near_capacity racks in as few batches as possible, following several constraints for the batches. The program also allows users to view summaries and export results as a CSV file. Debugging tools are included for testing purposes.

## The Problem

In summer of 2024, a researcher at a local hospital expressed to me their frustration about a tedious lab task they often have to complete manually. They are given hundreds of racks containing anywhere from 1 to 96 samples, and they must combine these racks into full or near-full racks while also minimizing the number of batches. The constraints are as follows:
- Each batch can have up to 20 racks, made up "source" racks (the ones being emptied) and "destination" racks (the ones being filled)
    - for example, you can combine 19 source racks into 1 destination rack, 18 source racks into 2 destination racks, etc
- Each destination rack can hold 96 samples

This means that for a batch with x source racks, there are 20 - x destination racks, and a total sample capacity of (20 - x) * 96.

## How the Algorithm Works

The algorithm starts by reading the rack data and populating the sample_frequencies vector based on the number of racks with each sample number. The program also creates a testing_array vector, which is used to temporarily store and manipulate sample numbers during the process of finding an optimal solution. As sample numbers are added to or removed from the testing_array, the sample_frequencies vector is updated accordingly to reflect these changes. Based on the number of source racks available, the algorithm chooses whether to create another batch using create_new_batch() or distribute_remainder(). 

In create_new_batch(), the program first determines the maximum number of sources that can be used for the current batch so that the total number of sample values doesn't exceed the available spots in the destination racks. The program also saves calculated testing array, consisting of the largest value and (num_sources - 1) of the smallest values, for later approximation purposes. After choosing the number of sources, the program clears the testing array and adds in order: 1) the largest available number, 2) the proportional number of each sample number based on their frequencies (to create a more balanced distribution), and 3) the lowest values available, until there is only 1 source spot available.

The program then calculates the ideal last sample number and checks its availability using the sample_frequencies map. If the ideal last spot is less than 1, the program decreases the total sample number in the testing array by removing a larger number and adding a smaller one, repeating this process until the last spot is greater than 1 and available. If the last spot is greater than 1, the program increases the total sample number in the testing array by removing the smallest sample number and adding a larger one, continuing this until the last spot becomes available or the number to add reaches the highest sample number available. If it reaches the highest sample number and the total in the testing array still doesn’t meet the goal, the program approximates by adding the largest sample number available. If ideal_last_spot becomes negative at any point in increase_testing_total, it instead uses the example testing array provided by choose_num_sources. It then adds the number it found to the testing array, after which it finalizes the batch by adding all the values in the testing array to the current batch object.  

The program continues onto the next batch using create_new_batch() until the number of source racks left is fewer than 19. Then, the program distributes the remaining source racks by adding as many racks as possible to a batch without exceeding the available destination spots based on the number of source racks. When the limit is reached, a new batch is created, and this process continues until all Source Racks have been distributed.

After distributing all the racks, the program asks the user if they would like an overview. The overview describes the number of batches, the count of source and destination racks in each batch, and the number of filled spots in the destination racks for each batch. Finally, it exports the final results to a .csv file, detailing the rack IDs and sample number of each source rack in each batch, along with the total number of spots filled in each batch.

## Notes about compiling and running
Ensure that Visual Studio is installed with the "Desktop development with C++" workload. Build the program and run with "debug".

Input file should be a .txt file. Each line should have a new source rack with a string for the rack id, followed by a space and a numeric value from 1 to 96 indicating the number of samples in the rack. See input file rack_data.txt for an example.

Place the input file in the "Debug" folder that is generated when Visual Studio builds the project. This is also where the output file (a .csv file) should be located once the program creates it.

Program has not been thoroughly tested on a wide variety of data distributions. Based on the researcher's descriptions, most data distributions contain vastly more racks with low sample numbers than high sample numbers, and I designed the program with this in mind.
