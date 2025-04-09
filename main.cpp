#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "object.hpp"
#include "imu_dat.hpp"


nlohmann::json readJsonFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    try {
        // Read the file in chunks to handle large files
        std::stringstream buffer;
        buffer << f.rdbuf();
        return nlohmann::json::parse(buffer.str());
    }
    catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + std::string(e.what()));
    }
}

std::vector<ImuObj> readCSV(const std::string& filename)
{
    std::ifstream file(filename);
    std::vector<ImuObj> data;

    std::string line;

    if(!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

            bool firstLine = true;

    
    while(std::getline(file, line))
    {

                    if (firstLine) {
            firstLine = false;
            continue; 
            }
        std::vector<std::string> row;
        std::stringstream ss(line);

        std::string cell;

        ImuObj obj;
        int columnIndex = 0;

        while(std::getline(ss, cell, ','))
        {
             try {
                switch(columnIndex) {
                    case 0: // timestamp
                        obj.timestamp = cell;
                        break;
                    case 1: // id
                        obj.id = std::stoi(cell);
                        break;
                    case 2: // yaw
                        obj.yaw = std::stoi(cell);
                        break;
                    case 3: // heading
                        obj.heading = std::stoi(cell);
                        break;
                    case 4: // state
                        obj.st = stringToState(cell);
                        break;
                    case 5: // acceleration
                        obj.acceleration = std::stod(cell);
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing column " << columnIndex << ": " << e.what() << std::endl;
            }
            columnIndex++;
        
        }
        if (columnIndex > 0) { 
            data.push_back(obj);
        }
    }
    file.close();
    return data;
}


std::string cleanJsonContent(const std::string& content) {
    std::string cleaned = content;
    
    // Remove any trailing commas before closing brackets or braces
    size_t pos = 0;
    while ((pos = cleaned.find("}},") != std::string::npos)) {
        cleaned.replace(pos + 2, 1, ""); // Remove the comma after }}
    }
    pos = 0;
    while ((pos = cleaned.find(",]", pos)) != std::string::npos) {
        cleaned.replace(pos, 2, "]");
    }
    pos = 0;
    while ((pos = cleaned.find(",}", pos)) != std::string::npos) {
        cleaned.replace(pos, 2, "}");
    }

    return cleaned;
}

bool parseJsonFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    try {
        // Read file content in chunks
        std::stringstream buffer;
        char chunk[4096];
        while (f.read(chunk, sizeof(chunk))) {
            buffer.write(chunk, f.gcount());
        }
        if (f.gcount() > 0) {
            buffer.write(chunk, f.gcount());
        }

        std::string content = buffer.str();
        
        // Clean the JSON content
        content = cleanJsonContent(content);

        // Try to parse the cleaned content
        auto json = nlohmann::json::parse(content);
        
        // If we got here, parsing succeeded
        std::cout << "JSON validation successful!" << std::endl;
        
        // Print structure information
        if (json.is_array()) {
            std::cout << "JSON is an array with " << json.size() << " elements." << std::endl;
            if (!json.empty()) {
                std::cout << "First element structure:" << std::endl;
                std::cout << json[0].dump(2) << std::endl;
            }
        } else if (json.is_object()) {
            std::cout << "JSON is an object with keys:" << std::endl;
            for (const auto& [key, value] : json.items()) {
                std::cout << "- " << key << std::endl;
            }
        }

        return true;
    } 
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n"
                  << "Error at byte position: " << e.byte << std::endl;
        
        // Show context around the error
        f.clear();
        f.seekg(std::max<std::streampos>(0, e.byte - 100));
        std::string context;
        std::getline(f, context);
        std::cerr << "Context around error:\n" << context << std::endl;
        
        // Additional error analysis
        if (context.find("}},") != std::string::npos) {
            std::cerr << "Hint: Found '}},' pattern. This might indicate nested object closure with an invalid trailing comma." << std::endl;
        }
        
        return false;
    }
}

int main()
{

    cObject cobj;
    ImuObj iObj;

   std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
   std::ifstream f("../task_cam_data.json");

   auto imuData = readCSV("/Users/svin/Desktop/Sentics/task_imu.csv");
   if (imuData.empty()) {
        std::cerr << "No IMU data was read from the file" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < std::min(imuData.size(), size_t(5)); ++i) {
        printImuObject(imuData[i]);
    }

    std::string jsonFilePath = "/Users/svin/Desktop/Sentics/task_cam_data.json";
    
    try {
        std::cout << "Attempting to parse JSON file: " << jsonFilePath << std::endl;
        
        if (!parseJsonFile(jsonFilePath)) {
            std::cerr << "Failed to parse JSON file" << std::endl;
            
            // Additional debugging
            std::ifstream f(jsonFilePath, std::ios::binary);
            if (f.is_open()) {
                // Read first 500 characters
                char buffer[501];
                f.read(buffer, 500);
                buffer[500] = '\0';
                
                std::cout << "\nFirst 500 characters of file:\n" << buffer << std::endl;
                
                // Check for common JSON formatting issues
                std::string content(buffer);
                size_t pos = 0;
                while ((pos = content.find("}},", pos)) != std::string::npos) {
                    std::cout << "Found problematic '}},' at position " << pos << std::endl;
                    pos++;
                }
            } 
            return -1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }



    return 0;
}