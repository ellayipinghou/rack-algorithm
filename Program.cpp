/*
Program.cpp
Implementation of the Program class. See comments within this file or Program.h header comment for more information.
*/

#include <iostream>
#include <fstream>
#include <string>
#include "Program.h"

using namespace std;

/* 
* name: read_data
* purpose: Prompts users to provide a file for rack data, creates a new Source Rack object for each line, and pushes them back to
           the all_sources vector. 
* arguments: none
* returns: none
* notes: rack data file must contain a new line for every Source Rack, represented by a rack id, followed by a space and an int sample
         number. Ex. SAMPLEid1 23
*/
void Program::read_data() {
    string filename;
    cout << "This is the Rack Distribution Program, please enter the name of a txt data file (must include .txt extension at end of name)" << endl;
    cin >> filename;

    ifstream infile;
    infile.open(filename);
    if (infile.fail()) {
        cout << "Error opening file. Check that file name is valid and that the file is located in the inner project folder" << endl;
        exit(EXIT_FAILURE);
    }
    while (!infile.eof()) {
        Source_Rack current_source;
        infile >> current_source.id;
        infile >> current_source.num_samples;
        all_sources.push_back(current_source);
    }
}

/*
* name: populate_frequencies
* purpose: iterates through the all_sources vector and updates the sample_frequencies hash map based on the number of sources with that 
           number of samples
* arguments: none
* returns: none
* notes: none
*/
void Program::populate_frequencies() {
    for (int i = 0; i < all_sources.size(); i++) {
        sample_frequencies[all_sources.at(i).num_samples]++;
    }
}

/*
* name: find_largest
* purpose: returns the largest undistributed sample number based on sample_frequencies
* arguments: none
* returns: the largest remaining sample number
* notes: none
*/
int Program::find_largest() {
    for (int i = rack_capacity; i > 0; i--) {
        //starting from the highest number, return the first sample num with non-zero frequency
        if (sample_frequencies[i] != 0) {
            return i;
        }
    }
    //if all are 0, all source racks have been used
    cerr << "error, see line 64" << endl;
    exit(EXIT_FAILURE);
}

/*
* name: distribute_racks
* purpose: main wrapper function that distributes all the source racks into batches
* arguments: none
* returns: none
* notes: none
*/
void Program::distribute_racks() {
    while (all_sources.size() > 19) {
        //choose the number of sources for the current batch
        int num_sources = choose_num_sources();
        create_new_batch(num_sources);
    }
    //if less than 19 left, create the last batch with all remaining sources
    distribute_remainder();
}

/*
* name: distribute_remainder
* purpose: create the last batch with all remaining sources
* arguments: none
* returns: none
* notes: only use when less than 19 sources are left to be distributed
*/
void Program::distribute_remainder() {
    //continue making new batches until there are no more sources left
    while (all_sources.size() > 0) {
        //reset testing array
        testing_array.clear();

        int sum = 0;
        for (int i = 0; i < all_sources.size(); i++) {
            sum += all_sources.at(i).num_samples;
            //i is the number of sources currently, so check if the sum surpasses the number of spots in the destination racks
            if (sum > (20 - i) * 96) {
                break;
            }
            testing_array.push_back(all_sources.at(i).num_samples);
        }
        //finalize spots by adding source racks to a new Batch, removing the source racks from all_sources, and pushing that Batch back
        finished_batches.push_back(finalize_spots());
    }
}

/*
* name: choose_num_sources
* purpose: chooses the number of sources to use for the current Batch based on sample_frequencies
* arguments: none
* returns: the number of sources to use
* notes: see comments for more information on algorithm
*/
int Program::choose_num_sources() {
    //reset the testing array
    testing_array.clear();

    //start with the smallest number of sources
    int num_sources = 1;
    int destination_spots = (20 - num_sources) * 96;

    //add the largest value
    int largest = find_largest();
    add_in_order(testing_array, largest);

    //find the most sources we can add to the highest value without the sample total exceeding the number of destination spots
    while (total_testing_samples() <= destination_spots && num_sources < 20) {
        //add the remaining spots with the smallest value left
        add_in_order(testing_array, find_smallest());
        //update number of sources and destination spots
        num_sources++;
        destination_spots = (20 - num_sources) * 96;
    }
    //remove all values from testing array to reset for when the values are actually added
    int num_to_remove = testing_array.size();
    for (int i = 0; i < num_to_remove; i++) {
        remove_from_testing(testing_array.at(0));
    }
    //at the point, the total has exceeding the destination spots, so decrease num sources and return
    return num_sources - 1;
}

/*
* name: is_valid
* purpose: tells whether there is at least 1 source rack with the given sample number left to be distributed
* arguments: none
* returns: true if valid, false if not valid
* notes: none
*/
bool Program::is_valid(int num) {
    if (sample_frequencies[num] > 0) {
        return true;
    }
    return false;
}

/*
* name: create_new_batch
* purpose: main function that finds the best combination for the next batch and creates it
* arguments: none
* returns: none
* notes: see comments for more details on algorithm
*/
void Program::create_new_batch(int num_source_racks) {
    //reset testing array each time
    testing_array.clear();

    //add all values except one and find the ideal last spot
    add_all_except_last(num_source_racks);
    fill_valid_sample_nums();
    int ideal_last_spot = ideal_last(num_source_racks);

    //decrease the total if necessary
    int to_add;
    decrease_testing_total(ideal_last_spot, to_add, num_source_racks);

    //increase total if necessary
    bool approximate = false;
    increase_testing_total(ideal_last_spot, to_add, num_source_racks, approximate);

    //if we've replaced the smallest in the array with the largest and the destination spots are still not fully filled, just add the largest available
    if (approximate) {
        add_in_order(testing_array, find_largest());
    }
    else {
        add_in_order(testing_array, ideal_last_spot);
    }
    //create the batch, add sources with corresponding sample numbers to it, and push the batch to the end of the finished_batches vector
    finished_batches.push_back(finalize_spots());
}


/*
* name: decrease_testing_total
* purpose: decrease sum of the samples in the testing array by replacing the higher values with lower values if necessary
* arguments: int references to the ideal last spot, the value to replace with, and the number of source racks in the current batch
* returns: none
* notes: the largest value is never replaced because we have validated using choose_num_samples that its sum with the
*        smallest remaining valid sample numbers will be less than or equal to the number of destination spots available
*/
void Program::decrease_testing_total(int& ideal_last_spot, int &to_add, int &num_source_racks) {
    //decrease only necessary if sum is higher than the number of available spots
    while (ideal_last_spot < 1) {

        //find the second largest value, which we will remove
        int second_largest = testing_array.at(testing_array.size() - 2);

        //decrement to find the next highest valid value, which we will add
        to_add = find_next_highest_valid(second_largest);

        //replace - note, these methods update the sample_frequencies array
        remove_from_testing(second_largest);
        add_in_order(testing_array, to_add);

        //update to_add and ideal_last_spot for the next loop
        to_add = find_next_highest_valid(to_add);
        ideal_last_spot = ideal_last(num_source_racks);
    }
}

/*
* name: find_next_highest_valid
* purpose: returns the next highest valid number based on the valid_sample_nums vector
* arguments: an int (current) for the value to start decrementing at
* returns: the next highest valid sample number
* notes: approximate is not needed here because this is just to get the total sample numbers to be less than the goal if necessary, afterwards we start
*        increasing the total again and approximate will be called by find_next_smallest_valid if needed
*/
int Program::find_next_highest_valid(int& current) {
    if (current == valid_sample_nums.at(1)) {
        //Note: it should never be the case that even adding all the smallest values to the largest value will be greater than the total,
        //since we've validated it with choose_num_sources
        return valid_sample_nums.at(0);
    }
    for (int i = valid_sample_nums.size() - 1; i > 0; i--) {
        if (current == valid_sample_nums.at(i)) {
            return valid_sample_nums.at(i - 1);
        }
    }
    //if made it to the end, then current doesn't exist in the valid sample numbers array
    cerr << "error: current does not exist in the valid sample nums or it is the smallest value already";
    exit(EXIT_FAILURE);
}

/*
* name: increase_testing_total
* purpose: increase sum of the samples in the testing array by replacing the lower values with higher values until the goal is reached or the highest remaining value is reached
* arguments: int references to the ideal last spot, the value to replace with, and the number of source racks in the current batch, bool reference that determines
*            whether or not the program should just approximate the total
* returns: none
* notes: none
*/
void Program::increase_testing_total(int& ideal_last_spot, int& to_add, int& num_source_racks, bool& approximate) {
    //increase until the ideal last spot is available or until the we've reached the largest possible value
    while (ideal_last_spot < 96 && !is_valid(ideal_last_spot)) {

        //find the smallest value, which we will remove
        int smallest_in_testing = valid_sample_nums.at(0);

        //increment to find the next smallest value, which we will add
        to_add = find_next_smallest_valid(smallest_in_testing, approximate);

        //if we've reached the highest possible value, return - we will just add the highest available value instead
        if (approximate) {
            return;
        }

        //remove the smallest and add the larger number, updating sample_frequencies and valid_sample_nums
        remove_from_testing(smallest_in_testing);
        add_in_order(testing_array, to_add);
        valid_sample_nums.erase(valid_sample_nums.begin());

        //update the ideal last spot for next loop
        ideal_last_spot = ideal_last(num_source_racks);
    }
}

/*
* name: find_next_smallest_valid
* purpose: returns the next smallest valid number based on the valid_sample_nums vector
* arguments: an int (current) for the value to start incrementing at and a bool that represents whether current is already the largest valid number and can no longer be incremented
* returns: the next smallest valid number
* notes: setting approximate to be true indicates that the program should add the largest valid number left, even it it doesn't make
         the sample total reach the desired total
*/
int Program::find_next_smallest_valid(int& current, bool& approximate) {
    if (valid_sample_nums.size() == 1) {
        approximate = true;
        return -1;
    }
    //return the next value in the valid_sample_nums array
    for (int i = 0; i < valid_sample_nums.size() - 1; i++) {
        if (current == valid_sample_nums.at(i)) {
            return valid_sample_nums.at(i + 1);
        }
    }
    //if made it to the end, then current doesn't exist in the valid sample numbers array
    cerr << "error: current isn't in valid list when adding next smallest" << endl;
    exit(EXIT_FAILURE);
}

/*
* name: export_results
* purpose: exports the results of the algorithm
* arguments: none
* returns: none
* notes: output will be a csv file containing the id, sample number, batch number, and number of samples in the batch for
*        each batch created
*/
void Program::export_results() {
    cout << "A csv file containing the results will be exported, please enter the name you would like to save the file as:" << endl;
    string output_name;
    cin >> output_name;
    string test = output_name.substr(output_name.size() - 4, 4);
    if (test != ".csv") {
        output_name += ".csv";
    }

    ofstream outfile;
    outfile.open(output_name);
    if (outfile.fail()) {
        cout << "Error creating file. Please check that the inputted file name is valid and re-run program" << endl;
    }

    //create output string and add information
    string output = "Rack ID,Sample Number,Batch Number,Number of Samples In Batch\n";

    for (int i = 0; i < finished_batches.size(); i++) {
        for (int j = 0; j < finished_batches.at(i).batch_sources.size(); j++) {
            //add ID to output
            output += (finished_batches.at(i).batch_sources.at(j).id);
            output += ",";

            //add number of samples in rack to output
            output += to_string(finished_batches.at(i).batch_sources.at(j).num_samples);
            output += ",";

            //add batch number to output
            output += to_string(i + 1);
            output += ",";

            //add number of samples in batch
            int total_spots_filled = 0;
            for (int k = 0; k < finished_batches.at(i).batch_sources.size(); k++) {
                total_spots_filled += finished_batches.at(i).batch_sources.at(k).num_samples;
            }
            output += to_string(total_spots_filled);
            output += "\n";
        }
        output += "\n";
    }

    outfile << output << endl;
    cout << endl << "Your text file has been created with the name " << output_name << endl;
    cout << "Output file should be located in same folder as input file" << endl;

    outfile.close();
}

/*
* name: print_summary
* purpose: prints a summary of the number of batches, the number of samples in each batch and the total sample sum of the batch
* arguments: none
* returns: none
* notes: none
*/
void Program::print_summary() {
    string overview;
    cout << "The program has created a solution, would you like to see to see an overview before exporting as a csv file? (y/n)" << endl;
    cin >> overview;

    if (overview == "y" || overview == "yes") {
        cout << "Number of batches is: " << finished_batches.size() << endl;
        for (int i = 0; i < finished_batches.size(); i++) {
            cout << "-- Batch number " << i + 1 << " --" << endl;
            cout << "Number of sources in this batch: " << finished_batches.at(i).batch_sources.size() << endl;

            int total_spots_filled = 0;
            for (int k = 0; k < finished_batches.at(i).batch_sources.size(); k++) {
                total_spots_filled += finished_batches.at(i).batch_sources.at(k).num_samples;
            }

            int num_destinations = total_spots_filled / 96;
            if (total_spots_filled % 96 != 0) {
                num_destinations++;
            }
            cout << "Number of destinations in this batch: " << num_destinations << endl;
            cout << "Number of spots filled in destination racks: " << total_spots_filled << endl;
        }
        cout << endl;
    }
}

/*
* name: fill_valid_sample_nums
* purpose: fills the valid_sample_nums vector with all sample numbers where the frequency is greater than 0 (meaning it is available to add to a batch)
* arguments: none
* returns: none
* notes: this function makes it easier to get the next largest or smallest sample number rather than directly using the sample_frequencies
*        array and having to keep incrementing or decrementing until the frequency is not 0
*/
void Program::fill_valid_sample_nums() {
    //reset vector
    valid_sample_nums.clear();
    for (auto it : sample_frequencies) {
        if (it.second > 0) {
            add_in_order(valid_sample_nums, it.first);
        }
    }
}

/*
* name: print_frequencies
* purpose: prints each key-value pair in the sample_frequencies, which is constantly updated as sample numbers are
           removed and added to testing array
* arguments: none
* returns: none
* notes: only used for testing purposes
*/
void Program::print_frequencies() {
    cout << endl;
    for (auto it : sample_frequencies) {
        cout << it.first << ": " << it.second << endl;
    }
}

/*
* name: print_sources
* purpose: prints the id and sample number of each source remaining in the all_sources vector
* arguments: none
* returns: none
* notes: only used for testing purposes
*/
void Program::print_sources() {
    for (int i = 0; i < all_sources.size(); i++) {
        cout << all_sources.at(i).id << " " << all_sources.at(i).num_samples << endl;
    }
}

/*
* name: print_sources
* purpose: prints a list of all the valid (available) sample numbers
* arguments: none
* returns: none
* notes: only used for testing purposes
*/
void Program::print_valid_sample_nums() {
    cout << "printing valid sample numbers: ";
    for (int i = 0; i < valid_sample_nums.size(); i++) {
        cout << valid_sample_nums.at(i) << " ";
    }
    cout << endl;
}

/*
* name: finalize_spots
* purpose: creates a new Batch and adds sources to the Batch based on the values in the testing array
* arguments: none
* returns: the Batch that was created
* notes: none
*/
Program::Batch Program::finalize_spots() {
    Batch curr_batch;
    curr_batch.batch_num = finished_batches.size() + 1;
    for (int i = 0; i < testing_array.size(); i++) {
        Source_Rack curr_source = find_source(testing_array.at(i));
        curr_batch.batch_sources.push_back(curr_source);
    }
    return curr_batch;
}

/*
* name: find_source
* purpose: finds and returns a Source_Rack in the all_sources array that has the sample number given, then removes it from the array
* arguments: an int sample number to find a Source_Rack for
* returns: none
* notes: none
*/
Program::Source_Rack Program::find_source(int sample_num) {
    for (int i = 0; i < all_sources.size(); i++) {
        if (all_sources.at(i).num_samples == sample_num) {
            //create new source rack with the same data members to return
            Source_Rack my_source;
            my_source.id = all_sources.at(i).id;
            my_source.num_samples = sample_num;

            //remove source rack from vector
            all_sources.erase(all_sources.begin() + i);
            return my_source;
        }
    }
    //if not found, something went wrong
    cerr << "source " << sample_num << "not found when finalizing spots, exiting now";
    exit(EXIT_FAILURE);
}

/*
* name: add_all_except_last
* purpose: adds the highest available sample number and fills the rest of the spots except for 1 with the smallest available sample numbers
* arguments: the number of source racks in the current batch
* returns: none
* notes: none
*/
void Program::add_all_except_last(int num_source_racks) {
    //add largest value
    add_in_order(testing_array, find_largest());

    //add frequencies based on ratios
    add_ratios(num_source_racks);

    //add extra of the smallest value to add until there is only 1 spot left
    while (testing_array.size() < num_source_racks - 1) {
        if (find_smallest() == -1) {
            cerr << "no more source racks to distribute, returning now" << endl;
            return;
        }
        else {
            add_in_order(testing_array, find_smallest());
        }
    }
}

/*
* name: ideal_last
* purpose: calculates the ideal last number by subtracting the total sum of the testing array from the number of available spots in the destination racks
* arguments: the number of source racks in the current batch
* returns: an int representing the ideal last number
* notes: none
*/
int Program::ideal_last(int num_source_racks) {
    int goal_sample_num = (20 - num_source_racks) * rack_capacity;
    return goal_sample_num - total_testing_samples();
}

/*
* name: total_testing_samples
* purpose: calculates the total sum of all the sample numbers in the testing array
* arguments: none
* returns: an int representing the total sum
* notes: none
*/
int Program::total_testing_samples() {
    int total = 0;
    for (int i = 0; i < testing_array.size(); i++) {
        total += testing_array.at(i);
    }
    return total;
}

/*
* name: find_smallest
* purpose: finds the smallest available sample number
* arguments: none
* returns: an int representing the smallest available sample number
* notes: none
*/
int Program::find_smallest() {
    for (int i = 1; i <= rack_capacity; i++) {
        if (sample_frequencies[i] != 0) {
            return i;
        }
    }
    //if no source racks left, return -1
    return -1;
}

/*
* name: add_ratios
* purpose: adds sample numbers to the testing array proportionally to their frequencies
* arguments: none
* returns: none
* notes: this may cause an bug if there are large sample numbers with an unexpectedly high frequency
*/
void Program::add_ratios(int num_source_racks) {
    //calculate how many 1's, 2's, etc to used based on frequencies
    for (int i = 1; i <= rack_capacity; i++) {
        //if there are no source_racks with that number of samples left, move on
        if (sample_frequencies[i] == 0) {
            continue;
        }
        else {
            //find amount to add based on ratio, truncate to an integer
            int amount_to_add = (sample_frequencies[i] * num_source_racks) / all_sources.size();
            //add that amount of the sample number to the vector as long as there are enough
            while (amount_to_add > 0 && sample_frequencies[i] > 0) {
                add_in_order(testing_array, i);
                amount_to_add--;
            }
        }
    }

}

/*
* name: print_testing_array
* purpose: prints all the values currently in the testing array
* arguments: none
* returns: none
* notes: only used for testing purposes
*/
void Program::print_testing_array() {
    if (testing_array.size() == 0) {
        cerr << "testing array is empty" << endl;
        return;
    }
    cout << "testing array: ";
    for (int i = 0; i < testing_array.size(); i++) {
        cout << testing_array.at(i) << " ";
    }
    cout << endl;
}

/*
* name: add_in_order
* purpose: adds sample numbers to a vector in non-decreasing order
* arguments: the vector to add the sample number to, the sample number to add
* returns: none
* notes: used with both the testing array and the valid sample numbers array. if adding to the testing array, decreases the corresponding frequency in the sample_frequencies
*        array to update availability
*/
void Program::add_in_order(vector<int> &which_vector, int to_add) {
    //if array is empty or to_add is the largest value, add to back
    if (which_vector.size() == 0 || to_add >= which_vector.at(which_vector.size() - 1)) {
        which_vector.push_back(to_add);
    } else {
        //otherwise, add to the first spot that would be in order
        for (int i = 0; i < which_vector.size(); i++) {
            if (to_add < which_vector.at(i)) {
                which_vector.insert(which_vector.begin() + i, to_add);
                break;
            }
        }
    }
    //update frequency if adding to testing array
    if (which_vector == testing_array) {
        sample_frequencies[to_add]--;
    }
}

/*
* name: remove_from_testing
* purpose: removes a sample numbers to the testing array and increases the corresponding frequency in sample_frequencies
*          to update availability
* arguments: the sample number to remove
* returns: none
* notes: none
*/
void Program::remove_from_testing(int to_remove) {
    //find index of element to erase
    int index = -1;

    for (int i = 0; i < testing_array.size(); i++) {
        if (testing_array.at(i) == to_remove) {
            index = i;
        }
    }
    if (index == -1) {
        cerr << to_remove << " is not in testing array" << endl;
    }
    testing_array.erase(testing_array.begin() + index);
    sample_frequencies[to_remove]++;
}
