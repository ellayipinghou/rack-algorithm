# Rack Algorithm 💉

## Overview 📜

This program utilizes a greedy algorithm to handle and process batch data efficiently. Given a number of objects (racks) with a value (sample number) ranging from 1 to 95, it combines the objects into near_capacity racks in as few batches as possible, following several constraints for the batches. The program also allows users to view summaries and export results as a CSV file. Debugging tools are included for testing purposes.

## The Problem 💥

In summer of 2024, a researcher at a local hospital expressed to me their frustration about a tedious lab task they often have to complete manually. They are given hundreds of racks containing anywhere from 1 to 96 samples, and they must combine these racks into full or near-full racks while also minimizing the number of batches. The constraints are as follows:
- Each batch can have up to 20 racks, made up "source" racks (the ones being emptied) and "destination" racks (the ones being filled)
    - for example, you can combine 19 source racks into 1 destination rack, 18 source racks into 2 destination racks, etc
- Each destination rack can hold 96 samples

This means that for a batch with x source racks, there are 20 - x destination racks, and a total sample capacity of (20 - x) * 96.

## Details: How the Algorithm Works 🔎

### Data Initialization
The algorithm begins by reading rack data and populating a sample_frequencies array that tracks how many racks contain each sample number (1-95). A testing_array vector is used to temporarily store and manipulate sample numbers while finding optimal batch combinations. As sample numbers are added to or removed from the testing array, the frequencies are updated in real-time to reflect availability.

### Main Distribution Loop
The program distributes all source racks using two main strategies:

#### For Large Batches (≥19 source racks remaining):
- Uses create_new_batch() to optimize each batch individually
- Continues until fewer than 19 source racks remain

#### For Remaining Racks (<19 source racks):
- Uses distribute_remainder() to efficiently pack remaining racks
- Creates batches by adding as many racks as possible without exceeding destination capacity
- Continues until all racks are distributed

### Batch Creation Process (create_new_batch)
#### 1. Determine Batch Size
- choose_num_sources() finds the maximum number of source racks that can fit in one batch
- Starts with the largest available sample number plus smallest values
- Incrementally adds sources until destination capacity would be exceeded
- Saves a backup_array as the last valid configuration before exceeding capacity

#### 2. Build Optimal Combination
- Clears the testing array and rebuilds it strategically:
    - Adds the largest available sample number first
    - Fills remaining spots proportionally based on sample frequencies (for balanced distribution)
    - Fills any remaining spots with the smallest available values
    - Leaves exactly one spot open for the "ideal last sample"

#### 3. Calculate and Adjust Final Sample
- Calculates the ideal_last_spot by subtracting current total from destination capacity
- If ideal value is too small (<1): Decreases total by replacing larger values with smaller ones
- If ideal value is too large (>95) or unavailable: Increases total by replacing smaller values with larger ones
- Fallback mechanism: If adjustments fail, uses the pre-calculated backup_array

#### 4. Finalize Batch
- Adds the final calculated sample number to complete the batch
- Creates a new Batch object and assigns actual source racks based on testing array values
- Removes used racks from the available pool

### Remainder Distribution (distribute_remainder)
For the final racks, the algorithm uses a simpler greedy approach:
- Calculates how many racks can fit in the current batch based on sample totals
- Ensures total sources + destinations doesn't exceed 20 racks per batch
- Creates new batches as needed until all racks are distributed

### Output and Results
After distribution is complete, the program:
- Provides an optional summary showing batch counts, source/destination ratios, and capacity utilization
- Exports detailed results to a CSV file with columns: Rack ID, Sample Number, Batch Number, Number Sources, Number Destinations, and Total Samples In Batch (with one row per source rack)

## Test Files 📂
The program includes 11 comprehensive test cases to validate the algorithm's performance across various data distributions. These test files, as well as a text file describing each input in more deatil (input_descriptions.txt) can be found in the same folder as "Rack_Final.vcxproj".

## Notes about compiling and running 💻
Ensure that Visual Studio is installed with the "Desktop development with C++" workload. Build the program and run with "debug".

Input file should be a .txt file. Each line should have a new source rack with a string for the rack id, followed by a space and a numeric value from 1 to 96 indicating the number of samples in the rack. See input file rack_data.txt for an example.

Place the input file in the "Debug" folder that is generated when Visual Studio builds the project. This is also where the output file (a .csv file) should be located once the program creates it.

Program has not been thoroughly tested on a wide variety of data distributions. Based on the researcher's descriptions, most data distributions contain vastly more racks with low sample numbers than high sample numbers, and I designed the program with this in mind.
