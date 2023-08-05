#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <numeric>
#include <cmath>

#include "trees.h"
#include "CSV_File.h"
#include "DataRow.h"

#include "SimpleWebServer/server_http.hpp"


using namespace std::chrono;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
namespace fs = std::filesystem;

std::string ui_path = "../../build";
std::string data_path = "../../data";

std::unordered_map<std::string, std::string> mime_types = {
        {"css", "text/css"},
        {"html", "text/html"},
        {"js", "application/javascript"},
        {"svg", "image/svg+xml"},
        {"ico", "image/vnd.microsoft.icon"},
        {"json", "application/json"},
        {"txt", "text/plain"},
        {"png", "image/png"},
};

struct Coordinates
{
    double lat;
    double lon;
};


bool is_valid_file(const std::string& p)
{
    std::string concat = ui_path + p;

    return fs::exists(concat) and fs::is_regular_file(concat);
}


std::string get_content(const std::string& p)
{
    if (not is_valid_file(p))
    {
        std::cout << "file not found" << std::endl;
        return "HTTP/1.1 404 NOT FOUND\r\nContent-Length: 0\r\n\r\n";
    }

    std::string concat = ui_path + p;

    // Read the file into a string
    std::ifstream file_stream(concat);
    if (!file_stream)
        exit(1);

    std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
            std::istreambuf_iterator<char>());

    // Close the file
    file_stream.close();

    int last = 0;

    for (int i = p.size() - 1; i >= 0; --i)
    {
        if (p[i] == '.')
        {
            last = i;
            break;
        }
    }

    std::string file_extension = p.substr(last + 1);

    std::string file_mime_type = mime_types[file_extension];

    return "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(file_contents.length()) + "\r\nContent-Type: " + file_mime_type + "\r\n\r\n" + file_contents;
}


/// using long longs and doubles because we live in the L memory paradigm
struct WindmillData
{
    double lon;
    double lat;
    std::string state;
    std::string county;
    long long year;
    long long capacity;
    double hub_height;
    double rotor_diam;
    double swept_area;
    double total_height;
    double proj_cap;
    long long proj_num_turbines;

    WindmillData() = default;
    WindmillData(
        double lon,
        double lat,
        std::string  state,
        std::string  county,
        long long year,
        long long capacity,
        double hub_height,
        double rotor_diam,
        double swept_area,
        double total_height,
        double proj_cap,
        long long proj_num_turbines
    )
        : lon(lon),
          lat(lat),
          state(std::move(state)),
          county(std::move(county)),
          year(year),
          capacity(capacity),
          hub_height(hub_height),
          rotor_diam(rotor_diam),
          swept_area(swept_area),
          total_height(total_height),
          proj_cap(proj_cap),
          proj_num_turbines(proj_num_turbines)
    {}
};


std::size_t vector_min(const std::vector<size_t>& vec)
{
    return *std::min_element(vec.begin(), vec.end());
}


struct WindmillDataset
{
    std::vector<std::string> states;
    std::vector<std::string> counties;
    std::vector<long long> years;
    std::vector<long long> capacities;
    std::vector<double> hub_heights;
    std::vector<double> rotor_diameters;
    std::vector<double> swept_areas;
    std::vector<double> total_heights;
    std::vector<double> proj_capacities;
    std::vector<long long> proj_num_turbines;
    std::vector<double> lats;
    std::vector<double> lons;

    size_t num_rows;

    explicit WindmillDataset(CSV_File csv)
        : states(csv.column_as_string("Site.State"))
        , counties(csv.column_as_string("Site.County"))
        , years(csv.column_as_int("Year"))
        , capacities(csv.column_as_int("Turbine.Capacity"))
        , hub_heights(csv.column_as_double("Turbine.Hub_Height"))
        , rotor_diameters(csv.column_as_double("Turbine.Rotor_Diameter"))
        , swept_areas(csv.column_as_double("Turbine.Swept_Area"))
        , total_heights(csv.column_as_double("Turbine.Total_Height"))
        , proj_capacities(csv.column_as_double("Project.Capacity"))
        , proj_num_turbines(csv.column_as_int("Project.Number_Turbines"))
        , lats(csv.column_as_double("Site.Latitude"))
        , lons(csv.column_as_double("Site.Longitude"))
    {
        num_rows = vector_min({
            states.size(),
            counties.size(),
            years.size(),
            capacities.size(),
            hub_heights.size(),
            rotor_diameters.size(),
            swept_areas.size(),
            total_heights.size(),
            proj_capacities.size(),
            proj_num_turbines.size(),
            lats.size(),
            lons.size()
        });
    }

    struct Iterator
    {
        size_t ind = 0;
        WindmillDataset& dataset;
        explicit Iterator(WindmillDataset& dataset) : dataset(dataset) {}

        Iterator& operator++()
        {
            ++ind;
            return *this;
        }

        Iterator operator++(int a)
        {
            ind++;
            return *this;
        }

        Iterator& operator--()
        {
            --ind;
            return *this;
        }

        Iterator operator--(int a)
        {
            ind--;
            return *this;
        }

        bool operator == (const Iterator& rhs) const
        {
            return this->ind == rhs.ind;
        }

        bool operator != (const Iterator& rhs) const
        {
            return this->ind != rhs.ind;
        }

        WindmillData operator*()
        {
            return {
                dataset.lons[ind],
                dataset.lats[ind],
                dataset.states[ind],
                dataset.counties[ind],
                dataset.years[ind],
                dataset.capacities[ind],
                dataset.hub_heights[ind],
                dataset.rotor_diameters[ind],
                dataset.swept_areas[ind],
                dataset.total_heights[ind],
                dataset.proj_capacities[ind],
                dataset.proj_num_turbines[ind]
            };
        }
    };

    Iterator begin()
    {
        return Iterator(*this);
    }

    Iterator end()
    {
        Iterator it(*this);
        it.ind = this->num_rows;
        return it;
    }
};


double rectangular_area(double lat1, double lat2, double lon1, double lon2)
{
    const double earth_radius_miles = 3963.1906f;

    // A = R^2 * (sin(lat1) - sin(lat2)) * (lon1 - lon2)
    return earth_radius_miles * earth_radius_miles * (sin(std::max(lat1, lat2) * M_PI / 180.) - sin(std::min(lat1, lat2)* M_PI / 180.)) * std::abs(lon1 - lon2) * M_PI / 180.;
}


int main()
{
    std::ifstream dataset(data_path + "/wind_turbines.csv");

    if (not dataset.is_open())
    {
        std::cerr << "Could not open wind_turbines.csv" << std::endl;
        return 1;
    }

    CSV_File csv(std::move(dataset));
    WindmillDataset w_dataset(csv);

    std::vector<std::pair<std::array<double, 2>, WindmillData>> datapoints;
    datapoints.reserve(w_dataset.num_rows);

    for (auto it = w_dataset.begin(); it != w_dataset.end(); ++it)
    {
        auto d = *it;
        datapoints.emplace_back(std::array<double, 2>({{d.lat, d.lon}}), d);
    }

    std::cout << "Starting tree construction" << std::endl;

    RangeTree<double, WindmillData, 2> rtree(datapoints.begin(), datapoints.end());
    KD_Tree<double, WindmillData, 2> kdtree(datapoints.begin(), datapoints.end());

    std::cout << "made it past tree construction" << std::endl;

    HttpServer server;
    server.config.port = 8080;

    server.resource["^/static/css/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/js/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/media/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

    server.resource["^/data/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

    server.resource["^/query"]["POST"] = [&rtree, &kdtree] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string content = request->content.string();

        /*
         * the query looks like this:
         * <lat1>,<lat2>,<lon1>,<lon2>
         */
        //{"coordinates":"-113.01951464913721,39.456595437139875,-103.35001894684127,44.60650516716939"}

        size_t startPos = content.find(":\"") + 2;
        size_t endPos = content.rfind('\"');
        std::string parsed_content = content.substr(startPos, endPos - startPos);

        DataRow coords(parsed_content);

        double lon1 = coords[0], lon2 = coords[2], lat1 = coords[1], lat2 = coords[3];

        std::cout << content << std::endl;

        std::cout << "coords: " << lat1 << " " << lat2 << " " << lon1 << " " << lon2 << std::endl;

//        *response <<  "HTTP/1.1 404 NOT FOUND\r\nContent-Length: 0\r\n\r\n";
//        return;


        auto start = high_resolution_clock::now();

        std::vector<WindmillData> rtree_results = rtree.range_query({{{lat1, lat2}, {lon1, lon2}}});

        auto stop = high_resolution_clock::now();

        long long rangetree_took = duration_cast<milliseconds>(stop - start).count();


        start = high_resolution_clock::now();

        std::vector<WindmillData> kdtree_results = kdtree.range_query({{{lat1, lat2}, {lon1, lon2}}});

        stop = high_resolution_clock::now();

        long long kdtree_took = duration_cast<milliseconds>(stop - start).count();

        /*
         * List of statistics to display:
         * min year, max year
         * average capacity
         * tallest turbine, shortest turbine,
         * average rotor diameter
         * number of turbines
         * turbine density
         * area selected
         */

        /*
         * Format output data like this:
         * {"Earliest built": <num>,
         *  "Latest built": <num>,
         *  "Average capacity": <num>,
         *  "Tallest turbine": <num>,
         *  "Shortest turbine":
         *  "Average rotor diameter",
         *  "Turbine density",
         *  "Area of selection"}
         */

        long long earliest_built = 0, latest_built = 0;
        double average_capacity = 0, tallest_turbine = 0, shortest_turbine = 0, average_rotor_diameter = 0, turbine_density = 0, selection_area = 0;

        if (!rtree_results.empty())
        {
            earliest_built = std::min_element(
                    rtree_results.begin(),
                    rtree_results.end(),
                    [](const WindmillData &a, const WindmillData &b) { return a.year < b.year; }
            )->year;

            latest_built = std::max_element(
                    rtree_results.begin(),
                    rtree_results.end(),
                    [](const WindmillData &a, const WindmillData &b) { return a.year < b.year; }
            )->year;

            average_capacity = std::accumulate(
                    rtree_results.begin(),
                    rtree_results.end(),
                    0ll,
                    [](long long a, const WindmillData &d) { return a + d.capacity; }) / (double) rtree_results.size();

            tallest_turbine = std::max_element(
                    rtree_results.begin(),
                    rtree_results.end(),
                    [](const WindmillData &a, const WindmillData &b) { return a.total_height < b.total_height; }
            )->total_height;

            shortest_turbine = std::min_element(
                    rtree_results.begin(),
                    rtree_results.end(),
                    [](const WindmillData &a, const WindmillData &b) { return a.total_height < b.total_height; }
            )->total_height;

            average_rotor_diameter = std::accumulate(
                    rtree_results.begin(),
                    rtree_results.end(),
                    0ll,
                    [](long long a, const WindmillData &d) { return a + d.rotor_diam; }) /
                                     (double) rtree_results.size();

            selection_area = rectangular_area(lat1, lat2, lon1, lon2);

            turbine_density = rtree_results.size() / selection_area;
        }

        std::stringstream ss;
        ss << "{\"Earliest built\":" << earliest_built << ',';
        ss << "\"Latest built\":" << latest_built << ',';
        ss << "\"Average capacity\":" << average_capacity << ',';
        ss << "\"Tallest turbine\":" << tallest_turbine << ',';
        ss << "\"Shortest turbine\":" << shortest_turbine << ',';
        ss << "\"Average rotor diameter\":" << average_rotor_diameter << ',';
        ss << "\"Turbine density\":" << turbine_density << ',';
        ss << "\"Number of Turbines\":" << rtree_results.size() << ',';
        ss << "\"Range tree query took\":" << rangetree_took << ',';
        ss << "\"K-d tree query took\":" << kdtree_took << ',';
        ss << "\"Area of selection\":" << selection_area << '}';

        const std::string resp = ss.str();
        std::cout << resp << std::endl;

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(resp.length()) + "\r\nContent-Type: text\r\n\r\n" + resp;
    };

    std::thread server_thread([&server]() {
        server.start();
    });

    server_thread.join();

    return 0;
}
