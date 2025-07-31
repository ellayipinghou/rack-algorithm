# Rack Algorithm 💉

## Overview 📜

This program automates the tedious process of combining laboratory sample racks into optimal batches. Using a greedy algorithm, it takes hundreds of individual racks (each containing 1-96 samples) and efficiently combines them into near-capacity destination racks, maximizing the utilization space while keeping the total number of processing batches reasonable.

### Key Features ✨:
- Intelligent batch optimization following laboratory constraints
- Automated CSV export for easy integration with existing workflows
- Comprehensive testing suite with 11 test cases
- Built-in debugging functions for validation

## The Problem 💥
In summer 2024, a hospital researcher approached me with a frustrating manual lab task that was eating up hours of their time. They regularly receive hundreds of sample racks containing anywhere from 1 to 96 samples each, and must manually figure out how to combine these into full or near-full racks while minimizing processing batches.

**The Challenge**: The process isn't as simple as just filling up racks - there are strict operational constraints:
- Rack Types: Each batch consists of "source" racks (being emptied) and "destination" racks (being filled)
- Batch Size Limit: Each processing batch can contain a maximum of 20 racks total (source + destination)
- Capacity: Each destination rack holds up to 96 samples
- Balance:  The above constraints mean that for every batch with X source racks, you have up to (20 - X) destination racks and a total capacity of (20 - X) * 96 samples

**Example**: If you have 18 source racks in a batch, you can only have 2 destination racks, giving you a total sample capacity of 2 × 96 = 192 samples

## Getting Started 💻 
### Prerequisites
- Windows environment (tested with Visual Studio's build system)
- Visual Studio with "Desktop development with C++" workload installed
  
### Setup and Running
1. **Clone the repository** and open Rack_Final.vcxproj in Visual Studio
2. **Build the project** using Debug configuration
3. **Prepare your input file:**
    - Create a .txt file with your rack data
    - Format: Each line should contain [Rack_ID] [Sample_Count]
    - Sample counts must be between 1 and 96
4. **Place your input file** in the "inputs" folder, located in the same directory as the Rack_Final.vcxproj file
5. **Run the program** and follow the prompts
6. **Find your results** in the generated CSV file in the "results" folder

### Example Input Format
RACK001 45</br>
RACK002 23</br>
RACK003 67</br>
RACK004 12</br>

## Details: How the Algorithm Works 🔬

### Data Initialization 📝
The algorithm begins by reading rack data and populating a sample_frequencies array that tracks how many racks contain each sample number (1-95). A testing_array vector is used to temporarily store and manipulate sample numbers while finding optimal batch combinations. As sample numbers are added to or removed from the testing array, the frequencies are updated in real-time to reflect availability.

### Main Distribution Loop 💫
The program distributes all source racks using two main strategies:

#### Optimization Phase (≥19 source racks remaining):
- Uses create_new_batch() to find optimal combinations from abundant choices
- Continues until fewer than 19 source racks remain

#### Cleanup Phase (<19 source racks remaining):
- Uses distribute_remainder() to efficiently pack remaining racks
- Creates batches by adding as many racks as possible without exceeding destination capacity
- Continues until all racks are distributed

### Batch Creation Process (create_new_batch) 📊
#### 1. Determine Batch Size
- choose_num_sources() finds the maximum number of source racks that can fit in one batch
- Starts with the largest available sample number
- Incrementally adds smallest sources until destination capacity would be exceeded
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

### Remainder Distribution (distribute_remainder) 🔎
For the final racks, the algorithm uses a simpler greedy approach:
- Processes remaining source racks in their current order (how they were read in)
- For each potential source rack, updates required destination racks based on cumulative sample count
- Stops adding source racks when number of source racks + destination racks in the batch exceeds 20
- Creates new batches as needed until all remaining racks are distributed

### Output and Results 📈
After distribution is complete, the program:
- Provides an optional summary showing batch counts, source/destination ratios, and capacity utilization
- Exports detailed results to a CSV file with columns: Rack ID, Sample Number, Batch Number, Number Sources, Number Destinations, and Total Samples In Batch (with one row per source rack)
    - The generated CSV file can be found in the "results" folder located in the same folder as Rack_Final.vcxproj

## Test Files 📂
The program includes 11 comprehensive test cases designed to validate the algorithm's performance across different data distributions and edge cases. These test files help ensure the algorithm works correctly under various real-world scenarios.

**What's included:**
- **Test input files**: rack_input_1.txt through rack_input_11.txt
- **Test result files**: result_1.csv through result_11.csv
- **Test documentation**: input_descriptions.txt provides detailed descriptions of each test case and what it's designed to validate

Input files are located in the "inputs" folder, in the same directory as Rack_Final.vcxproj

## Important Notes ⚠️
- The algorithm is optimized for typical laboratory distributions, which are generally skewed toward lower-numbered samples based on real-world usage patterns
  - Although the provided test files include some non-standard data distributions, performance may vary with these less typical cases
- The program contains little to no error-checking for invalid formats, so please ensure your inputs are formatted correctly
