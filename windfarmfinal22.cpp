//NAME-AHMAD MUJTABA
//ID-2020A7PS0962G
//EMAIL ID-f20200962@goa.bits-pilani.ac.in
// Necessary header files
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <bits/stdc++.h>


using namespace std;


// Structure to store details of each wind farm
struct WindFarm {
    std::string id;       // ID of the wind farm, e.g., "E1"
    double forecast;      // Current power output forecast for the wind farm
    double capacity;      // Maximum power output capacity of the wind farm
    double weight;        // Weight for re-dispatching, calculated as forecast/capacity
};


// Function to extract the numerical portion from wind farm ID
int extractNumber(const std::string& s) {
    return std::stoi(s.substr(1)); // Assumes ID format: a single letter followed by numbers
}


// Custom comparator for the map to ensure wind farms are sorted in natural number order
struct CustomCompare {
    bool operator()(const std::string& a, const std::string& b) const {
        // First compare by prefix letter (e.g., "E" in "E1")
        if(a[0] != b[0]) return a[0] < b[0];


        // Then compare by number (ensuring "E2" comes before "E10")
        return extractNumber(a) < extractNumber(b);
    }
};


int main() {
    // Path to the input CSV file
    std::string filePath = "C:/Users/Asus/Downloads/biourja-efzrr-y7i38ed9-input.csv";  //filepath
    std::ifstream file(filePath);


    // If the file couldn't be opened, print an error and exit
    if (!file.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return 1;
    }


    // Map to store wind farm details with custom ordering
    std::map<std::string, WindFarm, CustomCompare> windFarms;
    std::string line, word;


    // Skip the header line in the CSV
    std::getline(file, line);


    // Parse the CSV line by line
    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::vector<std::string> data;


        // Split the line at each comma
        while (std::getline(s, word, ',')) {
            data.push_back(word);
        }


        // Extract wind farm details from the parsed line
        std::string id = data[0];
        double forecast = std::stod(data[1]);
        double capacity = std::stod(data[2]);


        // Store the wind farm details in the map
        windFarms[id] = {id, forecast, capacity, 0.0};
    }


    // Close the CSV file after reading
    file.close();


    // Provided zonal forecasts
    std::map<std::string, double> zonalForecast = {
        {"East", 2800},
        {"North", 1500},
        {"West", 2000},
        {"South", 6500}
    };


    // State-level forecast
    double totalStateForecast = 12000;


    // Calculate weight for each wind farm
    for (auto &wf : windFarms) {
        wf.second.weight = wf.second.forecast / wf.second.capacity;
    }


    // Re-Dispatch at Zone Level
    for (const auto &zone : zonalForecast) {
        double totalZoneCapacity = 0;
        double totalZoneForecast = 0;
        
        // Calculate the total current forecast and capacity for each zone
        for (const auto &wf : windFarms) {
            if (wf.second.id.substr(0, 1) == zone.first.substr(0, 1)) {
                totalZoneCapacity += wf.second.capacity;
                totalZoneForecast += wf.second.forecast;
            }
        }
        
        // Adjust each wind farm's forecast based on the zone's forecast
        for (auto &wf : windFarms) {
            if (wf.second.id.substr(0, 1) == zone.first.substr(0, 1)) {
                wf.second.forecast = wf.second.weight * zone.second;
                // Ensure the forecast doesn't exceed the wind farm's capacity
                if (wf.second.forecast > wf.second.capacity) {
                    wf.second.forecast = wf.second.capacity;
                }
            }
        }
    }


    // State-Level Adjustment
    double totalNewForecast = 0;
    for (const auto &wf : windFarms) {
        totalNewForecast += wf.second.forecast;
    }
    
    // Calculate scaling factor based on state-level forecast
    double adjustmentFactor = totalStateForecast / totalNewForecast;
    
    // Adjust each wind farm's forecast using the scaling factor
    for (auto &wf : windFarms) {
        wf.second.forecast *= adjustmentFactor;
        // Ensure the forecast doesn't exceed the wind farm's capacity
        if (wf.second.forecast > wf.second.capacity) {
            wf.second.forecast = wf.second.capacity;
        }
    }


    // Calculate any remaining discrepancy between the new total forecast and state-level forecast
    totalNewForecast = 0;
    for (const auto &wf : windFarms) {
        totalNewForecast += wf.second.forecast;
    }


    double discrepancy = totalStateForecast - totalNewForecast;
    double lastAdjustment = 0;


    // Adjust the forecasts iteratively to minimize the discrepancy
    while (abs(discrepancy) > 0.001 && abs(discrepancy - lastAdjustment) > 0.001) {
        lastAdjustment = discrepancy;


        for (auto &wf : windFarms) {
            double adjustment = wf.second.weight * discrepancy;
            if (wf.second.forecast + adjustment > wf.second.capacity) {
                adjustment = wf.second.capacity - wf.second.forecast;
            } else if (wf.second.forecast + adjustment < 0) {
                adjustment = -wf.second.forecast;
            }
            wf.second.forecast += adjustment;
            discrepancy -= adjustment;
        }
    }


    // Print the final adjusted forecasts
    double finalTotal = 0;
    std::cout << "Plant_Name,Forecast\n";
    for (const auto &wf : windFarms) {
        std::cout << wf.second.id << "," << wf.second.forecast << "\n";
        finalTotal += wf.second.forecast;
    }
    std::cout << "Final Total: " << finalTotal << "MW" << std::endl;


    return 0;
}

