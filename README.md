# rack_algorithm

## The Problem

In summer of 2024, a researcher at a local hospital expressed to me their desire for a rack distribution algorithm. They often deal with hundreds of racks at a time, each of which contains anywhere from 1 to 96 samples. Typically, there are a lot of racks with just 1 or 2 samples, but this frequency decreases drastically as the sample number increases. The researcher's task is to combine racks with fewer samples into others, filling them up as much as possible (up to 96 samples per rack), while also minimizing the number of batches they work in. Each batch can have up to 20 racks, and these racks can be a mix of "source" racks (the ones being emptied) and "destination" racks (the ones being filled). For example, they might combine 19 source racks into 1 destination rack, or 18 source racks into 2 destination racks, etc.

## The Solution - Overview

Based on the information given about the typical sample distribution, I devised a C++ console program that efficiently combines source racks into destination racks in batches of 20. The algorithm uses the frequency of sample numbers from the source rack input data to choose the number of source racks in each batch and to create the batch's combination. The results from the sample input are satisfactory, as the program created 42 batches from 777 racks, the majority of which filled all the spots on the destination racks.

## The Solution - Details

The algorithm first reads in rack data and create a new Source_Rack object for each instance, initializing the rack id and sample number. Each Source_rack is pushed to to the back of an all_sources vector. The program then uses this vector to collect data about the number of racks with each sample number and stores the information in a "sample_frequencies" unordered map (note: I will be referring to this as an array from now on). The Source_Rack and sample_freqency structures are a part of the Program class, along with a Batch class that can hold a list of Source_Racks and a testing_array vector that the program uses to try out different sample combinations. As different numbers are added and removed from the testing array, the sample frequencies array is updated accordingly. (Note that the testing array only contains ints that represent the sample numbers of the source racks, so no source racks are actually added to the batch until the combination is finalized).

For each batch, the program chooses the highest number of source racks it can use if it adds the largest sample rack available to the testing array and fills the rest of the spots with the lowest-sample racks available. For example, if the highest sample rack has 94 samples, then there are 18 spots left at most, which the program fills with 1's. Now, the total number of samples in the batch is 112, which is greater than the 96 spots available. This is because 19 source racks leaves room for only 1 destination rack, so the program must try 18 source racks instead. With 18 source racks, there are 2 destination racks and therefore 192 destination rack spots - the total sample sum is 111, which is less than the 192, so it chooses this option.

After choosing the number of Source Racks in a Batch, the program empties the testing array and begins adding sample numbers. First it adds the highest sample number, and then it calculates the ratio of each sample number's frequency to the total sample sum of the available racks, truncating the result into an integer. Using this ratio, it adds a proportional amount of each sample number, and if there is more than 1 spot leftover, fills all except 1 with the lowest sample numbers available. (Note: there is a possible bug here if there is an unexpectedly high number of racks with high sample numbers, since the use of ratios may result in there being no extra spot, which is crucial to the next step).

It then calculates the ideal last sample number and checks if it is available based on the sample_frequencies array. If it is, it adds the final sample number to the testing array. Then, for each value in the testing array, it searches through the all_sources vector and adds a source rack with the corresponding sample number to the batch. Then, it removes the source rack from all_sources, updates the sample_frequencies array, and moves onto the next batch.

If the ideal last spot is not available, the program checks if the last spot is less than 1. If so, it decreases the total sample number in the testing array by removing a large number and adds a smaller number, repeating until the last spot is greater than 1 and available. If the last spot is not less than 1, the program increases the total sample number in the testing array by removing the smallest sample number and adding a larger number, repeating until the last spot is available or the number to add has reached the highest sample number available. If this is the case and the sum of the testing array still does not reach the goal number of samples, it approximates by adding the largest possible sample number - the rack will not be full, but it will be as close as possible. After finalizing the spots, the program moves onto the next batch.

After distributing all the racks, the program asks the user if they would like an overview. The overview includes the number of batches, the number of source and destination racks in each batch, and the number of spots filled in the destination racks of each batch. Then, it exports the final results to a .csv file, which lists the rack ids and sample number of each source rack in each batch, as well as the total number of spots filled in the batch.

## Notes about compiling and running
Ensure that Visual Studio is installed and the "Desktop development with C++" workload is selected before building the program.

Input file should be a .txt file. Each line should have a new source rack with a string for the rack id, followed by a space and a numeric value from 1 to 96 indicating the number of samples in the rack. See input file rack_data.txt for an example.

Place the input file in the "Debug" folder that is generated when Visual Studio builds the project. This is also where the output file (a .csv file) should be located once the program creates it.

Program may have bugs if there are large sample numbers with unexpectedly high frequencies in the source rack data.
